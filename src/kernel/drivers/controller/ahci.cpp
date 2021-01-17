#include "ahci.h"

#include <klib/string.h>
#include <klib/logger.h>
#include <klib/utility.h>
#include <memory/allocators.h>

#include <arch_factory.h>


/******************************************************************************
 * Helper functions
 *****************************************************************************/
ahci::SIG check_type(ahci::hba_port *port)
{
    // get data about device dection and power management
    auto sata_status = port->sata_status;
    auto power_mng = static_cast<ahci::IPM>((sata_status >> 8) & 0xf);
    auto detection = static_cast<ahci::DET>(sata_status & 0xf);
    auto signature = static_cast<ahci::SIG>(port->signature);

    // check if device active and generation 3 communication rate was
    // negotiated.
    if (detection != ahci::DET::GENERATION_3_RATE || power_mng != ahci::IPM::ACTIVE_STATE) {
        return ahci::SIG::ERROR;
    }

    return signature;
}

int find_cmdslot(ahci::hba_port *port, uint32_t total_slots)
{
    uint32_t slots = (port->sata_active | port->command_issue);
    for (uint32_t i = 0; i < total_slots; i++, slots >>= 1) {
        if ((slots & 1) == 0) {
            return i;
        }
    }

    return -1;
}

void convert_ahci_string(char *str, int len)
{
    int i;
    for (i = 0; i < len; i += 2) {
        if (str[i] == '\0' || str[i + 1] == '\0') {
            break;
        }

        klib::swap(str[i], str[i + 1]);
    }
    str[i - 1] = '\0';
}

void LBA48_command(ahci::hba_to_device *to_dev, uint8_t command, uint64_t lba, uint8_t sector_count)
{
    // LBA48: https://en.wikipedia.org/wiki/Logical_block_addressing#LBA48
    //    Named 48 because the addressing limit is 2^48 * 512 (128PiB).
    to_dev->command = command;
    to_dev->device = 0x40; // LBA mode
    to_dev->lba_sector = static_cast<uint8_t>(lba);
    to_dev->lba_cylinder_lo = static_cast<uint8_t>((lba >> 8) & 0xff);
    to_dev->lba_cylinder_hi = static_cast<uint8_t>((lba >> 16) & 0xff);
    to_dev->lba_sector_exp = static_cast<uint8_t>((lba >> 24) & 0xff);
    to_dev->lba_cylinder_lo_exp = static_cast<uint8_t>((lba >> 32) & 0xff);
    to_dev->lba_cylinder_hi_exp = static_cast<uint8_t>((lba >> 40) & 0xff);
    to_dev->count_lo = sector_count & 0xff;
    to_dev->count_hi = (sector_count >> 8) & 0xff;

    to_dev->fis_type = static_cast<uint8_t>(ahci::FIS_TYPES::FIS_TYPE_REG_H2D);
    to_dev->cmd_control = 1;
}

klib::unique_ptr<block_device> ahci::detect(pci_info_t info)
{
    // not an ahci controller, return
    if (info.klass != 0x1 || info.subclass != 0x6) {
        return nullptr;
    }

    // PCI BAR[5] points to ABAR base address, maps the address in the virtual address space and
    // bind that location to a hba_memory structure
    auto abar = reinterpret_cast<ahci::hba_memory*>(manager::instance().mapio(info.bars[5], 0, 0));
    return klib::unique_ptr<block_device>(new ahci::ahci_controller(abar));
}


/******************************************************************************
 * Class implementation
 *****************************************************************************/
ahci::ahci_controller::ahci_controller(hba_memory *abar) :
    block_device(0),
    abar_(abar)
{
    initialize();
    sata_identify();
}

