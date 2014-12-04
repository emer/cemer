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

#ifndef LayerActUnitSpec_h
#define LayerActUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LayerActUnitSpec);

class E_API LayerActUnitSpec : public LeabraUnitSpec {
  // Layer-driven activation unit spec -- use this for any layer that computes activation values at the layer-level, instead of using the usual net input, currents etc -- saves on computational costs by negating most functions
INHERITED(LeabraUnitSpec)
public:
  void	Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_ApplyInhib
    (LeabraUnitVars* uv, LeabraNetwork* net, int thr_no, LeabraLayerSpec* lspec,
     LeabraInhib* thr, float ival) override { };
  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override { };

  TA_SIMPLE_BASEFUNS(LayerActUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize() 		{ };
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

#endif // LayerActUnitSpec_h
