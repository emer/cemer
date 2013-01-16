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

#include "taArray_impl.h"
#include <MTRnd>

#include <DataChangedReason>
#include <taMisc>

void taArray_impl::Clear_Tmp_() {
  String val;
  El_SetFmStr_(El_GetTmp_(), val);
}

void taArray_impl::AddOnly_(const void* it) {
  if (size >= alloc_size) {
    if(!Alloc(size+1))
      return;
  }
  El_Copy_(FastEl_(size++), it);
}

void taArray_impl::Add_(const void* it) {
  AddOnly_(it);
  DataChanged(DCR_ARY_SIZE_CHANGED);
}

bool taArray_impl::AddUniqueOnly_(const void* it) {
  if(FindEl_(it) >= 0)
    return false;
  AddOnly_(it);
  return true;
}

bool taArray_impl::AddUnique_(const void* it) {
  if(FindEl_(it) >= 0)
    return false;
  Add_(it);
  return true;
}

bool taArray_impl::Alloc(int sz) {
  if (alloc_size < sz)  {
    // start w/ 4, double up to 64, then 1.5x thereafter
    if (alloc_size == 0) alloc_size = MAX(4, sz);
    else if (alloc_size < 64) alloc_size = MAX((alloc_size * 2), sz);
    else alloc_size =  MAX(((alloc_size * 3) / 2) , sz);
    char* nw = (char*)MakeArray_(alloc_size);
    if(!nw) {
      taMisc::Error("taArray_impl::Alloc -- malloc error -- array too big! this may be fatal");
      return false;
    }
    for (int i = 0; i < size; ++i) {
      El_Copy_(nw + (El_SizeOf_() * i), FastEl_(i));
    }
    SetArray_(nw);
  }
  return true;
}

bool taArray_impl::AllocExact(int sz) {
  alloc_size = MAX(sz, 1);      // need at least 1
  size = MIN(size, alloc_size);
  char* nw = (char*)MakeArray_(alloc_size);
  if(!nw) {
    taMisc::Error("taArray_impl::Alloc -- malloc error -- array too big! this may be fatal");
    return false;
  }
  for (int i = 0; i < size; ++i) {
    El_Copy_(nw + (El_SizeOf_() * i), FastEl_(i));
  }
  SetArray_(nw);
  return true;
}

void taArray_impl::Reset() {
  Reset_impl(); DataChanged(DCR_ARY_SIZE_CHANGED);
}

void taArray_impl::AddBlank(int n_els) {
  if (n_els < 0) n_els = 0;
  SetSize(size + n_els);
}

void taArray_impl::Copy_(const taArray_impl& cp) {
  if (cp.size < size) ReclaimOrphans_(cp.size, size - 1);
  else if (cp.size > alloc_size) {
    if(!Alloc(cp.size)) return;
  }

  for (int i=0; i < cp.size; ++i) {
    El_Copy_(FastEl_(i), cp.FastEl_(i));
  }
  size = cp.size;
}

void taArray_impl::SetSize(int new_size) {
  if (new_size < 0) new_size = 0;
  if (new_size == size) return;
  else if (new_size > size) {
    if(!Alloc(new_size)) return;
    Clear_Tmp_();
    Insert_(El_GetTmp_(), size, new_size - size);
  } else if (new_size < size)  {
    ReclaimOrphans_(new_size, size - 1);
  }
  size = new_size;
  DataChanged(DCR_ARY_SIZE_CHANGED);
}

bool taArray_impl::Equal_(const taArray_impl& ar) const {
  if (size != ar.size) return false;
  for (int i = 0; i < size; ++i)
    if (!El_Equal_(FastEl_(i), ar.FastEl_(i))) return false;
  return true;
}

int taArray_impl::FindEl_(const void* it, int where) const {
  int i;
  for(i=where; i<size; i++) {
    if(El_Compare_(it, FastEl_(i)) == 0)
      return i;
  }
  return -1;
}

void taArray_impl::InitVals_(const void* it, int start, int end) {
  if(end == -1) end = size;  else end = MIN(size, end);
  int i;
  for(i=start;i<end;i++) {
    El_Copy_(FastEl_(i), it);
  }
}

