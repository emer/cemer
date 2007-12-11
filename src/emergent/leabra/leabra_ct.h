// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "leabra_td.h"

#ifndef leabra_ct_h
#define leabra_ct_h

// pre-declarations
class CtLeabraCon;
class CtLeabraConSpec;
class CtLeabraRecvCons;
class CtLeabraUnit;
class CtLeabraUnitSpec;
class CtLeabraLayer;
class CtLeabraLayerSpec;
class CtLeabraNetwork; //

//////////////////////////////////////////////////////////////////
// 	Continuous Time Processing and Learning in Leabra	//
//////////////////////////////////////////////////////////////////

// The most abstract version features:
// * synaptic depression
// * present-trains-past version of CHL learning that accumulates send*recv products
//   during settling and uses that for periodic learning
// * NOTE: any overlap in srp_p and srp_m just serves to dilute training signal!
//   because it is a difference.. anything in common just subtracts out.  so,
//   no need to explore exponential integrations, etc -- just keep it simple blocks

class LEABRA_API CtLeabraCon : public CycleSynDepCon {
  // continuous time leabra con: most abstract version of continous time
INHERITED(CycleSynDepCon)
public:
  float		srp_m;		// #NO_SAVE sender-receiver activation product, minus phase
  float		srp_p;		// #NO_SAVE sender-receiver activation product, plus phase

  CtLeabraCon() { srp_m = srp_p = 0.0f; }
};

class LEABRA_API CtLeabraConSpec : public CycleSynDepConSpec {
  // continuous time leabra con spec: most abstract version of continous time
INHERITED(CycleSynDepConSpec)
public:
  bool		avg_srp_dwt;	// use average send-recv product over interval for dwt (else just use snapshot activations at end of each phase)

  inline void C_Compute_Srp(CtLeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    cn->srp_p += su->act_eq * ru->act_eq;
  }

  inline void C_Compute_CtCycle(CtLeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    if(avg_srp_dwt)
      C_Compute_Srp(cn, ru, su);
    C_Depress_Wt(cn, ru, su);
  }

