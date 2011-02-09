// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ta_program.h"

#include "css_machine.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "css_ta.h"
#include "ta_project.h"
#include "ta_program_els.h"

#include <QCoreApplication>
#include <QDir>
//#include <QUrlInfo>
#include <QFileInfo>
#include <QDateTime>

#ifdef TA_GUI
# include "ilineedit.h" // for iTextDialog
# include "ta_qt.h"
# include "ta_qtdialog.h"
#endif


///////////////////////////////////////////////////////////
//		Program Types
///////////////////////////////////////////////////////////

void ProgType::Initialize() {
  setUseStale(true);
}

void ProgType::Destroy() {
  CutLinks();
}

void ProgType::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(Program::IsForbiddenName(name))
    name = "My" + name;
}

void ProgType::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(Program::IsForbiddenName(name, false), quiet, rval,
		"Name:",name,"is forbidden -- choose another"))
    name = "My" + name;
}

taBase* ProgType::FindTypeName(const String& nm) const {
  if(name == nm) return (taBase*)this;
  return NULL;
}

const String ProgType::GenCssType() const {
  return "";
}

const String ProgType::GenCss(int indent_level) {
  String rval = Program::GetDescString(desc, indent_level);
  rval += GenCssPre_impl(indent_level);
  rval += GenCssBody_impl(indent_level);
  rval += GenCssPost_impl(indent_level);
  return rval;
} 

const String ProgType::GenListing(int indent_level) {
  String rval = Program::GetDescString(desc, indent_level);
  rval += cssMisc::Indent(indent_level) + GetDisplayName() + "\n";
  return rval;
}

bool ProgType::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgType::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgType::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}


//////////////////////////
//   ProgType_List	//
//////////////////////////

void ProgType_List::Initialize() {
  SetBaseType(&TA_ProgType);
  setUseStale(true);
}

void ProgType_List::El_SetIndex_(void* it_, int idx) {
  ProgType* it = (ProgType*)it_;
  if (it->name.empty()) {
    it->name = "Type_" + (String)idx;
  }
}

DynEnumType* ProgType_List::NewDynEnum() {
  return (DynEnumType*)New_gui(1, &TA_DynEnumType); // gui op
}

taBase* ProgType_List::FindTypeName(const String& nm)  const {
  for (int i = 0; i < size; ++i) {
    ProgType* it = FastEl(i);
    taBase* ptr = it->FindTypeName(nm);
    if(ptr) return ptr;
  }
  return NULL;
}

const String ProgType_List::GenCss(int indent_level) const {
  String rval(0, 40 * size, '\0'); // buffer with typical-ish room
  for (int i = 0; i < size; ++i) {
    ProgType* it = FastEl(i);
    rval += it->GenCss(indent_level); 
  }
  return rval;
}
const String ProgType_List::GenListing(int indent_level) const {
  String rval(0, 40 * size, '\0'); // buffer with typical-ish room
  for (int i = 0; i < size; ++i) {
    ProgType* it = FastEl(i);
    rval += it->GenListing(indent_level); 
  }
  return rval;
}

void ProgType_List::setStale() {
  inherited::setStale();
  // note: there are no vars just in programs anymore
  // if we are in a program group, dirty all progs
  // note: we have to test if in a prog first, otherwise we'll always get a group
//   Program* prog = GET_MY_OWNER(Program);
//   if (!prog) {
//     Program_Group* grp = GET_MY_OWNER(Program_Group);
//     if (grp)
//       grp->SetProgsStale();
//   }
}

bool ProgType_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgType_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgType_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

///////////////////////////////////////////////////////////
//		DynEnumType
///////////////////////////////////////////////////////////

void DynEnumItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(Program::IsForbiddenName(name))
    name = "My" + name;
}

void DynEnumItem::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(Program::IsForbiddenName(name, false), quiet, rval,
		"Name:",name,"is forbidden -- choose another"))
    name = "My" + name;
}

String DynEnumItem::GetDisplayName() const {
  return name + "=" + String(value);
}

bool DynEnumItem::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool DynEnumItem::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool DynEnumItem::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

///////////// List

void DynEnumItem_List::Initialize() {
  SetBaseType(&TA_DynEnumItem);
}

int DynEnumItem_List::FindNumIdx(int val) const {
  for(int i=0;i<size;i++)
    if(FastEl(i)->value == val) return i;
  return -1;
}

void DynEnumItem_List::OrderItems() {
  if(size == 0) return;
  int prval = FastEl(0)->value;
  for(int i=1;i<size;i++) {
    DynEnumItem* it = FastEl(i);
    if(it->value <= prval) {
      it->value = prval + 1;
      it->DataChanged(DCR_ITEM_UPDATED);
    }
    prval = it->value;
  }
}

void DynEnumItem_List::DataChanged(int dcr, void* op1, void* op2) {
//   cerr << "dyn enum: " << name << " invalidated due to type change" << endl;
  OrderItems();
  // we notify owner, so editing items causes related things to update,
  // typically used by ProgVar to make sure the enum list gets updated in gui
  taBase* own = GetOwner();
  if(own) {
    if(dcr <= DCR_CHILD_ITEM_UPDATED)
      own->DataChanged(DCR_CHILD_ITEM_UPDATED, (void*)this);
  }
  inherited::DataChanged(dcr, op1, op2);
}

bool DynEnumItem_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool DynEnumItem_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool DynEnumItem_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

///////////////////////////
//	DynEnumType

void DynEnumType::Initialize() {
  SetDefaultName();
  bits = false;
}

DynEnumItem* DynEnumType::NewEnum() {
  return (DynEnumItem*)enums.New_gui(1); // primarily a gui op
}

DynEnumItem* DynEnumType::AddEnum(const String& nm, int val) {
  DynEnumItem* it = (DynEnumItem*)enums.New(1);
  it->name = nm;
  it->value = val;
  enums.OrderItems();
  it->DataChanged(DCR_ITEM_UPDATED);
  return it;
}

void DynEnumType::SeqNumberItems(int first_val) {
  int val = first_val;
  for(int i=0;i<enums.size;i++) {
    DynEnumItem* it = enums.FastEl(i);
    it->value = val;
    it->DataChanged(DCR_ITEM_UPDATED);
    val++;
  }
}

bool DynEnumType::CopyToAllProgs() {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  Program* pg;
  taLeafItr i;
  FOR_ITR_EL(Program, pg, proj->programs., i) {
    DynEnumType* tp = (DynEnumType*)pg->types.FindName(name); // find my name
    if(!tp || tp == this || !tp->InheritsFrom(&TA_DynEnumType)) continue;
    tp->CopyFrom(this);
  }
  return true;
}

taBase* DynEnumType::FindTypeName(const String& nm) const {
  if(name == nm) return (taBase*)this;
  int idx = FindNameIdx(nm);
  if(idx >= 0)  {
    return enums.FastEl(idx);
  }
  return NULL;
}

String DynEnumType::GetDisplayName() const {
  return "enum " + name + " (" + String(enums.size) + " items)";
}

const String DynEnumType::GenCssPre_impl(int indent_level) {
  if(enums.size == 0) return _nilString;
  String il = cssMisc::Indent(indent_level); 
  String rval = il + "enum " + name + " {\n";
  return rval;
}

const String DynEnumType::GenCssBody_impl(int indent_level) {
  if(enums.size == 0) return _nilString;
  String il1 = cssMisc::Indent(indent_level+1);
  String rval;
  for(int i=0;i<enums.size;i++) {
    DynEnumItem* it = enums.FastEl(i);
    rval += il1 + it->name + " \t = ";
    if(bits)
      rval += String(1 << it->value, "%x") + ",";
    else
      rval += String(it->value) + ",";
    if(!it->desc.empty()) {
      if(it->desc.contains('\n'))
	rval += "  /* " + it->desc + " */";
      else
	rval += "  // " + it->desc;
    }
    rval += "\n";
  }
  return rval;
}

const String DynEnumType::GenCssPost_impl(int indent_level) {
  if(enums.size == 0) return _nilString;
  String il = cssMisc::Indent(indent_level); 
  String rval = il + "};\n";
  return rval;
}

ostream& DynEnumType::OutputType(ostream& strm) const {
  String rval = ((DynEnumType*)this)->GenCss(0);
  strm << rval;
  return strm;
}

void DynEnumType::DataChanged(int dcr, void* op1, void* op2) {
  // dynenum is programmed to send us notifies, we trap those and 
  // turn them into changes of us, to force gui to update (esp enum list)
  if (dcr == DCR_CHILD_ITEM_UPDATED) {
    DataChanged(DCR_ITEM_UPDATED);
    return; // don't send any further
  }
  inherited::DataChanged(dcr, op1, op2);
}

// todo: check config on bits with value > 31

void DynEnumType::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  enums.CheckConfig(quiet, rval);
}

bool DynEnumType::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool DynEnumType::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool DynEnumType::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

///////////////////////////////////////////////////////////
//		DynEnum value
///////////////////////////////////////////////////////////

void DynEnum::Initialize() {
  value = -1;
}

void DynEnum::Destroy() {
  CutLinks();
  value = -1;
}

String DynEnum::GetDisplayName() const {
  if((bool)enum_type)
    return enum_type->name + " " + NameVal();
  else
    return "(no dyn enum type!)";
}

/*TEMP String DynEnum::GetValStr(void* par, MemberDef* md, TypeDef::StrContext sc,
			  bool force_inline) const {
  if(sc == TypeDef::SC_DISPLAY)
    return GetDisplayName();
  else
    return inherited::GetValStr(par, md, sc, force_inline);
}*/

void DynEnum::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!enum_type, quiet, rval,
	     "enum_type is not set for this dynamic enum value");
}

int DynEnum::NumVal() const {
  if(!enum_type) return -1;
  if(enum_type->bits) return value;
  if(value < 0 || value >= enum_type->enums.size) return -1;
  return enum_type->enums.FastEl(value)->value;
}

const String DynEnum::NameVal() const {
  if(!enum_type) return -1;
  if(enum_type->bits) {
    String rval;
    for(int i=0;i<enum_type->enums.size;i++) {
      DynEnumItem* it = enum_type->enums.FastEl(i);
      if(value & (1 << it->value)) {
	if(!rval.empty()) rval += "|";
	rval += it->name;
      }
    }
    return rval;
  }
  if(value < 0 || value >= enum_type->enums.size) return "";
  return enum_type->enums.FastEl(value)->name;
}

bool DynEnum::SetNumVal(int val) {
  if(!enum_type) return false;
  if(enum_type->bits) {
    value = val;		// must just be literal value.. 
  }
  else {
    value = enum_type->FindNumIdx(val);
    if(TestError(value < 0, "SetNumVal", "value:", (String)val, "not found!"))
      return false;
  }
  return true;
}

bool DynEnum::SetNameVal(const String& nm) {
  if(!enum_type) return false;
  if(enum_type->bits) {
    DynEnumItem* it = enum_type->enums.FindName(nm);
    if(TestError(!it, "SetNameVal", "value label:", nm, "not found!"))
      return false;
    value |= 1 << it->value;
  }
  else {
    value = enum_type->FindNameIdx(nm);
    if(TestError(value < 0, "SetNameVal", "value label:", nm, "not found!"))
      return false;
  }
  return true;
}

bool DynEnum::ClearBitName(const String& nm) {
  if(!enum_type) return false;
  if(TestError(!enum_type->bits, "ClearBitName", "this can only be used for bits type enums"))
    return false;
  DynEnumItem* it = enum_type->enums.FindName(nm);
  if(TestError(!it, "value label:", nm, "not found!"))
    return false;
  value &= ~(1 << it->value);
  return true;
}


//////////////////////////
//   ProgVar		//
//////////////////////////

void ProgVar::Initialize() {
  var_type = T_Int;
  int_val = 0;
  real_val = 0.0;
  bool_val = false;
  object_type = &TA_taOBase;
  hard_enum_type = NULL;
  objs_ptr = false;
  flags = (VarFlags)(CTRL_PANEL | NULL_CHECK | EDIT_VAL | SAVE_VAL);
  reference = false;
  parse_css_el = NULL;
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
      is_global = true;		// this is the only case in which vars are global
  }
  if(is_global) {
    ClearVarFlag(LOCAL_VAR);
    ClearVarFlag(FUN_ARG);
    if(!objs_ptr && var_type == T_Object && object_type && object_type->InheritsFrom(&TA_taMatrix)) {
	TestWarning(true, "ProgVar", "for Matrix* ProgVar named:",name,
		    "Matrix pointers should be located in ProgVars (local vars) within the code, not in the global vars/args section, in order to properly manage the reference counting of matrix objects returned from various functions.");
    }
  }
  else {
    objs_ptr = false;		// this is incompatible with being local
    SetVarFlag(LOCAL_VAR);
    int_val = 0;
    real_val = 0.0;
    bool_val = false;
    object_val = NULL;
    ClearVarFlag(CTRL_PANEL);
    ClearVarFlag(CTRL_READ_ONLY);
    // now check for fun args
    ClearVarFlag(FUN_ARG);
    if(owner && owner->InheritsFrom(&TA_ProgVar_List)) {
      ProgVar_List* pvl = (ProgVar_List*)owner;
      if(pvl->owner && pvl->owner->InheritsFrom(&TA_Function))
	SetVarFlag(FUN_ARG);
    }
  }
  if(init_from) {		// NOTE: LOCAL_VAR is now editable and provides initializer value for local variables
    ClearVarFlag(EDIT_VAL);
  }
  else {
    SetVarFlag(EDIT_VAL);
  }
}

bool ProgVar::UpdateUsedFlag() {
  taDataLink* dl = data_link();
  if(!dl) return false;
  int cnt = 0;
  taDataLinkItr itr;
  taSmartRef* el;
  FOR_DLC_EL_OF_TYPE(taSmartRef, el, dl, itr) {
    taBase* spo = el->GetOwner();
    if(!spo) continue;
    cnt++;
  }
  if(objs_ptr) cnt++;		// always get one count for this -- always used..
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

void ProgVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(object_val.ptr() == this)	// this would be bad..
    object_val.set(NULL);
  name = taMisc::StringCVar(name); // make names C legal names -- just much safer
  if(Program::IsForbiddenName(name))
    name = "My" + name;
  // only send stale if the schema changed, not just the value
  String tfs = GetSchemaSig();
  // loading is a special case: initialize
  if (taMisc::is_loading) {
    taVersion v512(5, 1, 2);
    if(taMisc::loading_version < v512) { // everything prior to 512 had save val on by default
      SetVarFlag(SAVE_VAL);
    }
    m_prev_sig = tfs;
    m_this_sig = tfs;
  } else {
    m_prev_sig = m_this_sig;
    m_this_sig = tfs;
    if (m_prev_sig != m_this_sig) {
      setStale();
    }
  }
  SetFlagsByOwnership();
  UpdateUsedFlag();
  GetInitFromVar(true);		// warn 
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
  if(CheckError(Program::IsForbiddenName(name, false), quiet, rval,
		"Name:",name,"is forbidden -- choose another"))
    name = "My" + name;
  if(var_type == T_Object) {
    if(!HasVarFlag(LOCAL_VAR) && HasVarFlag(NULL_CHECK) && !object_val) {
      if(!quiet) taMisc::CheckError("Error in ProgVar in program:", prognm, "var name:",name,
				    "object pointer is NULL");
      rval = false;
    }
    if(object_type) {
      TestWarning(!objs_ptr && !HasVarFlag(LOCAL_VAR) && object_type->InheritsFrom(&TA_taMatrix),
		  "ProgVar", "for Matrix* ProgVar named:",name,
		  "Matrix pointers should be located in ProgVars (local vars) within the code, not in the global vars/args section, in order to properly manage the reference counting of matrix objects returned from various functions.");
    }
  }
  GetInitFromVar(true);		// warn 
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
    String& full_lbl, String& desc) const
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
    full_lbl = xtra_lbl;
    if (full_lbl.nonempty()) full_lbl += " ";
    full_lbl += GetName().elidedTo(16); // var name, not the member name
    if (desc.empty()) 
      desc = GetDesc(); // leave empty if empty
  } else { // something else, just do default
    inherited::GetSelectText(mbr,xtra_lbl, full_lbl, desc );
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
  }
  return _nilVariant;// compiler food
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

void ProgVar::DataChanged(int dcr, void* op1, void* op2) {
  // dynenum is programmed to send us notifies, we trap those and 
  // turn them into changes of us, to force gui to update (esp enum list)
  if ((dcr == DCR_CHILD_ITEM_UPDATED) && (op1 == &dyn_enum_val)) {
    DataChanged(DCR_ITEM_UPDATED);
    return; // don't send any further
  }
  inherited::DataChanged(dcr, op1, op2);
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
    else rval = name + " = " + 
	   hard_enum_type->Get_C_EnumString(int_val) + " (" + hard_enum_type->name + ")";
    break;
  case T_DynEnum:
    if((bool)dyn_enum_val.enum_type)
      rval = name + " = " + dyn_enum_val.NameVal() + " (" + dyn_enum_val.enum_type->name + ")";
    else
      rval = name + " = " + dyn_enum_val.NameVal() + " (no dyn enum type!)";
    break;
  default:
    rval = name + " invalid type!";
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
  UpdateAfterEdit();		// update used and other flags
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
    if(hard_enum_type)
      return hard_enum_type->name;
    else
      return "int";
  case T_DynEnum:
    if(dyn_enum_val.enum_type) {
      return dyn_enum_val.enum_type->name;
    }
    return "int";
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
  case T_Object:
    if(object_val)
      return object_val->GetPath();
    else
      return "NULL";
  case T_HardEnum:
    if(hard_enum_type)
      return hard_enum_type->GetValStr(&int_val);
    else
      return int_val;
  case T_DynEnum:
    return dyn_enum_val.NameVal();
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
  rval += ";\n";
  if(HasVarFlag(LOCAL_VAR)) {
    rval += name + " = " + GenCssInitVal() + ";\n";
  }
  return rval;
}

