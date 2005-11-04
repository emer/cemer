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
  String rval = in;
  bool cont = true;
  while (cont) {
    if (rval.contains(':'))
      rval = rval.after(':', -1);
    else if (rval.contains('/')) //note: works for "//" as well
      rval = rval.after('/', -1);
    else if (rval.contains('\\'))
      rval = rval.after('\\', -1);
    else cont = false;
  }
  return rval;
}
