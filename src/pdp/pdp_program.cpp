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
#include "ta_project.h"

// #include "css_basic_types.h"
// #include "css_c_ptr_types.h"
// #include "css_ta.h"

//////////////////////////
//  BasicDataLoop	//
//////////////////////////

void BasicDataLoop::Initialize() {
  order = SEQUENTIAL;
}

void BasicDataLoop::GetOrderVar() {
  Program* my_prog = program();
  if(!my_prog) return;
  if(!order_var || order_var->name != "data_loop_order") {
    if(!(order_var = my_prog->vars.FindName("data_loop_order"))) {
      order_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      order_var->name = "data_loop_order";
    }
  }
  order_var->var_type = ProgVar::T_HardEnum;
  order_var->hard_enum_type = TA_BasicDataLoop.sub_types.FindName("Order");
  order = (Order)order_var->int_val;
}

void BasicDataLoop::GetOrderVal() {
  if(!order_var) GetOrderVar();
  else order = (Order)order_var->int_val;
}

void BasicDataLoop::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(data_var);
  UpdateProgVarRef_NewOwner(order_var);
  GetOrderVar();
}

void BasicDataLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!data_var, quiet, rval,  "data_var = NULL");
  CheckError(!order_var, quiet, rval, "order_var = NULL");
  CheckError(!data_var->object_val || !data_var->object_val.ptr()->InheritsFrom(&TA_DataTable), quiet, rval,"data_var does not point to a data table");
  CheckProgVarRef(data_var, quiet, rval);
  CheckProgVarRef(order_var, quiet, rval);
}

const String BasicDataLoop::GenCssPre_impl(int indent_level) {
  String id1 = cssMisc::Indent(indent_level+1);
  String id2 = cssMisc::Indent(indent_level+2);
  String id3 = cssMisc::Indent(indent_level+3);
  String data_nm = data_var->name;

  String rval = cssMisc::Indent(indent_level) + "{ // BasicDataLoop " + data_nm + "\n";
  rval += id1 + "BasicDataLoop* data_loop = this" + GetPath(NULL,program()) + ";\n";
  rval += id1 + "data_loop->GetOrderVal(); // order_var variable controls order -- make sure we have current value\n";
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
}

void GroupedDataLoop::GetOrderVals() {
  if(!group_order_var || !item_order_var) GetOrderVars();
  else {
    group_order = (Order)group_order_var->int_val;
    item_order = (Order)item_order_var->int_val;
  }
}

void GroupedDataLoop::GetOrderVars() {
  Program* my_prog = program();
  if(!my_prog) return;
  if(!group_order_var || group_order_var->name != "group_order") {
    if(!(group_order_var = my_prog->vars.FindName("group_order"))) {
      group_order_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      group_order_var->name = "group_order";
    }
  }
  group_order_var->var_type = ProgVar::T_HardEnum;
  group_order_var->hard_enum_type = TA_GroupedDataLoop.sub_types.FindName("Order");
  group_order = (Order)group_order_var->int_val;

  if(!item_order_var || item_order_var->name != "item_order") {
    if(!(item_order_var = my_prog->vars.FindName("item_order"))) {
      item_order_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      item_order_var->name = "item_order";
    }
  }
  item_order_var->var_type = ProgVar::T_HardEnum;
  item_order_var->hard_enum_type = TA_GroupedDataLoop.sub_types.FindName("Order");
  item_order = (Order)item_order_var->int_val;
}

void GroupedDataLoop::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(data_var);
  UpdateProgVarRef_NewOwner(group_order_var);
  UpdateProgVarRef_NewOwner(item_order_var);
  GetOrderVars();
}

void GroupedDataLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!data_var, quiet, rval,  "data_var = NULL");
  CheckError(!data_var->object_val || !data_var->object_val.ptr()->InheritsFrom(&TA_DataTable), quiet, rval,"data_var does not point to a data table");
  CheckError(!group_order_var, quiet, rval,  "group_order_var = NULL");
  CheckError(!item_order_var, quiet, rval,  "item_order_var = NULL");
  CheckProgVarRef(data_var, quiet, rval);
  CheckProgVarRef(group_order_var, quiet, rval);
  CheckProgVarRef(item_order_var, quiet, rval);
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
  rval += id1 + "data_loop->GetOrderVals(); // order_var variables control order -- make sure we have current values\n";
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
  network_type = &TA_Network;
  counter = NULL;
}

