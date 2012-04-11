// Copyright, 1995-2007, Regents of the University of Colorado,
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


// css_ta.cc
// support for taBase type in css

#include <sstream>
#include "css_ta.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "ta_group.h"
#include "ta_project.h"

#ifdef TA_GUI
#include "ta_qttype_def.h"
#endif

/////////////////////////
//  cssTA	       //
/////////////////////////

void cssTA::Constr() {
  type_def = NULL;
}

cssTA::cssTA() : cssCPtr() {
  Constr();
}
cssTA::cssTA(void* it, int pc, TypeDef* td, const String& nm, cssEl* cls_par, bool ro)
: cssCPtr(it,pc,nm,cls_par,ro) {
  type_def = td;
}
cssTA::cssTA(const cssTA& cp) : cssCPtr(cp) {
  type_def = cp.type_def;
}
cssTA::cssTA(const cssTA& cp, const String& nm) : cssCPtr(cp,nm) {
  type_def = cp.type_def;
}

cssEl* cssTA::GetTypeObject() const {
  if(!type_def)	return &cssMisc::Void;
  cssElPtr s;
  if((s = cssMisc::TypesSpace.FindName((char*)type_def->name)) != 0)
    return s.El();
  if((s = cssMisc::TypesSpace.FindName("TA")) != 0)
    return s.El();
  return &cssMisc::Void;
}

String cssTA::PrintStr() const {
  void* pt = GetVoidPtr();
  String fh;
  if(pt && type_def)
    return type_def->Print(fh, pt);
  else
    return String("(") + GetTypeName() + ") " + name + " = " + PrintFStr();
}

String cssTA::PrintFStr() const {
  String rval;
  void* pt = GetVoidPtr();
  if(!pt)
    rval = "NULL";
  else
    rval = GetStr();
  return rval;
}

String& cssTA::PrintType(String& fh) const {
  for(int i=1;i<ptr_cnt;i++) fh << "*"; // ptr cnt
  if(type_def)
    type_def->PrintType(fh);
  else
    fh << "ta NULL type";
  return fh;
}

String& cssTA::PrintInherit(String& fh) const {
  if(type_def)
    type_def->PrintInherit(fh);
  return fh;
}

#ifdef TA_GUI
int cssTA::Edit(bool wait) {
  //WARNING: does not respect wait
  void* pt = GetVoidPtr();
  if(!pt || !type_def)
    return false;
  taiEdit* gc = type_def->ie;
  if (gc) {
    return gc->Edit(pt, false);
  }
  return false;
}
#endif // TA_GUI

void cssTA::Save(ostream& fh) {
  void* pt = GetVoidPtr();
  if(pt && type_def) {
    type_def->Dump_Save(fh, pt);
  }
}
void cssTA::Load(istream& fh) {
  void* pt = GetVoidPtr();
  if(pt && type_def) {
    type_def->Dump_Load(fh, pt);
  }
}

cssEl* cssTA::GetToken(int idx) const {
  if(!type_def)	return &cssMisc::Void;
  void* rval = type_def->tokens[idx];
  return new cssTA(rval, 1, type_def);
}

String& cssTA::PrintTokens(String& fh) const {
  if(type_def) {
    type_def->PrintTokens(fh);
  }
  return fh;
}

///////////////////////////////
//	Conversion ops	     //
///////////////////////////////

cssTA::operator TypeDef*() const {
  return type_def;
}

cssTA::operator void*() const {
  return GetVoidPtr(1);
}

void* cssTA::GetVoidPtrOfType(TypeDef* td) const {
  void* bptr = GetVoidPtr();
  if(!bptr) {
    cssMisc::Error(prog, "Null pointer for conversion to:",td->name,"from:",GetTypeName());
    return NULL;
  }
  void* rval = type_def->GetParAddr(td, bptr);
  if(!rval) {
    cssMisc::Error(prog, "Conversion to:",td->name,"is not a base type for:",GetTypeName());
  }
  return rval;
}

void* cssTA::GetVoidPtrOfType(const String& td) const {
  void* bptr = GetVoidPtr();
  if(!bptr) {
    cssMisc::Error(prog, "Null pointer for conversion to:",td,"from:",GetTypeName());
    return NULL;
  }
  void* rval = type_def->GetParAddr(td, bptr);
  if(!rval) {
    cssMisc::Error(prog, "Conversion to:",td,"is not a base type for:",GetTypeName());
  }
  return rval;
}

Variant cssTA::GetVar() const {
  // if it is a ptr to a TypeItem guy, we can provide a TypeItem guy
  if (ptr && type_def && type_def->DerivesFrom(&TA_TypeItem)) {
    if (ptr_cnt <= 1)
      return Variant((TypeItem*)ptr);
  }
  // could do getstr -- getting ptr is better.
  return Variant(GetVoidPtr());
}

String cssTA::GetStr() const {
  String rval;
  if(ptr && type_def) {
    if(ptr_cnt <= 1) {
      rval = type_def->GetValStr(ptr, NULL, NULL, TypeDef::SC_DEFAULT, true);
      // force inline
    }
    else if(ptr_cnt == 2) {
      // need the correct typedef here, either find it or make it..
      String ptrnm = type_def->name + "_ptr";
      TypeDef* ptr_typ = type_def->children.FindName(ptrnm);
      if(ptr_typ)
	rval = ptr_typ->GetValStr(ptr);
      else {
	TypeDef nw_tp(*type_def);
	nw_tp.ptr++;
	rval = nw_tp.GetValStr(ptr);
      }
    }
    return rval;
  }
  return String("NULL ptr of type: ") + GetTypeName();		// fallback
}

bool cssTA::AssignCheckSource(const cssEl& s) {
  if(s.GetType() != T_TA) {
    cssMisc::Error(prog, "Failed to assign TA C pointer of type:", GetTypeName(),
		   "source is non-TA object of type:", s.GetTypeName());
    return false;
  }
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  TypeDef* sp_typ = sp->GetNonRefTypeDef();
  if(!sp_typ) {
    cssMisc::Error(prog, "Failed to assign TA C pointer of type:", GetTypeName(),
		   "source object type info is NULL");
    return false;
  }
  if(type_def && !(sp_typ->InheritsFrom(type_def) || type_def->InheritsFrom(sp_typ))) {
    cssMisc::Error(prog, "Failed to assign TA C pointer of type:", GetTypeName(),
		   "source object type is incompatible:", sp_typ->name);
    return false;
  }
  return true;
}

bool cssTA::AssignObjCheck(const cssEl& s) {
  if(!ptr) {
    cssMisc::Error(prog, "Failed to copy to taBase C object of type:", GetTypeName(),
		   "our object is NULL");
    return false;
  }
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  // if(sp->GetNonRefPtrCnt() > 0) {
  //   cssMisc::Error(prog, "Failed to copy to taBase C object of type:", GetTypeName(),
  // 		   "we are an object and source is a pointer");
  //   return false;
  // }
  if(!sp->GetVoidPtr()) {
    cssMisc::Error(prog, "Failed to copy to taBase C object of type:", GetTypeName(),
		   "source is NULL");
    return false;
  }
  return true;
}