Program* ProgVar::GetInitFromProg() {
  TestError(!init_from, "GetInitFromProg", "init_from program is NULL for initializing variable:", name, "in program:", program()->name);
  return init_from.ptr();
}


const String ProgVar::GenCssInitFrom(int indent_level) {
  ProgVar* ivar = GetInitFromVar(false); // no warning now
  if(!ivar) return _nilString;
  String il = cssMisc::Indent(indent_level);
  String il1 = cssMisc::Indent(indent_level+1);
  String rval = il + "{ // init_from\n"; 
  rval.cat(il1).cat("Program* init_fm_prog = this").cat(GetPath(NULL, program())).cat("->GetInitFromProg();\n");
  rval.cat(il1).cat(name).cat(" = init_fm_prog->GetVar(\"").cat(name).cat("\");\n");
  rval.cat(il).cat("}\n");
  return rval;
}

cssEl* ProgVar::NewCssEl() {
  switch(var_type) {
  case T_Int:
    return new cssCPtr_int(&int_val, 0, name);
  case T_Real:
    return new cssCPtr_double(&real_val, 0, name);
  case T_String:
    return new cssCPtr_String(&string_val, 0, name);
  case T_Bool:
    return new cssCPtr_bool(&bool_val, 0, name);
  case T_Object: 
    return new cssSmartRef(&object_val, 0, &TA_taBaseRef, name);
  case T_HardEnum:
    return new cssCPtr_enum(&int_val, 0, name, hard_enum_type);
  case T_DynEnum:
    return new cssCPtr_DynEnum(&dyn_enum_val, 0, name);
  }
  return &cssMisc::Void;
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

//////////////////////////
//   ProgVar_List	//
//////////////////////////

void ProgVar_List::Initialize() {
  SetBaseType(&TA_ProgVar);
  var_context = VC_ProgVars;
  setUseStale(true);
}

void ProgVar_List::Copy_(const ProgVar_List& cp) {
  var_context = cp.var_context;

  // this is not needed: each individual guy will do it
  //  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Program);
}

void ProgVar_List::El_SetIndex_(void* it_, int idx) {
  ProgVar* it = (ProgVar*)it_;
  if (it->name.empty()) {
    it->name = "Var_" + (String)idx;
  }
}

void ProgVar_List::AddVarTo(taNBase* src) {
  if (!src) return;
  // if already exists, just ignore
  for (int i = 0; i < size; ++i) {
    ProgVar* it = FastEl(i);
    if ((it->var_type == ProgVar::T_Object) &&
      (it->object_val.ptr() == src))
      return;
  }
  ProgVar* it = (ProgVar*)New(1);
  it->SetObject(src);
  it->SetName(src->GetName());
  it->UpdateAfterEdit();
  if(taMisc::gui_active)
    tabMisc::DelayedFunCall_gui(it, "BrowserSelectMe");
}

void ProgVar_List::CreateDataColVars(DataTable* src) {
  if (!src) return;

  for(int i=0;i<src->data.size; i++) {
    DataCol* da = src->data[i];
    if(da->is_matrix || da->name.empty()) continue;
    ProgVar* it = FindName(da->name);
    if(!it)
      it = (ProgVar*)New(1);
    it->SetName(da->name);
    if(da->isString()) {
      it->var_type = ProgVar::T_String;
    }
    else if(da->valType() == VT_FLOAT || da->valType() == VT_DOUBLE) {
      it->var_type = ProgVar::T_Real;
    }
    else {
      it->var_type = ProgVar::T_Int;
    }
    it->UpdateAfterEdit();
  }
}

const String ProgVar_List::GenCss(int indent_level) const {
  String rval(0, 40 * size, '\0'); // buffer with typical-ish room
  int cnt = 0;
  for (int i = 0; i < size; ++i) {
    ProgVar* it = FastEl(i);
    bool is_arg = (var_context == VC_FuncArgs);
    if (is_arg) {
      if (cnt > 0)
        rval += ", ";
    } else {
      rval += cssMisc::Indent(indent_level); 
    }
    rval += it->GenCss(is_arg); 
    ++cnt;
  }
  return rval;
}

const String ProgVar_List::GenCssInitFrom(int indent_level) const {
  String rval;
  for(int i=0;i<size;i++) {
    ProgVar* var = FastEl(i);
    rval += var->GenCssInitFrom(indent_level);
  }
  return rval;
}

const String ProgVar_List::GenListing(int indent_level) const {
  String rval(0, 40 * size, '\0'); // buffer with typical-ish room
  int cnt = 0;
  for (int i = 0; i < size; ++i) {
    ProgVar* it = FastEl(i);
    bool is_arg = (var_context == VC_FuncArgs);
    if (is_arg) {
      if (cnt > 0)
        rval += ", ";
    }
    rval += it->GenListing(is_arg, indent_level); 
    ++cnt;
  }
  return rval;
}

ProgVar* ProgVar_List::FindVarType(ProgVar::VarType vart, TypeDef* td) {
  for (int i = 0; i < size; ++i) {
    ProgVar* it = FastEl(i);
    if(it->var_type == vart) {
      if((vart == ProgVar::T_Object) && (td != NULL)) {
	if(it->object_type == td) {
	  return it;
	}
      }
      else if((vart == ProgVar::T_HardEnum) && (td != NULL)) {
	if(it->hard_enum_type == td) {
	  return it;
	}
      }
      else {
	return it;
      }
    }
  }
  return NULL;
}

void ProgVar_List::setStale() {
  inherited::setStale();
  // note: there are no vars just in program groups anymore.. 
  // if we are in a program group, dirty all progs
  // note: we have to test if in a prog first, otherwise we'll always get a group
//   Program* prog = GET_MY_OWNER(Program);
//   if (!prog) {
//     Program_Group* grp = GET_MY_OWNER(Program_Group);
//     if (grp)
//       grp->SetProgsStale();
//   }
}

bool ProgVar_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgVar_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgVar_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

//////////////////////////
//   ProgVarRef_List	//
//////////////////////////

void ProgVarRef_List::Initialize() {
}

ProgVarRef_List::~ProgVarRef_List() {
  Reset();
}

ProgVarRef* ProgVarRef_List::FindVar(ProgVar* var, int& idx) const {
  for(int i=0;i<size;i++) {
    ProgVarRef* vrf = FastEl(i);
    if(vrf->ptr() == var) {
      idx = i;
      return vrf;
    }
  }
  idx = -1;
  return NULL;
}

ProgVarRef* ProgVarRef_List::FindVarName(const String& var_nm, int& idx) const {
  for(int i=0;i<size;i++) {
    ProgVarRef* vrf = FastEl(i);
    if(vrf->ptr() && ((ProgVar*)vrf->ptr())->name == var_nm) {
      idx = i;
      return vrf;
    }
  }
  idx = -1;
  return NULL;
}

int ProgVarRef_List::UpdatePointers_NewPar(taBase* lst_own, taBase* old_par, taBase* new_par) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    ProgVarRef* vrf = FastEl(i);
    nchg += lst_own->UpdatePointers_NewPar_Ref(*vrf, old_par, new_par);
  }
  return nchg;
}

int ProgVarRef_List::UpdatePointers_NewParType(taBase* lst_own, TypeDef* par_typ, taBase* new_par) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    ProgVarRef* vrf = FastEl(i);
    nchg += lst_own->UpdatePointers_NewParType_Ref(*vrf, par_typ, new_par);
  }
  return nchg;
}

int ProgVarRef_List::UpdatePointers_NewObj(taBase* lst_own, taBase* ptr_owner, taBase* old_ptr, taBase* new_ptr) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    ProgVarRef* vrf = FastEl(i);
    nchg += lst_own->UpdatePointers_NewObj_Ref(*vrf, ptr_owner, old_ptr, new_ptr);
  }
  return nchg;
}


//////////////////////////
//   ProgExprBase	//
//////////////////////////

cssProgSpace ProgExprBase::parse_prog;
cssSpace ProgExprBase::parse_tmp;

void ProgExprBase::Initialize() {
  flags = PE_NONE;
  parse_ve_off = 11;
  parse_ve_pos = 0;
}

void ProgExprBase::Destroy() {	
  CutLinks();
}

void ProgExprBase::InitLinks() {
  inherited::InitLinks();
  taBase::Own(var_names, this);
  taBase::Own(bad_vars, this);
}

void ProgExprBase::CutLinks() {
  vars.Reset();
  var_names.CutLinks();
  bad_vars.CutLinks();
  inherited::CutLinks();
}

void ProgExprBase::Copy_(const ProgExprBase& cp) {
  expr = cp.expr;
  flags = cp.flags;
  var_expr = cp.var_expr;
  var_names = cp.var_names;
  bad_vars = cp.bad_vars;

  //  vars = cp.vars;
  // the above copy does not set the owner!
  vars.Reset();
  for(int i=0;i<cp.vars.size;i++) {
    ProgVarRef* pvr = new ProgVarRef;
    vars.Add(pvr);
    pvr->Init(this);
    pvr->set(cp.vars[i]->ptr());
  }
}

void ProgExprBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying()) return;
  ProgEl* pel = GET_MY_OWNER(ProgEl);
  if(pel && (pel->GetEnabled() == 0)) return;
  ParseExpr();
  if(!HasExprFlag(NO_VAR_ERRS)) {
    ProgEl* pel = GET_MY_OWNER(ProgEl);
    if(!taMisc::is_loading && bad_vars.size > 0) {
      taMisc::Error("ProgExpr in program element:", pel->GetDisplayName(),"\n in program:", prg->name," Errors in expression -- the following variable names could not be found:", bad_vars[0],
		    (bad_vars.size > 1 ? bad_vars[1] : _nilString),
		    (bad_vars.size > 2 ? bad_vars[2] : _nilString),
		    (bad_vars.size > 3 ? bad_vars[3] : _nilString)
		    // (bad_vars.size > 4 ? bad_vars[4] : _nilString),
		    // (bad_vars.size > 5 ? bad_vars[5] : _nilString),
		    // (bad_vars.size > 6 ? bad_vars[6] : _nilString)
		    );
    }
  }
}

void ProgExprBase::UpdateProgExpr_NewOwner() {
  // note: this is assumed to be called *prior* to any updating of pointers!
  ProgEl* pel = GET_MY_OWNER(ProgEl);
  if(!pel) return;
  for(int i=0;i<vars.size;i++) {
    ProgVarRef* pvr = vars[i];
    pel->UpdateProgVarRef_NewOwner(*pvr);
  }
}

void ProgExprBase::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!HasExprFlag(NO_VAR_ERRS)) {
    Program* prg = GET_MY_OWNER(Program);
    ProgEl* pel = GET_MY_OWNER(ProgEl);
    if(prg && pel && bad_vars.size > 0) {
      rval = false;
      if(!quiet)
	taMisc::CheckError("ProgExpr in program element:", pel->GetDisplayName(),"\n in program:", prg->name," Errors in expression -- the following variable names could not be found:", bad_vars[0],
			   (bad_vars.size > 1 ? bad_vars[1] : _nilString),
			   (bad_vars.size > 2 ? bad_vars[2] : _nilString),
			   (bad_vars.size > 3 ? bad_vars[3] : _nilString)
			   // (bad_vars.size > 4 ? bad_vars[4] : _nilString),
			   // (bad_vars.size > 5 ? bad_vars[5] : _nilString),
			   // (bad_vars.size > 6 ? bad_vars[6] : _nilString)
			   );
    }
  }
}

int ProgExprBase::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int nchg = inherited::UpdatePointers_NewPar(old_par, new_par);
  nchg += vars.UpdatePointers_NewPar(this, old_par, new_par);
  return nchg;
}

int ProgExprBase::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) {
  int nchg = inherited::UpdatePointers_NewParType(par_typ, new_par);
  nchg += vars.UpdatePointers_NewParType(this, par_typ, new_par);
  return nchg;
}

int ProgExprBase::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  int nchg = inherited::UpdatePointers_NewObj(old_ptr, new_ptr);
  nchg += vars.UpdatePointers_NewObj(this, this, old_ptr, new_ptr);
  return nchg;
}

void ProgExprBase::SmartRef_DataDestroying(taSmartRef* ref, taBase* obj) {
  inherited::SmartRef_DataDestroying(ref, obj); // does UAE
}

void ProgExprBase::SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
				    int dcr, void* op1_, void* op2_)
{
  // we only update ourself if the schema of a var changed
  if (obj && obj->InheritsFrom(&TA_ProgVar)) {
    ProgVar* pv = (ProgVar*)obj;
    if (!pv->schemaChanged()) 
      return;
  }
  
  expr = GetFullExpr();		// update our expr to reflect any changes in variables.
  if(owner)			// usu we are inside something else
    owner->UpdateAfterEdit();
  else
    UpdateAfterEdit();
}

bool ProgExprBase::SetExpr(const String& ex) {
  expr = ex;
  UpdateAfterEdit();		// does parse
  return (bad_vars.size == 0);	// do we have any bad variables??
}

String ProgExprBase::GetName() const {
  if(owner) return owner->GetName();
  return _nilString;
}

String ProgExprBase::GetDisplayName() const {
  return expr;
}

void ProgExprBase::ParseExpr_SkipPath(int& pos) {
  int len = expr.length();
  int c;
  while((pos < len) && (isalnum(c=expr[pos]) || (c=='_') || (c=='.') || (c==':')
			|| (c=='[') || (c==']')))
    { var_expr.cat((char)c); pos++; }
}


int ProgExprBase::cssExtParseFun_pre(void* udata, const char* nm, cssElPtr& el_ptr) {
  String vnm = nm;
  if(vnm == "__tmp") return 0;	// skip

  ProgExprBase* pe = (ProgExprBase*)udata;
  Program* prog = GET_OWNER(pe, Program);
  int idx = 0;
  ProgVar* var = NULL;
  Function* fun = GET_OWNER(pe, Function); // first look inside any function we might live within

  if(vnm == "this") {
    cssEl* el = new cssTA_Base((void*)prog, 1, &TA_Program, "this");
    pe->parse_tmp.Push(el);
    el_ptr.SetDirect(el);
    return el->GetParse();
  }
  if(vnm == "run_state") {
    cssEl* el = new cssCPtr_enum(&(prog->run_state), 1, "run_state",
				 TA_Program.sub_types.FindName("RunState"));
    pe->parse_tmp.Push(el);
    el_ptr.SetDirect(el);
    return el->GetParse();
  }
  if(vnm == "ret_val") {
    cssEl* el = new cssCPtr_int(&(prog->ret_val), 1, "ret_val");
    pe->parse_tmp.Push(el);
    el_ptr.SetDirect(el);
    return el->GetParse();
  }

  if(fun)
    var = fun->FindVarName(vnm);
  if(!var)
    var = prog->FindVarName(vnm);
  if(var) {
    if(!pe->vars.FindVar(var, idx)) {
      ProgVarRef* prf = new ProgVarRef;
      prf->Init(pe);	// we own it
      prf->set(var);
      pe->vars.Add(prf);
      pe->var_names.Add(vnm);
      idx = pe->vars.size-1;
    }
    var->SetParseCssEl();
    el_ptr.SetDirect(var->parse_css_el);
    // update var_expr
    String subst = "$#" + (String)idx + "#$";
    String add_chunk = pe->expr.at(pe->parse_ve_pos-pe->parse_ve_off, parse_prog.src_pos - pe->parse_ve_pos - vnm.length());
//     String vnm_chunk = pe->expr.at(parse_prog.src_pos - pe->parse_ve_off - vnm.length(), vnm.length());
    pe->var_expr += add_chunk;
    pe->var_expr += subst;
    pe->parse_ve_pos = parse_prog.src_pos;

//     cerr << "var: " << vnm << " pos: " << pe->parse_ve_pos-pe->parse_ve_off << " add_chunk: " << add_chunk
// 	 << " vnm_chunk: " << vnm_chunk << endl;

    return var->parse_css_el->GetParse();
  }
  else {
    // not found -- check to see if it is some other thing:
    taBase* ptyp = prog->FindTypeName(vnm);
    if(ptyp) {
      if(ptyp->InheritsFrom(&TA_DynEnumType)) {
	cssEnumType* etyp = new cssEnumType(ptyp->GetName());
	pe->parse_tmp.Push(etyp);
	el_ptr.SetDirect(etyp);
	return CSS_TYPE;
      }
      else if(ptyp->InheritsFrom(&TA_DynEnumItem)) {
	DynEnumItem* eit = (DynEnumItem*)ptyp;
	cssEnum* eval = new cssEnum(eit->value, eit->name);
	pe->parse_tmp.Push(eval);
	el_ptr.SetDirect(eval);
	return CSS_VAR;
      }
    }
    else {
      Function* fun = prog->functions.FindName(vnm);
      if(fun && fun->name == vnm) { // findname will do crazy inherits thing on types, giving wrong match, so you need to make sure it is actually of the same name
	cssScriptFun* sfn = new cssScriptFun(vnm);
	pe->parse_tmp.Push(sfn);
	sfn->argc = fun->args.size;
	el_ptr.SetDirect(sfn);
	return CSS_FUN;
      }
    }
  }
  return 0;			// not found!
}

int ProgExprBase::cssExtParseFun_post(void* udata, const char* nm, cssElPtr& el_ptr) {
  String vnm = nm;
  if(vnm == "__tmp" || vnm == "this") return 0;	// skip that guy
  ProgExprBase* pe = (ProgExprBase*)udata;
  pe->bad_vars.AddUnique(vnm);	// this will trigger err msg later..
//   cerr << "added bad var: " << vnm << endl;
  return 0;				// we don't do any cleanup -- return false
}

