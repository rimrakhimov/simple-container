#!/bin/bash

IMG_NAME="fs_image.img"

# creates a new 1Gb image file if does not already exist
if ! test -f $IMG_NAME; then
	printf "\nImage file is creating\n"
	dd if=/dev/zero of=$IMG_NAME bs=100M count=10
else
	printf "\nImage file already exists\n"
fi

# create a 'ext4' filesystem if it does not already exist
CURRENT_FS=$(blkid -o value -s TYPE $IMG_NAME)
if [ $CURRENT_FS = "ext4" ]; then
	printf "\n'ext4' filesystem has already been created\n"
else
	printf "\nFilesystem is creating\n"
	mkfs.ext4 $IMG_NAME
fi
