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

#include "RecvCons_List.h"
#include <RecvCons>
#include <Projection>
#include <Layer>

void  RecvCons_List::Initialize() {
  SetBaseType(&TA_RecvCons);
}

RecvCons* RecvCons_List::NewPrjn(Projection* aprjn) {
  RecvCons* rval = (RecvCons*)New(1, aprjn->recvcons_type);
  rval->prjn = aprjn;
  rval->SetConType(aprjn->con_type); // set type of connection to this type..
  rval->SetConSpec(aprjn->con_spec.SPtr());
  return rval;
}

int RecvCons_List::FindPrjnIdx(Projection* aprjn) const {
  for(int g=0; g<size; g++) {
    RecvCons* cg = FastEl(g);
    if(cg->prjn == aprjn) {
      return g;
    }
  }
  return -1;
}

RecvCons* RecvCons_List::FindPrjn(Projection* aprjn) const {
  int idx = FindPrjnIdx(aprjn);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

int RecvCons_List::FindFromIdx(Layer* from) const {
  for(int g=0; g<size; g++) {
    RecvCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->from.ptr() == from)) {
      return g;
    }
  }
  return -1;
}

RecvCons* RecvCons_List::FindFrom(Layer* from) const {
  int idx = FindFromIdx(from);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

int RecvCons_List::FindFromNameIdx(const String& fm_nm) const {
  for(int g=0; g<size; g++) {
    RecvCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->from->name == fm_nm)) {
      return g;
    }
  }
  return -1;
}

RecvCons* RecvCons_List::FindFromName(const String& fm_nm) const {
  int idx = FindFromNameIdx(fm_nm);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}


int RecvCons_List::FindTypeFromIdx(TypeDef* prjn_td, Layer* from) const {
  for(int g=0; g<size; g++) {
    RecvCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->from.ptr() == from) &&
       (cg->prjn->InheritsFrom(prjn_td)))
    {
      return g;
    }
  }
  return -1;
}

RecvCons* RecvCons_List::FindTypeFrom(TypeDef* prjn_td, Layer* from) const {
  int idx = FindTypeFromIdx(prjn_td, from);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}


bool RecvCons_List::RemovePrjn(Projection* aprjn) {
  bool rval = false;
  int g;
  for(g=size-1; g>=0; g--) {
    RecvCons* cg = FastEl(g);
    if(cg->prjn == aprjn) {
      cg->prjn->projected = false;
      RemoveIdx(g);
      rval = true;
    }
  }
  return rval;
}

bool RecvCons_List::RemoveFrom(Layer* from) {
  bool rval = false;
  int g;
  for(g=size-1; g>=0; g--) {
    RecvCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->from.ptr() == from)) {
      cg->prjn->projected = false;
      RemoveIdx(g);
      rval = true;
    }
  }
  return rval;
}

