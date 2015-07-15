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

#ifndef VPUList_h
#define VPUList_h 1

// parent includes:
#include <taPtrList>
#include <VPUnref>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API VPUList : public taPtrList<VPUnref> {
  // ##NO_CSS ##NO_MEMBERS a list of unresolved variable pointers that are to be resolved later..
protected:
  void		El_Done_(void* it)	{ delete (VPUnref*)it; }
  String El_GetName_(void* it) const { return ((VPUnref*)it)->name; }
public:
  void	Resolve();	// attempt to resolve references

  virtual void	AddVPU(void* b, taBase* par, const String& p, MemberDef* md = NULL);

  ~VPUList()                            { Reset(); }
};

#endif // VPUList_h
