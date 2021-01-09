include_directories(ext2)
add_library(filesystems.o STATIC ext2/ext2.cpp)