klib::unique_ptr<char[]> ahci::ahci_controller::read(size_t lba, size_t bytes)
{
    // Information: https://en.wikipedia.org/wiki/Logical_block_addressing
    //
    // Intro:
    //   We're using LBA addressing here, which is a linear way to access disk data where each
    //   number describes a single block. It's also possible to use CHS geometry but I think
    //   it's obsolete.
    //
    // Conversion formulas (curiosity only):
    //   LBA      = (Cylinders x Heads/Cylinder + Heads) * Sectors/Track  + (Sectors - 1)
    //   Cylinder = LBA / (Heads/Cylinder * Sectors/Track)
    //   Head     = (LBA / Sectors/Track) % Heads/Cylinder
    //   Sector   = (LBA % Sectors/Track) + 1

    // sanity check, don't read an not initialized device
    if (sector_size_ == 0) {
        klib::logger::instance().log("sector size == 0, device is not initialized");
        return nullptr;
    }

    // create a buffer large enough to store the result
    auto buffer = klib::make_unique<char[]>(bytes);

    // read all required blocks
    sata_read(lba, reinterpret_cast<uintptr_t>(buffer.get()) - KVIRTUAL_ADDRESS, bytes);

    // move the buffer to a string container and return it
    return klib::move(buffer);
}

void ahci::ahci_controller::initialize()
{
    // AHCI 1.3.1 specification - page 104
    //  - Section 10.1.2 - System software specific initialization
    // 1. indicate that system sw is AHCI by global_host_control.AE = 1
    //                                        AE - AHCI Enable - bit 31
    abar_->global_host_control |= GHC::AE;

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
        if ((abar_->ports[i].command_status & PORT::CMD_CR) == 0 && // command list running
            (abar_->ports[i].command_status & PORT::CMD_FR) == 0 && // fis receive running
            (abar_->ports[i].command_status & PORT::CMD_FRE) == 0) { // fis receive enable
            // port is in IDLE state, let's see the next one
            continue;
        }

        //    b. otherwise port is not IDLE and should be place in the idle
        //       state before manipulating HBA and port specific registers.
        //    c. hba_port.command_status.Start = 0 and wait for 
        //       hba_port.command_status.Command List Running to return 0 when read.
        //       Wait 500 miliseconds (at least) for it to occur.
        abar_->ports[i].command_status &= ~PORT::CMD_ST;
        wait_until(abar_->ports[i].command_status, PORT::CMD_CR, false, 500);

        //    d. if hba_port.command_status.FIS Receive Enable == 1, SW should
        //       set it to 0 and wait at least 500 miliseconds for
        //       hba_port.command_status.FIS Receive Running to return 0 when read.
        if ((abar_->ports[i].command_status & PORT::CMD_FRE) != 0) {
            abar_->ports[i].command_status &= ~PORT::CMD_FRE;
            wait_until(abar_->ports[i].command_status, PORT::CMD_CR, false, 500);
        }

        //    e. if hba_port.command_status.Command List Running OR
        //       hba_port.command_status.FIS Receive Running do not clear to 0 correct,
        //       then we may need to do a port reset or a full HBA reset to recover.
        if ((abar_->ports[i].command_status & PORT::CMD_CR) != 0 ||
            (abar_->ports[i].command_status & PORT::CMD_FR) != 0) {
            reset(&abar_->ports[i]);
        }

        // 5. for each implemented port, allocate memory for and program:
        //    a. hba_port.comm_list_base_addr_[lo|hi]
        //    a.1. zero out the memory allocated
        //    we're going to allocate COMMAND_LIST_ENTRY_COUNT=32 command lists
        paddr_t physical_command_list;
        paddr_t physical_command_table;
        auto place = placement_kalloc(sizeof(command_list) * COMMAND_LIST_ENTRY_COUNT,
                                      &physical_command_list, true);
        if (place == nullptr) {
            // TODO: handle ahci error, out of memory
        }

        klib::memset(place, 0, sizeof(command_list) * COMMAND_LIST_ENTRY_COUNT);
        abar_->ports[i].comm_list_base_addr_lo = ptr_from(physical_command_list) & 0xffff'ffff;
        abar_->ports[i].comm_list_base_addr_hi = ptr_from(physical_command_list) >> 32;

        // now allocate memory for command table and descriptor tables (PRD_TABLE_ENTRY_COUNT=128)
        auto cmd_list = reinterpret_cast<command_list*>(place);
        place = placement_kalloc(sizeof(command_table) + sizeof(descriptor_table) * PRD_TABLE_ENTRY_COUNT,
                                 &physical_command_table, true);
        cmd_list->command_table_base_addr_lo = ptr_from(physical_command_table) & 0xffff'ffff;
        cmd_list->command_table_base_addr_hi = ptr_from(physical_command_table) >> 32;

        //    b. hba_port.fis_base_addr_[lo|hi]
        //    b.1. zero out the memory allocated
        paddr_t physical_fis;
        place = placement_kalloc(sizeof(fis), &physical_fis);
        if (place == nullptr) {
            // TODO: handle ahci error
        }

        klib::memset(place, 0, sizeof(fis));
        abar_->ports[i].fis_base_addr_lo = ptr_from(physical_fis) & 0xffff'ffff;
        abar_->ports[i].fis_base_addr_hi = ptr_from(physical_fis) >> 32;

        //    d. after set hba_port.fis_base_addr to the physical address of the FIS
        //       receive area set hba_port.command_status.FIS Receive Enable = 1
        abar_->ports[i].command_status |= PORT::CMD_FRE;
        start(&abar_->ports[i]);

        // 6. for each implemented port, clear hba_port.sata_error by writing 1 to each
        //    implemented bit location.
        abar_->ports[i].sata_error &= 0x783;
    }

    // 7. check events should cause an interrupt and set each implemented port's
    //    hba_port.interrupt_enable register with appropriate enables. To enable HBA
    //    to generate interrupts, must also set
    //    hba_memory.global_host_control.interrupt_enable = 1.
    abar_->global_host_control |= (1 << 1);

    // 4. determine how many command slots HBA supports, capabilities.Number Command Slots
    total_cmd_slots_ = (abar_->capabilities >> 8) & 0x1f;
}