void cssTA::PtrAssignPtr(const cssEl& s) {
  if(PtrAssignNullInt(s)) return; // don't proceed further
  if(s.GetType() == T_MbrCFun) {
    // crazy special case: todo: move this to a subtype??
    if((ptr_cnt == 1) && type_def->InheritsFrom(TA_void)) {
      cssMbrCFun& mbf = (cssMbrCFun&)s;
      MethodDef* fun = TA_taRegFun.methods.FindName(mbf.name);
      if(fun)
	*((ta_void_fun*)ptr) = fun->addr;
      else {
	cssMisc::Error(prog, "Failed to assign TA void* member fun of type:", GetTypeName(),
		       "Source is an unregistered member function named:", mbf.name,
		       "add #REG_FUN to function");
      }
    }
    return;
  }
  if(!AssignCheckSource(s)) return;
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  int sp_ptr_cnt = sp->GetNonRefPtrCnt();
  void* sp_ptr = sp->GetNonRefPtr();
  TypeDef* sp_typ = sp->GetNonRefTypeDef();
  if(ptr_cnt == sp_ptr_cnt) {
    ptr = sp_ptr;
//     type_def = sp_typ;		// this prevents any form of casting!
    SetClassParent(sp->class_parent);
  }
  else if((ptr_cnt == 1) && (sp_ptr_cnt == 0)) {
    ptr = sp_ptr;		// I now point to that guy
//     type_def = sp_typ;
    SetClassParent(sp->class_parent);
  }
  else if((ptr_cnt == 1) && (sp_ptr_cnt == 2) && sp_ptr) {
    ptr = *((void**)sp_ptr);	// deref that guy
//     type_def = sp_typ;
    SetClassParent(sp->class_parent);
  }
  else if((ptr_cnt == 2) && (sp_ptr_cnt <= 1)) {
    // I'm a ptr-ptr and this sets me to point to another guy
    if(PtrAssignPtrPtr(sp_ptr)) {
//       type_def = sp_typ;
    }
  }
  else {
    cssMisc::Error(prog, "Failed to assign TA pointer of type:", GetTypeName(),
		   "pointer mismatch.  our ptr_cnt == ", String(ptr_cnt),
		   "source ptr_cnt == ", String(sp_ptr_cnt));
  }
}

void cssTA::operator=(const String& s) {
  if(!ROCheck()) return;
  if(ptr_cnt == 0) {
    if(type_def->HasOption("INLINE") || type_def->InheritsFrom(TA_taList_impl)
       || type_def->InheritsFrom(TA_taArray_base)
       || type_def->InheritsFrom(TA_taArray_impl) || type_def->InheritsFrom(TA_void))
    {
      if(ptr)
	type_def->SetValStr(s, ptr);
      return;
    }
  }
  else {
    cssMisc::Error(prog, "Failed to assign cssTA pointer of type:", GetTypeName(),
		   "no non-taBase support for pointer assignment from strings");
  }
}

void cssTA::operator=(const cssEl& s) {
  if(((s.GetType() == T_String) || (s.GetPtrType() == T_String)) && (ptr)) {
    *this = s.GetStr();	// use string converter
    UpdateClassParent();
    return;
  }
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {		// assume mostly reference/pointer semantics
    PtrAssignPtr(s);
    return;
  }

  if(!AssignCheckSource(s)) return; // not a good source
  if(!AssignObjCheck(s)) return; // not a good source for obj
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  // use typedef generic copy routine!
  type_def->CopyFromSameType(ptr, sp->GetVoidPtr());
}

void cssTA::ArgCopy(const cssEl& s) {
  if(((s.GetType() == T_String) || (s.GetPtrType() == T_String)) && (ptr)) {
    *this = s.GetStr();	// use string converter
    UpdateClassParent();
    return;
  }
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {		// assign pointers in this case
    PtrAssignPtr(s);
    return;
  }

  if(!AssignCheckSource(s)) return; // not a good source
  if(!AssignObjCheck(s)) return; // not a good source for obj
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  // use typedef generic copy routine!
  type_def->CopyFromSameType(ptr, sp->GetVoidPtr());
}

int cssTA::GetMemberNo(const String& memb) const {
  return GetMemberNo_impl(type_def, memb);
}

cssEl* cssTA::GetMemberFmName(const String& memb) const {
  return GetMemberFmName_impl(type_def, GetVoidPtr(), memb);
}

cssEl* cssTA::GetMemberFmNo(int memb) const {
  return GetMemberFmNo_impl(type_def, GetVoidPtr(), memb);
}

int cssTA::GetMethodNo(const String& memb) const {
  return GetMethodNo_impl(type_def, memb);
}

cssEl* cssTA::GetMethodFmName(const String& memb) const {
  return GetMethodFmName_impl(type_def, GetVoidPtr(), memb);
}

cssEl* cssTA::GetMethodFmNo(int memb) const {
  return GetMethodFmNo_impl(type_def, GetVoidPtr(), memb);
}

cssEl* cssTA::GetScoped(const String& memb) const {
  return GetScoped_impl(type_def, GetVoidPtr(), memb);
}

////////////////////////////////////////////////////////////////////////
// 		cssTA_Base
////////////////////////////////////////////////////////////////////////

void cssTA_Base::Constr() {
  if((ptr_cnt == 0) && !ptr) {	// don't already have a guy: make it!
    if(!type_def) {
      cssMisc::Error(prog, "Can't create new TABase object -- type info is NULL");
      return;
    }
    taBase* nw = taBase::MakeToken(type_def);
    if(!nw) {
      cssMisc::Error(prog, "Can't create new taBase object -- probably instance is NULL -- turn on instance generation for type:", GetTypeName());
      return;
    }
    taBase::Ref(nw);
    ptr = (void*)nw;
    SetPtrFlag(OWN_OBJ);
  }
  else {
    taBase* ths = GetTAPtr();
    if(ths) {
      if(ptr_cnt == 1)		    // note: can set to ptr_cnt <= 1 to ref everything
	taBase::Ref(ths);		// always ref ptrs!
      type_def = ths->GetTypeDef();	// just to be sure
    }
  }
}

void cssTA_Base::Copy(const cssTA_Base& cp) {
  cssTA::Copy(cp);
  if(ptr_cnt == 0 && ptr) {	// we copied from other guy
    ptr = NULL;			// nullify, so it will be created in Constr()
  }
  Constr();
  if(ptr_cnt == 0 && ptr && cp.ptr) {
    taBase* obj = (taBase*)ptr;
    taBase* oth = (taBase*)cp.ptr;
    obj->Copy(oth);
  }
}

void cssTA_Base::CopyType(const cssTA_Base& cp) {
  cssTA::CopyType(cp);
  if(ptr_cnt == 0 && ptr) {	// we copied from other guy
    ptr = NULL;			// nullify, so it will be created in Constr()
  }
  Constr();
  if(ptr_cnt == 0 && ptr && cp.ptr) {
    taBase* obj = (taBase*)ptr;
    taBase* oth = (taBase*)cp.ptr;
    obj->Copy(oth);
  }
}

cssTA_Base::cssTA_Base(const cssTA_Base& cp) {
  Copy(cp);
  name = cp.name;
}

cssTA_Base::cssTA_Base(const cssTA_Base& cp, const String& nm) {
  Copy(cp);
  name = nm;
}

cssTA_Base::~cssTA_Base() {
  if((ptr_cnt == 0) && ptr && HasPtrFlag(OWN_OBJ)) {
    taBase* ths = (taBase*)ptr;
    taBase::UnRef(ths);
    ptr = NULL;
    ClearPtrFlag(OWN_OBJ);
  }
  else if(ptr_cnt == 1 && ptr) { // note: can set to ptr_cnt <= 1 to ref everything..
    taBase::DelPointer((taBase**)&ptr);
  }
}

String cssTA_Base::PrintStr() const {
  taBase* ths = GetTAPtr();
  String fh;
  if(ths)
    ths->Print(fh);
  else {
    fh = cssTA::PrintStr();
  }
  return fh;
}

String& cssTA_Base::PrintType(String& fh) const {
  for(int i=1;i<ptr_cnt;i++) fh << "*"; // ptr cnt
  taBase* ths = GetTAPtr();
  if(ths)
    ths->PrintType(fh);
  else
    cssTA::PrintType(fh);
  return fh;
}

