#!/bin/bash

set -e

die() {
    printf "$1\n" >&2
    exit $2
}

print_header() {
    printf '*************************************************************\n'
    printf '\t%s\n' "$1"
    printf '*************************************************************\n'
}

print_message() {
    printf " --> %s\n" "$1"
}

assert_target() {
    local target="$1"

    [[ "$target" != "release" && "$target" != "debug" ]] &&
       die "Unexpected target $target" 2
}

config_cross_compiler() {
    print_header "Setup the cross compiler"

    local contrib="$PWD/contrib"

    if [[ -d .build/compiler/bin && -f .build/compiler/.done ]]
    then
        export PATH=$PATH:$PWD/.build/compiler/bin
        print_message "Compiler looks fine, return"
        return
    fi

    mkdir -p .build/compiler/tmp
    pushd .build/compiler
        local cpath=$PWD

        pushd tmp
            if [[ ! -f .binutils ]]
            then
                if [[ ! -f binutils-2.34.tar.xz ]]
                then
                    print_message "Downloading binutils"
                    curl -O https://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.xz
                fi

                print_message "Building binutils"
                tar xf binutils-2.34.tar.xz
                mkdir -p build-binutils
                pushd build-binutils
                    ../binutils-2.34/configure \
                        --target=x86_64-elf \
                        --prefix="$cpath" \
                        --with-sysroot \
                        --disable-nls \
                        --disable-werror

                    make -j $(nproc)
                    make install
                popd # build-binutils
                touch .binutils
                rm binutils-2.34.tar.xz
            fi

            if [[ ! -f .gcc ]]
            then
                if [[ ! -f gcc-9.3.0.tar.xz ]]
                then
                    print_message "Downloading gcc"
                    curl -O https://ftp.gnu.org/gnu/gcc/gcc-9.3.0/gcc-9.3.0.tar.xz
                fi

                print_message "Building gcc"
                tar xf gcc-9.3.0.tar.xz
                # cp "$contrib/t-x86_64-elf" gcc-9.3.0/gcc/config/i386/
                # cp "$contrib/config.gcc" gcc-9.3.0/gcc/

                export "LD_LIBRARY_PATH=/usr/lib64:$LD_LIBRARY_PATH"

                mkdir -p build-gcc
                pushd build-gcc
                    export PATH="$PATH:$cpath/bin"
                    export TARGET=x86_64-elf
                    ../gcc-9.3.0/configure \
                        --target=x86_64-elf \
                        --prefix="$cpath" \
                        --disable-nls \
                        --enable-languages=c,c++ \
                        --without-headers

                    make -j $(nproc) all-gcc
                    make -j $(nproc) all-target-libgcc CFLAGS_FOR_TARGET='-O2 -mcmodel=large -mno-red-zone'
                    make install-gcc
                    make install-target-libgcc
                popd # build-gcc
                touch .gcc
                rm gcc-9.3.0.tar.xz
            fi
        popd # tmp

        rm -fr tmp
        touch .done
        print_message "Done"
    popd # build/compiler

    export PATH=$PATH:$PWD/.build/compiler/bin
}

makeit() {
    print_header "Runing CMake"

    local target="release"
    local verbose=""
    [[ $1 -eq 1 ]] && target="debug -DCMAKE_BUILD_TYPE=Debug"
    [[ $2 -eq 1 ]] && verbose="--verbose"

    print_message "target: $target, verbose: $2"

    mkdir -p .build
    pushd .build > /dev/null
    cmake -H.. -B$target -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    popd > /dev/null

    buildit $1 $2
}

buildit() {
    print_header "Building"

    [[ -d .build ]] || die "build dir doesn't exist" 1

    local target="release"
    local verbose=""
    [[ $1 -eq 1 ]] && target="debug"
    [[ $2 -eq 1 ]] && verbose="--verbose"

    pushd .build > /dev/null
    cmake --build $target $verbose
    popd > /dev/null

    ln -fs .build/$target/compile_commands.json .
}

