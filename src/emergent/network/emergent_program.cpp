// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "emergent_program.h"
#include "netstru.h"
#include "css_machine.h"
#include "ta_datatable_qtso.h"
#include "ta_project.h"

bool NetBaseProgEl::NetProgVarFilter(void* base_, void* var_) {
  bool rval = ObjProgVarFilter(base_, var_);
  if(!rval) return false; // doesn't pass basic test

  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->object_type && var->object_type->InheritsFrom(&TA_Network));
}


//////////////////////////
//     NetDataLoop	//
//////////////////////////

void NetDataLoop::Initialize() {
  order = SEQUENTIAL;
}

const String NetDataLoop::GenCssPre_impl(int indent_level) {
  String id1 = cssMisc::Indent(indent_level+1);
  String id2 = cssMisc::Indent(indent_level+2);
  String id3 = cssMisc::Indent(indent_level+3);
  if(!data_var || !index_var) return id1 + "// NetDataLoop ERROR vars not set!\n";
  String data_nm = data_var->name;
  String idx_nm = index_var->name;

  String rval = cssMisc::Indent(indent_level) + "{ // NetDataLoop " + data_nm + "\n";
  rval += id1 + "NetDataLoop* data_loop = this" + GetPath(NULL,program()) + ";\n";
  rval += id1 + "data_loop->GetOrderVal(); // order_var variable controls order -- make sure we have current value\n";
  rval += id1 + "data_loop->DMem_Initialize(network); // note: this assumes network variable exists!\n";
  rval += id1 + "data_loop->item_idx_list.SetSize(" + data_nm + "->ItemCount());\n";
  rval += id1 + "data_loop->item_idx_list.FillSeq();\n";
  rval += id1 + "if(data_loop->order == NetDataLoop::PERMUTED) data_loop->item_idx_list.Permute();\n";
  rval += id1 + "int st_idx = data_loop->dmem_this_proc; // start with different items in dmem (0 if not)\n";
  rval += id1 + "int inc_idx = data_loop->dmem_nprocs;  // this is 1 if no dmem\n";
  rval += id1 + "int mx_idx = data_loop->item_idx_list.size;\n";
  rval += id1 + "bool dmem_even = true; // is the total number an even multiple of dmem_nprocs?\n";
  rval += id1 + "if(mx_idx % inc_idx != 0) {\n";
  rval += id2 + "dmem_even = false;  mx_idx = ((mx_idx / inc_idx) + 1) * inc_idx; // round up\n";
  rval += id1 + "}\n";
  rval += id1 + "for(" + idx_nm + " = st_idx; " + idx_nm + " < mx_idx; " + idx_nm + " += inc_idx) {\n";
  rval += id2 + "int data_row_idx;\n";
  // note: cannot use random call here to select if over size because this throws off random
  // seed consistency between dmem processes!!!
  rval += id2 + "if(data_loop->order == NetDataLoop::RANDOM) data_row_idx = Random::IntZeroN(data_loop->item_idx_list.size);\n";
  rval += id2 + "else data_row_idx = data_loop->item_idx_list[" + idx_nm + " % data_loop->item_idx_list.size ];\n";
  rval += id2 + "if(!" + data_nm + "->ReadItem(data_row_idx)) break;\n";
  rval += id2 + data_nm + "->WriteItem(data_row_idx); // set write to same as read, just in case some mods are happening along the way\n";
  rval += id2 + "trial = " + idx_nm + "; network.trial = " + idx_nm + ";\n";
  return rval;
}

void NetDataLoop::DMem_Initialize(Network* net) {
#ifdef DMEM_COMPILE
  dmem_nprocs = net->dmem_trl_comm.nprocs;
  dmem_this_proc = net->dmem_trl_comm.this_proc;
#else
  dmem_nprocs = 1;
  dmem_this_proc = 0;
#endif
}

