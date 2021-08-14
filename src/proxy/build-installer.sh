#!/usr/bin/env bash

set -e

SRC_DIR=$1
BIN_DIR=$2
VERSION=$3

cd "$BIN_DIR/dist"
rm -f dsremap-proxy-*.tgz
rm -f dsremap-proxy-*.sh
tar czf dsremap-proxy-$VERSION.tgz *
TARSIZE=`du -b dsremap-proxy-$VERSION.tgz | cut -f1`
sed -e "s/@TARSIZE@/$TARSIZE/g" $SRC_DIR/installer.sh.in > dsremap-proxy-$VERSION.sh
cat dsremap-proxy-$VERSION.tgz >> dsremap-proxy-$VERSION.sh
