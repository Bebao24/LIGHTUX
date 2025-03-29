#!/bin/bash

set -e

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "${SCRIPTPATH}"

## TODO: Add some check for update machanism

fetchLimine() {
    rm -rf limine/
    git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1
    make -C limine
}

if [[ ! -f "limine/limine" ]]; then
    fetchLimine
fi


