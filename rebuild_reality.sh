#!/bin/sh -f

ssh reality "cd emergent/build; svn up ../; make -j8; make -j8; make -j8; sudo make install"