bool ProgExprBase::ParseExpr() {
  Program_Group* pgp = GET_MY_OWNER(Program_Group);
  if(TestError(!pgp, "ParseExpr", "no parent Program_Group found -- report to developers as bug"))
    return false;
  String pnm = GetPath_Long(NULL, pgp);

  // todo: temporary fix for saved wrong flags, remove after a while (4.0.10)
  ProgEl* pel = GET_MY_OWNER(ProgEl);
  if(pel) {
    pel->SetProgExprFlags();
  }

  vars.Reset();
  var_names.Reset();
  bad_vars.Reset();
  var_expr = _nilString;
  String parse_expr;
  if(HasExprFlag(FULL_STMT)) {
    parse_expr = expr;
    parse_ve_off = 0;
  }
  else if(HasExprFlag(FOR_LOOP_EXPR)) {
    parse_expr = "for(" + expr + ";;) .i;";
    parse_ve_off = 4;
  }
  else {
    if((bool)taMisc::types.FindName(expr)) {
      var_expr = expr;
      return true; // just a type name -- good!
    }
    parse_expr = "int __tmp= " + expr + ";";
    parse_ve_off = 11;		// offset is 11 due to 'int _tmp= '
  }
  parse_ve_pos = parse_ve_off;
  if(expr.empty() || expr == "<no_arg>") return true; // <no_arg> is a special flag..

  parse_prog.SetName(pnm);
  parse_prog.ClearAll();
  parse_prog.ext_parse_fun_pre = &cssExtParseFun_pre;
  parse_prog.ext_parse_fun_post = &cssExtParseFun_post;
  parse_prog.ext_parse_user_data = (void*)this;
  // use this for debugging exprs:
  //  parse_prog.SetDebug(6);

  parse_prog.CompileCode(parse_expr);	// use css to do all the parsing!

  for(int i=0;i<vars.size;i++)  {
    ProgVarRef* vrf = vars.FastEl(i);
    if(!TestError(!vrf->ptr(), "ParseExpr", "vrf->ptr() == NULL -- this shouldn't happen -- report as a bug!")) {
      ProgVar* var = (ProgVar*)vrf->ptr();
      var->ResetParseStuff();
    }
  }
  // get the rest
  if(parse_ve_pos-parse_ve_off < expr.length()) {
    String end_chunk = expr.at(parse_ve_pos-parse_ve_off, expr.length() - (parse_ve_pos-parse_ve_off));
    var_expr += end_chunk;
//     cerr << "added end_chunk: " << end_chunk << endl;
  }

  parse_tmp.Reset();
  parse_prog.ClearAll();

//   cerr << "var_expr: " << var_expr << endl;

  return (bad_vars.size == 0);	// do we have any bad variables??
}

String ProgExprBase::GetFullExpr() const {
  if(vars.size == 0) return var_expr;
  String rval = var_expr;
  for(int i=0;i<vars.size;i++) {
    ProgVarRef* vrf = vars.FastEl(i);
    if(vrf->ptr()) {
      rval.gsub("$#" + (String)i + "#$", ((ProgVar*)vrf->ptr())->name);
    }
    else {
      rval.gsub("$#" + (String)i + "#$", var_names[i]); // put in original name, as a cue..
    }
  }
  return rval;
}

// StringFieldLookupFun is in ta_program_qt.cpp

//////////////////////////
//   ProgExpr		//
//////////////////////////

bool ProgExpr::StdProgVarFilter(void* base_, void* var_) {
  if(!base_) return true;
  ProgExprBase* base = dynamic_cast<ProgExprBase*>(static_cast<taBase*>(base_));
  if(!base) return true;
  ProgVar* var = dynamic_cast<ProgVar*>(static_cast<taBase*>(var_));
  if(!var || !var->HasVarFlag(ProgVar::LOCAL_VAR)) return true; // definitely all globals
  Function* varfun = GET_OWNER(var, Function);
  if(!varfun) return true;	// not within a function, always go -- can't really tell scoping very well at this level -- could actually do it but it would be recursive and hairy
  Function* basefun = GET_OWNER(base, Function);
  if(basefun != varfun) return false; // different function scope
  return true;
}

void ProgExpr::Initialize() {
  var_lookup = NULL;
}

void ProgExpr::Destroy() {	
  CutLinks();
}

void ProgExpr::CutLinks() {
  if(var_lookup) {
    taBase::SetPointer((taBase**)&var_lookup, NULL);
  }
  inherited::CutLinks();
}

void ProgExpr::UpdateAfterEdit_impl() {
  if(var_lookup) {
    if(expr.empty())
      expr += var_lookup->name;
    else
      expr += " " + var_lookup->name;
    taBase::SetPointer((taBase**)&var_lookup, NULL);
  }
  inherited::UpdateAfterEdit_impl();
}

void ProgExpr_List::Initialize() {
  SetBaseType(&TA_ProgExpr);
  setUseStale(true);
}

void ProgExpr_List::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
}

void ProgExpr_List::UpdateProgExpr_NewOwner() {
  for (int i = 0; i < size; ++ i) {
    ProgExpr* pe = FastEl(i);
    pe->UpdateProgExpr_NewOwner();
  }
}
//////////////////////////
//   ProgArg		//
//////////////////////////

void ProgArg::Initialize() {
  arg_type = NULL;
  required = true; // generally true
  setUseStale(true); // always requires recompile
}

void ProgArg::Destroy() {	
  CutLinks();
}

void ProgArg::InitLinks() {
  inherited::InitLinks();
  taBase::Own(expr, this);
}

void ProgArg::CutLinks() {
  arg_type = NULL;
  expr.CutLinks();
  inherited::CutLinks();
}

void ProgArg::Copy_(const ProgArg& cp) {
  type = cp.type;
  name = cp.name;
  required = cp.required;
  def_val = cp.def_val;
  expr = cp.expr;
  arg_type = cp.arg_type;
}

void ProgArg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError((required && expr.empty()), quiet, rval,
    "An expression is required for this argument -- enter <no_arg> as the expression to explicitly not pass an argument for calling a Program (does not work for method or function calls)");
}

void ProgArg::SetVarAsExpr(ProgVar* prog_var) {
  if(!prog_var) return;
  expr.expr = prog_var->name;
  expr.UpdateAfterEdit();
}

bool ProgArg::UpdateFromVar(const ProgVar& cp) {
  bool any_changes = false;
  String ntyp = cp.GenCssType();
  if(type != ntyp) {
    any_changes = true;
    type = ntyp;
  }
  if(cp.var_type == ProgVar::T_Int) {
    if(arg_type != &TA_int) {
      arg_type = &TA_int;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_Real) {
    if(arg_type != &TA_double) {
      arg_type = &TA_double;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_String) {
    if(arg_type != &TA_taString) {
      arg_type = &TA_taString;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_Bool) {
    if(arg_type != &TA_bool) {
      arg_type = &TA_bool;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_Object) {
    if(arg_type != cp.object_type) {
      arg_type = cp.object_type;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_HardEnum) {
    if(arg_type != cp.hard_enum_type) {
      arg_type = cp.hard_enum_type;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_DynEnum) {
    if(arg_type != &TA_DynEnum) {
      arg_type = &TA_DynEnum;
      any_changes = true;
    }
  }
  return any_changes;
} 

bool ProgArg::UpdateFromType(TypeDef* td) {
  bool any_changes = false;
  if(arg_type != td) {
    arg_type = td;
    any_changes = true;
  }
  if(arg_type) {
    String ntyp = arg_type->Get_C_Name();
    if(type != ntyp) {
      type = ntyp;
      any_changes = true;
    }
  }
  return any_changes;
} 

String ProgArg::GetDisplayName() const {
  return type + " " + name + "=" + expr.GetFullExpr();
//  return expr.expr;
}

bool ProgArg::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgArg::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgArg::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

//////////////////////////
//   ProgArg_List	//
//////////////////////////

void ProgArg_List::Initialize() {
  SetBaseType(&TA_ProgArg);
  setUseStale(true);
}

void ProgArg_List::UpdateProgExpr_NewOwner() {
  for (int i = 0; i < size; ++ i) {
    ProgArg* pa = FastEl(i);
    pa->expr.UpdateProgExpr_NewOwner();
  }
}

void ProgArg_List::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  // make sure that once a def args is used, no more have values
  bool has_non_def = false;
  for (int i = size - 1; i >= 0; --i) {
    ProgArg* pa = FastEl(i);
    if (pa->required) break; // ok, no errors, and no more defs
    if (CheckError((has_non_def && pa->expr.empty()), quiet, rval,
      "Arg: " + pa->name + " cannot use default value because later args"
      " have a non-default value specified")) break;
    has_non_def = pa->expr.nonempty();
  }
}

const String ProgArg_List::GenCssBody_impl(int /*indent_level*/) {
  String rval = "(";
    for (int i = 0; i < size; ++ i) {
      ProgArg* pa = FastEl(i);
      //note: we test for violations of rules about def expressions, so
      // ok for us to just break here when we get to the first def
      if (!pa->required && pa->expr.empty()) break;
      pa->expr.ParseExpr();		// re-parse just to be sure!
      if (i > 0) rval += ", ";
      rval += pa->expr.GetFullExpr();
    }
  rval += ")";
  return rval;
}

bool ProgArg_List::UpdateFromVarList(ProgVar_List& targ) {
  bool any_changes = false;
  int i;  int ti;
  ProgArg* pa;
  ProgVar* pv;
  // delete args not in target; freshen those that are
  for (i = size - 1; i >= 0; --i) {
    pa = FastEl(i);
    pv = targ.FindName(pa->name);
    if(pv) {
      any_changes |= pa->UpdateFromVar(*pv);
    } else {
      RemoveIdx(i);
      any_changes = true;
    }
  }
  // add non-result args in target not in us, and put in the right order
  for (ti = 0; ti < targ.size; ++ti) {
    pv =targ.FastEl(ti);
    i = FindNameIdx(pv->name);
    if (i < 0) {
      pa = new ProgArg();
      pa->name = pv->name;
      pa->UpdateFromVar(*pv);
      Insert(pa, ti);
      any_changes = true;
    } else if (i != ti) {
      MoveIdx(i, ti);
      any_changes = true;
    }
  }
  return any_changes;
}

bool ProgArg_List::UpdateFromMethod(MethodDef* md) {
//NOTE: safe to call during loading
  bool any_changes = false;
  int i;  int ti;
  ProgArg* pa;
  // delete args not in md list
  for (i = size - 1; i >= 0; --i) {
    pa = FastEl(i);
    int ti = md->arg_names.FindEl(pa->name);
    if (ti >= 0) {
      any_changes |= pa->UpdateFromType(md->arg_types[ti]);
    } else {
      RemoveIdx(i);
      any_changes = true;
    }
  }
  // add args in target not in us, and put in the right order
  for (ti = 0; ti < md->arg_names.size; ++ti) {
    TypeDef* arg_typ = md->arg_types.FastEl(ti);
    String arg_nm = md->arg_names[ti];
    i = FindNameIdx(arg_nm);
    if (i < 0) {
      pa = new ProgArg();
      pa->name = arg_nm;
      pa->UpdateFromType(arg_typ);
      Insert(pa, ti);
      any_changes = true;
      //pa->expr.SetExpr(def_val); // set to this expr
    }
    else {
      pa = FastEl(i);
      if (i != ti) {
	MoveIdx(i, ti);
	any_changes = true;
      }
    }
    // have to do default for all, since it is not saved
    if(md->fun_argd < 0)
      pa->required = true;
    else
      pa->required = (md->fun_argd > ti);
    if (!pa->required) { // do default processing
      // get default value if available
      String def_val = md->arg_defs.SafeEl(ti);
      if (def_val.nonempty() && arg_typ->is_enum() && !def_val.contains("::")) {
        TypeDef* ot = arg_typ->GetOwnerType();
        if(ot)
          def_val = ot->name + "::" + def_val;
      }
      else if (arg_typ->InheritsFrom(TA_taString) ||
        ((arg_typ->ptr == 1) && arg_typ->InheritsFrom(&TA_char))) 
      {
        if(def_val.empty()) def_val = "\"\""; // empty string
      }
      pa->def_val = def_val;
    }
    else {			// required
      if(pa->arg_type->InheritsFormal(&TA_enum) && pa->expr.expr.empty()) {
	// pre-fill expr with lookup base for enum type -- makes lookup easier
	String eprfx = pa->arg_type->GetEnumPrefix();
	if(eprfx.nonempty()) {
	  TypeDef* ot = pa->arg_type->GetOwnerType();
	  if(ot) pa->expr.expr = ot->name + "::";
	  pa->expr.expr += eprfx;
	}
      }
    }
  }
  return any_changes;
}

bool ProgArg_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgArg_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgArg_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

//////////////////////////
//  ProgEl		//
//////////////////////////

bool ProgEl::StdProgVarFilter(void* base_, void* var_) {
  if(!base_) return true;
  ProgEl* base = static_cast<ProgEl*>(base_);
  ProgVar* var = static_cast<ProgVar*>(var_);
  if(!var->HasVarFlag(ProgVar::LOCAL_VAR)) return true; // definitely all globals
  Function* varfun = GET_OWNER(var, Function);
  if(!varfun) return true;	// not within a function, always go -- can't really tell scoping very well at this level -- could actually do it but it would be recursive and hairy
  Function* basefun = GET_OWNER(base, Function);
  if(basefun != varfun) return false; // different function scope
  return true;
}

bool ProgEl::ObjProgVarFilter(void* base_, void* var_) {
  bool rval = StdProgVarFilter(base_, var_);
  if(!rval) return false;
  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->var_type == ProgVar::T_Object);
}

bool ProgEl::DataProgVarFilter(void* base_, void* var_) {
  bool rval = ObjProgVarFilter(base_, var_);
  if(!rval) return false; // doesn't pass basic test

  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->object_type && var->object_type->InheritsFrom(&TA_DataTable));
}

bool ProgEl::DynEnumProgVarFilter(void* base_, void* var_) {
  bool rval = StdProgVarFilter(base_, var_);
  if(!rval) return false;
  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->var_type == ProgVar::T_DynEnum);
}

void ProgEl::Initialize() {
  flags = PEF_NONE;
  setUseStale(true);
}

void ProgEl::Destroy() {
}

void ProgEl::Copy_(const ProgEl& cp) {
  desc = cp.desc;
  flags = cp.flags;
}

void ProgEl::UpdateAfterMove_impl(taBase* old_owner) {
  inherited::UpdateAfterMove_impl(old_owner);

  if(!old_owner) return;
  Program* myprg = GET_MY_OWNER(Program);
  Program* otprg = (Program*)old_owner->GetOwner(&TA_Program);
  if(!myprg || !otprg || myprg == otprg) return;
  // don't update if not relevant

  // todo: this can now theoretically be done by UpdatePointers_NewPar_FindNew
  // but this was written first and it works..
  // automatically perform all necessary housekeeping functions!
  TypeDef* td = GetTypeDef();
  for(int i=0;i<td->members.size;i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_ProgExprBase)) {
      ProgExprBase* peb = (ProgExprBase*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgArg_List)) {
      ProgArg_List* peb = (ProgArg_List*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgExpr_List)) {
      ProgExpr_List* peb = (ProgExpr_List*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgVarRef)) {
      ProgVarRef* pvr = (ProgVarRef*)md->GetOff((void*)this);
      UpdateProgVarRef_NewOwner(*pvr);
    }
    else if(md->type->InheritsFrom(&TA_ProgramRef)) {
      ProgramRef* pvr = (ProgramRef*)md->GetOff((void*)this);
      if(pvr->ptr()) {
	Program_Group* mygp = GET_MY_OWNER(Program_Group);
	Program_Group* otgp = GET_OWNER(old_owner, Program_Group);
	Program_Group* pvgp = GET_OWNER(pvr->ptr(), Program_Group);
	if(mygp != otgp && (pvgp == otgp)) { // points to old group and we're in a new one
	  Program* npg = mygp->FindName(pvr->ptr()->name); // try to find new guy in my group
	  if(npg) pvr->set(npg);		    // set it!
	}
      }
    }
  }

  UpdatePointers_NewPar(otprg, myprg); // do the generic function to catch anything else..
  taProject* myproj = GET_OWNER(myprg, taProject);
  taProject* otproj = GET_OWNER(otprg, taProject);
  if(myproj && otproj && (myproj != otproj))
    UpdatePointers_NewPar(otproj, myproj);
  // then do it again if moving between projects
}

void ProgEl::UpdateAfterCopy(const ProgEl& cp) {
  Program* myprg = GET_MY_OWNER(Program);
  Program* otprg = (Program*)cp.GetOwner(&TA_Program);
  if(!myprg || !otprg || myprg == otprg || myprg->HasBaseFlag(taBase::COPYING)) return;
  // don't update if already being taken care of at higher level

  // todo: this can now theoretically be done by UpdatePointers_NewPar_FindNew
  // but this was written first and it works..
  // automatically perform all necessary housekeeping functions!
  TypeDef* td = GetTypeDef();
  for(int i=0;i<td->members.size;i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_ProgExprBase)) {
      ProgExprBase* peb = (ProgExprBase*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgArg_List)) {
      ProgArg_List* peb = (ProgArg_List*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgExpr_List)) {
      ProgExpr_List* peb = (ProgExpr_List*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgVarRef)) {
      ProgVarRef* pvr = (ProgVarRef*)md->GetOff((void*)this);
      UpdateProgVarRef_NewOwner(*pvr);
    }
    else if(md->type->InheritsFrom(&TA_ProgramRef)) {
      ProgramRef* pvr = (ProgramRef*)md->GetOff((void*)this);
      if(pvr->ptr()) {
	Program_Group* mygp = GET_MY_OWNER(Program_Group);
	Program_Group* otgp = GET_OWNER(otprg, Program_Group);
	Program_Group* pvgp = GET_OWNER(pvr->ptr(), Program_Group);
	if(mygp != otgp && (pvgp == otgp)) { // points to old group and we're in a new one
	  Program* npg = mygp->FindName(pvr->ptr()->name); // try to find new guy in my group
	  if(npg) pvr->set(npg);		    // set it!
	}
      }
    }
  }

  UpdatePointers_NewPar(otprg, myprg); // do the generic function to catch anything else..
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_taProject);
  // then do it again if moving between projects
}

