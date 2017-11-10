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

#include "LeabraExtraConSpecs.h"

#include <LeabraNetwork>
#include <LeabraUnitSpec>
#include <MSNCon>

#include <State_main>


///////////////////////////////////////////////////////////////////
//              DeepCtxt


#include "DeepCtxtConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(DeepCtxtConSpec);

void DeepCtxtConSpec::Initialize() {
  SetUnique("wt_scale", true);
  SetUnique("momentum", true);
  Initialize_core();
}

void DeepCtxtConSpec::Defaults_init() {
}

void DeepCtxtConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Ctxt_" + nm;
}

bool DeepCtxtConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraLayerSpec* rls = (LeabraLayerSpec*)rlay->GetMainLayerSpec();
  // LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
  
  if(rlay->CheckError(rls->decay.trial == 1.0f, quiet, rval,
                      "cannot have layer decay.trial = 1 when using temporal integration deep context learning -- this value will be set to 0 -- you can use .99f if you really want to")) {
    rls->decay.trial = 0.0f;
    rval = false;
  }

  LeabraLayer* slay = (LeabraLayer*)prjn->from.ptr();
  LeabraLayerSpec* sls = (LeabraLayerSpec*)slay->GetMainLayerSpec();
  if(slay->CheckError(sls->decay.trial == 1.0f, quiet, rval,
                      "cannot have layer decay.trial = 1 when using temporal integration deep context learning -- this value will be set to 0 -- you can use .99f if you really want to")) {
    sls->decay.trial = 0.0f;
    rval = false;
  }

  return rval;
}


///////////////////////////////////////////////////////////////////
//              SendDeepRaw, Mod

#include "SendDeepRawConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(SendDeepRawConSpec);

void SendDeepRawConSpec::Initialize() {
  SetUnique("learn", true);     // generally doesn't learn..
  Initialize_core();
}

void SendDeepRawConSpec::Defaults_init() {
}

void SendDeepRawConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Deep_Raw_" + nm;
}


#include "SendDeepModConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(SendDeepModConSpec);

void SendDeepModConSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void SendDeepModConSpec::Defaults_init() {
  SetUnique("learn", true);     // generally doesn't learn..
  learn = false;
  SetUnique("rnd", true);
  rnd.mean = 0.8f;
  rnd.var = 0.0f;
  SetUnique("wt_limits", true);
  wt_limits.sym = false;
}

void SendDeepModConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Deep_Mod_" + nm;
}


///////////////////////////////////////////////////////////////////
//              Marker, Special

#include "MarkerConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(MarkerConSpec);

void MarkerConSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void MarkerConSpec::Defaults_init() {
  SetUnique("rnd", true);
  rnd.mean = 0.0f; rnd.var = 0.0f;
  SetUnique("wt_limits", true);
  wt_limits.sym = false;
  SetUnique("wt_scale", true);
  wt_scale.rel = 0.0f;
  SetUnique("lrate", true);
  lrate = 0.0f;
  cur_lrate = 0.0f;
  SetUnique("learn", true);
  learn = false;
}

void MarkerConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Marker_" + nm;
}


#include "LeabraLimPrecConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(LeabraLimPrecConSpec);

void LeabraLimPrecConSpec::Initialize() {
  Initialize_core();
}


///////////////////////////////////////////////////////////////////
//              Diff learning: Delta, Hebb, Da

#include "LeabraDeltaConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(LeabraDeltaConSpec);

void LeabraDeltaConSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void LeabraDeltaConSpec::Defaults_init() {
  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  // SetUnique("wt_sig", true);
  // wt_sig.gain = 1.0f;  wt_sig.off = 1.0f;
}

#include "DaDeltaConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(DaDeltaConSpec);

void DaDeltaConSpec::Initialize() {
  Initialize_core();
}

void DaDeltaConSpec::Defaults_init() {
}


#include "DaHebbConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(DaHebbConSpec);

void DaHebbConSpec::Initialize() {
  Initialize_core();
}

void DaHebbConSpec::Defaults_init() {

}


TA_BASEFUNS_CTORS_DEFN(ChlSpecs);
TA_BASEFUNS_CTORS_DEFN(CHLConSpec);

