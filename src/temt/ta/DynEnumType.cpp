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

#include "DynEnumType.h"
#include <Program>
#include <taProject>
#include <DataTable>

#include <SigLinkSignal>

using namespace std;

void DynEnumType::Initialize() {
  SetDefaultName();
  bits = false;
}

DynEnumItem* DynEnumType::NewEnum() {
  return (DynEnumItem*)enums.New_gui(1); // primarily a gui op
}

DynEnumItem* DynEnumType::AddEnum(const String& nm, int val) {
  DynEnumItem* it = (DynEnumItem*)enums.New(1);
  it->name = nm;
  it->value = val;
  enums.OrderItems();
  it->SigEmitUpdated();
  return it;
}

void DynEnumType::SeqNumberItems(int first_val) {
  if(bits) {
    int val = 1;
    for(int i=0;i<enums.size;i++) {
      DynEnumItem* it = enums.FastEl(i);
      it->value = val;
      it->SigEmitUpdated();
      val = val << 1;
    }
  }
  else {
    int val = first_val;
    for(int i=0;i<enums.size;i++) {
      DynEnumItem* it = enums.FastEl(i);
      it->value = val;
      it->SigEmitUpdated();
      val++;
    }
  }
}

bool DynEnumType::CopyToAllProgs() {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  FOREACH_ELEM_IN_GROUP(Program, pg, proj->programs) {
    DynEnumType* tp = (DynEnumType*)pg->types.FindName(name); // find my name
    if(!tp || tp == this || !tp->InheritsFrom(&TA_DynEnumType)) continue;
    tp->CopyFrom(this);
  }
  return true;
}

taBase* DynEnumType::FindTypeName(const String& nm) const {
  if(name == nm) return (taBase*)this;
  int idx = FindNameIdx(nm);
  if(idx >= 0)  {
    return enums.FastEl(idx);
  }
  return NULL;
}

String DynEnumType::GetDisplayName() const {
  return name + " (enum: " + String(enums.size) + " items)";
}

void DynEnumType::GenCssPre_impl(Program* prog) {
  if(enums.size == 0) return;
  prog->AddLine(this, "enum " + name + " {", ProgLine::MAIN_LINE);
  prog->IncIndent();
}

void DynEnumType::GenCssBody_impl(Program* prog) {
  if(enums.size == 0) return;
  for(int i=0;i<enums.size;i++) {
    DynEnumItem* it = enums.FastEl(i);
    String code = it->name + " \t = ";
    if(bits)
      code += String(it->value, "%x") + ",";
    else
      code += String(it->value) + ",";
    prog->AddLine(it, code, it->desc);
  }
}

void DynEnumType::GenCssPost_impl(Program* prog) {
  if(enums.size == 0) return;
  prog->DecIndent();
  prog->AddLine(this, "};");
}

ostream& DynEnumType::OutputType(ostream& strm) const {
  String rval = "enum " + name + " {\n";
  for(int i=0;i<enums.size;i++) {
    DynEnumItem* it = enums.FastEl(i);
    String code = it->name + " \t = ";
    if(bits)
      code += String(it->value, "%x") + ",";
    else
      code += String(it->value) + ",";
    rval += "  " + code + "  //  " + it->desc + "\n";
  }
  rval += "\n}";
  strm << rval;
  return strm;
}

bool DynEnumType::EnumsFromDataTable(DataTable* dt, const Variant& col) {
  if(TestError(!dt, "EnumsFromDataTable", "data table is null")) return false;
  DataCol* da = dt->GetColData(col);
  if(!da) return false;
  enums.StructUpdate(true);
  enums.Reset();
  for(int i=0; i<dt->rows; i++) {
    String val = da->GetValAsString(i);
    AddEnum(val, i);
  }
  enums.StructUpdate(false);
  return true;
}

void DynEnumType::SigEmit(int sls, void* op1, void* op2) {
  // dynenum is programmed to send us notifies, we trap those and
  // turn them into changes of us, to force gui to update (esp enum list)
  if (sls == SLS_CHILD_ITEM_UPDATED) {
    SigEmitUpdated();
    return; // don't send any further
  }
  inherited::SigEmit(sls, op1, op2);
}

void DynEnumType::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  enums.CheckConfig(quiet, rval);
}

bool DynEnumType::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool DynEnumType::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool DynEnumType::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}
