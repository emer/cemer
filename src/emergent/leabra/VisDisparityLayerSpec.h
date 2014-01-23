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

#ifndef VisDisparityLayerSpec_h
#define VisDisparityLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(VisDispLaySpec);

class E_API VisDispLaySpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra visual disparity layer specs
INHERITED(taOBase)
public:
  bool		max_l;		// #DEF_true compute left = max(weighted left inputs)
  bool		incl_other_res;	// #DEF_true include other resolution layers that are connected to this one -- looks for VisDisparityLayerSpec layers and multiplies by their misc_1 values
  int		updt_cycles;	// #DEF_-1:5 (-1=always) how many cycles to compute updates for -- computation is expensive so if inputs are static, don't keep computing beyond what is necessary

  // todo: add stuff about horiz thign

  TA_SIMPLE_BASEFUNS(VisDispLaySpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(VisDisparityLayerSpec);

class E_API VisDisparityLayerSpec : public LeabraLayerSpec {
  // visual disparity layer spec: receiving layer units within groups encode different offset disparities (near..far) from two sending layers (first prjn MUST be VisDisparityPrjnSpec from right eye, second MUST be same spec type from left eye -- right is just one-to-one dominant driver) -- this layerspec computes sqrt(left*right) activation into ext of units, which can be added into netinput if !clamp.hard, or activation forced to this value otherwise -- also manages horizontal apeture problem -- use MarkerConSpec for these prjs to prevent computation otherwise
INHERITED(LeabraLayerSpec)
public:
  VisDispLaySpec	disp;	// disparity computation specs

  virtual void	ComputeDispToExt(LeabraLayer* lay, LeabraNetwork* net);
  // main function: compute disparity values into ext in units

  // these are two entry points for applying ext inputs depending on clamp.hard status
  void	Compute_ExtraNetin(LeabraLayer* lay, LeabraNetwork* net) override;
  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) override;

  bool  CheckConfig_Layer(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(VisDisparityLayerSpec);
// protected:
//   void UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // VisDisparityLayerSpec_h