void ProgEl::CheckError_msg(const char* a, const char* b, const char* c,
			    const char* d, const char* e, const char* f,
			    const char* g, const char* h) const {
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if(prg) prognm = prg->name;
  String objinfo = "Config Error in Program " + prognm + ": " + GetTypeDef()->name
    + " " + GetDisplayName() + " (path: " + GetPath(NULL, prg) + ")\n";
  taMisc::CheckError(objinfo, a, b, c, d, e, f, g, h);
}

bool ProgEl::CheckEqualsError(String& condition, bool quiet, bool& rval) {
  if(CheckError((condition.freq('=') == 1) && !(condition.contains(">=")
						|| condition.contains("<=")
						|| condition.contains("!=")),
		quiet, rval,
		"condition contains a single '=' assignment operator -- this is not the equals operator: == .  Fixed automatically")) {
    condition.gsub("=", "==");
    return true;
  }
  return false;
}

bool ProgEl::CheckProgVarRef(ProgVarRef& pvr, bool quiet, bool& rval) {
  if(!pvr) return false;
  Program* myprg = GET_MY_OWNER(Program);
  Program* otprg = GET_OWNER(pvr.ptr(), Program);
  if(CheckError(myprg != otprg, quiet, rval,
		"program variable:",pvr.ptr()->GetName(),"not in same program as me -- must be fixed!")) {
    return true;
  }
  return false;
}

bool ProgEl::UpdateProgVarRef_NewOwner(ProgVarRef& pvr) {
  ProgVar* cur_ptr = pvr.ptr();
  if(!cur_ptr) return false;
  String var_nm = cur_ptr->name;
  Program* my_prg = GET_MY_OWNER(Program);
  Program* ot_prg = GET_OWNER(cur_ptr, Program);
  if(!my_prg || !ot_prg || my_prg->HasBaseFlag(taBase::COPYING)) return false; // not updated
  Function* ot_fun = GET_OWNER(cur_ptr, Function);
  Function* my_fun = GET_MY_OWNER(Function);
  if(ot_fun && my_fun) {	       // both in functions
    if(my_fun == ot_fun) return false; // nothing to do
    String cur_path = cur_ptr->GetPath(NULL, ot_fun);
    MemberDef* md;
    ProgVar* pv = (ProgVar*)my_fun->FindFromPath(cur_path, md);
    if(pv && (pv->name == var_nm)) { pvr.set(pv); return true; }
    // ok, this is where we find same name or make one
    String cur_own_path = cur_ptr->owner->GetPath(NULL, ot_fun);
    taBase* pv_own_tab = my_fun->FindFromPath(cur_own_path, md);
    if(!pv_own_tab || !pv_own_tab->InheritsFrom(&TA_ProgVar_List)) {
      ProgVars* pvars = (ProgVars*)my_fun->fun_code.FindType(&TA_ProgVars);
      if(!pvars) {
	taMisc::Warning("Warning: could not find owner for program variable:", 
			var_nm, "in program:", my_prg->name, "on path:",
			cur_own_path, "setting var to null!");
	pvr.set(NULL);
	return false;
      }
      pv_own_tab = &(pvars->local_vars);
    }
    ProgVar_List* pv_own = (ProgVar_List*)pv_own_tab;
    pv = pv_own->FindName(var_nm);
    if(pv) { pvr.set(pv); return true; }	// got it!
    pv = my_fun->FindVarName(var_nm); // do more global search
    if(pv) { pvr.set(pv); return true; }	// got it!
    // now we need to add a clone of cur_ptr to our local list and use that!!
    pv = (ProgVar*)cur_ptr->Clone();
    pv_own->Add(pv);
    pv->CopyFrom(cur_ptr);	// somehow clone is not copying stuff -- try this
    pv->name = var_nm;		// just to be sure
    pv->DataChanged(DCR_ITEM_UPDATED);
    pvr.set(pv); // done!!
    taMisc::Info("Note: copied program variable:", 
		 var_nm, "from function:", ot_fun->name, "to function:",
		 my_fun->name, "because copied program element refers to it");
    taProject* myproj = GET_OWNER(my_prg, taProject);
    taProject* otproj = GET_OWNER(ot_prg, taProject);
    // update possible var pointers from other project!
    if(myproj && otproj && (myproj != otproj)) {
      pv->UpdatePointers_NewPar(otproj, myproj);
    }
  }
  else {
    if(my_prg == ot_prg) return false;	      // same program, no problem
    String cur_path = cur_ptr->GetPath(NULL, ot_prg);
    MemberDef* md;
    ProgVar* pv = (ProgVar*)my_prg->FindFromPath(cur_path, md);
    if(pv && (pv->name == var_nm)) { pvr.set(pv); return true; }
    // ok, this is where we find same name or make one
    String cur_own_path = cur_ptr->owner->GetPath(NULL, ot_prg);
    taBase* pv_own_tab = my_prg->FindFromPath(cur_own_path, md);
    if(!pv_own_tab || !pv_own_tab->InheritsFrom(&TA_ProgVar_List)) {
      taMisc::Warning("Warning: could not find owner for program variable:", 
		      var_nm, "in program:", my_prg->name, "on path:",
		      cur_own_path, "setting var to null!");
      pvr.set(NULL);
      return false;
    }
    ProgVar_List* pv_own = (ProgVar_List*)pv_own_tab;
    pv = pv_own->FindName(var_nm);
    if(pv) { pvr.set(pv); return true; }	// got it!
    pv = my_prg->FindVarName(var_nm); // do more global search
    if(pv) { pvr.set(pv); return true; }	// got it!
    // now we need to add a clone of cur_ptr to our local list and use that!!
    if(cur_ptr->objs_ptr && (bool)cur_ptr->object_val) {
      // copy the obj -- if copying var, by defn need to copy obj -- auto makes corresp var!
      taBase* varobj = cur_ptr->object_val.ptr();
      taBase* nwobj = varobj->Clone();
      nwobj->CopyFrom(varobj);	// should not be nec..
      nwobj->SetName(varobj->GetName()); // copy name in this case
      my_prg->objs.Add(nwobj);
      taMisc::Info("Note: copied program object:", 
		   varobj->GetName(), "from program:", ot_prg->name, "to program:",
		   my_prg->name, "because copied program element refers to it");
      pv = my_prg->FindVarName(var_nm); // get new var that was just created!
      if(pv) { pvr.set(pv); return true; }	// got it!
    }
    pv = (ProgVar*)cur_ptr->Clone();
    pv_own->Add(pv);
    pv->CopyFrom(cur_ptr);	// somehow clone is not copying stuff -- try this
    pv->name = var_nm;		// just to be sure
    pvr.set(pv); // done!!
    pv->DataChanged(DCR_ITEM_UPDATED);
    taMisc::Info("Note: copied program variable:", 
		 var_nm, "from program:", ot_prg->name, "to program:",
		 my_prg->name, "because copied program element refers to it");
    taProject* myproj = GET_OWNER(my_prg, taProject);
    taProject* otproj = GET_OWNER(ot_prg, taProject);
    // update possible var pointers from other project!
    if(myproj && otproj && (myproj != otproj)) {
      pv->UpdatePointers_NewPar(otproj, myproj);
    }
  }
  return true;
}

bool ProgEl::CheckConfig_impl(bool quiet) {
  if(HasProgFlag(OFF)) {
    ClearCheckConfig();
    return true;
  }
  return inherited::CheckConfig_impl(quiet);
}

void ProgEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  // automatically perform all necessary housekeeping functions!
  TypeDef* td = GetTypeDef();
  for(int i=0;i<td->members.size;i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_ProgVarRef)) {
      ProgVarRef* pvr = (ProgVarRef*)md->GetOff((void*)this);
      CheckProgVarRef(*pvr, quiet, rval);
    }
  }
}

void ProgEl::SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
				    int dcr, void* op1_, void* op2_) {
//NO!!!!! Does a UAE if content of ref changes; otherwise, don't need this
//  UpdateAfterEdit();		// just do this for all guys -- keeps display updated
}

const String ProgEl::GenCss(int indent_level) {
  if(HasProgFlag(OFF)) return "";
  String rval;
  if (useDesc() && !desc.empty()) {
    // we support multi-lines by using the multi-line form of comments
    if (desc.contains('\n')) {
      rval.cat(cssMisc::IndentLines("/* " + desc + " */\n", indent_level));
    } else {
      rval.cat(cssMisc::Indent(indent_level)).cat("// ").cat(desc).cat("\n");
    }
  }
  rval += GenCssPre_impl(indent_level);
  rval += GenCssBody_impl(indent_level);
  rval += GenCssPost_impl(indent_level);
  return rval;
}

const String ProgEl::GenListing(int indent_level) {
  String rval = Program::GetDescString(desc, indent_level);
  rval += cssMisc::Indent(indent_level) + GetDisplayName() + "\n";
  rval += GenListing_children(indent_level);
  return rval;
}

int ProgEl::GetEnabled() const {
  if(HasProgFlag(OFF)) return 0;
  ProgEl* par = parent();
  if (!par) return 1;
  if (par->GetEnabled())
    return 1;
  else return 0;
}

void ProgEl::SetEnabled(bool value) {
  SetProgFlagState(OFF, !value);
}


String ProgEl::GetStateDecoKey() const {
  String rval = inherited::GetStateDecoKey();
  if(rval.empty()) {
    if(HasProgFlag(NON_STD))
      return "ProgElNonStd";
    if(HasProgFlag(NEW_EL))
      return "ProgElNewEl";
    if(HasProgFlag(VERBOSE))
      return "ProgElVerbose";
  }
  return rval;
}

void ProgEl::SetOffFlag(bool off) {
  SetProgFlagState(OFF, off);
  DataChanged(DCR_ITEM_UPDATED);
}

void ProgEl::ToggleOffFlag() {
  ToggleProgFlag(OFF);
  DataChanged(DCR_ITEM_UPDATED);
}

void ProgEl::SetNonStdFlag(bool non_std) {
  SetProgFlagState(NON_STD, non_std);
  DataChanged(DCR_ITEM_UPDATED);
}

void ProgEl::ToggleNonStdFlag() {
  ToggleProgFlag(NON_STD);
  DataChanged(DCR_ITEM_UPDATED);
}

void ProgEl::SetNewElFlag(bool new_el) {
  SetProgFlagState(NEW_EL, new_el);
  DataChanged(DCR_ITEM_UPDATED);
}

void ProgEl::ToggleNewElFlag() {
  ToggleProgFlag(NEW_EL);
  DataChanged(DCR_ITEM_UPDATED);
}

void ProgEl::SetVerboseFlag(bool new_el) {
  SetProgFlagState(VERBOSE, new_el);
  DataChanged(DCR_ITEM_UPDATED);
}

void ProgEl::ToggleVerboseFlag() {
  ToggleProgFlag(VERBOSE);
  DataChanged(DCR_ITEM_UPDATED);
}

void ProgEl::PreGen(int& item_id) {
  if(HasProgFlag(OFF)) return;
  PreGenMe_impl(item_id);
  ++item_id;
  PreGenChildren_impl(item_id);
}

ProgVar* ProgEl::FindVarName(const String& var_nm) const {
  return NULL;
}

bool ProgEl::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgEl::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgEl::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

String ProgEl::GetColText(const KeyString& key, int itm_idx) const {
  String rval = inherited::GetColText(key, itm_idx);
  return rval.elidedTo(taMisc::program_editor_width);
}

const String ProgEl::GetToolTip(const KeyString& key) const {
  String rval = inherited::GetColText(key); // get full col text from tabase
  // include type names for further reference
  return String("(") + GetToolbarName() + " " + GetTypeName() + ") : " + rval;
}

String ProgEl::GetToolbarName() const {
  return "<base el>";
}


//////////////////////////
//   ProgEl_List	//
//////////////////////////

void ProgEl_List::Initialize() {
  SetBaseType(&TA_ProgEl);
  setUseStale(true);
}

void ProgEl_List::Destroy() {
  Reset();
}

void ProgEl_List::Copy_(const ProgEl_List& cp) {
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Program);
}

const String ProgEl_List::GenCss(int indent_level) {
  String rval;
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    rval += el->GenCss(indent_level); 
  }
  return rval;;
}

const String ProgEl_List::GenListing(int indent_level) {
  String rval;
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    rval += el->GenListing(indent_level); 
  }
  return rval;
}

String ProgEl_List::GetColHeading(const KeyString& key) const {
  static String col0("El Type");
  static String col1("El Description");
  if (key == key_type)  return col0;
  else if (key == key_disp_name) return col1;
  else return inherited::GetColHeading(key);
}

const KeyString ProgEl_List::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_type;
  case 1: return key_disp_name;
  default: return _nilKeyString;
  }
}


void ProgEl_List::PreGen(int& item_id) {
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    el->PreGen(item_id);
  }
}

ProgVar* ProgEl_List::FindVarName(const String& var_nm) const {
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    ProgVar* pv = el->FindVarName(var_nm);
    if(pv) return pv;
  }
  return NULL;
}

bool ProgEl_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgEl_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgEl_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

//////////////////////////
//  Loop		//
//////////////////////////

void Loop::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  loop_code.CheckConfig(quiet, rval);
}

const String Loop::GenCssBody_impl(int indent_level) {
  return loop_code.GenCss(indent_level + 1);
}

const String Loop::GenListing_children(int indent_level) {
  return loop_code.GenListing(indent_level + 1);
}

void Loop::PreGenChildren_impl(int& item_id) {
  loop_code.PreGen(item_id);
}
ProgVar* Loop::FindVarName(const String& var_nm) const {
  return loop_code.FindVarName(var_nm);
}

//////////////////////////
//    StaticMethodCall	//
//////////////////////////

void StaticMethodCall::Initialize() {
  method = NULL;
  min_type = &TA_taBase;
  object_type = &TA_taBase;
}

void StaticMethodCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

//  if(!taMisc::is_loading && method)
  if (method) { // needed to set required etc.
    if(meth_args.UpdateFromMethod(method)) { // changed
      if(taMisc::gui_active) {
	tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
      }
    }
  }
}

void StaticMethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!method, quiet, rval, "method is NULL");
}

void StaticMethodCall::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  meth_args.CheckConfig(quiet, rval);
}

const String StaticMethodCall::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!method) {
    rval += "//WARNING: StaticMethodCall not generated here -- object or method not specified\n";
    return rval;
  }
  
  if(IsVerbose()) {
    String argstmp = meth_args.GenCssBody_impl(indent_level);
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"calling static method " +
      object_type->name + "::" + method->name + argstmp.quote_esc() + "\");\n";
  }

  if (result_var)
    rval += result_var->name + " = ";
  rval += object_type->name;
  rval += "::";
  rval += method->name;
  rval += meth_args.GenCssBody_impl(indent_level);
  rval += ";\n";
  
  return rval;
}

String StaticMethodCall::GetDisplayName() const {
  if (!method)
    return "(method not selected)";
  
  String rval;
  if(result_var)
    rval += result_var->name + "=";
  rval += object_type->name;
  rval += "::";
  rval += method->name;
  rval += "(";
  for(int i=0;i<meth_args.size;i++) {
    ProgArg* pa = meth_args[i];
    if (i > 0)
      rval += ", ";
    rval += pa->expr.expr;
  }
  rval += ")";
  return rval;
}

//////////////////////////
//   ProgramCallBase	//
//////////////////////////

void ProgramCallBase::Initialize() {
}

void ProgramCallBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateArgs();		// always do this..  nondestructive and sometimes stuff changes anyway
  Program* prg = program();
  if(prg && !HasProgFlag(OFF)) {
    if(TestError(prg->init_code.IsParentOf(this), "UAE", "Cannot have a program call within init_code -- init_code should generally be very simple and robust code as it is not checked in advance of running (to prevent init-dependent Catch-22 scenarios) -- turning this call OFF"))
      SetProgFlag(OFF);

  }
}

void ProgramCallBase::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  prog_args.CheckConfig(quiet, rval);
  Program* prg = program();
  if(prg && !HasProgFlag(OFF)) {
    if(CheckError(prg->init_code.IsParentOf(this), quiet, rval, 
		  "Cannot have a program call within init_code -- init_code should generally be very simple and robust code as it is not checked in advance of running (to prevent init-dependent Catch-22 scenarios) -- turning this call OFF")) {
      SetProgFlag(OFF);
    }
  }
}

const String ProgramCallBase::GenCssArgSet_impl(const String trg_var_nm, int indent_level) {
  Program* trg = GetTarget_Compile();
  if(!trg) return _nilString;

  String rval;
  if (prog_args.size > 0) {
    rval += cssMisc::Indent(indent_level+1);
    rval += "// set global vars of target\n";
  }
  String nm;
  bool set_one = false;
  for (int i = 0; i < prog_args.size; ++i) {
    ProgArg* ths_arg = prog_args.FastEl(i);
    nm = ths_arg->name;
    ProgVar* prg_var = trg->args.FindName(nm);
    ths_arg->expr.ParseExpr();		// re-parse just to be sure!
    String argval = ths_arg->expr.GetFullExpr();
    if (!prg_var || argval.empty() || argval == "<no_arg>") continue; 
    set_one = true;
    rval += cssMisc::Indent(indent_level+1);
    rval += trg_var_nm + "->SetVar(\"" + prg_var->name + "\", " + argval + ");\n";
  }
  return rval;
}

