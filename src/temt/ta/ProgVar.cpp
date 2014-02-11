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

#include "ProgVar.h"
#include <Program>
#include <ProgVar_List>
#include <taSigLinkItr>
#include <MemberDef>

taTypeDef_Of(taMatrix);
taTypeDef_Of(Function);
taTypeDef_Of(taProject);

#include <SigLinkSignal>
#include <taMisc>

#include <css_machine.h>
#include <css_ta.h>
#include <css_c_ptr_types.h>

void ProgVar::Initialize() {
  var_type = T_UnDef;
  int_val = 0;
  real_val = 0.0;
  bool_val = false;
  object_type = &TA_taOBase;
  hard_enum_type = NULL;
  objs_ptr = false;
  flags = (VarFlags)(CTRL_PANEL | NULL_CHECK | EDIT_VAL | SAVE_VAL);
  reference = false;
  parse_css_el = NULL;
  css_idx = -1;
}

void ProgVar::Destroy() {
  CutLinks();
}

void ProgVar::InitLinks() {
  taBase::Own(object_val, this);
  taBase::Own(dyn_enum_val, this);
  inherited::InitLinks();
  if(!taMisc::is_loading) {
    SetFlagsByOwnership();
  }
}

void ProgVar::CutLinks() {
  FreeParseCssEl();
  object_val.CutLinks();
  dyn_enum_val.CutLinks();
  inherited::CutLinks();
}

void ProgVar::SetFlagsByOwnership() {
  bool is_global = false;
  if(owner && owner->InheritsFrom(&TA_ProgVar_List)) {
    ProgVar_List* pvl = (ProgVar_List*)owner;
    if(pvl->owner && pvl->owner->InheritsFrom(&TA_Program))
      is_global = true;         // this is the only case in which vars are global
  }
  if(is_global) {
    ClearVarFlag(LOCAL_VAR);
    ClearVarFlag(FUN_ARG);
    if(!objs_ptr && var_type == T_Object && object_type && object_type->InheritsFrom(&TA_taMatrix)) {
      if(!HasVarFlag(QUIET)) {
        TestWarning(true, "ProgVar", "for Matrix* ProgVar named:",name,
                    "Matrix pointers should be located in LocalVars within the code, not in the global vars/args section, in order to properly manage the reference counting of matrix objects returned from various functions.");
      }
    }
  }
  else {
    objs_ptr = false;           // this is incompatible with being local
    SetVarFlag(LOCAL_VAR);
    ClearVarFlag(CTRL_PANEL);
    ClearVarFlag(CTRL_READ_ONLY);
    ClearVarFlag(NULL_CHECK);
    // now check for fun args
    ClearVarFlag(FUN_ARG);
    if(owner && owner->InheritsFrom(&TA_ProgVar_List)) {
      ProgVar_List* pvl = (ProgVar_List*)owner;
      if(pvl->owner && pvl->owner->InheritsFrom(&TA_Function))
        SetVarFlag(FUN_ARG);
    }
  }
  if(init_from) {               // NOTE: LOCAL_VAR is now editable and provides initializer value for local variables
    ClearVarFlag(EDIT_VAL);
  }
  else {
    SetVarFlag(EDIT_VAL);
  }
}

bool ProgVar::UpdateUsedFlag() {
  taSigLink* dl = sig_link();
  if(!dl) return false;
  int cnt = 0;
  taSigLinkItr itr;
  taSmartRef* el;
  FOR_DLC_EL_OF_TYPE(taSmartRef, el, dl, itr) {
    taBase* spo = el->GetOwner();
    if(!spo) continue;
    cnt++;
  }
  if(objs_ptr) cnt++;           // always get one count for this -- always used..
  bool prv_val = HasVarFlag(USED);
  if(cnt > 0) {
    SetVarFlag(USED);
  }
  else {
    ClearVarFlag(USED);
  }
  return HasVarFlag(USED) != prv_val; // return value is whether we changed
}

int ProgVar::GetEnabled() const {
  return HasVarFlag(USED);
}

