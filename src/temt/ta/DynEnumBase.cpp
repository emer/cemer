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

#include "DynEnumBase.h"
#include <Program>
#include <taProject>

#include <SigLinkSignal>

using namespace std;

void DynEnumBase::Initialize() {
  SetDefaultName();
  bits = false;
}

DynEnumItem* DynEnumBase::AddEnum(const String& nm, int val) {
  DynEnumItem* it = (DynEnumItem*)enums.New(1);
  it->name = nm;
  it->value = val;
  enums.OrderItems();
  it->SigEmitUpdated();
  return it;
}

taBase* DynEnumBase::FindTypeName(const String& nm) const {
  if(name == nm) return (taBase*)this;
  int idx = FindNameIdx(nm);
  if(idx >= 0)  {
    return enums.FastEl(idx);
  }
  return NULL;
}

String DynEnumBase::GetDisplayName() const {
  return name + " (enum: " + String(enums.size) + " items)";
}

void DynEnumBase::GenCssPre_impl(Program* prog) {
  if(enums.size == 0) return;
  prog->AddLine(this, "enum " + name + " {", ProgLine::MAIN_LINE);
  prog->IncIndent();
}

void DynEnumBase::GenCssBody_impl(Program* prog) {
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

void DynEnumBase::GenCssPost_impl(Program* prog) {
  if(enums.size == 0) return;
  prog->DecIndent();
  prog->AddLine(this, "};");
}

ostream& DynEnumBase::OutputType(ostream& strm) const {
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

void DynEnumBase::SigEmit(int sls, void* op1, void* op2) {
  // dynenum is programmed to send us notifies, we trap those and
  // turn them into changes of us, to force gui to update (esp enum list)
  if (sls == SLS_CHILD_ITEM_UPDATED) {
    SigEmitUpdated();
    return; // don't send any further
  }
  inherited::SigEmit(sls, op1, op2);
}

void DynEnumBase::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  enums.CheckConfig(quiet, rval);
}

bool DynEnumBase::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool DynEnumBase::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool DynEnumBase::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}
