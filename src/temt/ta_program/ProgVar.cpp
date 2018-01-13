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

#include "ProgVar.h"
#include <Program>
#include <ProgVar_List>
#include <taSigLinkItr>
#include <MemberDef>
#include <EnumDef>

taTypeDef_Of(taMatrix);
taTypeDef_Of(Function);
taTypeDef_Of(taProject);
taTypeDef_Of(Loop);

#include <SigLinkSignal>
#include <ProgElChoiceDlg>
#include <ControlPanel>
#include <taProject>
#include <FlatTreeEl_List>
#include <Completions>
#include <ProgExprBase>

#include <taMisc>
#include <tabMisc>
#include <taiMisc>

#include <css_machine.h>
#include <css_ta.h>
#include <css_c_ptr_types.h>

TA_BASEFUNS_CTORS_DEFN(ProgVar);

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
  inherited::InitLinks();
  taBase::Own(object_val, this);
  taBase::Own(object_scope, this);
  taBase::Own(dyn_enum_val, this);
  taBase::Own(init_from, this);
  if(!taMisc::is_loading) {
    SetFlagsByOwnership();
  }
}

void ProgVar::CutLinks() {
  FreeParseCssEl();
  object_val.CutLinks();
  object_scope.CutLinks();
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
    ClearVarFlag(PGRM_ARG);
    Program* myprg = (Program*)GetOwner(&TA_Program);
    if(this->owner == &(myprg->args)) {
      SetVarFlag(PGRM_ARG);
      if(init_from) {
        init_from.set(NULL);        // cannot have an init_from!
        SetVarFlag(CTRL_PANEL);
        ClearVarFlag(CTRL_READ_ONLY);
      }
    }
    if(var_type == T_Object) {
      if(objs_ptr) {
        SetVarFlag(SAVE_VAL);   // all objs_ptr vars MUST save
      }
      else { // !objs_ptr
        if(!HasVarFlag(SAVE_VAL) && HasVarFlag(NULL_CHECK)) {
          if(!HasVarFlag(QUIET)) {
            TestWarning(true, "ProgVar", "for Object* ProgVar named:",name,
                        "has NULL_CHECK on but NOT SAVE_VAL -- this means that this variable WILL BE NULL when the program is loaded and you'll likely get an error message to this effect -- change one or the other of these settings to get rid of it.");
          }
        }
      }
    }
  }
  else {
    objs_ptr = false;           // this is incompatible with being local
    SetVarFlag(LOCAL_VAR);
    ClearVarFlag(SAVE_VAL);
    ClearVarFlag(PGRM_ARG);
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

bool ProgVar::UpdateUsedFlag_gui() {
  bool was_used = HasVarFlag(USED);
  bool rval = UpdateUsedFlag();
  if(was_used != HasVarFlag(USED)) {
    SigEmitUpdated();
  }
  return rval;
}

int ProgVar::GetEnabled() const {
  return HasVarFlag(USED);
}

int ProgVar::GetSpecialState() const {
  if(IsLocal()) return 0;
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
  object_scope = cp.object_scope;
  hard_enum_type = cp.hard_enum_type;
  dyn_enum_val = cp.dyn_enum_val;
  objs_ptr = cp.objs_ptr;
  flags = cp.flags;
  reference = cp.reference;
  desc = cp.desc;
  init_from = cp.init_from;
  
  objs_ptr = false; // only the original var can have special relationship with object
  object_val.set(cp.object_val.ptr());
  SetFlagsByOwnership();
}

bool ProgVar::CheckUndefType(const String& function_context, bool quiet) const {
  if (quiet) {
    return (var_type == T_UnDef);
  }
  else {
    if(TestError(var_type == T_UnDef, function_context, "Program variable type is undefined -- you must pick an appropriate data type for the variable to hold the information it needs to hold")) {
      return true;
    }
    return false;
  }
}

void ProgVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl(); // this will make it a legal C name

  Cleanup();                    // cleanup irrelevant variable values
  
  if(Program::IsForbiddenName(this, name)) {
    name = "My" + name;
  }

  // in cases like AssignTo we don't pop the choice dialog
  Program* pgrm = GET_MY_OWNER(Program);
  if(!pgrm) return;
  taProject* proj = pgrm->GetMyProj();
  if (proj && proj->no_dialogs) {
    if (CheckUndefType("UpdateAfterEdit", true)) {
      String var_nm;
      ProgElChoiceDlg dlg;
      taBase::Ref(dlg);
      int choice = 1;  // global
      if (this->flags | LOCAL_VAR) {
        choice = 0;
      }
      ProgVar::VarType vt = ProgVar::T_UnDef;
      int result = dlg.GetLocalGlobalChoice(var_nm, choice, vt);  // get the name and the type
      if (result == 1) {
        this->SetName(var_nm);
        this->var_type = vt;
      }
    }
  }
  
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
      // taMisc::DebugInfo("var:", name, "schema changed in UAE");
      setStale();
    }
    // else {
    //   taMisc::DebugInfo("var:", name, "schema NOT changed in UAE");
    // }
  }
  SetFlagsByOwnership();
  UpdateUsedFlag();
  GetInitFromVar(true);         // warn
  TestError(IsLocal() && var_type == T_HardEnum, "UpdateAfterEdit",
              "Hard-coded (C++) enum's are not supported for local variables -- please use an int or String variable instead.");

  UpdateCssObjVal();

  if(HasVarFlag(FUN_ARG)) {
    taBase* fun = owner->GetOwner();
    fun->SigEmitUpdated();
  }

  if(!taMisc::is_loading && !IsLocal()) {              // global name check
    ProgVar* othvar = pgrm->GlobalVarDupeCheck(this);
    if(othvar) {
      taMisc::Confirm("Duplicate global variable names found in program:",pgrm->name,
                      "Please rename one or the other of them!  Name:", name,
                      "one:", this->DisplayPath(), "other:", othvar->DisplayPath());
    }
  }
}