int ProgVar::GetSpecialState() const {
  if(HasVarFlag(LOCAL_VAR)) return 0;
  if(init_from) return 1;
  if(!HasVarFlag(SAVE_VAL)) return 4;
  return 0;
}

void ProgVar::Copy_(const ProgVar& cp) {
  var_type = cp.var_type;
  int_val = cp.int_val;
  real_val = cp.real_val;
  string_val = cp.string_val;
  bool_val = cp.bool_val;
  object_type = cp.object_type;
  object_val = cp.object_val;
  hard_enum_type = cp.hard_enum_type;
  dyn_enum_val = cp.dyn_enum_val;
  objs_ptr = cp.objs_ptr;
  flags = cp.flags;
  reference = cp.reference;
  desc = cp.desc;
  init_from = cp.init_from;

  if(var_type == T_Object) {
    if((bool)object_val) {
      // note that updatepointers will reset to null if not found, so it is key
      // to call the appropriate one based on owner of object being pointed to
      if(object_val->GetOwner(&TA_Program) == cp.GetOwner(&TA_Program))
        UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Program); // only look in program
      else
        UpdatePointers_NewPar_IfParNotCp(&cp, &TA_taProject); // only look outside of program
    }
  }
  SetFlagsByOwnership();
  if(objs_ptr) {
    Program* myprg = (Program*)GetOwner(&TA_Program);
    Program* othprg = (Program*)cp.GetOwner(&TA_Program);
    if(myprg == othprg)
      objs_ptr = false; // if in same program, then it is a duplicate and cannot be objs_ptr
  }
}

bool ProgVar::CheckUndefType(const String& function_context) const {
  if(TestError(var_type == T_UnDef, function_context, "Program variable type is undefined -- you must pick an appropriate data type for the variable to hold the information it needs to hold")) {
    return true;
  }
  return false;
}

void ProgVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl(); // this will make it a legal C name
  if(Program::IsForbiddenName(name)) {
    name = "My" + name;
  }
  CheckUndefType("UpdateAfterEdit");
  if(object_val.ptr() == this)  // this would be bad..
    object_val.set(NULL);
  // only send stale if the schema changed, not just the value
  String tfs = GetSchemaSig();
  // loading is a special case: initialize
  if(taMisc::is_loading) {
    taVersion v512(5, 1, 2);
    if(taMisc::loading_version < v512) { // everything prior to 5.1.2 had save val on by default
      SetVarFlag(SAVE_VAL);
    }
    m_prev_sig = tfs;
    m_this_sig = tfs;
  }
  else {
    m_prev_sig = m_this_sig;
    m_this_sig = tfs;
    if (m_prev_sig != m_this_sig) {
      setStale();
    }
  }
  SetFlagsByOwnership();
  UpdateUsedFlag();
  GetInitFromVar(true);         // warn
  TestError(HasVarFlag(LOCAL_VAR) && var_type == T_HardEnum, "UpdateAfterEdit",
              "Hard-coded (C++) enum's are not supported for local variables -- please use an int or String variable instead.");

  UpdateCssObjVal();
}

bool ProgVar::UpdateCssObjVal() {
  if(var_type != T_Object || HasVarFlag(LOCAL_VAR) || css_idx < 0)
    return false;

  Program* myprg = (Program*)GetOwner(&TA_Program);
  if(!myprg || !myprg->script || myprg->script->prog_vars.size <= css_idx) {
    css_idx = -1;
    return false;
  }

  cssEl* cssvar = myprg->script->prog_vars[css_idx];
  if(cssvar->GetType() != cssEl::T_TA) {
    css_idx = -1;
    return false;
  }
  cssSmartRef* sr = (cssSmartRef*)cssvar;
  if(sr->ptr != &object_val) {
    css_idx = -1;
    return false;
  }
  sr->UpdateCssRef();		// finally got our guy -- update it
  return true;
}


ProgVar* ProgVar::GetInitFromVar(bool warn) {
  if(!(bool)init_from) return NULL;
  ProgVar* ivar = init_from->FindVarName(name); // use our name
  TestWarning(warn && !ivar, "GetInitFromVar", "variable with my name:",name,
              "in init_from program:",init_from->name,"not found.");
  SetVarFlag(CTRL_READ_ONLY);
  return ivar;
}

void ProgVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if (prg) prognm = prg->name;
  CheckError(Program::IsForbiddenName(name, false), quiet, rval,
	     "Name:",name,"is a css reserved name used for something else -- please choose another name");
  CheckError(var_type == T_UnDef, quiet, rval,
	     "Program variable type is undefined -- you must pick an appropriate data type for the variable to hold the information it needs to hold");

  if(var_type == T_Object) {
    if(!HasVarFlag(LOCAL_VAR) && HasVarFlag(NULL_CHECK) && !object_val) {
      if(!quiet) taMisc::CheckError("Error in ProgVar in program:", prognm, "var name:",name,
                                    "object pointer is NULL");
      rval = false;
    }
    if(object_type) {
      if(!HasVarFlag(QUIET)) {
        TestWarning(!objs_ptr && !HasVarFlag(LOCAL_VAR) && object_type->InheritsFrom(&TA_taMatrix),
                    "ProgVar", "for Matrix* ProgVar named:",name,
                    "Matrix pointers should be located in LocalVars within the code, not in the global vars/args section, in order to properly manage the reference counting of matrix objects returned from various functions.");
      }
    }
  }
  GetInitFromVar(true);         // warn
}

void ProgVar::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  //  todo: make this conditional on some "obsessive" setting either in program
  // or in overall settings.  too many reduntant checks on the same object
  // triggered by this!!
//   if((var_type == T_Object) && (object_val)) {
//     object_val->CheckConfig(quiet, rval);
//   }
}

void ProgVar::GetSelectText(MemberDef* mbr, String xtra_lbl,
    String& full_lbl, String& eff_desc) const
{
  // when do seledit of the data member, use our var name, and desc
  const String& mn = mbr->name;
  if ((mn == "int_val") || (mn == "real_val") || (mn == "string_val") ||
    (mn == "bool_val") || (mn == "object_val") || (mn == "dyn_enum_val"))
  {
    if (xtra_lbl.empty()) { //note: typically is empty
      Program* prog = GET_MY_OWNER(Program);
      if (prog)
        xtra_lbl = prog->GetName().elidedTo(16);
    }
    String lbl = xtra_lbl;
    if (lbl.nonempty()) lbl += "_";
    lbl += GetName().elidedTo(16); 	// var name, not the member name
    full_lbl = taMisc::StringCVar(lbl);
    eff_desc = GetDesc();		// always use our desc, not default
  }
  else { // something else, just do default
    inherited::GetSelectText(mbr, xtra_lbl, full_lbl, eff_desc);
  }
}


void ProgVar::SetParseCssEl() {
  if(!parse_css_el) {
    parse_css_el = NewCssEl();
    cssEl::Ref(parse_css_el);
  }
}

void ProgVar::FreeParseCssEl() {
  if(parse_css_el) {
    cssEl::unRefDone(parse_css_el);
    parse_css_el = NULL;
  }
}

void ProgVar::ResetParseStuff() {
  FreeParseCssEl();
}

TypeDef* ProgVar::act_object_type() const {
  TypeDef* rval = &TA_taBase; // the min return value
  if (object_type) {
    if (object_val) rval = object_val->GetTypeDef();
    else rval = object_type;
  }
  return rval;
}

MemberDef* ProgVar::GetValMemberDef() {
  if(var_type == T_Int)
    return TA_ProgVar.members.FindName("int_val");
  else if(var_type == T_Real)
    return TA_ProgVar.members.FindName("real_val");
  else if(var_type == T_String)
    return TA_ProgVar.members.FindName("string_val");
  else if(var_type == T_Bool)
    return TA_ProgVar.members.FindName("bool_val");
  else if(var_type == T_Object)
    return TA_ProgVar.members.FindName("object_val");
  else if(var_type == T_HardEnum)
    return TA_ProgVar.members.FindName("hard_enum_type");
  else if(var_type == T_DynEnum)
    return TA_ProgVar.members.FindName("dyn_enum_val");
  return NULL;
}

