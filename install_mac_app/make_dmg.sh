#!/bin/bash

rtnm=$1

if [[ "$rtnm" == "" ]]; then
    echo "ERROR: you must specify file name without .pkg extension as only arg"
    exit 1
fi

srcnm="$rtnm"
dmgnm="$rtnm".dmg

echo "making dmg: $dmgnm from source directory: $srcnm"

hdiutil create -ov -srcfolder "$dmgnm" "$srcnm"