String& cssTA_Base::PrintInherit(String& fh) const {
  taBase* ths = GetTAPtr();
  if(ths)
    ths->PrintInherit(fh);
  else
    cssTA::PrintInherit(fh);
  return fh;
}

void cssTA_Base::Save(ostream& fh) {
  taBase* ths = GetTAPtr();
  if(ths)
    ths->Save_strm(fh);
}
void cssTA_Base::Load(istream& fh) {
  taBase* ths = GetTAPtr();
  if(ths)
    ths->Load_strm(fh);
}

void cssTA_Base::UpdateAfterEdit() {
  taBase* ths = GetTAPtr();
  if(cssMisc::call_update_after_edit && ths) ths->UpdateAfterEdit();
}

String cssTA_Base::GetStr() const {
  if(ptr_cnt == 1) {
    // if we are a pointer to a tabase, get the path -- otherwise for #INLINE
    // it can return the actual inline rep and this is bad for css arg types!
    taBase* ths = GetTAPtr();
    if(ths && ths->GetOwner())
      return ths->GetPathNames();
    else if(ths)
      return ths->GetValStr(NULL, NULL, TypeDef::SC_DEFAULT, true); // force_inline
  }
  else if(ptr_cnt == 0) {
    if(ptr) {
      taBase* obj = (taBase*)ptr;
      return obj->GetValStr(NULL, NULL, TypeDef::SC_DEFAULT, true); // force_inline
    }
  }
  return cssTA::GetStr();
}

void cssTA_Base::PtrAssignPtr(const cssEl& s) {
  if(PtrAssignNullInt(s)) return; // don't proceed further
  if(!AssignCheckSource(s)) return;
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  int sp_ptr_cnt = sp->GetNonRefPtrCnt();
  void* sp_ptr = sp->GetNonRefPtr();
  TypeDef* sp_typ = sp->GetNonRefTypeDef();
  if(ptr_cnt == sp_ptr_cnt) {
    if(ptr_cnt == 1)
      taBase::SetPointer((taBase**)&ptr, (taBase*)sp_ptr);
    else
      ptr = sp_ptr;
    type_def = sp_typ;
    SetClassParent(sp->class_parent);
  }
  else if((ptr_cnt == 1) && (sp_ptr_cnt == 0)) {
    taBase::SetPointer((taBase**)&ptr, (taBase*)sp_ptr);
    type_def = sp_typ;
    SetClassParent(sp->class_parent);
  }
  else if((ptr_cnt == 1) && (sp_ptr_cnt == 2) && sp_ptr) {
    taBase::SetPointer((taBase**)&ptr, *((taBase**)sp_ptr));
    type_def = sp_typ;
    SetClassParent(sp->class_parent);
  }
  else if((ptr_cnt == 2) && (sp_ptr_cnt <= 1)) {
    // I'm a ptr-ptr and this sets me to point to another guy
    if(PtrAssignPtrPtr(sp_ptr)) {
      type_def = sp_typ;
    }
  }
  else {
    cssMisc::Error(prog, "Failed to assign TA pointer of type:", GetTypeName(),
		   "pointer mismatch.  our ptr_cnt == ", String(ptr_cnt),
		   "source ptr_cnt == ", String(sp_ptr_cnt));
  }
}

bool cssTA_Base::PtrAssignPtrPtr(void* new_ptr_val) {
  if(!ptr) {
    cssMisc::Error(prog,  "Failed to assign taBase pointer-pointer of type:", GetTypeName(),
		   "our ptr is NULL");
    return false;
  }
  taBase::SetPointer((taBase**)ptr, (taBase*)new_ptr_val);
  UpdateClassParent();
  return true;
}

void cssTA_Base::PtrAssignNull() {
  if(ptr_cnt == 1)
    taBase::DelPointer((taBase**)&ptr);
  else
    inherited::PtrAssignNull();
}

void cssTA_Base::operator=(void* cp) {
// these are very bad because of the ref counting but we just have to assume the pointer is a taBase*!
#ifdef DEBUG
  taMisc::Warning("debug note: using cssTA_Base::operator=(void* cp) -- not a good idea so please change to use taBase* version!");
#endif
  *this = (taBase*)cp;		// just call taBase* version anyway
}

void cssTA_Base::operator=(void** cp) {
// these are very bad because of the ref counting but we just have to assume the pointer is a taBase*!
#ifdef DEBUG
  taMisc::Warning("debug note: using cssTA_Base::operator=(void** cp) -- not a good idea so please change to use taBase** version!");
#endif
  *this = (taBase**)cp;		// just call taBase* version anyway
}

void cssTA_Base::operator=(taBase* cp) {
  if((ptr_cnt == 0) && ptr) {	// value copy..
    if(!ptr) {
      cssMisc::Error(prog,  "Failed to assign taBase object of type:", GetTypeName(),
		     "our ptr is NULL");
      return;
    }
    if(!cp) {
      cssMisc::Error(prog,  "Failed to assign taBase object of type:", GetTypeName(),
		     "copy pointer is NULL");
      return;
    }
    taBase* obj = (taBase*)ptr;
    obj->Copy(cp);
    UpdateClassParent();
  }
  if(ptr_cnt == 1) {
    taBase::SetPointer((taBase**)&ptr, cp); // always use set pointer for ta base!
    if(ptr)
      type_def = ((taBase*)ptr)->GetTypeDef();
  }
  else if(ptr_cnt == 2) {
    PtrAssignPtrPtr(cp);
  }
}

void cssTA_Base::operator=(taBase** cp) {
  if(!cp) {
    cssMisc::Error(prog, "Failed to assign from taBase** -- pointer is NULL");
    return;
  }
  if((ptr_cnt == 0) && ptr) {
    if(!ptr) {
      cssMisc::Error(prog,  "Failed to assign taBase object of type:", GetTypeName(),
		     "our ptr is NULL");
      return;
    }
    if(!*cp) {
      cssMisc::Error(prog,  "Failed to assign taBase object of type:", GetTypeName(),
		     "copy pointer is NULL");
      return;
    }
    taBase* obj = (taBase*)ptr;
    obj->Copy(*cp);
    UpdateClassParent();
  }
  if(ptr_cnt == 1) {
    taBase::SetPointer((taBase**)&ptr, *cp); // always use set pointer!
    if(ptr)
      type_def = ((taBase*)ptr)->GetTypeDef();
  }
  else if(ptr_cnt == 2) {
    ptr = cp;
  }
}

void cssTA_Base::operator=(const String& s) {
  if(!ROCheck()) return;
  if(ptr_cnt <= 1 && !((s.startsWith('.') || s.startsWith("root.")))) {
    // not a path, use value assign
    if(ptr) {
      taBase* obj = (taBase*)ptr;
      obj->SetValStr(s, NULL, NULL, TypeDef::SC_DEFAULT, true);
      // force inline
    }
    return;
  }
  else {
    // note: cannot use SetValStr because it automatically does SetPointer!
    taBase* bs = NULL;
    String tmp_val = s;
    if((tmp_val != String::con_NULL) && (tmp_val != "Null") && (tmp_val != "__null")) {
      MemberDef* md = NULL;
      bs = tabMisc::root->FindFromPath(tmp_val, md);
      if(!bs) {
	taMisc::Warning("Invalid Path in cssTA_Base = String:",tmp_val);
	return;
      }
      if(md && (md->type->ptr == 1)) {
	bs = *((taBase**)bs);
	if(!bs) {
	  taMisc::Warning("Null object at end of path in cssTA_Base = String:",tmp_val);
	  return;
	}
      }
    }
    *this = bs;			// use above = opr
  }
}

