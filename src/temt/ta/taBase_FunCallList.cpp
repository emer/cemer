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

#include "taBase_FunCallList.h"
#include <taBase>
#include <taSigLink>
#include <taMisc>

void taBase_FunCallList::El_Done_(void* it_) {
  FunCallItem* it = (FunCallItem*)it_;
  it->it->RemoveDataClient(this);
  delete it;
}

bool taBase_FunCallList::AddBaseFun(taBase* obj, const String& fun_name) {
  FunCallItem* fci = new FunCallItem(obj, fun_name);
  Add(fci);
  obj->AddDataClient(this);
  return true;
}

void taBase_FunCallList::DataLinkDestroying(taSigLink* dl) {
  taBase* obj = dl->taData();
  if (!obj) return; // shouldn't happen;
  bool got_one = false;
  for(int i = size-1; i>=0; i--) {
    FunCallItem* fci = FastEl(i);
    if(fci->it == obj) {
      RemoveIdx(i);
      got_one = true;
    }
  }

  if(!got_one) {
    taMisc::Error("Internal error -- taBase_FunCallList DataDestroying_Ref didn't find base in base_funs!");
  }
}