void ProgramCallBase::UpdateArgs() {
  Program* trg = GetTarget_Compile();
  if(!trg) return;

  bool any_changes = prog_args.UpdateFromVarList(trg->args);
  // now go through and set default value for variables of same name in this program
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return;
  for(int i=0;i<prog_args.size; i++) {
    ProgArg* pa = prog_args.FastEl(i);
    if(!pa->expr.expr.empty()) continue; // skip if already set
    ProgVar* arg_chk = prg->args.FindName(pa->name);
    ProgVar* var_chk = prg->vars.FindName(pa->name);
    if(!arg_chk && !var_chk) continue; 
    pa->expr.SetExpr(pa->name);	// we found var of same name; set as arg value
    pa->DataChanged(DCR_ITEM_UPDATED);
  }
  if(any_changes && taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
  }
}

//////////////////////////
//   ProgramCall	//
//////////////////////////

void ProgramCall::Initialize() {
}

void ProgramCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!taMisc::is_loading) {
    if((bool)target) {
      String targ_ld_i = targ_ld_init.between("*", "*");
      if(targ_ld_init.empty() || !target.ptr()->GetName().contains(targ_ld_i)) {
	targ_ld_init = String("*") + target.ptr()->GetName() + "*"; // make it wild!
      }
    }
  }
}

void ProgramCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!target, quiet, rval, "target is NULL");
}

bool ProgramCall::CallsProgram(Program* prg) {
  return target.ptr() == prg;
}

Program* ProgramCall::GetTarget() {
  if(!target) {
    taMisc::CheckError("Program target is NULL in ProgramCall:",
		       desc, "in program:", program()->name);
  }
  // NOTE: we are assuming that it has been compiled from the Init process
  // calling here causes many recompiles because of the dirty bit propagation
  // during running
//   if(!target->CompileScript()) {
//     taMisc::CheckError("Program target script did not compile correctly in ProgramCall:",
// 		       desc, "in program:", program()->name);
//   }
  return target.ptr();
}

Program* ProgramCall::GetTarget_Compile() {
  return target.ptr();
}

void ProgramCall::PreGenMe_impl(int item_id) {
  // register as a subproc
  Program* prog = program();
  if (!prog) return; // shouldn't normally happen

  Program* trg = target.ptr();
  if(!trg || (prog->sub_progs_dir.FindEl(trg) < 0)) {
    // link in the call if targ is unique or null
    prog->sub_prog_calls.LinkUnique(this);
    if(trg) prog->sub_progs_dir.LinkUnique(trg); // add direct sub-prog
  }
}

void ProgramCall::AddTargetsToListAll(Program_List& all_lst) {
  Program* trg = target.ptr();
  if(trg) {
    all_lst.LinkUnique(trg);
  }
}

void ProgramCall::SetTarget(Program* target_) {
  target = target_;
}

const String ProgramCall::GenCompileScript(Program* prg) {
  String rval;
  if(!target) return rval;
  rval += "    target = this" + GetPath(NULL, prg) + "->GetTarget();\n";
  rval += "    target->CompileScript(true); // true = force!\n";
  return rval;
}

const String ProgramCall::GenCallInit(Program* prg) {
  String rval;
  if(!target) return rval;
  rval += "    target = this" + GetPath(NULL, prg) + "->GetTarget();\n";
  // set args for guys that are just passing our existing args/vars along
  for (int j = 0; j < prog_args.size; ++j) {
    ProgArg* ths_arg = prog_args.FastEl(j);
    ProgVar* prg_var = target->args.FindName(ths_arg->name);
    String argval = ths_arg->expr.GetFullExpr();
    if (!prg_var || argval.empty()) continue;
    // check to see if the value of this guy is an arg or var of this guy -- if so, propagate it
    ProgVar* arg_chk = prg->args.FindName(argval);
    ProgVar* var_chk = prg->vars.FindName(argval);
    if(!arg_chk && !var_chk) continue; 
    rval += "    target->SetVar(\"" + prg_var->name + "\", "
      + argval + ");\n";
  }
  rval += "    ret_val = target->CallInit(this);\n"; 
  return rval;
}

const String ProgramCall::GenCssPre_impl(int indent_level) {
  String rval;
  rval = cssMisc::Indent(indent_level);
  rval += "{ // call program: "; 
  if (target)
    rval += target->name;
  rval += "\n";
  return rval;
}

const String ProgramCall::GenCssBody_impl(int indent_level) {
  if (!target) return _nilString;
  String rval;
  indent_level++;		// everything is indented from outer block
  rval += cssMisc::Indent(indent_level);
  rval += "Program* target = this" + GetPath(NULL, program())+ "->GetTarget();\n";
  rval += cssMisc::Indent(indent_level);
  rval += "if(target) {\n";

  if(IsVerbose()) {
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"calling program\",target->name);\n";
  }

  rval += GenCssArgSet_impl("target", indent_level);

  rval += cssMisc::Indent(indent_level+1);
  rval += "{ target->Call(this); }\n";
  rval += cssMisc::Indent(indent_level);
  rval += "}\n";
  
  return rval;
}

const String ProgramCall::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "} // call program\n";
}

String ProgramCall::GetDisplayName() const {
  String rval = "Call ";
  if (target) {
    rval += target->GetName();
    if(prog_args.size > 0) {
      rval += "(";
      for(int i=0;i<prog_args.size;i++) {
	ProgArg* pa = prog_args.FastEl(i);
	if(i > 0) rval += ", ";
	rval += pa->expr.expr;   // GetDisplayName();
      }
      rval += ")";
    }
  }
  else
    rval += "(no program set)";
  return rval;
}

bool ProgramCall::LoadInitTarget() {
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return false;

  target.set(NULL);		// default is always to start off empty
  bool got = false;
  if(targ_ld_init.contains(',')) {
    String nm = targ_ld_init;
    while(nm.contains(',')) {
      String snm = nm.before(',');
      nm = nm.after(',');
      while(nm.firstchar() == ' ') nm = nm.after(' ');
      got = LoadInitTarget_impl(snm);
      if(got) break;
      if(!nm.contains(','))	// get last guy
	got = LoadInitTarget_impl(nm);
    }
  }
  else {
    got = LoadInitTarget_impl(targ_ld_init);
  }
  if(!got) {
    taMisc::Warning("ProgramCall in program:", prg->name,
		    "could not find load init target program to call named:",
		    targ_ld_init, "target is set to NULL and must be fixed manually!");
  }
  return got;
}

bool ProgramCall::LoadInitTarget_impl(const String& nm) {
  if(nm.empty()) return false;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return false;

  Program* tv = NULL;
  if(nm.contains("*")) {
    String nnm = nm;
    nnm.gsub("*","");
    tv = prg->FindProgramNameContains(nnm, false);
  }
  else {
    tv = prg->FindProgramName(nm, false);
  }
  if(tv) {
    target.set(tv);
    return true;
  }
  return false;
}


//////////////////////////
//   ProgramCallVar	//
//////////////////////////

void ProgramCallVar::Initialize() {
}

void ProgramCallVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void ProgramCallVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!prog_group, quiet, rval, "prog_group is NULL");
  CheckError(!prog_name_var, quiet, rval, "prog_name_var is NULL");
  CheckError(prog_name_var->var_type != ProgVar::T_String, quiet, rval,
	     "prog_name_var is not a String type -- must be");
  // todo: check all args!
}

bool ProgramCallVar::CallsProgram(Program* prg) {
  if(!prog_group) return false;
  return (prog_group->IsParentOf(prg));
}

Program_Group* ProgramCallVar::GetProgramGp() {
  if(!prog_group) {
    taMisc::CheckError("Program_Group prog_group is NULL in ProgramCallVar:",
		       desc, "in program:", program()->name);
    return NULL;
  }
  return prog_group;
}

Program* ProgramCallVar::GetTarget() {
  if(!prog_group) {
    taMisc::CheckError("Program_Group prog_group is NULL in ProgramCallVar:",
		       desc, "in program:", program()->name);
    return NULL;
  }
  if(!prog_name_var) {
    taMisc::CheckError("prog_name_var is NULL in ProgramCallVar:",
		       desc, "in program:", program()->name);
    return NULL;
  }
  String pnm = prog_name_var->string_val;
  Program* rval = prog_group->FindLeafName(pnm);
  if(!rval) {
    taMisc::CheckError("Program named:", pnm, "not found in Program_Group:",
		       prog_group->name, "path:", prog_group->GetPath(),
		       "in ProgramCallVar in program:", program()->name);
    return NULL;
  }
  return rval;
}

Program* ProgramCallVar::GetTarget_Compile() {
  if(!prog_group) {
    return NULL;
  }
  if(prog_group->leaves == 0) return NULL;
  return prog_group->Leaf(0);	// just return first guy
}

void ProgramCallVar::PreGenMe_impl(int item_id) {
  // register as a subproc
  Program* prog = program();
  if (!prog) return; // shouldn't normally happen

  Program* trg = GetTarget_Compile();
  if(!trg || (prog->sub_progs_dir.FindEl(trg) < 0)) {
    // link in the call if targ is unique or null
    prog->sub_prog_calls.LinkUnique(this);
    if(trg) {
      for(int j=0;j<prog_group->leaves;j++) {
	Program* strg = prog_group->Leaf(j);
	prog->sub_progs_dir.LinkUnique(strg); // add direct sub-progs
      }
    }
  }
}

void ProgramCallVar::AddTargetsToListAll(Program_List& all_lst) {
  if(!prog_group) return;
  for(int j=0;j<prog_group->leaves;j++) {
    Program* strg = prog_group->Leaf(j);
    all_lst.LinkUnique(strg);
  }
}

const String ProgramCallVar::GenCompileScript(Program* prg) {
  String rval;
  if(!prog_group) return rval;
  rval += "    Program_Group* spgp = this" + GetPath(NULL, prg) + "->GetProgramGp();\n";
  rval += "    for(int spi=0; spi<spgp->leaves; spi++) {\n";
  rval += "      Program* prg = spgp->Leaf(spi);\n";
  rval += "      prg->CompileScript(true); // true = force!\n";
  rval += "    }\n";
  return rval;
}

const String ProgramCallVar::GenCallInit(Program* prg) {
  String rval;
  if(!prog_group) return rval;
  Program* trg = GetTarget_Compile();
  if(!trg) return rval;

  rval += "    Program_Group* spgp = this" + GetPath(NULL, prg) + "->GetProgramGp();\n";
  rval += "    for(int spi=0; spi<spgp->leaves; spi++) {\n";
  rval += "      Program* prg = spgp->Leaf(spi);\n";

  for (int j = 0; j < prog_args.size; ++j) {
    ProgArg* ths_arg = prog_args.FastEl(j);
    ProgVar* prg_var = trg->args.FindName(ths_arg->name);
    String argval = ths_arg->expr.GetFullExpr();
    if (!prg_var || argval.empty()) continue;
    // check to see if the value of this guy is an arg or var of this guy -- if so, propagate it
    ProgVar* arg_chk = prg->args.FindName(argval);
    ProgVar* var_chk = prg->vars.FindName(argval);
    if(!arg_chk && !var_chk) continue; 
    rval += "      prg->SetVar(\"" + prg_var->name + "\", "
      + argval + ");\n";
  }
  rval += "      ret_val = prg->CallInit(this);\n"; 
  rval += "    }\n";
  return rval;
}

const String ProgramCallVar::GenCssPre_impl(int indent_level) {
  String rval;
  rval = cssMisc::Indent(indent_level);
  rval += "{ // call program from var (name) in group: "; 
  if(prog_group)
    rval += prog_group->name;
  rval += "\n";
  return rval;
}

const String ProgramCallVar::GenCssBody_impl(int indent_level) {
  if(!prog_group) return _nilString;
  String rval;
  indent_level++;		// everything is indented from outer block
  rval += cssMisc::Indent(indent_level);
  rval += "Program* target = this" + GetPath(NULL, program())+ "->GetTarget();\n";
  rval += cssMisc::Indent(indent_level);
  rval += "if(target) {\n";

  if(IsVerbose()) {
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"calling program\",target->name);\n";
  }

  rval += GenCssArgSet_impl("target", indent_level);

  rval += cssMisc::Indent(indent_level+1);
  rval += "{ target->Call(this); }\n";
  rval += cssMisc::Indent(indent_level);
  rval += "}\n";
  
  return rval;
}

const String ProgramCallVar::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "} // call program fm var\n";
}

String ProgramCallVar::GetDisplayName() const {
  String rval = "Call Fm ";
  if (prog_group) {
    rval += prog_group->GetName();
    if(prog_name_var) {
      rval += " " + prog_name_var->name;
    }
    if(prog_args.size > 0) {
      rval += "(";
      for(int i=0;i<prog_args.size;i++) {
	ProgArg* pa = prog_args.FastEl(i);
	if(i > 0) rval += ", ";
	rval += pa->expr.expr;   // GetDisplayName();
      }
      rval += ")";
    }
  }
  else
    rval += "(no program group set)";
  return rval;
}

//////////////////////////
//   Function_List	//
//////////////////////////

void Function_List::Initialize() {
  SetBaseType(&TA_Function);
  setUseStale(true);
}

void Function_List::Destroy() {
  Reset();
}

void Function_List::Copy_(const Function_List& cp) {
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Program);
}

const String Function_List::GenCss(int indent_level) {
  String rval;
  for (int i = 0; i < size; ++i) {
    Function* el = FastEl(i);
    rval += el->GenCss(indent_level); 
  }
  return rval;;
}

const String Function_List::GenListing(int indent_level) {
  String rval;
  for (int i = 0; i < size; ++i) {
    Function* el = FastEl(i);
    rval += el->GenListing(indent_level); 
  }
  return rval;
}

String Function_List::GetColHeading(const KeyString& key) const {
  static String col0("El Type");
  static String col1("El Description");
  if (key == key_type)  return col0;
  else if (key == key_disp_name) return col1;
  else return inherited::GetColHeading(key);
}

const KeyString Function_List::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_type;
  case 1: return key_disp_name;
  default: return _nilKeyString;
  }
}


void Function_List::PreGen(int& item_id) {
  for (int i = 0; i < size; ++i) {
    Function* el = FastEl(i);
    el->PreGen(item_id);
  }
}

ProgVar* Function_List::FindVarName(const String& var_nm) const {
  for (int i = 0; i < size; ++i) {
    Function* el = FastEl(i);
    ProgVar* pv = el->FindVarName(var_nm);
    if(pv) return pv;
  }
  return NULL;
}

bool Function_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool Function_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool Function_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}


//////////////////////////
//  Function		//
//////////////////////////

void Function::Initialize() {
  return_type = ProgVar::T_Int;
  object_type = &TA_taOBase;
  args.var_context = ProgVar_List::VC_FuncArgs;
}

void Function::InitLinks() {
  inherited::InitLinks();
  taBase::Own(args, this);
  taBase::Own(fun_code, this);

  if(!taMisc::is_loading) {
    if(fun_code.size == 0) {
      //      ProgVars* pv = (ProgVars*)
      fun_code.New(1, &TA_ProgVars); // make this by default because it is typically needed!
    }
  }
}

void Function::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  name = taMisc::StringCVar(name); // make names C legal names
  if(Program::IsForbiddenName(name))
    name = "My" + name;
}

void Function::UpdateAfterCopy(const ProgEl& cp) {
  inherited::UpdateAfterCopy(cp);
  Program* myprg = GET_MY_OWNER(Program);
  Program* otprg = (Program*)cp.GetOwner(&TA_Program);
  if(myprg && otprg && myprg == otprg && !myprg->HasBaseFlag(taBase::COPYING)) {
    // if within the same program, we need to update the *function* pointers 
    UpdatePointers_NewPar((taBase*)&cp, this); // update any pointers within this guy
  }
}

void Function::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(name.empty(), quiet, rval, "name is empty -- functions must be named");
  if(CheckError(Program::IsForbiddenName(name, false), quiet, rval,
		"Name:",name,"is forbidden -- choose another"))
    name = "My" + name;
  Function_List* flo = GET_MY_OWNER(Function_List);
  CheckError(!flo, quiet, rval, "Function must only be in .functions -- cannot be in .prog_code or .init_code -- this is the DEFINITION of the function, not calling the function (which is FunctionCall)");
}

void Function::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  args.CheckConfig(quiet, rval);
  fun_code.CheckConfig(quiet, rval);
}

const String Function::GenCssBody_impl(int indent_level) {
  ProgVar rvt; rvt.var_type = return_type;  rvt.object_type = object_type;
  String rval;
  rval += cssMisc::Indent(indent_level) + rvt.GenCssType() + " " + name + "(";
  if(args.size > 0) {
    rval += args.GenCss(0);
  }
  rval += ") {\n";
  rval += fun_code.GenCss(indent_level + 1);
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval;
}

const String Function::GenListing_children(int indent_level) {
  return fun_code.GenListing(indent_level + 1);
}

String Function::GetDisplayName() const {
  ProgVar rvt; rvt.var_type = return_type;  rvt.object_type = object_type;
  String rval;
  rval += rvt.GenCssType() + " " + name + "(";
  if(args.size > 0) {
    rval += args.GenCss(0);
  }
  rval += ")";
  return rval;
}

void Function::PreGenChildren_impl(int& item_id) {
  fun_code.PreGen(item_id);
}
ProgVar* Function::FindVarName(const String& var_nm) const {
  ProgVar* pv = args.FindName(var_nm);
  if(pv) return pv;
  return fun_code.FindVarName(var_nm);
}

void Function::UpdateCallerArgs() {
  Program* prog = program();
  if(!prog) return;
  
  taBase_PtrList fc_items;
  prog->Search("FunctionCall", fc_items, NULL,
	       false, // contains
	       true, // case_sensitive
	       false, // obj_name
	       true,   // obj_type
	       false,  // obj_desc
	       false,  // obj_val
	       false,  // mbr_name
	       false);  // type_desc

  for(int i=0;i<fc_items.size; i++) {
    taBase* it = fc_items[i];
    if(!it || !it->InheritsFrom(&TA_FunctionCall)) continue;
    FunctionCall* fc = (FunctionCall*)it;
    if(fc->fun.ptr() == this) {
      fc->UpdateArgs();
    }
  }
  prog->GuiFindFromMe(name);	// find all refs to me
}