String NetDataLoop::GetDisplayName() const {
  String ord_str = GetTypeDef()->GetEnumString("Order", order);
  String data_nm;
  if(data_var) data_nm = data_var->name;
  else data_nm = "ERROR: data_var not set!";
  return "Net Data Loop (" + ord_str + " over: " + data_nm + ")";
}


//////////////////////////
//  NetGroupedDataLoop	//
//////////////////////////

void NetGroupedDataLoop::Initialize() {
  group_order = PERMUTED;
  item_order = SEQUENTIAL;
  group_col = 0;
}

void NetGroupedDataLoop::GetOrderVals() {
  if(!group_order_var || !item_order_var) GetOrderVars();
  else {
    group_order = (Order)group_order_var->int_val;
    item_order = (Order)item_order_var->int_val;
  }
}

void NetGroupedDataLoop::GetOrderVars() {
  Program* my_prog = program();
  if(!my_prog) return;
  if(!group_order_var || group_order_var->name != "group_order") {
    if(!(group_order_var = my_prog->vars.FindName("group_order"))) {
      group_order_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      group_order_var->name = "group_order";
      group_order_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  group_order_var->var_type = ProgVar::T_HardEnum;
  group_order_var->hard_enum_type = TA_NetGroupedDataLoop.sub_types.FindName("Order");
  group_order = (Order)group_order_var->int_val;

  if(!item_order_var || item_order_var->name != "item_order") {
    if(!(item_order_var = my_prog->vars.FindName("item_order"))) {
      item_order_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      item_order_var->name = "item_order";
      item_order_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  item_order_var->var_type = ProgVar::T_HardEnum;
  item_order_var->hard_enum_type = TA_NetGroupedDataLoop.sub_types.FindName("Order");
  item_order = (Order)item_order_var->int_val;
}

void NetGroupedDataLoop::GetIndexVars() {
  Program* my_prog = program();
  if(!my_prog) return;
  if(!group_index_var) {
    if(!(group_index_var = my_prog->vars.FindName("group_index"))) {
      group_index_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      group_index_var->name = "group_index";
      group_index_var->ClearVarFlag(ProgVar::CTRL_PANEL); // generally not needed there
      group_index_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  group_index_var->var_type = ProgVar::T_Int;

  if(!item_index_var) {
    if(!(item_index_var = my_prog->vars.FindName("item_index"))) {
      item_index_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      item_index_var->name = "item_index";
      item_index_var->ClearVarFlag(ProgVar::CTRL_PANEL); // generally not needed there
      item_index_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  item_index_var->var_type = ProgVar::T_Int;
}

void NetGroupedDataLoop::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetOrderVars();
  GetIndexVars();
}

void NetGroupedDataLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!CheckError(!data_var, quiet, rval,  "data_var = NULL")) {
    if(!CheckError(!data_var->object_val, quiet, rval,"data_var is NULL!")) {
      CheckError(!data_var->object_val.ptr()->InheritsFrom(&TA_DataTable), 
		 quiet, rval,"data_var does not point to a data table");
    }
  }
  CheckError(!group_index_var, quiet, rval, "group_index_var = NULL");
  CheckError(!item_index_var, quiet, rval, "item_index_var = NULL");
  CheckError(!group_order_var, quiet, rval, "group_order_var = NULL");
  CheckError(!item_order_var, quiet, rval, "item_order_var = NULL");
}

void NetGroupedDataLoop::GetGroupList() {
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

void NetGroupedDataLoop::GetItemList(int group_idx) {
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

const String NetGroupedDataLoop::GenCssPre_impl(int indent_level) {
  String id1 = cssMisc::Indent(indent_level+1);
  String id2 = cssMisc::Indent(indent_level+2);
  String id3 = cssMisc::Indent(indent_level+3);
  if(!data_var || !group_index_var || !item_index_var) return id1 + "// NetGroupedDataLoop ERROR vars not set!\n";
  String data_nm = data_var->name;
  String gp_idx_nm = group_index_var->name;
  String it_idx_nm = item_index_var->name;

  String rval = cssMisc::Indent(indent_level) + "{ // NetGroupedDataLoop " + data_nm + "\n";
  rval += id1 + "NetGroupedDataLoop* data_loop = this" + GetPath(NULL,program()) + ";\n";
  rval += id1 + "data_loop->GetOrderVals(); // order_var variables control order -- make sure we have current values\n";
  rval += id1 + "data_loop->GetGroupList();\n";
  rval += id1 + "if(data_loop->group_order == NetGroupedDataLoop::PERMUTED) data_loop->group_idx_list.Permute();\n";
  rval += id1 + "for(" + gp_idx_nm + " = 0; " + gp_idx_nm + " < data_loop->group_idx_list.size; " + gp_idx_nm + "++) {\n";
  rval += id2 + "network.group = " + gp_idx_nm + ";\n";
  rval += id2 + "network.Init_Sequence(); // some algorithms use this to initialize at start\n";
  rval += id2 + "int group_data_row_idx;\n";
  rval += id2 + "if(data_loop->group_order == NetGroupedDataLoop::RANDOM) \n";
  rval += id2 + "   group_data_row_idx = data_loop->group_idx_list[Random::IntZeroN(data_loop->group_idx_list.size)];\n";
  rval += id2 + "else group_data_row_idx = data_loop->group_idx_list[" + gp_idx_nm + "];\n";
  rval += id2 + "data_loop->GetItemList(group_data_row_idx);\n";
  rval += id2 + "if(data_loop->item_order == NetGroupedDataLoop::PERMUTED) data_loop->item_idx_list.Permute();\n";
  rval += id2 + "for(" + it_idx_nm + " = 0; " + it_idx_nm + " < data_loop->item_idx_list.size; " + it_idx_nm + "++) {\n";
  rval += id3 + "int item_data_row_idx;\n";
  rval += id3 + "if(data_loop->item_order == NetGroupedDataLoop::RANDOM) \n";
  rval += id3 + "   item_data_row_idx = data_loop->item_idx_list[Random::IntZeroN(data_loop->item_idx_list.size)];\n";
  rval += id3 + "else item_data_row_idx = data_loop->item_idx_list[" + it_idx_nm + "];\n";
  rval += id3 + "if(!" + data_nm + "->ReadItem(item_data_row_idx)) break;\n";
  rval += id3 + data_nm + "->WriteItem(item_data_row_idx); // set write to same as read, in case mods happening along the way\n";
  rval += id3 + "trial = " + it_idx_nm + "; network.trial = " + it_idx_nm + ";\n";
  return rval;
}

const String NetGroupedDataLoop::GenCssBody_impl(int indent_level) {
  return loop_code.GenCss(indent_level + 3);
}

const String NetGroupedDataLoop::GenCssPost_impl(int indent_level) {
  if(!data_var || !group_index_var || !item_index_var) return "// NetGroupedDataLoop ERROR vars not set!\n";
  String rval = cssMisc::Indent(indent_level+2) + "} // item for loop\n";
  rval += cssMisc::Indent(indent_level+1) + "} // group for loop\n";
  rval += cssMisc::Indent(indent_level) + "} // NetGroupedDataLoop " + data_var->name + "\n";
  return rval;
}

String NetGroupedDataLoop::GetDisplayName() const {
  String group_ord_str = GetTypeDef()->GetEnumString("Order", group_order);
  String item_ord_str = GetTypeDef()->GetEnumString("Order", item_order);
  String data_nm;
  if(data_var) data_nm = data_var->name;
  else data_nm = "ERROR: data_var not set!";
  return "Net Grouped Data Loop (gp: " + group_ord_str + "\n itm: " + item_ord_str
    + " over: " + data_nm + ")";
}

///////////////////////////////////
//	Obsolete converters

void BasicDataLoop::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  cerr << "WARNING: BasicDataLoop type is obsolete -- change type to NetDataLoop -- edit: " 
       << GetPath() << " and press button to fix!" << endl;
}

void BasicDataLoop::ChangeToNetDataLoop() {
  GetIndexVar();
  ChangeMyType(&TA_NetDataLoop);
}

void GroupedDataLoop::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  cerr << "WARNING: GroupedDataLoop type is obsolete -- change type to NetGroupedDataLoop -- edit: " 
       << GetPath() << " and press button to fix!" << endl;
}

void GroupedDataLoop::ChangeToNetGroupedDataLoop() {
  GetIndexVars();
  ChangeMyType(&TA_NetGroupedDataLoop);
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
  if((bool)network_var && ((bool)network_var->object_val)) {
    network_type = network_var->object_val->GetTypeDef();
  }
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetLocalCtrVar();
}

void NetCounterInit::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!counter, quiet, rval, "counter is NULL");
  CheckError(!(bool)network_var, quiet, rval, "network_var = NULL");
  CheckError((bool)network_var && !network_var->object_val, quiet, rval,
	     "network_var object = NULL");
  CheckError(!local_ctr_var, quiet, rval, "local_ctr_var = NULL");
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
    local_ctr_var->DataChanged(DCR_ITEM_UPDATED);
  }
  local_ctr_var->var_type = ProgVar::T_Int;
}

const String NetCounterInit::GenCssBody_impl(int indent_level) {
  if(!counter || !network_var) return "// NetCounterInit ERROR: vars not set!\n";
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
  if((bool)network_var && ((bool)network_var->object_val)) {
    network_type = network_var->object_val->GetTypeDef();
  }
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetLocalCtrVar();
}

void NetCounterIncr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!counter, quiet, rval, "counter is NULL");
  CheckError(!network_var, quiet, rval, "network_var = NULL");
  CheckError((bool)network_var && !network_var->object_val, quiet, rval,
	     "network_var object = NULL");
  CheckError(!local_ctr_var, quiet, rval, "local_ctr_var = NULL");
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
    local_ctr_var->DataChanged(DCR_ITEM_UPDATED);
  }
  local_ctr_var->var_type = ProgVar::T_Int;
}

