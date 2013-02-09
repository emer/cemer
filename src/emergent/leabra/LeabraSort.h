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

#ifndef LeabraSort_h
#define LeabraSort_h 1

// parent includes:
#include <LeabraUnit>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraSort);

class E_API LeabraSort : public taPtrList<LeabraUnit> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Leabra used for sorting units in kwta computation
INHERITED(taPtrList<LeabraUnit>)
protected:
  int		El_Compare_(void* a, void* b) const
  { int rval=-1; if(((LeabraUnit*)a)->net < ((LeabraUnit*)b)->net) rval=1;
    else if(((LeabraUnit*)a)->net == ((LeabraUnit*)b)->net) rval=0; return rval; }
  // compare two items for purposes of sorting: descending order by net
public:
};

#endif // LeabraSort_h
