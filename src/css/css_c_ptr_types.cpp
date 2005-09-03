/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// css_ptr_i.cc

#include "css_c_ptr_types.h"
#include "ta_css.h"
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

String& cssCPtr_bool::GetStr() const {
  bool myb = *((bool*)GetNonNullVoidPtr());
  if(myb == true)
    ((cssEl*)this)->tmp_str = "true";
  else if(myb == false)
    ((cssEl*)this)->tmp_str = "false";
  else
    ((cssEl*)this)->tmp_str=String((int)myb);
  return (String&)tmp_str;
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

String& cssCPtr_enum::GetStr() const {
  MemberDef* md = GetEnumType();
  if(md != NULL) {
    ((cssEl*)this)->tmp_str = md->type->GetValStr(GetNonNullVoidPtr(), NULL, md);
    return (String&)tmp_str;
  }
  ((cssEl*)this)->tmp_str = String((Int)*this);
  return (String&)tmp_str;
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