//////////////////////////
//   FunctionCall	//
//////////////////////////

void FunctionCall::Initialize() {
}

void FunctionCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateArgs();		// always do this.. nondestructive and sometimes stuff changes anyway
}

void FunctionCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!fun, quiet, rval, "fun is NULL");
  if(fun) {
    Program* myprg = GET_MY_OWNER(Program);
    Program* otprg = GET_OWNER(fun.ptr(), Program);
    CheckError(myprg != otprg, quiet, rval,
	       "function call to:",fun.ptr()->GetName(),"not in same program as me -- must be fixed!");
  }
}

void FunctionCall::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  fun_args.CheckConfig(quiet, rval);
}

const String FunctionCall::GenCssBody_impl(int indent_level) {
  if (!fun) return _nilString;
  String rval;

  if(IsVerbose()) {
    String argstmp = fun_args.GenCssBody_impl(indent_level);
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"calling function " +
      fun->name + argstmp.quote_esc() + "\");\n";
  }

  rval += cssMisc::Indent(indent_level);
  if(result_var) {
    rval += result_var->name + "=";
  }
  rval += fun->name + "(";
  for (int i = 0; i < fun_args.size; ++i) {
    ProgArg* ths_arg = fun_args.FastEl(i);
    if(i > 0) rval += ", ";
    ths_arg->expr.ParseExpr();		// re-parse just to be sure!
    rval += ths_arg->expr.GetFullExpr();
  }
  rval += ");\n";
  return rval;
}

String FunctionCall::GetDisplayName() const {
  String rval;
  if(result_var)
    rval += result_var->name + "=";
  else
    rval += "Call ";
  if (fun) {
    rval += fun->name;
    if(fun_args.size > 0) {
      rval += "(";
      for(int i=0;i<fun_args.size;i++) {
        ProgArg* pa = fun_args.FastEl(i);
	if(i > 0) rval += ", ";
        rval += pa->expr.expr;  // GetDisplayName();
      }
      rval += ")";
    }
  } else
    rval += "(no function set)";
  return rval;
}

void FunctionCall::UpdateArgs() {
  if(!fun) return; // just leave existing stuff for now
  fun_args.UpdateFromVarList(fun->args);
}

///////////////////////////////////////////////////////////////
//  	ProgObjList


DataTable* ProgObjList::NewDataTable(int n_tables) {
  return (DataTable*)New_gui(n_tables, &TA_DataTable); // this is a gui op
}

void ProgObjList::GetVarsForObjs() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return;
  for(int i = 0; i < size; ++i) {
    taBase* obj = FastEl(i);
    String nm = obj->GetName();
    if(nm.empty()) continue;
    ProgVar* var = prog->vars.FindName(nm);
    if(var) {
      if((var->var_type != ProgVar::T_Object) || (var->object_val != obj)) {
	taMisc::Error("Program error: variable named:", nm,
		      "exists, but does not refer to object in objs list -- rename either to avoid conflict");
      }
      else {
	var->objs_ptr = true;	// make sure
	var->object_type = obj->GetTypeDef();
        var->DataChanged(DCR_ITEM_UPDATED);
      }
    }
    else {
      bool found_it = false;
      for(int j=0;j<prog->vars.size; j++) {
	ProgVar* tv = prog->vars[j];
	if((tv->var_type == ProgVar::T_Object) && (tv->object_val.ptr() == obj)) {
	  found_it = true;
	  tv->name = nm;	// update the name
	  tv->objs_ptr = true;	// make sure
	  tv->object_type = obj->GetTypeDef();
	  tv->UpdateAfterEdit(); // need UAE to update schema sig to cascade to progvar
	  //	  tv->DataChanged(DCR_ITEM_UPDATED);
	  break;
	}
      }
      if(!found_it) {
	var = (ProgVar*)prog->vars.New(1, &TA_ProgVar);
	var->name = nm;
	var->var_type = ProgVar::T_Object;
	var->object_val = obj;
	var->objs_ptr = true;
	var->object_type = obj->GetTypeDef();
	var->ClearVarFlag(ProgVar::CTRL_PANEL); // don't show in ctrl panel by default
        var->DataChanged(DCR_ITEM_UPDATED);
      }
    }
  }
  // now cleanup any orphaned 
  for(int i = prog->vars.size-1; i >= 0; --i) {
    ProgVar* var = prog->vars[i];
    if(!var->objs_ptr) continue;
    taBase* obj = FindName(var->name);
    if(obj == NULL)
      prog->vars.RemoveIdx(i);		// get rid of it
  }
}

void ProgObjList::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if(!taMisc::is_loading && !taMisc::is_duplicating && !isDestroying())
    GetVarsForObjs();
}

void ProgObjList::StructUpdateEls(bool begin) {
  for(int i=0;i<size;i++) {
    taBase* itm = FastEl(i);
    itm->StructUpdate(begin);
  }
}

void* ProgObjList::El_Own_(void* it_) {
  // note: gen the name first, so we don't need to do another notify
  // note: setting default name is mostly for things like taMatrix
  // that are normally anon, but that would like a name here in objs
  taBase* it = (taBase*)it_;
  if (it && it->GetName().empty()) {
    // first, try the default token-based naming
    it->SetDefaultName_();
    // if still empty, we could gen a synthetic name
    if (it->GetName().empty()) {
      it->SetName("Obj_" + String(size + 1));
    }
  }
  void* rval = inherited::El_Own_(it_);
  return rval;
}


//////////////////////////
//  Program		//
//////////////////////////

ProgLib* Program::prog_lib = NULL;
String_Array Program::forbidden_names;
bool Program::stop_req = false;
Program::StopReason Program::stop_reason = Program::SR_NONE;
String Program::stop_msg;
bool Program::step_mode = false;
ProgramRef Program::cur_step_prog;
int Program::cur_step_n = 1;
int Program::cur_step_cnt = 0;
Program::RunState Program::global_run_state = Program::NOT_INIT;

void Program::Initialize() {
  run_state = NOT_INIT;
  flags = PF_NONE;
  objs.SetBaseType(&TA_taNBase);
  ret_val = 0;
  sub_progs_updtd = false;
  m_stale = true; 
  prog_gp = NULL;
  m_checked = false;
  step_n = 1;
  if(!prog_lib)
    prog_lib = &Program_Group::prog_lib;
}

void Program::Destroy()	{ 
  CutLinks();
}

void Program::InitLinks() {
  inherited::InitLinks();
  taBase::Own(objs, this);
  taBase::Own(types, this);
  taBase::Own(args, this);
  taBase::Own(vars, this);
  taBase::Own(functions, this);
  taBase::Own(load_code, this);
  taBase::Own(init_code, this);
  taBase::Own(prog_code, this);
  taBase::Own(sub_prog_calls, this);
  taBase::Own(sub_progs_dir, this);
  taBase::Own(sub_progs_all, this);
  taBase::Own(sub_progs_step, this);
  taBase::Own(step_prog, this);
  prog_gp = GET_MY_OWNER(Program_Group);
  if(forbidden_names.size == 0)
    InitForbiddenNames();
}

void Program::CutLinks() {
  if(script) {			// clear first, before trashing anything!
    ExitShellScript();
    script->ClearAll();
    script->prog_vars.Reset();
    delete script;
    script = NULL;
  }
  step_prog.CutLinks();
  sub_progs_step.CutLinks();
  sub_progs_all.CutLinks();
  sub_progs_dir.CutLinks();
  sub_prog_calls.CutLinks();
  prog_code.CutLinks();
  load_code.CutLinks();
  init_code.CutLinks();
  functions.CutLinks();
  vars.CutLinks();
  args.CutLinks();
  types.CutLinks();
  objs.CutLinks();
  prog_gp = NULL;
  inherited::CutLinks();
}

void Program::Reset() {
  if(script) {			// clear first, before trashing anything!
    script->ClearAll();
    script->prog_vars.Reset();
  }
  sub_progs_step.Reset();
  sub_progs_all.Reset();
  sub_progs_dir.Reset();
  sub_prog_calls.Reset();
  prog_code.Reset();
  load_code.Reset();
  init_code.Reset();
  functions.Reset();
  vars.Reset();
  args.Reset();
  types.Reset();
  objs.Reset();
}

void Program::Copy_(const Program& cp) {
  if(script) {			// clear first, before trashing anything!
    script->ClearAll();
    script->prog_vars.Reset();
  }
  tags = cp.tags;
  desc = cp.desc;
  flags = cp.flags;
  objs = cp.objs;
  types = cp.types;
  args = cp.args;
  vars = cp.vars;
  functions = cp.functions;
  load_code = cp.load_code;
  init_code = cp.init_code;
  prog_code = cp.prog_code;
  step_prog = cp.step_prog;
  ret_val = 0; // redo
  m_stale = true; // require rebuild/refetch
  m_scriptCache = "";
  m_checked = false; // redo
  sub_prog_calls.RemoveAll();
  sub_progs_step.RemoveAll();
  sub_progs_all.RemoveAll();
  sub_progs_dir.RemoveAll();
  UpdatePointers_NewPar((taBase*)&cp, this); // update any pointers within this guy
  UpdatePointers_NewPar_IfParNotCp((taBase*)&cp, &TA_taProject); // also check for project copy
}

void Program::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //WARNING: the running css prog calls this on any changes to our vars,
  // such as ret_val -- therefore, DO NOT do things here that are incompatible
  // with the runtime, in particular, do NOT invalidate the following state flags:
  //   m_stale, script_compiled

  if(HasProgFlag(LOCKED)) SetBaseFlag(BF_GUI_READ_ONLY);
  else			  ClearBaseFlag(BF_GUI_READ_ONLY);
  
  //TODO: the following *do* affect generated script, so we should probably call
  // setDirty(true) if not running, and these changed:
  // name, (more TBD...)

  if(step_n < 1) step_n = 1;

  if(!step_prog) {
    if(sub_progs_step.size > 0)
      step_prog = sub_progs_step.Peek(); // set to last guy on list..
  }

  if(short_nm.empty()) {
    String use_nm = name;
    // todo: this is hacky and emergent-dependent...
    if(use_nm.startsWith("Leabra")) use_nm = use_nm.after("Leabra");
    if(use_nm.startsWith("Bp")) use_nm = use_nm.after("Bp");
    if(use_nm.startsWith("Cs")) use_nm = use_nm.after("Cs");
    if(use_nm.startsWith("So")) use_nm = use_nm.after("So");
    if(use_nm.length() > 8)
      use_nm = taMisc::RemoveVowels(use_nm);
    short_nm = taMisc::ShortName(use_nm);
  }
}

bool Program::CheckConfig_impl(bool quiet) {
  //TODO: global program dependencies and other objects -- check them all here
  bool rval = inherited::CheckConfig_impl(quiet);
  m_checked = true;
  if (!rval) ret_val = RV_CHECK_ERR;
  return rval;
}


void Program::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  objs.CheckConfig(quiet, rval);
  types.CheckConfig(quiet, rval);
  args.CheckConfig(quiet, rval);
  vars.CheckConfig(quiet, rval);
  functions.CheckConfig(quiet, rval);
  load_code.CheckConfig(quiet, rval);
  init_code.CheckConfig(quiet, rval);
  prog_code.CheckConfig(quiet, rval);
}

int Program::Call(Program* caller) {
  run_state = RUN;		// just a local update
  int rval = Cont_impl();
  if(stop_req) {
    script->Stop();		// stop us
    caller->script->Stop();	// stop caller!
    caller->script->Prog()->Frame()->pc = 0;
    run_state = STOP;		// we are done
    // NOTE: this backs up to restart the entire call to fun -- THIS DEPENDS ON THE CODE
    // that generates the call!!!!!  ALWAYS MUST BE IN A SUB-BLOCK of code..
  }
  else {
    script->Restart();		// restart script at beginning if run again	
    run_state = DONE;		// we are done
  }
  return rval;
} 

int Program::CallInit(Program* caller) {
  run_state = INIT;    // this is redundant if called from an existing INIT but otherwise needed
  Run_impl();
  CheckConfig(false);	// check after running!  see below
  script->Restart(); 	// for init, always restart script at beginning if run again	
  if(!taMisc::check_ok)
    run_state = NOT_INIT;
  else
    run_state = DONE;
  return ret_val;
} 

void Program::Init() {
//   cur_step_prog = NULL;  // if a program calls Init() directly, this will prevent stepping
  // it is not clear if we really need to clear this setting here
  ClearStopReq();		// NOTE: newly added 4/18/09 -- check for breakage..
  taProject* proj = GET_MY_OWNER(taProject);
  if(proj && proj->file_name.nonempty()) {
    QFileInfo fi(proj->file_name); // set to current working dir on init
    QDir::setCurrent(fi.absolutePath());
  }
  taMisc::Busy();
  SetRunState(INIT);
  DataChanged(DCR_ITEM_UPDATED_ND); // update button state
  // first run the Init code, THEN do the check.  this prevents a catch-22
  // with Init code that is designed to configure things so there won't be 
  // config errors!!  It exposes init code to the possibility of 
  // running unchecked code, so we need to make sure all progel's have 
  // an extra compile-time check and don't just crash (fail quietly -- err will showup later)
  taMisc::CheckConfigStart(false); // CallInit will do CheckConfig..
  if(!CompileScript(true)) {
    if (ret_val != RV_CHECK_ERR)
      ret_val = RV_COMPILE_ERR;
  }
  if(ret_val == RV_OK) {
    bool did_struct_updt = false;
    if(!HasProgFlag(OBJS_UPDT_GUI)) {
      objs.StructUpdateEls(true);
      did_struct_updt = true;
    }
    script->Run();
    if(did_struct_updt)
      objs.StructUpdateEls(false);
  }

  // get these here after all the sub-guys have been initialized -- should now be current
  GetSubProgsAll();
  GetSubProgsStep();

  taMisc::DoneBusy();
  // now check us..
  CheckConfig(false);
  taMisc::CheckConfigEnd(); // no flag, because any nested fails will have set it
  if (ret_val != RV_OK) ShowRunError();
  script->Restart();		// restart script at beginning if run again

  if(!taMisc::check_ok)
    SetRunState(NOT_INIT);
  else
    SetRunState(DONE);
  stop_req = false;  // this does not do full clear, so that information can be queried
  DataChanged(DCR_ITEM_UPDATED_ND); // update after macroscopic button-press action..
} 

void Program::InitScriptObj_impl() {
  AbstractScriptBase::InitScriptObj_impl(); // default one turns out to be fine..
}

bool Program::PreCompileScript_impl() {
  // as noted in abstractscriptbase: you must call this first to reset the script
  // because if you mess with the existing variables in prog_vars prior to 
  // resetting the script, it will get all messed up.  vars on this space are referred
  // to by a pointer to the space and an index off of it, which is important for autos
  // but actually not for these guys (but they are/were that way anyway).
  if(!AbstractScriptBase::PreCompileScript_impl()) return false;
  objs.GetVarsForObjs();
  UpdateProgVars();
  return true;
}

bool Program::AlreadyRunning() {
  if(run_state == RUN || run_state == INIT || global_run_state == RUN || 
     global_run_state == INIT) {
    taMisc::Info("A Program is already running -- cannot run until it is done");
    return true;
  }
  return false;
}

void Program::SetRunState(RunState value) {
  run_state = value;
  global_run_state = value;
}

int Program::Run_impl() {
  ret_val = RV_OK;
  if (!CompileScript()) {
    if (ret_val != RV_CHECK_ERR)
      ret_val = RV_COMPILE_ERR;
  }
  if (ret_val == RV_OK) {
    script->Run();
    // DO NOT DO!
    // DataChanged(DCR_ITEM_UPDATED_ND);
  }
  return ret_val;
}

int Program::Cont_impl() {
  // note: cont is fast and does not do any compile or run checks.
  // the user cannot access this without having pressed Init first, and that
  // does all the checks.  this is the standard paradigm for such things --
  // init does checks. run assumes things are ok & thus can be fast.
  script->Cont();
  // note: shared var state likely changed, so update gui
  script_compiled = true; // override any run-generated changes!!
  // do not update this -- too tight -- only at end!
  // DataChanged(DCR_ITEM_UPDATED_ND);
  return ret_val;
}

void Program::Run_Gui() {
  if(AlreadyRunning()) return;
  Run();
}

void Program::Run() {
  if(run_state == NOT_INIT) {
    Init();			// auto-press Init button!
  }
  if(run_state == STOP && stop_reason == SR_ERROR) {
    Init();			// auto-reinit after errors!
  }
  if(TestError(run_state != DONE && run_state != STOP, "Run",
	       "There was a problem with the Initialization of the Program (see css console for error messages) -- must fix before you can run.  Press Init first, look for errors, then Run")) {
    return;
  }
  ClearStopReq();
  step_mode = false;
  cur_step_prog = NULL;
  taMisc::Busy();
  SetRunState(RUN);
  DataChanged(DCR_ITEM_UPDATED_ND); // update button state
  bool did_struct_updt = false;
  if(!HasProgFlag(OBJS_UPDT_GUI)) {
    objs.StructUpdateEls(true);
    did_struct_updt = true;
  }
  Cont_impl();
  if(did_struct_updt)
    objs.StructUpdateEls(false);
  taMisc::DoneBusy();
  if (ret_val != RV_OK) ShowRunError();
  // unless we were stopped, we are done
  if(stop_req) {
    SetRunState(STOP);
    if((stop_reason == SR_ERROR) && (ret_val == RV_OK)) {
      ret_val = RV_RUNTIME_ERR;
    }
  }
  else {
    script->Restart();
    SetRunState(DONE);
  }
  stop_req = false;  // this does not do full clear, so that information can be queried
  DataChanged(DCR_ITEM_UPDATED_ND); // update after macroscopic button-press action..
} 

