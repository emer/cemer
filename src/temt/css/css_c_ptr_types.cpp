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


// css_ptr_i.cc

#include "css_c_ptr_types.h"
#include "css_ta.h"

#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taMatrix>

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

cssEl* cssCPtr_int::operator+(cssEl& t) {
  int val = GetIntRef("+");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r += *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal((Real)t,""); r->val += (Real)val; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64((ta_int64_t)t,""); r->val += (ta_int64_t)val; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val += (Int)t; return r;
  }
  }
}

cssEl* cssCPtr_int::operator-(cssEl& t) {
  int val = GetIntRef("-");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r -= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal(-(Real)t,""); r->val += (Real)val; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64(-(ta_int64_t)t,""); r->val += (ta_int64_t)val; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val -= (Int)t; return r;
  }
  }
}

cssEl* cssCPtr_int::operator*(cssEl& t) {
  int val = GetIntRef("*");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r *= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal((Real)t,""); r->val *= (Real)val; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64((ta_int64_t)t,""); r->val *= (ta_int64_t)val; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val *= (Int)t; return r;
  }
  }
}

cssEl* cssCPtr_int::operator/(cssEl& t) {
  int val = GetIntRef("/");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r /= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal(0.0,""); r->val = (Real)val / (Real)t; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64(0,""); r->val = (ta_int64_t)val / (ta_int64_t)t; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val /= (Int)t; return r;
  }
  }
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
  bool& myb = GetBoolRef("=");
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

cssEl* cssCPtr_short::operator+(cssEl& t) {
  int val = GetShortRef("+");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r += *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal((Real)t,""); r->val += (Real)val; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64((ta_int64_t)t,""); r->val += (ta_int64_t)val; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val += (Int)t; return r;
  }
  }
}

cssEl* cssCPtr_short::operator-(cssEl& t) {
  int val = GetShortRef("-");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r -= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal(-(Real)t,""); r->val += (Real)val; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64(-(ta_int64_t)t,""); r->val += (ta_int64_t)val; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val -= (Int)t; return r;
  }
  }
}

cssEl* cssCPtr_short::operator*(cssEl& t) {
  int val = GetShortRef("*");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r *= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal((Real)t,""); r->val *= (Real)val; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64((ta_int64_t)t,""); r->val *= (ta_int64_t)val; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val *= (Int)t; return r;
  }
  }
}

cssEl* cssCPtr_short::operator/(cssEl& t) {
  int val = GetShortRef("/");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r /= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal(0.0,""); r->val = (Real)val / (Real)t; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64(0,""); r->val = (ta_int64_t)val / (ta_int64_t)t; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val /= (Int)t; return r;
  }
  }
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

cssEl* cssCPtr_long::operator+(cssEl& t) {
  int val = GetLongRef("+");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r += *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal((Real)t,""); r->val += (Real)val; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64((ta_int64_t)t,""); r->val += (ta_int64_t)val; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val += (Int)t; return r;
  }
  }
}

cssEl* cssCPtr_long::operator-(cssEl& t) {
  int val = GetLongRef("-");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r -= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal(-(Real)t,""); r->val += (Real)val; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64(-(ta_int64_t)t,""); r->val += (ta_int64_t)val; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val -= (Int)t; return r;
  }
  }
}

cssEl* cssCPtr_long::operator*(cssEl& t) {
  int val = GetLongRef("*");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r *= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal((Real)t,""); r->val *= (Real)val; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64((ta_int64_t)t,""); r->val *= (ta_int64_t)val; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val *= (Int)t; return r;
  }
  }
}

cssEl* cssCPtr_long::operator/(cssEl& t) {
  int val = GetLongRef("/");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r /= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal(0.0,""); r->val = (Real)val / (Real)t; return r;
  }
  case T_Int64: {
    cssInt64* r = new cssInt64(0,""); r->val = (ta_int64_t)val / (ta_int64_t)t; return r;
  }
  default: {
    cssInt *r = new cssInt(val); r->val /= (Int)t; return r;
  }
  }
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

cssEl* cssCPtr_long_long::operator+(cssEl& t) {
  int64_t val = GetLongLongRef("+");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r += *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal((Real)t,""); r->val += (Real)val; return r;
  }
  default: {
    cssInt64 *r = new cssInt64(val); r->val += (ta_int64_t)t; return r;
  }
  }
}

cssEl* cssCPtr_long_long::operator-(cssEl& t) {
  int64_t val = GetLongLongRef("-");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r -= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal(-(Real)t,""); r->val += (Real)val; return r;
  }
  default: {
    cssInt64 *r = new cssInt64(val); r->val -= (ta_int64_t)t; return r;
  }
  }
}

cssEl* cssCPtr_long_long::operator*(cssEl& t) {
  int64_t val = GetLongLongRef("*");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r *= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal((Real)t,""); r->val *= (Real)val; return r;
  }
  default: {
    cssInt64 *r = new cssInt64(val); r->val *= (ta_int64_t)t; return r;
  }
  }
}