auto ahci::ahci_controller::sata_get_port()
{
    hba_port *port = nullptr;
    auto ports = abar_->ports_implemented;
    for (int i = 0; i < 32; i++, ports >>= 1) {
        // port not implemented: skip it
        if ((ports & 0x1) == 0) {
            continue;
        }

        // only handle SATA devices
        if (check_type(&abar_->ports[i]) != SIG::SATA) {
            continue;
        }

        port = &abar_->ports[i];
        break;
    }

    return port;
}

void ahci::ahci_controller::reset(hba_port *port)
{
    /*
     * Section 10.4.2 - Port reset
     * Software causes  a  port reset(COMRESET)by writing 1h to the PxSCTL.DET field to invoke
     * a COMRESET on the interface and start a re-establishment of Phy layer communications. Software
     * shall wait at least 1 millisecond before clearing PxSCTL.DET to 0h; this ensures that at least
     * one COMRESET signal is sent over the interface. After clearing PxSCTL.DET to 0h, software should
     * wait for communication to be re-established as indicated byPxSSTS.DET being set to 3h. Then
     * software should write all 1s to the PxSERR register to clear any bits that were set as part of
     * the port reset.When PxSCTL.DET is set to 1h, the HBA shall reset PxTFD.STS to 7Fh and shall
     * reset PxSSTS.DET to 0h. When PxSCTL.DET is set to 0h, upon receiving a COMINIT from the attached
     * device, PxTFD.STS.BSYshall be set to ’1’ by the HBA.
     */ 
    // clear start flag
    port->command_status &= ~PORT::CMD_ST;

    // wait until command list running flag is cleared
    wait_until(port->command_status, PORT::CMD_CR, false, 500);

    port->sata_control = 0x1;
    arch_factory::instance()->get_timer()->wait_for(100);
    port->sata_control = 0x0;

    while ((port->sata_status & 0x7) != 0x3) ;

    port->sata_error = 0xffff'ffff;
}

void ahci::ahci_controller::start(hba_port *port)
{
    // wait until command list running flag is cleared
    while (port->command_status & PORT::CMD_CR);

    // set FIS Receive Enable (FRE)
    port->command_status |= PORT::CMD_FRE;
    // start processing the command list
    port->command_status |= PORT::CMD_ST;
}

void ahci::ahci_controller::stop(hba_port *port)
{
    // clear start flag
    port->command_status &= ~PORT::CMD_ST;
    // clear FIS receive enable bit
    port->command_status &= ~PORT::CMD_FRE;

    // wait untils bit 'command list running' and 'fis receive running'
    // are cleared
    while ((port->command_status & PORT::CMD_FR) || port->command_status & PORT::CMD_CR) ;
}

