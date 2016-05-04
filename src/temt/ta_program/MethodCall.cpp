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

#include "MethodCall.h"
#include <MethodDef>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <Program>

#include <taiWidgetTokenChooser>
#include <taiWidgetMethodDefChooser>

#include <ProgVar>


TA_BASEFUNS_CTORS_DEFN(MethodCall);


void MethodCall::Initialize() {
  method = NULL;
  obj_type = &TA_taBase; // placeholder
}

void MethodCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(obj)
    obj_type = obj->act_object_type();
  else {
    obj_type = &TA_taBase; // placeholder
  }

//  if(!taMisc::is_loading && method)
  if (method) { // needed to set required etc.
    if(meth_args.UpdateFromMethod(method)) { // changed
      if(taMisc::gui_active) {
        tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
      }
    }
    meth_sig = method->prototype();
    meth_desc = method->desc;
  }
}

void MethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!obj, quiet, rval, "obj is NULL");
  CheckError(!method, quiet, rval, "method is NULL");
}

void MethodCall::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  meth_args.CheckConfig(quiet, rval);
}

void MethodCall::GenCssBody_impl(Program* prog) {
  if (!((bool)obj && method)) {
    prog->AddLine(this, "// WARNING: MethodCall not generated here -- obj or method not specified", ProgLine::MAIN_LINE);
    return;
  }

  String rval;
  if(result_var)
    rval += result_var->name + " = ";
  rval += obj->name;
  rval += "->";
  rval += method->name;
  rval += meth_args.GenCssArgs();
  rval += ";";

  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

String MethodCall::GetDisplayName() const {
  if (!obj) {
    return "object.method()";
  }
  
  String rval;
  if(result_var)
    rval += result_var->name + " = ";
  rval += obj->name;
  rval += "->";
  if (method) {
    rval += method->name;
    rval += "(";
    for(int i=0;i<meth_args.size;i++) {
      ProgArg* pa = meth_args[i];
      if (i > 0)
        rval += ", ";
      rval += pa->expr.expr; // GetDisplayName();
    }
    rval += ")";
  }
  else {
    rval += "method()";
  }
  return rval;
}

void MethodCall::Help() {
  if(obj && (bool)obj->object_val) {
    obj->object_val->Help();
    // todo: add check for method and select that in class browser guy..
  }
  else {
    inherited::Help();
  }
}

/*const String MethodCall::statusTip(const KeyString& ks) const {
  if (method)
    return method->prototype();
  else return inherited::statusTip(ks);
}*/

bool MethodCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  // fmt: [result = ]obj[.|->]method(args...
  // don't reject if user triggers parse with display name unedited
  if (CvtFmCodeCheckNames(code))
    return true;
  
  String dc = code;  dc.downcase();
  String dn = GetDisplayName(); dn = trim(dn.downcase());
  dn = dn.before(15);  // enough chars to distinguish
  if(dc.startsWith(dn))
    return true;

  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  if((mthobj.freq('.') + mthobj.freq("->")) != 1) return false;
  String objnm;
  if(mthobj.contains('.'))
    objnm = mthobj.before('.');
  else
    objnm = mthobj.before("->");
  objnm.trim();
  if (objnm.contains(' ')) {
    return false;  // too complicated an expression for this parser - fall back to a css expression
  }
  if(objnm.nonempty() && !objnm.contains('[')) return true;
  // syntax above should be enough to rule in -- no [ ] paths either tho -- nowhere to put
  return false;
}

bool MethodCall::CvtFmCode(const String& code) {
  if (code == GetDisplayName())  // don't bother to parse
    return true;
 
  String code_copy = code;
  String lhs = trim(code_copy.before('('));
  String mthobj = lhs;
  String rval;
  if(lhs.contains('=')) {
    mthobj = trim(lhs.after('='));
    rval = trim(lhs.before('='));
  }
  String objnm;
  String methnm;
  if(mthobj.contains('.')) {
    objnm = mthobj.before('.');
    methnm = mthobj.after('.');
  }
  else {
    objnm = mthobj.before("->");
    methnm = mthobj.after("->");
  }
  ProgVar* pv = FindVarNameInScope(objnm, true); // true = give option to make one
  if(!pv) return false;
  obj = pv;
  if(rval.nonempty())
    result_var = FindVarNameInScope(rval, true); // true = give option to make one
  if(obj)
    obj_type = obj->act_object_type();
  else {
    obj_type = &TA_taBase; // placeholder
  }
  MethodDef* md = obj_type->methods.FindName(methnm);
  if(md) {
    method = md;
    meth_sig = method->prototype();
    meth_desc = method->desc;
    meth_args.UpdateFromMethod(method);
    // now tackle the args
    // if none of the args have been set we need to get them added to the args list before parsing
    String args = trim(code_copy.after('('));
    args = trim(args.before(')', -1));
    meth_args.ParseArgString(args);
  }
  
  return true;
}

bool MethodCall::ChooseMe() {
  // first get the object
  bool keep_choosing = false;
  if (!obj) {
    taiWidgetTokenChooser* chooser =  new taiWidgetTokenChooser(&TA_ProgVar, NULL, NULL, NULL, 0, "");
    chooser->item_filter = (item_filter_fun)ProgEl::ObjProgVarFilter;
    chooser->SetTitleText("Choose the object for the method call");
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
  // now scope the method choices to the object type
  if (obj && keep_choosing) {
    TypeDef* obj_td = obj->act_object_type();
    taiWidgetMethodDefChooser* chooser =  new taiWidgetMethodDefChooser(obj_td, NULL, NULL, NULL, 0, "");
    chooser->SetTitleText("Choose the method to call");
    chooser->GetImage((MethodDef*)NULL, obj_td);
    bool okc = chooser->OpenChooser();
    if(okc && chooser->md()) {
      method = chooser->md();
      UpdateAfterEdit();
    }
    delete chooser;
  }
  return true;
}
