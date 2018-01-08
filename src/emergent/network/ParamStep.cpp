// Copyright 2016-2018, Regents of the University of Colorado,
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
TA_BASEFUNS_CTORS_DEFN(ParamStep_Group);

void ParamStep::Initialize() {
  epoch = 0;
  prev_epoch = 0;
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
  if(taMisc::is_loading) {
    prev_epoch = epoch;
  }
  else if(epoch != prev_epoch) {
    prev_epoch = epoch;
    ParamSeq* ps = GET_MY_OWNER(ParamSeq);
    if(!ps) return;
    ps->steps.Sort();           // re-sort!
  }
}

void ParamStep::AutoName() {
  ParamSeq* ps = GET_MY_OWNER(ParamSeq);
  if(!ps) return;
  String nm = ps->name + "_";
  if(epoch >= 0)
    nm += taMisc::LeadingZeros(epoch, 3);
  else
    nm += "off";
  SetName(nm);
}

void ParamStep::SetEpoch(int epc) {
  epoch = epc;
  prev_epoch = epc;             // prevent auto-sorting
  AutoName();
}


int ParamStep::GetEnabled() const {
  ParamSeq* ps = GET_MY_OWNER(ParamSeq);
  if(!ps) return -1;
  return ps->on;
}
