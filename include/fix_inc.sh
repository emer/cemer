#!/bin/bash

# this will automatically fix the .h files to include the proper actual header file

declare -a src_dirs=("../src/temt/ta_core" "../src/temt/ta_math" "../src/temt/ta_program" "../src/temt/ta_data" "../src/temt/ta_proj" "../src/temt/ta_gui" "../src/temt/ta_3d" "../src/temt/css" "../src/temt/json" "../src/emergent/virt_env" "../src/emergent/network" "../src/emergent/leabra" "../src/emergent/bp" "../src/emergent/cs" "../src/emergent/so" "../src/emergent/actr")

for file in *.h
do
    #    echo $file
    got="no"
    for dir in "${src_dirs[@]}"
    do
	 full_dir=$dir/$file 
	 if [ -f $full_dir ]; then
#	     echo "#include \"$full_dir\""
	     echo "#include \"$full_dir\"" > $file
	     got="yes"
	 fi
    done
    if [ "$got" == "no" ]; then
	echo "$file does not have a source!"
	cat $file
	echo "should this file be removed?"
	read rmyn
	if [ "$rmyn" == "y" ]; then
	    svn rm $file
	fi
    fi
done
