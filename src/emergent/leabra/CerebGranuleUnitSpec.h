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

// notes on use of LeabraUnit variables for the Granule unit:
// act_lrn = time-lagged learning activation value -- this is what Purkinje cells use to learn from
// net_ctxt = start time when unit crosses act_thr threshold activation
// act_ctxt = max activation of unit within activity window

// UnitSpec functions:
// in Compute_NetinInteg or Compute_IThresh -- negate net input for inhiitory competition during "inhibitory refractory period"
// in Compute_Act or a sub-function, we implement act_lrn etc.


eTypeDef_Of(CerebGranuleUnitSpec);

class E_API CerebGranuleUnitSpec : public LeabraUnitSpec {
  // cerebellum granule unit -- enforces phasic temporal activity and provides lagged learning signals
INHERITED(LeabraUnitSpec)
public:
  CerebGranuleSpecs     cereb;  // specs for cerebellar granule units

  virtual void	Compute_GranLearnAct(LeabraUnit* u, LeabraNetwork* net,
                                     int thread_no=-1);
  // compute the granule unit learning activation as a function of time

  void	Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int       thread_no=-1) override;
  void	Compute_Act(Unit* u, Network* net, int thread_no=-1) override;

  TA_SIMPLE_BASEFUNS(CerebGranuleUnitSpec);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // CerebGranuleUnitSpec_h
