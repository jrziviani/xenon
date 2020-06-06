# TODO: ugly hack to link derived and base classes together
#       so I can call a base class function from the derived one.
add_library(amd64_driver.o STATIC amd64_timer.cpp
                                  amd64_keyboard.cpp
                                  ../../../drivers/keyboard.cpp
                                  amd64_ide.cpp
                                  amd64_pci.cpp)
