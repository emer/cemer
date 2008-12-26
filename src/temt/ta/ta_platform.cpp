// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "ta_type.h"

#ifndef NO_TA_BASE
# include <QDir>
#endif

bool taPlatform::fileExists(const String& fname) {
  bool rval = false;
  fstream fin;
  fin.open(fname, ios::in);
  if (fin.is_open()) {
    rval = true;
  }
  fin.close();
  return rval;
}

String taPlatform::finalSep(const String& in) {
  if (in.length() == 0)
    return String();
  else {
    //NOTE: don't use [] below, because of insane MS VC++ ambiguity
    char c = in.elem(in.length() - 1);
    // note: need to check both seps, because of whacky Win/Cygwin mixtures
    if (( c == '\\') || (c == '/'))
      return in;
  } 
  return in + pathSep;
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

#ifdef NO_TA_BASE
String taPlatform::getHomePath() {
  return _nilString;
}
#endif

bool taPlatform::isQualifiedPath(const String& fname) {
//NOTE: we just check the union of Unix and Win -- we aren't asked if it 
// is valid for the platform, so ok to check all cases here
  if (fname.empty()) return false;
  char c = fname.elem(0);
  // handles all relative, and Unix absolute, and Win share paths
  if ((c == '.') || (c == '\\') || (c == '/')) return true; 
#ifdef TA_OS_WIN
  // Win x: drive letter path
  if ((fname.length() >= 2) && (fname.elem(1) == ':')) return true;
#endif
  return false;
}

bool taPlatform::mkdir(const String& dir) {
#ifdef NO_TA_BASE
  return false;
#else
  QDir d;
  return d.mkpath(dir);
#endif
}

bool taPlatform::mv(const String& fm, const String& to) {
#ifdef NO_TA_BASE
  return false;
#else
  QDir d;
  return d.rename(fm, to);
#endif
}

String taPlatform::noFinalSep(const String& in) {
  String rval = in;
  char c;
  while ((c = rval.lastchar()) && 
    ( (c == '/') || (c == '\\')))
    rval.truncate(rval.length() - 1);
  return rval;
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

String taPlatform::unescapeBackslash(const String& in) {
  // convert lexical double backslash to actual single backslash
  // i.e. you are reading C source, and want to convert to the string 
  String rval = in;
  rval.gsub("\\\\", "\\");
  return rval;
}

#ifdef TA_OS_WIN

// Windows implementation

#include "windows.h"
#include <errno.h>
#ifndef NO_TA_BASE
# include "shlobj.h"
#endif

#define BUFSIZE 1024
char tmpbuf[BUFSIZE];

const String taPlatform::pathSep('\\'); 

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
//note: sleazy, we should use the complicated way, but this seems to be correct...
// note: for Windows, we add the orgname, to keep it compliant
  return getenv("APPDATA") + PATH_SEP + taMisc::org_name + PATH_SEP + appname;
}

#ifndef NO_TA_BASE

String taPlatform::getDocPath() {
//NOTE: we don't want the "home" folder, we really want the user's My Documents folder
// since this can be moved from its default place, and/or renamed, we have to do this:
  TCHAR szPath[MAX_PATH];
  if(SUCCEEDED(SHGetFolderPath(
    NULL, 
    CSIDL_PERSONAL, 
    NULL, 
    0, 
    szPath))) 
  {
    return String(szPath);
  } else {
    taMisc::Warning("taPlatform::getHomePath(): SHGetFolderPath() failed unexpectedly, guessing at user's home folder...");
    return const_cast<const char*>(getenv("USERPROFILE")) + PATH_SEP + "My Documents";
  }
}

String taPlatform::getAppDocPath(const String& appname) {
  return getHomePath() + "\\" + capitalize(appname);
}

String taPlatform::getHomePath() {
  return String(getenv("USERPROFILE"));
}
#endif

String taPlatform::getTempPath() {
  String rval;
  DWORD retVal = GetTempPath(BUFSIZE, tmpbuf);
  if (retVal != 0)
    rval = String(tmpbuf);
  return rval;
}

String taPlatform::hostName() {
  return String(getenv("COMPUTERNAME"));
}

String taPlatform::lexCanonical(const String& in) {
  //NOTE: this routine will probably fail if server shares are used -- use mapped drive letters instead
  // ex. \\myserver\myshare\myfile.xx --> map \\myserver\myshare to Z: -> Z:\myfile
  // first, remove any double backslashes
  String rval = unescapeBackslash(in);
  // then, convert all forward slashes to Windows backslashes
  //  (this is the safest common-denominator)
  rval.gsub("/", "\\");
  // remove any double backslashes AGAIN -- this happens with gcc/maketa/mingw funky: dir\\/file
  rval = unescapeBackslash(rval);
  // note: we assume that pathing is case-exact (even though win filenames are case-insensitive)
  // but we do convert drive letter to upper case
  String drv = rval.before(":");
  if (drv.nonempty()) {
    drv.upcase();
    rval = drv + rval.from(":");
  }
  //TODO: maybe we should do a case conversion here, but possibly only to drive letter
  return rval;
}

void taPlatform::msleep(int msec) {
  Sleep(msec);
}

int taPlatform::processId() {
  return (int)GetCurrentProcessId();
}

int taPlatform::tickCount() {
  return (int)GetTickCount(); // is in ms
}

String taPlatform::userName() {
  return String(getenv("USERNAME"));
}

#elif defined(TA_OS_UNIX)

#include <unistd.h>
#include <time.h>

#ifdef TA_OS_MAC
# include <sys/types.h>
# include <sys/sysctl.h>
#endif

// Unix implementations

const String  taPlatform::pathSep('/'); 

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

#ifdef TA_OS_MAC
String taPlatform::getAppDocPath(const String& appname) {
  return getHomePath() + "/Library/" + capitalize(appname);
}
#else
String taPlatform::getAppDocPath(const String& appname) {
  return getHomePath() + "/lib/" + capitalize(appname);
}
#endif

#ifndef NO_TA_BASE
String taPlatform::getDocPath() {
  return getHomePath() + "/Documents";
}

String taPlatform::getHomePath() {
 // return getenv("HOME");
 return QDir::homePath();
}
#endif

String taPlatform::getTempPath() {
  String rval = "/tmp";
  return rval;
}

String taPlatform::hostName() {
  return String(getenv("HOSTNAME"));
}

String taPlatform::lexCanonical(const String& in) {
  // in Unix, we don't use backslashes, and all filename 
  // components are strictly case sensitive, so we are already
  // lexically canonical
/*note: we could do this... but note, not avail on win32/msvc
  char resolved_path[PATH_MAX];
  char* r = realpath(in, resolved_path);
  if (r) {
    return r; // same as our buff
  } */
  return in;
}


void taPlatform::msleep(int msec) {
  //note: specs say max usleep value is 1s, so we loop if necessary
  while (msec > 1000) {
    usleep(1000000);
    msec -= 1000;
  }
  usleep(msec * 1000);
}

int taPlatform::processId() {
  return (int)getpid();
}

int taPlatform::tickCount() {
  return (int)clock();
}

String taPlatform::userName() {
  return String(getenv("USER"));
}

#else // unknown platform
# error "undefined platform"
#endif // platform-specific routines
