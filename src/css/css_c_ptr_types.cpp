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
#include "ta_css.h"

#include "ta_matrix.h"
#include "ta_TA_type.h"

///////////////////
//     int       //
///////////////////
String cssCPtr_int::PrintStr() const {
  String rval = String(GetTypeName())+" "+ name+" --> ";
  if(GetVoidPtr() != NULL)
    rval += GetStr();
  else
    rval += "NULL";
  return rval;
}
void cssCPtr_int::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    *((int*)GetNonNullVoidPtr()) = (Int)t;
    if(class_parent != NULL)     class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_int::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) += (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) -= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) *= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) /= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) %= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator<<=(cssEl& t) {
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) <<= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator>>=(cssEl& t) {
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) >>= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) &= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) ^= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int*)GetNonNullVoidPtr()) |= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     short     //
///////////////////
void cssCPtr_short::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    *((short*)GetNonNullVoidPtr()) = (Int)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_short::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) += (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) -= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) *= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) /= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) %= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) <<= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) >>= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) &= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) ^= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  *((short*)GetNonNullVoidPtr()) |= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     bool      //
///////////////////

String cssCPtr_bool::GetStr() const {
  bool myb = *((bool*)GetNonNullVoidPtr());
  if(myb == true)
    return String("true");
  else if(myb == false)
    return String("false");
  else
    return String((int)myb);
}

void cssCPtr_bool::operator=(const String& cp) {
  bool* myb = (bool*)GetNonNullVoidPtr();
  if(cp == "false")
    *myb = false;
  else if(cp == "true")
    *myb = true;
  else
    *myb = (bool)(int)cp;
}

void cssCPtr_bool::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    if(t.GetType() == T_String)
      *this = t.GetStr();	// use string converter
    else
      *((bool*)GetNonNullVoidPtr()) = (Int)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}

///////////////////
//     long      //
///////////////////
void cssCPtr_long::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    *((long*)GetNonNullVoidPtr()) = (Int)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_long::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) += (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) -= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) *= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) /= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) %= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) <<= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) >>= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) &= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) ^= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  *((long*)GetNonNullVoidPtr()) |= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//  long_long    //
///////////////////
void cssCPtr_long_long::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    *((int64_t*)GetNonNullVoidPtr()) = (int64_t)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_long_long::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) += (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) -= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) *= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) /= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) %= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) <<= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) >>= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) &= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) ^= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  *((int64_t*)GetNonNullVoidPtr()) |= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     char      //
///////////////////
void cssCPtr_char::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    if(t.GetType() == T_String)
      *this = t.GetStr();	// use string converter
    else
      *((char*)GetNonNullVoidPtr()) = (Int)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_char::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) += (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) -= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) *= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) /= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) %= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) <<= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) >>= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) &= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) ^= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  *((char*)GetNonNullVoidPtr()) |= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     enum      //
///////////////////

MemberDef* cssCPtr_enum::GetEnumType() const {
  if((class_parent != NULL) && (class_parent->GetType() == T_TA)) {
    cssTA* clp = (cssTA*)class_parent;
    if(clp->type_def != NULL)
      return clp->type_def->members.FindName(name); // find me..
  }
  return NULL;
}

String cssCPtr_enum::GetStr() const {
  MemberDef* md = GetEnumType();
  if(md != NULL) {
    return md->type->GetValStr(GetNonNullVoidPtr(), NULL, md);
  }
  return String((Int)*this);
}

void cssCPtr_enum::operator=(const String& cp) {
  MemberDef* md = GetEnumType();
  if(md != NULL) {
    cssTA* clp = (cssTA*)class_parent;
    md->type->SetValStr(cp, GetNonNullVoidPtr(), clp->GetNonNullVoidPtr(), md);
    class_parent->UpdateAfterEdit();
    return;
  }
  *((int*)GetNonNullVoidPtr()) = (int)cp;
}
void cssCPtr_enum::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    if(t.GetType() == T_String)
      *this = t.GetStr();	// use string converter
    else
      *((int*)GetNonNullVoidPtr()) = (Int)t;
    if(class_parent != NULL)     class_parent->UpdateAfterEdit();
  }
}

