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

#ifndef CerebGranuleUnitSpec_h
#define CerebGranuleUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>
#include <LeabraUnitVars>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(CerebGranuleSpecs);

class E_API CerebGranuleSpecs : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra parameters for the cerebellar granule unit
INHERITED(SpecMemberBase)
public:
  float         act_thr;        // #DEF_0.5 activation threshold for initiating eligibility for learning and triggering the inhibitory window
  int           inhib_start_time; // #DEF_10 how long after unit crosses act_thr do we start inhibiting
  int           lrn_start_time;  // #DEF_60 starting time window (in cycles) for learning -- how long after activity crosses act_thr threshold does the learning window start?
  int           lrn_end_time;    // #DEF_90 ending time window (in cycles) for learning -- the learning window closes after this time
  float         inhib_net_pct;   // multiplier on net input to decrease it when a unit becomes inhibited -- going all the way to 0 reduces the effectiveness of FF_FB inhib..

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(CerebGranuleSpecs);
protected:
  SPEC_DEFAULTS;
  //  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

// notes on use of LeabraUnitVars variables for the Granule Unit:
// we use the stp parameters, and thus Granule units cannot have stp.on set!
// syn_nr = time-lagged learning activation value -- this is what Purkinje cells use to learn from
// syn_pr = time since the unit crossed the act_thr threshold activation
// syn_kre = max activation of unit within activity window 

// UnitSpec functions:
// in Compute_NetinExtras -- negate net input for inhibitory competition during "inhibitory refractory period"
// in Compute_Act, we implement delayed activation for learning


eTypeDef_Of(CerebGranuleUnitSpec);

class E_API CerebGranuleUnitSpec : public LeabraUnitSpec {
  // cerebellum granule unit -- enforces phasic temporal activity and provides lagged learning signals
INHERITED(LeabraUnitSpec)
public:
  CerebGranuleSpecs     cereb;  // specs for cerebellar granule units

 inline float& ActLag(LeabraUnitVars* uv)
 { return uv->syn_nr; }
 // #CAT_CerebGranule time-lagged activation value, used for learning
 inline float& TimeSinceThr(LeabraUnitVars* uv)
 { return uv->syn_pr; }
 // #CAT_CerebGranule get time since unit crossed the act_thr threshold activation
 inline float& ActMax(LeabraUnitVars* uv)
 { return uv->syn_kre; }
 // #CAT_CerebGranule get max activation of unit within activity window
 
  virtual void	Compute_GranLearnAct(LeabraUnitVars* u, LeabraNetwork* net,
                                     int thr_no);
  // compute the granule unit learning activation as a function of time

  float	Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net, int thr_no,
                            float& net_syn) override;
  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  bool CheckConfig_Unit(Unit* un, bool quiet);

  TA_SIMPLE_BASEFUNS(CerebGranuleUnitSpec);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // CerebGranuleUnitSpec_h