void cssTA_Base::operator=(const cssEl& s) {
  if(((s.GetType() == T_String) || (s.GetPtrType() == T_String)) && (ptr)) {
    *this = s.GetStr();	// use string converter
    UpdateClassParent();
    return;
  }
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {		// assume reference/ptr semantics
    PtrAssignPtr(s);
    taBase* ths = GetTAPtr();
    if(ths)
      type_def = ths->GetTypeDef();	// just to be sure
    return;
  }
  // copy by value is default!
  if(!AssignCheckSource(s)) return; // not a good source
  if(!AssignObjCheck(s)) return; // not a good source for obj

  cssTA* sp = (cssTA*)s.GetNonRefObj();
  taBase* obj = (taBase*)ptr;
  obj->Copy((taBase*)sp->GetVoidPtr());
  UpdateClassParent();
}

void cssTA_Base::ArgCopy(const cssEl& s) {
  if(((s.GetType() == T_String) || (s.GetPtrType() == T_String)) && (ptr)) {
    *this = s.GetStr();	// use string converter
    UpdateClassParent();
    return;
  }
  // if(!ROCheck()) return;
  if(ptr_cnt > 0) {		// all ptrs get init here!
    PtrAssignPtr(s);
    taBase* ths = GetTAPtr();
    if(ths)
      type_def = ths->GetTypeDef();	// just to be sure
    return;
  }

  // ptr_cnt == 0 -- initialize us from that guy
  if(!cssTA_Base::AssignCheckSource(s)) return; // not a good source -- use our version  
  if(!AssignObjCheck(s)) return; // not a good source for obj
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  taBase* obj = (taBase*)ptr;
  obj->Copy((taBase*)sp->GetVoidPtr());
  UpdateClassParent();
}

void cssTA_Base::InitAssign(const cssEl& s) {
  if(((s.GetType() == T_String) || (s.GetPtrType() == T_String)) && (ptr)) {
    *this = s.GetStr();	// use string converter
    UpdateClassParent();
    return;
  }
  // if(!ROCheck()) return;
  if(ptr_cnt > 0) {		// all ptrs get init here!
    PtrAssignPtr(s);
    taBase* ths = GetTAPtr();
    if(ths)
      type_def = ths->GetTypeDef();	// just to be sure
    return;
  }

  taBase* ths = GetTAPtr();
  // ptr_cnt == 0 -- initialize us from that guy
  if(!cssTA_Base::AssignCheckSource(s)) return; // not a good source -- use our version which is most strict, not any derived version that might be looser (e.g., Matrix)
  // here we change our type to be that of the other object if necc
  cssTA_Base* sp = (cssTA_Base*)s.GetNonRefObj();
  taBase* obj = sp->GetTAPtr();
  if(obj->GetTypeDef() == ths->GetTypeDef()) {
    ths->Copy(obj);		// just copy if same types
  }
  else {			// otherwise change to new type
    taBase* nw = obj->Clone();
    taBase::SetPointer((taBase**)&ptr, nw); // always use set pointer for ta base!
    // this will auto-free any existing ptr
    if(ptr)
      type_def = ((taBase*)ptr)->GetTypeDef();
  }
  UpdateClassParent();
}

cssEl* cssTA_Base::operator[](const Variant& i) const {
  taBase* ths = GetTAPtr();
  if(!ths) {
    cssMisc::Error(prog, "operator[]: NULL pointer");
    return &cssMisc::Void;
  }
  if(ths->InheritsFrom(&TA_DataTableCols)) {
    if(ths->ElView() && ths->ElView()->size > 1 && i.isMatrixType()) {
      int ic = ths->IterCount(); // how many we'll iterate over
      // multiple columns -- create a mega return value from iterating
      Variant_Matrix* vmat = new Variant_Matrix(1,ic);
      TA_FOREACH(dcolv, *ths) {
	taBase* dcol = dcolv.toBase();
	if(dcol) {
	  Variant rval = dcol->Elem(i);
	  vmat->SetFmVar_Flat(rval, FOREACH_itr->count);
	}
      }
      return new cssTA_Matrix(vmat);
    }
    else {
      return TAElem(ths, i);
    }
  }
  else {
    return TAElem(ths, i);
  }
}

cssEl* cssTA_Base::GetMemberFmName(const String& memb) const {
  taBase* ths = GetTAPtr();
  if(!ths) {
    cssMisc::Error(prog, "GetMember: NULL pointer");
    return &cssMisc::Void;
  }

  MemberDef* md;
  void* mbr = ths->FindMembeR(memb, md);
  if(!mbr) {
    cssMisc::Error(prog, "MembeR not found:", memb, "in class of type:", (char*)GetTypeName());
    return &cssMisc::Void;
  }
  String mbnm;
  TypeDef* mtd = &TA_taBase;
  if(md) {
    mbnm = md->name;
    mtd = md->type;
  }

  return GetElFromTA(mtd, mbr, mbnm, md, (cssEl*)this);
}

cssEl* cssTA_Base::NewOpr() {
  taBase* nw = taBase::MakeToken(type_def);
  if(!nw) {
    cssMisc::Error(prog, "New token of type:", GetTypeName(), "could not be made");
    return &cssMisc::Void;
  }
  return new cssTA_Base((void*)nw, 1, type_def); // this guy points to it..
}

void cssTA_Base::DelOpr() {
  taBase* ths = GetTAPtr();
  if(!ths) {
    cssMisc::Error(prog, "delete: NULL pointer");
    return;
  }
  taBase::DelPointer((taBase**)&ptr);
}

void cssTA_Base::InstallThis(cssProgSpace* ps) {
  taBase* ths = GetTAPtr();
  if(!ths)
    return;

  ps->hard_vars.PushUniqNameNew(this);

  for(int i=0; i<type_def->members.size; i++) {
    MemberDef* md = type_def->members.FastEl(i);
    cssEl* tmp = GetMemberEl_impl(type_def, (void*)ths, md);
    ps->hard_vars.PushUniqNameNew(tmp);
  }
  for(int i=0; i<type_def->methods.size; i++) {
    MethodDef* md = type_def->methods.FastEl(i);
    cssEl* tmp = GetMethodEl_impl(type_def, (void*)ths, md);
    ps->hard_funs.PushUniqNameNew(tmp);
  }
}

////////////////////////////////////////////////////////////////////////
// 		cssSmartRef
////////////////////////////////////////////////////////////////////////

cssSmartRef::~cssSmartRef() {
  if(cssref) {
    cssEl::unRefDone(cssref);
    cssref = NULL;
  }
}

void cssSmartRef::UpdateCssRef() {
  taSmartRef* sr = GetSmartRef();
  taBase* srp = GetSmartRefPtr();
  if(cssref && srp && cssref->ptr == srp && 
     (cssref->GetNonRefTypeDef() == srp->GetTypeDef())) {
    return;			// all set!
  }
  if(!sr || !srp) {
    if(!cssref) {
      cssref = new cssTA_Base(NULL, 1, &TA_taBase); // null guy
      cssEl::Ref(cssref);
    }
    else if(cssref->ptr) {	// pointing to outdated
      *cssref = (taBase*)NULL;
    }
    return;
  }
  // matrix is only special case guy at this point
  if(srp->InheritsFrom(&TA_taMatrix)) {
    if(!cssref || !cssref->IsTaMatrix()) {
      if(cssref) cssEl::unRefDone(cssref);
      cssref = new cssTA_Matrix(srp, 1, srp->GetTypeDef());
      cssEl::Ref(cssref);
      return;
    }
  }
  else {
    if(!cssref || cssref->IsTaMatrix()) {	// not a matrix anymore
      if(cssref) cssEl::unRefDone(cssref);
      cssref = new cssTA_Base(srp, 1, srp->GetTypeDef());
      cssEl::Ref(cssref);
      return;
    }
  }
  cssref->type_def = srp->GetTypeDef(); // always set
  *cssref = srp;
}

