#!/bin/sh

# this script coded by Randall O'Reilly

if [ "$#" -ne 4 ]; then
    echo "Must pass 4 args: 1 = atlas, 2 = start index to extract, 3 = end index to extract, 4 = output atlas"
    exit 1
fi

atin=$1
atst=$2
ated=$3
atout=$4

atstm1=$[$atst-1]
echo $atstm1

echo "Extracting indexes $atst - $ated (inclusive) from $atin to $atout"

fslmaths $atin -thr $atst -uthr $ated -sub $atstm1 $atout
