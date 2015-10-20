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

#include "ECoutUnitSpec.h"
#include <LeabraNetwork>
#include <MarkerConSpec>

TA_BASEFUNS_CTORS_DEFN(ECoutUnitSpec);

void ECoutUnitSpec::Initialize() {
  Defaults_init();
}

void ECoutUnitSpec::Defaults_init() {

}

bool ECoutUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  bool rval = true;

  bool got_ec_in = false;
  const int nrg = un->NRecvConGps(); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)un->RecvConGroup(g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(cs->InheritsFrom(&TA_MarkerConSpec) && recv_gp->size >= 1) {
      got_ec_in = true;
    }
  }

  if(un->CheckError(!got_ec_in, quiet, rval,
                "no projection from ECin Layer found: must recv a MarkerConSpec prjn from it, with at least one unit")) {
    return false;
  }

  return true;
}

void ECoutUnitSpec::ClampFromECin(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(deep.on) {
    Compute_DeepMod(u, net, thr_no);
  }
  const int nrg = u->NRecvConGps(net, thr_no);
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(!cs->InheritsFrom(&TA_MarkerConSpec))
      continue;
    LeabraUnitVars* su = (LeabraUnitVars*)recv_gp->UnVars(0, net);
    float inval = su->act_eq;
    u->act = clamp_range.Clip(inval);
    if(deep.on) {
      u->act *= u->deep_mod;
    }
    u->act_eq = u->act_nd = u->act;
    TestWrite(u->da, 0.0f);
    //    u->AddToActBuf(syn_delay);
  }
}

void ECoutUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->quarter == 3)
    ClampFromECin(u, net, thr_no);
  else
    inherited::Compute_Act_Rate(u, net, thr_no);
}

void ECoutUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->quarter == 3) 
    ClampFromECin(u, net, thr_no);
  else
    inherited::Compute_Act_Spike(u, net, thr_no);
}

float ECoutUnitSpec::Compute_SSE(UnitVars* ru, Network* rnet, int thr_no, bool& has_targ) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  
  float uerr = u->act_p - u->act_q1;
  if(fabsf(uerr) >= sse_tol)
    u->misc_1 = uerr * uerr;
  else
    TestWrite(u->misc_1, 0.0f);

  return inherited::Compute_SSE(ru, rnet, thr_no, has_targ);
}
