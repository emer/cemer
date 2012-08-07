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
  update_after = false;
  grouped = false;
  group_col = "Group";
  group_order = PERMUTED;
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

void NetDataLoop::GetOrderVal() {
  inherited::GetOrderVal();
  if(!grouped) return;

  if(!group_order_var) GetOrderVar();
  else {
    group_order = (Order)group_order_var->int_val;
  }
}

void NetDataLoop::GetOrderVar() {
  inherited::GetOrderVar();
  if(!grouped) return;

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
  group_order_var->hard_enum_type = TA_NetDataLoop.sub_types.FindName("Order");
  group_order = (Order)group_order_var->int_val;
}

void NetDataLoop::GetIndexVar() {
  inherited::GetIndexVar();
  Program* my_prog = program();
  if(!my_prog) return;

  if(!my_prog->vars.FindName("trial")) {
    ProgVar* trial = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    trial->name = "trial";
    trial->var_type = ProgVar::T_Int;
    trial->SetVarFlag(ProgVar::CTRL_READ_ONLY);
    trial->DataChanged(DCR_ITEM_UPDATED);
  }

  if(!grouped) return;

  if(!group_index_var) {
    if(!(group_index_var = my_prog->vars.FindName("group_index"))) {
      group_index_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
       group_index_var->name = "group_index";
      group_index_var->ClearVarFlag(ProgVar::CTRL_PANEL); // generally not needed there
      group_index_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  group_index_var->var_type = ProgVar::T_Int;
}

void NetDataLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!grouped) return;

  CheckError(!group_index_var, quiet, rval, "group_index_var = NULL");
  CheckError(!group_order_var, quiet, rval, "group_order_var = NULL");
}

