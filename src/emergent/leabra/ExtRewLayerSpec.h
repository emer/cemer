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

#ifndef ExtRewLayerSpec_h
#define ExtRewLayerSpec_h 1

// parent includes:
#include <ScalarValLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(OutErrSpec);

class E_API OutErrSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for computing external rewards based on output performance of network
INHERITED(SpecMemberBase)
public:
  float		err_tol;	// #DEF_0.5 error tolerance for counting an activation wrong
  bool		graded;		// #DEF_false compute a graded reward signal as a function of number of correct output values
  bool		no_off_err;	// #DEF_false do not count a unit wrong if it is off but target says on -- only count wrong units that are on but should be off
  float		scalar_val_max;	// #CONDEDIT_ON_graded maximum value for scalar value output layers when using a graded value -- reward is error normalized by this value, and clipped at min/max

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(OutErrSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(ExtRewSpec);

class E_API ExtRewSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for computing external rewards
INHERITED(SpecMemberBase)
public:
  float		err_val;	// #DEF_0 reward value for errors (when network does not respond correctly)
  float		norew_val;	// #DEF_0.5 reward value when no feedback information is present
  float		rew_val;	// #DEF_1 reward value for correct responses (positive rewards)

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(ExtRewSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(ExtRewLayerSpec);

class E_API ExtRewLayerSpec : public ScalarValLayerSpec {
  // computes external reward feedback: minus phase is zero, plus phase is reward value derived from network performance or other inputs (computed at start of 1+)
INHERITED(ScalarValLayerSpec)
public:
  enum RewardType {		// how do we get the reward values?
    OUT_ERR_REW,		// get rewards as a function of errors on the output layer ONLY WHEN RewTarg layer act > .5 -- get from markerconspec from output layer(s)
    EXT_REW,			// get rewards as external inputs marked as ext_flag = TARG to the first unit in the layer (if ext val == norew_val, then no ext rew signaled)
    DA_REW			// get rewards from da values on first unit in layer (if ext val == norew_val, then no ext rew signaled)
  };

  RewardType	rew_type;	// how do we get the reward values?
  OutErrSpec	out_err;	// #CONDEDIT_ON_rew_type:OUT_ERR_REW how to compute external rewards based on output performance
  ExtRewSpec	rew;		// misc reward computation specifications


  virtual void Compute_Rew(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ExtRew overall compute reward function: calls appropriate sub-function based on rew_type
    virtual void Compute_OutErrRew(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ExtRew get reward value based on external error (put in da val, clamp)
      virtual bool OutErrRewAvail(LeabraLayer* lay, LeabraNetwork* net);
      // #CAT_ExtRew figure out if reward is available on this trial (look if target signals are present)
      virtual float GetOutErrRew(LeabraLayer* lay, LeabraNetwork* net);
      // #CAT_ExtRew get reward value based on error at layer with MarkerConSpec connection: 1 = rew (correct), 0 = err, -1 = no info
    virtual void Compute_ExtRew(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ExtRew get external rewards from inputs (put in da val, clamp)
    virtual void Compute_DaRew(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ExtRew clamp external rewards as da values (put in da val, clamp)

    virtual void Compute_UnitDa
      (LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx,
       float er, LeabraUnit* u);
    // #CAT_ExtRew used in above routines: actually compute the unit da value based on external reward value er
    virtual void Compute_NoRewAct(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ExtRew used in above routines: clamp norew_val values for when no reward information is present

  // overrides:
  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(ExtRewLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // ExtRewLayerSpec_h
