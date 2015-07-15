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

#ifndef NameValue_h
#define NameValue_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(NameValue);

class TA_API NameValue : public taNBase {
  // #INSTANCE #NO_TOKENS #NO_UPDATE_AFTER name/value pair
INHERITED(taNBase)
public:
  String 	value;			// Value for name

  TA_BASEFUNS(NameValue);
private:
  void 	Copy_(const NameValue& cp)		{ value = cp.value; }
  void  Initialize() {}
  void	Destroy() {}
};

#endif // NameValue_h