clean() {
    print_header "Cleaning"

    [[ -d .build ]] || die "build dir doesn't exist" 1

    local target="release"
    local verbose=""
    local force=$3
    [[ $1 -eq 1 ]] && target="debug"
    [[ $2 -eq 1 ]] && verbose="--verbose"

    pushd .build > /dev/null
    if [[ ! -d $target ]]
    then
        print_message "Target $target doesn't exist, exit"
    else
        if [[ $force -eq 1 ]]
        then
            rm -fr $target
        else
            cmake --build $target --target clean $verbose
        fi
    fi

    popd > /dev/null
}

qemuit() {
    print_header "Running QEMU"

    [[ -d .build ]] || die "build dir doesn't exist" 1

    local target="release"
    local qemu="qemu"
    [[ $1 -eq 1 ]] && target="debug"
    [[ $2 -eq 1 ]] && qemu="qemu-debug"

    pushd .build > /dev/null
    if [[ ! -d $target ]]
    then
        print_message "Target $target doesn't exist, exit"
    else
        cmake --build $target --target $qemu
    fi

    popd > /dev/null
}

bochsit() {
    print_header "Running BOCHS"

    [[ -d .build ]] || die "build dir doesn't exist" 1

    local target="release"
    local bochs="bochs"
    [[ $1 -eq 1 ]] && target="debug"
    [[ $2 -eq 1 ]] && bochs="bochs-debugger"

    pushd .build > /dev/null
    if [[ ! -d $target ]]
    then
        print_message "Target $target doesn't exist, exit"
    else
        cmake --build $target --target $bochs
    fi

    popd > /dev/null
}

build() {
    local qemu=${1}
    local test=${2}
    local clean=${3}
    local debug=${4}
    local build=${5}
    local cmake=${6}
    local infra=${7}
    local verbose=${8}
    local gdb=${9}
    local bochs=${10}
    local force=${11}

    [[ $infra -eq 1 ]] && config_cross_compiler
    [[ $clean -eq 1 ]] && clean $debug $verbose $force
    [[ $cmake -eq 1 ]] && makeit $debug $verbose
    [[ $qemu -eq 1 ]]  && qemuit $debug $gdb
    [[ $bochs -eq 1 ]]  && bochsit $debug $gdb
}

helpme() {
    printf "run %s [options]\n" "$0"
    printf "Options:\n"
    printf "\t--build   (-b):\tBuild release target (+binutils and +gcc if first time)\n"
    printf "\t--clean   (-c):\tClean old objects (binutils and gcc won't be removed)\n"
    printf "\t--force   (-f):\tUse with --clean to completly remove a target \n"
    printf "\t--debug   (-d):\tBuild debug target\n"
    printf "\t--qemu    (-q):\tRun OS in QEMU emulator\n"
    printf "\t--gdb     (-g):\tRun OS in QEMU with GDB for debugging\n"
    printf "\t--bochs   (-x):\tRun OS in BOCHS emulator\n"
    printf "\t--verbose (-v):\tShow details during the build process\n"
    printf "\t--help    (-h):\tPrint this help\n"
}

main() {
    local action="$1"
    local gdb=0
    local qemu=0
    local test=0
    local clean=0
    local debug=0
    local build=0
    local cmake=0
    local infra=0
    local bochs=0
    local force=0
    local verbose=0

    local cnt=0
    while [[ "$1" =~ -[a-z] || "$1" =~ --[a-z]+ ]]; do
        local opt="$1"
        shift

        case "$opt" in
            -b|--build)
                infra=1
                cmake=1
                ;;

            -c|--clean)
                clean=1
                ;;

            -d|--debug)
                debug=1
                ;;

            -q|--qemu)
                qemu=1
                ;;

            -g|--gdb)
                gdb=1
                ;;

            -x|--bochs)
                bochs=1
                ;;

            -f|--force)
                force=1
                ;;

            -v|--verbose)
                verbose=1
                ;;

            -h|--help)
                helpme
                exit 0
                ;;

            *)
                printf "Wrong options: %s\n" "$opt"
                helpme
                exit 1
                ;;
        esac
        cnt=$((cnt+1))
    done

    if [[ $cnt == 0 ]]
    then
        printf "Wrong options: %s\n" "$@"
        helpme
        exit 1
    fi

    [[ $qemu == 1 ]] && bochs=0

    build $qemu $test $clean $debug $build $cmake $infra $verbose $gdb $bochs $force
}

main $@
