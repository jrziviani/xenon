cmake_minimum_required (VERSION 3.9.2)
project (hmos VERSION 0.0
              DESCRIPTION "homemade kernel"
              LANGUAGES CXX ASM)

set(CUSTOM_C x86_64-elf-gcc)
set(CUSTOM_CXX x86_64-elf-g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER ${CUSTOM_C})
set(CMAKE_CXX_COMPILER ${CUSTOM_CXX})
set(CMAKE_ASM_COMPILER ${CUSTOM_CXX})

add_subdirectory(src)
