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

#include "NetGroupedDataLoop.h"

void NetGroupedDataLoop::Initialize() {
  group_order = PERMUTED;
  item_order = SEQUENTIAL;
  group_col = 0;
  update_after = false;
}

void NetGroupedDataLoop::GetOrderVals() {
  if (!group_order_var || !item_order_var) {
    GetOrderVars();
  }
  else {
    group_order = (Order)group_order_var->int_val;
    item_order = (Order)item_order_var->int_val;
  }
}

void NetGroupedDataLoop::GetOrderVars() {
  Program* my_prog = program();
  if (!my_prog) return;

  if (!group_order_var || group_order_var->name != "group_order") {
    group_order_var = my_prog->vars.FindName("group_order");
    if (!group_order_var) {
      group_order_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      group_order_var->name = "group_order";
      group_order_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  group_order_var->var_type = ProgVar::T_HardEnum;
  group_order_var->hard_enum_type = TA_NetGroupedDataLoop.sub_types.FindName("Order");
  group_order = (Order)group_order_var->int_val;

  if (!item_order_var || item_order_var->name != "item_order") {
    item_order_var = my_prog->vars.FindName("item_order");
    if (!item_order_var) {
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
  if (!my_prog) return;

  if (!my_prog->vars.FindName("trial")) {
    ProgVar* trial = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    trial->name = "trial";
    trial->var_type = ProgVar::T_Int;
    trial->SetVarFlag(ProgVar::CTRL_READ_ONLY);
    trial->DataChanged(DCR_ITEM_UPDATED);
  }

  if (!group_index_var) {
    group_index_var = my_prog->vars.FindName("group_index");
    if (!group_index_var) {
      group_index_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      group_index_var->name = "group_index";
      group_index_var->ClearVarFlag(ProgVar::CTRL_PANEL); // generally not needed there
      group_index_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  group_index_var->var_type = ProgVar::T_Int;

  if (!item_index_var) {
    item_index_var = my_prog->vars.FindName("item_index");
    if (!item_index_var) {
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

