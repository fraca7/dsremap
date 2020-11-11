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
	apt -y install python3-aiohttp python3-daemon libbluetooth3

	# Kernel configuration
	echo "dtoverlay=dwc2" >> /boot/config.txt
	echo "dwc2" >> /etc/modules
	echo "libcomposite" >> /etc/modules
	echo "usb_f_fs" >> /etc/modules

	bash "/$2"

	exit 0
	;;

    *)
	if [ "$1" == "" ]; then
	    echo Missing image file
	    exit 1
	fi

	if [ "$2" == "" ]; then
	    echo Missing installer file
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

	IMGSIZE=`du -b "$1" | cut -f1`
	sed -e "s/@IMGSIZE@/$IMGSIZE/g" extractcreds.py.in > /mnt/usr/sbin/extractcreds
	chmod 755 /mnt/usr/sbin/extractcreds
	patch -p0 /mnt/etc/init.d/resize2fs_once < resize2fs_once.patch

	# Copy self and installer to root, relaunch in chroot
	name=`basename "$0"`
	inst=`basename "$2"`
	cp $0 /mnt/$name
	cp $2 /mnt
	chroot /mnt "/$name" setup $inst
	rm /mnt/$name
	rm /mnt/$inst

	umount /mnt/sys
	umount /mnt/dev/pts
	umount /mnt/dev
	umount /mnt/proc
	umount /mnt/boot
	umount /mnt
	losetup -d /dev/loop10
      ;;
esac
