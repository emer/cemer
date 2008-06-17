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
cssTA::cssTA(void* it, int pc, TypeDef* td, const char* nm, cssEl* cls_par, bool ro)
: cssCPtr(it,pc,nm,cls_par,ro) {
  Constr();
  type_def = td;
}
cssTA::cssTA(const cssTA& cp) : cssCPtr(cp) {
  type_def = cp.type_def;
}
cssTA::cssTA(const cssTA& cp, const char* nm) : cssCPtr(cp,nm) {
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

void cssTA::TypeInfo(ostream& fh) const {
  for(int i=1;i<ptr_cnt;i++) fh << "*"; // ptr cnt
  type_def->OutputType(fh);
}

void cssTA::Print(ostream& fh) const {
  void* pt = GetVoidPtr();
  if(pt)
    type_def->Output(fh, pt);
  else
    fh << PrintStr();
}

void cssTA::PrintR(ostream& fh) const {
  void* pt = GetVoidPtr();
  if(pt)
    type_def->OutputR(fh, pt);
  else
    fh << PrintStr();
}

void cssTA::InheritInfo(ostream& fh) const {
  type_def->OutputInherit(fh);
}

#ifdef TA_GUI
int cssTA::Edit(bool wait) {
  //WARNING: does not respect wait
  void* pt = GetVoidPtr();
  if(!pt)
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
  if(pt) {
    type_def->Dump_Save(fh, pt);
  }
}
void cssTA::Load(istream& fh) {
  void* pt = GetVoidPtr();
  if(pt) {
    type_def->Dump_Load(fh, pt);
  }
}

cssEl* cssTA::GetToken(int idx) const {
  void* rval = type_def->tokens[idx];
  return new cssTA(rval, 1, type_def);
}

void cssTA::TokenInfo(ostream& fh) const {
  type_def->tokens.List(fh);
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
    cssMisc::Error(prog, "Null pointer for conversion to:",td->name,"from:",type_def->name);
    return NULL;
  }
  void* rval = type_def->GetParAddr(td, bptr);
  if(!rval) {
    cssMisc::Error(prog, "Conversion to:",td->name,"is not a base type for:",type_def->name);
  }
  return rval;
}

void* cssTA::GetVoidPtrOfType(const char* td) const {
  void* bptr = GetVoidPtr();
  if(!bptr) {
    cssMisc::Error(prog, "Null pointer for conversion to:",td,"from:",type_def->name);
    return NULL;
  }
  void* rval = type_def->GetParAddr(td, bptr);
  if(!rval) {
    cssMisc::Error(prog, "Conversion to:",td,"is not a base type for:",type_def->name);
  }
  return rval;
}

Variant cssTA::GetVar() const {
  // could do getstr -- getting ptr is better.
  return Variant(GetVoidPtr());
}

