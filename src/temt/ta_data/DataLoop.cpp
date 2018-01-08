// Copyright 2013-2018, Regents of the University of Colorado,
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

taTypeDef_Of(DataTable);

#include <Program>
#include <NameVar_PArray>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataLoop);


void DataLoop::Initialize() {
  order = SEQUENTIAL;
}

DataTable* DataLoop::GetData() {
  if(!data_var) return NULL;
  if(data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)data_var->object_val.ptr();
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
      order_var->SigEmitUpdated();
    }
  }
  bool needs_updt = false;
  if(order_var->var_type != ProgVar::T_HardEnum)
    needs_updt = true;
  order_var->var_type = ProgVar::T_HardEnum;
  order_var->hard_enum_type = TA_DataLoop.sub_types.FindName("Order");
  order = (Order)order_var->int_val;
  if(needs_updt) {
    order_var->SigEmitUpdated();
  }
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
      index_var->SigEmitUpdated();
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
      CheckError(!data_var->object_val.ptr()->InheritsFrom(&TA_DataTable),
                 quiet, rval,"data_var does not point to a data table");
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
  prog->AddLine(this, String("DataLoop* data_loop = this") + GetPath(program()) + ";");
  prog->AddLine(this, "data_loop->GetOrderVal(); // order_var variable controls order -- make sure we have current value");
  prog->AddLine(this, String("data_loop->item_idx_list.SetSize(") + data_nm + "->rows);");
  prog->AddLine(this, "data_loop->item_idx_list.FillSeq();");
  prog->AddLine(this, "if(data_loop->order == DataLoop::PERMUTED) data_loop->item_idx_list.Permute();");
  prog->AddLine(this, String("for(") + idx_nm + " = 0; " + idx_nm + " < " + data_nm + "->rows; "
                + idx_nm + "++) {");
  prog->IncIndent();
  prog->AddLine(this, "int data_row_idx;");
  prog->AddLine(this, "if(data_loop->order == DataLoop::RANDOM) data_row_idx = Random::IntZeroN(data_loop->item_idx_list.size);");
  prog->AddLine(this, "else");
  prog->AddLine(this, String("  data_row_idx = data_loop->item_idx_list[") + idx_nm + "];");
  prog->AddLine(this, String("if(!") + data_nm + "->ReadItem(data_row_idx)) break; // set read index");
  prog->AddLine(this, String("if(!") + data_nm + "->WriteItem(data_row_idx)) break; // set write index too");
}

bool DataLoop::GenCssBody_impl(Program* prog) {
  loop_code.GenCss(prog);
  return true;
}

void DataLoop::GenCssPost_impl(Program* prog) {
  if(!data_var || !index_var) return;
  prog->DecIndent();
  prog->AddLine(this, "} // for loop");
  prog->DecIndent();
  prog->AddLine(this, "} // DataLoop " + data_var->name);
}

String DataLoop::GetDisplayName() const {
  String ord_str = GetTypeDef()->GetEnumString("Order", order);
  String rval = "Data Loop (" + ord_str + "): ";  // displaying order so users don't miss that information
  
  if(data_var)
    rval += " table=" + data_var->name + " ";
  else
    rval += " table=? ";
  
  if (index_var)
    rval += " index=" + index_var->name + " ";
  else
    rval += " index=? ";
  
  if (order_var)
    rval += " order_var=" + order_var->name + " ";
  else
    rval += " order_var=? ";
  
  return rval;
}

bool DataLoop::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if (CvtFmCodeCheckNames(code))
    return true;
  
  String dc = code;  dc.downcase();
  if(dc.startsWith("data loop")) return true;
  return false;
}

bool DataLoop::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String remainder = code.after(":");
  if(remainder.empty()) return true;
  
  NameVar_PArray nv_pairs;
  taMisc::ToNameValuePairs(remainder, nv_pairs);
  
  for (int i=0; i<nv_pairs.size; i++) {
    String name = nv_pairs.FastEl(i).name;
    name.downcase();
    String value = nv_pairs.FastEl(i).value.toString();
    
    if (name.startsWith("tab")) {
      data_var = FindVarNameInScope(value, false); // don't make
    }
    if (name.startsWith("index")) {
      index_var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("order var") || name.startsWith("order_var")) {
      order_var = FindVarNameInScope(value, false); // don't make
      if (order_var) {
        order = StringToOrderType(order_var->name);}
    }
  }
  
  SigEmitUpdated();
  return true;
}

DataLoop::Order DataLoop::StringToOrderType(const String& order_type) {
  if (order_type.startsWith("permute"))
    return DataLoop::PERMUTED;
  else if (order_type.startsWith("rand"))
    return DataLoop::RANDOM;
  else
    return DataLoop::SEQUENTIAL;
}

