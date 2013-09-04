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



#ifndef bp_h
#define bp_h

#include <Network>
#include <Schedule>
#include <SigmoidSpec>
#include <float_CircBuffer>
#include <ProjectBase>

#include "network_def.h"

// forwards this file
class BpConSpec;
class BpCon;
class BpRecvCons;
class BpSendCons;
class BpUnit;
class BpUnitSpec;
class BpNetwork;
//

// note: dwt is equivalent to the old dEdW and pdw = the old dwt

eTypeDef_Of(BpCon);

class E_API BpCon : public Connection {
  // #STEM_BASE ##CAT_Bp Bp connection
public:
  float		pdw;		// #NO_SAVE previous delta-weight change -- useful for viewing because current weight change (dwt) is typically reset to 0 when views are updated

  BpCon() { pdw = 0.0f; }
};

// ConSpec now has 3 versions of some functions, the two regular ones
// (one for the group and one for the connection), and now a third which is a
// virtual version of the C_ for use with the bias weights.
// every time a C_ version is overloaded, it is necessary to overload the B_
// version, where the new B_ just calls the new C_

eTypeDef_Of(BpConSpec);

class E_API BpConSpec : public ConSpec {
  // #STEM_BASE ##CAT_Bp Bp connection specifications
INHERITED(ConSpec)
public:
  enum MomentumType {
    AFTER_LRATE,		// apply momentum after learning rate (old pdp style)
    BEFORE_LRATE,		// apply momentum before learning rate
    NORMALIZED 			// like BEFORE, but normalize direction to unit length
  };

  float 	lrate;		// learning rate
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  Schedule	lrate_sched;	// schedule of learning rate over training epochs (multiplies lrate!)
  float 	momentum;	// momentum
  MomentumType	momentum_type;	// #CONDEDIT_OFF_momentum:0 type of momentum function to use
  float		momentum_c;	// #READ_ONLY complement of momentum (for NORMALIZED)
  float 	decay;		// decay rate (before lrate and momentum)
  void 		(*decay_fun)(BpConSpec* spec, BpCon* cn, BpUnit* ru, BpUnit* su);
  // #LIST_BpConSpec_WtDecay #CONDEDIT_OFF_decay:0 the weight decay function to use

  void 		C_Init_dWt(RecvCons* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_dWt(cg, cn, ru, su); ((BpCon*)cn)->dwt = 0.0f; }

  void 		C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_Weights(cg, cn, ru, su); ((BpCon*)cn)->pdw = 0.0f;}

  inline float		C_Compute_dEdA(BpCon* cn, BpUnit* ru, BpUnit* su);
  inline virtual float 	Compute_dEdA(BpSendCons* cg, BpUnit* su, Network* net);
  // get error from units I send to

  inline void 		C_Compute_dWt(BpCon* cn, BpUnit* ru, BpUnit* su);
  inline void 		Compute_dWt(RecvCons* cg, Unit* ru, Network* net);
  inline virtual void	B_Compute_dWt(BpCon* cn, BpUnit* ru);
  // Compute dE with respect to the weights

  inline void 	C_Compute_WtDecay(BpCon* cn, BpUnit* ru, BpUnit* su);
  // call the decay function

  inline void	C_BEF_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su); // BEFORE_LRATE
  inline void	C_AFT_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su); // AFTER_LRATE
  inline void	C_NRM_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su); // NORMALIZED
  inline void	Compute_Weights(RecvCons* cg, Unit* ru, Network* net);
  inline virtual void	B_Compute_Weights(BpCon* cn, BpUnit* ru);
  // for the bias unit

  virtual void	LogLrateSched(int epcs_per_step = 50, float n_steps=7);
  // #BUTTON #CAT_Learning establish a logarithmic learning rate schedule with given total number of steps (including first step at lrate) and epochs per step: numbers go down in sequence: 1, .5, .2, .1, .05, .02, .01, etc.. this is a particularly good lrate schedule for large nets on hard tasks

  virtual void	SetCurLrate(int epoch);
  // set current learning rate based on schedule given epoch

  void InitLinks();
  SIMPLE_COPY(BpConSpec);
  TA_BASEFUNS(BpConSpec);