void NetCounterInit::Destroy() {
  CutLinks();
}

void NetCounterInit::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(network_var);
  UpdateProgVarRef_NewOwner(local_ctr_var);
  GetLocalCtrVar();
  if((bool)network_var && ((bool)network_var->object_val)) {
    network_type = network_var->object_val->GetTypeDef();
  }
}

void NetCounterInit::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!counter, quiet, rval, "counter is NULL");
  CheckError(!(bool)network_var, quiet, rval, "network_var = NULL");
  CheckError((bool)network_var && !network_var->object_val, quiet, rval,
	     "network_var object = NULL");
  CheckError(!local_ctr_var, quiet, rval, "local_ctr_var = NULL");
  CheckProgVarRef(network_var, quiet, rval);
  CheckProgVarRef(local_ctr_var, quiet, rval);
}

String NetCounterInit::GetDisplayName() const {
  String rval = "Net Counter Init: ";
  if(counter) rval += counter->name;
  return rval;
}

void NetCounterInit::GetLocalCtrVar() {
  if(!counter) return;
  if((bool)local_ctr_var && (local_ctr_var->name == counter->name)) return;
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
  network_type = &TA_Network;
  counter = NULL;
}

void NetCounterIncr::Destroy() {
  CutLinks();
}

void NetCounterIncr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(network_var);
  UpdateProgVarRef_NewOwner(local_ctr_var);
  GetLocalCtrVar();
  if((bool)network_var && ((bool)network_var->object_val)) {
    network_type = network_var->object_val->GetTypeDef();
  }
}

void NetCounterIncr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!counter, quiet, rval, "counter is NULL");
  CheckError(!network_var, quiet, rval, "network_var = NULL");
  CheckError((bool)network_var && !network_var->object_val, quiet, rval,
	     "network_var object = NULL");
  CheckError(!local_ctr_var, quiet, rval, "local_ctr_var = NULL");
  CheckProgVarRef(network_var, quiet, rval);
  CheckProgVarRef(local_ctr_var, quiet, rval);
}

String NetCounterIncr::GetDisplayName() const {
  String rval = "Net Counter Incr: ";
  if(counter) rval += counter->name;
  return rval;
}

void NetCounterIncr::GetLocalCtrVar() {
  if(!counter) return;
  if((bool)local_ctr_var && (local_ctr_var->name == counter->name)) return;
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
}

void NetUpdateView::Destroy() {
  CutLinks();
}

void NetUpdateView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(network_var);
  UpdateProgVarRef_NewOwner(update_var);
  GetUpdateVar();
}

void NetUpdateView::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!network_var, quiet, rval, "network_var = NULL");
  CheckError(!update_var, quiet, rval, "update_var = NULL");
  CheckProgVarRef(network_var, quiet, rval);
  CheckProgVarRef(update_var, quiet, rval);
}

String NetUpdateView::GetDisplayName() const {
  return "Net Update View";
}

void NetUpdateView::GetUpdateVar() {
  if((bool)update_var && (update_var->name == "update_net_view")) return;
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


////////////////////////////////////////////////////
//		Named Units Framework
////////////////////////////////////////////////////

void InitNamedUnits::Initialize() {
  n_lay_name_chars = 1;
}

void InitNamedUnits::Destroy() {
  CutLinks();
}

void InitNamedUnits::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(input_data_var);
  UpdateProgVarRef_NewOwner(unit_names_var);
  UpdateProgVarRef_NewOwner(network_var);

  GetInputDataVar();
  GetUnitNamesVar();
  GetNetworkVar();
  if(!taMisc::is_loading)
    InitNamesTable();
}

// this is really all it does -- no actual code gen!!
void InitNamedUnits::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!input_data_var, quiet, rval, "input_data = NULL");
  CheckError(!unit_names_var, quiet, rval, "unit_names = NULL");
  CheckProgVarRef(input_data_var, quiet, rval);
  CheckProgVarRef(unit_names_var, quiet, rval);
  CheckProgVarRef(network_var, quiet, rval);
}