  virtual void Compute_CtCycle(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Compute_CtCycle((CtLeabraCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }
  // one cycle of processing at a Ct synapse -- expensive!!  todo: need to find ways to optimize
  
  // todo: hebbian is all whacky now too.

  inline float C_Compute_Err(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su,
			     float srp_norm);

  inline void Compute_dWt(RecvCons* cg, Unit* ru);

  inline override void 	C_Init_dWt(RecvCons*, Connection* cn, Unit*, Unit*)
  { ((CtLeabraCon*)cn)->srp_m = 0.0f; ((CtLeabraCon*)cn)->srp_p = 0.0f; }

  TA_BASEFUNS_NOCOPY(CtLeabraConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API CtLeabraRecvCons : public LeabraRecvCons {
  // continuous time leabra recv cons: most abstract version of continous time
INHERITED(LeabraRecvCons)
public:

  void	Compute_CtCycle(LeabraUnit* ru)
  { ((CtLeabraConSpec*)GetConSpec())->Compute_CtCycle(this, ru); }
  // #CAT_Learning compute one cycle of continuous time processing

  TA_BASEFUNS_NOCOPY(CtLeabraRecvCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API CtLeabraBiasSpec : public LeabraBiasSpec {
  // continuous time leabra bias spec: most abstract version of continous time
INHERITED(LeabraBiasSpec)
public:
  inline void B_Compute_CtCycle(CtLeabraCon* cn, LeabraUnit* ru) {
    cn->srp_p += ru->act_eq;
  }
  // #CAT_Learning compute one cycle of continuous time processing

 inline void B_Compute_dWt_Ct(CtLeabraCon* cn, LeabraUnit* ru, float srp_norm) {
    cn->srp_p *= srp_norm;
    float err = cn->srp_p - cn->srp_m;
    // now update for next phase..
    cn->srp_m = cn->srp_p;
    cn->srp_p = 0.0f;
    if(fabsf(err) >= dwt_thresh)
      cn->dwt += cur_lrate * err;
   }

  TA_BASEFUNS_NOCOPY(CtLeabraBiasSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////
// 	Ct Unit
//////////////////////////////////

class LEABRA_API CtLeabraUnitSpec : public DaModUnitSpec {
  // continuous time leabra unit spec: most abstract version of continous time
INHERITED(DaModUnitSpec)
public:
  virtual void 	Compute_CtCycle(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute one cycle of continuous time processing
  virtual void 	Compute_ActMP(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)

  override void Compute_dWt_impl(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);

  TA_BASEFUNS(CtLeabraUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API CtLeabraUnit : public DaModUnit {
  // continuous time leabra unit: most abstract version of continous time
INHERITED(DaModUnit)
public:
  void		Compute_CtCycle(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_CtCycle(this, lay, net); }
  // #CAT_Learning compute one cycle of continuous time processing
  void		Compute_ActMP(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_ActMP(this, lay, net); }
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)

  TA_BASEFUNS_NOCOPY(CtLeabraUnit);
private:
  void	Initialize();
  void	Destroy()	{ };
};


//////////////////////////////////
// 	Ct Layer
//////////////////////////////////

class LEABRA_API CtLeabraLayerSpec : public LeabraLayerSpec {
  // continuous time leabra layer spec: most abstract version of continous time
INHERITED(LeabraLayerSpec)
public:
  virtual void 	Compute_CtCycle(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute one cycle of continuous time processing
  virtual void 	Compute_ActMP(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)

  TA_BASEFUNS_NOCOPY(CtLeabraLayerSpec);
// protected:
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API CtLeabraLayer : public LeabraLayer {
  // continuous time leabra layer: most abstract version of continous time
INHERITED(LeabraLayer)
public:
  void 	Compute_CtCycle(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_CtCycle(this, net); };
  // #CAT_Learning compute one cycle of continuous time processing
  void 	Compute_ActMP(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_ActMP(this, net); };
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)

  TA_BASEFUNS_NOCOPY(CtLeabraLayer);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////
// 	Ct Network
//////////////////////////////////

class LEABRA_API CtLeabraNetwork : public LeabraNetwork {
  // continuous time leabra network: most abstract version of continous time -- uses tick counter to mark when weight updates are performed
INHERITED(LeabraNetwork)
public:
  int		cycles_per_tick;  // #DEF_10 #CAT_Counter number of cycles to perform per each tick of processing, where a tick consists of one weight update based on continuously sampled activation states
  float		srp_norm;	// #READ_ONLY #CAT_Statistic normalization factor to apply to send*recv activation product = 1.0 / cycles_per_tick

  virtual void 	Compute_CtCycle() ;
  // #CAT_Cycle compute one cycle of continuous-time processing, after activations are updated
  override void	Cycle_Run();

  virtual void 	Compute_ActMP() ;
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)

  override void	SetProjectionDefaultTypes(Projection* prjn);

  TA_SIMPLE_BASEFUNS(CtLeabraNetwork);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{}
};


/////////////////////////////////////////////////
//		Inlines

inline float CtLeabraConSpec::C_Compute_Err(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su, float srp_norm) {
  float err;
  if(avg_srp_dwt) {
    cn->srp_p *= srp_norm;
    err = cn->srp_p - cn->srp_m;
    // now update for next phase..
    cn->srp_m = cn->srp_p;
    cn->srp_p = 0.0f;
  }
  else {
    err = (ru->p_act_p * su->p_act_p) - (ru->p_act_m * su->p_act_m);
  }
  // wt is negative in linear form, so using opposite sign of usual here
  if(lmix.err_sb) {
    if(err > 0.0f)	err *= (1.0f + cn->wt);
    else		err *= -cn->wt;	
  }
  return err;
}

inline void CtLeabraConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
  CtLeabraUnit* lru = (CtLeabraUnit*)ru;
  LeabraRecvCons* lcg = (LeabraRecvCons*) cg;
  Compute_SAvgCor(lcg, lru);
  LeabraLayer* slay = (LeabraLayer*)cg->prjn->from.ptr();
  float srp_norm = ((CtLeabraNetwork*)slay->own_net)->srp_norm; // NOTE: must be Ct Net!
  for(int i=0; i<cg->cons.size; i++) {
    CtLeabraUnit* su = (CtLeabraUnit*)cg->Un(i);
    CtLeabraCon* cn = (CtLeabraCon*)cg->Cn(i);
    float orig_wt = cn->wt;
    C_Compute_LinFmWt(lcg, cn); // get weight into linear form
    C_Compute_dWt(cn, lru, 
		  C_Compute_Hebb(cn, lcg, lru->act_p, su->act_p),
		  C_Compute_Err(cn, lru, su, srp_norm));  
    cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
  }
}

#endif // leabra_ct_h