void ahci::ahci_controller::sata_identify()
{
    auto port = sata_get_port();
    if (port == nullptr) {
        klib::logger::instance().log("Error: can't find a SATA port available");
        return;
    }

    auto physical_sata = kvirt_to_physical(ptr_from(&identifier_));

    auto slot = find_cmdslot(port, total_cmd_slots_);
    if (slot == -1) {
        klib::logger::instance().log("Error: can't find a SATA port available");
        return;
    }

    auto cmd_list = reinterpret_cast<command_list*>((
                static_cast<uintptr_t>(port->comm_list_base_addr_hi) << 32 |
                port->comm_list_base_addr_lo) + KVIRTUAL_ADDRESS);
    cmd_list += slot;
    cmd_list->command_fis_length = sizeof(hba_to_device) / sizeof(int32_t);
    cmd_list->write = 0;
    cmd_list->prefetchable = 1;
    cmd_list->prdt_length = 1;

    auto cmd_table = reinterpret_cast<command_table*>((
                static_cast<uintptr_t>(cmd_list->command_table_base_addr_hi) << 32 |
                cmd_list->command_table_base_addr_lo) + KVIRTUAL_ADDRESS);
    cmd_table->descriptor_entry[0].data_base_address_lo = physical_sata & 0xfff'fffff;
    cmd_table->descriptor_entry[0].data_base_address_hi = physical_sata >> 32;
    cmd_table->descriptor_entry[0].data_byte_count = 511;
    cmd_table->descriptor_entry[0].interrupt_on_completition = 0;

    hba_to_device *to_dev = reinterpret_cast<hba_to_device*>(&cmd_table->command_cfis);
    to_dev->fis_type = static_cast<uint8_t>(ahci::FIS_TYPES::FIS_TYPE_REG_H2D);
    to_dev->cmd_control = 1;
    to_dev->command = 0xec;
    to_dev->device = 0;

    // issue the command
    port->command_issue = 1 << slot;

    while (true) {
        if ((port->command_issue & (1 << slot)) == 0) {
            break;
        }

        if (port->interrupt_status & (1 << 30)) {
            return;
        }
    }

    if (port->interrupt_status & (1 << 30)) {
        return;
    }

    // fix those byte inverted AHCI strings
    convert_ahci_string(identifier_.model, sizeof(identifier_.model));
    convert_ahci_string(identifier_.firmware, sizeof(identifier_.firmware));
    convert_ahci_string(identifier_.serial, sizeof(identifier_.serial));
    sector_size_ = identifier_.sectors;

    klib::logger::instance().log("%s, %s, %s", identifier_.model, identifier_.serial, identifier_.firmware);
}