String InitNamedUnits::GetDisplayName() const {
  return "Init Named Units";
}

bool InitNamedUnits::GetInputDataVar() {
  if(input_data_var) return true;
  Program* my_prog = program();
  if(!my_prog) return false;
  input_data_var = my_prog->args.FindVarType(ProgVar::T_Object, &TA_DataTable);
  if(input_data_var) return true;
  input_data_var = my_prog->vars.FindVarType(ProgVar::T_Object, &TA_DataTable);
  if(!input_data_var) {
    input_data_var = (ProgVar*)my_prog->args.New(1, &TA_ProgVar); // make an arg by default
    input_data_var->name = "input_data";
    input_data_var->var_type = ProgVar::T_Object;
  }
  return true;
}

bool InitNamedUnits::GetUnitNamesVar() {
  Program* my_prog = program();
  if(!my_prog) return false;
  if(!unit_names_var || (unit_names_var->name != "unit_names")) {
    if(!(unit_names_var = my_prog->vars.FindName("unit_names"))) {
      unit_names_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      unit_names_var->name = "unit_names";
    }
  }
  unit_names_var->var_type = ProgVar::T_Object;
  //  unit_names_var->UpdateAfterEdit();

  if(!unit_names_var->object_val) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(!proj) return false;
    DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("InputData");
    DataTable* rval = dgp->NewEl(1, &TA_DataTable);
    rval->name = "UnitNames";
    taMisc::Info("Note: created new data table named:", rval->name, "in .data.InputData");
    unit_names_var->object_val = rval;
  }
  return true;
}

bool InitNamedUnits::GetNetworkVar() {
  if((bool)network_var) return true;
  Program* my_prog = program();
  if(!my_prog) return false;
  network_var = my_prog->vars.FindName("network");
  if(!network_var) return false;
  network_var->var_type = ProgVar::T_Object;
  //  network_var->UpdateAfterEdit();
  return (bool)network_var->object_val;
}

const String InitNamedUnits::GenCssBody_impl(int indent_level) {
  return "";
}

bool InitNamedUnits::InitNamesTable() {
  if(TestError(!GetInputDataVar(), "InitNamesTable", "could not find input_data table -- must set that up to point to your input data table, and give name input_data"))
    return false;
  if(TestError(!GetUnitNamesVar(), "InitNamesTable", "could not find unit names data table -- this should not usually happen because it is auto-made if not found"))
    return false;

  bool rval = InitUnitNamesFmInputData((DataTable*)unit_names_var->object_val.ptr(),
				       (DataTable*)input_data_var->object_val.ptr());
  return rval;
}

bool InitNamedUnits::InitUnitNamesFmInputData(DataTable* unit_names,
					      const DataTable* input_data) {
  if(!unit_names || !input_data) {
    taMisc::Error("InitUnitNamesFmInputData", "null args");
    return false;
  }
  for(int i=0;i<input_data->cols();i++) {
    DataCol* idc = input_data->data.FastEl(i);
    if(!idc->isNumeric() || !idc->isMatrix()) continue;
    DataCol* ndc;
    if(idc->cell_dims() == 4)
      ndc = unit_names->FindMakeColMatrix(idc->name, VT_STRING,
					  idc->cell_dims(),
					  idc->GetCellGeom(0), idc->GetCellGeom(1), 
					  idc->GetCellGeom(2), idc->GetCellGeom(3));
    else
      ndc = unit_names->FindMakeColMatrix(idc->name, VT_STRING,
					  idc->cell_dims(),
					  idc->GetCellGeom(0), idc->GetCellGeom(1));
  }
  if(unit_names->rows <= 0)
    unit_names->AddBlankRow();		// only one row ever needed!
  return true;
}

bool InitNamedUnits::InitDynEnums() {
  Program* my_prog = program();
  if(!my_prog) return false;
  if(TestError(!GetUnitNamesVar(), "InitNamesTable", "could not find unit names data table -- this should not usually happen because it is auto-made if not found"))
    return false;

  DataTable* undt = (DataTable*)unit_names_var->object_val.ptr();
  if(!undt) return false;	// should not happen

  for(int i=0;i<undt->cols();i++) {
    DataCol* ndc = undt->data.FastEl(i);
    DynEnumType* det = (DynEnumType*)my_prog->types.FindName(ndc->name);
    if(!det) {
      det = my_prog->types.NewDynEnum();
      det->name = ndc->name;
    }
    String prefix = ndc->name.before(n_lay_name_chars);
    InitDynEnumFmUnitNames(det, ndc, prefix);
  }
  return true;
}

