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


// css_ptr_i.cc

#include "css_c_ptr_types.h"
#include "css_ta.h"

#include "ta_matrix.h"
#include "ta_project.h"
#include "ta_TA_type.h"

///////////////////
//     int       //
///////////////////

int cssCPtr_int::null_int = 0;

int& cssCPtr_int::GetIntRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((int*)nnp);
  }
  else
    NopErr(opr);
  return null_int;
}

void cssCPtr_int::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  GetIntRef("=") = (Int)t;
  UpdateClassParent();
}
void cssCPtr_int::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef("+=") += (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef("-=") -= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef("*=") *= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef("/=") /= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef("%=") %= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator<<=(cssEl& t) {
  if(!ROCheck()) return;
  GetIntRef("<<=") <<= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator>>=(cssEl& t) {
  if(!ROCheck()) return;
  GetIntRef(">>=") >>= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef("&=") &= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef("^=") ^= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef("|=") |= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}

///////////////////
//     bool      //
///////////////////

bool cssCPtr_bool::null_bool = 0;

bool& cssCPtr_bool::GetBoolRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((bool*)nnp);
  }
  else
    NopErr(opr);
  return null_bool;
}

String cssCPtr_bool::GetStr() const {
  bool myb = GetBoolRef("(String)");
  if(myb)
    return String("true");
  else 
    return String("false");
}

void cssCPtr_bool::operator=(const String& cp) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) { NopErr("=(String)"); return; }
  bool myb = GetBoolRef("=");
  if(cp == "false")
    myb = false;
  else if(cp == "true")
    myb = true;
  else
    myb = (bool)(int)cp;
}

void cssCPtr_bool::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String))
    *this = t.GetStr();	// use string converter
  else
    GetBoolRef("=") = (Int)t;
  UpdateClassParent();
}

void cssCPtr_bool::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetBoolRef("&=") &= (bool)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_bool::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetBoolRef("^=") ^= (bool)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_bool::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetBoolRef("|=") |= (bool)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}

///////////////////
//     short     //
///////////////////

short cssCPtr_short::null_short = 0;

short& cssCPtr_short::GetShortRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((short*)nnp);
  }
  else
    NopErr(opr);
  return null_short;
}

void cssCPtr_short::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  GetShortRef("=") = (Int)t;
  UpdateClassParent();
}
void cssCPtr_short::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef("+=") += (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef("-=") -= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef("*=") *= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef("/=") /= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef("%=") %= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef("<<=") <<= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef(">>=") >>= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef("&=") &= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef("^=") ^= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef("|=") |= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}

///////////////////
//     long      //
///////////////////

long cssCPtr_long::null_long = 0;

long& cssCPtr_long::GetLongRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((long*)nnp);
  }
  else
    NopErr(opr);
  return null_long;
}

void cssCPtr_long::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  GetLongRef("=") = (Int)t;
  UpdateClassParent();
}
void cssCPtr_long::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef("+=") += (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef("-=") -= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef("*=") *= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef("/=") /= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef("%=") %= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef("<<=") <<= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef(">>=") >>= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef("&=") &= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef("^=") ^= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef("|=") |= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}

///////////////////
//  long_long    //
///////////////////

int64_t cssCPtr_long_long::null_long_long = 0;

int64_t& cssCPtr_long_long::GetLongLongRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((int64_t*)nnp);
  }
  else
    NopErr(opr);
  return null_long_long;
}

