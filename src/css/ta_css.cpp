// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


// ta_css.cc
// support for taBase type in css

#include <sstream>
#include "ta_css.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "ta_group.h"
#include "ta_TA_type.h"

#ifdef TA_GUI
#include "ta_qttype_def.h"
#endif

void cssTA::Constr() {
  type_def = NULL;
}

cssTA::cssTA() : cssCPtr() {
  Constr();
}
cssTA::cssTA(void* it, int pc, TypeDef* td) : cssCPtr(it,pc) {
  Constr();
  type_def = td;
}
cssTA::cssTA(void* it, int pc, TypeDef* td, const char* nm) : cssCPtr(it,pc,nm) {
  Constr();
  type_def = td;
}
cssTA::cssTA(void* it, int pc, TypeDef* td, const char* nm, cssEl* cp, bool ro)
: cssCPtr(it,pc,nm,cp,ro) {
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
  if(type_def == NULL)	return &cssMisc::Void;
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
  void* pt = GetVoidPtr();
  String rval;
  if(pt == NULL)
    rval = "NULL";
  else if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_fstream)
	  || type_def->InheritsFrom(&TA_ostream))
    rval = String("<stream>") + String((long)ptr);
  else
    rval = GetStr();
  return rval;
}

void cssTA::TypeInfo(ostream& fh) const {
  void* pt = GetVoidPtr();
  if(type_def->InheritsFrom(&TA_TypeDef) && (pt != NULL))
    ((TypeDef*)pt)->OutputType(fh);
  else
    type_def->OutputType(fh);
}

void cssTA::Print(ostream& fh) const {
  void* pt = GetVoidPtr();
  if((pt != NULL) && !type_def->InheritsFrom(TA_TypeDef))
    type_def->Output(fh, pt);
  else {
    fh << PrintStr();
  }
}

void cssTA::PrintR(ostream& fh) const {
  void* pt = GetVoidPtr();
  if((pt != NULL) && !type_def->InheritsFrom(TA_TypeDef))
    type_def->OutputR(fh, pt);
  else {
    fh << PrintStr();
  }
}

void cssTA::InheritInfo(ostream& fh) const {
  void* pt = GetVoidPtr();
  if(type_def->InheritsFrom(&TA_TypeDef) && (pt != NULL))
    ((TypeDef*)pt)->OutputInherit(fh) << "\n";
  else
    type_def->OutputInherit(fh);
}

#ifdef TA_GUI
int cssTA::Edit(bool wait) {
  //WARNING: does not respect wait
  void* pt = GetVoidPtr();
  if (pt == NULL)
    return false;
  taiEdit* gc = type_def->ie;
  if (gc != NULL) {
    const iColor* bgclr = NULL;
    if (type_def->InheritsFrom(TA_taBase))
      bgclr = ((TAPtr)pt)->GetEditColor();
//3.2    return gc->Edit(pt, NULL, wait, false, bgclr);
    return gc->Edit(pt, false, bgclr);
  }
  return false;
}
#endif // TA_GUI

