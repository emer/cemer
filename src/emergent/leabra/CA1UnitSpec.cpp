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

#include "CA1UnitSpec.h"
#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(CA1UnitSpec);

void CA1UnitSpec::Initialize() {
  recall_decay = 1.0f;
  plus_decay = 1.0f;
  use_test_mode = true;
}

bool CA1UnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  bool rval = true;

  bool got_ec_in = false;
  bool got_ca3 = false;
  const int nrg = un->NRecvConGps(); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)un->RecvConGroup(g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    cs->SetUnique("wt_scale", true); // be sure!
    if(from->name.contains("EC") && from->name.contains("in")) {
      got_ec_in = true;
    }
    if(from->name.contains("CA3")) {
      got_ca3 = true;
    }
  }

  if(un->CheckError(!got_ca3, quiet, rval,
                     "no projection from CA3 Layer found: must recv from layer with a name that contains the string 'CA3'")) {
    return false;
  }
  if(un->CheckError(!got_ec_in, quiet, rval,
                "no projection from ECin Layer found: must recv from layer with a name that contains 'EC' and 'in'")) {
    return false;
  }

  return true;
}

void CA1UnitSpec::Compute_NetinScale(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  bool test_mode = (use_test_mode && net->train_mode == Network::TEST);
  bool init_netin = false;
  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();

    if(from->name.contains("EC") && from->name.contains("in")) {
      switch(net->quarter) {
      case 0:
        cs->wt_scale.abs = 1.0f;
        break;
      case 1:
        if(!test_mode)
          cs->wt_scale.abs = 0.0f;
        init_netin = true;
        break;
      case 3:
        cs->wt_scale.abs = 1.0f;
        init_netin = true;
        break;
      }
    }
    else if(from->name.contains("CA3")) {
      switch(net->quarter) {
      case 0:
        cs->wt_scale.abs = 0.0f;
        break;
      case 1:
        cs->wt_scale.abs = 1.0f;
        init_netin = true;
        break;
      case 3:
        if(!test_mode)
          cs->wt_scale.abs = 0.0f;
        init_netin = true;
        break;
      }
    }
  }

  if(init_netin) {
    Init_Netins(u, net, thr_no);
  }

  if(net->quarter == 1 && !test_mode) {
    DecayState(u, net, thr_no, recall_decay);
  }
  if(net->quarter == 3 && !test_mode) {
    DecayState(u, net, thr_no, plus_decay);
  }
  
  inherited::Compute_NetinScale(u, net, thr_no);
}  

