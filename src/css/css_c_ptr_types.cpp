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

int cssCPtr_int::null_int = 0;

void cssCPtr_int::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_Int))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    GetIntRef() = (Int)t;
    if(class_parent != NULL)     class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_int::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef() += (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef() -= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef() *= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef() /= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef() %= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator<<=(cssEl& t) {
  if(!ROCheck()) return;
  GetIntRef() <<= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator>>=(cssEl& t) {
  if(!ROCheck()) return;
  GetIntRef() >>= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef() &= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef() ^= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_int::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetIntRef() |= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     short     //
///////////////////

void cssCPtr_short::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_Short))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    GetShortRef() = (Int)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_short::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() += (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() -= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() *= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() /= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() %= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() <<= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() >>= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() &= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() ^= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_short::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetShortRef() |= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     bool      //
///////////////////

String cssCPtr_bool::GetStr() const {
  bool myb = GetBoolRef();
  if(myb == true)
    return String("true");
  else if(myb == false)
    return String("false");
  else
    return String((int)myb);
}

void cssCPtr_bool::operator=(const String& cp) {
  bool myb = GetBoolRef();
  if(cp == "false")
    myb = false;
  else if(cp == "true")
    myb = true;
  else
    myb = (bool)(int)cp;
}

void cssCPtr_bool::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_Bool))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    if((t.GetType() == T_String) || (t.GetPtrType() == T_String))
      *this = t.GetStr();	// use string converter
    else
      GetBoolRef() = (Int)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}

///////////////////
//     long      //
///////////////////

long cssCPtr_long::null_long = 0;

void cssCPtr_long::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_Long))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    GetLongRef() = (Int)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_long::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() += (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() -= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() *= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() /= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() %= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() <<= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() >>= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() &= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() ^= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongRef() |= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//  long_long    //
///////////////////

int64_t cssCPtr_long_long::null_long_long = 0;

void cssCPtr_long_long::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_LongLong)) 
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    GetLongLongRef() = (int64_t)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_long_long::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() += (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() -= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() *= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() /= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() %= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() <<= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() >>= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() &= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() ^= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_long_long::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetLongLongRef() |= (int64_t)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     char      //
///////////////////

void cssCPtr_char::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_Char))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    if((t.GetType() == T_String) || (t.GetPtrType() == T_String))
      *this = t.GetStr();	// use string converter
    else
      GetCharRef() = (Int)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_char::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() += (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() -= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() *= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() /= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() %= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator<<=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() <<= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator>>=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() >>= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() &= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() ^= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_char::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  GetCharRef() |= (Int)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     enum      //
///////////////////

TypeDef* cssCPtr_enum::GetEnumType() const {
  if(enum_type != NULL) return enum_type;
  if((class_parent != NULL) && (class_parent->GetType() == T_TA)) {
    cssTA* clp = (cssTA*)class_parent;
    if(clp->type_def != NULL) {
      MemberDef* md = clp->type_def->members.FindName(name); // find me..
      if(md != NULL)
	return md->type;
    }
  }
  return NULL;
}

String cssCPtr_enum::GetStr() const {
  TypeDef* et = GetEnumType();
  if(et != NULL) {
    return et->GetValStr(GetNonNullVoidPtr(), NULL);
  }
  return String((Int)*this);
}

void cssCPtr_enum::operator=(const String& cp) {
  TypeDef* et = GetEnumType();
  if(et != NULL) {
    cssTA* clp = (cssTA*)class_parent;
    et->SetValStr(cp, GetNonNullVoidPtr(), clp->GetNonNullVoidPtr());
    class_parent->UpdateAfterEdit();
    return;
  }
  GetIntRef() = (int)cp;
}
void cssCPtr_enum::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_Enum))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    if((t.GetType() == T_String) || (t.GetPtrType() == T_String))
      *this = t.GetStr();	// use string converter
    else
      GetIntRef() = (Int)t;
    if(class_parent != NULL)     class_parent->UpdateAfterEdit();
  }
}

