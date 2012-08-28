#!/bin/sh

# this script emailed by Jeremy Reynolds, coded by Randall O'Reilly

if [ "$#" -ne 3 ]; then
    echo "Must pass 3 atlas names: A = Main, B = New additions, C = merged output"
    exit 1
fi

ata=$1
atb=$2
atc=$3

echo "Appending atlas B = $atb onto end of atlas A = $ata into C = $atc"
echo "Any non-zero indexes in B take precedence over indexes in A, otherwise it defaults to A"

fslmaths $atb -thr 1 -bin B_nonzerovox
fslmaths B_nonzerovox.nii.gz -mul -1 -add 1 B_zerovox
fslmaths $ata -mas B_zerovox.nii.gz A_whenBiszero
Na=`fslstats $ata -R | awk '{print $2}'`
echo "Atlas A has $Na indexes"
fslmaths $atb -add $Na -mas B_nonzerovox.nii.gz -add A_whenBiszero.nii.gz $atc

/bin/rm B_nonzerovox* B_zerovox* A_whenBiszero*
