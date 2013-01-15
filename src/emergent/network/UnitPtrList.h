// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef UnitPtrList_h
#define UnitPtrList_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(UnitPtrList);

class EMERGENT_API UnitPtrList: public taPtrList<Unit> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network list of unit pointers, for sending connections
public:
  int UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  int UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  int UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);

  ~UnitPtrList()             { Reset(); }
};

#endif // UnitPtrList_h
