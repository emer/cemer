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

#include "MembSet.h"

MembSet_List::~MembSet_List() {
  Reset();
}

bool MembSet_List::GetFlatDataItem(int idx, MemberDef** mbr, taiData** dat) {
  for (int i = 0; i < size; ++i) {
    MembSet* ms = FastEl(i);
    int msd_size = ms->data_el.size;
    if (idx >= msd_size) {
      idx -= msd_size;
      continue;
    }
    if (idx < msd_size) {
      if (mbr) *mbr = ms->memb_el.SafeEl(idx);
      if (dat) *dat = ms->data_el.SafeEl(idx); // supposed to be 1:1, but safer
      return true;
    }
    break; // out of range
  }
  return false;
}

int MembSet_List::GetFlatDataIndex(taiData* dat) {
  if (!dat) return -1;
  int rval = 0;
  for (int i = 0; i < size; ++i) {
    MembSet* ms = FastEl(i);
    int ti_set = ms->data_el.FindEl(dat);
    if (ti_set >= 0) {
      return (rval + ti_set);
    } else {
      rval += ms->data_el.size;
    }
  }
  return -1;
}

int MembSet_List::GetFlatDataIndex(MemberDef* mbr, taBase* base) {
  if (!mbr || !base) return -1;
  int rval = 0;
  for (int i = 0; i < size; ++i) {
    MembSet* ms = FastEl(i);
    for (int j = 0; j < ms->data_el.size; ++j, ++rval) {
      if (mbr != ms->memb_el.PosSafeEl(j)) continue;
      if (ms->data_el.FastEl(j)->Base() == base) return rval;
    }
  }
  return -1;
}

int MembSet_List::GetDataSize() const {
  int rval = 0;
  for (int i = 0; i < size; ++i) {
    MembSet* ms = FastEl(i);
    rval += ms->data_el.size;
  }
  return rval;
}

void MembSet_List::ResetItems(bool data_only) {
  for (int i = size - 1; i >= 0; --i) {
    MembSet* ms = FastEl(i);
    ms->data_el.Reset();
    if (!data_only) ms->memb_el.Reset();
  }
}

void MembSet_List::SetMinSize(int n) {
  if (n < 0) return;
  Alloc(n); // noop if already sized larger
  while (n > size) {
    Add(new MembSet);
  }
}

