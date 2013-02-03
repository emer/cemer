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

#ifndef CycleSynDepCon_h
#define CycleSynDepCon_h 1

// parent includes:
#include <LeabraCon>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(CycleSynDepCon);

class LEABRA_API CycleSynDepCon : public LeabraCon {
  // synaptic depression connection at the cycle level (as opposed to the trial level) -- this is the simpler version -- Ca_i based version below
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (subject to synaptic depression) -- used for sending activation

  CycleSynDepCon() { effwt = 0.0f; }
};

#endif // CycleSynDepCon_h
