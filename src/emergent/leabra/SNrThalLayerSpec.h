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

#ifndef SNrThalLayerSpec_h
#define SNrThalLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:
eTypeDef_Of(PBWMUnGpData);

eTypeDef_Of(SNrThalMiscSpec);

class E_API SNrThalMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the snrthal layer
INHERITED(SpecMemberBase)
public:
  float	    go_thr;			// #DEF_0.5 threshold on activity to fire go -- only stripes that get this active will fire
  int       min_cycle;      // #DEF_10:25 #MIN_0 minimum cycle for gating -- cannot gate before this cycle
  int       max_cycle;      // #DEF_20:40 #MIN_0 maximum cycle for gating -- cannot gate after this cycle
  
  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(SNrThalMiscSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(SNrThalLayerSpec);

class E_API SNrThalLayerSpec : public LeabraLayerSpec {
  // Represents the substantia nigra, pars reticulata (SNr) and Thalamus (MD) circuits that project from basal ganglia up to frontal cortex -- activation is directly computed from matrix -- all nogo enters into matrix activations, not snrthal -- gating val reflected in act_mid, gating status in unit group data per stripe
INHERITED(LeabraLayerSpec)
public:
  enum GatingTypes {		// #BITS types of gating stripes present, for INPUT, IN_MNT, OUTPUT, etc. gating -- used for coordinating structure of network (projections mostly) -- all gating is functionally identical
    NO_GATE_TYPE = 0x00,	// #NO_BIT no type set
    INPUT = 0x01,			// Gating of input to PFC_in layers -- if active, these are first units in SNrThal layer
    MNT = 0x02,		        // Gating of maintenance in PFC_mnt layers -- if active, these are next units in SNrThal layer 
    OUTPUT = 0x04,			// Gating of output in PFC_out layers -- these have to be in their own separate SNrThal layer, because output gating occurs at different time
    MNT_OUT = 0x08,			// Less commonly used: Gating of pre-output maintenance in PFC_mnt_out layers -- if active, these are after MNT and before OUTPUT in SNrThal layer
    OUT_MNT = 0x10,			// Less commonly used: Gating of a fixation-like rep in PFC_out_mnt layers -- if active, these are last units in SNrThal layer

    IN_MNT_OUT = INPUT | MNT | OUTPUT, // #NO_BIT input mnt output -- typical default
  };

  GatingTypes		gating_types;	// types of gating units present within this SNrThal layer -- used for coordinating structure of network (projections mostly) -- snrthal is the official "source" of this setting, which is copied to associated matrix and pfc layers during config check
  SNrThalMiscSpec	snrthal;    	// misc specs for snrthal layer

  virtual void	Init_GateStats(LeabraLayer* lay, LeabraNetwork* net);
  // initialize the gating stats in the group data -- called by Trial_Init_Layer
  virtual void	Compute_GateActs(LeabraLayer* lay, LeabraNetwork* net);
  // compute gating activations -- called every cycle
  virtual void	Compute_GateStats(LeabraLayer* lay, LeabraNetwork* net);
  // update layer user data gating statistics which are useful to monitor for overall performance -- called at gate_cycle

  virtual void	ResetMntCount(LeabraLayer* lay, int gp_idx);
  // resets gpd->mnt_count to -1 for a single stripe -- called by PFCLayerSpec when maintenance count has been exceeded

  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) override;
  void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) override;
  void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) override;
  void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net) override;
  void TI_ClearContext(LeabraLayer* lay, LeabraNetwork* net) override;

  // don't do any learning:
  bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net) override
  { return false; }
  bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) override
  { return false; }
  bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) override
  { return false; }

  virtual void	GatingTypesNStripes(LeabraLayer* lay, int& n_in, int& n_mnt, int& n_out, int& n_mnt_out, int& n_out_mnt);
  // get the number of stripes associated with each gating type, based on matrix projections into the snrthal layer -- also updates the gating_types to reflect actual connectivity
  virtual int  SNrThalStartIdx(LeabraLayer* lay, GatingTypes gating_type, 
			       int& n_in, int& n_mnt, int& n_out, int& n_mnt_out, 
			       int& n_out_mnt);
  // returns the starting index for a given gating type within the SNrThal, and also returns the number of each type of stripe.  returns -1 if snrthal does not have that kind of stripe

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);


  TypeDef* 	UnGpDataType()  override { return &TA_PBWMUnGpData; }

  TA_SIMPLE_BASEFUNS(SNrThalLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // SNrThalLayerSpec_h