///////////////////
//     double    //
///////////////////

double cssCPtr_double::null_double = 0.0;

void cssCPtr_double::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_Real))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    GetDoubleRef() = (Real)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_double::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetDoubleRef() += (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_double::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetDoubleRef() -= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_double::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetDoubleRef() *= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_double::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetDoubleRef() /= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

///////////////////
//     float     //
///////////////////

void cssCPtr_float::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_Float))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    GetFloatRef() = (Real)t;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_float::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetFloatRef() += (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_float::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  GetFloatRef() -= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_float::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  GetFloatRef() *= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}
void cssCPtr_float::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  GetFloatRef() /= (Real)t;
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}


///////////////////
//     String    //
///////////////////

String cssCPtr_String::null_string;

void cssCPtr_String::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_String))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    GetStringRef() = t.GetStr();
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_String::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetStringRef() += t.GetStr();
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

cssEl* cssCPtr_String::operator[](int idx) const {
  void* pt = GetVoidPtr();	if(pt == NULL) return &cssMisc::Void;
  String* val = (String*)pt;
  String nw_val = val->elem(idx);
  return new cssString(nw_val);
}

cssCPtr_String::operator bool() const {
  String& val = GetStringRef();
  if(val == "true")
    return true;
  else if(val == "false")
    return false;
  else
    return (Int)val;
}

int cssCPtr_String::GetMethodNo(const char* memb) const {
  String& val = GetStringRef();
  return GetMethodNo_impl(&TA_taString, memb);
}

cssEl* cssCPtr_String::GetMethodFmName(const char* memb) const {
  String& val = GetStringRef();
  return GetMethodFmName_impl(&TA_taString, (void*)&val, memb);
}

cssEl* cssCPtr_String::GetMethodFmNo(int memb) const {
  String& val = GetStringRef();
  return GetMethodFmNo_impl(&TA_taString, (void*)&val, memb);
}

cssEl* cssCPtr_String::GetScoped(const char* memb) const {
  String& val = GetStringRef();
  return GetScoped_impl(&TA_taString, (void*)&val, memb);
}


///////////////////
//     Variant    //
///////////////////

Variant cssCPtr_Variant::null_var;

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
  if(GetVoidPtr() != NULL) {
    Variant& vl = *((Variant*)GetVoidPtr());
    rval += "(" + vl.getTypeAsString() + ") " + vl.toString();
  }
  else
    rval += "NULL";
  return rval;
}

void cssCPtr_Variant::TypeInfo(ostream& fh) const {
  TypeDef* typ = NULL;  void* base = NULL;
  Variant& val_r = GetVarRef();
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

void cssCPtr_Variant::InheritInfo(ostream& fh) const {
  TypeDef* typ = NULL;  void* base = NULL;
  Variant& val_r = GetVarRef();
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

void cssCPtr_Variant::operator=(const cssEl& t) {
  if((t.GetType() == T_C_Ptr) && (t.GetPtrType() == T_Variant))
    PtrAssignPtr((cssCPtr*)&t);
  else {
    if(!ROCheck()) return;
    GetVarRef() = t.GetVar();
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
  }
}
void cssCPtr_Variant::operator=(const Variant& val) {
  GetVarRef() = val;
}

void cssCPtr_Variant::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  GetVarRef() += t.GetVar();
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

cssEl* cssCPtr_Variant::operator[](int idx) const {
  void* pt = GetVoidPtr();	
  if (pt == NULL) return &cssMisc::Void;
  Variant& val = *(Variant*)pt;
  return GetVariantEl_impl(val, idx);
}

cssEl* cssCPtr_Variant::GetMemberFmNo(int memb) const {
  Variant& val = GetVarRef();
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
  Variant& val = GetVarRef();
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
  Variant& val = GetVarRef();
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
  Variant& val = GetVarRef();
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
  Variant& val = GetVarRef();
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

