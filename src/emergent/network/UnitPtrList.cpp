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

#include "UnitPtrList.h"

int UnitPtrList::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) {
  if(size <= 0) return 0;
  Unit* itm = FastEl(0);
  taBase* old_par = itm->GetOwner(par_typ);
  return UpdatePointers_NewPar(old_par, new_par);
}

int UnitPtrList::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    Unit* itm = FastEl(i);
    if(!itm) continue;
    if(itm == old_ptr) {           // if it is the old guy, it is by defn a link because we're not the owner..
      if(!new_ptr)                 // if replacement is null, just remove it
        RemoveIdx(i);
      else
        ReplaceLinkIdx(i, (Unit*)new_ptr);    // it is a link to old guy; replace it!
      nchg++;
    }
  }
  return nchg;
}