const String NetCounterIncr::GenCssBody_impl(int indent_level) {
  if(!counter || !network_var) return "// NetCounterInit ERROR: vars not set!\n";
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
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetUpdateVar();
}

void NetUpdateView::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!network_var, quiet, rval, "network_var = NULL");
  CheckError(!update_var, quiet, rval, "update_var = NULL");
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
    update_var->DataChanged(DCR_ITEM_UPDATED);
  }
  update_var->var_type = ProgVar::T_Bool;
}

const String NetUpdateView::GenCssBody_impl(int indent_level) {
  if(!network_var) return "// ERROR: network_var not set in NetUpdateView\n";
  String rval = cssMisc::Indent(indent_level) + "if(update_net_view) "
    + network_var->name + "->UpdateAllViews();\n";
  return rval;
}


////////////////////////////////////////////////////
//		Named Units Framework
////////////////////////////////////////////////////

void InitNamedUnits::Initialize() {
  n_lay_name_chars = 1;
  max_unit_chars = -1;
}

void InitNamedUnits::Destroy() {
  CutLinks();
}

void InitNamedUnits::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;

  GetInputDataVar();
  GetUnitNamesVar();
  GetNetworkVar();
  InitNamesTable();
}

// this is really all it does -- no actual code gen!!
void InitNamedUnits::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!input_data_var, quiet, rval, "input_data = NULL");
  CheckError(!unit_names_var, quiet, rval, "unit_names = NULL");
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
    input_data_var->object_type = &TA_DataTable;
    input_data_var->DataChanged(DCR_ITEM_UPDATED);
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
      unit_names_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  unit_names_var->var_type = ProgVar::T_Object;

  if(!unit_names_var->object_val) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(!proj) return false;
    DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("InputData");
    DataTable* rval = dgp->FindName("UnitNames");
    if(!rval) {
      rval = dgp->NewEl(1, &TA_DataTable);
      rval->name = "UnitNames";
      taMisc::Info("Note: created new data table named:", rval->name, "in .data.InputData");
      rval->DataChanged(DCR_ITEM_UPDATED);
      if(taMisc::gui_active) {
	tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe"); // todo: might be too radical.
      }
    }
    unit_names_var->object_val = rval;
    unit_names_var->object_type = &TA_DataTable;
    unit_names_var->DataChanged(DCR_ITEM_UPDATED);
  }
  return true;
}