void cssTA::Save(ostream& fh) {
  void* pt = GetVoidPtr();
  if(pt != NULL) {
    type_def->Dump_Save(fh, pt);
  }
}
void cssTA::Load(istream& fh) {
  void* pt = GetVoidPtr();
  if(pt != NULL) {
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

#ifdef CSS_SUPPORT_TYPEA
cssTA::operator TypeDef*() const {
  if(type_def->InheritsFrom(TA_TypeDef)) {
    if(ptr == NULL)
      return type_def;
    else if(ptr_cnt == 1)
      return (TypeDef*)ptr;
    else if((ptr_cnt == 2) && *((TypeDef**)ptr) != NULL)
      return *((TypeDef**)ptr);
  }
  return type_def;
}

cssTA::operator MemberDef*() const {
  if(type_def->InheritsFrom(TA_MemberDef)) {
    if(ptr == NULL)
      return NULL;
    else if(ptr_cnt == 1)
      return (MemberDef*)ptr;
    else if((ptr_cnt == 2) && *((MemberDef**)ptr) != NULL)
      return *((MemberDef**)ptr);
  }
  return NULL;
}

cssTA::operator MethodDef*() const {
  if(type_def->InheritsFrom(TA_MethodDef)) {
    if(ptr == NULL)
      return NULL;
    else if(ptr_cnt == 1)
      return (MethodDef*)ptr;
    else if((ptr_cnt == 2) && *((MethodDef**)ptr) != NULL)
      return *((MethodDef**)ptr);
  }
  return NULL;
}

#endif

void* cssTA::GetVoidPtrOfType(TypeDef* td) const {
  void* bptr = GetVoidPtr();
  if(bptr == NULL) {
    cssMisc::Error(prog, "Null pointer for conversion to:",td->name,"from:",type_def->name);
    return NULL;
  }
  void* rval = type_def->GetParAddr(td, bptr);
  if(rval == NULL) {
    cssMisc::Error(prog, "Conversion to:",td->name,"is not a base type for:",type_def->name);
  }
  return rval;
}

void* cssTA::GetVoidPtrOfType(const char* td) const {
  void* bptr = GetVoidPtr();
  if(bptr == NULL) {
    cssMisc::Error(prog, "Null pointer for conversion to:",td,"from:",type_def->name);
    return NULL;
  }
  void* rval = type_def->GetParAddr(td, bptr);
  if(rval == NULL) {
    cssMisc::Error(prog, "Conversion to:",td,"is not a base type for:",type_def->name);
  }
  return rval;
}

cssTA::operator TAPtr() const {
  return (TAPtr)GetVoidPtrOfType(&TA_taBase);
}
cssTA::operator TAPtr*() const {
  if(type_def->InheritsFrom(&TA_taBase)) { // allow this one check
    return (TAPtr*)GetVoidPtr(2);
  }
  CvtErr("(TAPtr*)"); return NULL;
}


String& cssTA::GetStr() const {
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_fstream)) {
    if(ptr == NULL)
      return no_string;
    *((istream*)*this) >> ((cssEl*)this)->tmp_str;
    return (String&)tmp_str;
  }
  if(type_def->InheritsFrom(TA_TypeDef)) {
    if(ptr == NULL)
      return no_string;
    else if(ptr_cnt == 1)
      return ((TypeDef*)ptr)->name;
    else if((ptr_cnt == 2) && *((TypeDef**)ptr) != NULL)
      return (*((TypeDef**)ptr))->name;
    return type_def->name;
  }
  else if(type_def->InheritsFrom(TA_MemberDef)) {
    if(ptr == NULL)
      return no_string;
    else if(ptr_cnt == 1)
      ((cssEl*)this)->tmp_str = TA_MemberDef_ptr.GetValStr((void*)&ptr);
    else if((ptr_cnt == 2) && *((MemberDef**)ptr) != NULL)
      ((cssEl*)this)->tmp_str = TA_MemberDef_ptr.GetValStr(((MemberDef**)ptr));
    return (String&)tmp_str;
  }
  else if(type_def->InheritsFrom(TA_MethodDef)) {
    if(ptr == NULL)
      return no_string;
    else if(ptr_cnt == 1)
      ((cssEl*)this)->tmp_str = TA_MethodDef_ptr.GetValStr((void*)&ptr);
    else if((ptr_cnt == 2) && *((MethodDef**)ptr) != NULL)
      ((cssEl*)this)->tmp_str = TA_MethodDef_ptr.GetValStr(((MethodDef**)ptr));
    return (String&)tmp_str;
  }
  if(ptr != NULL) {
    if(ptr_cnt == 1)
      ((cssEl*)this)->tmp_str = type_def->GetValStr(ptr);
    else if(ptr_cnt == 2) {
      // need the correct typedef here, either find it or make it..
      String ptrnm = type_def->name + "_ptr";
      TypeDef* ptr_typ = type_def->children.FindName(ptrnm);
      if(ptr_typ != NULL)
	((cssEl*)this)->tmp_str = ptr_typ->GetValStr(ptr);
      else {
	TypeDef nw_tp(*type_def);
	nw_tp.ptr++;
	((cssEl*)this)->tmp_str = nw_tp.GetValStr(ptr);
      }
    }
    return (String&)tmp_str;
  }
  return type_def->name;
}
cssTA::operator Real() const {
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_fstream)) {
    if(ptr == NULL)
      return 0;
    double rval;
    *((istream*)*this) >> rval;
    return rval;
  }
  return (Real)(cssCPtr)*this;
}
cssTA::operator Int() const {
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_fstream)) {
    if(ptr == NULL)
      return 0;
    int rval;
    *((istream*)*this) >> rval;
    return rval;
  }
  return (Int)(cssCPtr)*this;
}