void Program::ShowRunError() {
  //note: if there was a ConfigCheck or runtime error, the user already got a dialog
  if (ret_val == RV_CHECK_ERR || ret_val == RV_RUNTIME_ERR) return;
  String err_str = "Error: The Program did not run -- ret_val=";
  err_str.cat( GetTypeDef()->GetEnumString("ReturnVal", ret_val));
  if (ret_val == RV_COMPILE_ERR) {
    err_str += " (a program did not compile correctly: check the console for error messages)";
  }
  taMisc::Error(err_str);
}

void Program::Step_Gui(Program* step_prg) {
  if(AlreadyRunning()) return;	// already running!
  Step(step_prg);
}

void Program::Step(Program* step_prg) {
  if(run_state == NOT_INIT) {
    Init();			// auto-press Init button!
  }
  if(TestError(run_state != DONE && run_state != STOP, "Step",
	       "There was a problem with the Initialization of the Program (see css console for error messages) -- must fix before you can run.  Press Init first, look for errors, then Step")) {
    return;
  }
  ClearStopReq();
  step_mode = true;
  if(step_prg)
    cur_step_prog = step_prg;
  else
    cur_step_prog = step_prog;

  if(TestError(!cur_step_prog || !cur_step_prog->owner, "Step",
	       "step program selected to step by is either NULL or unowned and likely was deleted -- not Stepping")) {
    return;
  }

  if(step_prog != step_prg)	// save this as new default..
    step_prog = step_prg;

  cur_step_n = cur_step_prog->step_n; // get from target, not us..
  cur_step_cnt = 0;
    
  taMisc::Busy();
  SetRunState(RUN);
  DataChanged(DCR_ITEM_UPDATED_ND); // update button state
  bool did_struct_updt = false;
  if(!HasProgFlag(OBJS_UPDT_GUI)) {
    objs.StructUpdateEls(true);
    did_struct_updt = true;
  }
  Cont_impl();
  if(did_struct_updt)
    objs.StructUpdateEls(false);
  taMisc::DoneBusy();
  if (ret_val != 0) {//TODO: use enums and sensible output string
    taiChoiceDialog::ErrorDialog(NULL, String(
      "The Program did not run -- ret_val=").cat(String(ret_val)),
      "Operation Failed");
  }
  step_mode = false;
  cur_step_prog = NULL;
  if(stop_req) {
    SetRunState(STOP);
    if((stop_reason == SR_ERROR) && (ret_val == RV_OK)) {
      ret_val = RV_RUNTIME_ERR;
    }
  }
  else {
    script->Restart();
    SetRunState(DONE);
  }
  stop_req = false;		    // this does not do full clear, so that information can be queried
  DataChanged(DCR_ITEM_UPDATED_ND); // update after macroscopic button-press action..
}

void Program::SetStopReq(StopReason stop_rsn, const String& stop_message) {
  stop_req = true;
  stop_reason = stop_rsn;
  stop_msg = stop_message;
}

void Program::ClearStopReq() {
  stop_req = false;
  stop_reason = SR_NONE;
  stop_msg = _nilString;
}

void Program::Stop() {
  if(TestError(run_state != RUN, "Stop",
	       "Program is not running")) {
    return;
  }
  SetStopReq(SR_USER_STOP, name);
}

void Program::Abort() {
  SetStopReq(SR_USER_ABORT, name);
  Stop_impl();
}

void Program::Stop_impl() {
  script->Stop();
//   setRunState(STOP);
//   DataChanged(DCR_ITEM_UPDATED_ND); // update button state
}

bool Program::IsStepProg() {
  return (step_mode && (cur_step_prog.ptr() == this));
}

bool Program::StopCheck() {
  //NOTE: we call event loop even in non-gui compile, since we can presumably
  // have other ways of stopping, such as something from a socket etc.
  taiM->ProcessEvents();
  // note: this has to be the full processevents and not RunPending,
  // otherwise it never seems to get the events.
  //  taiMiscCore::ProcessEvents();
  // NOTE: the return value of this function is not actually what determines stopping
  // the above processEvents will process any Stop events and this will directly cause
  // css to stop in its tracks.
  if(run_state == STOP) return true;
  if(stop_req) {
    Stop_impl();
    return true;
  }
  if(IsStepProg()) {
    cur_step_cnt++;
    if(cur_step_cnt >= cur_step_n) {
      SetStopReq(SR_STEP_POINT, name);	// stop everyone else
      Stop_impl();			// time for us to stop
      return true;
    }
  }
  return false;
}

void Program::Compile() {
  CompileScript(true);		// always force if command entered
}

void Program::CmdShell() {
  CmdShellScript();
}

void Program::ExitShell() {
  ExitShellScript();
}

void Program::UpdateCallerArgs() {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return;
  Program* pg;
  taLeafItr i;
  FOR_ITR_EL(Program, pg, proj->programs., i) {
    ProgramCallBase* pc = pg->FindSubProgTarget(this);
    if(pc) {
      pc->UpdateArgs();
    }
  }
  proj->programs.GuiFindFromMe(name);	// find all refs to me in programs
}

void Program::ScriptCompiled() {
  AbstractScriptBase::ScriptCompiled();
  script_compiled = true;
  ret_val = 0;
  m_stale = false;
  DataChanged(DCR_ITEM_UPDATED_ND); // this will *not* call setDirty
}

void Program::setStale() {
  //note: we don't propagate setStale
  //note: 2nd recursive call of this during itself doesn't do anything
  if(run_state == RUN || run_state == INIT) return;	     // change is likely self-generated during running, don't do it!
  bool changed = false;
  if (script_compiled) {
    // make sure this always reflects stale status -- is used as check for compiling..
    script_compiled = false; 
    changed = true;
  }
  if (!m_stale) {  // prevent recursion and spurious inherited calls!!!!
    changed = true;
    m_stale = true;
    //note: actions in here will not recurse us, because m_stale is now set
//     sub_prog_calls.RemoveAll(); // will need to re-enumerate
  }
  if (changed) { // user will need to recompile/INIT
    run_state = NOT_INIT;
//obs    DataChanged(DCR_ITEM_UPDATED_ND); //note: doesn't recurse ud
    DataChanged(DCR_ITEM_UPDATED); //note: doesn't recurse ud
  }
}

bool Program::SetVar(const String& nm, const Variant& value) {
  ProgVar* var = FindVarName(nm);
  if(TestError(!var, "SetVar", "variable named:", nm, "not found!"))
    return false;
  var->SetVar(value);
  return true;
}

Variant Program::GetVar(const String& nm) {
  ProgVar* var = FindVarName(nm);
  if(TestError(!var, "GetVar", "variable named:", nm, "not found!"))
    return false;
  return var->GetVar();
}

bool Program::HasVar(const String& var_nm) {
  return (FindVarName(var_nm));
}

bool Program::SetVarFmArg(const String& arg_nm, const String& var_nm, bool quiet) {
  String arg_str = taMisc::FindArgByName(arg_nm);
  if(arg_str.empty()) return false; // no arg, no action
  bool rval = SetVar(var_nm, arg_str);
  if(TestError(!rval, "SetVarFmArg", "variable:",var_nm,
	       "not found in program:", name)) return false;
  if(!quiet)
    taMisc::Info("Set", var_nm, "in program:", name, "to:", arg_str);
  return true;
}

ProgVar* Program::FindVarName(const String& var_nm) const {
  // note: this does NOT look in functions!
  ProgVar* sv = args.FindName(var_nm);
  if(sv) return sv;
  sv = vars.FindName(var_nm);
  if(sv) return sv;
  return prog_code.FindVarName(var_nm);
}

taBase* Program::FindTypeName(const String& nm) const {
  return types.FindTypeName(nm);
}

Program* Program::FindProgramName(const String& prog_nm, bool warn_not_found) const {
  Program* rval = NULL;
  if(owner && owner->InheritsFrom(&TA_Program_Group)) {
    Program_Group* pg = (Program_Group*)owner;
    rval = pg->FindName(prog_nm);
    if(!rval) {
      taProject* proj = GET_MY_OWNER(taProject);
      if(proj) {
	rval = proj->programs.FindLeafName(prog_nm);
      }
    }
  }
  if(warn_not_found && !rval) {
    taMisc::Warning("program", name, "is looking for a program named:",
		    prog_nm, "but it was not found! Probably there will be more specific errors when you try to Init the program");
  }
  return rval;
}

Program* Program::FindProgramNameContains(const String& prog_nm, bool warn_not_found) const {
  Program* rval = NULL;
  if(owner && owner->InheritsFrom(&TA_Program_Group)) {
    Program_Group* pg = (Program_Group*)owner;
    rval = pg->FindNameContains(prog_nm);
    if(!rval) {
      taProject* proj = GET_MY_OWNER(taProject);
      if(proj) {
	rval = proj->programs.FindLeafNameContains(prog_nm);
      }
    }
  }
  if(warn_not_found && !rval) {
    taMisc::Warning("program", name, "is looking for a program containing:",
		    prog_nm, "but it was not found! Probably there will be more specific errors when you try to Init the program");
  }
  return rval;
}

ProgramCallBase* Program::FindSubProgTarget(Program* prg) {
  for(int i=0;i<sub_prog_calls.size;i++) {
    ProgramCallBase* pc = (ProgramCallBase*)sub_prog_calls.FastEl(i);
    if(pc->CallsProgram(prg)) {
      return pc;
    }
  }
  return NULL;
}

const String Program::GetDescString(const String& dsc, int indent_level) {
  String rval;
  if(!dsc.empty()) {
    // we support multi-lines by using the multi-line form of comments
    if (dsc.contains('\n')) {
      rval.cat(cssMisc::IndentLines("/* " + dsc + " */\n", indent_level));
    } else {
      rval.cat(cssMisc::Indent(indent_level)).cat("// ").cat(dsc).cat("\n");
    }
  }
  return rval;
}

void Program::GetSubProgsAll(int depth) {
  if(TestError((depth >= 100), "GetSubProgsAll",
	       "Probable recursion in programs detected -- maximum depth of 100 reached -- aborting"))
    return;
  sub_progs_updtd = true;
  sub_progs_all.Reset();
  for(int i=0;i<sub_prog_calls.size; i++) {
    ProgramCallBase* sp = (ProgramCallBase*)sub_prog_calls.FastEl(i);
    sp->AddTargetsToListAll(sub_progs_all);
  }
  int init_sz = sub_progs_all.size;
  for(int i=0;i<init_sz; i++) {
    Program* sp = sub_progs_all[i];
    sp->GetSubProgsAll(depth+1);	// no loops please!!!
    // now get our sub-progs sub-progs..
    for(int j=0;j<sp->sub_progs_all.size;j++) {
      Program* ssp = sp->sub_progs_all[j];
      sub_progs_all.LinkUnique(ssp);
    }
  }
}

void Program::GetSubProgsStep() {
  // strategy here is to just go through the sub_progs_all list and get all the ones with step
  // flags on -- this is the only really reliable way to do this in terms of picking up
  // deep step guys even through ProgramCallVar guys
  sub_progs_step.Reset();
  if(HasProgFlag(SELF_STEP)) {
    sub_progs_step.Link(this);
  }
  for(int i=0;i<sub_progs_all.size; i++) {
    Program* sp = (Program*)sub_progs_all.FastEl(i);
    if(!sp->HasProgFlag(Program::NO_STOP_STEP))
      sub_progs_step.Link(sp);	// guaranteed to be unique already
  }
}

const String Program::scriptString() {
  // enumerate all the progels, esp. to get subprocs registered
  // note: this is regenerated every time because stale is not always right
  // and code may depend on the state of external objects that can be updated
  // outside of the stale mechanism.  When the user presses Init, they get the
  // current fresh code regardless!  note that it doesn't do this obligatory
  // recompiles all the time -- that is only done at init too.
  sub_prog_calls.Reset();
  sub_progs_dir.Reset();
  ClearProgFlag(SELF_STEP); // this is set by StopStepPoint..
  int item_id = 0;
  functions.PreGen(item_id);
  init_code.PreGen(item_id);
  prog_code.PreGen(item_id);
    
  // now, build the new script code
  m_scriptCache = "// ";
  m_scriptCache += GetName();
  m_scriptCache += "\n\n/* globals added to hardvars:\n";
  m_scriptCache += "Program::RunState run_state; // our program's run state\n";
  m_scriptCache += "int ret_val;\n";
  if (args.size > 0) {
    m_scriptCache += "// args: global script parameters (arguments)\n";
    m_scriptCache += args.GenCss(0);
  }
  if (vars.size > 0) {
    m_scriptCache += "// vars: global (non-parameter) variables\n";
    m_scriptCache += vars.GenCss(0);
  }
  m_scriptCache += "*/\n\n";

  // types
  if (types.size > 0) {
    m_scriptCache += types.GenCss(0);
  }

  // Functions
  m_scriptCache += functions.GenCss(0); // ok if empty, returns nothing
    
  // __Init() routine, for our own els, and calls to subprog Init()
  m_scriptCache += "void __Init() {\n";
  // first, make sure any sub-progs are compiled
  if (sub_prog_calls.size > 0) {
    m_scriptCache += "  // First compile any subprogs that could be called from this one\n";
    m_scriptCache += "  { Program* target;\n";
    // note: this is a list of ProgramCall's, not the actual prog itself!
    for (int i = 0; i < sub_prog_calls.size; ++i) {
      ProgramCallBase* sp = (ProgramCallBase*)sub_prog_calls.FastEl(i);
      String scrtmp = sp->GenCompileScript(this);
      if(scrtmp.nonempty()) {
	m_scriptCache += "    if (ret_val != Program::RV_OK) return; // checks previous\n"; 
	m_scriptCache += scrtmp;
      }
    }
    m_scriptCache += "  }\n";
  }
  m_scriptCache += "  // init_from vars\n";
  m_scriptCache += args.GenCssInitFrom(1);
  m_scriptCache += vars.GenCssInitFrom(1);
  m_scriptCache += "  // run our init code\n";
  m_scriptCache += init_code.GenCss(1); // ok if empty, returns nothing
  if (sub_prog_calls.size > 0) {
    if (init_code.size >0) m_scriptCache += "\n";
    m_scriptCache += "  // Then call init on any subprogs that could be called from this one\n";
    m_scriptCache += "  { Program* target;\n";
    // note: this is a list of ProgramCall's, not the actual prog itself!
    for (int i = 0; i < sub_prog_calls.size; ++i) {
      ProgramCall* sp = (ProgramCall*)sub_prog_calls.FastEl(i);
      m_scriptCache += sp->GenCallInit(this);
    }
    m_scriptCache += "  }\n";
  }
  m_scriptCache += "}\n\n";
    
  m_scriptCache += "void __Prog() {\n";
  m_scriptCache += "  // init_from vars\n";
  m_scriptCache += args.GenCssInitFrom(1);
  m_scriptCache += vars.GenCssInitFrom(1);
  m_scriptCache += "  // prog_code\n";
  m_scriptCache += prog_code.GenCss(1);
  if(!(flags & NO_STOP_STEP)) {
    m_scriptCache += "  StopCheck(); // process pending events, including Stop and Step events\n";
  }
  m_scriptCache += "}\n\n";
  m_scriptCache += "\n";
    
  m_scriptCache += "ret_val = Program::RV_OK; // set elsewise on failure\n";
  m_scriptCache += "if (run_state == Program::INIT) {\n";
  m_scriptCache += "  __Init();\n";
  m_scriptCache += "} else {\n";
  m_scriptCache += "  __Prog();\n";
  m_scriptCache += "}\n";

  m_stale = false;
  return m_scriptCache;
}

const String Program::ProgramListing() {
  m_listingCache = "// ";
  m_listingCache += GetName();

  if (types.size > 0) {
    m_listingCache += "\n// types: new types defined for this program\n";
    m_listingCache += types.GenListing(0);
  }

  if (args.size > 0) {
    m_listingCache += "\n// args: global script parameters (arguments)\n";
    m_listingCache += args.GenListing(0);
  }
  if (vars.size > 0) {
    m_listingCache += "\n// vars: global (non-parameter) variables\n";
    m_listingCache += vars.GenListing(0);
  }

  if(functions.size > 0) {
    m_listingCache += "\n// functions: functions defined for this program\n";
    m_listingCache += functions.GenListing(0);
  }
    
  if(init_code.size > 0) {
    m_listingCache += "\n// init_code: code to initialize the program\n";
    m_listingCache += init_code.GenListing(0); // ok if empty, returns nothing
  }
    
  if(prog_code.size > 0) {
    m_listingCache += "\n// prog_code: main code to run program\n";
    m_listingCache += prog_code.GenListing(0);
  }
  return m_listingCache;
}

void  Program::UpdateProgVars() {
  // note: this assumes that script has been ClearAll'd
  script->prog_vars.Reset(); // removes/unref-deletes
  
  // add the ones in the object -- note, we use *pointers* to these
  // these are already installed by the InstallThis routine!!
//   cssEl* el = NULL;
//   el = new cssCPtr_enum(&run_state, 1, "run_state",
// 			TA_Program.sub_types.FindName("RunState"));
//   script->prog_vars.Push(el);
//   el = new cssCPtr_int(&ret_val, 1, "ret_val");
//   script->prog_vars.Push(el);
//   el = new cssTA_Base(&objs, 1, objs.GetTypeDef(), "objs");
//   script->prog_vars.Push(el);

  // add new in the program
  for (int i = 0; i < args.size; ++i) {
    ProgVar* sv = args.FastEl(i);
    cssEl* el = sv->NewCssEl();
    script->prog_vars.Push(el);
  } 
  for (int i = 0; i < vars.size; ++i) {
    ProgVar* sv = vars.FastEl(i);
    cssEl* el = sv->NewCssEl();
    script->prog_vars.Push(el); //refs
  } 
}

