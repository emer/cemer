#!/bin/sh -f

ssh -t reality "cd emergent/build; svn up ../; make -j8; make -j8; make -j8; sudo make install; emergent -nogui -ni"