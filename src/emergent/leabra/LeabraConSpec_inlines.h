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

#ifndef LeabraConSpec_inlines_h
#define LeabraConSpec_inlines_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:

inline void LeabraConSpec::C_Send_NetinDelta_Thread(Connection* cn, float* send_netin_vec,
				     LeabraUnit* ru, const float su_act_delta_eff) {
  send_netin_vec[ru->flat_idx] += cn->wt * su_act_delta_eff;
}

inline void LeabraConSpec::C_Send_NetinDelta_NoThread(Connection* cn, LeabraUnit* ru,
					   const float su_act_delta_eff) {
  ru->net_delta += cn->wt * su_act_delta_eff;
}

inline void LeabraConSpec::Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
					   int thread_no, float su_act_delta) {
  const float su_act_delta_eff = cg->scale_eff * su_act_delta;
  if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
    float* send_netin_vec = net->send_netin_tmp.el
      + net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
    CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread(cg->OwnCn(i), send_netin_vec,
					      (LeabraUnit*)cg->Un(i), su_act_delta_eff));
  }
  else {
    if(thread_no < 0) {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThread(cg->OwnCn(i), (LeabraUnit*)cg->Un(i),
						  su_act_delta_eff));
    }
    else {
      float* send_netin_vec = net->send_netin_tmp.el
	+ net->send_netin_tmp.FastElIndex(0, thread_no);
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread(cg->OwnCn(i), send_netin_vec,
						(LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
  }
}

inline float LeabraConSpec::C_Compute_Netin(LeabraCon* cn, LeabraUnit*, LeabraUnit* su) {
  return cn->wt * su->act;	// NOTE: doesn't work with spiking -- need a separate function to use act_eq for that case -- using act_eq does NOT work with scalarval etc
}

inline float LeabraConSpec::Compute_Netin(RecvCons* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin((LeabraCon*)cg->PtrCn(i), (LeabraUnit*)ru,
					     (LeabraUnit*)cg->Un(i)));
  return ((LeabraRecvCons*)cg)->scale_eff * rval;
}


////////////////////////////////////////////////////
//     Computing dWt: LeabraCHL

inline void LeabraConSpec::Compute_SAvgCor(LeabraSendCons* cg, LeabraUnit*) {
  LeabraLayer* fm = (LeabraLayer*)cg->prjn->from.ptr();
  float savg = .5f + savg_cor.cor * (fm->kwta.pct - .5f);
  savg = MAX(savg_cor.thresh, savg); // keep this computed value within bounds
  cg->savg_cor = .5f / savg;
}

inline float LeabraConSpec::C_Compute_Hebb(LeabraCon* cn, LeabraSendCons* cg,
					   float lin_wt, float ru_act, float su_act)
{
  return ru_act * (su_act * (cg->savg_cor - lin_wt) - (1.0f - su_act) * lin_wt);
}

// generec error term with sigmoid activation function, and soft bounding
inline float LeabraConSpec::C_Compute_Err_LeabraCHL(LeabraCon* cn, float lin_wt,
						    float ru_act_p, float ru_act_m,
						    float su_act_p, float su_act_m) {
  float err = (ru_act_p * su_act_p) - (ru_act_m * su_act_m);
  if(lmix.err_sb) {
    if(err > 0.0f)	err *= (1.0f - lin_wt);
    else		err *= lin_wt;
  }
  return err;
}

// combine hebbian and error-driven
inline void LeabraConSpec::C_Compute_dWt(LeabraCon* cn, LeabraUnit*,
					 float heb, float err) {
  float dwt = lmix.err * err + lmix.hebb * heb;
  cn->dwt += cur_lrate * dwt;
}

// combine hebbian and error-driven
// inline void LeabraConSpec::C_Compute_dWt_NoHebb(LeabraCon* cn, LeabraUnit*,
// 						float err) {
//   cn->dwt += cur_lrate * err;
// }

inline void LeabraConSpec::Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
  LeabraNetwork* net = (LeabraNetwork*)su->own_net();
  if(ignore_unlearnable && net && net->unlearnable_trial) return;

  Compute_SAvgCor(cg, su);
  if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

  for(int i=0; i<cg->size; i++) {
    LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
    LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
    float lin_wt = LinFmSigWt(cn->wt);
    C_Compute_dWt(cn, ru, 
		  C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
		  C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m,
					  su->act_p, su->act_m));  
  }
}

/////////////////////////////////////
//	Compute_Weights_LeabraCHL

inline void LeabraConSpec::C_Compute_Weights_LeabraCHL(LeabraCon* cn)
{
  if(cn->dwt != 0.0f) {
    cn->wt = SigFmLinWt(LinFmSigWt(cn->wt) + cn->dwt);
  }
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
  CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((LeabraCon*)cg->OwnCn(i)));
  //  ApplyLimits(cg, ru); limits are automatically enforced anyway
}

