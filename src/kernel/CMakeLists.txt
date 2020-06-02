include_directories(klib)

add_subdirectory(appendix)
add_subdirectory(arch)
add_subdirectory(memory)
add_subdirectory(proc)
add_subdirectory(drivers)

set(MAX_PAGE_SIZE 0x1000)
set(LINKER_SCRIPT "kernel.ld")
set(LFLAGS "-fno-PIC -fno-pie -fno-exceptions -fno-rtti -mno-red-zone -mcmodel=kernel -nostdlib -lgcc -Wl,-z,max-page-size=${MAX_PAGE_SIZE}")

add_executable(kernel
               klib/new.cpp
               main.cpp)

set_target_properties(kernel PROPERTIES
                      LINK_FLAGS "-T ${LINKER_SCRIPT} ${LFLAGS}")

target_link_libraries(kernel LINK_PUBLIC
                      amd64.o
                      proc.o
                      memory.o
                      driver_bus.o
                      appendix.o)

add_custom_command(TARGET kernel PRE_LINK
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E echo "Copying LD script"
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/kernel/arch/amd64/bootstrap/kernel.ld ./src/kernel)

add_custom_command(TARGET kernel POST_BUILD
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E echo_append "Generating ISO..."
                   COMMAND ${CMAKE_COMMAND} -E make_directory iso/boot/grub
                   COMMAND ${CMAKE_COMMAND} -E copy src/kernel/kernel iso/boot
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/contrib/grub.cfg iso/boot/grub
                   COMMAND ${GRUBRESCUE} -o xenon.iso iso > /dev/null 2>&1
                   COMMAND ${CMAKE_COMMAND} -E remove_directory iso
                   COMMAND ${OBJDUMP} -S src/kernel/kernel > kernel.asm
                   COMMAND ${OBJDUMP} -t src/kernel/kernel > kernel.sym
                   COMMAND ${CMAKE_COMMAND} -E echo "Finished")

add_custom_target(qemu
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  COMMAND qemu-img create hda.qcow2 -f qcow2 256M
                  COMMAND qemu-system-x86_64
                      -m 1G
                      -machine q35,accel=tcg
                      -cdrom xenon.iso
                      -d cpu_reset
                      -no-shutdown
                      -device ide-hd,bus=ide.0,drive=drive-sata0-0-0,id=sata0-0-0,bootindex=1
                      -drive file=hda.qcow2,format=qcow2,if=none,id=drive-sata0-0-0
                  DEPENDS kernel)

add_custom_target(qemu-debug
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/contrib/gdbinit .gdbinit
                  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/contrib/debug.sh debug.sh
                  COMMAND ./debug.sh ${CMAKE_BINARY_DIR}
                  DEPENDS kernel)

add_custom_target(bochs
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/contrib/bochsrc.txt bochsrc.txt
                  COMMAND bochs -f bochsrc.txt -q
                  DEPENDS kernel)

add_custom_target(bochs-debugger
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/contrib/bochsrc.txt bochsrc.txt
                  COMMAND bochs-debugger -f bochsrc.txt -q
                  DEPENDS kernel)
