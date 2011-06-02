#!/bin/sh -f

ssh dream "cd emergent_mpi/build; svn up ../; make -j4; make -j4; make -j4; sudo make install"