bool InitNamedUnits::GetNetworkVar() {
  if((bool)network_var) return true;
  Program* my_prog = program();
  if(!my_prog) return false;
  network_var = my_prog->args.FindName("network");
  if(!network_var) {
    network_var = my_prog->vars.FindName("network");
  }
  if(!network_var) return false;
  network_var->var_type = ProgVar::T_Object;
  network_var->DataChanged(DCR_ITEM_UPDATED);
  return (bool)network_var->object_val;
}

const String InitNamedUnits::GenCssBody_impl(int indent_level) {
  return "// InitNamedUnits: no action taken\n";
}

bool InitNamedUnits::InitNamesTable() {
  GetInputDataVar();
  if(TestError(!input_data_var->object_val.ptr(), "InitNamesTable", "could not find input_data table -- must set that up to point to your input data table"))
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
    taMisc::Error("InitUnitNamesFmInputData", "null args -- should not happen -- report bug!");
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
      det = (DynEnumType*)my_prog->types.New(1, &TA_DynEnumType);
      det->name = ndc->name;
    }
    String prefix = ndc->name.before(n_lay_name_chars);
    InitDynEnumFmUnitNames(det, ndc, prefix);
  }
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(&(my_prog->types), "BrowserExpandAll");
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
  int i;
  for(i=0;i<unit_names_col->cell_size();i++) {
    String cnm = unit_names_col->GetValAsStringM(-1, i);
    if(cnm.empty()) continue;
    cnm = prefix + "_" + cnm;
    dyn_enum->AddEnum(cnm, i);
  }
  String cnm = dyn_enum->name;
  cnm = prefix + "_N" + cnm;
  dyn_enum->AddEnum(cnm, i);
  return true;
}

