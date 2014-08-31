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

// member includes:
#include <LeabraSort>
#include <AvgMaxVals>

// declare all other types mentioned but not required to include:
class LeabraLayerSpec; // 

eTypeDef_Of(KWTAVals);

class E_API KWTAVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds values for kwta stuff
INHERITED(taOBase)
public:
  int		k;       	// target number of active units for this collection
  float		pct;		// actual percent activity in group
  float		pct_c;		// #HIDDEN complement of (1.0 - ) actual percent activity in group
  float		k_ithr;		// inhib threshold for k unit (top k for kwta_avg)
  float		k1_ithr;	// inhib threshold for k+1 unit (other units for kwta_avg)
  float		ithr_diff;	// normalized difference ratio for k vs k+1 ithr values: (k_ithr - k1_ithr) / k_ithr
  float         ffi;            // for FF_FB_INHIB, the amount of feedforward inhibition
  float         fbi;            // for FF_FB_INHIB, the amount of feedback inhibition (total)
  float         prv_trl_ffi;    // for FF_FB_INHIB, the amount of feedforward inhibition on the previous trial
  float         prv_phs_ffi;    // for FF_FB_INHIB, the amount of feedforward inhibition on the previous phase

  void		Compute_Pct(int n_units);
  void          InitVals();
  // initialize various state vals

  String       GetTypeDecoKey() const override { return "Layer"; }

  void	Copy_(const KWTAVals& cp);
  TA_BASEFUNS(KWTAVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(InhibVals);

class E_API InhibVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds values for inhibition
INHERITED(taOBase)
public:
  float		kwta;		// inhibition due to kwta function
  float		g_i;		// overall value of the inhibition
  float		gp_g_i;		// g_i from the layer or unit group, if applicable
  float		g_i_orig; 	// original value of the inhibition (before any layer group effects set in)

  String       GetTypeDecoKey() const override { return "Layer"; }

  void	Init() 	{ Initialize(); }
  void	Copy_(const InhibVals& cp);
  TA_BASEFUNS(InhibVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(LeabraInhib);

class E_API LeabraInhib {
  // ##CAT_Leabra holds threshold-computation values, used as a parent class for layers, etc
public:
  LeabraSort 	active_buf;	// #NO_SAVE #HIDDEN #CAT_Activation list of active units
  LeabraSort 	inact_buf;	// #NO_SAVE #HIDDEN #CAT_Activation list of inactive units

  AvgMaxVals	netin;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation net input values for the layer
  AvgMaxVals	acts;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation activation values for the layer
  AvgMaxVals	acts_p;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation plus-phase activation stats for the layer
  AvgMaxVals	acts_m;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation minus-phase activation stats for the layer
  AvgMaxVals	acts_m_avg;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation time-averaged minus-phase activation stats for the layer
  AvgMaxVals	acts_ctxt;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation TI act_ctxt activation stats for the layer
  AvgMaxVals	acts_mid;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation mid-minus phase activations -- only used in hippocampus at this point (possibly also in PBWM)
  float		phase_dif_ratio; // #NO_SAVE #READ_ONLY #SHOW #CAT_Activation phase-difference ratio (acts_m.avg / acts_p.avg)
 
  KWTAVals	kwta;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation values for kwta -- activity levels, etc NOTE THIS IS A COMPUTED VALUE: k IS SET IN LayerSpec!
  InhibVals	i_val;		// #NO_SAVE #READ_ONLY #SHOW #CAT_Activation inhibitory values computed by kwta
  AvgMaxVals	un_g_i;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation unit inhib values (optionally computed)
  float		maxda;		// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic maximum change in activation (delta-activation) over network; used in stopping settling

  void	Inhib_SetVals(float val)	{ i_val.g_i = val; i_val.g_i_orig = val; }
  void	Inhib_ResetSortBuf() 		{ active_buf.size = 0; inact_buf.size = 0; }
  void	Inhib_Init_Acts(LeabraLayerSpec* lay);
  void	Inhib_Initialize();
  void	Inhib_Copy_(const LeabraInhib& cp);
};

#endif // LeabraInhib_h