String cssTA::GetStr() const {
  String rval;
  if(ptr) {
    if(ptr_cnt <= 1) {
      rval = type_def->GetValStr(ptr);
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
  return type_def->name;
}

bool cssTA::AssignCheckSource(const cssEl& s) {
  if(s.GetType() != T_TA) {
    cssMisc::Error(prog, "Failed to assign TA C pointer of type:", type_def->name,
		   "source is non-TA object of type:", s.GetTypeName());
    return false;
  }
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  TypeDef* sp_typ = sp->GetNonRefTypeDef();
  if(!sp_typ) {
    cssMisc::Error(prog, "Failed to assign TA C pointer of type:", type_def->name,
		   "source object type info is NULL");
    return false;
  }
  if(type_def && !sp_typ->InheritsFrom(type_def)) {
    cssMisc::Error(prog, "Failed to assign TA C pointer of type:", type_def->name,
		   "source object type is incompatible:", sp_typ->name);
    return false;
  }
  return true;
}

bool cssTA::AssignObjCheck(const cssEl& s) {
  if(!ptr) {
    cssMisc::Error(prog, "Failed to copy to taBase C object of type:", type_def->name,
		   "our object is NULL");
    return false;
  }
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  if(sp->GetNonRefPtrCnt() > 0) {
    cssMisc::Error(prog, "Failed to copy to taBase C object of type:", type_def->name,
		   "we are an object and source is a pointer");
    return false;
  }
  if(!sp->GetNonRefPtr()) {
    cssMisc::Error(prog, "Failed to copy to taBase C object of type:", type_def->name,
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
	cssMisc::Error(prog, "Failed to assign TA void* member fun of type:", type_def->name,
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
    type_def = sp_typ;
    SetClassParent(sp->class_parent);
  }
  else if((ptr_cnt == 1) && (sp_ptr_cnt == 0)) {
    ptr = sp_ptr;		// I now point to that guy
    type_def = sp_typ;
    SetClassParent(sp->class_parent);
  }
  else if((ptr_cnt == 1) && (sp_ptr_cnt == 2) && sp_ptr) {
    ptr = *((void**)sp_ptr);	// deref that guy
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
    cssMisc::Error(prog, "Failed to assign TA pointer of type:", type_def->name,
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
    cssMisc::Error(prog, "Failed to assign cssTA pointer of type:", type_def->name,
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
  if(ptr_cnt > 0) {
    PtrAssignPtr(s);
    return;
  }

  if(!AssignCheckSource(s)) return; // not a good source
  if(!AssignObjCheck(s)) return; // not a good source for obj
  cssTA* sp = (cssTA*)s.GetNonRefObj();
  // use typedef generic copy routine!
  type_def->CopyFromSameType(ptr, sp->GetNonRefPtr());
}

cssEl* cssTA::GetElement_impl(taBase* ths, Variant i) const {
  TypeDef* eltd;
  void* el = ths->GetTA_Element(i, eltd);
  if(!eltd) {
    cssMisc::Error(prog, "This is not an array:", ths->GetName(),
		    "type:", ths->GetTypeDef()->name);
    return &cssMisc::Void;
  }
  String elno = i.toString();
  if(!el) {
    cssMisc::Error(prog, "Index out of range:", elno, "for", ths->GetName(),
		    "type:", ths->GetTypeDef()->name);
    return &cssMisc::Void;
  }
  String nm = ths->GetName() + String("[") + elno + "]";
  return GetElFromTA(eltd, el, nm, (MemberDef*)NULL, (cssEl*)this);
}

int cssTA::GetMemberNo(const char* memb) const {
  return GetMemberNo_impl(type_def, memb);
}

cssEl* cssTA::GetMemberFmName(const char* memb) const {
  return GetMemberFmName_impl(type_def, GetVoidPtr(), memb);
}

cssEl* cssTA::GetMemberFmNo(int memb) const {
  return GetMemberFmNo_impl(type_def, GetVoidPtr(), memb);
}

int cssTA::GetMethodNo(const char* memb) const {
  return GetMethodNo_impl(type_def, memb);
}

cssEl* cssTA::GetMethodFmName(const char* memb) const {
  return GetMethodFmName_impl(type_def, GetVoidPtr(), memb);
}

cssEl* cssTA::GetMethodFmNo(int memb) const {
  return GetMethodFmNo_impl(type_def, GetVoidPtr(), memb);
}

cssEl* cssTA::GetScoped(const char* memb) const {
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
      cssMisc::Error(prog, "Can't create new taBase object -- probably instance is NULL -- turn on instance generation for type:", type_def->name);
      return;
    }
    taBase::Ref(nw);
    ptr = (void*)nw;
    flags = (PtrFlags)(flags | OWN_OBJ);
  }
  else {
    taBase* ths = GetTAPtr();
    if(ths) {
      if(ptr_cnt == 1)
	taBase::Ref(ths);		// always ref ptrs!
      type_def = ths->GetTypeDef();	// just to be sure
    }
  }
}

cssTA_Base::cssTA_Base(const cssTA_Base& cp) : cssTA(cp) {
  if(ptr_cnt == 0 && ptr) {	// we copied from other guy
    ptr = NULL;			// nullify, so it will be created in Constr()
  }
  Constr();
  if(ptr_cnt == 0 && ptr && cp.ptr) {
    taBase* obj = (taBase*)ptr;
    taBase* oth = (taBase*)cp.ptr;
    obj->UnSafeCopy(oth);
  }
}

cssTA_Base::cssTA_Base(const cssTA_Base& cp, const char* nm) : cssTA(cp,nm) {
  if(ptr_cnt == 0 && ptr) {	// we copied from other guy
    ptr = NULL;			// nullify, so it will be created in Constr()
  }
  Constr();
  if(ptr_cnt == 0 && ptr && cp.ptr) {
    taBase* obj = (taBase*)ptr;
    taBase* oth = (taBase*)cp.ptr;
    obj->UnSafeCopy(oth);
  }
}

cssTA_Base::~cssTA_Base() {
  if((ptr_cnt == 0) && ptr && (flags & OWN_OBJ)) {
    taBase* ths = (taBase*)ptr;
    taBase::UnRef(ths);
    ptr = NULL;
    flags = (PtrFlags)(flags & ~OWN_OBJ);
  }
  if(ptr_cnt == 1 && ptr) {
    taBase::DelPointer((taBase**)&ptr);
  }
}

void cssTA_Base::TypeInfo(ostream& fh) const {
  for(int i=1;i<ptr_cnt;i++) fh << "*"; // ptr cnt
  taBase* ths = GetTAPtr();
  if(ths)
    ths->OutputType(fh);
  else
    type_def->OutputType(fh);
}

void cssTA_Base::Print(ostream& fh) const {
  taBase* ths = GetTAPtr();
  if(ths)
    ths->Output(fh) << "\n";
  else {
    fh << PrintStr();
  }
}

void cssTA_Base::PrintR(ostream& fh) const {
  taBase* ths = GetTAPtr();
  if(ths)
    ths->OutputR(fh) << "\n";
  else {
    fh << PrintStr();
  }
}

void cssTA_Base::InheritInfo(ostream& fh) const {
  taBase* ths = GetTAPtr();
  if(ths)
    ths->OutputInherit(fh) << "\n";
  else
    type_def->OutputInherit(fh) << "\n";
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
    if(ths)
      return ths->GetPath();
    else
      return "NULL";
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
    cssMisc::Error(prog, "Failed to assign TA pointer of type:", type_def->name,
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

void cssTA_Base::operator=(void* cp) {
// these are very bad because of the ref counting but we just have to assume the pointer is a taBase*!
#ifdef DEBUG
  cerr << "debug note: using cssTA_Base::operator=(void* cp) -- not a good idea so please change to use taBase* version!" << endl;
#endif
  *this = (taBase*)cp;		// just call taBase* version anyway
}

void cssTA_Base::operator=(void** cp) {
// these are very bad because of the ref counting but we just have to assume the pointer is a taBase*!
#ifdef DEBUG
  cerr << "debug note: using cssTA_Base::operator=(void** cp) -- not a good idea so please change to use taBase** version!" << endl;
#endif
  *this = (taBase**)cp;		// just call taBase* version anyway
}

void cssTA_Base::operator=(taBase* cp) {
  if((ptr_cnt == 0) && ptr) {
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
    obj->UnSafeCopy(cp);
    UpdateClassParent();
  }
  if(ptr_cnt == 1) {
    taBase::SetPointer((taBase**)&ptr, cp); // always use set pointer for ta base!
//     ptr = cp;
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
    obj->UnSafeCopy(*cp);
    UpdateClassParent();
  }
  if(ptr_cnt == 1) {
    taBase::SetPointer((taBase**)&ptr, *cp); // always use set pointer!
    //    ptr = *cp;
    if(ptr)
      type_def = ((taBase*)ptr)->GetTypeDef();
  }
  else if(ptr_cnt == 2) {
    ptr = cp;
  }
}

void cssTA_Base::operator=(const String& s) {
  if(!ROCheck()) return;
  if(ptr_cnt == 0) {
    //    cssTA::operator=(s);		// just do same thing
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
    if((tmp_val != String::con_NULL) && (tmp_val != "Null")) {
      MemberDef* md = NULL;
      bs = tabMisc::root->FindFromPath(tmp_val, md);
      if(!md || !bs) {
	taMisc::Warning("Invalid Path in cssTA_Base = String:",tmp_val);
	return;
      }
      if(md->type->ptr == 1) {
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
  if(ptr_cnt > 0) {
    PtrAssignPtr(s);
    taBase* ths = GetTAPtr();
    if(ths)
      type_def = ths->GetTypeDef();	// just to be sure
    return;
  }
  // basic ptr_cnt == 0 copy:
  if(!AssignCheckSource(s)) return; // not a good source
  if(!AssignObjCheck(s)) return; // not a good source for obj

  cssTA* sp = (cssTA*)s.GetNonRefObj();
  taBase* obj = (taBase*)ptr;
  obj->UnSafeCopy((taBase*)sp->GetNonRefPtr());
  UpdateClassParent();
}

cssEl* cssTA_Base::operator[](Variant i) const {
  taBase* ths = GetTAPtr();
  if(ths)
    return GetElement_impl(ths, i);
  cssMisc::Error(prog, "operator[]: NULL pointer");
  return &cssMisc::Void;
}

cssEl* cssTA_Base::GetMemberFmName(const char* memb) const {
  taBase* ths = GetTAPtr();
  if(!ths) {
    cssMisc::Error(prog, "GetMember: NULL pointer");
    return &cssMisc::Void;
  }

  void* mbr;
  MemberDef* md = ths->FindMembeR(memb, mbr);
//   MemberDef* md = type_def->members.FindNameAddrR(memb, ths, mbr);
  if(!md) {
    cssMisc::Error(prog, "MembeR not found:", memb, "in class of type:", (char*)type_def->name);
    return &cssMisc::Void;
  }

  return GetElFromTA(md->type, mbr, md->name, md, (cssEl*)this);
}

cssEl* cssTA_Base::NewOpr() {
  taBase* nw = taBase::MakeToken(type_def);
  if(!nw) {
    cssMisc::Error(prog, "New token of type:", type_def->name, "could not be made");
    return &cssMisc::Void;
  }
  taBase::Ref(nw);			// refer to this
  return new cssTA_Base((void*)nw, 1, type_def); // this guy points to it..
}

void cssTA_Base::DelOpr() {
  taBase* ths = GetTAPtr();
  if(!ths) {
    cssMisc::Error(prog, "delete: NULL pointer");
    return;
  }
  taBase::UnRef(ths);
  ptr = NULL;			// no longer point to this..
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

TypeDef* cssSmartRef::GetNonRefTypeDef() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr || !sr->ptr()) return type_def;
  return sr->ptr()->GetTypeDef();
}

int cssSmartRef::GetNonRefPtrCnt() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr || !sr->ptr()) return 0;
  return 1;			// we're basically a pointer to a ta base, not guy itself?
}

void* cssSmartRef::GetNonRefPtr() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr || !sr->ptr()) return ptr;
  return (void*)sr->ptr();
}

const char* cssSmartRef::GetTypeName() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return "taSmartRef";
  if(sr->ptr()) {
    return sr->ptr()->GetTypeDef()->name;
  }
  return cssTA::GetTypeName();
}

void cssSmartRef::Print(ostream& fh) const {
  fh << PrintStr();
}

void cssSmartRef::PrintR(ostream& fh) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) { fh << "NULL cssSmartRef"; return; }
  if(sr->ptr()) {
    sr->ptr()->GetTypeDef()->OutputR(fh, sr->ptr());
  }
  else {
    fh << PrintStr();
  }    
}

void cssSmartRef::TypeInfo(ostream& fh) const {
  for(int i=1;i<ptr_cnt;i++) fh << "*"; // ptr cnt
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) { fh << "NULL cssSmartRef"; return; }
  if(sr->ptr()) {
    sr->ptr()->GetTypeDef()->OutputType(fh);
    return;
  }
  cssTA::TypeInfo(fh);
}

void cssSmartRef::InheritInfo(ostream& fh) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) { fh << "NULL cssSmartRef"; return; }
  if(sr->ptr()) {
    sr->ptr()->GetTypeDef()->OutputInherit(fh);
    return;
  }
  cssTA::InheritInfo(fh);
}

