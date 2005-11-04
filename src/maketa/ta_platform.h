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


// ta_platform.h: platform-specific routines

#ifndef TA_PLATFORM_H
#define TA_PLATFORM_H

#include "ta_def.h"
#include "ta_string.h"

class TA_API taPlatform { // class to hold static members for platform specific functions
public:

  static int processId(); // returns a process-specific Id
  static int tickCount(); // ticks since system started -- def of a 'tick' is system dependent

// File and Path routines
  static const char    pathSep; // normal file path separator character, ex / -- use this for construction only
  static String finalSep(const String& in); // return string that has a valid final separator
  static String getTempPath(); // get a valid temporary path (that user can write to)
  static String getFileName(const String& in); // retrieves only the filename&ext, w/o leading path info
};


#endif

