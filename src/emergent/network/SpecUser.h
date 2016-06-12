// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef SpecUser_h
#define SpecUser_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class BaseSpec; //

eTypeDef_Of(SpecUser);

class E_API SpecUser : public taNBase {
  // Base class of an object that uses specs -- something that a spec can be applied to -- this is sometimes not the object that the spec actually operates on (e.g., UnitSpec is applied to Layers but operates on Units -- the SpecUser is the Layer in this case)
INHERITED(taNBase)
public:

  virtual bool  ApplySpecToMe(BaseSpec* spec) { return false; }
  // this is the one key method that must be overwritten in each base case to call the proper method to apply the given spec to this object -- returns true upon success, false for failure
  
  TA_BASEFUNS_NOCOPY(SpecUser);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // SpecUser_h