void NetDataLoop::GetGroupList() {
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

void NetDataLoop::GetItemList(int group_idx) {
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

void NetDataLoop::GenCssPre_impl(Program* prog) {
  if(!data_var || !index_var) {
    prog->AddLine(this, "// NetDataLoop ERROR vars not set!", ProgLine::MAIN_LINE);
    return;
  }
  String data_nm = data_var->name;
  String idx_nm = index_var->name;
  if(!grouped) {
    prog->AddLine(this, "{ // NetDataLoop " + data_nm, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->IncIndent();
    prog->AddLine(this, "NetDataLoop* data_loop = this" + GetPath(NULL,program()) + ";");
    prog->AddLine(this, "data_loop->GetOrderVal(); // order_var variable controls order -- make sure we have current value");
    prog->AddLine(this, "data_loop->DMem_Initialize(network); // note: this assumes network variable exists!");
    prog->AddLine(this, String("data_loop->item_idx_list.SetSize(") + data_nm + "->ItemCount());");
    prog->AddLine(this, "data_loop->item_idx_list.FillSeq();");
    prog->AddLine(this, "if(data_loop->order == NetDataLoop::PERMUTED) data_loop->item_idx_list.Permute();");
    prog->AddLine(this, "int st_idx = data_loop->dmem_this_proc; // start with different items in dmem (0 if not)");
    prog->AddLine(this, "int inc_idx = data_loop->dmem_nprocs;  // this is 1 if no dmem");
    prog->AddLine(this, "int mx_idx = data_loop->item_idx_list.size;");
    prog->AddLine(this, "bool dmem_even = true; // is the total number an even multiple of dmem_nprocs?");
    prog->AddLine(this, "if(mx_idx % inc_idx != 0) {");
    prog->IncIndent();
    prog->AddLine(this, "dmem_even = false;  mx_idx = ((mx_idx / inc_idx) + 1) * inc_idx; // round up");
    prog->DecIndent();
    prog->AddLine(this, "}");
    prog->AddLine(this, "for(" + idx_nm + " = st_idx; " + idx_nm + " < mx_idx; " + idx_nm + " += inc_idx) {");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"in for loop\"");
    prog->AddLine(this, "int data_row_idx;");
    // note: cannot use random call here to select if over size because this throws off random
    // seed consistency between dmem processes!!!
    prog->AddLine(this, "if(data_loop->order == NetDataLoop::RANDOM) data_row_idx = Random::IntZeroN(data_loop->item_idx_list.size);");
    prog->AddLine(this, "else data_row_idx = data_loop->item_idx_list[" + idx_nm + " % data_loop->item_idx_list.size ];");
    prog->AddLine(this, "if(!" + data_nm + "->ReadItem(data_row_idx)) break;");
    prog->AddLine(this, data_nm + "->WriteItem(data_row_idx); // set write to same as read, just in case some mods are happening along the way");
    prog->AddLine(this, "trial = " + idx_nm + "; network.trial = " + idx_nm + ";");
    if(update_after) {
      prog->AddLine(this, "network.UpdateAfterEdit();");
    }
  }
  else {
    if(!group_index_var) {
      prog->AddLine(this, "// NetDataLoop ERROR vars not set!", ProgLine::MAIN_LINE);
      return;
    }
    String gp_idx_nm = group_index_var->name;

    prog->AddLine(this, "{ // NetDataLoop " + data_nm, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->IncIndent();
    prog->AddLine(this, "NetDataLoop* data_loop = this" + GetPath(NULL,program()) + ";");
    prog->AddLine(this, "data_loop->GetOrderVal(); // order_var variables control order -- make sure we have current values");
    prog->AddLine(this, "data_loop->GetGroupList();");
    prog->AddLine(this, "if(data_loop->group_order == NetDataLoop::PERMUTED) data_loop->group_idx_list.Permute();");
    prog->AddLine(this,  "for(" + gp_idx_nm + " = 0; " + gp_idx_nm + " < data_loop->group_idx_list.size; " + gp_idx_nm + "++) {");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"in group for loop\"");
    prog->AddLine(this, "network.group = " + gp_idx_nm + ";");
    prog->AddLine(this, "network.Init_Sequence(); // some algorithms use this to initialize at start");
    prog->AddLine(this, "int group_data_row_idx;");
    prog->AddLine(this, "int group_data_row_idx_rnd;");
    prog->AddLine(this, "if(data_loop->group_order == NetDataLoop::RANDOM) {");
    prog->AddLine(this, "  group_data_row_idx_rnd = Random::IntZeroN(data_loop->group_idx_list.size);");
    prog->AddLine(this, "  group_data_row_idx = data_loop->group_idx_list[group_data_row_idx_rnd]; }");
    prog->AddLine(this, "else group_data_row_idx = data_loop->group_idx_list[" + gp_idx_nm + "];");
    prog->AddLine(this, "data_loop->GetItemList(group_data_row_idx);");
    prog->AddLine(this, "if(data_loop->order == NetDataLoop::PERMUTED) data_loop->item_idx_list.Permute();");
    prog->AddLine(this, "for(" + idx_nm + " = 0; " + idx_nm + " < data_loop->item_idx_list.size; " + idx_nm + "++) {");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"in item for loop\"");
    prog->AddLine(this, "int item_data_row_idx;");
    prog->AddLine(this, "int item_data_row_idx_rnd;");
    prog->AddLine(this, "if(data_loop->order == NetDataLoop::RANDOM) {");
    prog->AddLine(this, " item_data_row_idx_rnd = Random::IntZeroN(data_loop->item_idx_list.size);");
    prog->AddLine(this, " item_data_row_idx = data_loop->item_idx_list[item_data_row_idx_rnd]; }");
    prog->AddLine(this, "else item_data_row_idx = data_loop->item_idx_list[" + idx_nm + "];");
    prog->AddLine(this, "if(!" + data_nm + "->ReadItem(item_data_row_idx)) break;");
    prog->AddLine(this, data_nm + "->WriteItem(item_data_row_idx); // set write to same as read, in case mods happening along the way");
    prog->AddLine(this, "trial = " + idx_nm + "; network.trial = " + idx_nm + ";");
    if(update_after) {
      prog->AddLine(this, "network.UpdateAfterEdit();");
    }
  }
}

void NetDataLoop::GenCssPost_impl(Program* prog) {
  if(!grouped) {
    inherited::GenCssPost_impl(prog);
    return;
  }

  if(!data_var || !group_index_var || !index_var) {
    prog->AddLine(this, "// NetDataLoop ERROR vars not set!");
    return;
  }
  prog->DecIndent();
  prog->AddLine(this, "} // item for loop");
  prog->DecIndent();
  prog->AddLine(this, "} // group for loop");
  prog->DecIndent();
  prog->AddLine(this, "} // NetDataLoop " + data_var->name);
}

String NetDataLoop::GetDisplayName() const {
  if(grouped) {
    String group_ord_str = GetTypeDef()->GetEnumString("Order", group_order);
    String item_ord_str = GetTypeDef()->GetEnumString("Order", order);
    String data_nm;
    if(data_var) data_nm = data_var->name;
    else data_nm = "ERROR: data_var not set!";
    String group_index_nm;
    if(group_index_var) group_index_nm = group_index_var->name;
    else group_index_nm = "ERROR: group_index_var not set!";
    String item_index_nm;
    if(index_var) item_index_nm = index_var->name;
    else item_index_nm = "ERROR: index_var not set!";
    return "Net Data Loop (gp: " + group_ord_str + "\n itm: " + item_ord_str
      + " over: " + data_nm + " group index: " + group_index_nm
      + " item index: " + item_index_nm +")";
  }
  else {
    ((NetDataLoop*)this)->GetOrderVal();
    String ord_str = GetTypeDef()->GetEnumString("Order", order);
    String data_nm;
    if(data_var) data_nm = data_var->name;
    else data_nm = "ERROR: data_var not set!";
    String index_nm;
    if(index_var) index_nm = index_var->name;
    else index_nm = "ERROR: index_var not set!";
    return "Net Data Loop (" + ord_str + " over: " + data_nm + " index: " + index_nm +")";
  }
}


//////////////////////////
//  NetGroupedDataLoop	//
//////////////////////////

void NetGroupedDataLoop::Initialize() {
  group_order = PERMUTED;
  item_order = SEQUENTIAL;
  group_col = 0;
  update_after = false;
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

  if(!my_prog->vars.FindName("trial")) {
    ProgVar* trial = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    trial->name = "trial";
    trial->var_type = ProgVar::T_Int;
    trial->SetVarFlag(ProgVar::CTRL_READ_ONLY);
    trial->DataChanged(DCR_ITEM_UPDATED);
  }

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

void NetGroupedDataLoop::GenCssPre_impl(Program* prog) {
  if(!data_var || !group_index_var || !item_index_var) {
    prog->AddLine(this, "// NetGroupedDataLoop ERROR vars not set!", ProgLine::MAIN_LINE);
    return;
  }
  String data_nm = data_var->name;
  String gp_idx_nm = group_index_var->name;
  String it_idx_nm = item_index_var->name;

  prog->AddLine(this, "{ // NetGroupedDataLoop " + data_nm, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, "NetGroupedDataLoop* data_loop = this" + GetPath(NULL,program()) + ";");
  prog->AddLine(this, "data_loop->GetOrderVals(); // order_var variables control order -- make sure we have current values");
  prog->AddLine(this, "data_loop->GetGroupList();");
  prog->AddLine(this, "if(data_loop->group_order == NetGroupedDataLoop::PERMUTED) data_loop->group_idx_list.Permute();");
  prog->AddLine(this, "for(" + gp_idx_nm + " = 0; " + gp_idx_nm + " < data_loop->group_idx_list.size; " + gp_idx_nm + "++) {");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"in group for loop\"");
  prog->AddLine(this, "network.group = " + gp_idx_nm + ";");
  prog->AddLine(this, "network.Init_Sequence(); // some algorithms use this to initialize at start");
  prog->AddLine(this, "int group_data_row_idx;");
  prog->AddLine(this, "int group_data_row_idx_rnd;");
  prog->AddLine(this, "if(data_loop->group_order == NetGroupedDataLoop::RANDOM) {");
  prog->AddLine(this, " group_data_row_idx_rnd = Random::IntZeroN(data_loop->group_idx_list.size);");
  prog->AddLine(this, " group_data_row_idx = data_loop->group_idx_list[group_data_row_idx_rnd]; }");
  prog->AddLine(this, "else group_data_row_idx = data_loop->group_idx_list[" + gp_idx_nm + "];");
  prog->AddLine(this, "data_loop->GetItemList(group_data_row_idx);");
  prog->AddLine(this, "if(data_loop->item_order == NetGroupedDataLoop::PERMUTED) data_loop->item_idx_list.Permute();");
  prog->AddLine(this, "for(" + it_idx_nm + " = 0; " + it_idx_nm + " < data_loop->item_idx_list.size; " + it_idx_nm + "++) {");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"in item for loop\"");
  prog->AddLine(this, "int item_data_row_idx;");
  prog->AddLine(this, "int item_data_row_idx_rnd;");
  prog->AddLine(this, "if(data_loop->item_order == NetGroupedDataLoop::RANDOM) {");
  prog->AddLine(this, " item_data_row_idx_rnd = Random::IntZeroN(data_loop->item_idx_list.size);");
  prog->AddLine(this, " item_data_row_idx = data_loop->item_idx_list[item_data_row_idx_rnd]; }");
  prog->AddLine(this, "else item_data_row_idx = data_loop->item_idx_list[" + it_idx_nm + "];");
  prog->AddLine(this, "if(!" + data_nm + "->ReadItem(item_data_row_idx)) break;");
  prog->AddLine(this, data_nm + "->WriteItem(item_data_row_idx); // set write to same as read, in case mods happening along the way");
  prog->AddLine(this, "trial = " + it_idx_nm + "; network.trial = " + it_idx_nm + ";");
  if(update_after) {
    prog->AddLine(this, "network.UpdateAfterEdit();");
  }
}

