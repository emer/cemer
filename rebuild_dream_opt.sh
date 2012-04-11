#!/bin/sh -f

ssh -t dream "cd emergent/build; svn up ../; make -j4; make -j4; make -j4; sudo make install; emergent -nogui -ni"