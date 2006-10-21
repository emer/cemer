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

#ifdef TA_OS_MAC
# include <sys/types.h>
# include <sys/sysctl.h>
#endif

// Unix implementations

const char    taPlatform::pathSep = '/'; 

int taPlatform::cpuCount() {
#ifdef TA_OS_MAC
  int mib[2];
  int ncpu;
  size_t len;
  mib[0] = CTL_HW;
  mib[1] = HW_NCPU;
  len = sizeof(ncpu);
  sysctl(mib, 2, &ncpu, &len, NULL, 0);
  return ncpu;
#else
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
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
