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

#ifndef SArg_Array_h
#define SArg_Array_h 1

// parent includes:
#include <String_Array>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API SArg_Array : public String_Array {
  // ##CAT_Program string argument array: has labels for each argument to make it easier in the interface
INHERITED(String_Array)
public:
  String_Array  labels;         // #HIDDEN labels for each argument

  // note: all following key-based api routines are case sensitive
  bool          HasValue(const String& key) const;
    // returns true if there is an entry for the key
  String        GetValue(const String& key) const;
    // return the value for the key, or nil if none
  void          SetValue(const String& key, const String& value);
    // set or update the value for the key

#ifndef __MAKETA__
  int           Dump_Save_Value(std::ostream& strm, taBase* par=NULL, int indent = 0);
  int           Dump_Load_Value(std::istream& strm, taBase* par=NULL);
#endif

  void  UpdateAfterEdit();
  void  InitLinks();
  TA_BASEFUNS(SArg_Array);
private:
  void  Copy_(const SArg_Array& cp);
  void  Initialize();
  void  Destroy()       { };
};

#endif // SArg_Array_h
