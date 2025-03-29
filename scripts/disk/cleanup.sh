#!/bin/bash

if [ -z "${1}" ]; then
    echo "Please provide the correct arguments!"
    exit 1
fi

sudo umount -l "${1}"
sudo losetup -d /dev/loop101


