#!/bin/sh -f

ssh -t dream "cd emergent/build_mpi; svn up ../; make -j4; make -j4; sudo make install"

exec ./rebuild_notify.sh

# this version will rebuild plugins with last call
#ssh -t dream "cd emergent/build_mpi; svn up ../; make -j4; make -j4; sudo make install; emergent_mpi -nogui -ni"
