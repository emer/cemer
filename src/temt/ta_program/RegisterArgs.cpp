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

#include "RegisterArgs.h"
#include <ProgVar>
#include <Program>
#include <taMisc>
#include <ProgVarFmArg>
#include <MemberFmArg>
#include <DataColsFmArgs>
#include <ControlPanelsFmArgs>
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(RegisterArgs);


void RegisterArgs::Initialize() {
}

String RegisterArgs::GetDisplayName() const {
  String rval = "Register Args";
  return rval;
}

bool RegisterArgs::GenCssBody_impl(Program* prog) {
  prog->AddLine(this, "// Register Args:", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  AddArgsFmCode(prog, prog->prog_code);
  prog->AddLine(this, "taMisc::UpdateArgs();");
  prog->AddLine(this, "if(taMisc::CheckArgByName(\"Help\")) taMisc::HelpMsg();"); // extra help!
  return true;
}

void RegisterArgs::AddArgsFmCode(Program* prog, ProgEl_List& progs) {
  for(int i=0;i<progs.size;i++) {
    ProgEl* pel = progs[i];
    if(pel->InheritsFrom(&TA_ProgVarFmArg)) {
      ProgVarFmArg* pva = (ProgVarFmArg*)pel;
      pva->GenRegArgs(prog);
    }
    else if(pel->InheritsFrom(&TA_MemberFmArg)) {
      MemberFmArg* mfa = (MemberFmArg*)pel;
      mfa->GenRegArgs(prog);
    }
    else if(pel->InheritsFrom(&TA_DataColsFmArgs)) {
      DataColsFmArgs* dca = (DataColsFmArgs*)pel;
      dca->GenRegArgs(prog);
    }
    else if(pel->InheritsFrom(&TA_ControlPanelsFmArgs)) {
      ControlPanelsFmArgs* sea = (ControlPanelsFmArgs*)pel;
      sea->GenRegArgs(prog);
    }
    else {                      // look for sub-lists
      TypeDef* td = pel->GetTypeDef();
      for(int j=0;j<td->members.size;j++) {
        MemberDef* md = td->members[j];
        if(md->type->InheritsFrom(&TA_ProgEl_List)) {
          ProgEl_List* nxt_prgs = (ProgEl_List*)md->GetOff(pel);
          AddArgsFmCode(prog, *nxt_prgs);
        }
      }
    }
  }
}

bool RegisterArgs::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code; dc.downcase();
  String dn = GetDisplayName().downcase();
  if (dc.startsWith(dn))
    return true;
  return false;
}

bool RegisterArgs::CvtFmCode(const String& code) {
  return true;
}