///////////////////
//     double    //
///////////////////
String cssCPtr_double::PrintStr() const {
  String rval = String(GetTypeName())+" "+name+" --> ";
  if(GetVoidPtr() != NULL)
    rval += String(((Real)*this));
  else
    rval += "NULL";
  return rval;
}
void cssCPtr_double::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    *((double*)GetNonNullVoidPtr()) = (Real)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_double::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  *((double*)GetNonNullVoidPtr()) += (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_double::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  *((double*)GetNonNullVoidPtr()) -= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_double::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  *((double*)GetNonNullVoidPtr()) *= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_double::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  *((double*)GetNonNullVoidPtr()) /= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     float     //
///////////////////
void cssCPtr_float::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    *((float*)GetNonNullVoidPtr()) = (Real)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_float::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  *((float*)GetNonNullVoidPtr()) += (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_float::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  *((float*)GetNonNullVoidPtr()) -= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_float::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  *((float*)GetNonNullVoidPtr()) *= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_float::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  *((float*)GetNonNullVoidPtr()) /= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}


///////////////////
//     String    //
///////////////////
String cssCPtr_String::PrintStr() const {
  String rval = String(GetTypeName())+" "+name+" --> ";
  if(GetVoidPtr() != NULL)
    rval += *((String*)GetVoidPtr());
  else
    rval += "NULL";
  return rval;
}
void cssCPtr_String::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    *((String*)GetNonNullVoidPtr()) = t.GetStr();
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_String::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  *((String*)GetNonNullVoidPtr()) += t.GetStr();
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

cssEl* cssCPtr_String::operator[](int idx) const {
  void* pt = GetVoidPtr();	if(pt == NULL) return &cssMisc::Void;
  String* val = (String*)pt;
  String nw_val = val->elem(idx);
  return new cssString(nw_val);
}

int cssCPtr_String::GetMemberFunNo(const char* memb) const {
  int md;
  TA_taString.methods.FindName(memb, md);
  return md;
}
cssEl* cssCPtr_String::GetMemberFun(const char* memb) const {
  MethodDef* md = TA_taString.methods.FindName(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Member function not found:", memb, "in class of type: String");
    return &cssMisc::Void;
  }
  return GetMemberFun_impl(md);
}
cssEl* cssCPtr_String::GetMemberFun(int memb) const {
  MethodDef* md = TA_taString.methods.SafeEl(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Member function not found:", String(memb), "in class of type: String");
    return &cssMisc::Void;
  }
  return GetMemberFun_impl(md);
}
cssEl* cssCPtr_String::GetMemberFun_impl(MethodDef* md) const {
  void* pt = GetVoidPtr();	if(pt == NULL) return &cssMisc::Void;
  String* val = (String*)pt;
  if(md->stubp != NULL) {
    if(md->fun_argd >= 0)
      return new cssMbrCFun(VarArg, (void*)val, md->stubp, md->name);
    else
      return new cssMbrCFun(md->fun_argc, (void*)val, md->stubp, md->name);
  }
  else {
    cssMisc::Error(prog, "Function pointer not callable:", md->name, "of type:", md->type->name,
	      "in class of type: String");
    return &cssMisc::Void;
  }
}

cssEl* cssCPtr_String::GetScoped(const char* memb) const {
  EnumDef* ed = TA_taString.FindEnum(memb);
  if(ed != NULL) {
    return new cssInt(ed->enum_no);
  }

  MethodDef* md = TA_taString.methods.FindName(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Scoped element not found:", memb, "in class of type: String");
    return &cssMisc::Void;
  }

  return GetMemberFun_impl(md);
}


///////////////////
//     Variant    //
///////////////////

String cssCPtr_Variant::PrintStr() const {
  String rval = String(GetTypeName())+" "+name+" --> ";
  if(GetVoidPtr() != NULL)
    rval += (*((Variant*)GetVoidPtr())).toString();
  else
    rval += "NULL";
  return rval;
}
void cssCPtr_Variant::operator=(const cssEl& t) {
  if(t.GetType() == T_C_Ptr) PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    *((Variant*)GetNonNullVoidPtr()) = t.GetVar();
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_Variant::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  *((Variant*)GetNonNullVoidPtr()) += t.GetVar();
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

cssEl* cssCPtr_Variant::operator[](int idx) const {
  void* pt = GetVoidPtr();	
  if (pt == NULL) return &cssMisc::Void;
  Variant& val = *(Variant*)pt;
  return GetVariantEl_impl(val, idx);
}

int cssCPtr_Variant::GetMemberFunNo(const char* memb) const {
  int md;
  TA_Variant.methods.FindName(memb, md);
  return md;
}
cssEl* cssCPtr_Variant::GetMemberFun(const char* memb) const {
  MethodDef* md = TA_Variant.methods.FindName(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Member function not found:", memb, "in class of type: Variant");
    return &cssMisc::Void;
  }
  return GetMemberFun_impl(md);
}
cssEl* cssCPtr_Variant::GetMemberFun(int memb) const {
  MethodDef* md = TA_Variant.methods.SafeEl(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Member function not found:", String(memb), "in class of type: Variant");
    return &cssMisc::Void;
  }
  return GetMemberFun_impl(md);
}
cssEl* cssCPtr_Variant::GetMemberFun_impl(MethodDef* md) const {
  void* pt = GetVoidPtr();	if(pt == NULL) return &cssMisc::Void;
  Variant* val = (Variant*)pt;
  if(md->stubp != NULL) {
    if(md->fun_argd >= 0)
      return new cssMbrCFun(VarArg, (void*)val, md->stubp, md->name);
    else
      return new cssMbrCFun(md->fun_argc, (void*)val, md->stubp, md->name);
  }
  else {
    cssMisc::Error(prog, "Function pointer not callable:", md->name, "of type:", md->type->name,
	      "in class of type: Variant");
    return &cssMisc::Void;
  }
}

cssEl* cssCPtr_Variant::GetScoped(const char* memb) const {
  EnumDef* ed = TA_Variant.FindEnum(memb);
  if(ed != NULL) {
    return new cssInt(ed->enum_no);
  }

  MethodDef* md = TA_Variant.methods.FindName(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Scoped element not found:", memb, "in class of type: Variant");
    return &cssMisc::Void;
  }

  return GetMemberFun_impl(md);
}
