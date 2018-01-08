// Copyright 2006-2017, Regents of the University of Colorado,
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


// basic_types.cc

#include "css_basic_types.h"
#include "css_ta.h"

#ifdef TA_GUI
#include "css_qtdialog.h"
#endif

//#include "ta_TA_type.h"
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <int_Matrix>

#include <QPointer>

using namespace std;


//////////////////////////
//      cssInt          //
//////////////////////////

cssEl* cssInt::operator+(cssEl& t) {
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
    cssInt* r = new cssInt(*this,""); r->val += (Int)t; return r;
  }
  }
}

cssEl* cssInt::operator-(cssEl& t) {
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
    cssInt* r = new cssInt(*this,""); r->val -= (Int)t; return r;
  }
  }
}

cssEl* cssInt::operator*(cssEl& t) {
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
    cssInt* r = new cssInt(*this,""); r->val *= (Int)t; return r;
  }
  }
}

cssEl* cssInt::operator/(cssEl& t) {
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
    cssInt* r = new cssInt(*this,""); r->val /= (Int)t; return r;
  }
  }
}

//////////////////////////
// cssInt64             //
//////////////////////////

cssEl* cssInt64::operator+(cssEl& t) {
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
    cssInt64* r = new cssInt64(*this,""); r->val += (ta_int64_t)t; return r;
  }
  }
}

cssEl* cssInt64::operator-(cssEl& t) {
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
    cssInt64* r = new cssInt64(*this,""); r->val -= (ta_int64_t)t; return r;
  }
  }
}

cssEl* cssInt64::operator*(cssEl& t) {
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
    cssInt64* r = new cssInt64(*this,""); r->val *= (ta_int64_t)t; return r;
  }
  }
}

cssEl* cssInt64::operator/(cssEl& t) {
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
    cssInt64* r = new cssInt64(*this,""); r->val /= (ta_int64_t)t; return r;
  }
  }
}


//////////////////////////
// cssChar              //
//////////////////////////

void cssChar::operator=(const cssEl& t) {
  if((t.GetType() == T_String) || (t.GetPtrType() == T_String)) {
    *this = t.GetStr();         // use string converter
  }
  else {
    val = (Int)t;
  }
}

//////////////////////////
// cssReal             //
//////////////////////////

cssEl* cssReal::operator+(cssEl& t) {
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r += *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(*this,""); r->val += (Real)t; return r;
}

cssEl* cssReal::operator-(cssEl& t) {
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r -= *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(*this,""); r->val -= (Real)t; return r;
}

cssEl* cssReal::operator*(cssEl& t) {
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r *= *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(*this,""); r->val *= (Real)t; return r;
}

cssEl* cssReal::operator/(cssEl& t) {
  if(t.IsTaMatrix()) {
    taMatrix* mat = cssTA_Matrix::MatrixPtr(t);
    if(mat && !mat->IsSingleElemView()) {
      taMatrix* r = (taMatrix*)mat->Clone();
      r->InitValsFmVar(val);
      *r /= *mat;
      return new cssTA_Matrix(r);
    }
  }
  cssReal* r = new cssReal(*this,""); r->val /= (Real)t; return r;
}

//////////////////////////
//      cssString       //
//////////////////////////

String& cssString::PrintType(String& fh) const {
  return TA_taString.PrintType(fh);
}

String& cssString::PrintInherit(String& fh) const {
  return TA_taString.PrintInherit(fh);
}

cssEl* cssString::operator[](const Variant& idx) const {
  String nw_val = val[idx];     // use string code
  return new cssString(nw_val);
}

cssString::operator bool() const {
  if(val == "true")
    return true;
  else if(val == "false")
    return false;
  else
    return (Int)*this;
}

int cssString::GetMethodNo(const String& memb) const {
  return GetMethodNo_impl(&TA_taString, memb);
}

cssEl* cssString::GetMethodFmNo(int memb) const {
  return GetMethodFmNo_impl(&TA_taString, (void*)&val, memb);
}

cssEl* cssString::GetMethodFmName(const String& memb) const {
  return GetMethodFmName_impl(&TA_taString, (void*)&val, memb);
}

cssEl* cssString::GetScoped(const String& memb) const {
  return GetScoped_impl(&TA_taString, (void*)&val, memb);
}

cssString::operator taBase*() const {
  if(tabMisc::root) {
    MemberDef* md;
    void* rval = tabMisc::root->FindMembeR(val, md);
    if(rval)
      return (taBase*)rval;
  }
  return (taBase*)NULL;
}

cssString::operator TypeDef*() const {
  TypeDef* td = taMisc::FindTypeName(val, false); // no err
  if(td == NULL) {
    cssMisc::Error(prog, "Could not find type:", val); // this err..
    return NULL;
  }
  return td;
}

cssString::operator MemberDef*() const {
  MemberDef* md;
  TA_MemberDef.GetPtrType()->SetValStr(val, (void*)&md);
  if(md == NULL) {
    cssMisc::Error(prog, "Could not find member def:", val);
    return NULL;
  }
  return md;
}

cssString::operator MethodDef*() const {
  MethodDef* md;
  TA_MethodDef.GetPtrType()->SetValStr(val, (void*)&md);
  if(md == NULL) {
    cssMisc::Error(prog, "Could not find method def:", val);
    return NULL;
  }
  return md;
}

void cssString::Save(ostream& strm) {
  String str = GetStr();
  taMisc::write_quoted_string(strm, str, true); // always write the quotes
}

void cssString::Load(istream& strm) {
  int c = taMisc::skip_till_start_quote_or_semi(strm);
  if (c == '\"')                          // "
    c = taMisc::read_till_end_quote_semi_unesc(strm);// then till second followed by semi

  if(c != ';') {
    taMisc::Error("*** Missing ';' in dump file for string:", name);
    return;
  }
  *this = taMisc::LexBuf;       // set via string assgn
}


//////////////////////////
//      cssBool         //
//////////////////////////

String cssBool::GetStr() const {
  if(val == true)
    return String("true");
  else if(val == false)
    return String("false");
  else
    return String((int)val);
}

void cssBool::operator=(const cssEl& s) {
  if((s.GetType() == T_String) || (s.GetPtrType() == T_String))
    *this = s.GetStr();         // use string converter
  else
    val = (bool)s;
}

void cssBool::operator=(const String& cp) {
  if(cp == "true")
    val = true;
  else if(cp == "false")
    val = false;
  else
    val = (bool)strtol((const char*)cp, NULL, 0);
}


/////////////////////////
//  cssVariant         //
/////////////////////////

cssEl::cssTypes cssVariant::GetPtrType() const {
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
    case Variant::T_Float:
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
  case Variant::T_TypeItem:
    return T_TA;
  }
  return T_Variant;
}

cssVariant::operator taBase*() const {
  if (val.isBaseType()) {
    return val.toBase();
  }
  else {
    return inherited::operator taBase*();
  }
}

cssVariant::operator TypeDef*() const {
  if (TypeDef *rval = val.toTypeDef()) {
    return rval;
  }
  else {
    return inherited::operator TypeDef*();
  }
}

cssVariant::operator MemberDef*() const {
  if (MemberDef *rval = val.toMemberDef()) {
    return rval;
  }
  else {
    return inherited::operator MemberDef*();
  }
}

cssVariant::operator MethodDef*() const {
  if (MethodDef *rval = val.toMethodDef()) {
    return rval;
  }
  else {
    return inherited::operator MethodDef*();
  }
}

String cssVariant::PrintStr() const {
  return String(GetTypeName()) + " " + name + " = ("
    + val.getTypeAsString() + ") " + val.toString();
}

String& cssVariant::PrintType(String& fh) const {
  TypeDef* typ = NULL;  void* base = NULL;
  Variant& val_r = (Variant&)val;
  val_r.GetRepInfo(typ, base);
  fh << GetTypeName() << " (" << val_r.getTypeAsString() << ") " << name << ": ";
  if(val_r.type() == Variant::T_String) {
    typ->PrintType(fh);
  }
  else if(val_r.isBaseType() || val_r.isTypeItem()) {
    typ->PrintType(fh);
  }
  else {
    TA_Variant.PrintType(fh);
  }
  return fh;
}

