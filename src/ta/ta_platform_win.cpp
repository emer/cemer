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


// Windows implementations

#include "ta_platform.h"
#include "windows.h"

#define BUFSIZE 1024
char tmpbuf[BUFSIZE];

const char    taPlatform::pathSep = '\\'; 

int taPlatform::cpuCount() const {
  SYSTEM_INFO info;
  info.dwNumberOfProcessors = 0;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
}

int taPlatform::exec(const String& cmd) {
#error "must implement taPlatform::exec"
//unix  return system(cmd.chars());
}

String taPlatform::getTempPath() {
  String rval;
  DWORD retVal = GetTempPath(BUFSIZE, tmpbuf);
  if (retVal != 0)
    rval = String(tmpbuf);
  return rval;
}
int taPlatform::processId() {
  return (int)GetCurrentProcessId();
}
int taPlatform::tickCount() {
  return (int)GetTickCount(); // is in ms
}
