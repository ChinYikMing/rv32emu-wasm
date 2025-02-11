#!/usr/bin/env bash

set -e -u -o pipefail

. .ci/common.sh

check_platform
mkdir -p toolchain

if [[ "$#" == "0" ]] || [[ "$1" != "riscv-collab" ]]; then
    GCC_VER=14.2.0-3
    TOOLCHAIN_REPO=https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack

    if [[ ${MACHINE_TYPE} == "x86_64" ]]; then
        TOOLCHAIN_URL=${TOOLCHAIN_REPO}/releases/download/v${GCC_VER}/xpack-riscv-none-elf-gcc-${GCC_VER}-linux-x64.tar.gz
    elif [[ ${MACHINE_TYPE} == "aarch64" ]]; then
        TOOLCHAIN_URL=${TOOLCHAIN_REPO}/releases/download/v${GCC_VER}/xpack-riscv-none-elf-gcc-${GCC_VER}-linux-arm64.tar.gz
    elif [[ ${MACHINE_TYPE} == "arm64" ]]; then
        TOOLCHAIN_URL=${TOOLCHAIN_REPO}/releases/download/v${GCC_VER}/xpack-riscv-none-elf-gcc-${GCC_VER}-darwin-arm64.tar.gz
	echo "----------------------------------arm64 xPack toolchain-------------------------------------"
	echo "TOOLCHAIN URL: ${TOOLCHAIN_URL}"
    fi
else
    UBUNTU_VER=`lsb_release -r | cut -f2`
    GCC_VER=2025.01.20
    TOOLCHAIN_REPO=https://github.com/riscv-collab/riscv-gnu-toolchain
    TOOLCHAIN_URL=${TOOLCHAIN_REPO}/releases/download/${GCC_VER}/riscv32-elf-ubuntu-${UBUNTU_VER}-gcc-nightly-${GCC_VER}-nightly.tar.xz
fi

wget -q ${TOOLCHAIN_URL} -O- | tar -C toolchain --strip-components=1 -xz