String& cssVariant::PrintInherit(String& fh) const {
  TypeDef* typ = NULL;  void* base = NULL;
  Variant& val_r = (Variant&)val;
  val_r.GetRepInfo(typ, base);
  if(val_r.type() == Variant::T_String) {
    typ->PrintInherit(fh);
  }
  else if(val_r.isBaseType() || val_r.isTypeItem()) {
    typ->PrintInherit(fh);
  }
  else {
    TA_Variant.PrintInherit(fh);
  }
  return fh;
}

cssEl* cssVariant::operator+(cssEl& t) {
// string concatenation takes precedence over numeric addition
 if (val.isStringType()) {
   cssVariant* r = new cssVariant(*this,"");
   r->val += t.GetStr();
   return r;
 } else {
   cssVariant* r = new cssVariant(*this,"");
   r->val += t.GetVar();
   return r;
 }
}

cssEl* cssVariant::operator-(cssEl& t) {
  cssVariant* r = new cssVariant(*this,"");
  r->val -= t.GetVar();
  return r;
}

cssEl* cssVariant::operator*() {
  return cssEl::operator*();
}

cssEl* cssVariant::operator*(cssEl& t) {
  cssVariant* r = new cssVariant(*this,"");
  r->val *= t.GetVar();
  return r;
}

cssEl* cssVariant::operator/(cssEl& t) {
  cssVariant* r = new cssVariant(*this,"");
  r->val /= t.GetVar();
  return r;
}

// implement the to-the-power of cssVariant::operator as ^
cssEl* cssVariant::operator^(cssEl& t) {
  if (val.isNumeric()) {
    cssVariant* r = new cssVariant();
    r->val = pow(val.toDouble(), (Real)t);
    return r;
  } else return inherited::operator^(t);
}

cssEl* cssVariant::operator-() {
  cssVariant* r = new cssVariant(-val,"");
  return r;
}

cssEl* cssVariant::operator~() {
  cssVariant* r = new cssVariant(~val,"");
  return r;
}

//note: TypeItem guys don't get css info, so we don't handle those as Variants below...
cssEl* cssVariant::GetMemberFmNo(int memb) const {
  TypeDef* typ = NULL;  void* base = NULL;
  Variant& val_r = (Variant&)val;
  val_r.GetRepInfo(typ, base);
  if(val_r.type() == Variant::T_String) {
    return GetMemberFmNo_impl(typ, (String*)&val_r, memb);
  }
  else if(val_r.isBaseType()) {
    return GetMemberFmNo_impl(typ, val_r.toBase(), memb);
  }
  return GetMemberFmNo_impl(&TA_Variant, &val_r, memb);
}

cssEl* cssVariant::GetMemberFmName(const String& memb) const {
  TypeDef* typ = NULL;  void* base = NULL;
  Variant& val_r = (Variant&)val;
  val_r.GetRepInfo(typ, base);
  if(val_r.type() == Variant::T_String) {
    return GetMemberFmName_impl(typ, (String*)&val_r, memb);
  }
  else if(val_r.isBaseType() ) {
    return GetMemberFmName_impl(typ, val_r.toBase(), memb);
  }
  return GetMemberFmName_impl(&TA_Variant, &val_r, memb);
}

cssEl* cssVariant::GetMethodFmNo(int memb) const {
  TypeDef* typ = NULL;  void* base = NULL;
  Variant& val_r = (Variant&)val;
  val_r.GetRepInfo(typ, base);
  if(val_r.type() == Variant::T_String) {
    return GetMethodFmNo_impl(typ, (String*)&val_r, memb);
  }
  else if(val_r.isBaseType()) {
    return GetMethodFmNo_impl(typ, val_r.toBase(), memb);
  }
  return GetMethodFmNo_impl(&TA_Variant, &val_r, memb);
}

cssEl* cssVariant::GetMethodFmName(const String& memb) const {
  TypeDef* typ = NULL;  void* base = NULL;
  Variant& val_r = (Variant&)val;
  val_r.GetRepInfo(typ, base);
  if(val_r.type() == Variant::T_String) {
    return GetMethodFmName_impl(typ, (String*)&val_r, memb);
  }
  else if(val_r.isBaseType()) {
    return GetMethodFmName_impl(typ, val_r.toBase(), memb);
  }
  return GetMethodFmName_impl(&TA_Variant, &val_r, memb);
}

cssEl* cssVariant::GetScoped(const String& memb) const {
  TypeDef* typ = NULL;  void* base = NULL;
  Variant& val_r = (Variant&)val;
  val_r.GetRepInfo(typ, base);
  if(val_r.type() == Variant::T_String) {
    return GetScoped_impl(typ, (String*)&val_r, memb);
  }
  else if(val_r.isBaseType()) {
    return GetScoped_impl(typ, val_r.toBase(), memb);
  }
  return GetScoped_impl(&TA_Variant, &val_r, memb);
}

cssEl* cssVariant::operator[](const Variant& idx) const {
  return VarElem(val, idx);
}


//////////////////////////////////
//      cssPtr: Pointer Type    //
//////////////////////////////////


cssPtr::cssPtr() {
  Constr();
}
cssPtr::cssPtr(const cssElPtr& it) {
  Constr(); SetPtr(it);  SetElType(it.El()->GetTypeObject());
}
cssPtr::cssPtr(cssEl* typ) {
  Constr(); SetElType(typ);
}
cssPtr::cssPtr(cssEl* typ, const String& nm) {
  Constr(); name = nm; SetElType(typ);
}
cssPtr::cssPtr(cssEl* typ, const cssElPtr& it, const String& nm) {
  Constr(); name = nm; SetElType(typ); SetPtr(it);
}
cssPtr::cssPtr(const cssPtr& cp) {
  Constr(); Copy(cp); name = cp.name; // needs constr prior to copy to init typ info
}
cssPtr::cssPtr(const cssPtr& cp, const String& nm) {
  Constr(); Copy(cp); name = nm;
}
cssPtr::~cssPtr() {
  cssEl::DelRefElPtr(ptr);
  if(el_type != &cssMisc::Void) cssEl::unRefDone(el_type);
  el_type = &cssMisc::Void;
}

String cssPtr::PrintStr() const {
  String tmp;
  if((ptr.El()->GetType() == T_Class) || (ptr.El()->GetType() == T_ClassType))
    tmp = String(ptr.El()->GetTypeName())+" "+ptr.El()->name+" { }";
  else
    tmp = (ptr.El())->PrintStr();
  return String(GetTypeName()) + "* " + name + " --> " + tmp;
}
String cssPtr::GetStr() const {
  return String("--> ") + (ptr.El())->name + " = " + (ptr.El())->GetStr();
}

cssEl* cssPtr::GetNonPtrTypeObj() const {
  if((el_type->GetType() == T_Ptr) || (el_type->GetType() == T_Array) ||
     (el_type->GetType() == T_ArrayType))
    return ((cssPtr*)el_type->GetNonRefObj())->GetNonPtrTypeObj();
  return el_type;
}