cssTA::operator iostream*() const {
  if(type_def->InheritsFrom(&TA_iostream)) {
    if(type_def->InheritsFrom(&TA_fstream))
      return (iostream*)(fstream*)GetVoidPtr();
    if(type_def->InheritsFrom(&TA_stringstream))
      return (iostream*)(stringstream*)GetVoidPtr();
    return (iostream*)GetVoidPtr();
  }
  CvtErr("(iostream*)"); return NULL;
}
cssTA::operator istream*() const {
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
cssTA::operator ostream*() const {
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
cssTA::operator fstream*() const {
  if(type_def->InheritsFrom(&TA_fstream)) {
    return (fstream*)GetVoidPtr();
  }
  CvtErr("(fstream*)"); return NULL;
}

cssTA::operator stringstream*() const {
  if(type_def->InheritsFrom(&TA_stringstream)) {
    return (stringstream*)GetVoidPtr();
  }
  CvtErr("(stringstream*)"); return NULL;
}

cssTA::operator iostream**() const {
  if(type_def->InheritsFrom(&TA_iostream)) {
    if(type_def->InheritsFrom(&TA_fstream))
      return (iostream**)(fstream**)GetVoidPtr(2);
    if(type_def->InheritsFrom(&TA_stringstream))
      return (iostream**)(stringstream**)GetVoidPtr(2);
    return (iostream**)GetVoidPtr(2);
  }
  CvtErr("(iostream**)"); return NULL;
}
cssTA::operator istream**() const {
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
cssTA::operator ostream**() const {
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
cssTA::operator fstream**() const {
  if(type_def->InheritsFrom(&TA_fstream)) {
    return (fstream**)GetVoidPtr(2);
  }
  CvtErr("(fstream**)"); return NULL;
}

cssTA::operator stringstream**() const {
  if(type_def->InheritsFrom(&TA_stringstream)) {
    return (stringstream**)GetVoidPtr(2);
  }
  CvtErr("(stringstream**)"); return NULL;
}

cssEl* cssTA::operator<<(cssEl& s) {
  if(type_def->InheritsFrom(&TA_ostream) || type_def->InheritsFrom(&TA_iostream)) {
    ostream* strm = (ostream*)*this;
    if(strm != NULL) {
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
cssEl* cssTA::operator>>(cssEl& s) {
  if(type_def->InheritsFrom(&TA_istream) || type_def->InheritsFrom(&TA_iostream)) {
    istream* strm = (istream*)*this;
    if(strm != NULL) {
      if(s.GetType() == T_Int) {
	cssInt* tmp = (cssInt*)&s;
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
	cssReal* tmp = (cssReal*)&s;
	*strm >> tmp->val;
      }
      else if(s.GetType() == T_Array) {
	cssArray* ary = (cssArray*)&s;
	if((ary->el_type == NULL) || (ary->items == NULL)) {
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
	      c = taMisc::read_till_quote(*strm);
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

void cssTA::PtrAssignPtr(cssCPtr* s) {
  String tpnm = GetTypeName();
  String s_tpnm = s->GetTypeName();
  if(ptr_cnt == s->ptr_cnt) {
    // we are a member pointer to obj (not just a script variable)
    if((ptr_cnt == 1) && (class_parent != NULL) && (ptr != NULL) && (s->ptr != NULL) &&
       (s->GetType() == T_TA))
    {
      if(type_def->InheritsFrom(TA_taBase)) {
	TAPtr obj = (TAPtr)ptr;
	obj->UnSafeCopy((TAPtr)s->ptr);
	class_parent->UpdateAfterEdit();
      }
      else {
	cssMisc::Error(prog, "Cannot copy non-TA_Base objects");
      }
      return;
    }
    if((ptr_cnt == 1) && type_def->InheritsFrom(TA_ios) && (s->GetType() == T_TA) &&
       ((cssTA*)s)->type_def->InheritsFrom(TA_ios))
    {
      cssTA* st = (cssTA*)s;
      if(!st->type_def->InheritsFrom(type_def)) {
	// source must be my type or greater
	cssMisc::Error(prog, "Attempt to assign incompatible ios pointers:",
		       type_def->name, "!=", st->type_def->name);
	return;
      }
      // assignment amongst iostream types..
      if(type_def->InheritsFrom(TA_fstream) || type_def->InheritsFrom(TA_stringstream)) {
	ptr = s->ptr;
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
    }
    ptr = s->ptr;
    if((prog != NULL) && (prog->top->debug) && (tpnm != s_tpnm))
      cssMisc::Warning(prog, "Warning: assigning different ptr types");
  }
  else if(ptr_cnt == s->ptr_cnt + 1) {
    if((ptr != NULL)) {
      if(!ROCheck()) return;
      // note that this means that the css ptr points to a ptr, which is whats
      // gettting set here, not the css ptr
      if(type_def->InheritsFrom(&TA_taBase))
	taBase::SetPointer((TAPtr*)ptr, (TAPtr)s->ptr);
      else
	*((void**)ptr) = s->ptr;
      if(class_parent != NULL)	class_parent->UpdateAfterEdit();

      if((prog != NULL) && (prog->top->debug) && (tpnm != s_tpnm))
	cssMisc::Warning(prog, "Warning: assigning different ptr types");
    }
  }
  else {
    cssMisc::Error(prog, "Pointer assignment between incompatible pointers");
  }
}

void cssTA::operator=(const String& s) {
  if(ptr_cnt == 1) {
    if(type_def->HasOption("INLINE") || type_def->InheritsFrom(TA_taList_impl)
       || type_def->InheritsFrom(TA_taArray_base)
       || type_def->InheritsFrom(TA_taArray_impl) || type_def->InheritsFrom(TA_void))
    {
      if(ptr != NULL)
	type_def->SetValStr((const char*)s, ptr);
      return;
    }
    // we don't really have a pointer here, so don't use SetPointer
    // but treat string as a pointer string
    if(type_def->InheritsFrom(TA_taBase)) {
      MemberDef* md = NULL;
      String pth = s;
      TAPtr bs = tabMisc::root->FindFromPath(pth, md);	// it can eat the string
      if((bs == NULL) || (md == NULL))
	return;
      if(md->type->ptr == 1)
	bs = *((TAPtr*)bs);
      ptr = (void*)bs;
    }
    else if(type_def->InheritsFrom(TA_TypeDef)) {
      TypeDef* td = taMisc::types.FindName((const char*)s);
      if(td != NULL)
	ptr = (void*)td;
    }
    else if(type_def->InheritsFrom(TA_MemberDef)) {
      TA_MemberDef_ptr.SetValStr(s, (void*)&ptr);
    }
    else if(type_def->InheritsFrom(TA_MethodDef)) {
      TA_MethodDef_ptr.SetValStr(s, (void*)&ptr);
    }
  }
  else if((ptr != NULL) && (ptr_cnt == 2)) {	// otherwise treat as a pointer
    // need the correct typedef here, either find it or make it..
    String ptrnm = type_def->name + "_ptr";
    TypeDef* ptr_typ = type_def->children.FindName(ptrnm);
    if(ptr_typ != NULL)
      ptr_typ->SetValStr((const char*)s, ptr);
    else {
      TypeDef nw_tp(*type_def);
      nw_tp.ptr++;
      nw_tp.SetValStr((const char*)s, ptr);
    }
  }
}

void cssTA::operator=(const cssEl& s) {
  if(type_def->InheritsFrom(&TA_TypeDef) && (ptr_cnt==2)) { // if im a typedef-ptr-ptr
    if(s.GetType() == T_TA) {
      cssTA *tmp = (cssTA*)&s;
      if(tmp->type_def->InheritsFrom(&TA_TypeDef))
	PtrAssignPtr(tmp);
      else {
	*((TypeDef**)ptr) = tmp->type_def;	// get its typedef
	if(class_parent != NULL)	class_parent->UpdateAfterEdit();
      }
    }
    else if(s.GetType() == T_C_Ptr)
      PtrAssignPtr((cssCPtr*)&s);
    else {
      *((TypeDef**)ptr) = (TypeDef*)s;		// get the name
      if(class_parent != NULL)	class_parent->UpdateAfterEdit();
    }
  }
  else if(type_def->InheritsFrom(&TA_MemberDef) && (ptr_cnt==2)) { // if im a memberdef-ptr-ptr
    if(s.GetType() == T_TA) {
      cssTA *tmp = (cssTA*)&s;
      if(tmp->type_def->InheritsFrom(&TA_MemberDef))
	PtrAssignPtr(tmp);
      else {
	*((MemberDef**)ptr) = (MemberDef*)s;
	if(class_parent != NULL)	class_parent->UpdateAfterEdit();
      }
    }
    else if(s.GetType() == T_C_Ptr)
      PtrAssignPtr((cssCPtr*)&s);
    else {
      *((MemberDef**)ptr) = (MemberDef*)s;		// get the name
      if(class_parent != NULL)	class_parent->UpdateAfterEdit();
    }
  }
  else if(type_def->InheritsFrom(&TA_MethodDef) && (ptr_cnt==2)) { // if im a memberdef-ptr-ptr
    if(s.GetType() == T_TA) {
      cssTA *tmp = (cssTA*)&s;
      if(tmp->type_def->InheritsFrom(&TA_MethodDef))
	PtrAssignPtr(tmp);
      else {
	*((MethodDef**)ptr) = (MethodDef*)s;
	if(class_parent != NULL) class_parent->UpdateAfterEdit();
      }
    }
    else if(s.GetType() == T_C_Ptr)
      PtrAssignPtr((cssCPtr*)&s);
    else {
      *((MethodDef**)ptr) = (MethodDef*)s;		// get the name
      if(class_parent != NULL)	class_parent->UpdateAfterEdit();
    }
  }
  else {
    if(s.GetType() == T_TA) {
      PtrAssignPtr((cssCPtr*)&s);
    }
    else if(s.GetType() == T_C_Ptr)
      cssMisc::Error(prog, "Assigning typed pointer to non-typed ptr value");
    else if((s.GetType() == T_String) && (ptr != NULL)) {
      *this = s.GetStr();	// use string converter
      if(class_parent != NULL)	class_parent->UpdateAfterEdit();
    }
    else if(s.GetType() == T_MbrCFun) {
      if((ptr_cnt == 1) && type_def->InheritsFrom(TA_void)) {
	cssMbrCFun& mbf = (cssMbrCFun&)s;
	MethodDef* fun = TA_taRegFun.methods.FindName(mbf.name);
	if(fun != NULL)
	  *((ta_void_fun*)ptr) = fun->addr;
	else {
	  cssMisc::Error(prog, "Assignment to unregistered member function:", mbf.name);
	}
      }
    }
    else {
      int sval = (Int)s;
      if(sval == 0) {
	if(ptr_cnt > 1) {
	  *((void**)ptr) = NULL;
	  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
	}
	else
	  ptr = NULL;
      }
      else
	cssMisc::Error(prog, "Assigning internal pointer to non-internal ptr value");
    }
  }
}

void cssTA::InitAssign(const cssEl& s) {
  if(s.GetType() == T_TA) {
    PtrAssignPtr((cssCPtr*)&s);
    cssTA *tmp = (cssTA*)&s;
    type_def = tmp->type_def;
  }
  else if(s.GetType() == T_C_Ptr) {
    PtrAssignPtr((cssCPtr*)&s);
  }
  else {
    int sval = (Int)s;
    if(sval == 0)
      ptr = NULL;
    else
      cssMisc::Error(prog, "Assigning internal pointer to non-internal ptr value");
  }
}

void cssTA::CastFm(const cssEl& s) {
  if((s.GetType() == T_TA) || (s.GetType() == T_C_Ptr)) {
    PtrAssignPtr((cssCPtr*)&s);
  }
  else cssMisc::Error(prog, "Casting internal pointer to non-internal ptr value");
}

void cssTA::UpdateAfterEdit() {
  if(type_def->InheritsFrom(&TA_taBase)) {
    TAPtr ths = (TAPtr)GetVoidPtr();
    if(ths != NULL) ths->UpdateAfterEdit();
  }
}

cssEl* cssTA::operator[](int i) const {
  if(type_def->InheritsFrom(&TA_taBase)) {
    TAPtr ths = (TAPtr)GetVoidPtr();
    if(ths != NULL)
      return GetElement_impl(ths, i);
    cssMisc::Error(prog, "operator[]: NULL pointer");
    return &cssMisc::Void;
  }
  NopErr("[]");
  return &cssMisc::Void;
}

cssEl* cssTA::GetElement_impl(TAPtr ths, int i) const {
  TypeDef* eltd;
  void* el = ths->GetTA_Element(i, eltd);
  if(eltd == NULL) {
    cssMisc::Error(prog, "This is not an array:", ths->GetName(),
		    "type:", ths->GetTypeDef()->name);
    return &cssMisc::Void;
  }
  if(el == NULL) {
    cssMisc::Error(prog, "Index out of range:", String((int)i), "for", ths->GetName(),
		    "type:", ths->GetTypeDef()->name);
    return &cssMisc::Void;
  }
  String nm = ths->GetName() + String("[") + String((int)i) + "]";
  return GetFromTA(eltd, el, nm, (cssTA*)this);
}

int cssTA::GetMemberNo(const char* memb) const {
  int md;
  type_def->members.FindName(memb, md);	// just 1st order search
  return md;
}

cssEl* cssTA::GetMember(const char* memb) const {
  void* pt = GetVoidPtr();
  if(pt == NULL) {
    cssMisc::Error(prog, "GetMember: NULL pointer");
    return &cssMisc::Void;
  }

  void* mbr;
  MemberDef* md = type_def->members.FindNameAddrR(memb, pt, mbr);
  if(md == NULL) {
    cssMisc::Error(prog, "Member not found:", memb, "in class of type:", (char*)type_def->name);
    return &cssMisc::Void;
  }

  return GetMember_impl(md, mbr);
}

cssEl* cssTA::GetMember(int memb) const {
  void* pt = GetVoidPtr();
  if(pt == NULL) {
    cssMisc::Error(prog, "GetMember: NULL pointer");
    return &cssMisc::Void;
  }

  MemberDef* md = type_def->members.SafeEl(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Member not found:", String(memb), "in class of type:",
		    (const char*)type_def->name);
    return &cssMisc::Void;
  }
  void* mbr = md->GetOff(pt);
  return GetMember_impl(md, mbr);
}

cssEl* cssTA::GetMember_impl(MemberDef* md, void* mbr) const {
  return GetFromTA(md->type, mbr, md->name, (cssTA*)this, md);
}

cssEl* cssTA::GetFromTA(TypeDef* td, void* itm, const char* nm, cssTA*, MemberDef* md) const {
  TypeDef* nptd;

  nptd = td->GetNonPtrType(); // always create one of these

  if(nptd == NULL)
    return &cssMisc::Void;

  bool ro = false;
  if(md != NULL) {
    if(md->HasOption("READ_ONLY"))
      ro = true;
  }

  if(nptd->DerivesFrom(TA_bool))
    return new cssCPtr_bool(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFormal(TA_enum))
    return new cssCPtr_enum(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_int))
    return new cssCPtr_int(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_short))
    return new cssCPtr_short(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_long))
    return new cssCPtr_long(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_char))
    return new cssCPtr_char(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_unsigned) || nptd->DerivesFrom(TA_signed))
    return new cssCPtr_int(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_float))
    return new cssCPtr_float(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_double))
    return new cssCPtr_double(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_taString))
    return new cssCPtr_String(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_taBase))
    return new cssTA_Base(itm, td->ptr+1, nptd, nm, (cssEl*)this, ro);

  return new cssTA(itm, td->ptr+1, nptd, nm, (cssEl*)this, ro);
}

int cssTA::GetMemberFunNo(const char* memb) const {
  int md;
  type_def->methods.FindName(memb, md);
  return md;
}
cssEl* cssTA::GetMemberFun(const char* memb) const {
  void* pt = GetVoidPtr();
  if(pt == NULL) {
    cssMisc::Error(prog, "GetMemberFun: NULL pointer");
    return &cssMisc::Void;
  }

  MethodDef* md = type_def->methods.FindName(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Member function not found:", memb, "in class of type:",
	      type_def->name);
    return &cssMisc::Void;
  }

  return GetMemberFun_impl(md, pt);
}
cssEl* cssTA::GetMemberFun(int memb) const {
  void* pt = GetVoidPtr();
  if(pt == NULL) {
    cssMisc::Error(prog, "GetMemberFun: NULL pointer");
    return &cssMisc::Void;
  }

  MethodDef* md = type_def->methods.SafeEl(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Member function not found:", String(memb), "in class of type:",
	      type_def->name);
    return &cssMisc::Void;
  }

  return GetMemberFun_impl(md, pt);
}

cssEl* cssTA::GetMemberFun_impl(MethodDef* md, void* pt) const {
  if(md->stubp != NULL) {
    if(md->fun_argd >= 0)
      return new cssMbrCFun(VarArg, pt, md->stubp, md->name);
    else
      return new cssMbrCFun(md->fun_argc, pt, md->stubp, md->name);
  }
  else {
    cssMisc::Error(prog, "Function pointer not callable:", md->name, "of type:", md->type->name,
	      "in class of type:", type_def->name);
    return &cssMisc::Void;
  }
}

cssEl* cssTA::GetScoped(const char* memb) const {
  EnumDef* ed = type_def->FindEnum(memb);
  if(ed != NULL) {
    return new cssInt(ed->enum_no, memb);
  }

  TypeDef* td = type_def->sub_types.FindName(memb);
  if(td != NULL) {
    if(td->DerivesFormal(TA_enum))
      return new cssTAEnum(td);
    return new cssTA(NULL, 1, td);
  }

  void* pt = GetVoidPtr();
  if(pt == NULL) {
    cssMisc::Error(prog, "GetScoped: NULL pointer");
    return &cssMisc::Void;
  }

  MethodDef* meth = type_def->methods.FindName(memb);
  if(meth != NULL) {
    return GetMemberFun_impl(meth, pt);
  }

  void* mbr;
  MemberDef* md = type_def->members.FindNameAddr(memb, pt, mbr);
  if(md != NULL) {
    return GetMember_impl(md, mbr);
  }

  cssMisc::Error(prog, "Scoped element not found:", memb, "in class of type:", type_def->name);
  return &cssMisc::Void;
}

// cssTA_Base Functions (do the TAPtr version of the original command)

void cssTA_Base::Constr() {
  TAPtr ths = GetTAPtr();
  if(ths == NULL)
    return;
  type_def = ths->GetTypeDef();	// just to be sure
}

String cssTA_Base::PrintStr() const {
  return String("(") + GetTypeName() + ") " + name + " = " + PrintFStr();
}

String cssTA_Base::PrintFStr() const {
  String retv;
  TAPtr ths = GetTAPtr();
  if(ths == NULL)
    retv = "NULL";
  else
    retv = GetStr();
  return retv;
}

void cssTA_Base::TypeInfo(ostream& fh) const {
  TAPtr ths = GetTAPtr();
  if(ths)
    ths->OutputType(fh);
  else
    type_def->OutputType(fh);
}

void cssTA_Base::Print(ostream& fh) const {
  TAPtr ths = GetTAPtr();
  if(ths)
    ths->Output(fh) << "\n";
  else {
    fh << PrintStr();
  }
}

void cssTA_Base::PrintR(ostream& fh) const {
  TAPtr ths = GetTAPtr();
  if(ths)
    ths->OutputR(fh) << "\n";
  else {
    fh << PrintStr();
  }
}

void cssTA_Base::InheritInfo(ostream& fh) const {
  TAPtr ths = GetTAPtr();
  if(ths)
    ths->OutputInherit(fh) << "\n";
  else
    type_def->OutputInherit(fh) << "\n";
}

void cssTA_Base::Save(ostream& fh) {
  TAPtr ths = GetTAPtr();
  if(ths)
    ths->Save(fh);
}
void cssTA_Base::Load(istream& fh) {
  TAPtr ths = GetTAPtr();
  if(ths)
    ths->Load(fh);
}

void cssTA_Base::Assign_impl(const cssEl&) {
  TAPtr ths = GetTAPtr();
  if(ths == NULL)
    return;
//   if(s.GetType() == T_TA) {
//     cssTA *tmp = (cssTA*)&s;
//     if(name != "") {		// only if we have a name ourselves
//       String tmpnm = ths->GetName();
//       if((tmpnm == "") || (tmpnm.before("_", -1) == ths->GetTypeDef()->name))
// 	ths->SetName(name);
//     }
//   }
//   else if(s.GetType() == T_String)
//     ths->SetName((const char*)s);

  type_def = ths->GetTypeDef();	// just to be sure
}

void cssTA_Base::operator=(const cssEl& s) {
  cssTA::operator=(s);
  Assign_impl(s);
}
void cssTA_Base::InitAssign(const cssEl& s) {
  cssTA::InitAssign(s);
  Assign_impl(s);
}

void cssTA_Base::UpdateAfterEdit() {
  TAPtr ths = GetTAPtr();
  if(ths) ths->UpdateAfterEdit();
}

cssEl* cssTA_Base::operator[](int i) const {
  TAPtr ths = GetTAPtr();
  if(ths != NULL)
    return GetElement_impl(ths, i);
  cssMisc::Error(prog, "operator[]: NULL pointer");
  return &cssMisc::Void;
}

cssEl* cssTA_Base::GetMember(const char* memb) const {
  TAPtr ths = GetTAPtr();

  if(ths == NULL) {
    cssMisc::Error(prog, "GetMember: NULL pointer");
    return &cssMisc::Void;
  }

  void* mbr;
  MemberDef* md = ths->FindMembeR(memb, mbr);
  if(md == 0) {
    cssMisc::Error(prog, "Member not found:", memb, "in class of type:", (char*)type_def->name);
    return &cssMisc::Void;
  }

  return GetMember_impl(md, mbr);
}

cssEl* cssTA_Base::NewOpr() {
  TAPtr nw = taBase::MakeToken(type_def);
  if(nw == NULL) {
    cssMisc::Error(prog, "New token of type:", type_def->name, "could not be made");
    return &cssMisc::Void;
  }
  taBase::Ref(nw);			// refer to this
  return new cssTA_Base((void*)nw, 1, type_def); // this guy points to it..
}

void cssTA_Base::DelOpr() {
  TAPtr ths = GetTAPtr();
  if(ths == NULL) {
    cssMisc::Error(prog, "delete: NULL pointer");
    return;
  }
  taBase::UnRef(ths);
  ptr = NULL;			// no longer point to this..
}


void cssTA_Base::InstallThis(cssProgSpace* ps) {
  TAPtr ths = GetTAPtr();
  if(ths == NULL)
    return;

  ps->hard_vars.PushUniqNameNew(this);

  int i;
  cssEl* tmp;
  for(i=0; i<type_def->members.size; i++) {
    MemberDef* md = type_def->members.FastEl(i);
    tmp = GetMember_impl(md, md->GetOff((void*)ths));
    ps->hard_vars.PushUniqNameNew(tmp);
  }
  for(i=0; i<type_def->methods.size; i++) {
    MethodDef* md = type_def->methods.FastEl(i);
    tmp = GetMemberFun_impl(md, (void*)ths);
    ps->hard_funs.PushUniqNameNew(tmp);
  }
}

////////////////////////////////

TypeDef* cssFStream::TA_TypeDef() {
  return &TA_fstream;
}

void cssSStream::Constr() {
  ptr = new std::stringstream;
}

cssSStream::cssSStream()
  : cssTA(NULL, 1, &TA_stringstream)
{ Constr(); }
cssSStream::cssSStream(const char* nm)
  : cssTA(NULL, 1, &TA_stringstream, nm)
{ Constr(); }
cssSStream::cssSStream(const cssSStream& cp)
  : cssTA(cp)
{ Constr(); }
cssSStream::cssSStream(const cssSStream& cp, const char*)
  : cssTA(cp)
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

//////////////////////////////////
// 	cssTAEnum		//
//////////////////////////////////

void cssTAEnum::operator=(const String& cp) {
  type_def->SetValStr((const char*)cp, ptr);
//    cssElPtr val_ptr = type_def->enums->FindName((const char*)cp);
//    if(val_ptr == 0)
//      val = (int)strtol((const char*)cp, NULL, 0);
//    else
//      val = ((cssTAEnum*)val_ptr.El())->val;
}

void cssTAEnum::operator=(const cssEl& s) {
  if(s.GetType() == T_String)
    *this = s.GetStr();		// use string converter
  else
    val = (Int)s;
}

bool cssTAEnum::operator==(cssEl& s) {
  EnumDef* ed = type_def->FindEnum((const char*)s);
  if(ed == NULL)
    return (val == (Int)s);
  else
    return (val == ed->enum_no);
}

bool cssTAEnum::operator!=(cssEl& s) {
  EnumDef* ed = type_def->FindEnum((const char*)s);
  if(ed == 0)
    return (val != (Int)s);
  else
    return (val != ed->enum_no);
}
