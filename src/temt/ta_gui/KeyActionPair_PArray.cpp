// Copyright 2017, Regents of the University of Colorado,
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

#include "KeyActionPair_PArray.h"

#include <taiMisc>

int KeyActionPair_PArray::FindKeySequence(QKeySequence key_sequence, int start) const {
  if (key_sequence.isEmpty()) {
    return -1;
  }
  
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-start; i>=0; i--) {
      if(FastEl(i).key_sequence == key_sequence)
        return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).key_sequence == key_sequence)
        return i;
    }
  }
  return -1;
}

int KeyActionPair_PArray::FindAction(taiMisc::BoundAction action, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-start; i>=0; i--) {
      if(FastEl(i).action == action)
        return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).action == action)
        return i;
    }
  }
  return -1;
}

int KeyActionPair_PArray::FindPair(taiMisc::BoundAction action, QKeySequence key_sequence, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-start; i>=0; i--) {
      if((FastEl(i).action == action) && (FastEl(i).key_sequence == key_sequence))
        return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).action == action)
        return i;
    }
  }
  return -1;
}

KeyActionPair* KeyActionPair_PArray::GetPair(taiMisc::BoundAction action, QKeySequence key_sequence, int start) const {
  int idx = FindPair(action, key_sequence, start);
  if (idx != -1) {
    return &SafeEl(idx);
  }
  else {
    return NULL;
  }
}

KeyActionPair* KeyActionPair_PArray::GetPairFromAction(taiMisc::BoundAction action, int start) const {
  int idx = FindAction(action, start);
  if (idx != -1) {
    return &SafeEl(idx);
  }
  else {
    return NULL;
  }
}


taiMisc::BoundAction KeyActionPair_PArray::GetAction(QKeySequence key_sequence) {
  int idx = FindKeySequence(key_sequence);
  if(idx < 0)
    return taiMisc::NULL_ACTION;
  return FastEl(idx).action;
}

QKeySequence KeyActionPair_PArray::GetKeySequence(taiMisc::BoundAction action) {
  int idx = FindAction(action);
  if(idx < 0)
    return QKeySequence("");
  return FastEl(idx).key_sequence;
}