//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_XCAL

inline void LeabraConSpec::
C_Compute_dWt_CtLeabraXCAL_trial(LeabraCon* cn, LeabraUnit* ru,
				 float su_avg_s, float su_avg_m, float su_act_mult) {
  float srs = ru->avg_s * su_avg_s;
  float srm = ru->avg_m * su_avg_m;
  float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
  float lthr = su_act_mult * ru->avg_l;
  float effthr = xcal.thr_m_mix * srm + lthr;
  cn->dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
}

inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
  LeabraNetwork* net = (LeabraNetwork*)su->own_net();
  if(ignore_unlearnable && net && net->unlearnable_trial) return;

  float su_avg_s = su->avg_s;
  float su_avg_m = su->avg_m;

  float su_act_mult = xcal.thr_l_mix * su->avg_m;

  for(int i=0; i<cg->size; i++) {
    LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
    C_Compute_dWt_CtLeabraXCAL_trial((LeabraCon*)cg->OwnCn(i), ru, su_avg_s, su_avg_m,
				     su_act_mult);
  }
}

/////////////////////////////////////
//	Compute_Weights_CtLeabraXCAL

inline void LeabraConSpec::C_Compute_Weights_CtLeabraXCAL(LeabraCon* cn) {
  if(cn->dwt != 0.0f) {
    // always do soft bounding, at this point (post agg across processors, etc)
    float lin_wt = LinFmSigWt(cn->wt);
    // always do soft bounding
    if(cn->dwt > 0.0f)	cn->dwt *= (1.0f - lin_wt);
    else		cn->dwt *= lin_wt;
    cn->wt = SigFmLinWt(lin_wt + cn->dwt);
  }
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
  CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL((LeabraCon*)cg->OwnCn(i)));
  //  ApplyLimits(cg, ru); limits are automatically enforced anyway
}


//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_XCalC -- receiver based for triggered learning..

inline void LeabraConSpec::
C_Compute_dWt_CtLeabraXCalC(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
  // todo: add some further modulation by avg_ds???
  float srs = ru->avg_s * su->avg_s;
  float srm = ru->avg_m * su->avg_m;
  float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
  float effthr = xcal.thr_m_mix * srm + xcal.thr_l_mix * su->avg_m * ru->avg_l;
  cn->dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
}

inline void LeabraConSpec::Compute_dWt_CtLeabraXCalC(LeabraRecvCons* cg, LeabraUnit* ru) {
  for(int i=0; i<cg->size; i++) {
    LeabraUnit* su = (LeabraUnit*)cg->Un(i);
    C_Compute_dWt_CtLeabraXCalC((LeabraCon*)cg->PtrCn(i), ru, su);
  }
}

//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_CAL -- NOTE: Requires LeabraSRAvgCon connections!

//////////////////////////////////////////////////////////////////////////////////
//     CtLeabra CAL SRAvg stuff

inline void LeabraConSpec::C_Compute_SRAvg_m(LeabraSRAvgCon* cn, float ru_act, float su_act) {
  cn->sravg_m += ru_act * su_act;
}

inline void LeabraConSpec::C_Compute_SRAvg_ms(LeabraSRAvgCon* cn, float ru_act, float su_act) {
  float sr = ru_act * su_act;
  cn->sravg_m += sr;
  cn->sravg_s += sr;
}

inline void LeabraConSpec::Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s) {
  if(do_s) {
    CON_GROUP_LOOP(cg, C_Compute_SRAvg_ms((LeabraSRAvgCon*)cg->OwnCn(i),
					  ((LeabraUnit*)cg->Un(i))->act_lrn, su->act_lrn));
  }
  else {
    CON_GROUP_LOOP(cg, C_Compute_SRAvg_m((LeabraSRAvgCon*)cg->OwnCn(i), 
					 ((LeabraUnit*)cg->Un(i))->act_lrn, su->act_lrn));
  }
}

inline void LeabraConSpec::C_Trial_Init_SRAvg(LeabraSRAvgCon* cn) {
  cn->sravg_s = 0.0f;
  cn->sravg_m = 0.0f;
}

inline void LeabraConSpec::Trial_Init_SRAvg(LeabraSendCons* cg, LeabraUnit* su) {
  CON_GROUP_LOOP(cg, C_Trial_Init_SRAvg((LeabraSRAvgCon*)cg->OwnCn(i)));
}

inline void LeabraConSpec::C_Compute_dWt_CtLeabraCAL(LeabraSRAvgCon* cn,
						     float sravg_s_nrm, float sravg_m_nrm) {
  cn->dwt += cur_lrate * (sravg_s_nrm * cn->sravg_s - sravg_m_nrm * cn->sravg_m);
}

