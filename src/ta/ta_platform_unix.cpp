// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.


#include "ta_platform.h"

//#include <process.h>
#include <unistd.h>
#include <time.h>

// Unix implementations

const char    taPlatform::pathSep = '/'; 

int taPlatform::cpuCount() {
  return sysconf(_SC_NPROCESSORS_ONLN);
}

int taPlatform::exec(const String& cmd) {
  return system(cmd.chars());
}

String taPlatform::getTempPath() {
  String rval = "/tmp";
  return rval;
}

int taPlatform::processId() {
  return (int)getpid();
}

int taPlatform::tickCount() {
  return (int)clock();
}
