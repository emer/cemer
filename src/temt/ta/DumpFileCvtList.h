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

#ifndef DumpFileCvtList_h
#define DumpFileCvtList_h 1

// parent includes:
#include <DumpFileCvt>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(DumpFileCvtList);

class TA_API DumpFileCvtList : public taPtrList<DumpFileCvt> {
  // #CAT_File list of available dump file converters
protected:
  void          El_Done_(void* it)      { delete (DumpFileCvt*)it; }
  String El_GetName_(void* it) const { return ((DumpFileCvt*)it)->proj_type_base; }
public:
  ~DumpFileCvtList()                            { Reset(); }
};

#endif // DumpFileCvtList_h
