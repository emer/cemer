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

#include "SendCons_List.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(SendCons_List);

void SendCons_List::Initialize() {
  SetBaseType(&TA_SendCons);
}

SendCons* SendCons_List::NewPrjn(Projection* aprjn) {
  SendCons* rval = (SendCons*)New(1, aprjn->sendcons_type);
  rval->prjn = aprjn;
  rval->SetConType(aprjn->con_type); // set type of connection to this type..
  rval->SetConSpec(aprjn->con_spec.SPtr());
  return rval;
}

int SendCons_List::FindPrjnIdx(Projection* aprjn) const {
  for(int g=0; g<size; g++) {
    SendCons* cg = FastEl(g);
    if(cg->prjn == aprjn) {
      return g;
    }
  }
  return -1;
}

SendCons* SendCons_List::FindPrjn(Projection* aprjn) const {
  int idx = FindPrjnIdx(aprjn);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

int SendCons_List::FindToIdx(Layer* to) const {
  for(int g=0; g<size; g++) {
    SendCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->layer == to)) {
      return g;
    }
  }
  return -1;
}

SendCons* SendCons_List::FindTo(Layer* to) const {
  int idx = FindToIdx(to);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

int SendCons_List::FindToNameIdx(const String& to_nm) const {
  for(int g=0; g<size; g++) {
    SendCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->layer->name == to_nm)) {
      return g;
    }
  }
  return -1;
}

SendCons* SendCons_List::FindToName(const String& to_nm) const {
  int idx = FindToNameIdx(to_nm);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}


int SendCons_List::FindTypeToIdx(TypeDef* prjn_td, Layer* to) const {
  for(int g=0; g<size; g++) {
    SendCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->layer == to) &&
       (cg->prjn->InheritsFrom(prjn_td)))
    {
      return g;
    }
  }
  return -1;
}

SendCons* SendCons_List::FindTypeTo(TypeDef* prjn_td, Layer* to) const {
  int idx = FindTypeToIdx(prjn_td, to);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

bool SendCons_List::RemovePrjn(Projection* aprjn) {
  bool rval = false;
  int g;
  for(g=size-1; g>=0; g--) {
    SendCons* cg = FastEl(g);
    if(cg->prjn == aprjn) {
      cg->prjn->projected = false;
      RemoveEl(cg);
      rval = true;
    }
  }
  return rval;
}

bool SendCons_List::RemoveTo(Layer* to) {
  bool rval = false;
  int g;
  for(g=size-1; g>=0; g--) {
    SendCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->layer == to)) {
      cg->prjn->projected = false;
      RemoveEl(cg);
      rval = true;
    }
  }
  return rval;
}
