include_directories(controller)
add_library(drivers.o STATIC keyboard.cpp
                             bus/pci.cpp
                             controller/ahci.cpp)
