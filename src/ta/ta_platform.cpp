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

#ifndef NO_TA_BASE
# include <QDir>
#endif

String taPlatform::finalSep(const String& in) {
  if (in.length() == 0)
    return String();
  //NOTE: don't use [] below, because of insane MS VC++ ambiguity
  else if (in.elem(in.length() - 1) == pathSep)
    return in;
  else return in + pathSep;
}

String taPlatform::getFileName(const String& in) {
  int pfs = posFinalSep(in);
  if (pfs < 0) return in;
  else return in.after(pfs);
}


String taPlatform::getFilePath(const String& in) {
  int pfs = posFinalSep(in);
  if (pfs < 0) return _nilString;
  else return in.before(pfs + 1); // we include the finalpos
}

#ifndef NO_TA_BASE
String taPlatform::getHomePath() {
  return String(QDir::homePath());
}
#endif

bool taPlatform::mkdir(const String& dir) {
#ifdef NO_TA_BASE
  return false;
#else
  QDir d;
  return d.mkpath(dir);
#endif
}

int taPlatform::posFinalSep(const String& in) {
  int rval = in.length() - 1;
  char c;
  while (rval >= 0) {
    c = in.elem(rval);
    if ((c == '/') || (c == '\\')  || (c == ':'))
      break;
    --rval;
  }
  return rval;
}

void taPlatform::sleep(int sec) {
  msleep(sec * 1000);
}

#ifdef TA_OS_WIN

// Windows implementation

#include "windows.h"

#define BUFSIZE 1024
char tmpbuf[BUFSIZE];

const char    taPlatform::pathSep = '\\'; 

int taPlatform::cpuCount() {
  SYSTEM_INFO info;
  info.dwNumberOfProcessors = 0;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
}

int taPlatform::exec(const String& cmd) {
  int rval = system(cmd.chars());
  // if allegedly successful, still need to test for error
  if (rval == 0) { 
    if (errno == ENOENT)
      rval = -1;
  }
  return rval;
}


String taPlatform::getAppDataPath(const String& appname) {
  return getHomePath() + "\\Application Data\\" + appname;
}

#ifdef NO_TA_BASE
String taPlatform::getHomePath() {
  return getenv("USERPROFILE");
}
#endif

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

void taPlatform::msleep(int msec) {
  Sleep(msec);
}

/*evil void taPlatform::usleep(int usec) {
#error "must implement usleep on Windows"
} */

int taPlatform::tickCount() {
  return (int)GetTickCount(); // is in ms
}

#elif defined(TA_OS_UNIX)

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
  int mib[2] = {CTL_HW, HW_NCPU};
  int ncpu;
  size_t len = sizeof(ncpu);
  sysctl(mib, 2, &ncpu, &len, NULL, 0);
  return ncpu;
#else
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

int taPlatform::exec(const String& cmd) {
  return system(cmd.chars());
}

String taPlatform::getAppDataPath(const String& appname) {
  return getHomePath() + "/." + appname;
}

#ifdef NO_TA_BASE
String taPlatform::getHomePath() {
  return getenv("HOME");
}
#endif

String taPlatform::getTempPath() {
  String rval = "/tmp";
  return rval;
}

int taPlatform::processId() {
  return (int)getpid();
}

void taPlatform::msleep(int msec) {
  //note: specs say max usleep value is 1s, so we loop if necessary
  while (msec > 1000) {
    usleep(1000000);
    msec -= 1000;
  }
  usleep(msec * 1000);
}

void taPlatform::usleep(int usec) {
  ::usleep(usec);
}


int taPlatform::tickCount() {
  return (int)clock();
}

#else // unknown platform
# error "undefined platform"
#endif // platform-specific routines
