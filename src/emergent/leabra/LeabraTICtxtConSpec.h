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

#ifndef LeabraTICtxtConSpec_h
#define LeabraTICtxtConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraTICtxtConSpec);

class E_API LeabraTICtxtConSpec : public LeabraConSpec {
  // leabra TI (temporal integration) context con spec -- use for SELF projection in a layer to implement LeabraTI context activation and learning -- only uses the wt_scale.abs to control magnitude of context netinput, and doesn't compete with other prjns for relative wt scale at all
INHERITED(LeabraConSpec)
public:
  bool          ti_learn_pred;  // learns to train context to predict outcome -- see also ti_mode on network -- this is NOT tied to that setting -- can intermix the two, though it doesn't really make sense

  // special!
  override bool  NetinScale_ExcludeFromNorm(LeabraRecvCons* recv_gp, LeabraLayer* from)
  { return true; }
  override void	Compute_NetinScale(LeabraRecvCons* recv_gp, LeabraLayer* from);

  inline void C_Send_CtxtNetin_Thread(Connection* cn, float* send_netin_vec,
                                      LeabraUnit* ru, const float su_act_eff) {
    send_netin_vec[ru->flat_idx] += cn->wt * su_act_eff;
  }
  // #IGNORE

  inline void C_Send_CtxtNetin_NoThread(Connection* cn, LeabraUnit* ru,
                                        const float su_act_eff) {
    ru->net_ctxt += cn->wt * su_act_eff;
  }
  // #IGNORE

  inline void Send_CtxtNetin(LeabraSendCons* cg, LeabraNetwork* net,
                             int thread_no, float su_act) {
    const float su_act_eff = cg->scale_eff * su_act;
    if(thread_no < 0) {
      CON_GROUP_LOOP(cg, C_Send_CtxtNetin_NoThread(cg->OwnCn(i), (LeabraUnit*)cg->Un(i),
                                                   su_act_eff));
    }
    else {
      float* send_netin_vec = net->send_netin_tmp.el
        + net->send_netin_tmp.FastElIndex(0, thread_no);
      CON_GROUP_LOOP(cg, C_Send_CtxtNetin_Thread(cg->OwnCn(i), send_netin_vec,
                                                 (LeabraUnit*)cg->Un(i), su_act_eff));
    }
  }
  // #CAT_Activation sender-based activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // don't send regular net inputs..
  override void Send_NetinDelta(LeabraSendCons*, LeabraNetwork* net, int thread_no, 
				float su_act_delta_eff) { };
  override float Compute_Netin(RecvCons* cg, Unit* ru) { return 0.0f; }


  inline void C_Compute_dWt_Delta(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float dwt;
    if(ti_learn_pred) {
      dwt = (ru->act_p - ru->act_ctxt) * su->p_act_p;
    }
    else {
      dwt = (ru->act_p - ru->act_m) * su->p_act_p;
    }
    if(lmix.err_sb) {
      float lin_wt = LinFmSigWt(cn->wt);
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
    }
    cn->dwt += cur_lrate * dwt;
  }

  inline void C_Compute_dWt_Delta_CAL(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float dwt;
    if(ti_learn_pred) {
      dwt = (ru->act_p - ru->act_ctxt) * su->p_act_p;
    }
    else {
      dwt = (ru->act_p - ru->act_m) * su->p_act_p;
    }
    cn->dwt += cur_lrate * dwt;
    // soft bounding is managed in the weight update phase, not in dwt
  }

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta(cn, ru, su);  
    }
  }

  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta_CAL(cn, ru, su);  
    }
  }

  override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta_CAL(cn, ru, su);  
    }
  }

  TA_SIMPLE_BASEFUNS(LeabraTICtxtConSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // LeabraTICtxtConSpec_h
