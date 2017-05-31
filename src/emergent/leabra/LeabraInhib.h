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

#ifndef LeabraInhib_h
#define LeabraInhib_h 1

// parent includes:

// member includes
#include <AvgMaxVals>

// declare all other types mentioned but not required to include:
class LeabraLayerSpec; // 

eTypeDef_Of(LeabraInhibVals);

class E_API LeabraInhibVals : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds values for computed inhibition
INHERITED(taOBase)
public:
  float         ffi;            // computed feedforward inhibition
  float         fbi;            // computed feedback inhibition (total)
  float		g_i;		// overall value of the inhibition -- this is what is added into the unit g_i inhibition level (along with any synaptic unit-driven inhibition)
  float		g_i_orig; 	// original value of the inhibition (before any layer group effects set in)
  float		lay_g_i;	// for unit groups, this is the layer-level inhibition that is MAX'd with the unit-group level inhibition to produce the net inhibition, if unit_gp_inhib is on

  void          InitVals();
  // initialize the inhibition values

  String       GetTypeDecoKey() const override { return "Layer"; }

  void	Init() 	{ Initialize(); }
  void	Copy_(const LeabraInhibVals& cp);
  TA_BASEFUNS(LeabraInhibVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(LeabraInhib);

class E_API LeabraInhib {
  // ##CAT_Leabra holds inhibition computation values, used as a parent class for layers and UnitGr etc
public:
  float 	acts_m_avg;	// #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS time-averaged minus-phase activation stats for the layer -- this is used for adaptive inhibition tuning (inhib_adapt in LeabraLayerSpec) -- time constant in layer spec avg_act.tau and initialized to avg_act.init -- saved with weights
  float 	acts_p_avg;	// #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS time-averaged plus-phase activation stats for the layer -- time constant in layer spec avg_act.tau and initialized to avg_act.init -- this is used for netinput scaling (via _eff version) and should match reasonably well with act_avg.init value -- saved with weights
  float 	acts_p_avg_eff;	// #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS acts_p_avg * avg_act.adjust factor -- this is the effective value actually used for netinput scaling based on layer activation levels -- saved with weights
  LeabraInhibVals i_val;        // #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation computed inhibitory values
  AvgMaxVals	netin;		// #NO_SAVE #READ_ONLY #SHOW #CAT_Activation average, maximum net input values for the layer
  AvgMaxVals	netin_raw;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum raw net input values for the layer (synaptic netinput only -- no extras)
  AvgMaxVals	acts;		// #NO_SAVE #READ_ONLY #SHOW #CAT_Activation average, maximum activation values for the layer
  AvgMaxVals	acts_eq;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum act_eq values for the layer
  AvgMaxVals	acts_raw;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum act_raw values for the layer
  AvgMaxVals	acts_prvq;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum act_eq values for the layer, for the previous quarter (updated every quarter)
  AvgMaxVals	acts_m;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum minus-phase activation stats for the layer (from acts_eq at end of minus phase)
  AvgMaxVals	acts_p;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum plus-phase activation stats for the layer (from acts_eq at end of plus phase)
  AvgMaxVals	acts_q0;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum previous-trial activation stats for the layer (from acts_p at start of new trial)
  AvgMaxVals	un_g_i;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum unit total inhibitory conductance values (optionally updated -- use for unit inhibition case)
  AvgMaxVals    am_deep_raw;    // #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum deep_raw values for the layer
  AvgMaxVals    am_deep_raw_net;  // #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum deep_raw_net values for the layer
  AvgMaxVals    am_deep_mod_net; // #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum deep_mod_net values for the layer
  AvgMaxVals    am_deep_ctxt;    // #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum deep_ctxt values for the layer

  void	Inhib_SetVals(float val)	{ i_val.g_i = val; i_val.g_i_orig = val; }
  // set computed inhibition values -- sets original too
  void	Inhib_Init_Acts(LeabraLayerSpec* lay);
  void	Inhib_Initialize();
  void	Inhib_Copy_(const LeabraInhib& cp);
};

#endif // LeabraInhib_h