bool InitNamedUnits::LabelNetwork(bool propagate_names) {
  if(TestError(!GetUnitNamesVar(), "LabelNetwork", "could not find unit names data table -- this should not usually happen because it is auto-made if not found"))
    return false;
  if(TestError(!GetNetworkVar(), "LabelNetwork", "network variable is not set and could not find one -- please set and try again"))
    return false;

  DataTable* undt = (DataTable*)unit_names_var->object_val.ptr();
  if(!undt) return false;	// should not happen
  if(TestError(undt->rows < 1, "LabelNetwork", "unit names table doesn't have 1 or more rows!")) {
    return false;
  }

  Network* net = (Network*)network_var->object_val.ptr();
  if(!net) return false;	// should not happen

  for(int i=0;i<undt->cols();i++) {
    DataCol* ndc = undt->data.FastEl(i);
    Layer* lay = (Layer*)net->layers.FindLeafName(ndc->name);
    if(!lay) continue;
    InitLayerFmUnitNames(lay, ndc, max_unit_chars);
  }
  if(propagate_names)
    net->GetLocalistName();	// propagate
  return true;
}

bool InitNamedUnits::InitLayerFmUnitNames(Layer* lay, const DataCol* unit_names_col, int max_un_chars) {
  if(!lay || !unit_names_col) {
    taMisc::Error("InitLayerFmUnitNames", "null args");
    return false;
  }
  const MatrixGeom& cg = unit_names_col->cell_geom;
  taMatrix* nmat = (const_cast<DataCol*>(unit_names_col))->GetValAsMatrix(-1);
  if(nmat) {
    taBase::Ref(nmat);

    if(lay->unit_groups && cg.dims() == 4) { // check if all but first group is empty
      bool hugp_empty = true;
      int gx, gy, ux, uy;
      for(gy = 0; gy<cg.dim(3); gy++) {
	for(gx = 0; gx<cg.dim(2); gx++) {
	  if(gx == 0 && gy == 0) continue; // skip 1st gp
	  for(uy = 0; uy<cg.dim(1); uy++) {
	    for(ux = 0; ux<cg.dim(0); ux++) {
	      if(nmat->SafeElAsStr(ux,uy,gx,gy).nonempty()) {
		hugp_empty = false;
		break;
	      }
	    }
	  }
	}
      }
      if(hugp_empty) {
	lay->unit_names.SetGeom(2, cg.dim(0), cg.dim(1)); // just set for 1st gp
      }
      else {
	lay->unit_names.SetGeomN(cg); // get our geom
      }
    }
    else {
      lay->unit_names.SetGeomN(cg); // get our geom
    }
    for(int i=0;i<nmat->size && i<lay->unit_names.size;i++) {
      String cnm = nmat->SafeElAsStr_Flat(i);
      lay->unit_names.SetFmStr_Flat(cnm.elidedTo(max_un_chars), i);
    }
    taBase::unRefDone(nmat);
  }
  lay->SetUnitNames();		// actually set from these names
  return true;
}

