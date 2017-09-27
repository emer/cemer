// Copyright 2017, Regents of the University of Colorado,
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

#ifndef LeabraConSpec_core_h
#define LeabraConSpec_core_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraConSpec_core);

class E_API LeabraConSpec_core : public taNBase {
  // <describe here in full detail in one extended line comment>
INHERITED(taNBase)
public:

  TA_SIMPLE_BASEFUNS(LeabraConSpec_core);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // LeabraConSpec_core_h
