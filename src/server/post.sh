#!/usr/bin/env bash

update-rc.d dsremap defaults

if [ ! -e "$INSTALL_DIR/venv" ]; then
    virtualenv -p `which python3` "$INSTALL_DIR/venv"
    . "$INSTALL_DIR/venv"
    pip install -r "$INSTALL_DIR/requirements.txt"
fi