void cssCPtr_long_long::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  GetLongLongRef("=") = (Int)t;
  UpdateClassParent();
}
void cssCPtr_long_long::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef("+=") += (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef("-=") -= (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef("*=") *= (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef("/=") /= (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef("%=") %= (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef("<<=") <<= (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef(">>=") >>= (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef("&=") &= (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef("^=") ^= (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef("|=") |= (int64_t)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}

///////////////////
//     char      //
///////////////////

char cssCPtr_char::null_char = 0;

char& cssCPtr_char::GetCharRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((char*)nnp);
  }
  else
    NopErr(opr);
  return null_char;
}

void cssCPtr_char::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String))
    *this = t.GetStr();	// use string converter
  else
    GetCharRef("=") = (Int)t;
  UpdateClassParent();
}
void cssCPtr_char::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef("+=") += (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef("-=") -= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef("*=") *= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef("/=") /= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef("%=") %= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef("<<=") <<= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef(">>=") >>= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef("&=") &= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef("^=") ^= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef("|=") |= (Int)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}

///////////////////
//     enum      //
///////////////////

TypeDef* cssCPtr_enum::GetEnumType() const {
  if(enum_type) return enum_type;
  if((class_parent) && (class_parent->GetType() == T_TA)) {
    cssTA* clp = (cssTA*)class_parent;
    if(clp->type_def) {
      MemberDef* md = clp->type_def->members.FindName(name); // find me..
      if(md)
	return md->type;
    }
  }
  return NULL;
}

String cssCPtr_enum::GetStr() const {
  if(ptr_cnt > 0) { NopErr("(String)"); return ""; }
  TypeDef* et = GetEnumType();
  if(et) {
    return et->GetValStr(GetNonNullVoidPtr("(String)"), NULL);
  }
  return String((Int)*this);
}

void cssCPtr_enum::operator=(const String& cp) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) { NopErr("=(String)"); return; }
  TypeDef* et = GetEnumType();
  if(et) {
    cssTA* clp = (cssTA*)class_parent;
    if(clp) {
      et->SetValStr(cp, GetNonNullVoidPtr("="), clp->GetNonNullVoidPtr("="));
      class_parent->UpdateAfterEdit();
    }
    else
      et->SetValStr(cp, GetNonNullVoidPtr("="));
  }
}
void cssCPtr_enum::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String))
    *this = t.GetStr();	// use string converter
  else
    GetIntRef("=") = (Int)t;
  UpdateClassParent();
}

bool cssCPtr_enum::operator==(cssEl& t) {
  if(ptr_cnt > 0) return cssCPtr::operator==(t);
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String)) {
    return GetStr() == t.GetStr();
  }
  return GetIntRef("==") == (Int)t;
}
bool cssCPtr_enum::operator!=(cssEl& t) {
  if(ptr_cnt > 0) return cssCPtr::operator!=(t);
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String)) {
    return GetStr() != t.GetStr();
  }
  return GetIntRef("!=") != (Int)t;
}


///////////////////
//     double    //
///////////////////

double cssCPtr_double::null_double = 0.0;

double& cssCPtr_double::GetDoubleRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((double*)nnp);
  }
  else
    NopErr(opr);
  return null_double;
}

void cssCPtr_double::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  GetDoubleRef("=") = (Real)t;
  UpdateClassParent();
}
void cssCPtr_double::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetDoubleRef("+=") += (Real)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_double::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetDoubleRef("-=") -= (Real)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_double::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetDoubleRef("*=") *= (Real)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_double::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetDoubleRef("/=") /= (Real)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}

///////////////////
//     float     //
///////////////////

float cssCPtr_float::null_float = 0.0;

float& cssCPtr_float::GetFloatRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((float*)nnp);
  }
  else
    NopErr(opr);
  return null_float;
}
void cssCPtr_float::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  GetFloatRef("=") = (Real)t;
  UpdateClassParent();
}
void cssCPtr_float::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetFloatRef("+=") += (Real)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_float::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetFloatRef("-=") -= (Real)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_float::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetFloatRef("*=") *= (Real)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_float::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetFloatRef("/=") /= (Real)t;
  if(class_parent)	class_parent->UpdateAfterEdit();
}


///////////////////
//     String    //
///////////////////

String cssCPtr_String::null_string;


cssCPtr_String::~cssCPtr_String() {
  if(gf != NULL)
    taRefN::unRefDone(gf);
  gf = NULL;
}

void cssCPtr_String::Constr() {
  gf = NULL;
}
String& cssCPtr_String::GetStringRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((String*)nnp);
  }
  else
    NopErr(opr);
  return null_string;
}
void cssCPtr_String::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  GetStringRef("=") = t.GetStr();
  UpdateClassParent();
}
void cssCPtr_String::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetStringRef("+=") += t.GetStr();
  if(class_parent)	class_parent->UpdateAfterEdit();
}

cssEl* cssCPtr_String::operator[](int idx) const {
  if(ptr_cnt > 0) { NopErr("[]"); return &cssMisc::Void; }
  String& val = GetStringRef("[]");
  String nw_val = val.elem(idx);
  return new cssString(nw_val);
}