bool InitNamedUnits::ViewDataLegend() {
  if(!input_data_var) return false;
  DataTable* idat = (DataTable*)input_data_var->object_val.ptr();
  if(!idat) return false;
  if(!unit_names_var) return false;
  DataTable* ndat = (DataTable*)unit_names_var->object_val.ptr();
  if(!ndat) return false;

  idat->SetUserData("N_ROWS", 5);
  GridTableView* igtv = idat->NewGridView();
  T3DataViewFrame* frame = igtv->GetFrame();

  ndat->SetUserData("N_ROWS", 1);
  ndat->SetUserData("MAT_VAL_TEXT", 1);
  ndat->SetUserData("WIDTH", 3.3f);
  GridTableView* ngtv = ndat->NewGridView(frame);

  ngtv->main_xform.translate.y = 1.13f;
  ngtv->main_xform.scale = .33f;
  frame->Render();
  frame->ViewAll(); 
  frame->GetCameraPosOrient();
  return true;
}


//////////////////////////
//  Set Units Lit
//////////////////////////

void SetUnitsLit::Initialize() {
  set_nm = true;
  offset = 0;
}

void SetUnitsLit::Destroy() {
  CutLinks();
}

void SetUnitsLit::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetInputDataVar();
}

void SetUnitsLit::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!input_data_var, quiet, rval, "input_data = NULL");
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
    input_data_var->DataChanged(DCR_ITEM_UPDATED);
  }
  return true;
}

