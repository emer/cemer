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

#ifndef taVersion_h
#define taVersion_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taVersion {
  // #EDIT_INLINE simple value class for version info
public:
  ushort        major;
  ushort        minor;
  ushort        step;
  ushort        build;

  void          set(ushort mj, ushort mn, ushort st = 0, ushort bld = 0)
    {major = mj; minor = mn; step = st; build = bld;}
  void          setFromString(String ver); // parse, mj.mn.st-build
  const String  toString()
    {return String(major).cat(".").cat(String(minor)).cat(".").
       cat(String(step)).cat(".").cat(String(build));}

  void          Clear() {major = minor = step = build = 0;} //
  bool          GtEq(ushort mj, ushort mn, ushort st = 0) {
    return (major > mj) || ((major == mj) && (minor > mn)) ||
      ((major == mj) && (minor == mn) && (step >= st));
  }
  // true if the version is greater than or equal to the indicated version

  bool          operator>=(taVersion& cmp)
  { return GtEq(cmp.major, cmp.minor, cmp.step); }
  bool          operator==(taVersion& cmp)
  { return (major == cmp.major && minor == cmp.minor && step == cmp.step); }
  bool          operator!=(taVersion& cmp)
  { return (major != cmp.major || minor != cmp.minor || step == cmp.step); }
  bool          operator>(taVersion& cmp)
  { return (major > cmp.major) || ((major == cmp.major) && (minor > cmp.minor)) ||
      ((major == cmp.major) && (minor == cmp.minor) && (step > cmp.step)); }
  bool          operator<(taVersion& cmp)
  { return !GtEq(cmp.major, cmp.minor, cmp.step); }

  taVersion() {Clear();} //
  taVersion(ushort mj, ushort mn, ushort st = 0, ushort bld = 0)
    {set(mj, mn, st, bld);} //
  taVersion(const String& ver) {setFromString(ver);}
  // implicit copy and assign

  static int    BeforeOrOf(char sep, String& in);
  // #IGNORE for parsing
};

#endif // taVersion_h
