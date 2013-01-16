// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "DataLoop.h"

#include <taMisc>


void DataLoop::Initialize() {
  order = SEQUENTIAL;
}

DataBlock* DataLoop::GetData() {
  if(!data_var) return NULL;
  if(data_var->object_type != &TA_DataBlock) return NULL;
  return (DataBlock*)data_var->object_val.ptr();
}
/*DataTable* DataLoop::GetData() {
  if(!data_var) return NULL;
  if(data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)data_var->object_val.ptr();
}*/

void DataLoop::GetOrderVar() {
  Program* my_prog = program();
  if (!my_prog) return;

  if (!order_var) {
    order_var = my_prog->vars.FindName("data_loop_order");
    if (!order_var) {
      order_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      order_var->name = "data_loop_order";
      order_var->DataItemUpdated();
    }
  }
  order_var->var_type = ProgVar::T_HardEnum;
  order_var->hard_enum_type = TA_DataLoop.sub_types.FindName("Order");
  order = (Order)order_var->int_val;
}

void DataLoop::GetIndexVar() {
  Program* my_prog = program();
  if (!my_prog) return;

  if (!index_var) {
    index_var = my_prog->vars.FindName("data_loop_index");
    if (!index_var) {
      index_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      index_var->name = "data_loop_index";
      index_var->ClearVarFlag(ProgVar::CTRL_PANEL); // generally not needed there
      index_var->DataItemUpdated();
    }
  }
  index_var->var_type = ProgVar::T_Int;
}

void DataLoop::GetOrderVal() {
  if(!order_var) GetOrderVar();
  else order = (Order)order_var->int_val;
}

void DataLoop::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetOrderVar();
  GetOrderVal();
  GetIndexVar();
}

void DataLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!CheckError(!data_var, quiet, rval,  "data_var = NULL")) {
    if(data_var->object_val) {
      CheckError(!data_var->object_val.ptr()->InheritsFrom(&TA_DataBlock),
                 quiet, rval,"data_var does not point to a data table (or data block)");
    }
  }
  CheckError(!index_var, quiet, rval, "index_var = NULL");
  CheckError(!order_var, quiet, rval, "order_var = NULL");
}

void DataLoop::GenCssPre_impl(Program* prog) {
  if(!data_var || !index_var) {
    prog->AddLine(this, "// DataLoop ERROR vars not set!", ProgLine::MAIN_LINE);
    return;
  }
  String data_nm = data_var->name;
  String idx_nm = index_var->name;

  prog->AddLine(this, String("{ // DataLoop ") + data_nm, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, String("DataLoop* data_loop = this") + GetPath(NULL,program()) + ";");
  prog->AddLine(this, "data_loop->GetOrderVal(); // order_var variable controls order -- make sure we have current value");
  prog->AddLine(this, String("data_loop->item_idx_list.SetSize(") + data_nm + "->ItemCount());");
  prog->AddLine(this, "data_loop->item_idx_list.FillSeq();");
  prog->AddLine(this, "if(data_loop->order == DataLoop::PERMUTED) data_loop->item_idx_list.Permute();");
  prog->AddLine(this, String("for(") + idx_nm + " = 0; " + idx_nm + " < " + data_nm + "->ItemCount(); "
                + idx_nm + "++) {");
  prog->IncIndent();
  prog->AddLine(this, "int data_row_idx;");
  prog->AddLine(this, "if(data_loop->order == DataLoop::RANDOM) data_row_idx = Random::IntZeroN(data_loop->item_idx_list.size);");
  prog->AddLine(this, "else");
  prog->AddLine(this, String("  data_row_idx = data_loop->item_idx_list[") + idx_nm + "];");
  prog->AddLine(this, String("if(!") + data_nm + "->ReadItem(data_row_idx)) break; // set read index");
  prog->AddLine(this, String("if(!") + data_nm + "->WriteItem(data_row_idx)) break; // set write index too");
}

void DataLoop::GenCssBody_impl(Program* prog) {
  loop_code.GenCss(prog);
}

void DataLoop::GenCssPost_impl(Program* prog) {
  if(!data_var || !index_var) return;
  prog->DecIndent();
  prog->AddLine(this, "} // for loop");
  prog->DecIndent();
  prog->AddLine(this, "} // DataLoop " + data_var->name);
}

String DataLoop::GetDisplayName() const {
  ((DataLoop*)this)->GetOrderVal();
  String ord_str = GetTypeDef()->GetEnumString("Order", order);
  String data_nm;
  if(data_var) data_nm = data_var->name;
  else data_nm = "ERROR: data_var not set!";
  String index_nm;
  if(index_var) index_nm = index_var->name;
  else index_nm = "ERROR: index_var not set!";
  return "DataTable Loop (" + ord_str + " over: " + data_nm + " index: " + index_nm +")";
}

void DataLoop::SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
                                    int dcr, void* op1_, void* op2_) {
  GetOrderVal();
  UpdateAfterEdit();
}
