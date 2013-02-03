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

#ifndef SRAvgCaiSynDepCon_h
#define SRAvgCaiSynDepCon_h 1

// parent includes:
#include <LeabraSRAvgCon>

// member includes:

// declare all other types mentioned but not required to include:

// todo: could inherit from CaiSynDepCon/Spec and probably save a lot of code, but sravg guys might be more difficult -- try that later

TypeDef_Of(SRAvgCaiSynDepCon);

class LEABRA_API SRAvgCaiSynDepCon : public LeabraSRAvgCon {
  // send-recv average at the connection level learning in XCal, combined with synaptic depression connection at the cycle level, based on synaptic integration of calcium
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (subject to synaptic depression) -- used for sending activation
  float		cai;		// #NO_SAVE intracellular postsynaptic calcium current integrated over cycles, used for synaptic depression

  SRAvgCaiSynDepCon() { effwt = 0.0f; cai = 0.0f; }
};

#endif // SRAvgCaiSynDepCon_h