cssCPtr_String::operator bool() const {
  if(ptr_cnt > 0) return (bool)ptr;
  String& val = GetStringRef("(bool)");
  if(val == "true")
    return true;
  else if(val == "false")
    return false;
  else 
    return !val.empty();
}

cssCPtr_String::operator taBase*() const {
  if(ptr_cnt > 0) { CvtErr("taBase*"); return NULL; }
  void* rval = NULL;
  if((tabMisc::root) && (tabMisc::root->FindMembeR(GetStringRef(), rval) != 0))
    return (taBase*)rval;
  return (taBase*)NULL;
}

cssCPtr_String::operator TypeDef*() const {
  if(ptr_cnt > 0) { CvtErr("taBase*"); return NULL; }
  String& nm = GetStringRef();
  TypeDef* td = taMisc::types.FindName(nm);
  if(td == NULL) {
    cssMisc::Error(prog, "Could not find type:", nm);
    return NULL;
  }
  return td;
}

cssCPtr_String::operator MemberDef*() const {
  if(ptr_cnt > 0) { CvtErr("taBase*"); return NULL; }
  String& nm = GetStringRef();
  MemberDef* md;
  TA_MemberDef_ptr.SetValStr(nm, (void*)&md);
  if(md == NULL) {
    cssMisc::Error(prog, "Could not find member def:", nm);
    return NULL;
  }
  return md;
}

cssCPtr_String::operator MethodDef*() const {
  if(ptr_cnt > 0) { CvtErr("taBase*"); return NULL; }
  String& nm = GetStringRef();
  MethodDef* md;
  TA_MethodDef_ptr.SetValStr(nm, (void*)&md);
  if(md == NULL) {
    cssMisc::Error(prog, "Could not find method def:", nm);
    return NULL;
  }
  return md;
}

cssCPtr_String::operator ostream*() const {
  String& nm = GetStringRef();
  cssCPtr_String* ths = (cssCPtr_String*)this;
  if(ths->gf == NULL) {
    ths->gf = taFiler::New();
    taRefN::Ref(ths->gf);
  }
  ths->gf->setFileName(nm);
  ostream* strm = ths->gf->open_write();
  if((strm == NULL) || !(ths->gf->open_file)) {
    cssMisc::Error(prog, "String -> ostream*: could not open file", nm);
    return (ostream*)NULL;
  }
  return strm;
}

cssCPtr_String::operator istream*() const {
  String& nm = GetStringRef();
  cssCPtr_String* ths = (cssCPtr_String*)this;
  if(ths->gf == NULL) {
    ths->gf = taFiler::New();
    taRefN::Ref(ths->gf);
  }
  ths->gf->setFileName(nm);
  istream* strm = ths->gf->open_read();
  if((strm == NULL) || !(ths->gf->open_file)) {
    cssMisc::Error(prog, "String -> istream*: could not open file", nm);
    return (istream*)NULL;
  }
  return strm;
}

int cssCPtr_String::GetMethodNo(const char* memb) const {
  return GetMethodNo_impl(&TA_taString, memb);
}

cssEl* cssCPtr_String::GetMethodFmName(const char* memb) const {
  void* sp = GetNonNullVoidPtr("->()");
  if(!sp) return &cssMisc::Void;
  String& val = *((String*)sp);
  return GetMethodFmName_impl(&TA_taString, (void*)&val, memb);
}

cssEl* cssCPtr_String::GetMethodFmNo(int memb) const {
  void* sp = GetNonNullVoidPtr("->()");
  if(!sp) return &cssMisc::Void;
  String& val = *((String*)sp);
  return GetMethodFmNo_impl(&TA_taString, (void*)&val, memb);
}

cssEl* cssCPtr_String::GetScoped(const char* memb) const {
  void* sp = GetNonNullVoidPtr("::");
  if(!sp) return &cssMisc::Void;
  String& val = *((String*)sp);
  return GetScoped_impl(&TA_taString, (void*)&val, memb);
}


///////////////////
//     Variant    //
///////////////////

Variant cssCPtr_Variant::null_var;

Variant& cssCPtr_Variant::GetVarRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr(opr);
    if(nnp) return *((Variant*)nnp);
  }
  else
    NopErr(opr);
  return null_var;
}

