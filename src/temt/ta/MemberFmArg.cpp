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

#include "MemberFmArg.h"
#include <Program>
#include <ProgVar>
#include <taMisc>


void MemberFmArg::Initialize() {
  update_after = true;
  quiet = false;
  prv_obj = NULL;
}

void MemberFmArg::Copy_(const MemberFmArg& cp) {
  MemberFmArg::StatTypeDef(0)->CopyOnlySameType((void*)this, (void*)&cp);
  UpdateAfterCopy(cp);
  prv_obj = obj.ptr();
}

void MemberFmArg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(prv_obj && (bool)obj && (obj.ptr() != prv_obj)) {
    if((bool)obj->object_val && (bool)prv_obj->object_val &&
       !obj->object_val->InheritsFrom(prv_obj->object_val->GetTypeDef())) {
      path = "";                // reset path
      arg_name = "";
    }
  }
  prv_obj = obj.ptr();
  if(arg_name.empty() && path.nonempty()) {
    if(path.contains("."))
      arg_name = path.after(".",-1);
    else
      arg_name = path;
  }
}

void MemberFmArg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(arg_name.empty(), quiet, rval, "arg_name is empty");
}

void MemberFmArg::GenCssBody_impl(Program* prog) {
  if (!(bool)obj || path.empty() || arg_name.empty()) {
    prog->AddLine(this, "// WARNING: MemberFmArg not generated here -- obj or path not specified or expr empty", ProgLine::MAIN_LINE);
    return;
  }

  String flpth = obj->name + "->" + path;

  prog->AddLine(this, "{ String arg_str = taMisc::FindArgByName(\"" + arg_name + "\");",
                ProgLine::MAIN_LINE);
  prog->IncIndent();
  prog->AddLine(this, "if(arg_str.nonempty()) {");
  prog->IncIndent();
  prog->AddLine(this, flpth + " = arg_str;");
  if(update_after) {
    prog->AddLine(this, obj->name + "->UpdateAfterEdit();");
    if(path.contains('.')) {
      // also do uae on immediate owner!
      prog->AddLine(this, obj->name + "->" + path.before('.',-1) + "->UpdateAfterEdit();");
    }
  }

  if(!quiet || IsVerbose())     // special case
    prog->AddLine(this, String("taMisc::Info(\"Set ") + flpth + " to:\"," + flpth + ");");
  prog->DecIndent();
  prog->AddLine(this, "}");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void MemberFmArg::GenRegArgs(Program* prog) {
  prog->AddLine(this, String("taMisc::AddEqualsArgName(\"") + arg_name + "\");");
  prog->AddLine(this, String("taMisc::AddArgNameDesc(\"") + arg_name
                + "\", \"MemberFmArg: obj = " + (((bool)obj) ? obj->name : "NOT SET")
                + " path = " + path + "\");");
}

String MemberFmArg::GetDisplayName() const {
  if (!obj || path.empty())
    return "(object or path not selected)";

  String rval;
  rval = obj->name + "->" + path + " = ";
  rval += "Arg: " + arg_name;
  return rval;
}
