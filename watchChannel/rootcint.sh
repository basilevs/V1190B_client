#!/bin/sh

TARGET="$PWD"
cd ..
rootcint -f rootdict.cpp -c gui.h LinkDef.h || exit 1
true