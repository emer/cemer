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
  network_type = &TA_Network;
  counter = NULL;
}

void NetCounterInit::Destroy() {
  CutLinks();
}

void NetCounterInit::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetLocalCtrVar();
  if(network_var && (network_var->object_val)) {
    network_type = network_var->object_val->GetTypeDef();
  }
}

bool NetCounterInit::CheckConfig(bool quiet) {
  if(!inherited::CheckConfig(quiet)) return false;
  if(!counter) {
    if(!quiet) taMisc::Error("Error in NetCounterInit in program:", program()->name, "counter is NULL");
    return false;
  }
  if(!network_var) {
    if(!quiet) taMisc::Error("Error in NetCounterInit in program:", program()->name, "network_var = NULL");
    return false;
  }
  if(!network_var->object_val) {
    if(!quiet) taMisc::Error("Error in NetCounterInit in program:", program()->name, "network_var object = NULL");
    return false;
  }
  if(!local_ctr_var) {
    if(!quiet) taMisc::Error("Error in NetCounterInit in program:", program()->name, "local_ctr_var = NULL");
    return false;
  }
  return true;
}

String NetCounterInit::GetDisplayName() const {
  String rval = "Net Counter Init: ";
  if(counter) rval += counter->name;
  return rval;
}

void NetCounterInit::GetLocalCtrVar() {
  if(!counter) return;
  if(local_ctr_var) return;
  Program* my_prog = program();
  if(!my_prog) return;
  if(!(local_ctr_var = my_prog->vars.FindName(counter->name))) {
    local_ctr_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    local_ctr_var->name = counter->name;
  }
  local_ctr_var->var_type = ProgVar::T_Int;
}

const String NetCounterInit::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + counter->name + " = 0;\n";
  rval += cssMisc::Indent(indent_level) + network_var->name + "->" + counter->name + " = " + counter->name + ";\n";
  return rval;
}

//////////////////////////////////////
// incr

void NetCounterIncr::Initialize() {
  network_var = NULL;
  local_ctr_var = NULL;
  network_type = &TA_Network;
  counter = NULL;
}

void NetCounterIncr::Destroy() {
  CutLinks();
}

void NetCounterIncr::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetLocalCtrVar();
  if(network_var && (network_var->object_val)) {
    network_type = network_var->object_val->GetTypeDef();
  }
}

bool NetCounterIncr::CheckConfig(bool quiet) {
  if(!inherited::CheckConfig(quiet)) return false;
  if(!counter) {
    if(!quiet) taMisc::Error("Error in NetCounterIncr in program:", program()->name, "counter is NULL");
    return false;
  }
  if(!network_var) {
    if(!quiet) taMisc::Error("Error in NetCounterIncr in program:", program()->name, "network_var = NULL");
    return false;
  }
  if(!network_var->object_val) {
    if(!quiet) taMisc::Error("Error in NetCounterIncr in program:", program()->name, "network_var object = NULL");
    return false;
  }
  if(!local_ctr_var) {
    if(!quiet) taMisc::Error("Error in NetCounterIncr in program:", program()->name, "local_ctr_var = NULL");
    return false;
  }
  return true;
}

String NetCounterIncr::GetDisplayName() const {
  String rval = "Net Counter Incr: ";
  if(counter) rval += counter->name;
  return rval;
}

void NetCounterIncr::GetLocalCtrVar() {
  if(!counter) return;
  if(local_ctr_var) return;
  Program* my_prog = program();
  if(!my_prog) return;
  if(!(local_ctr_var = my_prog->vars.FindName(counter->name))) {
    local_ctr_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    local_ctr_var->name = counter->name;
  }
  local_ctr_var->var_type = ProgVar::T_Int;
}

const String NetCounterIncr::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + counter->name + "++;\n";
  rval += cssMisc::Indent(indent_level) + network_var->name + "->" + counter->name + " = " + counter->name + ";\n";
  return rval;
}

//////////////////////////
//  Network Update	//
//////////////////////////

void NetUpdateView::Initialize() {
  network_var = NULL;
  update_var = NULL;
}

void NetUpdateView::Destroy() {
  CutLinks();
}

void NetUpdateView::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetUpdateVar();
}

bool NetUpdateView::CheckConfig(bool quiet) {
  if(!inherited::CheckConfig(quiet)) return false;
  if(!network_var) {
    if(!quiet) taMisc::Error("Error in NetUpdateView in program:", program()->name, "network_var = NULL");
    return false;
  }
  if(!update_var) {
    if(!quiet) taMisc::Error("Error in NetUpdateView in program:", program()->name, "update_var = NULL");
    return false;
  }
  return true;
}

String NetUpdateView::GetDisplayName() const {
  return "Net Update View";
}

void NetUpdateView::GetUpdateVar() {
  if(update_var) return;
  Program* my_prog = program();
  if(!my_prog) return;
  if(!(update_var = my_prog->vars.FindName("update_net_view"))) {
    update_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    update_var->name = "update_net_view";
  }
  update_var->var_type = ProgVar::T_Bool;
  update_var->UpdateAfterEdit();
}

const String NetUpdateView::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "if(update_net_view) "
    + network_var->name + "->UpdateAllViews();\n";
  return rval;
}