cssEl* cssCPtr_long_long::operator/(cssEl& t) {
  int64_t val = GetLongLongRef("/");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r /= *mat;
      return new cssTA_Matrix(r);
    }
  }
  switch (t.GetPtrType()) {
  case T_Float:
  case T_Real: {
    cssReal* r = new cssReal(0.0,""); r->val = (Real)val / (Real)t; return r;
  }
  default: {
    cssInt64 *r = new cssInt64(val); r->val /= (ta_int64_t)t; return r;
  }
  }
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
    cssTA* clp = (cssTA*)class_parent->GetNonRefObj();
    TypeDef* nrtd = clp->GetNonRefTypeDef();
    if(nrtd) {
      MemberDef* md = nrtd->members.FindName(name); // find me..
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

cssEl* cssCPtr_enum::operator==(cssEl& t) {
  if(ptr_cnt > 0) return cssCPtr::operator==(t);
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String)) {
    return new cssBool(GetStr() == t.GetStr());
  }
  return new cssBool(GetIntRef("==") == (Int)t);
}
cssEl* cssCPtr_enum::operator!=(cssEl& t) {
  if(ptr_cnt > 0) return cssCPtr::operator!=(t);
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String)) {
    return new cssBool(GetStr() != t.GetStr());
  }
  return new cssBool(GetIntRef("!=") != (Int)t);
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

cssEl* cssCPtr_double::operator+(cssEl& t) {
  double val = GetDoubleRef("+");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r += *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(val,""); r->val += (Real)t; return r;
}

cssEl* cssCPtr_double::operator-(cssEl& t) {
  double val = GetDoubleRef("-");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r -= *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(val,""); r->val -= (Real)t; return r;
}

cssEl* cssCPtr_double::operator*(cssEl& t) {
  double val = GetDoubleRef("*");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r *= *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(val,""); r->val *= (Real)t; return r;
}