void cssPtr::operator=(const cssEl& s) {
  if((s.GetType() == T_Ptr) || (s.GetType() == T_Array)) {
    cssPtr* tmp = (cssPtr*)s.GetNonRefObj();
    cssEl* s_type = tmp->GetNonPtrTypeObj();
    cssEl* nptr_type = GetNonPtrTypeObj();

    // if it's a void pointer, use type of what it points to
    // if it's a class inst, get its class type
    if(s_type->GetType() == T_Void)
      s_type = tmp->ptr.El();
    if(s_type->GetType() == T_Class)
      s_type = ((cssClassInst*)s_type->GetNonRefObj())->type_def;

    // if either one's a Void, then assignment is automatically ok
    bool auto_ok = false;
    if((nptr_type->GetType() == T_Void) || (s_type->GetType() == T_Void))
      auto_ok = true;
    if(!auto_ok) {
      if(nptr_type->GetType() != s_type->GetType()) {
        cssMisc::Error(prog, "Pointer type mismatch between:",nptr_type->GetTypeName(),"and",
                       s_type->GetTypeName());
        return;
      }
      // if we have class types, check for inheritance
      // allow both up and down casting
      else if(nptr_type->GetType() == T_ClassType) {
        cssClassType* ths_class = (cssClassType*) nptr_type->GetNonRefObj();
        cssClassType* s_class = (cssClassType*) s_type->GetNonRefObj();
        if(!ths_class->InheritsFrom(s_class) &&
           !s_class->InheritsFrom(ths_class)) {
          cssMisc::Error(prog, "Pointer type mismatch between:",nptr_type->GetTypeName(),"and",
                         s_type->GetTypeName());
          return;
        }
      }
    }

    // assignment is now type-correct if you get here
    // if its an array, we refer to it directly, not to its pointer
    if(s.GetType() == T_Array)
      SetPtr(tmp->GetAddr());
    else
      SetPtr(tmp->ptr);
  }
  else {
    int sval = (Int)s;
    if(sval == 0)
      SetPtr(cssMisc::VoidElPtr);
    else
      cssMisc::Error(prog, "Assigning pointer to non-ptr value");
  }
}

cssElPtr& cssPtr::GetOprPtr() const {
  if(ptr.El()->GetType() == T_Array)
    return ((cssArray*)ptr.El()->GetNonRefObj())->ptr;
  else
    return (cssElPtr&)ptr;
}

int cssPtr::GetMemberNo(const String& memb) const {
  // even if ptr is null, check type
  if(ptr.El() == &cssMisc::Void) {
    if(el_type->GetType() == T_ClassType)
      return el_type->GetMemberNo(memb);
    return -1;
  }
  return ptr.El()->GetMemberNo(memb);
}

int cssPtr::GetMethodNo(const String& memb) const {
  // even if ptr is null, check type
  if(ptr.El() == &cssMisc::Void) {
    if(el_type->GetType() == T_ClassType)
      return el_type->GetMethodNo(memb);
    return -1;
  }
  return ptr.El()->GetMethodNo(memb);
}

cssEl* cssPtr::GetMethodFmName(const String& memb) const {
  if(el_type->GetType() == T_ClassType) {
    cssEl* rval = el_type->GetMethodFmName(memb);
    if(rval->GetType() == T_MbrScriptFun) {
      cssMbrScriptFun* mbrf = (cssMbrScriptFun*)rval;
      if(mbrf->is_virtual)
        return ptr.El()->GetMethodFmName(memb); // get from the pointer itself
      else
        return rval;            // get from the declared class of the ptr
    }
  }
  return ptr.El()->GetMethodFmName(memb);
}

cssEl* cssPtr::GetMethodFmNo(int memb) const {
  if(el_type->GetType() == T_ClassType) {
    cssEl* rval = el_type->GetMethodFmNo(memb);
    if(rval->GetType() == T_MbrScriptFun) {
      cssMbrScriptFun* mbrf = (cssMbrScriptFun*)rval;
      if(mbrf->is_virtual)
        return ptr.El()->GetMethodFmNo(memb); // get from the pointer itself
      else
        return rval;            // get from the declared class of the ptr
    }
  }
  return ptr.El()->GetMethodFmNo(memb);
}

cssEl* cssPtr::operator[](const Variant& i) const {
  cssElPtr tmp = GetOprPtr();
  if(i.isNumeric()) {
    tmp += i.toInt();
  }
  else if(i.isMatrixType()) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(i.toMatrix());
    if(!cmat) {
      cssMisc::Error(prog, "cssPtr::operator[]",
                     "index matrix is NULL or not an int_Matrix");
      return &cssMisc::Void;
    }
    if(cmat->size <= 0) return &cssMisc::Void;
    tmp += cmat->FastEl_Flat(0); // just get first element -- all that is supported
  }
  return tmp.El();
}

//////////////////////////
//      Array Type      //
//////////////////////////

void cssArray::Constr() {
  items = NULL;
}

void cssArray::Constr(int no) {
  items = new cssSpace(no, "");
  // we don't actually have any items yet
  ptr.SetSpace(items);
}

void cssArray::Copy(const cssArray& cp) {
  Constr(cp.items->alloc_size);
  cssEl::Copy(cp);
  SetElType(cp.el_type);
  el_type->prog = prog;
  items->Copy(*(cp.items));
  if (ptr.El() != &cssMisc::Void) cssEl::unRefDone(ptr.El());
  if (items->size > 0) {
    ptr.SetSpace(items, 0);
    cssEl::Ref(ptr.El());
  }
  else {
    ptr.SetSpace(items);
  }
}

cssArray::cssArray() {
  Constr();
}
cssArray::cssArray(int no) {
  Constr(); Constr(no);
}
cssArray::cssArray(int no, cssEl* it) {
  Constr(); Constr(no);
  SetElType(it);
  prog = el_type->prog;
  Fill(el_type);
}
cssArray::cssArray(int no, const String& nm) {
  Constr(); Constr(no); name = nm;
}
cssArray::cssArray(int no, cssEl* it, const String& nm) {
  Constr(); Constr(no); name = nm;
  SetElType(it);
  prog = el_type->prog;
  Fill(el_type);
}
cssArray::cssArray(const cssArray& cp) {
  Copy(cp); name = cp.name;
}
cssArray::cssArray(const cssArray& cp, const String& nm) {
  Copy(cp);
  name = nm;
}
cssArray::cssArray(const cssArrayType& cp, const String& nm) {
  Constr();
  prog = cp.prog;
  name = nm;
  Constr(cp.size);
  SetElType(cp.el_type);
  el_type->prog = prog;
  Fill(el_type);
}
cssArray::~cssArray() {
  if(ptr.NotNull())
    cssEl::unRefDone(ptr.El());
  ptr.Reset();
  if(items != NULL)
    delete items;
  items = NULL;
}

cssEl::RunStat cssArray::Do(cssProg* prg) {
  prog = prg;
  el_type->prog = prog;
  prog->Stack()->Push(this);
  return cssEl::Running;
}

String cssArray::GetStr() const {
  return PrintFStr();
}

int cssArray::Alloc(int no) {
  if (ptr.El() != &cssMisc::Void) cssEl::unRefDone(ptr.El());
  if(items != NULL)
    delete items;
  Constr(no);
  Fill(el_type);
  return true;
}

int cssArray::AllocAll(int na, cssEl* arg[]) {
  int n_alloc = (int)*arg[0];
  Alloc(n_alloc);
  if((na == 0) || (el_type->GetType() != T_Array &&
                   el_type->GetType() != T_ArrayType))
    return true;
  int i;
  for(i=0; i<items->size; i++) {
    ((cssArray*)items->FastEl(i))->AllocAll(na-1, arg+1);
  }
  return true;
}

void cssArray::Fill(cssEl* it) {
  int old_size = items->size;
  if(it->GetType() == T_ClassType) {
    int i;
    for(i=items->size; i<items->alloc_size; i++) {
      cssClassInst* tmp = new cssClassInst((cssClassType*)it, "");
      tmp->prog = prog;
      tmp->SetAddr(items->Push(tmp));
      tmp->ConstructToken();
    }
  }
  else if (it->GetType() == T_ArrayType) {
    int i;
    for(i=items->size; i<items->alloc_size; i++) {
      cssArray* tmp = new cssArray(*(cssArrayType*)it, "");
      tmp->SetAddr(items->Push(tmp));
    }
  }
  else if(it->GetType() == T_EnumType) {
    int i;
    for(i=items->size; i<items->alloc_size; i++) {
      cssEnum* tmp = new cssEnum((cssEnumType*)it, 0, "");
      tmp->SetAddr(items->Push(tmp));
    }
  }
  else {
    int i;
    for(i=items->size; i<items->alloc_size; i++) {
      cssEl* tmp = it->AnonClone();
      tmp->SetAddr(items->Push(tmp));
    }
  }
  if (old_size == 0 && items->size > 0) {
    ptr.SetSpace(items, 0);
    cssEl::Ref(ptr.El());
  }
}
// arg[1] = name, arg[2] = type, arg[3] = number of dimensions
cssEl* cssArray::MakeToken_stub(int, cssEl* arg[]) {
  cssEl* artyp = arg[2];
  int n_dims = (int)*(arg[3]);
  int i;
  for(i=0; i<n_dims; i++)
    artyp = new cssArray(0, artyp, arg[1]->GetStr());
  return artyp;
}


