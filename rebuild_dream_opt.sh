#!/bin/sh -f

ssh dream "cd emergent/build; svn up ../; make -j4; make -j4; make -j4; sudo make install"