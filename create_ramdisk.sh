#!/bin/sh
sudo mkdir -p $1
sudo mount -t tmpfs -o size=$2 tmpfs $1
df -h