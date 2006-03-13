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

// Code common to all platforms

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

