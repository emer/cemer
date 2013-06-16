// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "taArray_base.h"
#include <int_Matrix>
#include <byte_Matrix>
#include <taBaseItr>
#include <taMisc>

using namespace std;

void taArray_base::Initialize() {
  el_view_mode = IDX_UNK;
}

void taArray_base::CutLinks() {
  Reset();
  el_view.CutLinks();
  el_view_mode = IDX_UNK;
  owner = NULL;
  taOBase::CutLinks();
}

bool taArray_base::SetElView(taMatrix* view_mat, IndexMode md) {
  if(!IterValidate(view_mat, md, 1)) return false;
  el_view = view_mat;
  el_view_mode = md;
  return true;
}

taArray_base* taArray_base::NewElView(taMatrix* view_mat, IndexMode md) const {
  if(!IterValidate(view_mat, md, 1)) return NULL;
  taArray_base* rval = (taArray_base*)MakeToken(); // make a token of me
  rval->Copy_Duplicate(*this);                     // full copy
  rval->SetElView(view_mat, md);
  return rval;
}

Variant taArray_base::Elem(const Variant& idx, IndexMode mode) const {
  if(mode == IDX_UNK) {
    mode = IndexModeDecode(idx, 1);
    if(mode == IDX_UNK) return _nilVariant;
  }
  if(!IndexModeValidate(idx, mode, 1))
    return _nilVariant;
  switch(mode) {
  case IDX_IDX: {
    return SafeElAsVar(idx.toInt());
    break;
  }
  case IDX_NAME:
  case IDX_MISC:
  case IDX_NAMES: {
    TestError(true, "Elem::IDX_NAME/S or MISC", "no support for these index types in arrays");
    return _nilVariant;
    break;
  }
  case IDX_COORD: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    return SafeElAsVar(cmat->FastEl_Flat(0));
    break;
  }
  case IDX_COORDS:
  case IDX_FRAMES: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    if(cmat->size == 1)
      return SafeElAsVar(cmat->FastEl_Flat(0));
    int_Matrix* imat = new int_Matrix(1,0);
    TA_FOREACH(mitm, *cmat) { // use iterator on matrix so it can be filtered too
      int el_idx = mitm.toInt();
      if(el_idx < 0) el_idx += size;
      if(el_idx < 0 || el_idx >= size) continue; // skip out of range at this point
      imat->Add(el_idx);
    }
    taArray_base* nwvw = NewElView(imat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_SLICE: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    int start = cmat->FastEl_Flat(0);
    int end = cmat->FastEl_Flat(1);
    int step = cmat->FastEl_Flat(2);
    if(step == 0) step = 1;
    if(!FixSliceValsFromSize(start, end, size))
      return _nilVariant;
    int_Matrix* imat = new int_Matrix(1,0);
    if(step > 0) {
      for(int i = start; i < end; i += step) {
        imat->Add(i);
      }
    }
    else {
      for(int i = end-1; i >= start; i += step) {
        imat->Add(i);
      }
    }
    taArray_base* nwvw = NewElView(imat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_MASK: {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(idx.toMatrix());
    if(TestError(cmat->dim(0) != size, "Elem::IDX_MASK",
                 "index matrix dim(0):", String(cmat->dim(0)),
                 "is not size of list:", String(size)))
      return false;
    if(el_view && el_view_mode == IDX_MASK) {
      // take intersection of the existing mask
      byte_Matrix* am = (byte_Matrix*)(*cmat && *ElView());
      taBase::Ref(am);
      cmat->Copy(am);
      taBase::UnRef(am);
    }
    taArray_base* nwvw = NewElView(cmat, IDX_MASK);
    return (Variant)nwvw;
    break;
  }
  case IDX_UNK:
    break;
  }
  return _nilVariant;
}

taBaseItr* taArray_base::Iter() const {
  taBaseItr* rval = new taBaseItr;
  taBase::Ref(rval);
  return rval;
}

Variant taArray_base::IterElem(taBaseItr* itr) const {
  if(!itr) return _nilVariant;
  return SafeElAsVar(itr->el_idx);
}

String& taArray_base::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << "[" << size << "] {";
  String_PArray strs;
  strs.Alloc(size);
  TA_FOREACH(vitm, *this) {
    strs.Add(El_GetStr_(FastEl_(FOREACH_itr->el_idx)));
  }
  taMisc::FancyPrintList(strm, strs, indent+1);
  taMisc::IndentString(strm, indent);
  strm << "}";
  return strm;
}

void taArray_base::SigEmit(int sls, void* op1, void* op2) {
  taOBase::SigEmit(sls, op1, op2);
}

String taArray_base::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
                              bool force_inline) const {
  if(force_inline) {
    return GetValStr();         // this is the taArray_impl version that actually lists items
  }
  else {
    String nm = " Size: ";
    nm += String(size);
    nm += String(" (") + GetTypeDef()->name + ")";
    return nm;
  }
}

bool taArray_base::SetValStr(const String& val, void* par, MemberDef* memb_def,
                            TypeDef::StrContext sc, bool force_inline) {
  InitFromString(val);
  return true;
}

int taArray_base::ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                void* par, TypeDef* par_typ, MemberDef* memb_def, TypeDef::StrContext sc) {
  int rval = 0;
  String mypath = GetPathNames();
  for(int i=0; i<size; i++) {
    String str = El_GetStr_(FastEl_(i));
    if(!str.contains(srch)) continue;
    String orig = str;
    rval += str.gsub(srch, repl);
    El_SetFmStr_(FastEl_(i), str);
    taMisc::Info("Replaced string value in array object:",
                 mypath,"orig val:", orig, "new val:", str);
  }
  if(rval > 0)
    UpdateAfterEdit();
  return rval;
}

int taArray_base::Dump_Save_Value(ostream& strm, taBase*, int) {
  strm << "{ ";
  int i;
  for(i=0;i<size;i++) {
    strm << El_GetStr_(FastEl_(i)) << ";";
  }
  return true;
}

int taArray_base::Dump_Load_Value(istream& strm, taBase*) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)    return EOF;
  if(c == ';') // just a path
    return 2;  // signal that just a path was loaded..

  if(TestError((c != '{'), "Dump_Load_Value",
               "Missing '{' in dump file")) return false;
  c = taMisc::read_till_rb_or_semi(strm);
  int cnt = 0;
  while((c == ';') && (c != EOF)) {
    if(cnt > size-1)
      Add_(El_GetTmp_());
    El_SetFmStr_(FastEl_(cnt++), taMisc::LexBuf);
    c = taMisc::read_till_rb_or_semi(strm);
  }
  this->SetSize(cnt);
  if (c==EOF)   return EOF;
  return true;
}
