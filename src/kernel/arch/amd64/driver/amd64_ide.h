#ifndef AMD64_IDE
#define AMD64_IDE

#include <drivers/ide.h>
#include <drivers/bus/pci.h>

namespace xenon
{
    class amd64_ide : public ide
    {
    public:
        void on_disk_event() final;
    };

    void detect_ide(pci_info_t info);
}

#endif // AMD64_IDE
