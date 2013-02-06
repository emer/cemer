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

#include "NetDataLoop.h"
#include <DataTable>
#include <Network>

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
  if (!grouped) return;

  if (!group_order_var) {
    GetOrderVar();
  }
  else {
    group_order = (Order)group_order_var->int_val;
  }
}

void NetDataLoop::GetOrderVar() {
  inherited::GetOrderVar();
  if (!grouped) return;

  Program* my_prog = program();
  if (!my_prog) return;
  if (!group_order_var || group_order_var->name != "group_order") {
    group_order_var = my_prog->vars.FindName("group_order");
    if (!group_order_var) {
      group_order_var = (ProgVar*) my_prog->vars.New(1, &TA_ProgVar);
      group_order_var->name = "group_order";
      group_order_var->SigEmitUpdated();
    }
  }
  group_order_var->var_type = ProgVar::T_HardEnum;
  group_order_var->hard_enum_type = TA_NetDataLoop.sub_types.FindName("Order");
  group_order = (Order)group_order_var->int_val;
}

void NetDataLoop::GetIndexVar() {
  inherited::GetIndexVar();
  Program* my_prog = program();
  if (!my_prog) return;

  if (!my_prog->vars.FindName("trial")) {
    ProgVar* trial = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    trial->name = "trial";
    trial->var_type = ProgVar::T_Int;
    trial->SetVarFlag(ProgVar::CTRL_READ_ONLY);
    trial->SigEmitUpdated();
  }

  if (!grouped) return;

  if (!group_index_var) {
    group_index_var = my_prog->vars.FindName("group_index");
    if (!group_index_var) {
      group_index_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      group_index_var->name = "group_index";
      group_index_var->ClearVarFlag(ProgVar::CTRL_PANEL); // generally not needed there
      group_index_var->SigEmitUpdated();
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


