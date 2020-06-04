#!/bin/bash
set -x
readonly DIR="$1"

[[ ! -f hda.qcow2 ]] && qemu-image create hda.qcow2 -f qcow2 256M

qemu-system-x86_64                                                          \
    -m 1G -machine q35,accel=tcg                                            \
    -cdrom "$DIR/xenon.iso"                                                 \
    -device ide-hd,bus=ide.0,drive=drive-sata0-0-0,id=sata0-0-0,bootindex=1 \
    -drive file=hda.qcow2,format=qcow2,if=none,id=drive-sata0-0-0           \
    -device piix3-usb-uhci,id=usb,bus=pcie.0,addr=0xc                       \
    -vga std                                                                \
    -s -S -d cpu_reset & disown
sleep 1
konsole --workdir $DIR -e gdb src/kernel/kernel