String cssSmartRef::PrintStr() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) { return "NULL cssSmartRef"; }
  return cssref->PrintStr();
}

String cssSmartRef::PrintFStr() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) { return "NULL cssSmartRef"; }
  return cssref->PrintFStr();
}

String& cssSmartRef::PrintType(String& fh) const {
  fh << "SmartRef: ";
  cssref->PrintType(fh);
  return fh;
}

String& cssSmartRef::PrintInherit(String& fh) const {
  fh << "SmartRef: ";
  cssref->PrintInherit(fh);
  return fh;
}

void* cssSmartRef::GetVoidPtrOfType(TypeDef* td) const {
  if(td->InheritsFrom(&TA_taSmartRef)) return cssTA::GetVoidPtrOfType(td);
  return cssref->GetVoidPtrOfType(td);
}

void* cssSmartRef::GetVoidPtrOfType(const String& td) const {
  if(td == "taSmartRef") return cssTA::GetVoidPtrOfType(td);
  return cssref->GetVoidPtrOfType(td);
}  

void cssSmartRef::operator=(taBase* s) {
  taSmartRef* sr = GetSmartRef();
  if(!sr) return;		// nothing to do
  sr->set(s);			// for direct tabase init, always set pointer
  UpdateCssRef();
}

void cssSmartRef::operator=(taBase** cp) {
  if(!cp) {
    cssMisc::Error(prog, "Failed to assign from taBase** -- pointer is NULL");
    return;
  }
  taSmartRef* sr = GetSmartRef();
  if(!sr) return;
  sr->set(*cp);
  UpdateCssRef();
}

void cssSmartRef::operator=(const cssEl& s) {
  taSmartRef* sr = GetSmartRef();
  if(!sr) return;
  if(cssref && cssref->IsTaMatrix()) { // matrix has value semantics
    if(sr->ptr()) {
      cssref->operator=(s);		// use the ref -- will use value semantic copy
    }
    else {
      PtrAssignPtr(s);		// set our pointer
    }
  }
  else {
    PtrAssignPtr(s);		// just update the ref pointer for all other cases -- no value copy semantics
  }
}

void cssSmartRef::ArgCopy(const cssEl& s) {
  PtrAssignPtr(s);		// always just set pointer in all cases
}

void cssSmartRef::PtrAssignPtr(const cssEl& s) {
  taSmartRef* sr = GetSmartRef();
  if(!sr) return;
  sr->set((taBase*)s);	// set as a taptr
  UpdateCssRef();
}

////////////////////////////////////////////////////////////////////////
// 		cssIOS
////////////////////////////////////////////////////////////////////////

String cssIOS::PrintFStr() const {
  String rval;
  void* pt = GetVoidPtr();
  if(!pt)
    rval = "NULL";
  else 
    rval = String("<stream>") + String((long)ptr);
  return rval;
}

String cssIOS::GetStr() const {
  String rval;
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_fstream)) {
    if(!ptr)
      return rval;
    *((istream*)*this) >> rval;
    return rval;
  }
  return cssTA::GetStr();
}

cssIOS::operator Real() const {
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_fstream)) {
    if(!ptr)
      return 0;
    double rval;
    *((istream*)*this) >> rval;
    return rval;
  }
  return cssTA::operator Real();
}
cssIOS::operator Int() const {
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_fstream)) {
    if(!ptr)
      return 0;
    int rval;
    *((istream*)*this) >> rval;
    return rval;
  }
  return cssTA::operator Int();
}

cssIOS::operator iostream*() const {
  if(type_def->InheritsFrom(&TA_iostream)) {
    if(type_def->InheritsFrom(&TA_fstream))
      return (iostream*)(fstream*)GetVoidPtr();
    if(type_def->InheritsFrom(&TA_stringstream))
      return (iostream*)(stringstream*)GetVoidPtr();
    return (iostream*)GetVoidPtr();
  }
  CvtErr("(iostream*)"); return NULL;
}
cssIOS::operator istream*() const {
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_iostream)) {
    if(type_def->InheritsFrom(&TA_fstream))
      return (istream*)(fstream*)GetVoidPtr();
    if(type_def->InheritsFrom(&TA_stringstream))
      return (istream*)(stringstream*)GetVoidPtr();
    if(type_def->InheritsFrom(&TA_iostream))
      return (istream*)(iostream*)GetVoidPtr();
    return (istream*)GetVoidPtr();
  }
  CvtErr("(istream*)"); return NULL;
}
cssIOS::operator ostream*() const {
  if(type_def->InheritsFrom(&TA_ostream) || type_def->InheritsFrom(&TA_iostream)) {
    if(type_def->InheritsFrom(&TA_fstream))
      return (ostream*)(fstream*)GetVoidPtr();
    if(type_def->InheritsFrom(&TA_stringstream))
      return (ostream*)(stringstream*)GetVoidPtr();
    if(type_def->InheritsFrom(&TA_iostream))
      return (ostream*)(iostream*)GetVoidPtr();
    return (ostream*)GetVoidPtr();
  }
  CvtErr("(ostream*)"); return NULL;
}
cssIOS::operator fstream*() const {
  if(type_def->InheritsFrom(&TA_fstream)) {
    return (fstream*)GetVoidPtr();
  }
  CvtErr("(fstream*)"); return NULL;
}

cssIOS::operator stringstream*() const {
  if(type_def->InheritsFrom(&TA_stringstream)) {
    return (stringstream*)GetVoidPtr();
  }
  CvtErr("(stringstream*)"); return NULL;
}

cssIOS::operator iostream**() const {
  if(type_def->InheritsFrom(&TA_iostream)) {
    if(type_def->InheritsFrom(&TA_fstream))
      return (iostream**)(fstream**)GetVoidPtr(2);
    if(type_def->InheritsFrom(&TA_stringstream))
      return (iostream**)(stringstream**)GetVoidPtr(2);
    return (iostream**)GetVoidPtr(2);
  }
  CvtErr("(iostream**)"); return NULL;
}
cssIOS::operator istream**() const {
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_iostream)) {
    if(type_def->InheritsFrom(&TA_fstream))
      return (istream**)(fstream**)GetVoidPtr(2);
    if(type_def->InheritsFrom(&TA_stringstream))
      return (istream**)(stringstream**)GetVoidPtr(2);
    if(type_def->InheritsFrom(&TA_iostream))
      return (istream**)(iostream**)GetVoidPtr(2);
    return (istream**)GetVoidPtr(2);
  }
  CvtErr("(istream*)"); return NULL;
}
cssIOS::operator ostream**() const {
  if(type_def->InheritsFrom(&TA_ostream) || type_def->InheritsFrom(&TA_iostream)) {
    if(type_def->InheritsFrom(&TA_fstream))
      return (ostream**)(fstream**)GetVoidPtr(2);
    if(type_def->InheritsFrom(&TA_stringstream))
      return (ostream**)(stringstream**)GetVoidPtr(2);
    if(type_def->InheritsFrom(&TA_iostream))
      return (ostream**)(iostream**)GetVoidPtr(2);
    return (ostream**)GetVoidPtr(2);
  }
  CvtErr("(ostream**)"); return NULL;
}
cssIOS::operator fstream**() const {
  if(type_def->InheritsFrom(&TA_fstream)) {
    return (fstream**)GetVoidPtr(2);
  }
  CvtErr("(fstream**)"); return NULL;
}