bool ProgVar::schemaChanged() {
  return (m_this_sig != m_prev_sig);
}

void ProgVar::SetInt(int val) {
  var_type = T_Int;
  int_val = val;
}

void ProgVar::SetReal(double val) {
  var_type = T_Real;
  real_val = val;
}

void ProgVar::SetString(const String& val) {
  var_type = T_String;
  string_val = val;
}

void ProgVar::SetBool(bool val) {
  var_type = T_Bool;
  bool_val = val;
}

void ProgVar::SetObject(taBase* val) {
  var_type = T_Object;
  object_val = val;
  if(object_val) object_type = object_val->GetTypeDef();
}

void ProgVar::SetObjectType(TypeDef* td) {
  var_type = T_Object;
  if(object_type != td) {
    object_type = td;
    object_val = NULL;
  }
  UpdateAfterEdit();
}

void ProgVar::SetHardEnum(TypeDef* enum_type, int val) {
  var_type = T_HardEnum;
  int_val = val;
  hard_enum_type = enum_type;
}

void ProgVar::SetDynEnum(int val) {
  var_type = T_DynEnum;
  dyn_enum_val.SetNumVal(val);
}

void ProgVar::SetDynEnumName(const String& val) {
  var_type = T_DynEnum;
  dyn_enum_val.SetNameVal(val);
}

void ProgVar::SetVar(const Variant& value) {
  switch(var_type) {
  case T_Int:
    int_val = value.toInt();
    break;
  case T_Real:
    real_val = value.toDouble();
    break;
  case T_String:
    string_val = value.toString();
    break;
  case T_Bool:
    bool_val = value.toBool();
    break;
  case T_Object:
    object_val = value.toBase();
    UpdateCssObjVal();		// need to update our css object!
    break;
  case T_HardEnum:
    int_val = value.toInt();
    break;
  case T_DynEnum:
    if(value.isStringType())
      dyn_enum_val.SetNameVal(value.toString());
    else
      dyn_enum_val.SetNumVal(value.toInt());
    break;
  case T_UnDef:
    CheckUndefType("SetVar");
    break;
  }
}

Variant ProgVar::GetVar() {
  switch(var_type) {
  case T_Int:
    return Variant(int_val);
    break;
  case T_Real:
    return Variant(real_val);
    break;
  case T_String:
    return Variant(string_val);
    break;
  case T_Bool:
    return Variant(bool_val);
    break;
  case T_Object:
    return Variant(object_val.ptr());
    break;
  case T_HardEnum:
    return Variant(int_val);
    break;
  case T_DynEnum:
    return dyn_enum_val.NumVal();
    break;
  case T_UnDef:
    CheckUndefType("GetVar");
    break;
  }
  return _nilVariant;// compiler food
}

bool ProgVar::SetValStr(const String& val, void* par, MemberDef* memb_def,
                        TypeDef::StrContext sc, bool force_inline) {
  // taMisc::DebugInfo("setting progvar to string:", val);
  if(val.contains("{")) {
    return inherited::SetValStr(val, par, memb_def, sc, force_inline);
  }
  SetVar((Variant)val);
  return true;
}

void ProgVar::Cleanup() {
  if (!((var_type == T_Int) || (var_type == T_HardEnum)))
    int_val = 0;
  if (var_type != T_Real)  real_val = 0.0;
  if (var_type != T_String)  string_val = _nilString;
  if (var_type != T_Bool)  bool_val = false;
  if (var_type != T_Object) {
    //note: its ok to leave whatever type is there
    object_val.CutLinks();
  }
  if (var_type != T_HardEnum) {
    hard_enum_type = NULL;
  }
  //TODO: anything about DynEnums???
}

void ProgVar::SigEmit(int sls, void* op1, void* op2) {
  // dynenum is programmed to send us notifies, we trap those and
  // turn them into changes of us, to force gui to update (esp enum list)
  if ((sls == SLS_CHILD_ITEM_UPDATED) && (op1 == &dyn_enum_val)) {
    SigEmitUpdated();
    return; // don't send any further
  }
  inherited::SigEmit(sls, op1, op2);
}

