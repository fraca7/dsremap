#!/usr/bin/env bash

set -e

BASEDIR=`realpath "$0"`
BASEDIR=`dirname "$BASEDIR"`

. "$BASEDIR/venv/bin/activate"
python "$BASEDIR/dsremap_serve.py" "$@"
