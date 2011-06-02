#!/bin/sh -f

ssh -t dream "cd emergent_mpi/build; svn up ../; make -j4; make -j4; make -j4; sudo make install"