String cssArray::PrintStr() const {
  String tmp = items->PrintFStr();
  return String(GetTypeName())+" "+name
    +"["+String((int)items->size)+"] {\n" + tmp + "}";
}
String cssArray::PrintFStr() const {
  String tmp = items->PrintFStr();
  return String("[")+String((int)items->size)+"] {" + tmp + "}";
}
String& cssArray::PrintType(String& fh) const {
  fh << GetTypeName() << " " << name << "[" << items->size << "]";
  return fh;
}

void cssArray::operator=(const cssEl& cp) {
  if(cp.GetType() != T_Array) {
    NoAssgn();
    return;
  }
  cssArray* sary = (cssArray*)&cp;
  int i;
  for(i=0; (i<items->size) && (i<sary->items->size); i++) {
    cssEl* trg = items->FastEl(i);
    cssEl* src = sary->items->FastEl(i);
    *trg = *src;                // do an item-wise copy
  }
}

void cssArray::Save(ostream& strm) {
  strm << "[" << items->size << "] { ";
  int i;
  for(i=0; i<items->size; i++) {
    cssEl* itm = items->FastEl(i);
    itm->Save(strm);
    strm << ";";
  }
  strm << "}";
}

void cssArray::Load(istream& strm) {
  int c = taMisc::skip_white(strm, true);
  if(c != '[') {
    taMisc::Error("Did not get array size");
    taMisc::skip_past_err(strm);
    return;
  }
  strm.get();
  c = taMisc::read_word(strm, true);
  if(c != ']') {
    taMisc::Error("Did not get array size");
    taMisc::skip_past_err(strm);
    return;
  }
  int sz = (int)taMisc::LexBuf;
  Alloc(sz);

  c = taMisc::read_till_lbracket(strm, true);
  if(c != '{') {
    taMisc::Error("Did not get array starting bracket");
    taMisc::skip_past_err(strm);
    return;
  }
  strm.get();                   // get the bracket
  int i;
  for(i=0; i<items->size; i++) {
    cssEl* itm = items->FastEl(i);
    itm->Load(strm);
    if(strm.peek() == ';') strm.get(); // skip past ending semi
    if(strm.peek() == '}')
      break;
  }
  c = taMisc::read_till_rbracket(strm, true);
  if(c != '}') {
    taMisc::Error("Did not get array terminator '}'");
    taMisc::skip_past_err(strm);
    return;
  }
  strm.get();                   // get rb
  if(strm.peek() == ';') strm.get(); // skip past ending semi
}


//////////////////////////
//    Array Def Type    //
//////////////////////////

void cssArrayType::Constr() {
  size = 0;
}
void cssArrayType::Copy(const cssArrayType& cp) {
  cssEl::Copy(cp);              // note: skips copying array itself..
  SetSize(cp.size);
  SetElType(cp.el_type);
}
void cssArrayType::CopyType(const cssArrayType& cp) {
  cssArray::CopyType(cp);       // copy type shoudl be good all the way thru
  SetSize(cp.size);
}

cssArrayType::cssArrayType() {
   Constr();
   SetElType(&cssMisc::Void);
}
cssArrayType::cssArrayType(int no, cssEl* it)
{
  Constr();
  SetSize(no);
  SetElType(it);
}
cssArrayType::cssArrayType(int no, cssEl* it, const String& nm)
{
  Constr();
  name = nm;
  SetSize(no);
  SetElType(it);
}
cssArrayType::cssArrayType(const cssArrayType& cp)
{
  Constr();
  Copy(cp);
  name = cp.name;
}
cssArrayType::cssArrayType(const cssArrayType& cp, const String& nm)
{
  Constr();
  Copy(cp);
  name = nm;
}
cssArrayType::~cssArrayType()
{
}
int cssArrayType::AllocAll(int na, cssEl* arg[]) {
  int n_alloc = (int)*arg[0];
  Alloc(n_alloc);
  if((na == 0) || (el_type->GetType() != T_ArrayType))
    return true;
  ((cssArrayType*)el_type)->AllocAll(na-1, arg+1);
  return true;
}

// arg[1] = name, arg[2 .. na-2] = sizes, arg[na-1] = n_dims, arg[na] = type
cssEl* cssArrayType::MakeToken_stub(int na, cssEl* arg[]) {
  cssEl* artyp = arg[na];
  int n_dims = (int)*(arg[na-1]);
  int i;
  for(i=0; i<n_dims; i++) {
    artyp = new cssArrayType((int)*(arg[na-2-i]), artyp, arg[1]->GetStr());
  }
  return artyp;
}
String cssArrayType::PrintStr() const {
  return String(GetTypeName())+" "+name+
    "["+String(size)+"] { (definition only) }";
}
String cssArrayType::PrintFStr() const {
  return String("[")+String(size)+"] { (definition only) }";
}
String& cssArrayType::PrintType(String& fh) const {
  fh << GetTypeName() << " " << name << "[" << size << "]";
  return fh;
}


/////////////////////////////////
//      Reference Type         //
/////////////////////////////////

cssElPtr cssRef::GetAddr() const {
  if(addr != NULL)
    return *addr;
  return ptr;           // its always the thing you refer to, not you!
}

void cssRef::operator=(const cssEl& cp) {
  if(cp.IsRef()) {
    cssRef* tmp = (cssRef*)&cp;
    ptr = tmp->ptr;
  }
  else {
    ptr.El()->operator=(cp);
  }
}

void cssRef::ArgCopy(const cssEl& cp) {
  if(cp.IsRef()) {
    cssRef* tmp = (cssRef*)&cp;
    ptr = tmp->ptr;
  }
  else {
    ptr = cp.GetAddr();
  }
}

// todo: ref needs base class type to perform correctly for non-virt functions!!
int cssRef::GetMemberNo(const String& memb) const {
  if(ptr.El() == &cssMisc::Void) {
    return -1;
  }
  return ptr.El()->GetMemberNo(memb);
}

int cssRef::GetMethodNo(const String& memb) const {
  if(ptr.El() == &cssMisc::Void) {
    return -1;
  }
  return ptr.El()->GetMethodNo(memb);
}

//////////////////////////
//      cssEnumType     //
//////////////////////////

void cssEnumType::Constr() {
//   type_name = "(Enum)";
  enums = new cssSpace;
  //  enums->name = "enums";
  enum_cnt = 0;
}

void cssEnumType::Copy(const cssEnumType& cp) {
  cssEl::Copy(cp);
  type_name = cp.type_name;
  enums->Copy(*(cp.enums));
}

void cssEnumType::CopyType(const cssEnumType& cp) {
  cssEl::CopyType(cp);
  type_name = cp.type_name;
  enums->Copy(*(cp.enums));
}

cssEnumType::cssEnumType(const cssEnumType& cp) {
  Constr(); Copy(cp); name = cp.name;
}
cssEnumType::cssEnumType(const cssEnumType& cp, const String& nm) {
  Constr(); Copy(cp);
  name = nm;
}
cssEnumType::~cssEnumType() {
  delete enums;
}

void cssEnumType::SetTypeName(const String& nm) {
  name = nm; type_name = String("(") + nm + ")";
  enums->name = name + "::enums";
}

String& cssEnumType::PrintType(String& fh) const {
  fh << "enum " << name << " {\n";
  enums->PrintTypeNameVals(fh,1); // indent 1
  fh << "}\n";
  return fh;
}

cssEl* cssEnumType::GetScoped(const String& memb) const {
  cssElPtr rval = enums->FindName(memb);
  if(rval == 0) {
    cssMisc::Error(prog, "Scoped type not found:", memb, "in enum:",GetTypeName());
    return &cssMisc::Void;
  }
  return rval.El();
}

// the enum type makes an enum instance of itself..
cssEl* cssEnumType::MakeToken_stub(int, cssEl* arg[]) {
  return new cssEnum(this, 0, arg[1]->GetStr());
}