void NetGroupedDataLoop::GenCssPost_impl(Program* prog) {
  if(!data_var || !group_index_var || !item_index_var) {
    prog->AddLine(this, "// NetGroupedDataLoop ERROR vars not set!");
    return;
  }
  prog->DecIndent();
  prog->AddLine(this, "} // item for loop");
  prog->DecIndent();
  prog->AddLine(this, "} // group for loop");
  prog->DecIndent();
  prog->AddLine(this, "} // NetGroupedDataLoop " + data_var->name);
}

String NetGroupedDataLoop::GetDisplayName() const {
  String group_ord_str = GetTypeDef()->GetEnumString("Order", group_order);
  String item_ord_str = GetTypeDef()->GetEnumString("Order", item_order);
  String data_nm;
  if(data_var) data_nm = data_var->name;
  else data_nm = "ERROR: data_var not set!";
  String group_index_nm;
  if(group_index_var) group_index_nm = group_index_var->name;
  else group_index_nm = "ERROR: group_index_var not set!";
  String item_index_nm;
  if(item_index_var) item_index_nm = item_index_var->name;
  else item_index_nm = "ERROR: item_index_var not set!";
  return "Net Grouped Data Loop (gp: " + group_ord_str + "\n itm: " + item_ord_str
    + " over: " + data_nm + " group index: " + group_index_nm
    + " item index: " + item_index_nm +")";
}