String ProgVar::GetDisplayName() const {
  String rval;
  switch(var_type) {
  case T_Int:
    rval = name + " = " + String(int_val) + " (int)";
    break;
  case T_Real:
    rval = name + " = " + String(real_val) + " (real)";
    break;
  case T_String:
    rval = name + " = " + string_val + " (String)";
    break;
  case T_Bool:
    rval = name + " = " + ((bool_val ? "true" : "false")) + " (bool)";
    break;
  case T_Object:
    if(!object_type) rval = name + " = NULL object type";
    else rval = name + " = " + ((object_val ? object_val->GetDisplayName() : "NULL"))
           + " (" + object_type->name + ")";
    break;
  case T_HardEnum:
    if(!hard_enum_type) rval = name + " = NULL hard enum type";
    else {
      bool show_scope = false;
      rval = name + " = "
        + hard_enum_type->Get_C_EnumString(int_val, show_scope)
        + " (" + hard_enum_type->name + ")";
    }
    break;
  case T_DynEnum:
    if((bool)dyn_enum_val.enum_type)
      rval = name + " = " + dyn_enum_val.NameVal() + " (" + dyn_enum_val.enum_type->name + ")";
    else
      rval = name + " = " + dyn_enum_val.NameVal() + " (no dyn enum type!)";
    break;
  case T_UnDef:
    rval = name + " undefined type!";
    break;
  default:
    rval = name + " invalid type!";
    break;
  }
  if(init_from) {
    rval += "  --  init from: " + init_from->name;
  }
  return rval;
}

String ProgVar::GetSchemaSig() const {
  STRING_BUF(rval, 125);
  // NOTE: the sig is not 100% optimized, but we keep it simple...
  // sig is following: name, type, obj_type, enum_type
  // note that dyn_enum are just fancy int generators, so don't factor in
  rval.cat(name).cat(";").cat(var_type).cat(";")
   .cat((object_type) ? object_type->name : "").cat(";")
   .cat((hard_enum_type) ? hard_enum_type->name : "");
  return rval;
}

taBase::DumpQueryResult ProgVar::Dump_QuerySaveMember(MemberDef* md) {
  DumpQueryResult rval = DQR_SAVE; // only used for membs we match below
  if (md->name == "int_val")
    rval = ((var_type == T_Int) || (var_type == T_HardEnum)) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "real_val")
    rval = (var_type == T_Real) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "string_val")
    rval = (var_type == T_String) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "bool_val")
    rval = (var_type == T_Bool) ? DQR_SAVE : DQR_NO_SAVE;
  else if ((md->name == "object_type") || (md->name == "object_val"))
    rval = (var_type == T_Object) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "hard_enum_type")
    rval = (var_type == T_HardEnum) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "dyn_enum_val")
    rval = (var_type == T_DynEnum) ? DQR_SAVE : DQR_NO_SAVE;
  else
    return inherited::Dump_QuerySaveMember(md);

  if(!HasVarFlag(LOCAL_VAR) && !HasVarFlag(SAVE_VAL)) rval = DQR_NO_SAVE; // always save LOCAL_VAR's because they are initializers
  return rval;
}

const String ProgVar::GenCss(bool is_arg) {
  UpdateAfterEdit();            // update used and other flags
  return is_arg ? GenCssArg_impl() : GenCssVar_impl() ;
}

const String ProgVar::GenListing(bool is_arg, int indent_level) {
  if(is_arg) {
    return GenCssArg_impl();
  }
  String rval = Program::GetDescString(desc, indent_level);
  rval += cssMisc::Indent(indent_level) + GetDisplayName() + "\n";
  return rval;
}

const String ProgVar::GenCssType() const {
  switch(var_type) {
  case T_Int:
    return "int";
  case T_Real:
    return "double";
  case T_String:
    return "String";
  case T_Bool:
    return "bool";
  case T_Object:
    if(object_val)
      return object_val->GetTypeDef()->name + "*";
    else
      return object_type->name + "*";
  case T_HardEnum:
    if(hard_enum_type) {
      // Show scope of the enumerated value, if possible.
      if (TypeDef* par_td = hard_enum_type->GetOwnerType()) {
        return par_td->GetPathName() + "::" + hard_enum_type->name;
      }
      else {
        return hard_enum_type->name;
      }
    }
    else {
      return "int";
    }
  case T_DynEnum:
    if(dyn_enum_val.enum_type) {
      return dyn_enum_val.enum_type->name;
    }
    return "int";
  case T_UnDef:
    return "void";
    break;
  }
  return "";
}

