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

#include "taFixedArray_impl.h"

#include <taMisc>

void taFixedArray_impl::Add_(const void* it) {
  if(!Alloc_(size + 1)) return;
  El_Copy_(FastEl_(size++), it);
}

bool taFixedArray_impl::AddUnique_(const void* it) {
  if (Find_(it) >= 0)
    return false;
  Add_(it);
  return true;
}

bool taFixedArray_impl::Alloc_(uint alloc) {
  char* nw = (char*)MakeArray_(alloc);
  if(!nw) {
    taMisc::Error("taFixedArray_impl::Alloc_ -- malloc error -- array is too big -- could be fatal!");
    return false;
  }
  for (int i = 0; i < size; ++i) {
    El_Copy_(nw + (El_SizeOf_() * i), FastEl_(i));
  }
  SetArray_(nw);
  return true;
}

void taFixedArray_impl::Copy_(const taFixedArray_impl& cp) {
  if (cp.size < size) ReclaimOrphans_(cp.size, size - 1);
  else if(!Alloc_(cp.size)) return;

  for (int i=0; i < cp.size; ++i) {
    El_Copy_(FastEl_(i), cp.FastEl_(i));
  }
  size = cp.size;
}

void taFixedArray_impl::SetSize(int new_size) {
  if (new_size < 0) new_size = 0;
  if (new_size > size) {
    if(!Alloc_(new_size)) return;
    const void* blank = El_GetBlank_();
    for (int i = size; i < new_size; ++i) {
      El_Copy_(FastEl_(i), blank);
    }
  } else if (new_size < size) {
    ReclaimOrphans_(new_size, size - 1);
  }
  size = new_size;
}

bool taFixedArray_impl::Equal_(const taFixedArray_impl& src) const {
  //NOTE: this should only be called if src is same type as us (use dynamic_cast)
  if (size != src.size) return false;
  for (int i = 0; i < size; ++i) {
    if (!El_Equal_(FastEl_(i), src.FastEl_(i))) return false;
  }
  return true;
}

int taFixedArray_impl::Find_(const void* it, int where) const {
  int i;
  for (i=where; i<size; i++) {
    if (El_Equal_(it, FastEl_(i)))
      return i;
  }
  return -1;
}

void taFixedArray_impl::InitVals_(const void* it, int start, int end) {
  if (end == -1) end = size;
  else end = MIN(size, end);
  for (int i = start; i < end; ++i) {
    El_Copy_(FastEl_(i), it);
  }
}

void taFixedArray_impl::Insert_(const void* it, int where, int n) {
  if ((where > size) || (n <= 0)) return; // errors
  if (where < 0) where = size; // -1 means at end
  if(!Alloc_(size + n)) return; // pre-add stuff

  int i;
  // if not appending, move the items
  if ((where < size)) {
    int n_mv = size - where;    // number that must be moved
    for (i = size - 1; i >= where; --i) {       // shift everyone over
      El_Copy_(FastEl_(i + n_mv), FastEl_(i));
    }
  }
  // write the new items
  for (i=where; i < where + n; ++i)
    El_Copy_(FastEl_(i), it);
  size += n;
}

const void* taFixedArray_impl::SafeEl_(int i) const {
  i=Index(i);
  if (InRange(i)) return FastEl_(i);
  else            return El_GetErr_();
}