//////////////////////////
//  Network Counters	//
//////////////////////////

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

//////////////////////////////////////
// init

String NetCounterInit::GetDisplayName() const {
  String rval = "Net Counter Init: ";
  if(counter) rval += counter->name;
  return rval;
}

void NetCounterInit::GenCssBody_impl(Program* prog) {
  if(!counter || !network_var) {
    prog->AddLine(this, "// NetCounterInit ERROR: vars not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, counter->name + " = 0;");
  prog->AddLine(this, network_var->name + "->" + counter->name + " = " + counter->name + ";",
		ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->AddVerboseLine(this, false, "\"new value:\", String(" + counter->name + ")");
  if(update_after)
    prog->AddLine(this, network_var->name + "->UpdateAfterEdit();");
}

//////////////////////////////////////
// incr

String NetCounterIncr::GetDisplayName() const {
  String rval = "Net Counter Incr: ";
  if(counter) rval += counter->name;
  return rval;
}

void NetCounterIncr::GenCssBody_impl(Program* prog) {
  if(!counter || !network_var) {
    prog->AddLine(this, "// NetCounterIncr ERROR: vars not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, counter->name + "++;");
  prog->AddLine(this, network_var->name + "->" + counter->name + " = " + counter->name + ";",
		ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->AddVerboseLine(this, false, "\"new value:\", String(" + counter->name + ")");
  if(update_after)
    prog->AddLine(this, network_var->name + "->UpdateAfterEdit();");
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

void NetUpdateView::GenCssBody_impl(Program* prog) {
  if(!network_var) {
    prog->AddLine(this, "// ERROR: network_var not set in NetUpdateView", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "if(update_net_view || this.IsStepProg())"
		+ network_var->name + "->UpdateAllViews();", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
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

void InitNamedUnits::GenCssBody_impl(Program* prog) {
  prog->AddLine(this, "// InitNamedUnits: no action taken", ProgLine::MAIN_LINE);
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
    cnm = taMisc::StringCVar(prefix + "_" + cnm); // make it legal!
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
  Network* net = (Network*)network_var->object_val.ptr();
  if(!net) return false;	// should not happen

  net->SetUnitNamesFromDataTable(undt, max_unit_chars, propagate_names);
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
  // not sure we really want to do this
  //    fr->SaveCurView(0);
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

bool SetUnitsLit::GenCss_OneUnit(Program* prog, DynEnum& un, const String& idnm, DataTable* idat) {
  int colno;
  if(un.IsSet()) {
    DynEnumType* det = un.enum_type.ptr();
    colno = idat->FindColNameIdx(det->name, true);
    if(TestError(colno < 0, "GenCss",
		 "data table column:",det->name,"not found in input data table:",
		 idat->name)) return false;
    if(un.NumVal() >= 0) {	// could be neg
      String rval;
      rval += idnm + ".SetValAsFloatM(1.0, " + String(colno) + ", -1, ";
      if(offset != 0)
	rval += String(offset) + "+" + un.NameVal() + ");";
      else
	rval += un.NameVal() + ");";
      prog->AddLine(this, rval);
      if(set_nm) {
	colno = idat->FindColNameIdx("Name", true);
	if(colno >= 0) {
	  String rval2 = "{ String nm = " + idnm + ".GetValAsString(" + String(colno)
	    + ", -1); if(!nm.empty()) nm += \"_\"; nm += \"" + un.NameVal().after("_") + "\"; "
	    + idnm + ".SetValAsString(nm, " + String(colno) + ", -1); }";
	  prog->AddLine(this, rval2);
	  // todo: add verbose 
	}
      }
      return true;
    }
  }
  return false;
}

void SetUnitsLit::GenCssBody_impl(Program* prog) {
  if(!input_data_var) {
    prog->AddLine(this, "// input_data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  String idnm = input_data_var->name;
  DataTable* idat = (DataTable*)input_data_var->object_val.ptr();
  if(!idat) {
    prog->AddLine(this, "// input_data not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "// " + GetDisplayName(), ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  GenCss_OneUnit(prog, unit_1, idnm, idat);
  GenCss_OneUnit(prog, unit_2, idnm, idat);
  GenCss_OneUnit(prog, unit_3, idnm, idat);
  GenCss_OneUnit(prog, unit_4, idnm, idat);
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

bool SetUnitsVar::GenCss_OneUnit(Program* prog, ProgVarRef& un, const String& idnm, DataTable* idat) {
  int colno;
  if((bool)un && (bool)un->dyn_enum_val.enum_type) {
    DynEnumType* det = un->dyn_enum_val.enum_type.ptr();
    colno = idat->FindColNameIdx(det->name, true);
    if(TestError(colno < 0, "GenCss",
		 "data table column:",det->name,"not found in input data table:",
		 idat->name)) return false;
    // if var has ability to go negative, check..
    bool neg_chk = false;
    if(det->enums.SafeEl(0) && det->enums.SafeEl(0)->value < 0) {
      prog->AddLine(this, "if(" + un->name + " >= 0) {");
      prog->IncIndent();
      neg_chk = true;
    }
    String rval;
    rval += idnm + ".SetValAsFloatM(1.0, " + String(colno) + ", -1, ";
    if(offset)
      rval += offset->name + "+" + un->name + ");";
    else
      rval += un->name + ");";
    prog->AddLine(this, rval);
    if(set_nm) {
      colno = idat->FindColNameIdx("Name", true);
      if(colno >= 0) {
	String rval2 = "{ String nm = " + idnm + ".GetValAsString(" + String(colno)
	  + ", -1); if(!nm.empty()) nm += \"_\"; String nwnm = " + un->name
	  + "; nwnm = nwnm.after(\"_\"); nm += nwnm; "
	  + idnm + ".SetValAsString(nm, " + String(colno) + ", -1); }";
	prog->AddLine(this, rval2);
      }
    }
    if(neg_chk) {
      prog->DecIndent();
      prog->AddLine(this, "}");
    }
    return true;
  }
  return false;
}

void SetUnitsVar::GenCssBody_impl(Program* prog) {
  if(!input_data_var) {
    prog->AddLine(this, "// input_data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  String idnm = input_data_var->name;
  DataTable* idat = (DataTable*)input_data_var->object_val.ptr();
  if(!idat) {
    prog->AddLine(this, "// input_data not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "// " + GetDisplayName(), ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  GenCss_OneUnit(prog, unit_1, idnm, idat);
  GenCss_OneUnit(prog, unit_2, idnm, idat);
  GenCss_OneUnit(prog, unit_3, idnm, idat);
  GenCss_OneUnit(prog, unit_4, idnm, idat);
}


//////////////////////////
//  WtInitPrompt	//
//////////////////////////

void WtInitPrompt::Initialize() {
  prompt = "Do you want to Initialize Network Weights? (WARNING: this will erase all previous training!)";
  yes_label = "Yes";
  no_label = "No";
}

void WtInitPrompt::GenCssPre_impl(Program* prog) {
  if(taMisc::gui_active && !taMisc::server_active) {
    prog->AddLine(this, "{ int chs = 0;");
    prog->IncIndent();
    prog->AddLine(this, String("if(network->epoch > 0) chs = taMisc::Choice(\"")
		  + prompt + "\", \"" + yes_label + "\", \"" + no_label + "\");",
		  ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->AddLine(this, "if(chs == 0) {");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"inside choice == yes\"");
  }
  else {
    prog->AddLine(this, "{");		// just a block to run..
    prog->IncIndent();
  }
}

void WtInitPrompt::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
  if(taMisc::gui_active && !taMisc::server_active) {	// extra close
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
}