bool InitNamedUnits::InitDynEnumFmUnitNames(DynEnumType* dyn_enum,
					    const DataCol* unit_names_col,
					    const String& prefix) {
  if(!dyn_enum || !unit_names_col) {
    taMisc::Error("InitDynEnumFmUnitNames", "null args");
    return false;
  }
  dyn_enum->enums.Reset();
  for(int i=0;i<unit_names_col->cell_size();i++) {
    String cnm = unit_names_col->GetValAsStringM(-1, i);
    if(cnm.empty()) continue;
    cnm = prefix + "_" + cnm;
    dyn_enum->AddEnum(cnm, i);
  }
  return true;
}

bool InitNamedUnits::LabelNetwork() {
  // todo: write this
  return false;
}

// TODO: check for overlap and append first letter of layer/col name to disambig
// do UnitNameLit and Var -- should be easy!

// add offset for set -- for groups

// do for 12ax -- better test case

//////////////////////////
//  Set Units Lit
//////////////////////////

void SetUnitsLit::Initialize() {
  set_nm = true;
}

void SetUnitsLit::Destroy() {
  CutLinks();
}

void SetUnitsLit::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(input_data_var);
  GetInputDataVar();
}

void SetUnitsLit::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!input_data_var, quiet, rval, "input_data = NULL");
  CheckProgVarRef(input_data_var, quiet, rval);
}

String SetUnitsLit::GetDisplayName() const {
  String rval = "Set Units: ";
  if(unit_1.IsSet()) rval += unit_1.NameVal() + " ";
  if(unit_2.IsSet()) rval += unit_2.NameVal() + " ";
  if(unit_3.IsSet()) rval += unit_3.NameVal() + " ";
  if(unit_4.IsSet()) rval += unit_4.NameVal();
  return rval;
}

bool SetUnitsLit::GetInputDataVar() {
  if(input_data_var) return true;
  Program* my_prog = program();
  if(!my_prog) return false;
  input_data_var = my_prog->args.FindVarType(ProgVar::T_Object, &TA_DataTable);
  if(input_data_var) return true;
  input_data_var = my_prog->vars.FindVarType(ProgVar::T_Object, &TA_DataTable);
  if(!input_data_var) {
    input_data_var = (ProgVar*)my_prog->args.New(1, &TA_ProgVar); // make an arg by default
    input_data_var->name = "input_data";
    input_data_var->var_type = ProgVar::T_Object;
  }
  return true;
}

bool SetUnitsLit::GenCss_OneUnit(String& rval, DynEnum& un, const String& idnm, 
				 DataTable* idat, const String& il) {
  int colno;
  if(un.IsSet()) {
    if(idat->FindColName(un.enum_type->name, colno, true)) {
      rval += il + idnm + ".SetValAsFloatM(1.0, " + String(colno) + ", -1, "
	+ un.NameVal() + ");\n";
      if(set_nm) {
	if(idat->FindColName("Name", colno, true)) {
	  rval += il + "{ String nm = " + idnm + ".GetValAsString(" + String(colno)
	    + ", -1); nm += \"_" + un.NameVal() + "\"; "
	    + idnm + ".SetValAsString(nm, " + String(colno) + ", -1); }\n";
	}
      }
      return true;
    }
  }
  return false;
}

const String SetUnitsLit::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  if(!input_data_var) return il + "// input_data_var not set!\n";
  String idnm = input_data_var->name;
  DataTable* idat = (DataTable*)input_data_var->object_val.ptr();
  if(!idat) return il + "// input_data not set!\n";
  String rval;
  GenCss_OneUnit(rval, unit_1, idnm, idat, il);
  GenCss_OneUnit(rval, unit_2, idnm, idat, il);
  GenCss_OneUnit(rval, unit_3, idnm, idat, il);
  GenCss_OneUnit(rval, unit_4, idnm, idat, il);
  return rval;
}

