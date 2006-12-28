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
#include "netstru.h"
#include "css_machine.h"
#include "ta_datatable.h"

// #include "css_basic_types.h"
// #include "css_c_ptr_types.h"
// #include "css_ta.h"

//////////////////////////
//  BasicDataLoop	//
//////////////////////////

void BasicDataLoop::Initialize() {
  order = SEQUENTIAL;
  loop_test = "This is not used here!";
}

void BasicDataLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if (!data_var) {
    if(!quiet) taMisc::CheckError("Error in BasicDataLoop in program:", program()->name, "data_var = NULL");
    rval = false;
  }
  if(!data_var->object_val || !data_var->object_val.ptr()->InheritsFrom(&TA_DataTable)) {
    if(!quiet) taMisc::CheckError("Error in BasicDataLoop in program:", program()->name, "data_var does not point to a data table");
    rval = false;
  }
}

const String BasicDataLoop::GenCssPre_impl(int indent_level) {
  String id1 = cssMisc::Indent(indent_level+1);
  String id2 = cssMisc::Indent(indent_level+2);
  String id3 = cssMisc::Indent(indent_level+3);
  String data_nm = data_var->name;

  String rval = cssMisc::Indent(indent_level) + "{ // BasicDataLoop " + data_nm + "\n";
  rval += id1 + "BasicDataLoop* data_loop = this" + GetPath(NULL,program()) + ";\n";
  rval += id1 + "data_loop->DMem_Initialize(network); // note: this assumes network variable exists!\n";
  rval += id1 + "data_loop->item_idx_list.SetSize(" + data_nm + "->ItemCount());\n";
  rval += id1 + "data_loop->item_idx_list.FillSeq();\n";
  rval += id1 + "if(data_loop->order == BasicDataLoop::PERMUTED) data_loop->item_idx_list.Permute();\n";
  rval += id1 + data_nm + "->ReadOpen();\n";
  rval += id1 + "int st_idx = data_loop->dmem_this_proc; // start with different items in dmem (0 if not)\n";
  rval += id1 + "int inc_idx = data_loop->dmem_nprocs;  // this is 1 if no dmem\n";
  rval += id1 + "int mx_idx = data_loop->item_idx_list.size;\n";
  rval += id1 + "bool dmem_even = true; // is the total number an even multiple of dmem_nprocs?\n";
  rval += id1 + "if(mx_idx % inc_idx != 0) {\n";
  rval += id2 + "dmem_even = false;  mx_idx = ((mx_idx / inc_idx) + 1) * inc_idx; // round up\n";
  rval += id1 + "}\n";
  rval += id1 + "for(int list_idx = st_idx; list_idx < mx_idx; list_idx += inc_idx) {\n";
  rval += id2 + "int data_idx;\n";
  rval += id2 + "if(data_loop->order == BasicDataLoop::RANDOM) data_idx = Random::IntZeroN(data_loop->item_idx_list.size);\n";
  rval += id2 + "else {\n";
  rval += id3 + "if(list_idx < data_loop->item_idx_list.size) data_idx = data_loop->item_idx_list[list_idx];\n";
  rval += id3 + "else data_idx = Random::IntZeroN(data_loop->item_idx_list.size); // draw at random from list if over max -- need to process something for dmem to stay in sync\n";
  rval += id3 + "}\n";
  rval += id2 + "if(!" + data_nm + "->ReadItem(data_idx)) break;\n";
  return rval;
}

const String BasicDataLoop::GenCssBody_impl(int indent_level) {
  return loop_code.GenCss(indent_level + 2);
}

const String BasicDataLoop::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level+1) + "} // for loop\n";
  rval += cssMisc::Indent(indent_level) + "} // BasicDataLoop " + data_var->name + "\n";
  return rval;
}

void BasicDataLoop::DMem_Initialize(Network* net) {
#ifdef DMEM_COMPILE
  dmem_nprocs = net->dmem_trl_comm.nprocs;
  dmem_this_proc = net->dmem_trl_comm.this_proc;
#else
  dmem_nprocs = 1;
  dmem_this_proc = 0;
#endif
}

String BasicDataLoop::GetDisplayName() const {
  String ord_str = GetTypeDef()->GetEnumString("Order", order);
  String data_nm;
  if(data_var) data_nm = data_var->name;
  return "data table loop (" + ord_str + " over: " + data_nm + ")";
}


//////////////////////////
//  GroupedDataLoop	//
//////////////////////////

void GroupedDataLoop::Initialize() {
  group_order = PERMUTED;
  item_order = SEQUENTIAL;
  group_col = 0;
  loop_test = "This is not used here!";
}

void GroupedDataLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!data_var) {
    if(!quiet) taMisc::CheckError("Error in GroupedDataLoop in program:", program()->name, "data_var = NULL");
    rval = false;
  }
  if(!data_var->object_val || !data_var->object_val.ptr()->InheritsFrom(&TA_DataTable)) {
    if(!quiet) taMisc::CheckError("Error in GroupedDataLoop in program:", program()->name, "data_var does not point to a data table");
    rval = false;
  }
}

void GroupedDataLoop::GetGroupList() {
  DataTable* dt = (DataTable*)data_var->object_val.ptr();
  group_idx_list.Reset();
  String prv_gp_nm;
  for(int i=0;i<dt->ItemCount();i++) {
    String gp_nm = dt->GetValAsString(group_col, i);
    if(gp_nm != prv_gp_nm) {
      group_idx_list.Add(i);
      prv_gp_nm = gp_nm;
    }
  }
}

