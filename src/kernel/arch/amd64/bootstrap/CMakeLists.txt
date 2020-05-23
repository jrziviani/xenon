add_library(amd64_boot.o STATIC boot.S
                                gdt.S
                                isr.S
                                exception.cpp
                                segments.cpp)