const String ProgVar::GenCssInitVal() const {
  switch(var_type) {
  case T_Int:
    return int_val;
  case T_Real:
    return real_val;
  case T_String:
    return "\"" + string_val + "\"";
  case T_Bool:
    return bool_val;
  case T_Object: {
    if(object_val) {
      return object_val->GetPath();
    }
    else {
      if(object_type && HasVarFlag(NEW_OBJ)) {
	return "new " + object_type->name;
      }
      else {
	return "NULL";
      }
    }
    break;
  }
  case T_HardEnum:
    if(hard_enum_type)
      return hard_enum_type->Get_C_EnumString(int_val);
    else
      return int_val;
  case T_DynEnum:
    return dyn_enum_val.NameVal();
  case T_UnDef:
    return "";
    break;
  }
  return "";
}

// note: *never* initialize variables because they are cptrs to actual current
// value in object..
const String ProgVar::GenCssArg_impl() {
  String rval;
  rval += GenCssType();
  if(reference)
    rval += "&";
  rval += " ";
  rval += name;
  return rval;
}

const String ProgVar::GenCssVar_impl() {
  String rval;
  rval += GenCssType() + " ";
  rval += name;
  rval += ";  ";
  if(HasVarFlag(LOCAL_VAR)) {
    rval += name + " = " + GenCssInitVal() + ";";
  }
  return rval;
}

Program* ProgVar::GetInitFromProg() {
  TestError(!init_from, "GetInitFromProg", "init_from program is NULL for initializing variable:", name, "in program:", program()->name);
  return init_from.ptr();
}


void ProgVar::GenCssInitFrom(Program* prog) {
  ProgVar* ivar = GetInitFromVar(false); // no warning now
  if(!ivar) return;
  prog->AddLine(this, "{ // init_from");
  prog->IncIndent();
  prog->AddLine(this, String("Program* init_fm_prog = this").cat(GetPath(NULL, program())).cat("->GetInitFromProg();"));
  prog->AddLine(this, String(name + " = init_fm_prog->GetVar(\"").cat(name).cat("\");\n"));
  prog->DecIndent();
  prog->AddLine(this, "}");
}

cssEl* ProgVar::NewCssEl() {
  switch(var_type) {
  case T_Int:
    return new cssCPtr_int(&int_val, 0, name);
    break;
  case T_Real:
    return new cssCPtr_double(&real_val, 0, name);
    break;
  case T_String:
    return new cssCPtr_String(&string_val, 0, name);
    break;
  case T_Bool:
    return new cssCPtr_bool(&bool_val, 0, name);
    break;
  case T_Object:
    return new cssSmartRef(&object_val, 0, TypeDef::FindGlobalTypeName("taBaseRef"), name);
    break;
  case T_HardEnum:
    return new cssCPtr_enum(&int_val, 0, name, hard_enum_type);
    break;
  case T_DynEnum:
    return new cssCPtr_DynEnum(&dyn_enum_val, 0, name);
    break;
  case T_UnDef:
    return new cssCPtr_int(&int_val, 0, name);
    break;
  }
  return &cssMisc::Void;
}

String ProgVar::CodeGetDesc(const String& code) {
  if(code.contains("//")) {
    desc = trim(code.after("//"));
    return trim(code.before("//"));
  }
  if(code.contains("/*")) {
    desc = trim(code.after("/*"));
    if(desc.contains("*/"))
      desc = trim(desc.before("*/",-1));
    return trim(code.before("/*"));
  }
  return code;
}

bool ProgVar::BrowserEditSet(const String& code, int move_after) {
  String cd = CodeGetDesc(code);
  return true;
}


bool ProgVar::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgVar::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgVar::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}