void taArray_impl::Insert_(const void* it, int where, int n) {
  if((where > size) || (n <= 0)) return;
  if ((size + n) > alloc_size) {
    if(!Alloc(size + n)) return;        // pre-add stuff
  }
  if((where==size) || (where < 0)) {
    int i;
    for (i=0; i<n; i++) AddOnly_(it);
    DataChanged(DCR_ARY_SIZE_CHANGED);
    return;
  }
  int i;
  int n_mv = size - where;      // number that must be moved
  size += n;
  int trg_o = size-1;
  int src_o = size-1-n;
  for(i=0; i<n_mv; i++)         // shift everyone over
    El_Copy_(FastEl_(trg_o - i), FastEl_(src_o - i));
  for(i=where; i<where+n; i++)
    El_Copy_(FastEl_(i), it);
  DataChanged(DCR_ARY_SIZE_CHANGED);
}

bool taArray_impl::MoveIdx(int fm, int to) {
  if((size == 0) || (fm >= size) || (to >= size)) return false;

  void* tmp = El_GetTmp_();
  El_Copy_(tmp, FastEl_(fm));
  int j;
  for(j=fm; j < size-1; j++) {          // compact, if necc
    El_Copy_(FastEl_(j), FastEl_(j+1));
  }
  for(j=size-1; j>to; j--) {
    El_Copy_(FastEl_(j), FastEl_(j-1));
  }
  El_Copy_(FastEl_(to), tmp);
  return true;
}

bool taArray_impl::SwapIdx(int pos1, int pos2) {
  if ((pos1 < 0) || (pos2 < 0) || (pos1 >= size) || (pos2 >= size)) return false;
  if (pos1 == pos2) return true;
  void* tmp = El_GetTmp_();
  El_Copy_(tmp, FastEl_(pos1));
  El_Copy_(FastEl_(pos1), FastEl_(pos2));
  El_Copy_(FastEl_(pos2), tmp);
  return true;
}

void taArray_impl::Permute() {
  int i, nv;
  void* tmp = El_GetTmp_();
  for(i=0; i<size; i++) {
    nv = (int) ((MTRnd::genrand_int32() % (size - i)) + i); // get someone from the future
    El_Copy_(tmp, FastEl_(i));
    El_Copy_(FastEl_(i), FastEl_(nv));  // swap with yourself
    El_Copy_(FastEl_(nv), tmp);
  }
}

bool taArray_impl::RemoveEl_(const void* it) {
  int i = FindEl_(it);
  if (i < 0) return false;
  return RemoveIdx(i);
}

bool taArray_impl::RemoveIdx(uint i, int n) {
  if((int)i >= size) return false;
  n = MIN(n, size-(int)i);
  int j;
  for(j=i; j < size-n; j++)
    El_Copy_(FastEl_(j), FastEl_(j+n));
  size -= n;
  DataChanged(DCR_ARY_SIZE_CHANGED);
  return true;
}

void taArray_impl::RemoveIdxOnly(int i) {
  for(int j=i; j < size-1; j++)
    El_Copy_(FastEl_(j), FastEl_(j+1));
  size -= 1;
}

const void* taArray_impl::SafeEl_(int i) const {
  i = Index(i);
  if (InRange(i)) return ((taArray_impl*)this)->FastEl_(i); //safe const cast
  else            return El_GetErr_();
}

void taArray_impl::Sort(bool descending) {
  if(size <= 1) return;
  int lt_compval = -1;          // comparison return value for less-than
  if(descending)
    lt_compval = 1;
  // lets do a heap sort since it requires no secondary storage
  int n = size;
  int l,j,ir,i;
  void* tmp = El_GetTmp_();

  l = (n >> 1)+1;
  ir = n;
  for(;;){
    if(l>1)
      El_Copy_(tmp,FastEl_(--l -1)); // tmp = ra[--l]
    else {
      El_Copy_(tmp,FastEl_(ir-1)); // tmp = ra[ir]
      El_Copy_(FastEl_(ir-1),FastEl_(0)); // ra[ir] = ra[1]
      if(--ir == 1) {
        El_Copy_(FastEl_(0),tmp); // ra[1]=tmp
        return;
      }
    }
    i=l;
    j=l << 1;
    while(j<= ir) {
      if(j<ir && (El_Compare_(FastEl_(j-1),FastEl_(j)) == lt_compval)) j++;
      if(El_Compare_(tmp,FastEl_(j-1)) == lt_compval) { // tmp < ra[j]
        El_Copy_(FastEl_(i-1),FastEl_(j-1)); // ra[i]=ra[j];
        j += (i=j);
      }
      else j = ir+1;
    }
    El_Copy_(FastEl_(i-1),tmp); // ra[i] = tmp;
  }
}

