#!/bin/sh -f

mkdir build_dbg
cd build_dbg; cmake ../ -DCMAKE_BUILD_TYPE=Debug; make; sudo make install
