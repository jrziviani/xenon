add_subdirectory(bootstrap)
add_subdirectory(driver)
add_subdirectory(memory)
add_subdirectory(video)
add_subdirectory(proc)

add_library(amd64.o OBJECT amd64_interface.cpp)

target_link_libraries(amd64.o PUBLIC amd64_boot.o
                                     amd64_proc.o
                                     amd64_memory.o
                                     amd64_video.o
                                     amd64_driver.o)
