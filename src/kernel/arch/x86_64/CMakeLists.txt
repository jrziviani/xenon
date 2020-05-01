add_subdirectory(memory)
add_subdirectory(video)

add_library(x86_64.o OBJECT boot.S
                            isr.S
                            gdt.S
                            x86_interface.cpp
                            x86_timer.cpp
                            exception.cpp
                            segments.cpp)

target_link_libraries(x86_64.o PUBLIC x86_64_memory.o
                                      x86_64_video.o)