cssIOS::operator stringstream**() const {
  if(type_def->InheritsFrom(&TA_stringstream)) {
    return (stringstream**)GetVoidPtr(2);
  }
  CvtErr("(stringstream**)"); return NULL;
}

cssEl* cssIOS::operator<<(cssEl& s) {
  if(type_def->InheritsFrom(&TA_ostream) || type_def->InheritsFrom(&TA_iostream)) {
    ostream* strm = (ostream*)*this;
    if(name == "cout" || name == "cerr") {
      String str;
      if(s.GetType() == T_Int) {
	if(strm->flags() & ios::hex)
	  str << String((Int)s, "0x%x"); 
	else if(strm->flags() & ios::oct)
	  str << String((Int)s, "0%o"); 
	else
	  str << String((Int)s);
      }
      else if(s.GetType() == T_Real)
	str << (Real) s;
      else
	str << s.PrintFStr();
      taMisc::ConsoleOutputChars(str); // use console output!
    }
    else if(strm) {
      if(s.GetType() == T_Int)
	*strm << (Int) s;
      else if(s.GetType() == T_Real)
	*strm << (Real) s;
      else
	*strm << (const char*)s;
    }
    return this;
  }
  NopErr("<<");
  return this;
}
cssEl* cssIOS::operator>>(cssEl& s) {
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_iostream)) {
    istream* strm = (istream*)*this;
    if(strm) {
      if(s.GetType() == T_Int) {
	cssInt* tmp = (cssInt*)s.GetNonRefObj();
	String tpnm = tmp->GetTypeName();
	if(tpnm == "(char)") {
	  char ctmp;
	  *strm >> ctmp;
	  tmp->val = (int)ctmp;
	}
	else
	  *strm >> tmp->val;
      }
      else if(s.GetType() == T_Real) {
	cssReal* tmp = (cssReal*)s.GetNonRefObj();
	*strm >> tmp->val;
      }
      else if(s.GetType() == T_Array) {
	cssArray* ary = (cssArray*)s.GetNonRefObj();
	if(!ary->el_type || !ary->items) {
	  cssMisc::Error(prog, "Error: Array has no item type");
	  return this;
	}
	int i = 0;
	cssString* tp_str = new cssString();
	while(true) {
	  *strm >> ws;
	  int c = strm->peek();
	  if((c == ',') || (c == '{')) {
	    strm->get();
	    continue;
	  }
	  if((c == '}') || (c == EOF)) { // needs to be } terminated..
	    strm->get();
	    break;
	  }
	  if(ary->items->size < i+1) {
	    cssEl* tmp = ary->el_type->AnonClone();
	    tmp->SetAddr(ary->items->Push(tmp));
	  }
	  if(ary->el_type->GetType() == T_Int) {
	    cssInt* tmp = (cssInt*)ary->items->FastEl(i);
	    *strm >> tmp->val;
	  }
	  else if(ary->el_type->GetType() == T_Real) {
	    cssReal* tmp = (cssReal*)ary->items->FastEl(i);
	    *strm >> tmp->val;
	  }
	  else {
	    if(c == '\"') { // "
	      strm->get();
	      c = taMisc::read_till_end_quote(*strm);
	      if(c == EOF)	break;
	      tp_str->val = taMisc::LexBuf;
	    }
	    else {
	      *strm >> tp_str->val;
	    }
	    *(ary->items->FastEl(i)) = *tp_str; // use built-in conversion routines
	  }
	  i++;
	}
	delete tp_str;
      }
      else {
	cssString* tmp = new cssString();
	*strm >> tmp->val;
	s = *tmp;			// use built-in conversion routines
	delete tmp;
      }
    }
    return this;
  }
  NopErr(">>");
  return this;
}

void cssIOS::PtrAssignPtr(const cssEl& s) {
  if(s.GetType() != T_TA) {
    cssTA::PtrAssignPtr(s);
    return;
  }
  cssTA* st = (cssTA*)s.GetNonRefObj();
  if(ptr_cnt == st->ptr_cnt) {
    if((ptr_cnt == 1) && !class_parent && st->type_def->InheritsFrom(TA_ios)) {
      if(!st->type_def->InheritsFrom(type_def)) {
	// source must be my type or greater
	cssMisc::Error(prog, "Attempt to assign incompatible ios pointers:",
		       GetTypeName(), "!=", st->GetTypeName());
	return;
      }
      // assignment amongst iostream types..
      if(type_def->InheritsFrom(TA_fstream) || type_def->InheritsFrom(TA_stringstream)) {
	ptr = st->ptr;
	return;
      }
      if(type_def->InheritsFrom(TA_iostream)) {
	ptr = (void*)(iostream*)*st;
	return;
      }
      if(type_def->InheritsFrom(TA_istream)) {
	ptr = (void*)(istream*)*st;
	return;
      }
      if(type_def->InheritsFrom(TA_ostream)) {
	ptr = (void*)(ostream*)*st;
	return;
      }
      ptr = st->ptr;
      if((prog) && (prog->top->debug) && (type_def != st->type_def))
	cssMisc::Warning(prog, "Warning: assigning different ptr types");
      return;
    }
  }
  cssTA::PtrAssignPtr(s);	// fall back
}

////////////////////////////////////////////////////////////////////////
// 		cssFStream
////////////////////////////////////////////////////////////////////////

TypeDef* cssFStream::TA_TypeDef() {
  return &TA_fstream;
}

void cssSStream::Constr() {
  ptr = new std::stringstream;
}

cssSStream::cssSStream()
  : cssIOS(NULL, 1, &TA_stringstream)
{ Constr(); }
cssSStream::cssSStream(const String& nm)
  : cssIOS(NULL, 1, &TA_stringstream, nm)
{ Constr(); }
cssSStream::cssSStream(const cssSStream& cp)
  : cssIOS(cp)
{ Constr(); }
cssSStream::cssSStream(const cssSStream& cp, const String&)
  : cssIOS(cp)
{ Constr(); }
cssSStream::~cssSStream()
{ std::stringstream* str = (std::stringstream*)ptr; delete str; }

///////////////////////////////

TypeDef* cssLeafItr::TA_TypeDef() {
  return &TA_taLeafItr;
}

void cssLeafItr::Constr() {
  ptr = new taLeafItr;
}

cssLeafItr::~cssLeafItr() {
  taLeafItr* lf = (taLeafItr*)ptr;
  delete lf;
}

////////////////////////////////////////////////////////////////////////
// 		cssTypeDef
////////////////////////////////////////////////////////////////////////

String cssTypeDef::PrintStr() const {
  String fh;
  return PrintType(fh);
}

String cssTypeDef::PrintFStr() const {
  String fh = "type: ";
  void* pt = GetVoidPtr();
  if(pt)
    fh += ((TypeDef*)pt)->name;
  else
    fh += "NULL";
  return fh;
}

String& cssTypeDef::PrintType(String& fh) const {
  for(int i=1;i<ptr_cnt;i++) fh << "*"; // ptr cnt
  void* pt = GetVoidPtr();
  if(pt)
    ((TypeDef*)pt)->PrintType(fh);
  else
    cssTA::PrintType(fh);
  return fh;
}

String& cssTypeDef::PrintInherit(String& fh) const {
  void* pt = GetVoidPtr();
  if(pt)
    ((TypeDef*)pt)->PrintInherit(fh);
  else
    cssTA::PrintInherit(fh);
  return fh;
}

cssTypeDef::operator TypeDef*() const {
  if(!ptr)
    return type_def;
  else if(ptr_cnt == 1)
    return (TypeDef*)ptr;
  else if((ptr_cnt == 2) && *((TypeDef**)ptr))
    return *((TypeDef**)ptr);
  return NULL;
}

