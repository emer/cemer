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

#ifndef LeabraInhib_h
#define LeabraInhib_h 1

// parent includes:

// member includes
#include <AvgMaxVals>

// declare all other types mentioned but not required to include:
class LeabraLayerSpec; // 

eTypeDef_Of(LeabraInhibVals);

class E_API LeabraInhibVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds values for computed inhibition
INHERITED(taOBase)
public:
  float         ffi;            // computed feedforward inhibition
  float         fbi;            // computed feedback inhibition (total)
  float		g_i;		// overall value of the inhibition -- this is what is added into the unit g_i inhibition level (along with any synaptic unit-driven inhibition)
  float		g_i_orig; 	// original value of the inhibition (before any layer group effects set in)
  float		lay_g_i;	// for unit groups, this is the layer-level inhibition that is MAX'd with the unit-group level inhibition to produce the net inhibition, if unit_gp_inhib is on
  float		laygp_g_i;	// for layers, this is the layer-group-level inhibition that is MAX'd with the layer-level inhibition to produce the net layer-level inhibition, if lay_gp_inhib is on

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
  float 	acts_m_avg;	// #READ_ONLY #SHOW #CAT_Activation time-averaged minus-phase activation stats for the layer -- time constant in layer spec avg_act.tau and initialized to avg_act.init -- this is used for netinput scaling (via _eff version) and should match reasonably well with act_avg.init value
  float 	acts_m_avg_eff;	// #READ_ONLY #SHOW #CAT_Activation acts_m_avg * avg_act.adjust factor -- this is the effective value actually used for netinput scaling based on layer activation levels
  float 	acts_p_avg;	// #READ_ONLY #SHOW #CAT_Activation time-averaged plus-phase activation stats for the layer -- time constant in layer spec avg_act.tau and initialized to avg_act.init
  LeabraInhibVals i_val;        // #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation computed inhibitory values
  AvgMaxVals	netin;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum net input values for the layer
  AvgMaxVals	acts;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum activation values for the layer
  AvgMaxVals	acts_eq;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum act_eq values for the layer
  AvgMaxVals	acts_m;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum minus-phase activation stats for the layer (from acts_eq at end of minus phase)
  AvgMaxVals	acts_p;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum plus-phase activation stats for the layer (from acts_eq at end of plus phase)
  AvgMaxVals	nets_ctxt;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum TI net_ctxt stats for the layer
  AvgMaxVals	un_g_i;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation average, maximum unit total inhibitory conductance values (optionally updated -- use for unit inhibition case)

  void	Inhib_SetVals(float val)	{ i_val.g_i = val; i_val.g_i_orig = val; }
  // set computed inhibition values -- sets original too
  void	Inhib_Init_Acts(LeabraLayerSpec* lay);
  void	Inhib_Initialize();
  void	Inhib_Copy_(const LeabraInhib& cp);
};

#endif // LeabraInhib_h
