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

#ifndef KeyActionPair_PArray_h
#define KeyActionPair_PArray_h 1

// parent includes:
#include <taPlainArray>

// member includes:
#include <KeyActionPair>
#include <taiMisc>

// declare all other types mentioned but not required to include:

taTypeDef_Of(KeyActionPair_PArray);

class TA_API KeyActionPair_PArray : public taPlainArray<KeyActionPair> {
  // #NO_TOKENS a plain-array of key_sequence and action pairs
  INHERITED(taPlainArray<KeyActionPair>)
  
public:
  int   FindKeySequence(QKeySequence key_sequence, int start=0) const;
  // find by key sequence  (start < 0 = from end)
  int   FindAction(taiMisc::BoundAction act, int start=0) const;
  // #IGNORE find by action (start < 0 = from end)
  int   FindPair(taiMisc::BoundAction act, QKeySequence key_sequence, int start=0) const;
  // #IGNORE find the pair that matches on action and on sequence
  KeyActionPair*   GetPair(taiMisc::BoundAction act, QKeySequence key_sequence, int start=0) const;
  // #IGNORE find the pair that matches on action and on sequence
  KeyActionPair*   GetPairFromAction(taiMisc::BoundAction act, int start=0) const;
  // #IGNORE find the first pair that matches on action

  taiMisc::BoundAction GetAction(QKeySequence key_sequence);
  // #IGNORE get action from key_sequence; isNull if not found
  QKeySequence GetKeySequence(taiMisc::BoundAction action);
  // #IGNORE get key_sequence from the action; Null if not found

  void  operator=(const KeyActionPair_PArray& cp)           { Copy_Duplicate(cp); }
  KeyActionPair_PArray()                                    { };
  KeyActionPair_PArray(const KeyActionPair_PArray& cp)      { Copy_Duplicate(cp); }

protected:
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(((KeyActionPair*)a)->action > ((KeyActionPair*)b)->action) rval=1; else if(((KeyActionPair*)a)->action == ((KeyActionPair*)b)->action) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
  { return (((KeyActionPair*)a) == ((KeyActionPair*)b)); }
};

#endif // KeyActionPair_PArray_h
