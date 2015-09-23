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

#ifndef D1D2UnitSpec_h
#define D1D2UnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(D1D2UnitSpec);

class E_API D1D2UnitSpec : public LeabraUnitSpec {
  // Medium Spiny Neuron, which is principal cell of the striatum -- determines the dopamine receptor type and patch / matrix specialization of the neuron, and overall anatomical location (dorsal / ventral), which are then used by the D1D2ConSpec and other areas in determining learning and other dynamics
INHERITED(LeabraUnitSpec)
public:
  enum DAReceptor {             // type of dopamine receptor expressed
    D1R,                        // Primarily expresses Dopamine D1 Receptors -- dopamine is excitatory and bursts of dopamine lead to increases in synaptic weight, while dips lead to decreases -- direct pathway in dorsal striatum
    D2R,                        // Primarily expresses Dopamine D2 Receptors -- dopamine is inhibitory and bursts of dopamine lead to decreases in synaptic weight, while dips lead to increases -- indirect pathway in dorsal striatum
  };

  enum Valence {                // appetitive vs. aversive valence US-coding (VENTRAL only)
    APPETITIVE,                 // has an appetitive (positive valence) US coding
    AVERSIVE,                   // has an aversive (negative valence) US coding
  };

  DAReceptor            dar;            // type of dopamine receptor: D1 vs. D2
  
  TA_SIMPLE_BASEFUNS(D1D2UnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // D1D2UnitSpec_h
