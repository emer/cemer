#!/bin/sh -f

ssh -t reality "cd emergent/build; svn up ../; make -j4; make -j4; sudo make install"

# this version will rebuild plugins with last call
#ssh -t reality "cd emergent/build; svn up ../; make -j4; make -j4; sudo make install; emergent -nogui -ni"
