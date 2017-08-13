#!/bin/bash

# this will automatically fix the .h files to include the proper actual header file

#declare -a src_dirs=("../src/temt/quarter" )

for file in ../../src/temt/quarter/*.h
do
    echo $file
    incfl=$(basename $file)
    echo "#include \"$file\"" > $incfl
done
