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

#ifndef BasAmygUnitSpec_h
#define BasAmygUnitSpec_h 1

// parent includes:
#include <D1D2UnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(BasAmygUnitSpec);

class E_API BasAmygUnitSpec : public D1D2UnitSpec {
  // Basal Amygdala units -- specifies the dopamine receptor subtypes
INHERITED(D1D2UnitSpec)
public:
  Valence       valence;        // US valence coding -- positive or negative US's

  TA_SIMPLE_BASEFUNS(BasAmygUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // BasAmygUnitSpec_h
