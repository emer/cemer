// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef taProjVersion_h
#define taProjVersion_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taProjVersion);

class TA_API taProjVersion : public taOBase {
  // #EDIT_INLINE project version numbering information
INHERITED(taOBase)
public:
  int   major;                  // use for incompatible or other major changes
  int   minor;                  // use for compatible or minor incremental changes
  int   step;                   // use for incremental steps -- automatically incremented during SaveNoteChanges

  void          Set(int mj, int mn, int st = 0) { major = mj; minor = mn; step = st; }
  void          SetFromString(String ver); // parse, mj.mn.st
  const String  GetString()
  { return String(major).cat(".").cat(String(minor)).cat(".").cat(String(step)).cat(".");}

  void          Clear() {major = minor = step = 0;} // reset version info to 0
  bool          GtEq(int mj, int mn, int st = 0); // true if the version is greater than or equal to the indicated version

  TA_SIMPLE_BASEFUNS(taProjVersion);
private:
  void  Initialize() { Clear(); }
  void  Destroy()    { };
};

#endif // taProjVersion_h
