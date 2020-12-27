add_library(drivers.o STATIC keyboard.cpp
                            ide.cpp
                            bus/pci.cpp
                            controller/ahci.cpp)
