add_library(driver_bus.o STATIC bus/pci.cpp)
add_library(driver.o STATIC keyboard.cpp
                            ahci.cpp
                            ide.cpp)