void GroupedDataLoop::GetItemList(int group_idx) {
  DataTable* dt = (DataTable*)data_var->object_val.ptr();
  item_idx_list.Reset();
  String prv_gp_nm = dt->GetValAsString(group_col, group_idx);
  for(int i=group_idx;i<dt->ItemCount();i++) {
    String gp_nm = dt->GetValAsString(group_col, i);
    if(gp_nm != prv_gp_nm)
      break;
    item_idx_list.Add(i);
  }
}

const String GroupedDataLoop::GenCssPre_impl(int indent_level) {
  String id1 = cssMisc::Indent(indent_level+1);
  String id2 = cssMisc::Indent(indent_level+2);
  String id3 = cssMisc::Indent(indent_level+3);
  String data_nm = data_var->name;

  String rval = cssMisc::Indent(indent_level) + "{ // GroupedDataLoop " + data_nm + "\n";
  rval += id1 + "GroupedDataLoop* data_loop = this" + GetPath(NULL,program()) + ";\n";
  rval += id1 + "data_loop->GetGroupList();\n";
  rval += id1 + "if(data_loop->group_order == GroupedDataLoop::PERMUTED) data_loop->group_idx_list.Permute();\n";
  rval += id1 + data_nm + "->ReadOpen();\n";
  rval += id1 + "for(int group_list_idx = 0; group_list_idx < data_loop->group_idx_list.size; group_list_idx++) {\n";
  rval += id2 + "int group_data_idx;\n";
  rval += id2 + "if(data_loop->group_order == GroupedDataLoop::RANDOM) \n";
  rval += id2 + "   group_data_idx = data_loop->group_idx_list[Random::IntZeroN(data_loop->group_idx_list.size)];\n";
  rval += id2 + "else group_data_idx = data_loop->group_idx_list[group_list_idx];\n";
  rval += id2 + "data_loop->GetItemList(group_data_idx);\n";
  rval += id2 + "if(data_loop->item_order == GroupedDataLoop::PERMUTED) data_loop->item_idx_list.Permute();\n";
  rval += id2 + "for(int item_list_idx = 0; item_list_idx < data_loop->item_idx_list.size; item_list_idx++) {\n";
  rval += id3 + "int item_data_idx;\n";
  rval += id3 + "if(data_loop->item_order == GroupedDataLoop::RANDOM) \n";
  rval += id3 + "   item_data_idx = data_loop->item_idx_list[Random::IntZeroN(data_loop->item_idx_list.size)];\n";
  rval += id3 + "else item_data_idx = data_loop->item_idx_list[item_list_idx];\n";
  rval += id3 + "if(!" + data_nm + "->ReadItem(item_data_idx)) break;\n";
  return rval;
}

const String GroupedDataLoop::GenCssBody_impl(int indent_level) {
  return loop_code.GenCss(indent_level + 3);
}

const String GroupedDataLoop::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level+2) + "} // item for loop\n";
  rval += cssMisc::Indent(indent_level+1) + "} // group for loop\n";
  rval += cssMisc::Indent(indent_level) + "} // GroupedDataLoop " + data_var->name + "\n";
  return rval;
}

String GroupedDataLoop::GetDisplayName() const {
  String group_ord_str = GetTypeDef()->GetEnumString("Order", group_order);
  String item_ord_str = GetTypeDef()->GetEnumString("Order", item_order);
  String data_nm;
  if(data_var) data_nm = data_var->name;
  return "grouped table loop (gp: " + group_ord_str + "\n itm: " + item_ord_str
    + " over: " + data_nm + ")";
}

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

void NetCounterInit::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetLocalCtrVar();
  if(network_var && (network_var->object_val)) {
    network_type = network_var->object_val->GetTypeDef();
  }
}

void NetCounterInit::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!counter) {
    if(!quiet) taMisc::CheckError("Error in NetCounterInit in program:", program()->name, "counter is NULL");
    rval = false;
  }
  if(!network_var) {
    if(!quiet) taMisc::CheckError("Error in NetCounterInit in program:", program()->name, "network_var = NULL");
    rval = false;
  }
  if(network_var && !network_var->object_val) {
    if(!quiet) taMisc::CheckError("Error in NetCounterInit in program:", program()->name, "network_var object = NULL");
    rval = false;
  }
  if(!local_ctr_var) {
    if(!quiet) taMisc::CheckError("Error in NetCounterInit in program:", program()->name, "local_ctr_var = NULL");
    rval = false;
  }
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

void NetCounterIncr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetLocalCtrVar();
  if(network_var && (network_var->object_val)) {
    network_type = network_var->object_val->GetTypeDef();
  }
}

void NetCounterIncr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!counter) {
    if(!quiet) taMisc::CheckError("Error in NetCounterIncr in program:", program()->name, "counter is NULL");
    rval = false;
  }
  if(!network_var) {
    if(!quiet) taMisc::CheckError("Error in NetCounterIncr in program:", program()->name, "network_var = NULL");
    rval = false;
  }
  if(network_var && !network_var->object_val) {
    if(!quiet) taMisc::CheckError("Error in NetCounterIncr in program:", program()->name, "network_var object = NULL");
    rval = false;
  }
  if(!local_ctr_var) {
    if(!quiet) taMisc::CheckError("Error in NetCounterIncr in program:", program()->name, "local_ctr_var = NULL");
    rval = false;
  }
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

void NetUpdateView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetUpdateVar();
}

void NetUpdateView::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!network_var) {
    if(!quiet) taMisc::CheckError("Error in NetUpdateView in program:", program()->name, "network_var = NULL");
    rval = false;
  }
  if(!update_var) {
    if(!quiet) taMisc::CheckError("Error in NetUpdateView in program:", program()->name, "update_var = NULL");
    rval = false;
  }
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

