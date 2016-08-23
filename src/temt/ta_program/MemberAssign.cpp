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

#include "MemberAssign.h"
#include <Program>
#include <taMisc>

#include <taiWidgetTokenChooser>
#include <taiWidgetMemberDefChooser>

TA_BASEFUNS_CTORS_DEFN(MemberAssign);

void MemberAssign::Initialize() {
  update_after = false;
}

void MemberAssign::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if((bool)obj && expr.empty()) { // assume ok to set default here based on obj
    TypeDef* ot = obj->act_object_type();
    taBase* base_base = obj->object_val;
    update_after = UAEInProgram(path, ot, base_base);
  }
}


void MemberAssign::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

void MemberAssign::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();             // re-parse just to be sure!
  if (!(bool)obj || path.empty() || expr.empty()) {
    prog->AddLine(this, "// WARNING: MemberAssign not generated here -- obj or path not specified or expr empty", ProgLine::MAIN_LINE);
    return;
  }

  String path_term = path;
  String path_pre = path;
  if(path_term.contains('.')) {
    path_term = path_term.after('.', -1);
    path_pre = path.before('.', -1);
  }
  String opath;
  if(path_term != path) {
    if(path_pre.startsWith('['))
      opath = obj->name + path_pre;
    else
      opath = obj->name + "->" + path_pre;
  }
  else {
    opath = obj->name;
  }
  opath = trim(opath);
  String fpath = opath + "->" + path_term;
  String rval;
  if(opath.endsWith(']')) {     // itr expression
    rval = "set(" + opath + ", \"" + path_term + "\", " + expr.GetFullExpr() + ");";
  }
  else {
    rval = fpath + " = " + expr.GetFullExpr() + ";";
  }
  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  if (update_after) {
    prog->AddLine(this, obj->name + "->UpdateAfterEdit();");
    if(path_term != path) {
        // also do uae on immediate owner!
      if(opath.endsWith(']')) { // itr expression -- need to use call
        prog->AddLine(this, "call(" + opath + ", \"UpdateAfterEdit\");");
      }
      else {
        prog->AddLine(this, opath + "->UpdateAfterEdit();");
      }
    }
  }
  prog->AddVerboseLine(this, true, "\"prev value:\", String(" + fpath + ")"); // moved above
  prog->AddVerboseLine(this, false, "\"new  value:\", String(" + fpath + ")"); // after
}

String MemberAssign::GetDisplayName() const {
  String rval;
  if(obj)
    rval = obj->name;
  else
    rval = "object";
  if(path.empty())
    rval += ".member";
  else if(path.startsWith('['))
    rval += path;
  else
    rval += "." + path;
  rval += " = ";
  rval += expr.GetFullExpr();
  return rval;
}

bool MemberAssign::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains('=')) return false;
  String lhs = code.before('=');
  if(!(lhs.contains('.') || lhs.contains("->"))) return false;
  return true;                  // probably enough?
}

bool MemberAssign::CvtFmCode(const String& code) {
  String lhs = trim(code.before('='));
  // if(lhs.contains('(') || lhs.contains(' ')) return false; // exclude others
  String objnm;
  String pathnm;
  if(lhs.contains('.')) {
    objnm = lhs.before('.');
    pathnm = lhs.after('.');
  }
  else {
    objnm = lhs.before("->");
    pathnm = lhs.after("->");
  }
  if(objnm.contains('[')) {
    if(pathnm.nonempty())
      pathnm = objnm.from('[') + "." + pathnm;
    else
      pathnm = objnm.from('[');
    objnm = objnm.before('[');
  }
  if(objnm == "?") return false;
  ProgVar* pv = FindVarNameInScope(objnm, true); // true = give option to make one
  if(!pv) return false;
  bool do_uae_updt = false;
  if(obj != pv) {
    obj = pv;
    do_uae_updt = true;
  }
  if(path != pathnm) {
    path = pathnm;
    do_uae_updt = true;
  }
  if(do_uae_updt) {
    TypeDef* ot = obj->act_object_type();
    taBase* base_base = obj->object_val;
    update_after = UAEInProgram(path, ot, base_base);
  }
  String rhs = trim(code.after('='));
  if(rhs.endsWith(';')) rhs = rhs.before(';',-1);
  expr.SetExpr(rhs);
  UpdateAfterEdit_impl();
  return true;
}

bool MemberAssign::ChooseMe() {
  // first get the object
  bool keep_choosing = false;
  if (!obj) {
    taiWidgetTokenChooser* chooser =  new taiWidgetTokenChooser(&TA_ProgVar, NULL, NULL, NULL, 0, "");
    chooser->SetTitleText("Choose the object that has the member you want to set");
    chooser->item_filter = (item_filter_fun)ProgEl::ObjProgVarFilter;
    Program* scope_program = GET_MY_OWNER(Program);
    chooser->GetImageScoped(NULL, &TA_ProgVar, scope_program, &TA_Program); // scope to this guy
    bool okc = chooser->OpenChooser();
    if(okc && chooser->token()) {
      ProgVar* tok = (ProgVar*)chooser->token();
      obj.set(tok);
      UpdateAfterEdit();
      keep_choosing = true;
    }
    delete chooser;
  }
  
  // now scope the member choices to the object type
  if (obj && keep_choosing) {
    TypeDef* obj_td = obj->act_object_type();
    taiWidgetMemberDefChooser* chooser =  new taiWidgetMemberDefChooser(obj_td, NULL, NULL, NULL, 0, "");
    chooser->SetTitleText("Choose the member to set");
    chooser->GetImage((MemberDef*)NULL, obj_td);
    bool okc = chooser->OpenChooser();
    if(okc && chooser->md()) {
      member_lookup = chooser->md();
      UpdateAfterEdit();
    }
    delete chooser;
  }
    
  return true;
}
