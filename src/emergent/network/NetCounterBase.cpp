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

#include "NetCounterBase.h"
#include <MemberDef>

#include <taMisc>

TypeDef_Of(Network);


void NetCounterBase::Initialize() {
  network_type = &TA_Network;
  counter = NULL;
  update_after = false;
}

void NetCounterBase::Destroy() {
  CutLinks();
}

void NetCounterBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if((bool)network_var && ((bool)network_var->object_val)) {
    network_type = network_var->object_val->GetTypeDef();
  }
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetLocalCtrVar();
}

void NetCounterBase::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!counter, quiet, rval, "counter is NULL");
  CheckError(!(bool)network_var, quiet, rval, "network_var = NULL");
  CheckError((bool)network_var && !network_var->object_val, quiet, rval,
             "network_var object = NULL");
  CheckError(!local_ctr_var, quiet, rval, "local_ctr_var = NULL");
}

void NetCounterBase::GetLocalCtrVar() {
  if (!counter) return;
  if (local_ctr_var && (local_ctr_var->name == counter->name)) return;

  Program* my_prog = program();
  if (!my_prog) return;

  local_ctr_var = my_prog->vars.FindName(counter->name);
  if (!local_ctr_var) {
    local_ctr_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    local_ctr_var->name = counter->name;
    local_ctr_var->SigEmitUpdated();
  }
  local_ctr_var->var_type = ProgVar::T_Int;
}