void taArray_impl::ShiftLeft(int nshift) {
  if (nshift <= 0) return;
  if (nshift >= size) {
    Reset();
    return;
  }

  int i;
  for(i=0; i < size - nshift; i++) {
    El_Copy_(FastEl_(i), FastEl_(i+nshift)); // move left..
  }
  size = size - nshift;         // update the size now..
  DataChanged(DCR_ARY_SIZE_CHANGED);
}

void taArray_impl::ShiftLeftPct(float pct) {
  int nshift = (int)(pct * (float)size);
  if(nshift == 0)    nshift = 1;
  ShiftLeft(nshift);
}

int taArray_impl::V_Flip(int width){
  if (size % width) return false;
  int size_orig = size;
  int from_end,from_start;
  for(from_end = size-width,from_start = 0;
      (from_start <from_end);
      from_end -= width, from_start += width){
    int j;
    for(j=0;j<width;j++){
      AddOnly_(FastEl_(from_start+j));
      El_Copy_(FastEl_(from_start+j),FastEl_(from_end+j));
      El_Copy_(FastEl_(from_end+j),FastEl_(size-1));
      RemoveIdxOnly(size-1);
    }
  }
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
  return true;
}

void taArray_impl::Duplicate(const taArray_impl& cp) {
  int i;
  int size_orig = size;
  for(i=0; i<cp.size; i++)
    AddOnly_(cp.FastEl_(i));
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
}

void taArray_impl::DupeUnique(const taArray_impl& cp) {
  int i;
  int size_orig = size;
  for(i=0; i<cp.size; i++)
    AddUniqueOnly_(cp.FastEl_(i));
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
}

void taArray_impl::Copy_Common(const taArray_impl& cp) {
  int i;
  int mx_idx = MIN(size, cp.size);
  for(i=0; i<mx_idx; i++)
    El_Copy_(FastEl_(i), cp.FastEl_(i));
}

void taArray_impl::Copy_Duplicate(const taArray_impl& cp) {
  Copy_Common(cp);
  int size_orig = size;
  for(int i=size; i<cp.size; i++)
    AddOnly_(cp.FastEl_(i));
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
}

void taArray_impl::CopyVals(const taArray_impl& from, int start, int end, int at) {
  if(end == -1) end = from.size;  else end = MIN(from.size, end);
  int len = end - start;
  if(size < at + len)
    SetSize(at + len);
  int i, trg;
  for(i=start, trg=at;i<end;i++, trg++)
    El_Copy_(FastEl_(trg), from.FastEl_(i));
}

String& taArray_impl::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << "[" << size << "] {";
  String_PArray strs;
  strs.Alloc(size);
  for(int i=0;i<size;i++) {
    strs.Add(El_GetStr_(FastEl_(i)));
  }
  taMisc::FancyPrintList(strm, strs, indent+1);
  taMisc::IndentString(strm, indent);
  strm << "}";
  return strm;
}

String taArray_impl::GetArrayStr() const {
  String rval = "[" + String(size) + "] {";
  for(int i=0;i<size;i++) {
    rval += " " + El_GetStr_(FastEl_(i)) + ",";
  }
  rval += "}";
  return rval;
}

void taArray_impl::InitFromString(const String& val) {
  int size_orig = size;
  String tmp = val;
  Reset_impl();
  tmp = tmp.after('{');         // find starting point
  while(tmp.length() > 0) {
    String el_val = tmp.before(',');
    if(el_val.empty()) {
      el_val = tmp.before('}');
      if (el_val.empty())
        break;
    }
    tmp = tmp.after(',');
    if (el_val.contains(' '))
      el_val = el_val.after(' ');
    AddOnly_(El_GetTmp_());             // add a blank
    El_SetFmStr_(FastEl_(size-1), String(el_val));
  }
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
}
