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

#ifndef taiEditOfSArg_h
#define taiEditOfSArg_h 1

// parent includes:
#include <taiEditOfArray>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taiEditOfSArg);

class TA_API taiEditOfSArg : public taiEditOfArray {
  TAI_TYPEBASE_SUBCLASS(taiEditOfSArg, taiEditOfArray);
public:
  int           BidForEdit(TypeDef* td);
protected:
  override taiEditorOfWidgetsClass* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};

#endif // taiEditOfSArg_h