bool SetUnitsLit::GenCss_OneUnit(String& rval, DynEnum& un, const String& idnm, 
				 DataTable* idat, const String& il) {
  int colno;
  if(un.IsSet()) {
    DynEnumType* det = un.enum_type.ptr();
    if(TestError(!idat->FindColName(det->name, colno, true), "GenCss",
		 "data table column:",det->name,"not found in input data table:",
		 idat->name)) return false;
    if(un.NumVal() >= 0) {	// could be neg
      rval += il + idnm + ".SetValAsFloatM(1.0, " + String(colno) + ", -1, ";
      if(offset != 0)
	rval += String(offset) + "+" + un.NameVal() + ");\n";
      else
	rval += un.NameVal() + ");\n";
      if(set_nm) {
	if(idat->FindColName("Name", colno, true)) {
	  rval += il + "{ String nm = " + idnm + ".GetValAsString(" + String(colno)
	    + ", -1); if(!nm.empty()) nm += \"_\"; nm += \"" + un.NameVal().after("_") + "\"; "
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
  rval += il + "// " + GetDisplayName() + "\n";
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
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetInputDataVar();
}

void SetUnitsVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!input_data_var, quiet, rval, "input_data = NULL");
  if(!CheckError((bool)unit_1 && unit_1->var_type != ProgVar::T_DynEnum, quiet, rval, 
		 "unit_1 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError((bool)unit_1 && !unit_1->dyn_enum_val.enum_type, quiet, rval, 
	       "unit_1 does not have enum_type set -- must be set to one associated with layer unit names!");
  if(!CheckError((bool)unit_2 && unit_2->var_type != ProgVar::T_DynEnum, quiet, rval, 
		 "unit_2 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError((bool)unit_2 && !unit_2->dyn_enum_val.enum_type, quiet, rval, 
	       "unit_2 does not have enum_type set -- must be set to one associated with layer unit names!");
  if(!CheckError((bool)unit_3 && unit_3->var_type != ProgVar::T_DynEnum, quiet, rval, 
		 "unit_3 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError((bool)unit_3 && !unit_3->dyn_enum_val.enum_type, quiet, rval, 
	       "unit_3 does not have enum_type set -- must be set to one associated with layer unit names!");
  if(!CheckError((bool)unit_4 && unit_4->var_type != ProgVar::T_DynEnum, quiet, rval, 
		 "unit_4 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError((bool)unit_4 && !unit_4->dyn_enum_val.enum_type, quiet, rval, 
	       "unit_4 does not have enum_type set -- must be set to one associated with layer unit names!");
}

String SetUnitsVar::GetDisplayName() const {
  String rval = "Set Units Vars: ";
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
    input_data_var->DataChanged(DCR_ITEM_UPDATED);
  }
  return true;
}

bool SetUnitsVar::GenCss_OneUnit(String& rval, ProgVarRef& un, const String& idnm, 
				 DataTable* idat, const String& il) {
  int colno;
  if((bool)un && (bool)un->dyn_enum_val.enum_type) {
    DynEnumType* det = un->dyn_enum_val.enum_type.ptr();
    if(TestError(!idat->FindColName(det->name, colno, true), "GenCss",
		 "data table column:",det->name,"not found in input data table:",
		 idat->name)) return false;
    // if var has ability to go negative, check..
    bool neg_chk = false;
    if(det->enums.SafeEl(0) && det->enums.SafeEl(0)->value < 0) {
      rval += il + "if(" + un->name + " >= 0) {\n";
      neg_chk = true;
    }
    rval += il + idnm + ".SetValAsFloatM(1.0, " + String(colno) + ", -1, ";
    if(offset)
      rval += offset->name + "+" + un->name + ");\n";
    else
      rval += un->name + ");\n";
    if(set_nm) {
      if(idat->FindColName("Name", colno, true)) {
	rval += il + "{ String nm = " + idnm + ".GetValAsString(" + String(colno)
	  + ", -1); if(!nm.empty()) nm += \"_\"; String nwnm = " + un->name
	  + "; nwnm = nwnm.after(\"_\"); nm += nwnm; "
	  + idnm + ".SetValAsString(nm, " + String(colno) + ", -1); }\n";
      }
    }
    if(neg_chk)
      rval += il + "}\n";
    return true;
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
  rval += il + "// " + GetDisplayName() + "\n";
  GenCss_OneUnit(rval, unit_1, idnm, idat, il);
  GenCss_OneUnit(rval, unit_2, idnm, idat, il);
  GenCss_OneUnit(rval, unit_3, idnm, idat, il);
  GenCss_OneUnit(rval, unit_4, idnm, idat, il);
  return rval;
}


//////////////////////////
//  WtInitPrompt	//
//////////////////////////

void WtInitPrompt::Initialize() {
  prompt = "Do you want to Initialize Network Weights? (WARNING: this will erase all previous training!)";
  yes_label = "Yes";
  no_label = "No";
}

const String WtInitPrompt::GenCssPre_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  String rval;
  if(taMisc::gui_active && !taMisc::server_active) {
    rval = il + "{ int chs = 0;\n";
    rval += il + "  if(network->epoch > 0) chs = taMisc::Choice(\"" + prompt + "\", \""
      + yes_label + "\", \""
      + no_label + "\");\n";
    rval += cssMisc::Indent(indent_level+1) + "if(chs == 0) {\n";
  }
  else {
    rval = il + "{\n";		// just a block to run..
  }
  return rval; 
}