cssEl::cssTypes cssCPtr_Variant::GetPtrType() const {
  Variant& val = GetVarRef();
  switch(val.type()) {
  case Variant::T_Invalid:
    return T_Variant;
  case Variant::T_Bool:
    return T_Bool;
  case Variant::T_Int:
    return T_Int;
  case Variant::T_UInt:
    return T_Int;
  case Variant::T_Int64:
    return T_LongLong;
  case Variant::T_UInt64:
    return T_LongLong;
  case Variant::T_Double:
    return T_Real;
  case Variant::T_Char:
    return T_Char;
  case Variant::T_String:
    return T_String;
  case Variant::T_Ptr:
    return T_C_Ptr;
  case Variant::T_Base:
  case Variant::T_Matrix:
    return T_TA;
  }
  return T_Variant;
}

String cssCPtr_Variant::PrintStr() const {
  String rval = String(GetTypeName())+" "+name+" --> ";
  if(GetVoidPtr()) {
    Variant& vl = *((Variant*)GetVoidPtr());
    rval += "(" + vl.getTypeAsString() + ") " + vl.toString();
  }
  else
    rval += "NULL";
  return rval;
}

void cssCPtr_Variant::TypeInfo(ostream& fh) const {
  TypeDef* typ = NULL;  void* base = NULL;
  if(GetVoidPtr()) {
    Variant& val_r = *((Variant*)GetVoidPtr());
    val_r.GetRepInfo(typ, base);
    fh << GetTypeName() << " (" << val_r.getTypeAsString() << ") " << name << ": ";
    if(val_r.type() == Variant::T_String) {
      typ->OutputType(fh);
    }
    else if(val_r.isBaseType()) {
      typ->GetNonPtrType()->OutputType(fh);
    }
    else {
      TA_Variant.OutputType(fh);
    }
  }
  else {
    fh << GetTypeName() << " " << name << ": ";
    TA_Variant.OutputType(fh);
  }
}

void cssCPtr_Variant::InheritInfo(ostream& fh) const {
  TypeDef* typ = NULL;  void* base = NULL;
  if(GetVoidPtr()) {
    Variant& val_r = *((Variant*)GetVoidPtr());
    val_r.GetRepInfo(typ, base);
    if(val_r.type() == Variant::T_String) {
      typ->OutputInherit(fh);
    }
    else if(val_r.isBaseType()) {
      typ->GetNonPtrType()->OutputInherit(fh);
    }
    else {
      TA_Variant.OutputInherit(fh);
    }
  }
  else {
    TA_Variant.OutputInherit(fh);
  }
}

void cssCPtr_Variant::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  GetVarRef("=") = t.GetVar();
  UpdateClassParent();
}
void cssCPtr_Variant::operator=(const Variant& val) {
  GetVarRef() = val;
}

void cssCPtr_Variant::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetVarRef("+=") += t.GetVar();
  if(class_parent)	class_parent->UpdateAfterEdit();
}

void cssCPtr_Variant::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetVarRef("-=") -= t.GetVar();
  if(class_parent)	class_parent->UpdateAfterEdit();
}

void cssCPtr_Variant::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetVarRef("*=") *= t.GetVar();
  if(class_parent)	class_parent->UpdateAfterEdit();
}

void cssCPtr_Variant::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetVarRef("/=") /= t.GetVar();
  if(class_parent)	class_parent->UpdateAfterEdit();
}

cssEl* cssCPtr_Variant::operator[](int idx) const {
  if(ptr_cnt > 0) { NopErr("[]"); return &cssMisc::Void; }
  Variant& val = GetVarRef("[]");
  return GetVariantEl_impl(val, idx);
}

cssEl* cssCPtr_Variant::GetMemberFmNo(int memb) const {
  void* sp = GetNonNullVoidPtr("->");
  if(!sp) return &cssMisc::Void;
  Variant& val = *((Variant*)sp);
  TypeDef* typ = NULL;  void* base = NULL;
  val.GetRepInfo(typ, base);
  if(val.type() == Variant::T_String) {
    return GetMemberFmNo_impl(typ, (String*)&val, memb);
  }
  else if(val.isBaseType()) {
    return GetMemberFmNo_impl(typ, val.toBase(), memb);
  }
  return GetMemberFmNo_impl(&TA_Variant, &val, memb);
}

