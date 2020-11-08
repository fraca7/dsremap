#!/usr/bin/env bash

if [ "$UID" != "0" ]; then
    echo Must be run as root
    exit 1
fi

if [ "`uname -n`" != "raspberrypi" ]; then
    echo Must run on the RPi
    exit 1
fi

set -e

case "$1" in
    setup)
	# This is run in the chroot
	apt update
	apt -y install python3-aiohttp

	# Kernel configuration
	echo "dtoverlay=dwc2" >> /boot/config.txt
	echo "dwc2" >> /etc/modules
	echo "libcomposite" >> /etc/modules
	echo "usb_f_fs" >> /etc/modules

	# Enable HTTP server at startup
	update-rc.d dsremap defaults

	exit 0
	;;

    *)
	if [ "$1" == "" ]; then
	    echo Missing image file
	    exit 1
	fi

	losetup /dev/loop10 "$1"
	partprobe /dev/loop10

	mount -t auto /dev/loop10p2 /mnt
	mount -t auto /dev/loop10p1 /mnt/boot

	mount -t proc proc /mnt/proc
	mount --bind /dev /mnt/dev
	mount --bind /dev/pts /mnt/dev/pts
	mount --bind /sys /mnt/sys

	# Copy files before chrooting
	cp scripts/dsremap.service /mnt/etc/avahi/services/dsremap.service
	cp -a scripts/dsremap-initscript /mnt/etc/init.d/dsremap
	IMGSIZE=`du -b "$1" | cut -d\  -f1`
	sed -e "s/@IMGSIZE@/$IMGSIZE/g" scripts/extractcreds.py.in > /mnt/usr/sbin/extractcreds
	chmod 755 /mnt/usr/sbin/extractcreds
	patch -p0 /mnt/etc/init.d/resize2fs_once < resize2fs_once.patch

	pushd ../pairing && make clean all && popd

	mkdir /mnt/opt/dsremap
	cp -a ../pairing/pairing /mnt/opt/dsremap/pairing
	cp -a server/dsremap_serve.py /mnt/opt/dsremap/server

	# Clean obj files owned by root
	pushd ../pairing && make clean && popd

	# Copy self to root, relaunch in chroot
	name=`basename "$0"`
	cp $0 /mnt/$name
	chroot /mnt "/$name" setup
	rm /mnt/$name

	umount /mnt/sys
	umount /mnt/dev/pts
	umount /mnt/dev
	umount /mnt/proc
	umount /mnt/boot
	umount /mnt
	losetup -d /dev/loop10
      ;;
esac
