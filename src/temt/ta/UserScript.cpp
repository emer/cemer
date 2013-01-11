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

#include "UserScript.h"

using namespace std;

void UserScript::SetProgExprFlags() {
  script.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
  script.SetExprFlag(ProgExpr::FULL_STMT); // full statements for parsing
}

void UserScript::Initialize() {
  static String _def_user_script("// TODO: Add your CSS script code here.\n");
  script.expr = _def_user_script;
  SetProgExprFlags();
}

void UserScript::UpdateAfterEdit_impl() {
  SetProgExprFlags();
  inherited::UpdateAfterEdit_impl();
}

void UserScript::GenCssBody_impl(Program* prog) {
  script.ParseExpr();           // re-parse just to be sure!
  prog->AddLine(this, script.GetFullExpr(), ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

String UserScript::GetDisplayName() const {
  return script.expr;
}

void UserScript::ImportFromFile(istream& strm) {
  script.expr = _nilString;
  char c;
  while((c = strm.get()) != EOF) {
    script.expr += c;
  }
  UpdateAfterEdit();
}

void UserScript::ImportFromFileName(const String& fnm) {
  String full_fnm = taMisc::FindFileOnLoadPath(fnm);
  fstream strm;
  strm.open(full_fnm, ios::in);
  ImportFromFile(strm);
  strm.close();
}

void UserScript::ExportToFile(ostream& strm) {
  strm << script.GetFullExpr();
}

void UserScript::ExportToFileName(const String& fnm) {
  fstream strm;
  strm.open(fnm, ios::out);
  ExportToFile(strm);
  strm.close();
}