void ahci::ahci_controller::sata_execute(hba_port *port, uint16_t command, size_t lba,
                                         uintptr_t buffer, size_t buffer_size)
{
    static_assert(sizeof(hba_to_device) == 20, "H2D struct must have 20 bytes");

    if ((buffer & 0x1) == 1) {
        klib::logger::instance().log("buffer address must be aligned");
        return;
    }

    // Basic Stpes when Building a Command - page 70 - $5.5.1
    // 5. Set PxCI.CI(pFreeSlot) to indicate HBA that command is active.
    auto slot = find_cmdslot(port, total_cmd_slots_);
    if (slot == -1) {
        // TODO: no slot found
        return;
    }

    // 3. Build command header at PxCLB[CH(pFreeSlot)]:
    auto cmd_list = reinterpret_cast<command_list*>((
                static_cast<uintptr_t>(port->comm_list_base_addr_hi) << 32 |
                port->comm_list_base_addr_lo) + KVIRTUAL_ADDRESS);
    cmd_list += slot;
    //    CFL set to the length of the command in CFIS area
    //    HBA uses this field to know the length of the FIS it will send to device
    //    sizeof(hba_to_device) / sizeof(int32_t) means 5 doublewords.
    cmd_list->command_fis_length = sizeof(hba_to_device) / sizeof(int32_t);
    //    W(rite) bit set if data is going to the device
    cmd_list->write = (command == 0x35) ? 1 : 0;
    //    P(refetch) optional
    cmd_list->prefetchable = 1;

    // Interrupt on Completion is the way to implement  a non-block read/write
    auto cmd_table = reinterpret_cast<command_table*>((
                static_cast<uintptr_t>(cmd_list->command_table_base_addr_hi) << 32 |
                cmd_list->command_table_base_addr_lo) + KVIRTUAL_ADDRESS);

    size_t sector_count = 0;
    auto len = klib::min(buffer_size, PRD_MAX_DATA_LENGTH) / cmd_list->prdt_length;
    for (uint16_t i = 0; i < cmd_list->prdt_length && len <= buffer_size; i++, len += len) {
        cmd_table->descriptor_entry[i].data_base_address_lo = buffer & 0xffff'ffff;
        cmd_table->descriptor_entry[i].data_base_address_hi = buffer << 32;
        cmd_table->descriptor_entry[i].data_byte_count = len;
        cmd_table->descriptor_entry[i].interrupt_on_completition = 1;
        buffer += len;
        sector_count++;
    }

    //    Physical Region Descriptor Table Length
    //    HBA uses this to know when to stop fetching PRDs. Each entry is 4 doublewords
    //    so we need to divide sector_count / 16 (4 doublewords * 4 bytes/doubleword) to
    //    know how many PRDs will be fetched. This can be 0 to 65,535 entries.
    cmd_list->prdt_length = sector_count;

    hba_to_device *to_dev = reinterpret_cast<hba_to_device*>(&cmd_table->command_cfis);
    LBA48_command(to_dev, command, lba, cmd_list->prdt_length);

    port->command_issue = 1 << slot;

    // Processing Completed Commands
    //    Processes the interrupt generated by device for command completition,
    //    in the interrupt service routine, software checks IS.IPS to determine
    //    which ports have an interupt pending. For each port that has interrupt
    //    pending:
    //    1. determine the cause of the interrupt by reading PxIS register.
    //    2. clear appropriate bits in PxIS corresponding to the cause of the
    //       interrupt
    //    3. clears interrupt bit in IS.IPS corresponding to the port
    //    4. if executing non-queued commands, reads PxCI register and compares the
    //       current value to the list of commands previously issued by software
    //       that are still outstanding. If executing native queued commands, reads
    //       PxSACT and compares current value to the list of commands previously issued
    //       by software. Software completes with success any outstanding command
    //       whose corresponding bit has been cleared in the respective register.
    //       PxCI and PxSACT are volatile registers, software should only use their
    //       values to determine commands that have completed, not which commands
    //       have previously been issued.
    //    5. If there were errors, noted in PxIS register, perform error recovery
    //       actions ($6.2.2).
    while (true) {
        if ((port->command_issue & (1 << slot)) == 0) {
            break;
        }

        if (port->interrupt_status & (1 << 30)) {
            return;
        }
    }

    if (port->interrupt_status & (1 << 30)) {
        return;
    }

    //
    // Transfer Examples: $5.6 - page 71
    // ---------------------+-----------------------------------------------------------
    //  Macro State         | HBA State Machine States
    //  --------------------+-----------------------------------------------------------
    //  Exam:Fetch          | P:idle -> P:selectCmd -> P:fetchCmd -> P:idle
    //  Exam:Transmit       | P:idle -> CFIS:Xmit -> CFIS:success -> P:idle
    //  Exam:AcceptNonData  | P:idle -> NDR:entry -> NDR:accept
    //  Exam:DMAReceive     | DR:entry -> DR:receive -> DR:updateByteCound -> P:idle
    //  Exam:DMATransmit    | DX:entry -> DX:transmit -> DX:updateByteCound -> P:idle
    //  Exam:D2HIntr        | regFIS:entry -> regFIS:clearCI -> regFIS:setIntr ->
    //                      | regFIS:setIS -> regFIS:genIntr -> regFIS:updateSig ->
    //                      | PM:aggr
    //  Exam:D2HNoIntr      | regFIS:entry -> regFIS:clearCI -> regFIS:updateSig ->
    //                      | PM:aggr
    //  Exam:DMASetup       | DmaSet:entry -> DmaSet:setIntr -> DmaSet:setIS ->
    //                      | DmaSet:genIntr -> DmaSet:autoActivate
    //  --------------------+-----------------------------------------------------------
    //
    //
    // DMA Data Trasnfers - page 71 - $5.6.2
    // 5.6.2.3 - ATA DMA Read
    // 1. Software builds a command as described in $5.5.1, the command must have
    //    a PRD table, is not ATAPI and is not queued. It's DMA read (data to memory)
    //    so CH(pFreeSlot).W must be 0. CH(pFreeSlot).P (prefetch) may be 1 or 0.
    // 2. HBA must transfer the command to the device traversing the macro states
    //    Exame:Fetch and Exam:Transmit.
    // 3. As a DMA read command, the response from device must be a Data FIS. When
    //    arrives, HBA must traverse Exam:DMAReceive macro state.
    // 4. If Data FIS doesn't satisfy the transfer count, another Data FIS must be
    //    sent from the device, HBA must traverse Exam:DMAReceive again, until the
    //    transfer count is satisfied. When Data FIS that completes the transfer
    //    count finishes, the next FIS from the device must be a D2H register FIS.
    // 5. HBA accept this FIS by traversing Exam:AcceptNonData. If D2H register FIS
    //    has the I vit set to 1, HBA must traverse the Exam:D2HIntr macro state.
    //    Otherwise, it must traverse Exam:D2HNoIntr state.

    // references:
    //   src/add-ons/kernel/busses/scsi/ahci/ahci_port.cpp (haiku)
    //   https://en.wikipedia.org/wiki/SCSI_CDB
    // switch (request->cdb[0]) {
    //     case SCSI_OP_TEST_UNIT_READY:
    //     break;
    //  }
    // https://github.com/haiku/haiku/blob/7f8f4c9c8c1d951b3fa1ad1b7315cf900c6b5fd6/src/add-ons/kernel/busses/scsi/ahci/ahci_port.cpp
    // https://github.com/haiku/haiku/blob/7f8f4c9c8c1d951b3fa1ad1b7315cf900c6b5fd6/src/add-ons/kernel/busses/scsi/ahci/ahci_port.cpp
    // https://github.com/haiku/haiku/blob/7f8f4c9c8c1d951b3fa1ad1b7315cf900c6b5fd6/src/add-ons/kernel/busses/scsi/ahci/ahci_port.cpp
}

