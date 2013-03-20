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

#ifndef PFCConSpec_h
#define PFCConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(PFCConSpec);

class E_API PFCConSpec : public LeabraConSpec {
  // for connections into PFC units -- modulates learning as function of gating
INHERITED(LeabraConSpec)
public:
  bool  lrn_gate_only;          // learn only on gating (actually trial after gating)

#if 0                           // use this if we end up with stripe-specific scale_eff
  override void  Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
				 int thread_no, float su_act_delta) {
    if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
      float* send_netin_vec = net->send_netin_tmp.el
	+ net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
      for(int i=0; i<cg->size; i++) {
	LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	LeabraRecvCons* rcg = (LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx());
	C_Send_NetinDelta_Thread(cg->OwnCn(i), send_netin_vec, ru,
			       su_act_delta * rcg->scale_eff);
      }
    }
    else {
      if(thread_no < 0) {
	for(int i=0; i<cg->size; i++) {
	  LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	  LeabraRecvCons* rcg = (LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx());
	  C_Send_NetinDelta_NoThread(cg->OwnCn(i), ru, su_act_delta * rcg->scale_eff);
	}
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	for(int i=0; i<cg->size; i++) {
	  LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	  LeabraRecvCons* rcg = (LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx());
	  C_Send_NetinDelta_Thread(cg->OwnCn(i), send_netin_vec, ru,
				 su_act_delta *	rcg->scale_eff);
	}
      }
    }
  }
#endif

  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    float su_avg_s = su->avg_s;
    float su_avg_m = su->avg_m;

    float su_act_mult = xcal.thr_l_mix * su->avg_m;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      if(lrn_gate_only && ru->misc_1 == 0.0f) continue; // no learn outside of gating
      C_Compute_dWt_CtLeabraXCAL_trial((LeabraCon*)cg->OwnCn(i), ru, su_avg_s, su_avg_m,
                                       su_act_mult);
    }
  }

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_SAvgCor(cg, su);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      if(lrn_gate_only && ru->misc_1 == 0.0f) continue; // no learn outside of gating
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      float lin_wt = LinFmSigWt(cn->wt);
      C_Compute_dWt(cn, ru, 
                    C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
                    C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m,
                                            su->act_p, su->act_m));  
    }
  }


  TA_SIMPLE_BASEFUNS(PFCConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // PFCConSpec_h
