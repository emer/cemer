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

#include "LocalVars.h"
#include <Program>
#include <ProgVar>

TA_BASEFUNS_CTORS_DEFN(LocalVars);

taTypeDef_Of(float_Matrix);
taTypeDef_Of(double_Matrix);
taTypeDef_Of(int_Matrix);
taTypeDef_Of(String_Matrix);
taTypeDef_Of(Variant_Matrix);


void LocalVars::Initialize() {
}

void LocalVars::Destroy() {
  CutLinks();
}

void LocalVars::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  local_vars.CheckConfig(quiet, rval);
}

void LocalVars::GenCssBody_impl(Program* prog) {
  if(local_vars.size > 0) {
    prog->AddLine(this, "// local variables", ProgLine::MAIN_LINE); // best we've got
    local_vars.GenCss_ProgVars(prog);
  }
}

const String LocalVars::GenListing_children(int indent_level) {
  return local_vars.GenListing(indent_level + 1);
}

String LocalVars::GetDisplayName() const {
  String rval;
  rval += "LocalVars (";
  rval += String(local_vars.size);
  rval += " vars)";
  return rval;
}

ProgVar* LocalVars::FindVarName(const String& var_nm) const {
  return local_vars.FindName(var_nm);
}

ProgVar* LocalVars::AddVar() {
  return (ProgVar*)local_vars.New_gui(1);
}

ProgVar* LocalVars::AddFloatMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_float_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}
ProgVar* LocalVars::AddDoubleMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_double_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}
ProgVar* LocalVars::AddIntMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_int_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}
ProgVar* LocalVars::AddStringMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_String_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}
ProgVar* LocalVars::AddVarMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_Variant_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}

bool LocalVars::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;
  dc.downcase();
  // look for container
  if (dc.startsWith("local_") || dc.startsWith("localv") || dc.startsWith("loc_var")) {
    return true;
  }
  // no? look for data type
  if(!code.contains(' ')) return false; // must have at least one space
  String vartyp = trim(code.before(' '));
  if(vartyp.endsWith('*')) vartyp = vartyp.before('*',-1);
  TypeDef* td = TypeDef::FindGlobalTypeName(vartyp, false);
  if(td != NULL) return true;   // yep.
  return false;
}

bool LocalVars::CvtFmCode(const String& code) {
  String dc = code;
  dc.downcase();
  if (dc.startsWith("local_") || dc.startsWith("localv") || dc.startsWith("loc_var")) {
    // we generally try to make sure that the toolbox name works..
    return true;
  }

  String vartyp = trim(code.before(' '));
  if(vartyp.endsWith('*')) vartyp = vartyp.before('*',-1);
  String varnm = trim(code.after(' '));
  if(varnm.endsWith(';')) varnm = varnm.before(';',-1);
  TypeDef* td = TypeDef::FindGlobalTypeName(vartyp, false);
  if(td == NULL) return false; // shouldn't happen
  ProgVar* var = AddVar();
  var->SetName(varnm);
  if(vartyp == "int")
    var->SetInt(0);
  else if((vartyp == "float") || (vartyp == "double"))
    var->SetReal(0.0);
  else if(vartyp == "String")
    var->SetString("");
  else if(vartyp == "bool")
    var->SetBool(false);
  else {
    var->SetObjectType(td);     // catch all.
  }
  return true;
}


