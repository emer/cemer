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

#ifndef DumpPathSubList_h
#define DumpPathSubList_h 1

// parent includes:
#include <taPtrList>
#include <DumpPathSub>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API DumpPathSubList : public taPtrList<DumpPathSub> {
  // ##NO_CSS ##NO_MEMBERS
INHERITED(taPtrList<DumpPathSub>)
protected:
  void		El_Done_(void* it)	{ delete (DumpPathSub*)it; }
  String	El_GetName_(void* it) const { return ((DumpPathSub*)it)->old_path; }
public:
  void	AddPath(TypeDef* td, taBase* par, String& o, String& n);
  void 	FixPath(TypeDef* td, taBase* par, String& path);
  // fixes given path (td and par are for scoping search)
  void 	unFixPath(TypeDef* td, taBase* par, String& path);
  // un-fixes given path to original path value

  ~DumpPathSubList()            { Reset(); }
};

#endif // DumpPathSubList_h
