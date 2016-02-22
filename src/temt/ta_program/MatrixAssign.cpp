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

#include "MatrixAssign.h"
#include <Program>
#include <taMisc>

#include <taiWidgetTokenChooser>

taTypeDef_Of(DataTable);
taTypeDef_Of(taMatrix);


TA_BASEFUNS_CTORS_DEFN(MatrixAssign);

void MatrixAssign::Initialize() {
  data_table = false;
}

void MatrixAssign::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!variable, quiet, rval, "variable is NULL");
  if(variable) {
    data_table = (variable->object_type == &TA_DataTable);
    if(!data_table) {
      CheckError(!variable->object_type->InheritsFrom(&TA_taMatrix), quiet, rval,
             "variable does not point to a Matrix or DataTable object -- must do so!");
    }
  }
  if(data_table) {
    CheckError(col.expr.empty(), quiet, rval,
               "column expression is empty -- must specify it for data table variable");
    col.CheckConfig(quiet,rval);
  }
  CheckError(dim0.expr.empty(), quiet, rval,
             "first dimension expression is empty -- must specify at least one dimension");

  dim0.CheckConfig(quiet,rval);
  dim1.CheckConfig(quiet,rval);
  dim2.CheckConfig(quiet,rval);
  dim3.CheckConfig(quiet,rval);
  dim4.CheckConfig(quiet,rval);
  dim5.CheckConfig(quiet,rval);
  expr.CheckConfig(quiet, rval);
}


String MatrixAssign::GetIndexExpr() const {
  String mtx_code;
  if(data_table) {
    if(col.expr.empty())
      mtx_code << "[\"column\"]";
    else
      mtx_code << "[" << col.GetFullExpr() << "]";
  }
  mtx_code += "[";
  if(dim0.empty())
    mtx_code << "index";
  else
    mtx_code << dim0.GetFullExpr();
  if(dim1.expr.nonempty()) {
    mtx_code << "," << dim1.GetFullExpr();
  }
  if(dim2.expr.nonempty()) {
    mtx_code << "," << dim2.GetFullExpr();
  }
  if(dim3.expr.nonempty()) {
    mtx_code << "," << dim3.GetFullExpr();
  }
  if(dim4.expr.nonempty()) {
    mtx_code << "," << dim4.GetFullExpr();
  }
  if(dim5.expr.nonempty()) {
    mtx_code << "," << dim5.GetFullExpr();
  }
  mtx_code += "]";
  return mtx_code;
}

void MatrixAssign::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();             // re-parse just to be sure!
  if(data_table) {
    col.ParseExpr();
  }
  dim0.ParseExpr();
  dim1.ParseExpr();
  dim2.ParseExpr();
  dim3.ParseExpr();
  dim4.ParseExpr();
  dim5.ParseExpr();
  if (!variable) {
    prog->AddLine(this, "// WARNING: MatrixAssign not generated here -- variable not specified", ProgLine::MAIN_LINE);
    return;
  }

  String mtx_code = variable->name + GetIndexExpr();
  String code = mtx_code + " = " + expr.GetFullExpr() + ";";
  
  prog->AddLine(this, code, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"prev value:\", String(" + mtx_code + ")"); // moved above
  prog->AddVerboseLine(this, false, "\"new  value:\", String(" + mtx_code + ")"); // after
}

String MatrixAssign::GetDisplayName() const {
  String rval;
  if(variable) {
    rval = variable->name;
  }
  else {
    rval = "variable";
  }

  String idx = GetIndexExpr();
  rval += idx;
  rval += " = " + expr.GetFullExpr();
  return rval;
}

bool MatrixAssign::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.endsWith(';')) return false; // don't pick up css exprs
  if(code.freq('=') >= 1) {
    String lhs = code.before('=');
    if(lhs.contains('[') && lhs.contains(']') && !lhs.contains('.') &&
       !lhs.contains("->")) return true;
  }
  return false;
}

bool MatrixAssign::CvtFmCode(const String& code) {
  String lhs = trim(code.before('='));
  String rhs = trim(code.after('='));
  if(rhs.endsWith(';')) rhs = rhs.before(';',-1);

  String varnm = lhs.before('[');
  if(varnm == "variable") return false;
  variable = FindVarNameInScope(varnm, true); // option to make
  expr.SetExpr(rhs);

  if(variable) {
    data_table = (variable->object_type == &TA_DataTable);
  }
  String idxs = lhs.from('[');
  if(idxs.empty()) return false;
  if(data_table) {
    String cols = idxs.between('[', ']');
    idxs = idxs.after(']');
    col.SetExpr(cols);
  }
  idxs = idxs.between('[', ']');
  if(idxs.empty()) return false;
  int dim = 0;
  while(idxs.nonempty()) {
    String ds = idxs;
    if(ds.contains(',')) {
      ds = ds.before(',');
      idxs = idxs.after(',');
    }
    else {
      idxs = "";
    }
    switch(dim) {
    case 0:  dim0.SetExpr(ds); break;
    case 1:  dim1.SetExpr(ds); break;
    case 2:  dim2.SetExpr(ds); break;
    case 3:  dim3.SetExpr(ds); break;
    case 4:  dim4.SetExpr(ds); break;
    case 5:  dim5.SetExpr(ds); break;
    }
    dim++;
  }
  return true;
}

bool MatrixAssign::ChooseMe() {
  // first get the object
  if (!variable) {
    taiWidgetTokenChooser* chooser =  new taiWidgetTokenChooser(&TA_ProgVar, NULL, NULL, NULL, 0, "");
    chooser->SetTitleText("Choose the variable you will be setting");
    Program* scope_program = GET_MY_OWNER(Program);
    chooser->GetImageScoped(NULL, &TA_ProgVar, scope_program, &TA_Program); // scope to this guy
    bool okc = chooser->OpenChooser();
    if(okc && chooser->token()) {
      variable = (ProgVar*)chooser->token();
      UpdateAfterEdit();
    }
    delete chooser;
  }
  return true;
}
