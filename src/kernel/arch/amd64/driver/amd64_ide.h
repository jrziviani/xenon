#ifndef AMD64_IDE
#define AMD64_IDE

#include <drivers/ide.h>
#include <drivers/bus/pci.h>

class amd64_ide : public ide
{
public:
    void on_disk_event() final;
};

#endif // AMD64_IDE