protected:
  SPEC_DEFAULTS;
  override void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};

// the following functions are possible weight decay functions

// #REG_FUN
E_API void Bp_Simple_WtDecay(BpConSpec* spec, BpCon* cn, BpUnit* ru, BpUnit* su)
// #LIST_BpConSpec_WtDecay Simple weight decay (subtract decay*wt)
     ;				// term here so scanner picks up comment
// #REG_FUN
E_API void Bp_WtElim_WtDecay(BpConSpec* spec, BpCon* cn, BpUnit* ru, BpUnit* su)
// #LIST_BpConSpec_WtDecay Weight Elimination (Rumelhart) weight decay
     ;				// term here so scanner picks up comment

eTypeDef_Of(BpRecvCons);

class E_API BpRecvCons : public RecvCons {
  // #STEM_BASE ##CAT_Bp group of Bp recv connections
INHERITED(RecvCons)
public:
  // these are "convenience" functions for those defined in the spec

  void	SetCurLrate(int epoch) { ((BpConSpec*)GetConSpec())->SetCurLrate(epoch); }

  TA_BASEFUNS_NOCOPY(BpRecvCons);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

eTypeDef_Of(BpSendCons);

class E_API BpSendCons : public SendCons {
  // #STEM_BASE ##CAT_Bp group of Bp sending connections
INHERITED(SendCons)
public:
  // these are "convenience" functions for those defined in the spec

  float Compute_dEdA(BpUnit* su, Network* net)
  { return ((BpConSpec*)GetConSpec())->Compute_dEdA(this, su, net); }

  TA_BASEFUNS_NOCOPY(BpSendCons);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

eTypeDef_Of(BpUnitSpec);

class E_API BpUnitSpec : public UnitSpec {
  // #STEM_BASE ##CAT_Bp specifications for Bp units
INHERITED(UnitSpec)
public:
  SigmoidSpec	sig;		// sigmoid activation parameters
  float		err_tol;	// error tolerance: no error signal for a unit if |targ-act| < err_tol) (i.e., as if act == targ exactly) -- often useful to set to .05 or so to prevent over-learning with binary training signals -- big weights often needed to get very high or low activations
  void 		(*err_fun)(BpUnitSpec* spec, BpUnit* u);
  // #LIST_BpUnit_Error this points to the error fun, set appropriately

  // generic unit functions
  override void	Init_Acts(Unit* u, Network* net);
  override void Compute_Netin(Unit* u, Network* net, int thread_no=-1);
  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);
  override void Compute_dWt(Unit* u, Network* net, int thread_no=-1);
  override void Compute_Weights(Unit* u, Network* net, int thread_no=-1);

  // bp special functions
  virtual void 	Compute_Error(BpUnit* u, BpNetwork* net, int thread_no=-1);
  // call the unit error function (only on target units)
  virtual void 	Compute_dEdA(BpUnit* u, BpNetwork* net, int thread_no=-1);
  // compute derivative of error with respect to unit activation
  virtual void 	Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no=-1);
  // compute derivative of error with respect to unit net input

  virtual void	SetCurLrate(BpUnit* u, int epoch);
  // set current learning rate based on epoch

  virtual void	GraphActFun(DataTable* graph_data, float min = -5.0, float max = 5.0);
  // #BUTTON #NULL_OK graph the activation function (NULL = new graph log)

  void	InitLinks();
  SIMPLE_COPY(BpUnitSpec);
  TA_BASEFUNS(BpUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	Defaults_init();
};

// the following functions are possible error functions.

// #REG_FUN
E_API void Bp_Squared_Error(BpUnitSpec* spec, BpUnit* u)
// #LIST_BpUnit_Error Squared error function for bp
     ;				// term here so scanner picks up comment
// #REG_FUN
E_API void Bp_CrossEnt_Error(BpUnitSpec* spec, BpUnit* u)
// #LIST_BpUnit_Error Cross entropy error function for bp
     ;				// term here so scanner picks up comment

eTypeDef_Of(BpUnit);

class E_API BpUnit : public Unit {
  // #STEM_BASE ##CAT_Bp ##DMEM_SHARE_SETS_4 standard feed-forward Bp unit
INHERITED(Unit)
public:
  float 	err; 		// #VIEW_HOT this is E, not dEdA
  float 	dEdA;		// #LABEL_dEdA #DMEM_SHARE_SET_3 #VIEW_HOT error wrt activation
  float 	dEdNet;		// #LABEL_dEdNet #VIEW_HOT error wrt net input