cssEl* cssSmartRef::GetToken(int idx) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return &cssMisc::Void;
  if(sr->ptr()) {
    void* rval = sr->ptr()->GetTypeDef()->tokens[idx];
    return new cssTA(rval, 1, sr->ptr()->GetTypeDef());
  }
  return cssTA::GetToken(idx);
}

void cssSmartRef::TokenInfo(ostream& fh) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return;
  if(sr->ptr()) {
    sr->ptr()->GetTypeDef()->tokens.List(fh);
  }
  cssTA::TokenInfo(fh);
}

cssSmartRef::operator void*() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return NULL;
  return sr->ptr();
}

cssSmartRef::operator bool() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return false;
  return (bool)sr->ptr();
}

void* cssSmartRef::GetVoidPtrOfType(TypeDef* td) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return NULL;
  if(sr->ptr()) {
    return sr->ptr()->GetTypeDef()->GetParAddr(td, sr->ptr());
  }
  return cssTA::GetVoidPtrOfType(td);
}

void* cssSmartRef::GetVoidPtrOfType(const char* td) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return NULL;
  if(sr->ptr()) {
    return sr->ptr()->GetTypeDef()->GetParAddr(td, sr->ptr());
  }
  return cssTA::GetVoidPtrOfType(td);
}  

Variant cssSmartRef::GetVar() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return _nilVariant;
  if(sr->ptr()) {
    return Variant(sr->ptr());
  }
  return cssTA::GetVar();
}