String cssTypeDef::GetStr() const {
  String rval;
  if(!ptr)
    return "type: NULL";
  else if(ptr_cnt == 1)
    return ((TypeDef*)ptr)->name;
  else if((ptr_cnt == 2) && *((TypeDef**)ptr))
    return (*((TypeDef**)ptr))->name;
  return GetTypeName();
}

void cssTypeDef::operator=(const String& s) {
  if(ptr_cnt == 1) {
    TypeDef* td = taMisc::types.FindName(s);
    if(td)
      ptr = (void*)td;
  }
}

void cssTypeDef::operator=(const cssEl& s) {
  if(ptr_cnt==2) { // if im a typedef-ptr-ptr
    if(s.GetType() == T_TA) {
      cssTA* tmp = (cssTA*)s.GetNonRefObj();
      if(tmp->type_def->InheritsFrom(&TA_TypeDef))
	PtrAssignPtr(s);
      else {
	*((TypeDef**)ptr) = tmp->type_def;	// get its typedef
	UpdateClassParent();
      }
    }
    else {
      *((TypeDef**)ptr) = (TypeDef*)s;		// get the name
      UpdateClassParent();
    }
    return;
  }
  cssTA::operator=(s);
}

////////////////////////////////////////////////////////////////////////
// 		cssMemberDef
////////////////////////////////////////////////////////////////////////

cssMemberDef::operator MemberDef*() const {
  if(!ptr)
    return NULL;
  else if(ptr_cnt == 1)
    return (MemberDef*)ptr;
  else if((ptr_cnt == 2) && *((MemberDef**)ptr))
    return *((MemberDef**)ptr);
  return NULL;
}

String cssMemberDef::GetStr() const {
  String rval;
  if(!ptr)
    return rval;
  else if(ptr_cnt == 1)
    rval = TA_MemberDef_ptr.GetValStr((void*)&ptr);
  else if((ptr_cnt == 2) && *((MemberDef**)ptr))
    rval = TA_MemberDef_ptr.GetValStr(((MemberDef**)ptr));
  return rval;
}

void cssMemberDef::operator=(const String& s) {
  if(ptr_cnt == 1) {
    TA_MemberDef_ptr.SetValStr(s, (void*)&ptr);
    return;
  }
  cssTA::operator=(s);
}

void cssMemberDef::operator=(const cssEl& s) {
  if(ptr_cnt==2) { // if im a memberdef-ptr-ptr
    if(s.GetType() == T_TA) {
      cssTA* tmp = (cssTA*)s.GetNonRefObj();
      if(tmp->type_def->InheritsFrom(&TA_MemberDef))
	PtrAssignPtr(s);
      else {
	*((MemberDef**)ptr) = (MemberDef*)s;
	UpdateClassParent();
      }
    }
    else {
      *((MemberDef**)ptr) = (MemberDef*)s;		// get the name
      UpdateClassParent();
    }
    return;
  }
  cssTA::operator=(s);
}

////////////////////////////////////////////////////////////////////////
// 		cssMethodDef
////////////////////////////////////////////////////////////////////////

cssMethodDef::operator MethodDef*() const {
  if(!ptr)
    return NULL;
  else if(ptr_cnt == 1)
    return (MethodDef*)ptr;
  else if((ptr_cnt == 2) && *((MethodDef**)ptr))
    return *((MethodDef**)ptr);
  return NULL;
}

String cssMethodDef::GetStr() const {
  String rval;
  if(!ptr)
    return rval;
  else if(ptr_cnt == 1)
    rval = TA_MethodDef_ptr.GetValStr((void*)&ptr);
  else if((ptr_cnt == 2) && *((MethodDef**)ptr))
    rval = TA_MethodDef_ptr.GetValStr(((MethodDef**)ptr));
  return rval;
}

void cssMethodDef::operator=(const String& s) {
  if(ptr_cnt == 1) {
    TA_MethodDef_ptr.SetValStr(s, (void*)&ptr);
    return;
  }
  cssTA::operator=(s);
}

void cssMethodDef::operator=(const cssEl& s) {
  if(ptr_cnt==2) { // if im a methoddef-ptr-ptr
    if(s.GetType() == T_TA) {
      cssTA *tmp = (cssTA*)s.GetNonRefObj();
      if(tmp->type_def->InheritsFrom(&TA_MethodDef))
	PtrAssignPtr(s);
      else {
	*((MethodDef**)ptr) = (MethodDef*)s;
	UpdateClassParent();
      }
    }
    else {
      *((MethodDef**)ptr) = (MethodDef*)s;		// get the name
      UpdateClassParent();
    }
  }
  cssTA::operator=(s);
}


////////////////////////////////////////////////////////////////////////
// 		cssTA_Matrix
////////////////////////////////////////////////////////////////////////

bool cssTA_Matrix::IsMatrix(const cssEl& s) {
  if(s.GetType() != T_TA) return false;
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  TypeDef* sp_typ = sp->GetNonRefTypeDef();
  if(sp_typ->InheritsFrom(&TA_taMatrix)) {
    return (bool)sp->GetVoidPtr(); // only valid ones with real pointers
  }
  return false;
}

taMatrix* cssTA_Matrix::MatrixPtr(const cssEl& s) {
  return ((cssTA_Matrix*)s.GetNonRefObj())->GetMatrixPtr();  
}

cssTA_Matrix::cssTA_Matrix(taMatrix* mtx)
  : cssTA_Base(mtx, 0, mtx->GetTypeDef()) {
  SetPtrFlag(OWN_OBJ);		// mark us as owner
  taBase::Ref(mtx);
  // note: if set ptr_cnt <= 1 ref'ing in TA_Base, then off
  // ptr_cnt <= 1 can cause crashing at exit due to other 0 ptr guys that
  // are not properly  ref'd in the software -- can just go thru and fix
  // all those however.. -- safer in general to have the ptr_cnt = 0 do the
  // ref..

  // actually, there is a problem where temp Matrix objs don't get Own'd and 
  // InitLinks is only called on ownership, so then their members don't get
  // ref'd, and then this 0 ref/deref guy nukes them..

  // best soln is to have temp guys be owned, but this can be tricky as to 
  // how to uniquely trigger it..
}

cssTA_Matrix::~cssTA_Matrix() {
}

String cssTA_Matrix::GetStr() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return "NULL";
  if(ptr_cnt == 1) {
    // if we are a pointer to a tabase, get the path -- otherwise for #INLINE
    // it can return the actual inline rep and this is bad for css arg types!
    if(ths && ths->owner) {
      return ths->GetPathNames();
    }
  }
  String fh;
  return ths->Print(fh);	// use natural print
}

cssEl* cssTA_Matrix::operator[](const Variant& i) const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) {
    cssMisc::Error(prog, "operator[]: NULL pointer");
    return &cssMisc::Void;
  }
  return TAElem(ths, i);
}

cssTA_Matrix::operator Real() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return 0.0;
  double rval = 0.0;
  TA_FOREACH(vitm, *ths) {
    if(FOREACH_itr->count == 0) {
      rval = vitm.toDouble();
    }
    else {
      cssMisc::Error(prog, "cannot convert matrix to single scalar value");
      return 0.0;
    }
  }
  return rval;
}

cssTA_Matrix::operator Int() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return 0;
  int rval = 0;
  TA_FOREACH(vitm, *ths) {
    if(FOREACH_itr->count == 0) {
      rval = vitm.toInt();
    }
    else {
      cssMisc::Error(prog, "cannot convert matrix to single scalar value");
      return 0;
    }
  }
  return rval;
}

cssTA_Matrix::operator bool() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return 0;
  bool rval = 0;
  TA_FOREACH(vitm, *ths) {
    if(FOREACH_itr->count == 0) {
      rval = vitm.toBool();
    }
    else {
      cssMisc::Error(prog, "cannot convert matrix to single scalar value");
      return 0;
    }
  }
  return rval;
}

