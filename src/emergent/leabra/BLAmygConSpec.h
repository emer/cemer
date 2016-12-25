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

#ifndef BLAmygConSpec_h
#define BLAmygConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(BLAmygConSpec);

class E_API BLAmygConSpec : public LeabraConSpec {
  // <describe here in full detail in one extended line comment>
INHERITED(LeabraConSpec)
public:

  TA_SIMPLE_BASEFUNS(BLAmygConSpec);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // BLAmygConSpec_h
