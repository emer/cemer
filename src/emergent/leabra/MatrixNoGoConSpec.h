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

#ifndef MatrixNoGoConSpec_h
#define MatrixNoGoConSpec_h 1

// parent includes:
#include <MatrixConSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(MatrixNoGoConSpec);

class LEABRA_API MatrixNoGoConSpec : public MatrixConSpec {
  // Learning of Matrix_NoGo pathway input connections based on dopamine modulation of activation -- learns from recv (nogo) activity at end of minus phase, and sending activity at time of gating (act_mid).  also uses recv scale_eff for stripe-specific wt scale params
INHERITED(MatrixConSpec)
public:

  inline void C_Compute_dWt_Matrix(LeabraCon* cn, float lin_wt, 
				   float mtx_act_m, float mtx_da, float su_act_lrn) {
    float sr_prod = mtx_act_m * su_act_lrn;
    float dwt = -mtx_da * sr_prod; // reverse the da here for NoGo -- not reversed anywhere else before this!
    if(lmix.err_sb) {
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
    }
    cn->dwt += cur_lrate * dwt;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      MatrixCon* cn = (MatrixCon*)cg->OwnCn(i);
      if(ru->dav == 0.0f) continue; // if dav == 0 then was not gated!  in any case, dwt = 0
      C_Compute_dWt_Matrix(cn, LinFmSigWt(cn->wt), ru->act_m, ru->dav, cn->sact_lrn);
      // note: using cn->sact_lrn as having saved sending activation in Compute_MidMinusAct
    }
  }

  override void  Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
				 int thread_no, float su_act_delta) {
    if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
      float* send_netin_vec = net->send_netin_tmp.el
	+ net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
      for(int i=0; i<cg->size; i++) {
	LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	LeabraRecvCons* rcg = (LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx());
	C_Send_NetinDelta_Thrd(cg->OwnCn(i), send_netin_vec, ru,
			       su_act_delta * rcg->scale_eff);
      }
    }
    else {
      if(thread_no < 0) {
	for(int i=0; i<cg->size; i++) {
	  LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	  LeabraRecvCons* rcg = (LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx());
	  C_Send_NetinDelta_NoThrd(cg->OwnCn(i), ru, su_act_delta * rcg->scale_eff);
	}
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	for(int i=0; i<cg->size; i++) {
	  LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	  LeabraRecvCons* rcg = (LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx());
	  C_Send_NetinDelta_Thrd(cg->OwnCn(i), send_netin_vec, ru,
				 su_act_delta *	rcg->scale_eff);
	}
      }
    }
  }

  TA_SIMPLE_BASEFUNS(MatrixNoGoConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MatrixNoGoConSpec_h
