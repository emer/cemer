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

#include "taSmartPtr.h"
#include <taMisc>

TypeDef* taSmartPtr::GetBaseType(TypeDef* this_typ) {
  TypeDef* targ = this_typ;
  String act_name;
  while (targ) {
    act_name = targ->name.after("taSmartPtrT_");
    if (act_name.nonempty()) {
      TypeDef* rval = taMisc::FindTypeName(act_name);
      if (rval && rval->InheritsFrom(&TA_taBase))
        return rval;
    }
    targ = targ->GetParent();
  }
  return &TA_taBase; // default
}

