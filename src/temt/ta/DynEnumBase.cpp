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
#include <int_Array>
#include <String_Array>
#include <int_Matrix>
#include <String_Matrix>

#include <SigLinkSignal>

TA_BASEFUNS_CTORS_DEFN(DynEnumBase);
SMARTREF_OF_CPP(DynEnumBase); // DynEnumBaseRef

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
  String rval = name + " (enum: " + String(enums.size) + " items)";
  if(desc.nonempty())
    rval += " // " + desc;
  return rval;
}

bool DynEnumBase::BrowserEditSet(const String& code, int move_after) {
  String cd = CodeGetDesc(code);
  if(cd.contains("(enum:"))
    cd = trim(cd.before("(enum:"));
  return taNBase::BrowserEditSet(cd, move_after);
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

String DynEnumBase::NumberToName(int val) const {
  String rval;
  if(bits) {
    String rval;
    for(int i=0;i<enums.size;i++) {
      DynEnumItem* it = enums.FastEl(i);
      if(val & it->value) {
        if(!rval.empty()) rval += "|";
        rval += it->name;
      }
    }
  }
  else {
    for(int i=0;i<enums.size;i++) {
      DynEnumItem* it = enums.FastEl(i);
      if(val == it->value) {
        return it->name;
      }
    }
  }
  return rval;
}

static String dyn_enum_last_err_val;

int DynEnumBase::NameToNumber(const String& nm) const {
  int rval = 0;
  if(bits) {
    String strval = nm;
    while(strval.nonempty()) {
      String curstr = strval;
      if(strval.contains('|')) {
        curstr = strval.before('|');
        strval = strval.after('|');
      }
      else {
        strval = _nilString;
      }
      DynEnumItem* it = enums.FindName(curstr);
      if(!it) {
        if(dyn_enum_last_err_val != curstr) {
          if(!TestWarning(!it, "SetNameVal", "value label:", curstr, "not found!")) {
            rval |= it->value;
          }
          dyn_enum_last_err_val = curstr;
        }
      }
    }
  }
  else {
    DynEnumItem* it = enums.FindName(nm);
    if(!it) {
      if(dyn_enum_last_err_val != nm) {
        if(!TestError(!it, "SetNameVal", "value label:", nm, "not found!")) {
          rval = it->value;
        }
        dyn_enum_last_err_val = nm;
      }
    }
  }
  return rval;
}

void  DynEnumBase::NumberToName_Array(String_Array& names, const int_Array& vals) const {
  if(names.size < vals.size) {
    names.SetSize(vals.size);
  }
  for(int i=0;i<vals.size; i++) {
    names.FastEl(i) = NumberToName(vals.FastEl(i));
  }
}

void  DynEnumBase::NameToNumber_Array(int_Array& vals, const String_Array& names) const {
  dyn_enum_last_err_val = "";
  if(vals.size < names.size) {
    vals.SetSize(names.size);
  }
  for(int i=0;i<names.size; i++) {
    vals.FastEl(i) = NameToNumber(names.FastEl(i));
  }
}

void  DynEnumBase::NumberToName_Matrix(String_Matrix& names, const int_Matrix& vals) const {
  if(names.size < vals.size) {
    names.SetGeom(1,vals.size);
  }
  for(int i=0;i<vals.size; i++) {
    names.FastEl_Flat(i) = NumberToName(vals.FastEl_Flat(i));
  }
}

void  DynEnumBase::NameToNumber_Matrix(int_Matrix& vals, const String_Matrix& names) const {
  dyn_enum_last_err_val = "";
  if(vals.size < names.size) {
    vals.SetGeom(1,names.size);
  }
  for(int i=0;i<names.size; i++) {
    vals.FastEl_Flat(i) = NameToNumber(names.FastEl_Flat(i));
  }
}