cssEl* cssEnumType::FindValue(int val) const {
  int i;
  for(i=0; i<enums->size; i++) {
    cssEl* itm = enums->FastEl(i);
    if((int)*itm == val)
      return itm;
  }
  return &cssMisc::Void;
}



//////////////////////////
//      cssEnum         //
//////////////////////////

void cssEnum::Constr() {
  type_def = &cssMisc::VoidEnumType;
}

void cssEnum::Copy(const cssEnum& cp) {
  cssInt::Copy(cp);
  SetEnumType(cp.type_def);
}

void cssEnum::CopyType(const cssEnum& cp) {
  cssInt::CopyType(cp);
  SetEnumType(cp.type_def);
}

cssEnum::cssEnum(const cssEnum& cp) {
  Constr();
  Copy(cp);
  name = cp.name;
}

cssEnum::cssEnum(const cssEnum& cp, const String& nm) {
  Constr();
  Copy(cp);
  name = nm;
}

cssEnum::cssEnum(cssEnumType* cp, Int vl, const String& nm) {
  Constr();
  val = vl;
  name = nm;
  SetEnumType(cp);
}

cssEnum::~cssEnum() {
  if(type_def != &cssMisc::VoidEnumType)
    cssEl::unRefDone(type_def);
  type_def = &cssMisc::VoidEnumType;
}

const char* cssEnum::GetTypeName() const {
  return (const char*)type_def->type_name;
}

String cssEnum::PrintStr() const {
  cssEl* nm = type_def->FindValue(val);
  if(nm == &cssMisc::Void)
    return cssInt::PrintStr();
  return String(GetTypeName())+" " + name + " = " + nm->name;
}

String cssEnum::PrintFStr() const {
  cssEl* nm = type_def->FindValue(val);
  if(nm == &cssMisc::Void)
    return cssInt::PrintFStr();
  return nm->name;
}

String cssEnum::GetStr() const {
  cssEl* nm = type_def->FindValue(val);
  if(nm == &cssMisc::Void)
    return cssInt::GetStr();
  return nm->name;
}

void cssEnum::operator=(const String& cp) {
  cssElPtr val_ptr = type_def->enums->FindName(cp);
  if(val_ptr == 0)
    val = (int)strtol((const char*)cp, NULL, 0);
  else
    val = ((cssEnum*)val_ptr.El())->val;
}

void cssEnum::operator=(const cssEl& s) {
  if((s.GetType() == T_String) || (s.GetPtrType() == T_String))
    *this = s.GetStr();         // use string converter
  else
    val = (Int)s;
}

cssEl* cssEnum::operator==(cssEl& s) {
  cssElPtr val_ptr = type_def->enums->FindName(s.GetStr());
  if(val_ptr == 0)
    return new cssBool(val == (Int)s);
  else
    return new cssBool(val == ((cssEnum*)val_ptr.El())->val);
}

cssEl* cssEnum::operator!=(cssEl& s) {
  cssElPtr val_ptr = type_def->enums->FindName(s.GetStr());
  if(val_ptr == 0)
    return new cssBool(val != (Int)s);
  else
    return new cssBool(val != ((cssEnum*)val_ptr.El())->val);
}


//////////////////////////////////
//      css Class Member        //
//////////////////////////////////

void cssClassMember::Constr() {
  mbr_type = &cssMisc::Void;
}

void cssClassMember::Copy(const cssClassMember& cp) {
  cssEl::Copy(cp);
  SetMbrType(cp.mbr_type);
}

void cssClassMember::CopyType(const cssClassMember& cp) {
  cssEl::CopyType(cp);
  SetMbrType(cp.mbr_type);
}

cssClassMember::cssClassMember(cssEl* mbtp) {
  Constr();
  SetMbrType(mbtp);
}

cssClassMember::cssClassMember(cssEl* mbtp, const String& mbnm) {
  Constr();
  SetMbrType(mbtp);
  name = mbnm;
}

cssClassMember::cssClassMember(const cssClassMember& cp) {
  Constr();
  Copy(cp);
  name = cp.name;
}

cssClassMember::cssClassMember(const cssClassMember& cp, const String& nm) {
  Constr();
  Copy(cp);
  name = nm;
}

cssClassMember::~cssClassMember() {
  cssEl::DelRefPointer(&mbr_type);
}

String cssClassMember::PrintStr() const {
  return String(mbr_type->GetTypeName()) + " " + name;
}

String cssClassMember::PrintFStr() const {
  return String(mbr_type->GetTypeName()) + " " + name;
}


//////////////////////////////////
//      css Class Type          //
//////////////////////////////////

void cssClassType::Constr() {
  type_name = "(Class)";
  members = new cssSpace;
  methods = new cssSpace;
  parents = new cssSpace;
  types = new cssSpace;
  last_top = NULL;
  multi_space = false;
  members->name = "members";
  methods->name = "methods";
  parents->name = "parents";
  types->name = "types";
  members->el_retv.SetClassMember(this); // any reference to a class member needs updated..
  methods->el_retv.SetNVirtMethod(this); // non-virtual is default assumption
}

void cssClassType::AddBuiltins() {
  methods->PushUniqNameOld
    (new cssMbrCFun(2, NULL, &cssClassType::InheritsFrom_stub, "InheritsFrom"));
  methods->PushUniqNameOld
    (new cssMbrCFun(2, NULL, &cssClassType::Load_stub, "Load"));
  methods->PushUniqNameOld
    (new cssMbrCFun(2, NULL, &cssClassType::Save_stub, "Save"));
}

void cssClassType::Copy(const cssClassType& cp) {
  cssEl::Copy(cp);
  type_name = cp.type_name;
  members->Copy(*(cp.members));
  member_desc.Duplicate(cp.member_desc);
  member_opts.Duplicate(cp.member_opts);
  methods->Copy(*(cp.methods));
  parents->Copy(*(cp.parents));
  types->Copy(*(cp.types));
  desc = cp.desc;
  opts = cp.opts;
  last_top = cp.last_top;
  multi_space = cp.multi_space;
}

void cssClassType::CopyType(const cssClassType& cp) {
  Copy(cp);
}

cssClassType::cssClassType(const cssClassType& cp) {
  Constr();
  Copy(cp);
  name = cp.name;
}

cssClassType::cssClassType(const cssClassType& cp, const String& nm) {
  Constr();
  Copy(cp);
  name = nm;
}
cssClassType::~cssClassType() {
  delete members;
  delete methods;
  delete parents;
  delete types;
}

// the class type makes a class instance of itself unless a membdefn
cssEl* cssClassType::MakeToken_stub(int, cssEl* arg[]) {
  if(cssMisc::parsing_membdefn)
    return Clone();
  else
    return new cssClassInst(this, arg[1]->GetStr());
}

// invoke the constructor
cssEl* cssClassType::NewOpr() {
  MakeTempToken(cssMisc::cur_top->Prog());
  cssClassInst* itm = (cssClassInst*) cssMisc::cur_top->Prog()->Stack()->Pop();
  itm->ConstructToken();
  cssElPtr ptr((cssEl*) itm);
  return new cssPtr(ptr);       // make it a pointer to the item..
}


void cssClassType::ConstructToken(cssClassInst* tok) {
  int i;
  for(i=0; i<parents->size; i++) {
    ((cssClassType*)parents->FastEl(i))->ConstructToken(tok);
  }
  for(i=0; i<tok->members->size; i++) {
    cssEl* mb = tok->members->FastEl(i);
    mb->prog = tok->prog;
    if(mb->GetType() == T_Class)
      ((cssClassInst*)mb->GetNonRefObj())->ConstructToken();
    // arrays of objects already constructed via cssArray::Fill()
  }
  ConstructToken_impl(tok);
}

void cssClassType::DestructToken(cssClassInst* tok) {
  int i;
//  members will get destructors run when they are unref'ed
  DestructToken_impl(tok);      // destroy me first, then parents..
  for(i=0; i<parents->size; i++) {
    ((cssClassType*)parents->FastEl(i))->DestructToken(tok);
  }
}