  // these are "convenience" functions for those defined in the spec
  void SetCurLrate(int epoch)   { ((BpUnitSpec*)GetUnitSpec())->SetCurLrate(this, epoch); }
  void Compute_Error(BpNetwork* net, int thread_no=-1)
  { ((BpUnitSpec*)GetUnitSpec())->Compute_Error(this, net, thread_no); }
  void Compute_dEdA(BpNetwork* net, int thread_no=-1)
  { ((BpUnitSpec*)GetUnitSpec())->Compute_dEdA(this, net, thread_no); }
  void Compute_dEdNet(BpNetwork* net, int thread_no=-1)
  { ((BpUnitSpec*)GetUnitSpec())->Compute_dEdNet(this, net, thread_no); }
  void Compute_dEdA_dEdNet(BpNetwork* net, int thread_no=-1)
  { Compute_dEdA(net, thread_no); Compute_Error(net, thread_no); Compute_dEdNet(net, thread_no); }

  void	Copy_(const BpUnit& cp); // units need hi-performance custom copy
  TA_BASEFUNS(BpUnit);
private:
  void 	Initialize();
  void 	Destroy()		{ };
};

#ifndef __MAKETA__
typedef void (BpUnit::*BpUnitMethod)(BpNetwork*, int);
// this is required to disambiguate unit thread method guys -- double casting
#endif 

// inline functions (for speed)

inline float BpConSpec::C_Compute_dEdA(BpCon* cn, BpUnit* ru, BpUnit*) {
  return cn->wt * ru->dEdNet;
}
inline float BpConSpec::Compute_dEdA(BpSendCons* cg, BpUnit* su, Network* net) {
  float rval = 0.0f;
  CON_GROUP_LOOP(cg,rval += C_Compute_dEdA((BpCon*)cg->PtrCn(i), (BpUnit*)cg->Un(i,net),
                                           su));
  return rval;
}

inline void BpConSpec::C_Compute_dWt(BpCon* cn, BpUnit* ru, BpUnit* su) {
  cn->dwt += su->act * ru->dEdNet;
}
inline void BpConSpec::Compute_dWt(RecvCons* cg, Unit* ru, Network* net) {
  CON_GROUP_LOOP(cg,C_Compute_dWt((BpCon*)cg->OwnCn(i), (BpUnit*)ru,
                                  (BpUnit*)cg->Un(i,net)));
}
inline void BpConSpec::B_Compute_dWt(BpCon* cn, BpUnit* ru) {
  cn->dwt += ru->dEdNet;
}

inline void BpConSpec::C_Compute_WtDecay(BpCon* cn, BpUnit* ru, BpUnit* su) {
  if(decay_fun != NULL)
    (*decay_fun)(this, cn, ru, su);
}
inline void BpConSpec::C_AFT_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su) {
  C_Compute_WtDecay(cn, ru, su); // decay goes into dwt...
  cn->pdw = cur_lrate * cn->dwt + momentum * cn->pdw;
  cn->wt += cn->pdw;
  cn->dwt = 0.0f;
}
inline void BpConSpec::C_BEF_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su) {
  C_Compute_WtDecay(cn, ru, su);
  cn->pdw = cn->dwt + momentum * cn->pdw;
  cn->wt += cur_lrate * cn->pdw;
  cn->dwt = 0.0f;
}
inline void BpConSpec::C_NRM_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su) {
  C_Compute_WtDecay(cn, ru, su);
  cn->pdw = momentum_c * cn->dwt + momentum * cn->pdw;
  cn->wt += cur_lrate * cn->pdw;
  cn->dwt = 0.0f;
}

