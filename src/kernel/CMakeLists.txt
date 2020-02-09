include_directories(arch/x86_64)
include_directories(libs)
include_directories(mm)

set(MAX_PAGE_SIZE 0x1000)
set(LINKER_SCRIPT "kernel.ld")
set(LFLAGS "-fno-PIC -fno-pie -mno-red-zone -mcmodel=kernel -nostdlib -lgcc -Wl,-z,max-page-size=${MAX_PAGE_SIZE}")

add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-m64")
add_compile_options("-ffreestanding")
add_compile_options("-mcmodel=kernel")
add_compile_options("-mno-red-zone")
add_compile_options("-mno-sse")
add_compile_options("-mno-sse2")
add_compile_options("-mno-mmx")
add_compile_options("-fno-rtti")
add_compile_options("-fno-PIC")
add_compile_options("-fno-pie")
add_compile_options("-fno-exceptions")
add_compile_options("-z max-page-size=${MAX_PAGE_SIZE}")
add_definitions("-DDEBUG")

add_library(boot.o OBJECT arch/x86_64/boot.S)

add_executable(kernel
               main.cpp)

set_target_properties(kernel PROPERTIES
                      LINK_FLAGS "-T ${LINKER_SCRIPT} ${LFLAGS}")

target_link_libraries(kernel LINK_PUBLIC
                      boot.o)

add_custom_command(TARGET kernel PRE_LINK
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E echo "Copying LD script"
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/kernel/arch/x86_64/kernel.ld .)

add_custom_command(TARGET kernel POST_BUILD
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E echo_append "Generating ISO..."
                   COMMAND ${CMAKE_COMMAND} -E make_directory iso/boot/grub
                   COMMAND ${CMAKE_COMMAND} -E copy src/kernel/kernel iso/boot
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/contrib/grub.cfg iso/boot/grub
                   COMMAND grub2-mkrescue -o xenon.iso iso > /dev/null 2>&1
                   COMMAND ${CMAKE_COMMAND} -E remove_directory iso
                   COMMAND ${CMAKE_COMMAND} -E echo "Finished")

add_custom_target(qemu
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  COMMAND qemu-system-x86_64 -m 1G -cdrom xenon.iso
                  DEPENDS kernel)

add_custom_target(qemu-debug
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/contrib/gdbinit .gdbinit
                  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/contrib/debug.sh debug.sh
                  COMMAND ./debug.sh ${CMAKE_BINARY_DIR}
                  DEPENDS kernel)