//////////////////////////
//  Set Units Var
//////////////////////////

void SetUnitsVar::Initialize() {
  set_nm = true;
}

void SetUnitsVar::Destroy() {
  CutLinks();
}

void SetUnitsVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(input_data_var);
  GetInputDataVar();
}

void SetUnitsVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!input_data_var, quiet, rval, "input_data = NULL");
  CheckProgVarRef(input_data_var, quiet, rval);
  if(!CheckError(unit_1 && unit_1->var_type != ProgVar::T_DynEnum, quiet, rval, 
		 "unit_1 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError(unit_1 && !unit_1->dyn_enum_val.enum_type, quiet, rval, 
	       "unit_1 does not have enum_type set -- must be set to one associated with layer unit names!");
  if(!CheckError(unit_2 && unit_2->var_type != ProgVar::T_DynEnum, quiet, rval, 
		 "unit_2 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError(unit_2 && !unit_2->dyn_enum_val.enum_type, quiet, rval, 
	       "unit_2 does not have enum_type set -- must be set to one associated with layer unit names!");
  if(!CheckError(unit_3 && unit_3->var_type != ProgVar::T_DynEnum, quiet, rval, 
		 "unit_3 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError(unit_3 && !unit_3->dyn_enum_val.enum_type, quiet, rval, 
	       "unit_3 does not have enum_type set -- must be set to one associated with layer unit names!");
  if(!CheckError(unit_4 && unit_4->var_type != ProgVar::T_DynEnum, quiet, rval, 
		 "unit_4 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError(unit_4 && !unit_4->dyn_enum_val.enum_type, quiet, rval, 
	       "unit_4 does not have enum_type set -- must be set to one associated with layer unit names!");

    
}

String SetUnitsVar::GetDisplayName() const {
  String rval = "Set Units: Vars";
  if(unit_1) rval += unit_1->name + " ";
  if(unit_2) rval += unit_2->name + " ";
  if(unit_3) rval += unit_3->name + " ";
  if(unit_4) rval += unit_4->name + " ";
  return rval;
}

bool SetUnitsVar::GetInputDataVar() {
  if(input_data_var) return true;
  Program* my_prog = program();
  if(!my_prog) return false;
  input_data_var = my_prog->args.FindVarType(ProgVar::T_Object, &TA_DataTable);
  if(input_data_var) return true;
  input_data_var = my_prog->vars.FindVarType(ProgVar::T_Object, &TA_DataTable);
  if(!input_data_var) {
    input_data_var = (ProgVar*)my_prog->args.New(1, &TA_ProgVar); // make an arg by default
    input_data_var->name = "input_data";
    input_data_var->var_type = ProgVar::T_Object;
  }
  return true;
}

bool SetUnitsVar::GenCss_OneUnit(String& rval, ProgVarRef& un, const String& idnm, 
				 DataTable* idat, const String& il) {
  int colno;
  if(un) {
    if(idat->FindColName(un->dyn_enum_val.enum_type->name, colno, true)) {
      rval += il + idnm + ".SetValAsFloatM(1.0, " + String(colno) + ", -1, "
	+ un->name + ");\n";
      if(set_nm) {
	if(idat->FindColName("Name", colno, true)) {
	  rval += il + "{ String nm = " + idnm + ".GetValAsString(" + String(colno)
	    + ", -1); nm += \"_\" + " + un->name + "; "
	    + idnm + ".SetValAsString(nm, " + String(colno) + ", -1); }\n";
	}
      }
      return true;
    }
  }
  return false;
}

const String SetUnitsVar::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  if(!input_data_var) return il + "// input_data_var not set!\n";
  String idnm = input_data_var->name;
  DataTable* idat = (DataTable*)input_data_var->object_val.ptr();
  if(!idat) return il + "// input_data not set!\n";
  String rval;
  GenCss_OneUnit(rval, unit_1, idnm, idat, il);
  GenCss_OneUnit(rval, unit_2, idnm, idat, il);
  GenCss_OneUnit(rval, unit_3, idnm, idat, il);
  GenCss_OneUnit(rval, unit_4, idnm, idat, il);
  return rval;
}


