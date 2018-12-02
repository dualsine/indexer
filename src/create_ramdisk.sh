#!/bin/sh

#sudo umount $1 > /dev/null 2>&1

sudo mkdir -p $1 > /dev/null
sudo chmod -R 777 $1 > /dev/null
df -h | grep $1 > /dev/null
if [ $? != 0 ]; then
    sudo mount -t tmpfs -o size=$2 tmpfs $1
    echo "ramdisk created:"
    df -h
fi