bool ProgVar::UpdateCssObjVal() {
  if(var_type != T_Object || IsLocal() || css_idx < 0)
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
  CheckError(Program::IsForbiddenName(this, name, false), quiet, rval,
             "Name:",name,"is a css reserved name used for something else -- please choose another name");
  
  CheckError(var_type == T_UnDef, quiet, rval,
             "Program variable type is undefined -- you must pick an appropriate data type for the variable to hold the information it needs to hold");
  
  if(var_type == T_Object && !IsLocal() && HasVarFlag(NULL_CHECK)) {
    CheckError(!object_val, quiet, rval,
               "Error in ProgVar in program:", prognm, "var name:",name, "object pointer is NULL");
  }
  
  if(var_type == T_DynEnum && prg) {
    if(!dyn_enum_val.enum_type) {
      CheckError(dyn_enum_val.enum_type, quiet, rval,
                 "Error in ProgVar in program:", prognm, "var name:",name,
                 "enum_type is not set for this dynamic enum value");
    }
    else {
      Program* dtprg = GET_OWNER(dyn_enum_val.enum_type, Program);
      CheckError((!dtprg || prg != dtprg), quiet, rval,
                 "Error in ProgVar in program:", prognm, "var name:",name, "enum_type is in a different Program:",
                 dtprg->name, "than this variable");
    }
  }
  
  GetInitFromVar(true);         // warn
}

