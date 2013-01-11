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

#include "taiMimeFactory_List.h"

taiMimeFactory_List* taiMimeFactory_List::g_instance; 

void taiMimeFactory_List::setInstance(taiMimeFactory_List* value)
{
  g_instance = value;
}

taiMimeFactory* taiMimeFactory_List::StatGetInstanceByType(TypeDef* td) {
  taiMimeFactory_List* inst = instance();
  if (inst) return inst->GetInstanceByType(td);
  else return NULL;
}

void taiMimeFactory_List::Destroy() {
  if (g_instance == this) g_instance = NULL;
}

taiMimeFactory* taiMimeFactory_List::GetInstanceByType(TypeDef* td) {
  if (!td) return NULL;
  if (!td->DerivesFrom(&TA_taiMimeFactory)) return NULL;
  taiMimeFactory* rval = NULL;
  for (int i = 0; i < size; ++i) {
    rval = FastEl(i);
    //NOTE: if comparing of objects fails for some reason, compare names
    if (rval->GetTypeDef()->GetNonPtrType() == td)
      return rval;
  }
  // not here yet, so make one
  rval = (taiMimeFactory*)New(1, td);
  return rval;
}

