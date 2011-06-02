#!/bin/sh -f

ssh -t grey "cd emergent/build_dbg; svn up ../; make -j2; make -j2; make -j2; sudo make install"