// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "pdp_program.h"

#include "css_machine.h"
// #include "css_basic_types.h"
// #include "css_c_ptr_types.h"
// #include "ta_css.h"

//////////////////////////
//  Network Counters	//
//////////////////////////

void NetCounterInit::Initialize() {
  network_var = NULL;
  local_ctr_var = NULL;
}

void NetCounterInit::InitLinks() {
  inherited::InitLinks();
  taBase::Own(network_var, this);
  taBase::Own(local_ctr_var, this);
}

void NetCounterInit::CutLinks() {
  network_var.CutLinks();
  local_ctr_var.CutLinks();
  inherited::CutLinks();
}

void NetCounterInit::Destroy() {
}

void NetCounterInit::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetLocalCtrVar();
  if(counter.empty() && local_ctr_var) {
    counter = local_ctr_var->name;
  }
}

String NetCounterInit::GetDisplayName() const {
  return "Net Counter Init: " + counter;
}

void NetCounterInit::GetLocalCtrVar() {
  if(counter.empty()) return;
  if(local_ctr_var) return;
  Program* my_prog = GET_MY_OWNER(Program);
  if(!my_prog) return;
  if(!(local_ctr_var = my_prog->vars.FindName(counter))) {
    local_ctr_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    local_ctr_var->name = counter;
  }
  local_ctr_var->var_type = ProgVar::T_Int;
}

const String NetCounterInit::GenCssBody_impl(int indent_level) {
  if(!network_var) {
    taMisc::Warning("NetCounterInit: network_var = NULL -- no code generated!");
    return cssMisc::Indent(indent_level) + "// NetCounterInit: Error, network_var = NULL!\n";
  }
  if(!local_ctr_var) {
    taMisc::Warning("NetCounterInit: local_ctr_var = NULL -- no code generated!");
    return cssMisc::Indent(indent_level) + "// NetCounterInit: Error, local_ctr_var = NULL!\n";
  }
  String rval = cssMisc::Indent(indent_level) + counter + " = 0;\n";
  rval += cssMisc::Indent(indent_level) + network_var->name + "->" + counter + " = " + counter + ";\n";
  return rval;
}

//////////////////////////////////////
// incr

void NetCounterIncr::Initialize() {
  network_var = NULL;
  local_ctr_var = NULL;
}

void NetCounterIncr::InitLinks() {
  inherited::InitLinks();
  taBase::Own(network_var, this);
  taBase::Own(local_ctr_var, this);
}

void NetCounterIncr::CutLinks() {
  network_var.CutLinks();
  local_ctr_var.CutLinks();
  inherited::CutLinks();
}

void NetCounterIncr::Destroy() {
}

void NetCounterIncr::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetLocalCtrVar();
  if(counter.empty() && local_ctr_var) {
    counter = local_ctr_var->name;
  }
}

String NetCounterIncr::GetDisplayName() const {
  return "Net Counter Incr: " + counter;
}

void NetCounterIncr::GetLocalCtrVar() {
  if(counter.empty()) return;
  if(local_ctr_var) return;
  Program* my_prog = GET_MY_OWNER(Program);
  if(!my_prog) return;
  if(!(local_ctr_var = my_prog->vars.FindName(counter))) {
    local_ctr_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    local_ctr_var->name = counter;
  }
  local_ctr_var->var_type = ProgVar::T_Int;
}

const String NetCounterIncr::GenCssBody_impl(int indent_level) {
  if(!network_var) {
    taMisc::Warning("NetCounterIncr: network_var = NULL -- no code generated!");
    return cssMisc::Indent(indent_level) + "// NetCounterIncr: Error, network_var = NULL!\n";
  }
  if(!local_ctr_var) {
    taMisc::Warning("NetCounterIncr: local_ctr_var = NULL -- no code generated!");
    return cssMisc::Indent(indent_level) + "// NetCounterIncr: Error, local_ctr_var = NULL!\n";
  }
  String rval = cssMisc::Indent(indent_level) + counter + "++;\n";
  rval += cssMisc::Indent(indent_level) + network_var->name + "->" + counter + " = " + counter + ";\n";
  return rval;
}

