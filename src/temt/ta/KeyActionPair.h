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

#ifndef KeyActionPair_h
#define KeyActionPair_h 1

// parent includes:

// member includes:
#include <taString>
#include <taiMisc>

// declare all other types mentioned but not required to include:

taTypeDef_Of(KeyActionPair);

class TA_API KeyActionPair {
  // A key sequence paired with an enum representing a an action
  
public:
  taiMisc::BoundAction      action; // #IGNORE
  QKeySequence              key_sequence; //
  
  // only compare the action
  bool	operator>(const KeyActionPair& kap) { return action > kap.action; }
  bool	operator<(const KeyActionPair& kap) { return action < kap.action; }
  bool	operator>=(const KeyActionPair& kap) { return action >= kap.action; }
  bool	operator<=(const KeyActionPair& kap) { return action <= kap.action; }
  // for equals compare the action and the key sequence
  bool	operator==(const KeyActionPair& kap) { return ((action == kap.action) && (key_sequence == kap.key_sequence)); }
  
  KeyActionPair(const KeyActionPair& cp) { action = cp.action; key_sequence = cp.key_sequence; }
  KeyActionPair(QKeySequence a_key_sequence, taiMisc::BoundAction act) { key_sequence = a_key_sequence; action = act; }
  KeyActionPair()	{ };
  ~KeyActionPair()	{ };
};

#endif // KeyActionPair_h