inline void BpConSpec::Compute_Weights(RecvCons* cg, Unit* ru, Network* net) {
  if(momentum_type == AFTER_LRATE) {
    CON_GROUP_LOOP(cg, C_AFT_Compute_Weights((BpCon*)cg->OwnCn(i),
                                             (BpUnit*)ru, (BpUnit*)cg->Un(i,net)));
  }
  else if(momentum_type == BEFORE_LRATE) {
    CON_GROUP_LOOP(cg, C_BEF_Compute_Weights((BpCon*)cg->OwnCn(i),
                                             (BpUnit*)ru, (BpUnit*)cg->Un(i,net)));
  }
  else {
    CON_GROUP_LOOP(cg, C_NRM_Compute_Weights((BpCon*)cg->OwnCn(i),
                                             (BpUnit*)ru, (BpUnit*)cg->Un(i,net)));
  }
  ApplyLimits(cg, ru, net);
}

inline void BpConSpec::B_Compute_Weights(BpCon* cn, BpUnit* ru) {
  if(momentum_type == AFTER_LRATE)
    C_AFT_Compute_Weights(cn, ru, NULL);
  else if(momentum_type == BEFORE_LRATE)
    C_BEF_Compute_Weights(cn, ru, NULL);
  else
    C_NRM_Compute_Weights(cn, ru, NULL);
  C_ApplyLimits(cn, ru, NULL);
}

//////////////////////////////////////////
//	Additional ConSpec Types	//
//////////////////////////////////////////

eTypeDef_Of(HebbBpConSpec);

class E_API HebbBpConSpec : public BpConSpec {
  // Simple Hebbian wt update (send act * recv act)
INHERITED(BpConSpec)
public:
  inline void 		C_Compute_dWt(BpCon* cn, BpUnit* ru, BpUnit* su);
  inline void 		Compute_dWt(RecvCons* cg, Unit* ru, Network* net);

  inline void		B_Compute_dWt(BpCon* cn, BpUnit* ru);

  TA_BASEFUNS_NOCOPY(HebbBpConSpec);
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
};

inline void HebbBpConSpec::C_Compute_dWt(BpCon* cn, BpUnit* ru, BpUnit* su) {
  cn->dwt += ((ru->ext_flag & Unit::TARG) ? ru->targ : ru->act) * su->act;
}

inline void HebbBpConSpec::Compute_dWt(RecvCons* cg, Unit* ru, Network* net) {
  CON_GROUP_LOOP(cg,C_Compute_dWt((BpCon*)cg->OwnCn(i), (BpUnit*)ru,
                                  (BpUnit*)cg->Un(i,net)));
}

inline void HebbBpConSpec::B_Compute_dWt(BpCon* cn, BpUnit* ru) {
  cn->dwt +=  ((ru->ext_flag & Unit::TARG) ? ru->targ : ru->act);
}

eTypeDef_Of(ErrScaleBpConSpec);

class E_API ErrScaleBpConSpec : public BpConSpec {
  // con spec that scales the error by given parameter
INHERITED(BpConSpec)
public:
  float		err_scale;	// the scaling parameter

  inline float 		C_Compute_dEdA(BpCon* cn, BpUnit* ru, BpUnit* su);
  inline float 		Compute_dEdA(BpSendCons* cg, BpUnit* su, Network* net);