cssEl* cssCPtr_Variant::GetMemberFmName(const char* memb) const {
  void* sp = GetNonNullVoidPtr("->");
  if(!sp) return &cssMisc::Void;
  Variant& val = *((Variant*)sp);
  TypeDef* typ = NULL;  void* base = NULL;
  val.GetRepInfo(typ, base);
  if(val.type() == Variant::T_String) {
    return GetMemberFmName_impl(typ, (String*)&val, memb);
  }
  else if(val.isBaseType()) {
    return GetMemberFmName_impl(typ, val.toBase(), memb);
  }
  return GetMemberFmName_impl(&TA_Variant, &val, memb);
}

cssEl* cssCPtr_Variant::GetMethodFmNo(int memb) const {
  void* sp = GetNonNullVoidPtr("->()");
  if(!sp) return &cssMisc::Void;
  Variant& val = *((Variant*)sp);
  TypeDef* typ = NULL;  void* base = NULL;
  val.GetRepInfo(typ, base);
  if(val.type() == Variant::T_String) {
    return GetMethodFmNo_impl(typ, (String*)&val, memb);
  }
  else if(val.isBaseType()) {
    return GetMethodFmNo_impl(typ, val.toBase(), memb);
  }
  return GetMethodFmNo_impl(&TA_Variant, &val, memb);
}

cssEl* cssCPtr_Variant::GetMethodFmName(const char* memb) const {
  void* sp = GetNonNullVoidPtr("->()");
  if(!sp) return &cssMisc::Void;
  Variant& val = *((Variant*)sp);
  TypeDef* typ = NULL;  void* base = NULL;
  val.GetRepInfo(typ, base);
  if(val.type() == Variant::T_String) {
    return GetMethodFmName_impl(typ, (String*)&val, memb);
  }
  else if(val.isBaseType()) {
    return GetMethodFmName_impl(typ, val.toBase(), memb);
  }
  return GetMethodFmName_impl(&TA_Variant, &val, memb);
}

cssEl* cssCPtr_Variant::GetScoped(const char* memb) const {
  void* sp = GetNonNullVoidPtr("::");
  if(!sp) return &cssMisc::Void;
  Variant& val = *((Variant*)sp);
  TypeDef* typ = NULL;  void* base = NULL;
  val.GetRepInfo(typ, base);
  if(val.type() == Variant::T_String) {
    return GetScoped_impl(typ, (String*)&val, memb);
  }
  else if(val.isBaseType()) {
    return GetScoped_impl(typ, val.toBase(), memb);
  }
  return GetScoped_impl(&TA_Variant, &val, memb);
}

/////////////////////
//     dynenum     //
/////////////////////

DynEnum cssCPtr_DynEnum::null_enum;

DynEnum& cssCPtr_DynEnum::GetEnumRef(const char* opr) const {
  if(ptr_cnt == 0) {
    void* nnp = GetNonNullVoidPtr();
    if(nnp) return *((DynEnum*)nnp);
  }
  else
    NopErr(opr);
  return null_enum;
}

void cssCPtr_DynEnum::TypeInfo(ostream& fh) const {
  fh << GetTypeName() << name << ": ";
  if(GetVoidPtr() == NULL) {
    fh << "NULL";
  }
  else {
    fh << "\n";
    DynEnum& enm = GetEnumRef();
    if(enm.enum_type)
      enm.enum_type->OutputType(fh);
  }
}

void cssCPtr_DynEnum::operator=(const String& cp) {
  GetEnumRef().SetNameVal(cp);
}
void cssCPtr_DynEnum::operator=(const cssEl& t) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(t);
    return;
  }
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String))
    *this = t.GetStr();	// use string converter
  else
    GetEnumRef("=").SetNumVal((Int)t);
  UpdateClassParent();
}
bool cssCPtr_DynEnum::operator==(cssEl& t) {
  if(ptr_cnt > 0) return cssCPtr::operator==(t);
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String)) {
    return GetEnumRef().NameVal() == t.GetStr();
  }
  return GetEnumRef().NumVal() == (Int)t;
}
bool cssCPtr_DynEnum::operator!=(cssEl& t) {
  if(ptr_cnt > 0) return cssCPtr::operator!=(t);
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String)) {
    return GetEnumRef().NameVal() != t.GetStr();
  }
  return GetEnumRef().NumVal() != (Int)t;
}