inline void LeabraConSpec::Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
  // note: not doing all the checks for layers/groups inactive in plus phase: not needed since no hebb stuff
  LeabraNetwork* net = (LeabraNetwork*)su->own_net();
  if(ignore_unlearnable && net && net->unlearnable_trial) return;

  LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
  CON_GROUP_LOOP(cg,
		 C_Compute_dWt_CtLeabraCAL((LeabraSRAvgCon*)cg->OwnCn(i),
			   rlay->sravg_vals.s_nrm, rlay->sravg_vals.m_nrm));
}

/////////////////////////////////////
//	Compute_Weights_CtLeabraCAL

inline void LeabraConSpec::C_Compute_Weights_CtLeabraCAL(LeabraSRAvgCon* cn)
{
  // always do soft bounding, at this point (post agg across processors, etc)
  float lin_wt = LinFmSigWt(cn->wt);
  if(cn->dwt > 0.0f)	cn->dwt *= (1.0f - lin_wt);
  else			cn->dwt *= lin_wt;

  if(cn->dwt != 0.0f) {
    cn->wt = SigFmLinWt(lin_wt + cn->dwt);
  }
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
  CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraCAL((LeabraSRAvgCon*)cg->OwnCn(i)));
  //  ApplyLimits(cg, ru); limits are automatically enforced anyway
}


/////////////////////////////////////
// Master dWt function

inline void LeabraConSpec::Compute_Leabra_dWt(LeabraSendCons* cg, LeabraUnit* su) {
  if(!learn) return;
  switch(learn_rule) {
  case LEABRA_CHL:
    Compute_dWt_LeabraCHL(cg, su);
    break;
  case CTLEABRA_CAL:
    Compute_dWt_CtLeabraCAL(cg, su);
    break;
  case CTLEABRA_XCAL:
  case CTLEABRA_XCAL_C:
    Compute_dWt_CtLeabraXCAL(cg, su);
    break;
  }
}

inline void LeabraConSpec::Compute_Leabra_Weights(LeabraSendCons* cg, LeabraUnit* su) {
  if(!learn) return;
  switch(learn_rule) {
  case LEABRA_CHL:
    Compute_Weights_LeabraCHL(cg, su);
    break;
  case CTLEABRA_CAL:
    Compute_Weights_CtLeabraCAL(cg, su);
    break;
  case CTLEABRA_XCAL:
  case CTLEABRA_XCAL_C:
    Compute_Weights_CtLeabraXCAL(cg, su);
    break;
  }
}

//////////////////////////////////////////////////////////////////////////////////
//     Compute dWt Norm: receiver based 

inline void LeabraConSpec::Compute_dWt_Norm(LeabraRecvCons* cg, LeabraUnit* ru) {
  if(!learn || !wt_sig.dwt_norm) return;
  float sum_dwt = 0.0f;
  for(int i=0; i<cg->size; i++) {
    LeabraCon* cn = (LeabraCon*)cg->PtrCn(i);
    sum_dwt += cn->dwt;
  }
  if(sum_dwt == 0.0f) return;
  float dwnorm = sum_dwt / (float)cg->size;
  for(int i=0; i<cg->size; i++) {
    LeabraCon* cn = (LeabraCon*)cg->PtrCn(i);
    cn->dwt -= dwnorm;
  }
}

//////////////////////////////////////////////////////////////////////
//	Bias Weights: threshold dwt versions in LeabraBiasSpec

inline void LeabraConSpec::B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
  float err = ru->act_p - ru->act_m;
  cn->dwt += cur_lrate * err;
}
  
// default is not to do anything tricky with the bias weights
inline void LeabraConSpec::B_Compute_Weights(LeabraCon* cn, LeabraUnit* ru) {
  if(!learn) return;
  cn->pdw = cn->dwt;
  cn->wt += cn->dwt;
  cn->dwt = 0.0f;
  C_ApplyLimits(cn, ru, NULL);
}

inline void LeabraConSpec::B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						      LeabraLayer* rlay) {
  // cal only for bias weights: only err is useful contributor to this learning
  float dw = ru->avg_s - ru->avg_m;
  cn->dwt += cur_lrate * dw;
}

inline void LeabraConSpec::B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						     LeabraLayer* rlay) {
  float dw = ru->avg_s - ru->avg_m;
  cn->dwt += cur_lrate * dw;
}

/////////////////////////////////////
// Master Bias dWt function

inline void LeabraConSpec::B_Compute_Leabra_dWt(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
  if(!learn) return;
  switch(learn_rule) {
  case LEABRA_CHL:
    B_Compute_dWt_LeabraCHL(cn, ru);
    break;
  case CTLEABRA_CAL:
    B_Compute_dWt_CtLeabraCAL(cn, ru, rlay);
    break;
  case CTLEABRA_XCAL:
  case CTLEABRA_XCAL_C:
    B_Compute_dWt_CtLeabraXCAL(cn, ru, rlay);
    break;
  }
}


#endif // LeabraConSpec_inlines_h