String Program::GetProgLibPath(ProgLibs library) {
  if(library == SEARCH_LIBS) {
    taMisc::Error("Cannot do SEARCH_LIBS for saving -- program saved in local directory!");
    return "";
  }
  String path = "./";
  if(library == USER_LIB)
    path = taMisc::prog_lib_paths.GetVal("UserLib").toString();
  else if(library == SYSTEM_LIB)
    path = taMisc::prog_lib_paths.GetVal("SystemLib").toString();
  else if(library == WEB_LIB)
    path = taMisc::prog_lib_paths.GetVal("WebLib").toString();
  if(library != WEB_LIB) {
    QFileInfo qfi(path);
    if(!qfi.isDir()) {
      QDir qd;
      qd.mkpath(path);		// attempt to make it..
      taMisc::Warning("Note: did mkdir for program library directory:", path);
    }
  }
  return path;
}

void Program::SaveToProgLib(ProgLibs library) {
  String path = GetProgLibPath(library);
  String fname = path + "/" + name + ".prog";
  QFileInfo qfi(fname);
  if(qfi.isFile()) {
    int chs = taMisc::Choice("Program library file: " + fname + " already exists: Overwrite?",
			     "Ok", "Cancel");
    if(chs == 1) return;
  }
  SaveAs(fname);
  Program_Group::prog_lib.FindPrograms();
}

Variant Program::GetGuiArgVal(const String& fun_name, int arg_idx) {
  if(fun_name != "LoadFromProgLib") return inherited::GetGuiArgVal(fun_name, arg_idx);
  if(!prog_lib) return  _nilVariant;
  //  return _nilVariant;			     // return nil anyway!
  ProgLibEl* pel = prog_lib->FindName(name); // find our name
  return Variant(pel);
}

int Program::GetSpecialState() const {
  if(HasProgFlag(LOCKED)) return 4; // red
  if(HasProgFlag(STARTUP_RUN)) return 3; // green
  if(HasProgFlag(NO_STOP_STEP)) return 2; // may not want this one -- might be too much color..
  return 0;
}

void Program::LoadFromProgLib(ProgLibEl* prog_type) {
  if(TestError(!prog_type, "LoadFromProgLib", "program type is null")) return;
  if(TestError(prog_type->is_group, "LoadFromProgLib",
	       "cannot load a program group file into a single program!")) return;
//   Reset();
  prog_type->LoadProgram(this);
}

void Program::DataChanged(int dcr, void* op1, void* op2) {
  // just for debug trapping..
  inherited::DataChanged(dcr, op1, op2);
  sub_progs_updtd = false;
}

void Program::RunLoadInitCode() {
  // automatically do the program call guys!
  int item_id = 0;
  functions.PreGen(item_id);
  init_code.PreGen(item_id);
  prog_code.PreGen(item_id);
  for (int i = 0; i < sub_prog_calls.size; ++i) {
    ProgramCallBase* sp = (ProgramCallBase*)sub_prog_calls.FastEl(i);
    sp->LoadInitTarget();	// just call this directly!
  }

  // then run the load_code
  static cssProgSpace init_scr;
  init_scr.ClearAll();

  init_scr.prog_vars.Reset(); // removes/unref-deletes
  
  // add the ones in the object -- note, we use *pointers* to these
  // just the most relevant guys: not all the other stuff!
  cssEl* el = new cssTA_Base(&objs, 1, objs.GetTypeDef(), "objs");
  init_scr.prog_vars.Push(el);

  // add new in the program
  for (int i = 0; i < args.size; ++i) {
    ProgVar* sv = args.FastEl(i);
    el = sv->NewCssEl();
    init_scr.prog_vars.Push(el);
  } 
  for (int i = 0; i < vars.size; ++i) {
    ProgVar* sv = vars.FastEl(i);
    el = sv->NewCssEl();
    init_scr.prog_vars.Push(el); //refs
  } 

  STRING_BUF(code_str, 2048);
  code_str += "Program* this = " + GetPath() + ";\n";
  code_str += load_code.GenCss(1); // ok if empty, returns nothing

  // todo: debugging -- remove!
//   cerr << code_str << endl;
//   taMisc::FlushConsole();

  init_scr.CompileCode(code_str);
  init_scr.Run();
  init_scr.ClearAll();
}

void Program::SaveScript(ostream& strm) {
  strm << scriptString();
}

#ifdef TA_GUI
void Program::ViewScript() {
  ViewScript_impl();
}

void Program::ViewScript_Editor() {
  String fnm = name + "_view.css";
  fstream strm;
  strm.open(fnm, ios::out);
  SaveScript(strm);
  strm.close();

  taMisc::EditFile(fnm);
}

void Program::ViewScript_impl() {
  view_script = scriptString();
  TypeDef* td = GetTypeDef();
  MemberDef* md = td->members.FindName("view_script");
  taiStringDataHost* host_ = new taiStringDataHost(md, this, td, true, false, NULL, true);
  // args are: read_only, modal, parent, line_nos
  host_->Constr("Css Script for program: " + name);
  host_->Edit(false);
//   iTextEditDialog* dlg = new iTextEditDialog(true); // readonly
//   dlg->setText(scriptString());
//   dlg->exec();
}
#endif  // TA_GUI

void Program::SaveListing(ostream& strm) {
  strm << ProgramListing();
}

#ifdef TA_GUI
void Program::ViewListing() {
  taiStringDataHost* host_ = NULL;
//   iMainWindowViewer* cur_win = taiMisc::active_wins.Peek_MainWindow();
  //  host_ = taiMisc::FindEdit(base, cur_win);
  view_listing = ProgramListing();
  if(!host_) {
    TypeDef* td = GetTypeDef();
    MemberDef* md = td->members.FindName("view_listing");
    host_ = new taiStringDataHost(md, this, td, true, false, NULL, true);
    // args are: read_only, modal, parent, line_nos
    host_->Constr("Listing of program elements for program: " + name);
    host_->Edit(false);
  }

//   iTextEditDialog* dlg = new iTextEditDialog(true); // readonly
//   dlg->setText(ProgramListing());
//   dlg->exec();
}

void Program::ViewListing_Editor() {
  String fnm = name + "_list.css";
  fstream strm;
  strm.open(fnm, ios::out);
  SaveListing(strm);
  strm.close();

  taMisc::EditFile(fnm);
}

#endif  // TA_GUI

void Program::InitForbiddenNames() {
  if(forbidden_names.size > 0) return;
  forbidden_names.Add("run_state");
  forbidden_names.Add("ret_val");
  forbidden_names.Add("this");

  TypeDef* type_def = &TA_Program;
  for(int i=0; i<type_def->members.size; i++) {
    MemberDef* md = type_def->members.FastEl(i);
    forbidden_names.Add(md->name);
  }
  for(int i=0; i<type_def->methods.size; i++) {
    MethodDef* md = type_def->methods.FastEl(i);
    forbidden_names.Add(md->name);
  }
//   forbidden_names.List();
  // todo: probably could add more.. need to test..
}

bool Program::IsForbiddenName(const String& chk_nm, bool warn) {
  if((forbidden_names.FindEl(chk_nm) < 0) && !(bool)taMisc::types.FindName(chk_nm)) return false;
  if(!warn) return true;
  taMisc::Error("Program::IsForbiddenName -- Name:", chk_nm,
		"is not allowed as it clashes with other hard-coded variable names -- please choose a different one!");
  return true;
}

void Program::MakeTemplate_fmtype(Program* prog, TypeDef* td) {
  taBase* tok = (taBase*)td->GetInstance();
  if(tok) {
    taBase* o = tok->MakeToken();
    o->SetName("New" + td->name);
    prog->init_code.Add(o);
  }
  for(int i=0;i<td->children.size;i++) {
    TypeDef* chld = td->children[i];
    MakeTemplate_fmtype(prog, chld);
  }
}

Program* Program::MakeTemplate() {
//TODO: this will probably get nuked and replaced with a generic maker on .root
  Program* prog = new Program;
  {ProgVar* o = new ProgVar; o->SetName("NewProgVar"); prog->vars.Add(o);}
  //note: prog args go into a ProgramCall etc., so we just add the tmpl to the objects
  {ProgArg* o = new ProgArg; o->SetName("NewProgArg"); prog->objs.Add(o);}
  //note: put in .init since that will get searched first
  
  MakeTemplate_fmtype(prog, &TA_ProgEl);
  return prog;
}
  
bool Program::SelectCtrlFunsForEdit(SelectEdit* editor, const String& extra_label,
				    const String& sub_gp_nm) {
  if(!editor) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "SelectFunForEdit", "cannot find project")) return false;
    editor = (SelectEdit*)proj->edits.New(1);
  }
  TypeDef* td = GetTypeDef();
  bool rval = true;
  MethodDef* md = td->methods.FindName("Init"); // "" = desc field
  if(md) rval = editor->SelectMethod(this, md, extra_label, "", sub_gp_nm);
  md = td->methods.FindName("Run_Gui");
  if(md) rval |= editor->SelectMethod(this, md, extra_label, "", sub_gp_nm);
  md = td->methods.FindName("Step_Gui");
  if(md) rval |= editor->SelectMethod(this, md, extra_label, "", sub_gp_nm);
  md = td->methods.FindName("Stop");
  if(md) rval |= editor->SelectMethod(this, md, extra_label, "", sub_gp_nm);
  return rval;
}

#ifndef TA_GUI
// see ta_program_qt.cpp
iProgramPanel* Program::FindMyProgramPanel() {
  return NULL;
}
bool Program::BrowserSelectMe_ProgItem(taOBase* itm) {
  return false;
}
bool Program::BrowserExpandAll_ProgItem(taOBase* itm) {
  return false;
}
bool Program::BrowserCollapseAll_ProgItem(taOBase* itm) {
  return false;
}
#endif

//////////////////////////
//  Program_Group	//
//////////////////////////

void Program_Group::Initialize() {
  SetBaseType(&TA_Program);
}

void Program_Group::InitLinks() {
  inherited::InitLinks();
  taBase::Own(step_prog, this);
  if(prog_lib.not_init) {
    taBase::Ref(prog_lib);
    prog_lib.FindPrograms();
  }
}

void Program_Group::CutLinks() {
  inherited::CutLinks();
}

void Program_Group::Copy_(const Program_Group& cp) {
  desc = cp.desc;
}

void Program_Group::SaveToProgLib(Program::ProgLibs library) {
  String path = Program::GetProgLibPath(library);
  String fname = path + "/" + name + ".progp";
  QFileInfo qfi(fname);
  if(qfi.isFile()) {
    int chs = taMisc::Choice("Program library file: " + fname + " already exists: Overwrite?",
			     "Ok", "Cancel");
    if(chs == 1) return;
  }
  SaveAs(fname);
  Program_Group::prog_lib.FindPrograms();
}

void Program_Group::SetProgsStale() {
//obs: WARNING: this will cause us to also receive setStale for each prog call
  taLeafItr itr;
  Program* prog;
  FOR_ITR_EL(Program, prog, this->, itr) {
    prog->setStale();
  }
}

ProgLib Program_Group::prog_lib;

taBase* Program_Group::NewFromLib(ProgLibEl* prog_type) {
  return prog_lib.NewProgram(prog_type, this);
}

taBase* Program_Group::NewFromLibByName(const String& prog_nm) {
  return prog_lib.NewProgramFmName(prog_nm, this);
}

Variant Program_Group::GetGuiArgVal(const String& fun_name, int arg_idx) {
  if(fun_name != "LoadFromProgLib") return inherited::GetGuiArgVal(fun_name, arg_idx);
  ProgLibEl* pel = prog_lib.FindName(name); // find our name
  return Variant(pel);
}

void Program_Group::LoadFromProgLib(ProgLibEl* prog_type) {
  if(TestError(!prog_type, "LoadFromProgLib", "program type to load is null")) return;
  if(TestError(!prog_type->is_group, "LoadFromProgLib", 
	       "cannot load a single program file into a program group!")) return;
//   taLeafItr itr;
//   Program* prog;
//   FOR_ITR_EL(Program, prog, this->, itr) {
//     prog->Reset();
//   }
  prog_type->LoadProgramGroup(this);
}

bool Program_Group::RunStartupProgs() {
  bool any_run = false;
  taLeafItr itr;
  Program* prog;
  FOR_ITR_EL(Program, prog, this->, itr) {
    if(!prog->HasProgFlag(Program::STARTUP_RUN)) continue;
    cerr << "Running startup program: " << prog->name << endl;
    prog->Init();
    if((prog->ret_val == Program::RV_OK) && (prog->run_state == Program::DONE)) {
      prog->Run();
      any_run = true;
    }
  }
  return any_run;
}

//////////////////////////
//  Program_List	//
//////////////////////////

void Program_List::Initialize() {
  SetBaseType(&TA_Program);
}


//////////////////
//  ProgLib	//
//////////////////

void ProgLibEl::Initialize() {
  is_group = false;
}

void ProgLibEl::Destroy() {
  is_group = false;		// just for a breakpoint..
}

taBase* ProgLibEl::NewProgram(Program_Group* new_owner) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(is_group) {
    Program_Group* pg = (Program_Group*)new_owner->NewGp(1);
    LoadProgramGroup(pg);
    Program* first_guy = pg->Leaf(0);
    if(taMisc::gui_active && first_guy)
      tabMisc::DelayedFunCall_gui(first_guy, "BrowserSelectMe");
    return pg;
  }

  Program* pg = new_owner->NewEl(1, &TA_Program);
  LoadProgram(pg);
  if(taMisc::gui_active)
    tabMisc::DelayedFunCall_gui(pg, "BrowserSelectMe");
  return pg;
}

bool ProgLibEl::LoadProgram(Program* prog) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(is_group) {
    taMisc::Error("ProgLibEl::LoadProgram -- cannot load a program group file into a single program!");
    return false;
  }
  prog->Load(path);
  prog->UpdateAfterEdit();	// make sure
  prog->RunLoadInitCode();
  return true;
}

bool ProgLibEl::LoadProgramGroup(Program_Group* prog_gp) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(!is_group) {
    taMisc::Error("ProgLibEl::LoadProgram -- cannot load a single program file into a program group!");
    return false;
  }
  prog_gp->Load(path);
  prog_gp->UpdateAfterEdit();
  for(int i=0;i<prog_gp->leaves;i++) {
    Program* prog = prog_gp->Leaf(i);
    prog->UpdateAfterEdit();	// make sure
    prog->RunLoadInitCode();
  }
  return true;
}

bool ProgLibEl::ParseProgFile(const String& fnm, const String& path) {
  filename = fnm;
  if(filename.contains(".progp"))
    is_group = true;
  else
    is_group = false;
  URL = "file:" + path + "/" + filename;
  String openfnm = path + "/" + filename;
  fstream strm;
  strm.open(openfnm, ios::in);
  if(strm.bad() || strm.eof()) {
    taMisc::Error("ProgLibEl::ParseProgFile: could not open file name:", openfnm);
    return false;
  }
  bool rval = false;
  int c = taMisc::read_till_rb_or_semi(strm); // skips over entire path header!
  while((c != EOF) && !strm.eof() && !strm.bad()) {
    c = taMisc::read_till_eol(strm); // skip next line
    if(c == EOF) break;
    if(taMisc::LexBuf.contains("name=")) {
      name = taMisc::LexBuf.after("name=");
      name.gsub("\"", "");
      if(name.lastchar() == ';') name = name.before(';');
    }
    if(taMisc::LexBuf.contains("tags=")) {
      tags = taMisc::LexBuf.after("tags=");
      tags.gsub("\"", "");
      if(tags.lastchar() == ';') tags = tags.before(';');
      ParseTags();
    }
    if(taMisc::LexBuf.contains("desc=")) {
      desc = taMisc::LexBuf.after("desc=");
      desc.gsub("\"", "");
      if(desc.lastchar() == ';') desc = desc.before(';');
      rval = true;
      break;
    }
  }
  strm.close();
  // todo: should use QUrlInfo instead -- need QtNetwork module access!
  QFileInfo urlinfo(openfnm);
  QDateTime mod = urlinfo.lastModified();
  date = mod.toString(Qt::ISODate);
  return rval;
}

void ProgLibEl::ParseTags() {
  tags_array.Reset();
  if(tags.empty())
    return;
  String tmp = tags;
  while(tmp.contains(',')) {
    String tag = tmp.before(',');
    tag.gsub(" ","");		// nuke spaces
    tags_array.Add(tag);
    tmp = tmp.after(',');
  }
  if(!tmp.empty()) {
    tmp.gsub(" ","");		// nuke spaces
    tags_array.Add(tmp);
  }
}

void ProgLibEl_List::Initialize() {
}

void ProgLib::Initialize() {
  not_init = true;
}

void ProgLib::FindPrograms() {
  Reset();			// clear existing
  for(int pi=0; pi< taMisc::prog_lib_paths.size; pi++) {
    NameVar pathvar = taMisc::prog_lib_paths[pi];
    String path = pathvar.value.toString();
    String lib_name = pathvar.name;
    QDir dir(path);
    QStringList files = dir.entryList();
    for(int i=0;i<files.size();i++) {
      String fl = files[i];
      if(!fl.contains(".prog")) continue;
      ProgLibEl* pe = new ProgLibEl;
      pe->lib_name = lib_name;
      if(pe->ParseProgFile(fl, path))
	Add(pe);
      else
	delete pe;
    }
  }
  not_init = false;
}

taBase* ProgLib::NewProgram(ProgLibEl* prog_type, Program_Group* new_owner) {
  if(prog_type == NULL) return NULL;
  return prog_type->NewProgram(new_owner);
}

taBase* ProgLib::NewProgramFmName(const String& prog_nm, Program_Group* new_owner) {
  return NewProgram(FindName(prog_nm), new_owner);
}