String cssSmartRef::GetStr() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return "NULL";
  if(sr->ptr()) {
    return sr->ptr()->GetTypeDef()->GetValStr(sr->ptr());
  }
  return cssTA::GetStr();
}

cssSmartRef::operator taBase*() const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(sr && sr->ptr()) {
    return sr->ptr();
  }
  return NULL;
}

void cssSmartRef::operator=(taBase* cp) {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return;
  sr->set(cp);
}

void cssSmartRef::operator=(taBase** cp) {
  if(!cp) {
    cssMisc::Error(prog, "Failed to assign from taBase** -- pointer is NULL");
    return;
  }
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return;
  sr->set(*cp);
}

void cssSmartRef::operator=(const String& s) {
  type_def->SetValStr(s, GetVoidPtr());	// treats string as a path to object..
}

void cssSmartRef::operator=(const cssEl& s) {
  PtrAssignPtr(s);		// this is the only copy op supported..
}

void cssSmartRef::PtrAssignPtr(const cssEl& s) {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(!sr) return;
  sr->set((taBase*)s);	// set as a taptr
}

void cssSmartRef::UpdateAfterEdit() {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(cssMisc::call_update_after_edit && sr->ptr()) {
    sr->ptr()->UpdateAfterEdit();
    return;
  }
  // nothing left to do: not avail: sr->UpdateAfterEdit();
}