void cssClassType::CallVoidMethod(cssClassInst* tok, const String& meth_nm) {
  String nm = meth_nm;
  if(nm.contains(')')) {
    nm = nm.before(')');
    nm = nm.after('(');
  }
  cssElPtr ptr = methods->FindName(nm); // find name of this type..
  if(ptr == 0) return;                                 // didn't find it..
  cssMbrScriptFun* meth = (cssMbrScriptFun*)methods->FastEl(ptr.dx);
  if(meth->GetSubProg() != NULL) {
    meth->GetSubProg()->state |= cssProg::State_NoBreak; // do not break this guy
  }

  // get the appropriate context to run in: either token top
  // or the generic top-level
  cssProg* prg = cssMisc::CDtorProg;
  prg->state |= cssProg::State_IsTmpProg; // flag to cssProgSpace::Cont to not run past this guy
  prg->state |= cssProg::State_NoBreak;
  cssProgSpace* old_tok_top = NULL;
  cssProgSpace* old_prg_top = NULL;
  if((tok->prog != NULL) && (tok->prog->top != NULL)) {
    // todo: this should not be needed anymore
    cssMisc::SetCurTop(tok->prog->top);    // reparent to current top
    old_prg_top = prg->SetTop(cssMisc::cur_top);
  }
  else {
    cssMisc::SetCurTop(cssMisc::Top);
    old_prg_top = prg->SetTop(cssMisc::cur_top);
    tok->prog = prg;
  }

  cssEl::Ref(tok);              // make sure it doesn't get to 0 refcount in arg del..
  cssEl::Ref(meth);

  prg->Reset();
  // not using prog::code function because it might go to another top..
  cssElPtr elp;  elp.SetDirect(meth);           // code the meth
  cssInst* tmp = new cssInst(prg, elp);
  tmp->idx = prg->AddCode(tmp);

  prg->Restart();
  prg->top->Shove(prg);
  prg->Stack()->Push(&cssMisc::Void); // argstop
  prg->Stack()->Push(tok);
  prg->top->Cont();             // runs it
  prg->top->Pull();             // pull the meth/dtor off of stack!
  prg->Reset();

  if(meth->GetSubProg() != NULL) {
    meth->GetSubProg()->state &= ~cssProg::State_NoBreak; // do not break this guy
  }

  cssEl::unRef(tok);            // undo that
  cssEl::unRef(meth);           // undo that

  if(old_prg_top != NULL)       prg->PopTop(old_prg_top);
  if(old_tok_top != NULL)       tok->prog->PopTop(old_prg_top);
  cssMisc::PopCurTop();
}

void cssClassType::ConstructToken_impl(cssClassInst* tok) {
  CallVoidMethod(tok, type_name);
}

void cssClassType::DestructToken_impl(cssClassInst* tok) {
  String nm = type_name;
  if(nm.contains(')')) {
    nm = nm.before(')');
    nm = nm.after('(');
  }
  nm = "~" + nm;
  CallVoidMethod(tok, nm);
}

void cssClassType::UpdateAfterEdit_impl(cssClassInst* tok) {
  CallVoidMethod(tok, "UpdateAfterEdit");
}

void cssClassType::SetTypeName(const String& nm) {
  name = nm; type_name = String("(") + nm + ")";
  members->name = name + "::members";
  methods->name = name + "::methods";
  parents->name = name + "::parents";
  types->name = name + "::types";
}

String cssClassType::GetStr() const {
  return PrintFStr();
}
void cssClassType::AddParent(cssClassType* par) {
  parents->Push(par);
  members->CopyUniqNameNew(*(par->members));
  // copy in member_desc and member_opts into proper slots
  member_desc.SetSize(members->size);
  member_opts.SetSize(members->size);
  int i;
  for(i=0; i < par->members->size; i++) {
    int dx = members->FindName(par->members->FastEl(i)->name).dx;
    member_desc[dx] = par->member_desc[i];
    member_opts[dx] = par->member_opts[i];
  }
  methods->CopyUniqNameNew(*(par->methods));
  types->CopyUniqNameNew(*(par->types));
}

bool cssClassType::InheritsFrom(const cssClassType* cp) {
  // identity?
  if(strcmp(GetTypeName(), cp->GetTypeName()) == 0)
    return true;

  // recurse through parents
  int i;
  for (i = 0; i < parents->size; i++) {
    cssClassType* par = (cssClassType*) parents->El(i);
    if(par->InheritsFrom(cp)) return true;
  }
  return false;
}

cssEl* cssClassType::InheritsFrom_stub(void*, int, cssEl* arg[]) {
  // 'this' must be a class or class object to even get here
  cssClassType* ths;
  if(arg[1]->GetType() == T_Class)
    ths = ((cssClassInst*)arg[1])->type_def;
  else
    ths = (cssClassType*)arg[1];

  cssEl* arg2 = arg[2]->GetActualObj();
  cssClassType* that;
  if(arg2->GetType() == T_Class)
    that = ((cssClassInst*)arg2->GetNonRefObj())->type_def;
  else if (arg2->GetType() == T_ClassType)
    that = (cssClassType*)arg2->GetNonRefObj();
  else {
    cssElPtr tp_ptr;
    if((ths->prog != NULL) && (ths->prog->top != NULL))
      tp_ptr = ths->prog->top->FindTypeName(arg2->GetStr());
    else
      tp_ptr = ths->last_top->FindTypeName(arg2->GetStr());
    if((tp_ptr == 0) || (tp_ptr.El()->GetType() != T_ClassType)) {
      cssMisc::Error(ths->prog, "argument is not a class, class object, or class type name");
      return &cssMisc::Void;
    }
    that = (cssClassType*)tp_ptr.El();
  }
  return new cssBool(ths->InheritsFrom(that));
}

cssEl* cssClassType::Load_stub(void*, int, cssEl* arg[]) {
  // 'this' must be a class or class object to even get here
  cssClassInst* ths;
  if(arg[1]->GetType() == T_Class)
    ths = (cssClassInst*)arg[1]->GetNonRefObj();
  else {
    cssMisc::Error(NULL, "Load must be called on a class instance, not just a type");
    return &cssMisc::Void;
  }

  istream* fh = (istream*)*(arg[2]);
  if((fh == NULL) || !fh->good())
    return &cssMisc::Void;
  ths->Load(*fh);
  return &cssMisc::Void;
}

void cssClassType::MbrSetDesc(int mbr, const String& des) {
  if (member_desc.InRange(mbr) && member_opts.InRange(mbr)) {
    SetDesc_impl(member_desc.FastEl(mbr), member_opts.FastEl(mbr), des);
  }
}

cssEl* cssClassType::Save_stub(void*, int, cssEl* arg[]) {
  // 'this' must be a class or class object to even get here
  cssClassInst* ths;
  if(arg[1]->GetType() == T_Class)
    ths = (cssClassInst*)arg[1]->GetNonRefObj();
  else {
    cssMisc::Error(NULL, "Save must be called on a class instance, not just a type");
    return &cssMisc::Void;
  }

  ostream* fh = (ostream*)*(arg[2]);
  if((fh == NULL) || !fh->good())
    return &cssMisc::Void;
  ths->Save(*fh);
  return &cssMisc::Void;
}

String  cssClassType::PrintStr() const {
  String tmp = members->PrintStr();
  return String(GetTypeName())+" "+name+" {\n" + tmp + "\n}";
}
String  cssClassType::PrintFStr() const {
  String tmp = members->PrintStr();
  return String("{\n") + tmp + "\n}";
}
String& cssClassType::PrintInherit(String& fh) const {
  if(parents->size > 0) {
    fh << name << " : ";
    int i;
    (*parents)[0]->PrintInherit(fh);
    for(i=1; i<parents->size; i++) {
      fh << ", ";
      (*parents)[i]->PrintInherit(fh);
    }
  }
  else {
    fh << name;
  }
  return fh;
}