  TA_SIMPLE_BASEFUNS(ErrScaleBpConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize()	{ err_scale = 1.0f; }
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

inline float ErrScaleBpConSpec::C_Compute_dEdA(BpCon* cn, BpUnit* ru, BpUnit*) {
  return err_scale * cn->wt * ru->dEdNet;
}
inline float ErrScaleBpConSpec::Compute_dEdA(BpSendCons* cg, BpUnit* su, Network* net) {
  float rval = 0.0f;
  CON_GROUP_LOOP(cg,rval += C_Compute_dEdA((BpCon*)cg->PtrCn(i), (BpUnit*)cg->Un(i,net),
                                           su));
  return rval;
}

eTypeDef_Of(DeltaBarDeltaBpCon);

class E_API DeltaBarDeltaBpCon : public BpCon {
  // delta-bar-delta connection object with local learning rate
public:
  float 		lrate; 		// #NO_SAVE local learning rate

  DeltaBarDeltaBpCon() { lrate = 0.0f; }
};

eTypeDef_Of(DeltaBarDeltaBpConSpec);

class E_API DeltaBarDeltaBpConSpec : public BpConSpec {
  // delta-bar-delta has local learning rates for each connection
INHERITED(BpConSpec)
public:
  float		lrate_incr;	// rate of learning rate increase (additive)
  float		lrate_decr;	// rate of learning rate decrease (multiplicative)
  float		act_lrate_incr;	// #HIDDEN actual lrate increase (times lrate)

  void 		C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_Weights(cg, cn, ru, su); ((DeltaBarDeltaBpCon*)cn)->lrate = lrate;}
  // set initial learning rate

  inline void	C_UpdateLrate(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su);
  inline void	C_BEF_Compute_Weights(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su);
  inline void	C_AFT_Compute_Weights(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su);
  inline void	C_NRM_Compute_Weights(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su);
  inline virtual void	Compute_Weights(RecvCons* cg, Unit* ru, Network* net);
  inline virtual void	B_Compute_Weights(BpCon* cn, BpUnit* ru);

  TA_SIMPLE_BASEFUNS(DeltaBarDeltaBpConSpec);
protected:
  SPEC_DEFAULTS;
  override void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};

inline void DeltaBarDeltaBpConSpec::C_UpdateLrate
(DeltaBarDeltaBpCon* cn, BpUnit*, BpUnit*)
{
  float prod = cn->pdw * cn->dwt;
  if(prod > 0.0f)
    cn->lrate += act_lrate_incr;
  else if(prod < 0.0f)
    cn->lrate *= lrate_decr;
  // prod = 0 means first epoch, don't change lrate..
}

inline void DeltaBarDeltaBpConSpec::C_AFT_Compute_Weights
(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su)
{
  C_Compute_WtDecay(cn, ru, su); // decay goes into dwt...
  C_UpdateLrate(cn, ru, su);
  cn->pdw = cn->lrate * cn->dwt + momentum * cn->pdw;
  cn->wt += cn->pdw;
  cn->dwt = 0.0f;
}
inline void DeltaBarDeltaBpConSpec::C_BEF_Compute_Weights
(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su)
{
  C_Compute_WtDecay(cn, ru, su);
  C_UpdateLrate(cn, ru, su);
  cn->pdw = cn->dwt + momentum * cn->pdw;
  cn->wt += cn->lrate * cn->pdw;
  cn->dwt = 0.0f;
}
inline void DeltaBarDeltaBpConSpec::C_NRM_Compute_Weights
(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su)
{
  C_Compute_WtDecay(cn, ru, su);
  C_UpdateLrate(cn, ru, su);
  cn->pdw = momentum_c * cn->dwt + momentum * cn->pdw;
  cn->wt += cn->lrate * cn->pdw;
  cn->dwt = 0.0f;
}

inline void DeltaBarDeltaBpConSpec::Compute_Weights(RecvCons* cg, Unit* ru, Network* net) {
  if(momentum_type == AFTER_LRATE) {
    CON_GROUP_LOOP(cg, C_AFT_Compute_Weights((DeltaBarDeltaBpCon*)cg->OwnCn(i),
                                             (BpUnit*)ru, (BpUnit*)cg->Un(i,net)));
  }
  else if(momentum_type == BEFORE_LRATE) {
    CON_GROUP_LOOP(cg, C_BEF_Compute_Weights((DeltaBarDeltaBpCon*)cg->OwnCn(i),
                                             (BpUnit*)ru, (BpUnit*)cg->Un(i,net)));
  }
  else {
    CON_GROUP_LOOP(cg, C_NRM_Compute_Weights((DeltaBarDeltaBpCon*)cg->OwnCn(i),
                                             (BpUnit*)ru, (BpUnit*)cg->Un(i,net)));
  }
  ApplyLimits(cg, ru, net);
}

inline void DeltaBarDeltaBpConSpec::B_Compute_Weights(BpCon* cn, BpUnit* ru) {
  if(momentum_type == AFTER_LRATE)
    C_AFT_Compute_Weights((DeltaBarDeltaBpCon*)cn, ru, NULL);
  else if(momentum_type == BEFORE_LRATE)
    C_BEF_Compute_Weights((DeltaBarDeltaBpCon*)cn, ru, NULL);
  else
    C_NRM_Compute_Weights((DeltaBarDeltaBpCon*)cn, ru, NULL);
  C_ApplyLimits(cn, ru, NULL);
}


//////////////////////////////////////////
//	Additional Unit Types		//
//////////////////////////////////////////

eTypeDef_Of(BpContextSpec);

class E_API BpContextSpec : public BpUnitSpec {
  // for context units in simple recurrent nets (SRN), expects one-to-one prjn from layer it copies, must be AFTER that layer in .layers
INHERITED(BpUnitSpec)
public:
  float		hysteresis;	 // hysteresis factor: (1-hyst)*new + hyst*old
  float		hysteresis_c;	 // #READ_ONLY complement of hysteresis
  RandomSpec	initial_act;	 // initial activation value
  String	variable;	 // name of unit variable to copy into
  Unit::ExtType	unit_flags;	 // flags to set on the unit after copying value
  MemberDef*	var_md;		 // #IGNORE memberdef of variable

