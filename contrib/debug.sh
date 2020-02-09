#!/bin/bash
set -x
readonly DIR="$1"

qemu-system-x86_64 -m 1G -cdrom "$DIR/xenon.iso" -S -s & disown
sleep 1
konsole --workdir $DIR -e gdb src/kernel/kernel