cssEl* cssCPtr_double::operator/(cssEl& t) {
  double val = GetDoubleRef("/");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r /= *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(val,""); r->val /= (Real)t; return r;
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

cssEl* cssCPtr_float::operator+(cssEl& t) {
  float val = GetFloatRef("+");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r += *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(val,""); r->val += (Real)t; return r;
}

cssEl* cssCPtr_float::operator-(cssEl& t) {
  float val = GetFloatRef("-");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r -= *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(val,""); r->val -= (Real)t; return r;
}

cssEl* cssCPtr_float::operator*(cssEl& t) {
  float val = GetFloatRef("*");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r *= *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(val,""); r->val *= (Real)t; return r;
}

cssEl* cssCPtr_float::operator/(cssEl& t) {
  float val = GetFloatRef("/");
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r /= *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(val,""); r->val /= (Real)t; return r;
}


///////////////////
//     String    //
///////////////////

String cssCPtr_String::null_string;


cssCPtr_String::~cssCPtr_String() {
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

cssEl* cssCPtr_String::operator[](const Variant& idx) const {
  if(ptr_cnt > 0) { NopErr("[]"); return &cssMisc::Void; }
  String& val = GetStringRef("[]");
  String nw_val = val[idx];	// use string code
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
  if(tabMisc::root) {
    MemberDef* md;
    void* rval = tabMisc::root->FindMembeR(GetStringRef(), md);
    if(rval)
      return (taBase*)rval;
  }
  return (taBase*)NULL;
}

cssCPtr_String::operator TypeDef*() const {
  if(ptr_cnt > 0) { CvtErr("taBase*"); return NULL; }
  String& nm = GetStringRef();
  TypeDef* td = taMisc::FindTypeName(nm);
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
  TA_MemberDef.GetPtrType()->SetValStr(nm, (void*)&md);
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
  TA_MethodDef.GetPtrType()->SetValStr(nm, (void*)&md);
  if(md == NULL) {
    cssMisc::Error(prog, "Could not find method def:", nm);
    return NULL;
  }
  return md;
}

int cssCPtr_String::GetMethodNo(const String& memb) const {
  return GetMethodNo_impl(&TA_taString, memb);
}

cssEl* cssCPtr_String::GetMethodFmName(const String& memb) const {
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

cssEl* cssCPtr_String::GetScoped(const String& memb) const {
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
  case Variant::T_TypeItem:	// todo: handle this case!
    break;
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

String& cssCPtr_Variant::PrintType(String& fh) const {
  TypeDef* typ = NULL;  void* base = NULL;
  if(GetVoidPtr()) {
    Variant& val_r = *((Variant*)GetVoidPtr());
    val_r.GetRepInfo(typ, base);
    fh << GetTypeName() << " (" << val_r.getTypeAsString() << ") " << name << ": ";
    if(val_r.type() == Variant::T_String) {
      typ->PrintType(fh);
    }
    else if(val_r.isBaseType()) {
      typ->GetNonPtrType()->PrintType(fh);
    }
    else {
      TA_Variant.PrintType(fh);
    }
  }
  else {
    fh << GetTypeName() << " " << name << ": ";
    TA_Variant.PrintType(fh);
  }
  return fh;
}

String& cssCPtr_Variant::PrintInherit(String& fh) const {
  TypeDef* typ = NULL;  void* base = NULL;
  if(GetVoidPtr()) {
    Variant& val_r = *((Variant*)GetVoidPtr());
    val_r.GetRepInfo(typ, base);
    if(val_r.type() == Variant::T_String) {
      typ->PrintInherit(fh);
    }
    else if(val_r.isBaseType()) {
      typ->GetNonPtrType()->PrintInherit(fh);
    }
    else {
      TA_Variant.PrintInherit(fh);
    }
  }
  else {
    TA_Variant.PrintInherit(fh);
  }
  return fh;
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

cssEl* cssCPtr_Variant::operator[](const Variant& idx) const {
  if(ptr_cnt > 0) { NopErr("[]"); return &cssMisc::Void; }
  Variant& val = GetVarRef("[]");
  return VarElem(val, idx);
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

cssEl* cssCPtr_Variant::GetMemberFmName(const String& memb) const {
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

cssEl* cssCPtr_Variant::GetMethodFmName(const String& memb) const {
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

cssEl* cssCPtr_Variant::GetScoped(const String& memb) const {
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

String& cssCPtr_DynEnum::PrintType(String& fh) const {
  fh << GetTypeName() << name << ": ";
  if(GetVoidPtr() == NULL) {
    fh << "NULL";
  }
  else {
    fh << "\n";
    DynEnum& enm = GetEnumRef();
    if(enm.enum_type)
      enm.enum_type->PrintType(fh);
  }
  return fh;
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
cssEl* cssCPtr_DynEnum::operator==(cssEl& t) {
  if(ptr_cnt > 0) return cssCPtr::operator==(t);
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String)) {
    return new cssBool(GetEnumRef().NameVal() == t.GetStr());
  }
  return new cssBool(GetEnumRef().NumVal() == (Int)t);
}
cssEl* cssCPtr_DynEnum::operator!=(cssEl& t) {
  if(ptr_cnt > 0) return cssCPtr::operator!=(t);
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String)) {
    return new cssBool(GetEnumRef().NameVal() != t.GetStr());
  }
  return new cssBool(GetEnumRef().NumVal() != (Int)t);
}

void cssCPtr_DynEnum::operator+=(cssEl& t)	{
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef("+=");
  de.SetNumVal(de.NumVal() + (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_DynEnum::operator-=(cssEl& t)	{
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef("-=");
  de.SetNumVal(de.NumVal() - (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_DynEnum::operator*=(cssEl& t)	{
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef("*=");
  de.SetNumVal(de.NumVal() * (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_DynEnum::operator/=(cssEl& t)	{
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef("/=");
  de.SetNumVal(de.NumVal() / (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_DynEnum::operator%=(cssEl& t)	{
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef("%=");
  de.SetNumVal(de.NumVal() % (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_DynEnum::operator<<=(cssEl& t) {
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef("<<=");
  de.SetNumVal(de.NumVal() << (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_DynEnum::operator>>=(cssEl& t) {
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef(">>=");
  de.SetNumVal(de.NumVal() >> (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_DynEnum::operator&=(cssEl& t)	{
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef("&=");
  de.SetNumVal(de.NumVal() & (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_DynEnum::operator^=(cssEl& t)	{
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef("^=");
  de.SetNumVal(de.NumVal() ^ (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}
void cssCPtr_DynEnum::operator|=(cssEl& t)	{
  if(!ROCheck()) return;
  DynEnum& de = GetEnumRef("|=");
  de.SetNumVal(de.NumVal() | (Int)t);
  if(class_parent)	class_parent->UpdateAfterEdit();
}

int cssCPtr_DynEnum::GetMethodNo(const String& memb) const {
  return GetMethodNo_impl(&TA_DynEnum, memb);
}

cssEl* cssCPtr_DynEnum::GetMethodFmName(const String& memb) const {
  void* sp = GetNonNullVoidPtr("->()");
  if(!sp) return &cssMisc::Void;
  DynEnum& val = *((DynEnum*)sp);
  return GetMethodFmName_impl(&TA_DynEnum, (void*)&val, memb);
}

cssEl* cssCPtr_DynEnum::GetMethodFmNo(int memb) const {
  void* sp = GetNonNullVoidPtr("->()");
  if(!sp) return &cssMisc::Void;
  DynEnum& val = *((DynEnum*)sp);
  return GetMethodFmNo_impl(&TA_DynEnum, (void*)&val, memb);
}

cssEl* cssCPtr_DynEnum::GetScoped(const String& memb) const {
  void* sp = GetNonNullVoidPtr("::");
  if(!sp) return &cssMisc::Void;
  DynEnum& val = *((DynEnum*)sp);
  return GetScoped_impl(&TA_DynEnum, (void*)&val, memb);
}