  override void	Init_Acts(Unit* u, Network* net);
  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);
  // copy activation from corresponding unit in projection from layer

  // nullify all other functions..
  override void Compute_Netin(Unit*, Network* net, int thread_no=-1) 	{ };
  override void Init_dWt(Unit*, Network* net) 	{ };
  override void Compute_dWt(Unit*, Network* net, int thread_no=-1) 	{ };
  override void Compute_Weights(Unit*, Network* net, int thread_no=-1) 	{ };

  // bp special functions
  override void Compute_Error(BpUnit*, BpNetwork* net, int thread_no=-1)  { };
  override void Compute_dEdA(BpUnit*, BpNetwork* net, int thread_no=-1)	  { };
  override void Compute_dEdNet(BpUnit*, BpNetwork* net, int thread_no=-1) { }; //

  TA_SIMPLE_BASEFUNS(BpContextSpec);
protected:
  SPEC_DEFAULTS;
  override void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

eTypeDef_Of(LinearBpUnitSpec);

class E_API LinearBpUnitSpec : public BpUnitSpec {
  // linear unit in Bp
INHERITED(BpUnitSpec)
public:
  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);
  override void	Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no=-1);

  TA_BASEFUNS(LinearBpUnitSpec);
protected:
  SPEC_DEFAULTS;
  override void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ Initialize(); }
};

eTypeDef_Of(ThreshLinBpUnitSpec);

class E_API ThreshLinBpUnitSpec : public BpUnitSpec {
  // thresholded linear unit in Bp
INHERITED(BpUnitSpec)
public:
  float		threshold;

  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);
  override void	Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no=-1);

  TA_SIMPLE_BASEFUNS(ThreshLinBpUnitSpec);
protected:
  SPEC_DEFAULTS;
  override void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

eTypeDef_Of(NoisyBpUnitSpec);

class E_API NoisyBpUnitSpec : public BpUnitSpec {
  // Bp with noisy output signal (act plus noise)
INHERITED(BpUnitSpec)
public:
  RandomSpec	noise;		// what kind of noise to add to activations

  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);

  TA_SIMPLE_BASEFUNS(NoisyBpUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

eTypeDef_Of(StochasticBpUnitSpec);

class E_API StochasticBpUnitSpec : public BpUnitSpec {
  // Bp with a binary stochastic activation function
INHERITED(BpUnitSpec)
public:
  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);

  TA_BASEFUNS_NOCOPY(StochasticBpUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

eTypeDef_Of(RBFBpUnitSpec);

class E_API RBFBpUnitSpec : public BpUnitSpec {
  // Radial basis function (Gaussian) function units in Bp
INHERITED(BpUnitSpec)
public:
  float         var;            // variance of Gaussian
  float         norm_const;     // #HIDDEN normalization const for Gaussian
  float         denom_const;    // #HIDDEN denominator const for Gaussian

  override void Compute_Netin(Unit* u, Network* net, int thread_no=-1);
  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);
  override void	Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no=-1);

  TA_SIMPLE_BASEFUNS(RBFBpUnitSpec);
protected:
  SPEC_DEFAULTS;
  override void	UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()               { };
  void	Defaults_init() 	{ };
};

