#include "ahci.h"

#include <klib/stdint.h>
#include <klib/string.h>
#include <memory/allocators.h>

#include <arch_factory.h>

namespace xenon
{
    enum class IPM : uint8_t
    {
        ACTIVE_STATE = 1,
    };

    enum class DET : uint8_t
    {
        GENERATION_3_RATE = 3,
    };

    enum class SIG : uint64_t
    {
        SATA            = 0x101,
        SATAPI          = 0xEB140101,
        ENCLOSURE       = 0xC33C0101,
        PORT_MULTIPLIER = 0x96690101,
        ERROR           = 0,
    };

    enum PORT
    {
        CMD_LIST = (1 << 15),
    };

    SIG check_type(ahci::hba_port *port)
    {
        // get data about device dection and power management
        auto sata_status = port->sata_status;
        auto power_mng = static_cast<IPM>((sata_status >> 8) & 0xf);
        auto detection = static_cast<DET>(sata_status & 0xf);
        auto signature = static_cast<SIG>(port->signature);

        // check if device active and generation 3 communication rate was
        // negotiated.
        if (detection != DET::GENERATION_3_RATE || power_mng != IPM::ACTIVE_STATE) {
            return SIG::ERROR;
        }

        return signature;
    }

    void ahci::probe(hba_memory *abar)
    {
        auto ports = abar->ports_implemented;
        for (int i = 0; i < 32; i++, ports >>= 1) {
            // port not implemented: skip it
            if ((ports & 1) == 0) {
                continue;
            }

            switch (check_type(&abar->ports[i])) {
                case SIG::SATA:
                    break;

                case SIG::SATAPI:
                    break;

                case SIG::PORT_MULTIPLIER:
                    break;

                case SIG::ENCLOSURE:
                    break;

                default:    // no drive
                    break;
            }
        }
    }

    ahci::ahci_controller::ahci_controller(hba_memory *abar) :
        abar_(abar)
    {
        initialize();
    }

    void ahci::ahci_controller::initialize()
    {
        // AHCI 1.3.1 specification - page 104
        // 1. indicate that system sw is AHCI by global_host_control.AE = 1
        //                                        AE - AHCI Enable - bit 31
        abar_->global_host_control |= (1 << 31);

        // 2. determine which ports are implemented by HBA, by reading the
        //    ports_implemented register (how many ports available and which
        //    registers need to be initialized.
        auto ports = abar_->ports_implemented;
        for (int i = 0; i < 32; i++, ports >>= 1) {
            // port not implemented: skip it
            if ((ports & 0x1) == 0) {
                continue;
            }

            // 3. ensure the controller is not in the running state by reading
            //    each implemented port command_status register.
            //    a. hba_port.command_status.Start
            //       AND hba_port.command_status.FIS Receive Enable
            //       AND hba_port.command_status.FIS Receive Running
            //       AND hba_port.command_status.Command List Running == 0
            //       THEN port is in IDLE state
            if ((abar_->ports[i].command_status & PORT::CMD_LIST) == 0 && // command list running
                (abar_->ports[i].command_status & (1 << 14)) == 0 && // fis receive running
                (abar_->ports[i].command_status & (1 <<  4)) == 0) { // fis receive enable
                // port is in IDLE state, let's see the next one
                continue;
            }

            //    b. otherwise port is not IDLE and should be place in the idle
            //       state before manipulating HBA and port specific registers.
            //    c. hba_port.command_status.Start = 0 and wait for 
            //       hba_port.command_status.Command List Running to return 0 when read.
            //       Wait 500 miliseconds (at least) for it to occur.
            abar_->ports[i].command_status &= ~1;
            wait_until(abar_->ports[i].command_status, 1 << 15, false, 500);

            //    d. if hba_port.command_status.FIS Receive Enable == 1, SW should
            //       set it to 0 and wait at least 500 miliseconds for
            //       hba_port.command_status.FIS Receive Running to return 0 when read.
            if ((abar_->ports[i].command_status & (1 << 4)) == 1) {
                abar_->ports[i].command_status &= ~(1 << 4);
                wait_until(abar_->ports[i].command_status, 1 << 15, false, 500);
            }

            //    e. if hba_port.command_status.Command List Running OR
            //       hba_port.command_status.FIS Receive Running do not clear to 0 correct,
            //       then we may need to do a port reset or a full HBA reset to recover.
            if ((abar_->ports[i].command_status & (1 << 15)) != 0 ||
                (abar_->ports[i].command_status & (1 << 14)) != 0) {
                reset();
            }

            // 5. for each implemented port, allocate memory for and program:
            //    a. hba_port.comm_list_base_addr_[lo|hi]
            paddr_t physical_command_list;
            auto place = placement_kalloc(sizeof(command_list_entry) * COMMAND_LIST_ENTRY_COUNT,
                                          &physical_command_list);

            memset(place, 0, sizeof(command_list_entry) * COMMAND_LIST_ENTRY_COUNT);
            abar_->ports[i].comm_list_base_addr_lo = ptr_from(physical_command_list) & 0xffffffff;
            abar_->ports[i].comm_list_base_addr_hi = ptr_from(physical_command_list) >> 32;

            //    b. hba_port.fis_base_addr_[lo|hi]
            //    c. zero out the memory allocated
            //    d. after set hba_port.fis_base_addr to the physical address of the FIS
            //       receive area set hba_port.command_status.FIS Receive Enable = 1
            // 6. for each implemented port, clear hba_port.sata_error by writing 1 to each
            //    implemented bit location.

        }

        // 4. determine how many command slots HBA supports, capabilities.Number Command Slots
        uint32_t n_command_slots = (abar_->capabilities >> 8) & 0x1f;
    }

    void ahci::ahci_controller::reset()
    {
    }

    void ahci::ahci_controller::start()
    {

    }

    bool ahci::ahci_controller::wait_until(uint32_t reg, uint32_t port, bool cond, uint32_t time)
    {
        auto expect = (cond) ? port : 0;

        arch_factory::instance().call()->get_timer()->wait_for(time);

        // after sleeping for _time_ miliseconds, check if the bit has the value
        // expected. If so, return true.
        if ((reg & port) == expect) {
            return true;
        }

        // return false to inform that condition was not satisfied.
        return false;
    }

    // Initialization
    // 7. check events should cause an interrupt and set each implemented port's
    //    hba_port.interrupt_enable register with appropriate enables. To enable HBA
    //    to generate interrupts, must also set
    //    hba_memory.global_host_control.interrupt_enable = 1.
    // 8. ORDER:
    //    a. hba_port.interrupt_status=0
    //    b. interrupt_status_register.interrupt_status_pending=0
    //    before programming
    //    c. hba_port.interrupt_enable and
    //    d. hba_memory.global_host_control.interrupt_enable
    //    These will prevent residual bits set in these registers from causing an
    //    interrupt to be asserted,
}