String& cssClassType::PrintType(String& fh) const {
  fh << "class ";
  PrintInherit(fh);
  fh << " {\n  // " << desc << "\n";
  if(types->size > 0) {
    fh << "\n  // types\n";
    types->PrintTypeNameVals(fh,1); // indent
  }
  fh << "\n  // members\n";
  String_PArray col1;
  String_PArray col2;
  for(int i=0; i<members->size; i++) {
    cssClassMember* mbr = (cssClassMember*)members->FastEl(i);
    String c1 = mbr->mbr_type->GetTypeName();
    if(c1.contains(')')) {
      c1 = c1.before(')');
      c1 = c1.after('(');
    }
    col1.Add(c1);
    String c2;
    c2 << mbr->name;
    if (mbr->mbr_type->GetType() == cssEl::T_Array) {
      cssArray* ar = (cssArray*) mbr->mbr_type->GetNonRefObj();
      c2 << '[' << ar->items->size << ']';
    }
    else if (mbr->mbr_type->GetType() == cssEl::T_ArrayType) {
      cssArrayType* ar = (cssArrayType*) mbr->mbr_type->GetNonRefObj();
      c2 << '[' << ar->size << ']';
    }
    if(!member_desc[i].empty())
      c2 << "\t// " << member_desc[i];
    col2.Add(c2);
  }
  taMisc::FancyPrintTwoCol(fh, col1, col2, 1); // indent
  col1.Reset();
  col2.Reset();
  fh << "\n  // methods\n";
  for(int i=0; i<methods->size; i++) {
    String c1;  String c2;
    if(methods->FastEl(i)->GetType() != T_MbrScriptFun) {
      c1 << "builtin:";
      c2 << methods->FastEl(i)->PrintStr();
      col1.Add(c1); col2.Add(c2);
      continue;
    }
    cssMbrScriptFun* meth = (cssMbrScriptFun*)methods->FastEl(i);
    if((cssMisc::cur_top->debug == 0) && meth->is_tor)
      continue;
    c1 = String(meth->argv[0].El()->GetTypeName());
    c1 = c1.before(')');
    c1 = c1.after('(');
    col1.Add(c1);
    c2 << meth->name << "(";
    for(int j=2; j<=meth->argc; j++) {
      c2 << meth->argv[j].El()->GetTypeName() << " " << meth->argv[j].El()->name;
      if(j >= meth->def_start) {
        c2 << " = " << meth->arg_defs.El((j-meth->def_start))->PrintFStr();
      }
      if(j < meth->argc)
        c2 << ", ";
    }
    c2 << ")\t// " << meth->desc;
    if(prog && prog->top->debug > 0)
      c2 << " top: " << meth->fun->top->name;
    col2.Add(c2);
  }
  taMisc::FancyPrintTwoCol(fh, col1, col2, 1); // indent
  fh << "}\n";
  return fh;
}

void cssClassType::SetDesc_impl(String& dsc, String& opt, const String& des) {
  dsc = "";
  String tmp = des;
  tmp.gsub("\"", "'");          // don't let any quotes get through
  tmp.gsub("\n", " ");          // replace whitespace..
  tmp.gsub("\t", " ");          // replace whitespace..
  String ud;
  while(tmp.contains('#')) {
    dsc += tmp.before('#');
    tmp = tmp.after('#');
    if(tmp.contains(' '))
      ud = tmp.before(' ');
    else
      ud = tmp;
    tmp = tmp.after(' ');
    opt += ud + " ";
  }
  dsc += tmp;
}

String cssClassType::OptionAfter_impl(const String& optns, const String& opt) const {
  if(!optns.contains(opt))
    return "";
  String rval = ((String&)optns).after(opt);
  rval = rval.before(' ');
  return rval;
}

void cssClassType::GetComments(cssEl* obj, cssElPtr cmt) {
  member_desc.SetSize(members->size);
  member_opts.SetSize(members->size);

  if (!cmt) return;

  String comment = ((cssString*)cmt.El())->val;

  if (obj == this) {    // class description
    SetDesc(comment);
  }
  else if (obj->GetType() == T_MbrScriptFun) {  // method description
    ((cssMbrScriptFun*)obj)->SetDesc(comment);
  }
  else {
    int mbridx = GetMemberNo(obj->name);
    if (mbridx >= 0) { // member description
      MbrSetDesc(mbridx, comment);
    }
  }
}

int cssClassType::GetMemberNo(const String& memb) const {
  cssElPtr rval = members->FindName(memb);
  if(rval == 0)
    return -1;
  return rval.dx;
}
cssEl* cssClassType::GetMemberFmName(const String& memb) const {
  cssMisc::Warning(prog, "Getting member:",memb,"from class type (not inst)", GetTypeName());
  cssElPtr rval = members->FindName(memb);
  if(rval == 0) {
    cssMisc::Error(prog, "Member not found:", memb, "in class:", name, "of type:",
              GetTypeName());
    return &cssMisc::Void;
  }
  return rval.El();
}
cssEl* cssClassType::GetMemberFmNo(int memb) const {
  cssEl* rval = members->El(memb);
  if(rval == NULL) {
    cssMisc::Error(prog, "Member not found:", String(memb), "in class:", name, "of type:",
              GetTypeName());
    return &cssMisc::Void;
  }
  return rval;
}

int cssClassType::GetMethodNo(const String& memb) const {
  cssElPtr rval = methods->FindName(memb);
  if(rval == 0)
    return -1;
  return rval.dx;
}
cssEl* cssClassType::GetMethodFmName(const String& memb) const {
  cssElPtr rval = methods->FindName(memb);
  if(rval == 0) {
    cssMisc::Error(prog, "Member function not found:", memb, "in class:", name, "of type:",
              GetTypeName());
    return &cssMisc::Void;
  }
  if(rval.El()->GetType() != T_MbrScriptFun &&
     rval.El()->GetType() != T_MbrCFun) {
    cssMisc::Error(prog, "Member:", memb, "of type:", rval.El()->GetTypeName(),
              "is not a function, in class:", name, "of type:", GetTypeName());
    return &cssMisc::Void;
  }
  return rval.El();
}
cssEl* cssClassType::GetMethodFmNo(int memb) const {
  cssEl* rval = methods->El(memb);
  if(rval == NULL) {
    cssMisc::Error(prog, "Member function not found:", String(memb), "in class:", name, "of type:",
              GetTypeName());
    return &cssMisc::Void;
  }
  if(rval->GetType() != T_MbrScriptFun &&
     rval->GetType() != T_MbrCFun) {
    cssMisc::Error(prog, "Member:", String(memb), "of type:", rval->GetTypeName(),
              "is not a function, in class:", name, "of type:", GetTypeName());
    return &cssMisc::Void;
  }
  return rval;
}
cssEl* cssClassType::GetScoped(const String& memb) const {
  cssElPtr rval = types->FindName(memb);
  if(rval == 0) {
    rval = methods->FindName(memb);
    if(rval == 0) {
      rval = members->FindName(memb);
      if(rval == 0) {
        cssMisc::Error(prog, "Scoped type not found:", memb, "in class:", name, "of type:",
                       GetTypeName());
        return &cssMisc::Void;
      }
      else
        rval = ((cssClassMember*)rval.El())->mbr_type;
    }
  }
  return rval.El();
}


//////////////////////////////////
//      css Class Inst          //
//////////////////////////////////

void cssClassInst::Constr() {
  members = new cssSpace;
  members->name = "members";
  type_def = &cssMisc::VoidClassType;
}

void cssClassInst::Copy(const cssClassInst& cp) {
  cssEl::Copy(cp);
  SetClassType(cp.type_def);
  members->Copy(*(cp.members));
}

void cssClassInst::CopyType(const cssClassInst& cp) {
  cssEl::CopyType(cp);
  SetClassType(cp.type_def);
  members->Copy(*(cp.members));
}

cssClassInst::cssClassInst(const cssClassInst& cp) {
  Constr();
  Copy(cp);
  name = cp.name;
}
cssClassInst::cssClassInst(const cssClassInst& cp, const String& nm) {
  Constr();
  Copy(cp);
  name = nm;
}