eTypeDef_Of(BumpBpUnitSpec);

class E_API BumpBpUnitSpec : public BpUnitSpec {
  // bump function in Bp: Gaussian of std net input
INHERITED(BpUnitSpec)
public:
  float         mean;           // mean of Gaussian
  float         std_dev;        // std deviation of Gaussian
  float         std_dev_r;      // #HIDDEN reciprocal of std_dev

  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);
  override void	Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no=-1);

  TA_SIMPLE_BASEFUNS(BumpBpUnitSpec);
protected:
  SPEC_DEFAULTS;
  override void	UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()               { };
  void	Defaults_init() 	{ };
};

eTypeDef_Of(ExpBpUnitSpec);

class E_API ExpBpUnitSpec : public BpUnitSpec {
  // exponential units in Bp: simple exponent of net input
INHERITED(BpUnitSpec)
public:
  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);
  override void	Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no=-1);

  TA_BASEFUNS_NOCOPY(ExpBpUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize()	{ };
  void  Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(SoftMaxBpUnitSpec);

class E_API SoftMaxBpUnitSpec : public BpUnitSpec {
  // SoftMax Units: first one-to-one prjn is from corresp exponential unit, second prjn is from single summing linear unit, this then divides two
INHERITED(BpUnitSpec)
public:

  override void Compute_Netin(Unit* u, Network* net, int thread_no=-1) { };
  // do nothing
  override void Compute_Act(Unit* u, Network* net, int thread_no=-1);
  override void	Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no=-1);

  // don't update my weights
  override void Compute_dWt(Unit*, Network* net, int thread_no=-1)	{ };
  override void Compute_Weights(Unit*, Network* net, int thread_no=-1)	{ };

  TA_BASEFUNS_NOCOPY(SoftMaxBpUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize()	{ };
  void  Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(BpLayer);

class E_API BpLayer : public Layer {
  // #STEM_BASE ##CAT_Bp A feedforward backpropagation layer
INHERITED(Layer)
public:

  TA_BASEFUNS_NOCOPY(BpLayer);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

eTypeDef_Of(BpNetwork);

class E_API BpNetwork : public Network {
  // #STEM_BASE ##CAT_Bp project for feedforward backpropagation networks (recurrent backprop is in RBpNetwork)
INHERITED(Network)
public:
  bool			bp_to_inputs;	// #DEF_false backpropagate errors to input layers (faster if not done, which is the default)

  virtual void	SetCurLrate();
  // #CAT_Learning set current learning rate, based on network epoch counter

  override void	Compute_NetinAct();

  virtual void	Compute_dEdA_dEdNet();
  // #CAT_Learning compute derivatives of error with respect to activations & net inputs (backpropagate)
  virtual void	Compute_Error();
  // #CAT_Learning compute local error values, for display purposes only (only call when testing, not training)

  override void	Compute_dWt();
  override void	Compute_Weights_impl();
  
  virtual void		Trial_Run(); // #CAT_Bp run one trial of Bp: calls SetCurLrate, Compute_NetinAct, Compute_dEdA_dEdNet, and, if train_mode == TRAIN, Compute_dWt.  If you want to save some speed just for testing, you can just call Compute_NetinAct and skip the other two (esp Compute_dEdA_dEdNet, which does a full backprop and is expensive, but often useful for visualization & testing)
  
  override void	SetProjectionDefaultTypes(Projection* prjn);

  TA_SIMPLE_BASEFUNS(BpNetwork);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{}
};

eTypeDef_Of(BpProject);

class E_API BpProject : public ProjectBase {
  // #STEM_BASE ##CAT_Bp project for backpropagation networks
INHERITED(ProjectBase)
public:

  TA_BASEFUNS_NOCOPY(BpProject);
private:
  void	Initialize();
  void 	Destroy()		{}
};



#endif // bp.h
