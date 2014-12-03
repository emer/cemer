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

#include "CerebGranuleUnitSpec.h"
#include <LeabraUnit>

TA_BASEFUNS_CTORS_DEFN(CerebGranuleUnitSpec);

TA_BASEFUNS_CTORS_DEFN(CerebGranuleSpecs);


void CerebGranuleSpecs::Initialize() {
  act_thr = 0.5f;
  inhib_start_time = 10;
  lrn_start_time = 60;
  lrn_end_time = 90;
}

void CerebGranuleSpecs::Defaults_init() {
}

void CerebGranuleUnitSpec::Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net,
                                           int       thr_no) {
  inherited::Compute_NetinInteg(u, net, thr_no);
  int time_since_thr = (int)u->ti_ctxt;
  if(time_since_thr > cereb.inhib_start_time) {
    // by turning net input off here, we allow other gran
    // cells to win the competition
    u->net = 0.0f;
  }
}

void CerebGranuleUnitSpec::Compute_GranLearnAct(LeabraUnitVars* u, LeabraNetwork* net,
                                                int thr_no) {
  // int time_since_thr = (int)u->ti_ctxt;
  // if(time_since_thr == 0) {
  //   u->act_lrn = 0.0f;
  //   if(u->act > cereb.act_thr) {
  //     time_since_thr = 1;
  //     u->act_ctxt = u->act;     // current act max
  //   }
  //   else {
  //     u->act_ctxt = 0.0f;       // reset max always
  //   }
  // }
  // else {      // if we get here, we've crossed threshold
  //   time_since_thr++;
  //   if(time_since_thr < cereb.inhib_start_time) {
  //     u->act_ctxt = MAX(u->act_ctxt, u->act); // get max within time window
  //     u->act_lrn = 0.0f;        // no learning yet
  //   }
  //   else if(time_since_thr < cereb.lrn_start_time) {
  //     u->act_lrn = 0.0f;        // no learning yet
  //   }
  //   else if(time_since_thr <= cereb.lrn_end_time) {
  //     u->act_lrn = u->act_ctxt; // set learning to previous max
  //   }
  //   else {                      // at end of time window
  //     time_since_thr = 0;       // reset everything
  //     u->act_ctxt = 0.0f;
  //     u->act_lrn = 0.0f;
  //   }
  // }
  // u->ti_ctxt = time_since_thr; // update counter
}

void CerebGranuleUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no)
{
  inherited::Compute_Act_Rate(u, net, thr_no);
  Compute_GranLearnAct(u, net, thr_no);
}

void CerebGranuleUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no)
{
  inherited::Compute_Act_Spike(u, net, thr_no);
  Compute_GranLearnAct(u, net, thr_no);
}