void* cssTA_Matrix::GetVoidPtrOfType(TypeDef* td) const {
  void* bptr = GetVoidPtr();
  if(!bptr) {
    cssMisc::Error(prog, "Null pointer for conversion to:",td->name,"from:",GetTypeName());
    return NULL;
  }
  if(ptr_cnt == 1 && td->DerivesFrom(&TA_taMatrix)) {
    if(td->DerivesFrom(&TA_int_Matrix)) {
      return (int_Matrix*)*this;
    }
    else if(td->DerivesFrom(&TA_byte_Matrix)) {
      return (byte_Matrix*)*this;
    }
    else if(td->DerivesFrom(&TA_float_Matrix)) {
      return (float_Matrix*)*this;
    }
    else if(td->DerivesFrom(&TA_double_Matrix)) {
      return (double_Matrix*)*this;
    }
    else if(td->DerivesFrom(&TA_String_Matrix)) {
      return (String_Matrix*)*this;
    }
    else if(td->DerivesFrom(&TA_Variant_Matrix)) {
      return (Variant_Matrix*)*this;
    }
  }
  void* rval = type_def->GetParAddr(td, bptr);
  if(!rval) {
    cssMisc::Error(prog, "Conversion to:",td->name,"is not a base type for:",GetTypeName());
  }
  return rval;
}

void* cssTA_Matrix::GetVoidPtrOfType(const String& std) const {
  void* bptr = GetVoidPtr();
  if(!bptr) {
    cssMisc::Error(prog, "Null pointer for conversion to:",std,"from:",GetTypeName());
    return NULL;
  }
  TypeDef* td = taMisc::types.FindName(std);
  if(!td) {
    cssMisc::Error(prog, "GetVoidPtrOfType: Type name:",std,"not found in list of types");
    return NULL;
  }
  return GetVoidPtrOfType(td);
}

cssTA_Matrix::operator int_Matrix*() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return NULL;
  if(ths->InheritsFrom(&TA_int_Matrix))
    return (int_Matrix*)ths;
  int_Matrix* rval = new int_Matrix;
  rval->Copy(ths);		// use generic variant-based copy
  return rval;
}

cssTA_Matrix::operator byte_Matrix*() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return NULL;
  if(ths->InheritsFrom(&TA_byte_Matrix))
    return (byte_Matrix*)ths;
  byte_Matrix* rval = new byte_Matrix;
  rval->Copy(ths);		// use generic variant-based copy
  return rval;
}

cssTA_Matrix::operator float_Matrix*() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return NULL;
  if(ths->InheritsFrom(&TA_float_Matrix))
    return (float_Matrix*)ths;
  float_Matrix* rval = new float_Matrix;
  rval->Copy(ths);		// use generic variant-based copy
  return rval;
}

cssTA_Matrix::operator double_Matrix*() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return NULL;
  if(ths->InheritsFrom(&TA_double_Matrix))
    return (double_Matrix*)ths;
  double_Matrix* rval = new double_Matrix;
  rval->Copy(ths);		// use generic variant-based copy
  return rval;
}

cssTA_Matrix::operator String_Matrix*() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return NULL;
  if(ths->InheritsFrom(&TA_String_Matrix))
    return (String_Matrix*)ths;
  String_Matrix* rval = new String_Matrix;
  rval->Copy(ths);		// use generic variant-based copy
  return rval;
}

cssTA_Matrix::operator Variant_Matrix*() const {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) 
    return NULL;
  if(ths->InheritsFrom(&TA_Variant_Matrix))
    return (Variant_Matrix*)ths;
  Variant_Matrix* rval = new Variant_Matrix;
  rval->Copy(ths);		// use generic variant-based copy
  return rval;
}

bool cssTA_Matrix::AssignCheckSource(const cssEl& s) {
  if(s.GetType() != T_TA) {
    // cssMisc::Error(prog, "Failed to assign TA C pointer of type:", GetTypeName(),
    // 		   "source is non-TA object of type:", s.GetTypeName());
    // return false;
    return true;		// use variant
  }
  if(IsMatrix(s)) return true;
  return false;
}

void cssTA_Matrix::operator=(const cssEl& s) {
  if(((s.GetType() == T_String) || (s.GetPtrType() == T_String)) && (ptr)) {
    *this = s.GetStr();	// use string converter
    UpdateClassParent();
    return;
  }
  if(!ROCheck()) return;
  taMatrix* ths = GetMatrixPtr();
  if(!ths || ptr_cnt > 1) {	// if our ptr is null, we need to set it, otherwise use value copy -- only reset pointer if ptr ptr
    if(IsMatrix(s)) {
      PtrAssignPtr(s);
      taBase* nwths = GetTAPtr();
      if(nwths)
	type_def = nwths->GetTypeDef();	// just to be sure
    }
    return;
  }
  if(IsMatrix(s)) {		// copy by value
    taMatrix* oth = MatrixPtr(s);
    if(oth) {
      ths->Copy(oth); // use generic copy matrix routine for this operator -- does the right thing..
    }
  }
  else {
    Variant ovar = s.GetVar();
    if(!ovar.isInvalid()) {
      *ths = ovar; // use matrix routine for this operator
    }
  }
  UpdateClassParent();
}

cssEl* cssTA_Matrix::operator+(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths + *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths + ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator-(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths - *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths - ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator-() {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  taMatrix* rval = -*ths; // use matrix routine for this operator
  if(rval) return new cssTA_Matrix(rval);
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator*(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths * *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths * ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator/(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths / *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths / ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator%(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths % *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths % ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator^(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths ^ *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths ^ ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

void cssTA_Matrix::operator+=(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      *ths += *oth; // use matrix routine for this operator
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      *ths += ovar; // use matrix routine for this operator
    }
  }
}

void cssTA_Matrix::operator-=(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      *ths -= *oth; // use matrix routine for this operator
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      *ths -= ovar; // use matrix routine for this operator
    }
  }
}

void cssTA_Matrix::operator*=(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      *ths *= *oth; // use matrix routine for this operator
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      *ths *= ovar; // use matrix routine for this operator
    }
  }
}

void cssTA_Matrix::operator/=(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      *ths /= *oth; // use matrix routine for this operator
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      *ths /= ovar; // use matrix routine for this operator
    }
  }
}

void cssTA_Matrix::operator%=(cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      *ths %= *oth; // use matrix routine for this operator
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      *ths %= ovar; // use matrix routine for this operator
    }
  }
}

cssEl* cssTA_Matrix::operator< (cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths < *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths < ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator> (cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths > *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths > ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator<= (cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths <= *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths <= ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator>= (cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths >= *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths >= ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator== (cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths == *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths == ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator!= (cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths != *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths != ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator&& (cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths && *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths && ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator|| (cssEl& t) {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  if(IsMatrix(t)) {
    taMatrix* oth = MatrixPtr(t);
    if(oth) {
      taMatrix* rval = *ths || *oth; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  else {
    Variant ovar = t.GetVar();
    if(!ovar.isInvalid()) {
      taMatrix* rval = *ths || ovar; // use matrix routine for this operator
      if(rval) return new cssTA_Matrix(rval);
    }
  }
  return &cssMisc::Void;
}

cssEl* cssTA_Matrix::operator! () {
  taMatrix* ths = GetMatrixPtr();
  if(!ths) return &cssMisc::Void;
  taMatrix* rval = !*ths; // use matrix routine for this operator
  if(rval) return new cssTA_Matrix(rval);
  return &cssMisc::Void;
}