cssClassInst::cssClassInst(cssClassType* cp, const String& nm) {
  Constr();
  name = nm;
  SetClassType(cp);
  int i;
  for(i=0; i < cp->members->size; i++) {
    cssClassMember* mb = (cssClassMember*)cp->members->FastEl(i);
    if(mb->mbr_type->GetType() == T_ClassType) {
      members->Push(new cssClassInst((cssClassType*)mb->mbr_type->GetNonRefObj(), mb->name));
    }
    else if(mb->mbr_type->GetType() == T_ArrayType) {
      members->Push(new cssArray(*(cssArrayType*)mb->mbr_type->GetNonRefObj(), mb->name));
    }
    else if(mb->mbr_type->GetType() == T_EnumType) {
      members->Push(new cssEnum((cssEnumType*)mb->mbr_type->GetNonRefObj(), 0, mb->name));
    }
    else {
      cssEl* nwmb = mb->mbr_type->Clone();
      nwmb->name = mb->name;
      members->Push(nwmb);
    }
  }
}

cssClassInst::~cssClassInst() {
  // one could do this, but its probably a bit slow..
  // cssivSession::CancelObjEdits(this);
  DestructToken();
  delete members;
  if(type_def != &cssMisc::VoidClassType)
    cssEl::unRefDone(type_def);
  type_def = &cssMisc::VoidClassType;
}

void cssClassInst::ConstructToken() {
  if(type_def != &cssMisc::VoidClassType)
    type_def->ConstructToken(this);
}

void cssClassInst::DestructToken() {
  if(type_def != &cssMisc::VoidClassType)
    type_def->DestructToken(this);
}

void cssClassInst::UpdateAfterEdit() {
  if(type_def != &cssMisc::VoidClassType)
    type_def->UpdateAfterEdit_impl(this);
}
#ifdef TA_GUI
int cssClassInst::Edit(bool wait) {
  if (!taMisc::gui_active)
    return false;

  cssProgSpace* top;
  if (prog != NULL)
    top = prog->top;
  else
    top = cssMisc::cur_top;
  cssiEditDialog* dlg = new cssiEditDialog(this, top);
  dlg->Constr();
  return dlg->Edit(wait);
}
#endif //

String cssClassInst::GetStr() const {
  return PrintFStr();
}

String  cssClassInst::PrintStr() const {
  String tmp;
  members->PrintTypeNameVals(tmp,1);
  return String(GetTypeName())+" "+name+" {\n" + tmp + "}";
}
String  cssClassInst::PrintFStr() const {
  String tmp;
  members->PrintTypeNameVals(tmp,1);
  return String("{\n") + tmp + "}";
}

const char* cssClassInst::GetTypeName() const {
  return type_def->GetTypeName();
}
String& cssClassInst::PrintInherit(String& fh) const {
  return type_def->PrintInherit(fh);
}

String& cssClassInst::PrintType(String& fh) const {
  return type_def->PrintType(fh);
}

int cssClassInst::GetMemberNo(const String& memb) const {
  cssElPtr rval = members->FindName(memb);
  if(rval == 0)
    return -1;
  return rval.dx;
}
cssEl* cssClassInst::GetMemberFmName(const String& memb) const {
  cssElPtr rval = members->FindName(memb);
  if(rval == 0) {
    cssMisc::Error(prog, "Member not found:", memb, "in class:", name, "of type:",
              GetTypeName());
    return &cssMisc::Void;
  }
  return rval.El();
}
cssEl* cssClassInst::GetMemberFmNo(int memb) const {
  cssEl* rval = members->El(memb);
  if(rval == NULL) {
    cssMisc::Error(prog, "Member not found:", String(memb), "in class:", name, "of type:",
              GetTypeName());
    return &cssMisc::Void;
  }
  return rval;
}

int cssClassInst::GetMethodNo(const String& memb) const {
  if(type_def != &cssMisc::VoidClassType)
    return type_def->GetMethodNo(memb);
  return -1;
}
cssEl* cssClassInst::GetMethodFmName(const String& memb) const {
  if(type_def != &cssMisc::VoidClassType)
    return type_def->GetMethodFmName(memb);
  cssMisc::Error(prog, "Member function not found:", memb, "in class:", name, "of type:",
                 GetTypeName());
  return &cssMisc::Void;
}
cssEl* cssClassInst::GetMethodFmNo(int memb) const {
  if(type_def != &cssMisc::VoidClassType)
    return type_def->GetMethodFmNo(memb);
  cssMisc::Error(prog, "Member function not found:", String(memb), "in class:", name, "of type:",
                 GetTypeName());
  return &cssMisc::Void;
}
cssEl* cssClassInst::GetScoped(const String& memb) const {
  if(type_def != &cssMisc::VoidClassType)
    return type_def->GetScoped(memb);
  cssMisc::Error(prog, "Scoped type not found:", memb, "in class:", name, "of type:",
                 GetTypeName());
  return &cssMisc::Void;
}

void cssClassInst::operator=(const cssEl& s) {
  if(s.GetType() != T_Class) {
    cssMisc::Error(prog, "Assign between a class and a non-class not allowed");
    return;
  }
  cssClassInst* tmp = (cssClassInst*)&s;
  int i;
  cssElPtr pt;
  for(i=0; i<members->size; i++) {
    if((pt = tmp->members->FindName((char*)members->els[i]->name)) != 0) {
      *(members->els[i]) = *(pt.El());
    }
  }
}

void cssClassInst::Load(istream& strm) {
  int c;
  c = taMisc::read_till_lbracket(strm,false); // get the lbrack
  if(c == EOF) return;
  if(c != '{') {
    taMisc::Error("*** left bracket not found for start of class");
    taMisc::skip_past_err(strm);
    return;
  }
  String tp_nm = taMisc::LexBuf;
  tp_nm = tp_nm.before(' ');
  cssElPtr tpptr = prog->top->FindTypeName(tp_nm);
  if((tpptr == 0) || (tpptr.El()->GetType() != T_ClassType) ||
     !type_def->InheritsFrom((cssClassType*)tpptr.El())) {
    taMisc::Error("*** type name:",tp_nm,"not a valid type, or this type:",
                  type_def->name, "does not inherit from it");
    taMisc::skip_past_err(strm);
    return;
  }

  do {
    c = taMisc::read_word(strm, true);
    if(c == EOF) return;
    if(c == '}') {
      strm.get();
      if(strm.peek() == ';') strm.get(); // skip past ending semi
      return;
    }

    String mb_name = taMisc::LexBuf;
    if((c == ' ') || (c == '\n') || (c == '\t')) { // skip past white
      strm.get();
      c = taMisc::skip_white(strm, true);
    }
    if(c != '=') {
      taMisc::Error("*** did not get '=' for member");
      taMisc::skip_past_err(strm);
      continue;
    }
    else {
      strm.get();               // get the equals sign
      c = taMisc::skip_white(strm, true); // then skip past any new whitespace
    }

    cssElPtr mb_ptr = members->FindName(mb_name);
    if(mb_ptr == 0) {
      taMisc::Error("*** Member:", mb_name, "not found in type:", type_def->name);
      taMisc::skip_past_err(strm);
      continue;
    }
    cssEl* mb = mb_ptr.El();

    mb->Load(strm);

  } while(1);
}

void cssClassInst::Save(ostream& strm) {
  strm << type_def->name << " {\n";
  int i;
  for(i=0; i<members->size; i++) {
    cssEl* mb = members->FastEl(i);
    strm << mb->name << " = " ;
    mb->Save(strm);
    strm << ";\n";

  }
  strm << "}";
}



//////////////////////////////////
//      css Sub Shell           //
//////////////////////////////////

String cssSubShell::PrintStr() const {
  return String(GetTypeName()) + " " + name + " -> " + prog_space.name;
}

String cssSubShell::PrintFStr() const {
  return prog_space.name;
}

cssSubShell::cssSubShell() {
  Constr();
}
cssSubShell::cssSubShell(const String& nm) {
  name = nm; Constr();
  prog_space.name = nm;
}
cssSubShell::cssSubShell(const cssSubShell& cp) {
  Constr(); Copy(cp); name = cp.name;
}
cssSubShell::cssSubShell(const cssSubShell& cp, const String& nm) {
  Constr(); Copy(cp); name = nm;
  prog_space.name = nm;
}
