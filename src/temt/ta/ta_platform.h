// Copyright, 1995-2005, Regents of the University of Colorado,
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


// ta_platform.h: platform-specific routines

#ifndef TA_PLATFORM_H
#define TA_PLATFORM_H

#include "ta_def.h"
#include "ta_string.h"

#define PATH_SEP taPlatform::pathSep

class TA_API taPlatform { // class to hold static members for platform specific functions
public:

// Computer info, and timing
  static int	      cpuCount(); // number of physical cpus
  static String	      hostName(); // name of the computer
  static int	      processId(); // returns a process-specific Id
  static int	      tickCount(); // ticks since system started -- def of a 'tick' is system dependent

// File and Path routines
  static const String    pathSep; // normal file path separator character, ex / -- use this for construction of paths only (use qt parsing routines to parse paths)
  static String finalSep(const String& in); // return string that has a valid final separator
  static bool		fileExists(const String& fname); // returns true if the file exists (can be absolute or partial pathed)
  static bool		isQualifiedPath(const String& fname); // true if the fname is already an absolute or qualified relative path
  static String getTempPath(); // get a valid temporary path (that user can write to)
  static String getHomePath(); // the user's home folder
  static String getAppDataPath(const String& appname); // root for preference data
  static String getFileName(const String& in); // retrieves only the filename&ext, w/o leading path info
  static String getFilePath(const String& in); // retrieves only the file path, if any (no final separator)
  static int	posFinalSep(const String& in); // position of final path separator, -1 if not found
  static bool	mkdir(const String& dir); // make or assert the directory, true if ok
  static String unescapeBackslash(const String& in); 
   // insures that C escaped backslashes are decoded -- typically used by maketa when reading from preprocesser {#[line] Xxx <fname>} values
  static String lexCanonical(const String& in); 
   // a canonical lexical form, for comparing exactly paths/files (note: does not dereference .. etc.); also insures the C escaped backslashes are decoded
  
// User Info
  static String	      userName(); // username of logged in user
  
// Process execution
  static int	exec(const String& cmd); //
  
// Sleeping
  static void	sleep(int sec); // sleep the specified number of seconds
  static void	msleep(int msec); // sleep the specified number of milliseconds
//  static void	usleep(int usec); // #IGNORE sleep the specified number of microseconds MAX 1s

};


#endif