void ahci::ahci_controller::sata_read(size_t lba, uintptr_t buffer, size_t buffer_size)
{
    auto port = sata_get_port();
    if (port == nullptr) {
        klib::logger::instance().log("Error: SATA read: can't find a SATA port available");
        return;
    }

    // 0x25 - Read DMA Ext
    sata_execute(port, 0x25, lba, buffer, buffer_size);
}

void ahci::ahci_controller::sata_write(size_t lba, uintptr_t buffer, size_t buffer_size)
{
    auto port = sata_get_port();
    if (port == nullptr) {
        klib::logger::instance().log("Error: SATA write: can't find a SATA port available");
        return;
    }

    // 0x35 - Write DMA Ext
    sata_execute(port, 0x35, lba, buffer, buffer_size);
}

bool ahci::ahci_controller::wait_until(uint32_t reg, uint32_t port, bool cond, uint32_t time)
{
    auto expect = (cond) ? port : 0;

    arch_factory::instance()->get_timer()->wait_for(time);

    // after sleeping for _time_ miliseconds, check if the bit has the value
    // expected. If so, return true.
    if ((reg & port) == expect) {
        return true;
    }

    // return false to inform that condition was not satisfied.
    return false;
}

// Initialization
// 8. ORDER:
//    a. hba_port.interrupt_status=0
//    b. interrupt_status_register.interrupt_status_pending=0
//    before programming
//    c. hba_port.interrupt_enable and
//    d. hba_memory.global_host_control.interrupt_enable
//    These will prevent residual bits set in these registers from causing an
//    interrupt to be asserted,