void CHLConSpec::Initialize() {
  Initialize_core();
}


///////////////////////////////////////////////////////////////////
//              Special Algos: TD, PVLV, PBWM, Cereb

#include "TDRewPredConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(TDRewPredConSpec);

void TDRewPredConSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void TDRewPredConSpec::Defaults_init() {
  use_trace_act_avg = false;
  
  SetUnique("rnd", true);
  rnd.mean = 0.0f;
  rnd.var = 0.0f;

  SetUnique("wt_limits", true);
  wt_limits.type = WeightLimits::NONE;
  wt_limits.sym = false;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
  wt_sig.off = 1.0f;
  
  SetUnique("wt_bal", true);
  wt_bal.on = false;
}


////////////////////////////////////////////////////////////////////
//              PVLV -- Amyg


#include "LatAmygConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(LatAmygGains);
TA_BASEFUNS_CTORS_DEFN(LatAmygConSpec);

void LatAmygConSpec::Initialize() {
  Initialize_core();
}

void LatAmygConSpec::Defaults_init() {
}


#include "BasAmygConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(BasAmygLearnSpec);
TA_BASEFUNS_CTORS_DEFN(BasAmygConSpec);

void BasAmygConSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void BasAmygConSpec::Defaults_init() {
  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
}

bool BasAmygConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->GetMainUnitSpec();
  
  if(rlay->CheckError(!rus->InheritsFromName("BasAmygUnitSpec"), quiet, rval,
                      "requires receiving unit to use an BasAmygUnitSpec"))
    return false;
  return rval;
}


#include "BLAmygConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(BLAmygLearnSpec);
TA_BASEFUNS_CTORS_DEFN(BLAmygConSpec);

void BLAmygConSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void BLAmygConSpec::Defaults_init() {
  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
}

bool BLAmygConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->GetMainUnitSpec();
  
  if(rlay->CheckError(!rus->InheritsFromName("BLAmygUnitSpec"), quiet, rval,
                    "requires receiving unit to use an BLAmygUnitSpec"))
    return false;
  return rval;
}


#include "CElAmygConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(CElAmygLearnSpec);
TA_BASEFUNS_CTORS_DEFN(CElAmygConSpec);

void CElAmygConSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void CElAmygConSpec::Defaults_init() {
  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
}

bool CElAmygConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->GetMainUnitSpec();
  
  if(rlay->CheckError(!rus->InheritsFromName("CElAmygUnitSpec"), quiet, rval,
                    "requires receiving unit to use an CElAmygUnitSpec"))
    return false;
  return rval;
}


////////////////////////////////////////////////////////////////////
//              PVLV -- BG / VS

#include "MSNConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(MSNTraceSpec);
TA_BASEFUNS_CTORS_DEFN(MSNTraceThalLrates);
TA_BASEFUNS_CTORS_DEFN(MSNConSpec);

void MSNConSpec::Initialize() {
  min_obj_type = &TA_MSNCon;
  Defaults_init();
  Initialize_core();
}

void MSNConSpec::Defaults_init() {
  burst_da_gain = 1.0f;
  dip_da_gain = 1.0f;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;
  // SetUnique("lrate", true);
  lrate = 0.005f;
  learn_qtr = Q2_Q4;            // beta frequency default
  // use_unlearnable = false;
}

bool MSNConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->GetMainUnitSpec();
  
  if(rlay->CheckError(rus->GetStateSpecType() != LEABRA_NETWORK_STATE::T_MSNUnitSpec, quiet, rval,
                      "requires receiving unit to use an MSNUnitSpec"))
    return false;
  return rval;
}


////////////////////////////////////////////////////////////////////
//              Hippo

#include "HippoEncoderConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(HippoEncoderConSpec);

void HippoEncoderConSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void HippoEncoderConSpec::Defaults_init() {
}


////////////////////////////////////////////////////////////////////
//              Cerebellum

#include "CerebPfPcConSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(CerebPfPcConSpec);

void CerebPfPcConSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void CerebPfPcConSpec::Defaults_init() {
}
