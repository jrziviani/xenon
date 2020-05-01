#!/bin/bash
set -x
readonly DIR="$1"

qemu-system-x86_64 -m 1G -machine q35,accel=tcg -cdrom "$DIR/xenon.iso" -s -S -d cpu_reset & disown
sleep 1
konsole --workdir $DIR -e gdb src/kernel/kernel