cssEl* cssSmartRef::operator[](Variant i) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(sr->ptr())
    return GetElement_impl(sr->ptr(), i);
  return cssTA::operator[](i);
}

cssEl* cssSmartRef::GetMemberFmName(const char* memb) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(sr->ptr())
    return GetMemberFmName_impl(sr->ptr()->GetTypeDef(), sr->ptr(), memb);
  return cssTA::GetMemberFmName(memb);
}

cssEl* cssSmartRef::GetMemberFmNo(int memb) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(sr->ptr())
    return GetMemberFmNo_impl(sr->ptr()->GetTypeDef(), sr->ptr(), memb);
  return cssTA::GetMemberFmNo(memb);
}

cssEl* cssSmartRef::GetMethodFmName(const char* memb) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(sr->ptr())
    return GetMethodFmName_impl(sr->ptr()->GetTypeDef(), sr->ptr(), memb);
  return cssTA::GetMethodFmName(memb);
}

cssEl* cssSmartRef::GetMethodFmNo(int memb) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(sr->ptr())
    return GetMethodFmNo_impl(sr->ptr()->GetTypeDef(), sr->ptr(), memb);
  return cssTA::GetMethodFmNo(memb);
}

cssEl* cssSmartRef::GetScoped(const char* memb) const {
  taSmartRef* sr = (taSmartRef*)GetVoidPtr();
  if(sr->ptr())
    return GetScoped_impl(sr->ptr()->GetTypeDef(), sr->ptr(), memb);
  return cssTA::GetScoped(memb);
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
    if(strm) {
      if(s.GetType() == T_Int)
	*strm << (Int) s;
      else if(s.GetType() == T_Real)
	*strm << (Real) s;
      else
	*strm << (const char*)s;
      // this makes too many linebreaks:
//       taMisc::FlushConsole();
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
		       type_def->name, "!=", st->type_def->name);
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
cssSStream::cssSStream(const char* nm)
  : cssIOS(NULL, 1, &TA_stringstream, nm)
{ Constr(); }
cssSStream::cssSStream(const cssSStream& cp)
  : cssIOS(cp)
{ Constr(); }
cssSStream::cssSStream(const cssSStream& cp, const char*)
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

const char* cssTypeDef::GetTypeName() const {
//   void* pt = GetVoidPtr();
//   if(pt)
//     return ((TypeDef*)pt)->name;
  // just use default typedef name
  return cssTA::GetTypeName();
}

void cssTypeDef::Print(ostream& fh) const {
  fh << PrintStr();
}

void cssTypeDef::PrintR(ostream& fh) const {
  fh << PrintStr();
}

void cssTypeDef::TypeInfo(ostream& fh) const {
  for(int i=1;i<ptr_cnt;i++) fh << "*"; // ptr cnt
  void* pt = GetVoidPtr();
  if(pt)
    ((TypeDef*)pt)->OutputType(fh);
  else
    type_def->OutputType(fh);
}

void cssTypeDef::InheritInfo(ostream& fh) const {
  void* pt = GetVoidPtr();
  if(pt)
    ((TypeDef*)pt)->OutputInherit(fh) << "\n";
  else
    type_def->OutputInherit(fh);
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
    return rval;
  else if(ptr_cnt == 1)
    return ((TypeDef*)ptr)->name;
  else if((ptr_cnt == 2) && *((TypeDef**)ptr))
    return (*((TypeDef**)ptr))->name;
  return type_def->name;
}

void cssTypeDef::operator=(const String& s) {
  if(ptr_cnt == 1) {
    TypeDef* td = taMisc::types.FindName((const char*)s);
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
