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

#include "ParamStep.h"
#include <ParamSeq>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ParamStep);
TA_BASEFUNS_CTORS_DEFN(ParamStep_List);

void ParamStep::Initialize() {
  epoch = 0;
}

void ParamStep::InitLinks() {
  inherited::InitLinks();
  if(!taMisc::is_loading) {
    ParamSeq* ps = GET_MY_OWNER(ParamSeq);
    if(ps) {
      String nm = ps->name + "_new";
      SetName(nm);
    }
  }
}

void ParamStep::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  AutoName();
}

void ParamStep::AutoName() {
  ParamSeq* ps = GET_MY_OWNER(ParamSeq);
  if(!ps) return;
  String nm = ps->name + "_" + taMisc::LeadingZeros(epoch, 3);
  SetName(nm);
}

int ParamStep::GetEnabled() const {
  ParamSeq* ps = GET_MY_OWNER(ParamSeq);
  if(!ps) return -1;
  return ps->on;
}
