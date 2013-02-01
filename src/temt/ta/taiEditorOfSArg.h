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

#ifndef taiEditorOfSArg_h
#define taiEditorOfSArg_h 1

// parent includes:
#include <taiEditorOfArray>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiEditorOfSArg : public taiEditorOfArray {
  // ##NO_INSTANCE
INHERITED(taiEditorOfArray)
public:
  bool          ShowMember(MemberDef* md) const;

  taiEditorOfSArg(void* base, TypeDef* tp, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0);
  taiEditorOfSArg()    { };
protected:
  override void         Constr_AryData();
};

#endif // taiEditorOfSArg_h
