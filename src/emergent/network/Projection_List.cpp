// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "Projection_List.h"
#include <Network>


#include <SigLinkSignal>

TA_BASEFUNS_CTORS_DEFN(Projection_List);

void Projection_List::Initialize() {
  SetBaseType(&TA_Projection);
  send_prjns = false;
  setUseStale(true);
}

Projection* Projection_List::ConnectFrom(Layer* lay) {
  if(!lay) return NULL;
  Projection* prjn = (Projection*)New(1);
  prjn->SetCustomFrom(lay);
  return prjn;
}

Projection* Projection_List::FindPrjnFrom(Layer* lay) {
  for(int pi=0; pi < size; pi++) {
    Projection* prjn = FastEl(pi);
    if(prjn->MainIsActive() && prjn->from.ptr() == lay)
      return prjn;
  }
  return NULL;
}

Projection* Projection_List::FindPrjnTo(Layer* lay) {
  for(int pi=0; pi < size; pi++) {
    Projection* prjn = FastEl(pi);
    if(prjn->MainIsActive() && prjn->layer == lay)
      return prjn;
  }
  return NULL;
}