bool ProgVar::CheckMatrixAssignFmMethod(bool quiet, bool& rval, MethodDef* meth, ProgEl* pel) {
  if(HasVarFlag(QUIET) || HasVarFlag(NEW_OBJ)) return false;
  if(var_type != ProgVar::T_Object) return false;
  if(!object_type || !object_type->InheritsFrom(&TA_taMatrix))
    return false;
  if(!(meth->name.startsWith("GetValAsMatrix") || meth->name.startsWith("GetRangeAsMatrix")))
     return false;
  // we have a suspect, now check for local vars
  if(!IsLocal()) {
    CheckError(true, quiet, rval,
               "ProgVar", "for Matrix* ProgVar named:",name,
               "being assigned a temporary matrix from GetValAsMatrix -- these Matrix pointer variables MUST be located in LocalVars within the code, within the proper looping scope where the variable is re-used, NOT in the global vars/args section, in order to properly manage the reference counting of matrix objects returned from various functions.");
    return true;
  }

  taBase* pel_loop = pel->GetOwnerOfType(&TA_Loop);
  if(!pel_loop) return false;   // if not in a loop, then one-time and we don't care!
  taBase* var_loop = this->GetOwnerOfType(&TA_Loop);
  if(pel_loop != var_loop) {    // now we have a problem
    CheckError(true, quiet, rval,
               "ProgVar", "for Matrix* ProgVar named:",name,
               "being assigned a temporary matrix from GetValAsMatrix -- these Matrix pointer variables MUST be located WITHIN the same loop (for, while etc) that the GetValAsMatrix call is in, in order to properly manage the reference counting of matrix objects returned from various functions.");
    return true;
  }
  return false;
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

void ProgVar::GetControlPanelLabel(MemberDef* mbr, String& label, const String& xtra_lbl, bool short_label) const
{
  const String& mn = mbr->name;
  if ((mn == "int_val") || (mn == "real_val") || (mn == "string_val") ||
      (mn == "bool_val") || (mn == "object_val") || (mn == "value")) // value is the dyn_enum case
  {
    if(xtra_lbl.nonempty()) {
      label = xtra_lbl + "_";
    }
    else if(short_label) {
      label = "";
    }
    else {
      Program* prog = GET_MY_OWNER(Program);
      if (prog) {
        label = prog->GetName().CamelToSnake().elidedTo(taiMisc::CP_ITEM_ELIDE_LENGTH_LONG);
      }
      if (label.nonempty()) {
        label += "_";
      }
    }
    label += GetName().CamelToSnake();
    // always do the full variable name!
    // .elidedTo(taiMisc::CP_ITEM_ELIDE_LENGTH_SHORT); // var name, not the member name
    label = taMisc::StringCVar(label);
  }
  else { // something else, just do default
    inherited::GetControlPanelLabel(mbr, label, xtra_lbl, short_label);
  }
}

void ProgVar::GetControlPanelDesc(MemberDef* mbr, String& eff_desc) const {
  const String& mn = mbr->name;
  if ((mn == "int_val") || (mn == "real_val") || (mn == "string_val") ||
      (mn == "bool_val") || (mn == "object_val") || (mn == "value")) // value is the dyn_enum case
  {
    eff_desc = GetDesc();       // always use our desc, not default
  }
  else { // something else, just do default
    inherited::GetControlPanelDesc(mbr, eff_desc);
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
  // this is called by expressions after they parse us as a variable
  FreeParseCssEl();
  UpdateUsedFlag_gui();
}

TypeDef* ProgVar::act_object_type() const {
  TypeDef* rval = &TA_taBase; // the min return value

  if(var_type == T_String)
    return &TA_taString;
  else if(var_type == T_DynEnum)
    return &TA_DynEnum;
  else if(var_type == T_Object) {
    if (object_type) {
      if (object_val) rval = object_val->GetTypeDef();
      else rval = object_type;
    }
  }
  return rval;
}

MemberDef* ProgVar::GetValMemberDef() const {
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
    return TA_ProgVar.members.FindName("int_val");
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

void ProgVar::SetHardEnumName(TypeDef* enum_type, const String& str_val) {
  var_type = T_HardEnum;
  hard_enum_type = enum_type;
  // this version deals with BITS properly:
  hard_enum_type->SetValStr_enum(str_val, (void*)&int_val, hard_enum_type);
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
  case T_Object: {
    taBase* bs = value.toBase();
    if(!bs) {
      if(value.isStringType()) {
        if(value.toString() == "NULL" || value.toString() == "null" ||
           value.toString() == "0")
          object_val = bs;
      }
    }
    else {
      object_val = bs;
    }
    UpdateCssObjVal();		// need to update our css object!
    break;
  }
  case T_HardEnum:
    if(value.isStringType() && hard_enum_type) {
      EnumDef* ed = hard_enum_type->FindEnum(value.toString());
      if(ed)
        int_val = ed->enum_no;
    }
    else {
      int_val = value.toInt();
    }
    break;
  case T_DynEnum:
    if(value.isStringType())
      dyn_enum_val.SetNameVal(value.toString());
    else
      dyn_enum_val.SetNumVal(value.toInt());
    break;
  case T_UnDef: {
    switch(value.type()) {
    case Variant::T_Bool:
      var_type = T_Bool;
      break;
    case Variant::T_Int:
    case Variant::T_UInt:
    case Variant::T_Int64:
    case Variant::T_UInt64:
      var_type = T_Int;
      break;
    case Variant::T_Double:
    case Variant::T_Float:
      var_type = T_Real;
      break;
    case Variant::T_Char:
      var_type = T_String;
      break;
    case Variant::T_String:
      var_type = T_String;
      break;
    case Variant::T_Base:
    case Variant::T_Matrix:
      var_type = T_Object;
      break;
    default: // compiler food
      break;
    }
    if(var_type != T_UnDef) {
      SetVar(value);            // reset
      if(var_type == T_Object && object_val) { // init type..
        object_type = object_val->GetTypeDef();
      }
    }
    CheckUndefType("SetVar");
    break;
  }
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
  SetValFromString(val);
  return true;
}

void ProgVar::SetValFromString(const String& str_val) {
  switch(var_type) {
  case T_Int:
    int_val = (int)(double)str_val; // allow conversion from float to int to work properly
    break;
  case T_Real:
    real_val = (double)str_val;
    break;
  case T_String:
    string_val = str_val;
    break;
  case T_Bool:
    bool_val = str_val.toBool();
    break;
  case T_Object: {
    if(str_val == "NULL" || str_val == "null" || str_val == "0") {
      object_val = NULL;
    }
    else {
      Variant var = (Variant)str_val;
      object_val = var.toBase();
    }
    UpdateCssObjVal();		// need to update our css object!
    break;
  }
  case T_HardEnum:
    if(hard_enum_type) {
      EnumDef* ed = hard_enum_type->FindEnum(str_val);
      if(ed)
        int_val = ed->enum_no;
    }
    else {
      int_val = (int)str_val;
    }
    break;
  case T_DynEnum:
    dyn_enum_val.SetNameVal(str_val);
    break;
  case T_UnDef:
    CheckUndefType("SetVar");
    break;
  }
}

String ProgVar::GetStringVal() {
  if(var_type == T_HardEnum && hard_enum_type) {
    String rval = hard_enum_type->Get_C_EnumString(int_val, false);
    return rval;
  }
  else if(var_type == T_DynEnum) {
    String nv = dyn_enum_val.NameVal();
    if(nv.nonempty()) return nv;
  }
  else if(var_type == T_Object) {
    if(object_val) {
      return object_val->GetPathFromProj();
    }
    return "NULL";
  }
  return GetVar().toString();
}


void ProgVar::Cleanup() {
  if (!((var_type == T_Int) || (var_type == T_HardEnum)))
    int_val = 0;
  if (var_type != T_Real)  real_val = 0.0;
  if (var_type != T_String)  string_val = _nilString;
  if (var_type != T_Bool)  bool_val = false;
  if (var_type != T_Object) {
    //note: its ok to leave whatever type is there
    object_val.set(NULL);
    object_scope.set(NULL);
  }
  if (var_type != T_HardEnum) {
    hard_enum_type = NULL;
  }
  if (var_type != T_DynEnum) {
    dyn_enum_val.enum_type = NULL;
    dyn_enum_val.value = 0;
  }
}

void ProgVar::ToggleSaveVal() {
  if (HasVarFlag(SAVE_VAL)) {
    ClearVarFlag(SAVE_VAL);
  }
  else {
    SetVarFlag(SAVE_VAL);
  }
  SigEmitUpdated();
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
    if(!object_type) rval = name + " = (NULL object type)";
    else rval = name + " = " + ((object_val ? object_val->GetDisplayName() : "NULL"))
           + " (" + object_type->name + ")";
    break;
  case T_HardEnum:
    if(!hard_enum_type) rval = name + " = (NULL hard enum type)";
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
      rval = name + " = " + dyn_enum_val.NameVal() + " (NULL dyn enum type)";
    break;
  case T_UnDef:
    rval = name + " = (undefined type)";
    break;
  default:
    rval = name + " = (invalid type)";
    break;
  }
  if(HasVarFlag(FUN_ARG) && reference) {
    rval = rval.before(')',-1) + "&)";
  }
  if(object_type && HasVarFlag(NEW_OBJ)) {
    rval += " -- new";
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
  bool always_save = false;
  if (md->name == "int_val") {
    rval = ((var_type == T_Int) || (var_type == T_HardEnum)) ? DQR_SAVE : DQR_NO_SAVE;
  }
  else if (md->name == "real_val") {
    rval = (var_type == T_Real) ? DQR_SAVE : DQR_NO_SAVE;
  }
  else if (md->name == "string_val") {
    rval = (var_type == T_String) ? DQR_SAVE : DQR_NO_SAVE;
  }
  else if (md->name == "bool_val") {
    rval = (var_type == T_Bool) ? DQR_SAVE : DQR_NO_SAVE;
  }
  else if ((md->name == "object_type")) {
    always_save = true;
    rval = (var_type == T_Object) ? DQR_SAVE : DQR_NO_SAVE;
  }
  else if ((md->name == "objs_ptr")) {
    always_save = true;
    rval = (var_type == T_Object) ? DQR_SAVE : DQR_NO_SAVE;
  }
  else if ((md->name == "object_val")) {
    rval = (var_type == T_Object) ? DQR_SAVE : DQR_NO_SAVE;
  }
  else if (md->name == "hard_enum_type") {
    always_save = true;
    rval = (var_type == T_HardEnum) ? DQR_SAVE : DQR_NO_SAVE;
  }
  else if (md->name == "dyn_enum_val") {
    always_save = true;         // yes -- dyn enum type is in there and must be saved
    rval = (var_type == T_DynEnum) ? DQR_SAVE : DQR_NO_SAVE;
  }
  else {
    return inherited::Dump_QuerySaveMember(md);
  }

  // override saving based on SAVE_VAL -- only if not always_save flag on, and
  // always save LOCAL_VAR's because they are initializers
  if(!HasVarFlag(SAVE_VAL) && !always_save && !IsLocal()) {
    rval = DQR_NO_SAVE;
  }
  return rval;
}

const String ProgVar::GenCss(bool is_arg) {
  if(is_arg) {
    // UpdateAfterEdit();   // this is called in parent css gen process
    return GenCssArg_impl();
  }
  UpdateAfterEdit();            // update flags etc
  return GenCssVar_impl() ;
}

const String ProgVar::GenListing(bool is_arg, int indent_level) const {
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
    if(object_val) {
      return object_val->GetTypeDef()->name + "*";
    }
    else {
      if((var_type == ProgVar::T_Object) && object_type->InheritsFrom(&TA_ios)) {
        return object_type->name;
      }
      else {
        return object_type->name + "*";
      }
    }
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
  case T_DynEnum: {
    String nv = dyn_enum_val.NameVal();
    if(nv.nonempty()) return nv;
    return "0";
  }
  case T_UnDef:
    return "";
    break;
  }
  return "0";                   // usually bad to return blank..
}

// note: *never* initialize variables because they are cptrs to actual current
// value in object..
const String ProgVar::GenCssArg_impl() const {
  String rval;
  rval += GenCssType();
  if(reference)
    rval += "&";
  rval += " ";
  rval += name;
  return rval;
}

const String ProgVar::GenCssVar_impl() const {
  String rval;
  rval += GenCssType() + " ";
  rval += name;
  rval += ";  ";
  if(IsLocal()) {
    if(!((var_type == ProgVar::T_Object) && object_type->InheritsFrom(&TA_ios))) {
      rval += name + " = " + GenCssInitVal() + ";";
    }
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
  prog->AddLine(this, String("Program* init_fm_prog = this").cat(GetPath(program())).cat("->GetInitFromProg();"));
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

TypeDef* ProgVar::GetOurTypeDef() const {
  switch(var_type) {
  case T_Int:
    return &TA_int;
  case T_Real:
    return &TA_double;
  case T_String:
    return &TA_taString;
  case T_Bool:
    return &TA_bool;
  case T_Object:
    return object_type;
  case T_HardEnum:
    return hard_enum_type;
  case T_DynEnum:
    return &TA_DynEnum;
  case T_UnDef:
    return &TA_void;
  }
  return &TA_void;
}

ProgVar::VarType ProgVar::GetTypeFromTypeDef(TypeDef* td) {
  if(td == NULL) return T_UnDef;
  if(td->IsBool()) return T_Bool;
  if(td->IsInt()) return T_Int;
  if(td->IsEnum()) return T_HardEnum;
  if(td->IsFloat()) return T_Real;
  if(td->IsString()) return T_String;
  return T_Object;              // must be..
}

TypeDef* ProgVar::GetTypeDefFromString(const String& tstr, bool& ref) {
  String vtype = tstr;
  ref = false;
  if(vtype.endsWith("&")) {
    vtype = vtype.before("&");
    ref = true;
  }
  if(vtype.endsWith("*")) vtype = vtype.before("*");
  if(vtype == "String") vtype = "taString";
  if(vtype == "string") vtype = "taString";
  if(vtype == "real") vtype = "float";
  TypeDef* td = NULL;
  if(vtype.contains("::")) {
    String tn = vtype.before("::");
    td = taMisc::FindTypeName(tn, false); // no err
    if(td) {
      td = td->FindSubType(vtype.after("::"));
    }
  }
  else {
    td = taMisc::FindTypeName(vtype, false); // no err
  }
  return td;
}

bool ProgVar::SetTypeFromTypeDef(TypeDef* td, bool ref) {
  var_type = GetTypeFromTypeDef(td);
  if(var_type == ProgVar::T_Object) {
    object_type = td;
  }
  else if(var_type == ProgVar::T_HardEnum) {
    hard_enum_type = td;
  }
  if(HasVarFlag(FUN_ARG))
    reference = ref;
  return true;
}

bool ProgVar::SetTypeAndName(const String& ty_nm) {
  String vtype = ty_nm.before(' ');
  bool ref;
  TypeDef* td = GetTypeDefFromString(vtype, ref);
  bool updtd = false;
  if(td) {
    SetTypeFromTypeDef(td, ref);
    updtd = true;
  }
  else {
    Program* prg = GET_MY_OWNER(Program);
    if(prg) {
      ProgType* pt = prg->types.FindName(vtype);
      if(pt) {
        var_type = T_DynEnum;
        dyn_enum_val.enum_type = (DynEnumBase*)pt;
      }
    }
  }
  String nm = trim(ty_nm.after(' '));
  if(nm.nonempty()) {
    SetName(nm);
    updtd = true;
  }
  if(updtd) {
    SigEmitUpdated();
  }
  return true;
}

int ProgVar::ReplaceValStr
(const String& srch, const String& repl, const String& mbr_filt,
 void* par, TypeDef* par_typ, MemberDef* memb_def, TypeDef::StrContext sc, bool replace_deep) {
  String cur_val = BrowserEditString(); // current best string rep
  int rval = cur_val.gsub(srch, repl);
  if(rval > 0) {
    taMisc::Info("Replaced string value in ProgVar of type:", GetTypeDef()->name,
                 "now:", cur_val);
    BrowserEditSet(cur_val);
  }
  return rval;
}

FlatTreeEl* ProgVar::GetFlatTree(FlatTreeEl_List& ftl, int nest_lev, FlatTreeEl* par_el,
                                 const taBase* par_obj, MemberDef* md) const {
  FlatTreeEl* fel = NULL;
  if(md) {
    fel = ftl.NewMember(nest_lev, md, par_obj, par_el);
  }
  else {
    fel = ftl.NewObject(nest_lev, this, par_el);
  }
  GetFlatTreeValue(ftl, fel);   // get our value
  // ftl.GetFlatTreeMembers_ListsOnly(fel, this); // nothing!
  return fel;
}

void ProgVar::GetFlatTreeValue(FlatTreeEl_List& ftl, FlatTreeEl* ft, bool ptr) const {
  if(ptr) {
    inherited::GetFlatTreeValue(ftl, ft, ptr);
    return;
  }
  else {
    ft->value = GetDiffString();
  }
}

String ProgVar::GetDiffString() const {
  if(HasVarFlag(SAVE_VAL)) {
    return BrowserEditString();
  }
  String rval = name;
  TypeDef* td = GetOurTypeDef();
  rval += " (" + td->name + ")";
  if(HasVarFlag(FUN_ARG) && reference) {
    rval = rval.before(')',-1) + "&)";
  }
  if(object_type && HasVarFlag(NEW_OBJ)) {
    rval += " -- new";
  }
  if(init_from) {
    rval += "  --  init from: " + init_from->name;
  }
  return rval;
}

bool ProgVar::BrowserEditSet(const String& code, int move_after) {
  String cd = ProgEl::CodeGetDesc(code, desc);
  if(cd.empty()) return false;
  if(cd.contains("undefined")) return false;
  if(cd.contains('(') && cd.contains(')')) {
    // our own output format
    String vtype = cd.between('(', ')');
    bool ref;
    TypeDef* td = GetTypeDefFromString(vtype, ref);
    if(td) {
      SetTypeFromTypeDef(td, ref);
    }
    cd = trim(cd.before('('));
    if(cd.empty()) return true;
    if(cd.contains('=')) {
      String nm = trim(cd.before('='));
      String val = trim(cd.after('='));
      if(nm.nonempty())
        SetName(nm);
      if(val.nonempty())
        SetVar(val);
    }
  }
  else {
    // maybe traditional var def format
    if(cd.contains('=')) {
      String nm = trim(cd.before('='));
      String val = trim(cd.after('='));
      if(nm.nonempty()) {
        if(nm.contains(' ')) {
          SetTypeAndName(nm);
        }
        else {
          SetName(nm);
        }
      }
      if(val.nonempty()) {
        Variant var;
        var.setFromStringGuessType(val); // this allows unknown types to be set from variant type!
        SetVar(var);
      }
    }
    else if(cd.contains(' ')) {
      SetTypeAndName(cd);
    }
    else {
      bool ref;
      TypeDef* td = GetTypeDefFromString(cd, ref); // see if it is a type?
      if(td) {
        SetTypeFromTypeDef(td, ref);
      }
      else {
        SetName(cd);
      }
    }
  }
  UpdateAfterEdit();            // this is essential for setting update schema bit
  return true;
}

bool ProgVar::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgVar::BrowserEditMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserEditMe_ProgItem(this);
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

String ProgVar::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_disp_name) {
    String rval = GetDisplayName();
    if(desc.nonempty())
      rval +=  " // " + desc;
    return rval;
  }
  return inherited::GetColText(key, itm_idx);
}

void ProgVar::AddVarToControlPanel(ControlPanel* ctrl_panel, bool short_label) {
  taBase* base = this;
  MemberDef* mbr = GetValMemberDef();
  
  if (var_type == ProgVar::T_DynEnum) {
    base = (taBase*)&this->dyn_enum_val;
    if (base) {
      mbr = base->FindMemberName("value");
    }
  }
  
  if(!ctrl_panel) {
    taProject* proj = GetMyProj();
    if(TestError(!proj, "AddToControlPanel", "cannot find project")) return;
    ctrl_panel = (ControlPanel*)proj->ctrl_panels.New(1);
  }
  ctrl_panel->AddMember(base, mbr, "", "", "", short_label);
}

bool ProgVar::AddToControlPanel(MemberDef* member, ControlPanel* ctrl_panel) {
  taBase* base = this;
  MemberDef* mbr = member;
  
  if (var_type == ProgVar::T_DynEnum) {
    base = (taBase*)&this->dyn_enum_val;
    if (base) {
      mbr = base->FindMemberName("value");
    }
  }
  else {
    if (!mbr) {
      mbr = GetValMemberDef();
    }
  }
  
  if(!ctrl_panel) {
    taProject* proj = GetMyProj();
    if(TestError(!proj, "AddToControlPanel", "cannot find project")) return false;
    ctrl_panel = (ControlPanel*)proj->ctrl_panels.New(1);
  }
  return ctrl_panel->AddMemberPrompt(base, mbr);
}

void ProgVar::RenameToObj() {
  if(var_type != T_Object) return;
  if(object_val) {
    String nm = object_val->GetName();
    if(nm.nonempty()) {
      String nw_nm = nm.CamelToSnake();
      if(name != nw_nm) {
        SetName(nw_nm);
        UpdateAfterEdit();        // apply!
      }
    }
  }
}

bool ProgVar::ReplaceWithVar(ProgVar* repl_var) {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->UpdatePointers_NewObj(this, repl_var);
}

void ProgVar::GetMemberCompletionList(const MemberDef* md, const String& cur_txt, Completions& completions) {
  TypeDef* completion_td = NULL;
  bool ref = false;
  if (completion_type.nonempty()) {
    completion_td = ProgVar::GetTypeDefFromString(completion_type, ref);
  }
  else {  // control panel member uses same completion type as actual var pointed to - of course
    ControlPanelMember* cp_member = NULL;
    taBase* base = GetOwner(&TA_ControlPanelMember);
    if (base) {
      cp_member = (ControlPanelMember*)base;
      if (cp_member) {
        ProgVar* pv = (ProgVar*)cp_member->base;
        if (pv) {
          String comp_type = pv->completion_type;
          completion_td = ProgVar::GetTypeDefFromString(comp_type, ref);
        }
      }
    }
  }
  if (completion_td) {
    bool include_subtypes = true;
    ProgExprBase::GetTokensOfType(completion_td, &completions.object_completions, this->GetMyProj(), &TA_taProject, include_subtypes);
    String txt = cur_txt;
    txt = txt.trimr();
    if (txt.endsWith(':')) {
      completions.pre_text = cur_txt;
    }
  }
}

