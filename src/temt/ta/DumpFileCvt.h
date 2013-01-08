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

#ifndef DumpFileCvt_h
#define DumpFileCvt_h 1

// parent includes:

// member includes:
#include <NameVar_PArray>

// declare all other types mentioned but not required to include:


class TA_API DumpFileCvt {
  // ##NO_TOKENS #CAT_File parameters to convert a dump file
public:
  String        proj_type_base; // base name  of project (e.g., "Leabra" for "LeabraProject")
  String        key_srch_str;   // search string to identify this project type
  NameVar_PArray repl_strs; // search/replace strings (name -> value)

  DumpFileCvt(const String& prj_typ, const String& srch_st)
    { proj_type_base = prj_typ; key_srch_str = srch_st; }
};

#endif // DumpFileCvt_h
