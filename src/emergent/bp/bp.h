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
class BpUnitVars;
class BpUnitSpec;
class BpNetwork;
//

// note: dwt is equivalent to the old dEdW and pdw = the old dwt

eTypeDef_Of(BpCon);

class E_API BpCon : public Connection {
  // #STEM_BASE ##CAT_Bp Bp connection
public:
  float		pdw;		// #NO_SAVE previous delta-weight change -- needed for momentum term in the learning mechanism

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
 
  enum BpConVars {
    PDW = DWT+1,                // previous delta weight
  };

  float 	lrate;		// learning rate
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  Schedule	lrate_sched;	// schedule of learning rate over training epochs (multiplies lrate!)
  float 	momentum;	// momentum
  MomentumType	momentum_type;	// #CONDEDIT_OFF_momentum:0 type of momentum function to use
  float		momentum_c;	// #READ_ONLY complement of momentum (for NORMALIZED)
  float 	decay;		// decay rate (before lrate and momentum)
  void 		(*decay_fun)(BpConSpec* spec, float& wt, float& dwt);
  // #LIST_BpConSpec_WtDecay #CONDEDIT_OFF_decay:0 the weight decay function to use

  inline void   Init_dWt(ConGroup* cg, Network* net, int thr_no) override {
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    for(int i=0; i<cg->size; i++) {
      C_Init_dWt(dwts[i]);
      pdws[i] = 0.0f;
    }
  }

  inline void   Init_Weights(ConGroup* cg, Network* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);

    if(rnd.type != Random::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i], thr_no);
        C_Init_dWt(dwts[i]);
        pdws[i] = 0.0f;
      }
    }
  }

  inline float		C_Compute_dEdA(const float wt, const float ru_dEdNet)
  { return wt * ru_dEdNet; }
  // #IGNORE 
  inline virtual float 	Compute_dEdA(ConGroup* cg, Network* net, int thr_no);
  // get error from units I send to

  inline void 		C_Compute_dWt(float& dwt, const float ru_dEdNet,
                                     const float su_act)
  { dwt += su_act * ru_dEdNet; }
  // #IGNORE
  inline void 		Compute_dWt(ConGroup* cg, Network* net, int thr_no) override;
  // Compute dE with respect to the weights

  inline void 	C_Compute_WtDecay(float& wt, float& dwt) {
    if(decay_fun != NULL)
      (*decay_fun)(this, wt, dwt);
  }
  inline void C_AFT_Compute_Weights(float& wt, float& dwt, float& pdw) {
    C_Compute_WtDecay(wt, dwt); // decay goes into dwt...
    pdw = cur_lrate * dwt + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
  }
  // AFTER_LRATE
  inline void C_BEF_Compute_Weights(float& wt, float& dwt, float& pdw) {
    C_Compute_WtDecay(wt, dwt); // decay goes into dwt...
    pdw = dwt + momentum * pdw;
    wt += cur_lrate * pdw;
    dwt = 0.0f;
  }
  // BEFORE_LRATE
  inline void C_NRM_Compute_Weights(float& wt, float& dwt, float& pdw) {
    C_Compute_WtDecay(wt, dwt); // decay goes into dwt...
    pdw = momentum_c * dwt + momentum * pdw;
    wt += cur_lrate * pdw;
    dwt = 0.0f;
  }
  // NORMALIZED

  inline void	Compute_Weights(ConGroup* cg, Network* net, int thr_no) override;

  inline void B_Init_dWt(UnitVars* uv, Network* net, int thr_no) override;
  inline void B_Compute_dWt(UnitVars* uv, Network* net, int thr_no) override;
  inline void B_Compute_Weights(UnitVars* uv, Network* net, int thr_no) override;

  virtual void	LogLrateSched(int epcs_per_step = 50, float n_steps=7);
  // #BUTTON #CAT_Learning establish a logarithmic learning rate schedule with given total number of steps (including first step at lrate) and epochs per step: numbers go down in sequence: 1, .5, .2, .1, .05, .02, .01, etc.. this is a particularly good lrate schedule for large nets on hard tasks

  virtual void	SetCurLrate(int epoch);
  // set current learning rate based on schedule given epoch

  void InitLinks() override;
  SIMPLE_COPY(BpConSpec);
  TA_BASEFUNS(BpConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};

// the following functions are possible weight decay functions

// #REG_FUN
E_API void Bp_Simple_WtDecay(BpConSpec* spec, float& wt, float& dwt)
// #LIST_BpConSpec_WtDecay Simple weight decay (subtract decay*wt)
     ;				// term here so maketa picks up comment
// #REG_FUN
E_API void Bp_WtElim_WtDecay(BpConSpec* spec, float& wt, float& dwt)
// #LIST_BpConSpec_WtDecay Weight Elimination (Rumelhart) weight decay
     ;				// term here so maketa picks up comment

eTypeDef_Of(BpUnitSpec);

class E_API BpUnitSpec : public UnitSpec {
  // #STEM_BASE ##CAT_Bp specifications for Bp units
INHERITED(UnitSpec)
public:
  SigmoidSpec	sig;		// sigmoid activation parameters
  float		err_tol;	// error tolerance: no error signal for a unit if |targ-act| < err_tol) (i.e., as if act == targ exactly) -- often useful to set to .05 or so to prevent over-learning with binary training signals -- big weights often needed to get very high or low activations
  void 		(*err_fun)(BpUnitSpec* spec, BpUnitVars* u);
  // #LIST_BpUnit_Error this points to the error fun, set appropriately

  // generic unit functions
  void Init_Acts(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_Netin(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_Act(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_dWt(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_Weights(UnitVars* uv, Network* net, int thr_no) override;

  // bp special functions
  virtual void 	Compute_Error(BpUnitVars* u, BpNetwork* net, int thr_no);
  // call the unit error function (only on target units)
  virtual void 	Compute_dEdA(BpUnitVars* u, BpNetwork* net, int thr_no);
  // compute derivative of error with respect to unit activation
  virtual void 	Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no);
  // compute derivative of error with respect to unit net input

  inline void   Compute_dEdA_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no)
  { Compute_dEdA(u, net, thr_no); Compute_Error(u, net, thr_no);
    Compute_dEdNet(u, net, thr_no); }

  virtual void	SetCurLrate(BpUnitVars* u, BpNetwork* net, int thr_no);
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
E_API void Bp_Squared_Error(BpUnitSpec* spec, BpUnitVars* u)
// #LIST_BpUnit_Error Squared error function for bp
     ;				// term here so scanner picks up comment
// #REG_FUN
E_API void Bp_CrossEnt_Error(BpUnitSpec* spec, BpUnitVars* u)
// #LIST_BpUnit_Error Cross entropy error function for bp
     ;				// term here so scanner picks up comment

eTypeDef_Of(BpUnitVars);

class E_API BpUnitVars : public UnitVars {
  // #STEM_BASE ##CAT_Bp standard feed-forward Bp unit
public:
  float         bias_pdw;       // #VIEW_HOT previous bias weight change
  float 	err; 		// #VIEW_HOT error value -- this is E for target units, not dEdA
  float 	dEdA;		// #VIEW_HOT derivative of error wrt activation
  float 	dEdNet;		// #VIEW_HOT derivative of error wrt net input
};

eTypeDef_Of(BpUnit);

class E_API BpUnit : public Unit {
  // #STEM_BASE ##CAT_Bp standard feed-forward Bp unit
INHERITED(Unit)
public:
  inline UnitVars::ExtFlags ext_flag() { return GetUnitVars()->ext_flag; }
  // #CAT_UnitVar external input flags -- determines whether the unit is receiving an external input (EXT), target (TARG), or comparison value (COMP)
  inline float& targ()  { return GetUnitVars()->targ; }
  // #VIEW_HOT #CAT_UnitVar target value: drives learning to produce this activation value
  inline float& ext()   { return GetUnitVars()->ext; }
  // #VIEW_HOT #CAT_UnitVar external input: drives activation of unit from outside influences (e.g., sensory input)
  inline float& act()   { return GetUnitVars()->act; }
  // #VIEW_HOT #CAT_UnitVar activation value -- what the unit communicates to others
  inline float& net()   { return GetUnitVars()->net; }
  // #VIEW_HOT #CAT_UnitVar net input value -- what the unit receives from others  (typically sum of sending activations times the weights)
  inline float& bias_wt() { return GetUnitVars()->bias_wt; }
  // #VIEW_HOT #CAT_UnitVar bias weight value -- the bias weight acts like a connection from a unit that is always active with a constant value of 1 -- reflects intrinsic excitability from a biological perspective
  inline float& bias_dwt() { return GetUnitVars()->bias_dwt; }
  // #VIEW_HOT #CAT_UnitVar change in bias weight value as computed by a learning mechanism

  float&        bias_pdw()
  { return ((BpUnitVars*)GetUnitVars())->bias_pdw; }
  // #VIEW_HOT previous bias weight change
  float&	err()
  { return ((BpUnitVars*)GetUnitVars())->err; }
  // #VIEW_HOT error value -- this is E for target units, not dEdA
  float& 	dEdA()
  { return ((BpUnitVars*)GetUnitVars())->dEdA; }
  // #VIEW_HOT derivative of error wrt activation
  float& 	dEdNet()
  { return ((BpUnitVars*)GetUnitVars())->dEdNet; }
  // #VIEW_HOT derivative of error wrt net input

  TA_BASEFUNS_NOCOPY(BpUnit);
private:
  void 	Initialize()            { };
  void 	Destroy()		{ };
};

///////////////////////////////////////////////////////////
//      inline functions (for speed)

inline float BpConSpec::Compute_dEdA(ConGroup* cg, Network* net, int thr_no) {
  // this is ptr-con based and thus very slow..
  float rval = 0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_dEdA(cg->PtrCn(i,WT,net),
                                            ((BpUnitVars*)cg->UnVars(i,net))->dEdNet));
  return rval;
}

inline void BpConSpec::Compute_dWt(ConGroup* cg, Network* net, int thr_no) {
  BpUnitVars* ru = (BpUnitVars*)cg->ThrOwnUnVars(net, thr_no);
  const float ru_dEdNet = ru->dEdNet;
  float* dwts = cg->OwnCnVar(DWT);
  CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_dEdNet, cg->UnVars(i,net)->act));
  // todo: if unit act is all in a contiguous vector, and with vec chunking, this 
  // could be a very fast vector op
}

inline void BpConSpec::Compute_Weights(ConGroup* cg, Network* net, int thr_no) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  float* pdwts = cg->OwnCnVar(PDW);
  if(momentum_type == AFTER_LRATE) {
    CON_GROUP_LOOP(cg, C_AFT_Compute_Weights(wts[i], dwts[i], pdwts[i]));
  }
  else if(momentum_type == BEFORE_LRATE) {
    CON_GROUP_LOOP(cg, C_BEF_Compute_Weights(wts[i], dwts[i], pdwts[i]));
  }
  else {
    CON_GROUP_LOOP(cg, C_NRM_Compute_Weights(wts[i], dwts[i], pdwts[i]));
  }
  ApplyLimits(cg, net, thr_no);
}

inline void BpConSpec::B_Init_dWt(UnitVars* uv, Network* net, int thr_no) {
  C_Init_dWt(uv->bias_wt);
  ((BpUnitVars*)uv)->bias_pdw = 0.0f;
}

inline void BpConSpec::B_Compute_dWt(UnitVars* uv, Network* net, int thr_no) {
  uv->bias_dwt += ((BpUnitVars*)uv)->dEdNet;
}

inline void BpConSpec::B_Compute_Weights(UnitVars* u, Network* net, int thr_no) {
  BpUnitVars* uv = (BpUnitVars*)u;
  if(momentum_type == AFTER_LRATE)
    C_AFT_Compute_Weights(uv->bias_wt, uv->bias_dwt, uv->bias_pdw);
  else if(momentum_type == BEFORE_LRATE)
    C_BEF_Compute_Weights(uv->bias_wt, uv->bias_dwt, uv->bias_pdw);
  else
    C_NRM_Compute_Weights(uv->bias_wt, uv->bias_dwt, uv->bias_pdw);
  C_ApplyLimits(uv->bias_wt);
}

//////////////////////////////////////////
//	Additional ConSpec Types	//
//////////////////////////////////////////

eTypeDef_Of(HebbBpConSpec);

class E_API HebbBpConSpec : public BpConSpec {
  // Simple Hebbian wt update (send act * recv act)
INHERITED(BpConSpec)
public:
  inline void C_Compute_dWt(float& dwt, const float ru_act, const float su_act)
  { dwt += su_act * ru_act; }

  inline void Compute_dWt(ConGroup* cg, Network* net, int thr_no) override {
    BpUnitVars* ru = (BpUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    const float ru_act = (ru->ext_flag & UnitVars::TARG) ? ru->targ : ru->act;
    float* dwts = cg->OwnCnVar(DWT);
    CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_act, cg->UnVars(i,net)->act));
  }

  inline void B_Compute_dWt(UnitVars* uv, Network* net, int thr_no) override {
    const float ru_act = (uv->ext_flag & UnitVars::TARG) ? uv->targ : uv->act;
    uv->bias_dwt += ru_act;            // is this really what we want?
  }

  TA_BASEFUNS_NOCOPY(HebbBpConSpec);
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
};


eTypeDef_Of(ErrScaleBpConSpec);

class E_API ErrScaleBpConSpec : public BpConSpec {
  // con spec that scales the error by given parameter
INHERITED(BpConSpec)
public:
  float		err_scale;	// the scaling parameter

  inline float 		C_Compute_dEdA(const float wt, const float ru_dEdNet)
  { return err_scale * wt * ru_dEdNet; }

  inline float 	Compute_dEdA(ConGroup* cg, Network* net, int thr_no) override {
    // this is ptr-con based and thus very slow..
    float rval = 0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_dEdA(cg->PtrCn(i,WT,net),
                                              ((BpUnitVars*)cg->UnVars(i,net))->dEdNet));
    return rval;
  }

  TA_SIMPLE_BASEFUNS(ErrScaleBpConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize()	{ err_scale = 1.0f; }
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

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
  enum DBDBpConVars {
    LRATE = PDW+1,                // local learning rate
  };

  float		lrate_incr;	// rate of learning rate increase (additive)
  float		lrate_decr;	// rate of learning rate decrease (multiplicative)
  float		act_lrate_incr;	// #HIDDEN actual lrate increase (times lrate)

  inline void   Init_Weights(ConGroup* cg, Network* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    float* lrs = cg->OwnCnVar(LRATE);

    if(rnd.type != Random::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i], thr_no);
        C_Init_dWt(dwts[i]);
        pdws[i] = 0.0f;
        lrs[i] = lrate;
      }
    }
  }

  inline void	C_UpdateLrate(float& lrate, const float dwt, const float pdw) {
    const float prod = pdw * dwt;
    if(prod > 0.0f)
      lrate += act_lrate_incr;
    else if(prod < 0.0f)
      lrate *= lrate_decr;
    // prod = 0 means first epoch, don't change lrate..
  }

  inline void C_AFT_Compute_Weights(float& wt, float& dwt, float& pdw, float& lrate) {
    C_Compute_WtDecay(wt, dwt); // decay goes into dwt...
    C_UpdateLrate(lrate, dwt, pdw);
    pdw = cur_lrate * dwt + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
  }

  inline void C_BEF_Compute_Weights(float& wt, float& dwt, float& pdw, float& lrate) {
    C_Compute_WtDecay(wt, dwt); // decay goes into dwt...
    C_UpdateLrate(lrate, dwt, pdw);
    pdw = dwt + momentum * pdw;
    wt += cur_lrate * pdw;
    dwt = 0.0f;
  }

  inline void C_NRM_Compute_Weights(float& wt, float& dwt, float& pdw, float& lrate) {
    C_Compute_WtDecay(wt, dwt); // decay goes into dwt...
    C_UpdateLrate(lrate, dwt, pdw);
    pdw = momentum_c * dwt + momentum * pdw;
    wt += cur_lrate * pdw;
    dwt = 0.0f;
  }

  inline void	Compute_Weights(ConGroup* cg, Network* net, int thr_no) override {
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdwts = cg->OwnCnVar(PDW);
    float* lrates = cg->OwnCnVar(LRATE);
    if(momentum_type == AFTER_LRATE) {
      CON_GROUP_LOOP(cg, C_AFT_Compute_Weights(wts[i], dwts[i], pdwts[i], lrates[i]));
    }
    else if(momentum_type == BEFORE_LRATE) {
      CON_GROUP_LOOP(cg, C_BEF_Compute_Weights(wts[i], dwts[i], pdwts[i], lrates[i]));
    }
    else {
      CON_GROUP_LOOP(cg, C_NRM_Compute_Weights(wts[i], dwts[i], pdwts[i], lrates[i]));
    }
    ApplyLimits(cg, net, thr_no);
  }

  // todo: we need a new BpDBDUnitVars type, with the bias_lrate
  // inline void B_Compute_Weights(UnitVars* u, Network* net, int thr_no) override {
  //   BpUnitVars* uv = (BpUnitVars*)u;
  //   if(momentum_type == AFTER_LRATE)
  //     C_AFT_Compute_Weights(uv->bias_wt), uv->bias_dwt),
  //                           uv->bias_pdw), uv->bias_lrate));
  //   else if(momentum_type == BEFORE_LRATE)
  //     C_BEF_Compute_Weights(uv->bias_wt), uv->bias_dwt),
  //                           uv->bias_pdw), uv->bias_lrate));
  //   else
  //     C_NRM_Compute_Weights(uv->bias_wt), uv->bias_dwt),
  //                           uv->bias_pdw), uv->bias_lrate));
  //   C_ApplyLimits(uv->bias_WT));
  // }

  TA_SIMPLE_BASEFUNS(DeltaBarDeltaBpConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};

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
  UnitVars::ExtFlags unit_flags;	 // flags to set on the unit after copying value
  MemberDef*	var_md;		 // #IGNORE memberdef of variable

  void Init_Acts(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_Act(UnitVars* uv, Network* net, int thr_no) override;
  // copy activation from corresponding unit in projection from layer

  // nullify all other functions..
  void Compute_Netin(UnitVars*, Network* net, int thr_no) 	override { };
  void Init_dWt(UnitVars*, Network* net, int thr_no) 	override { };
  void Compute_dWt(UnitVars*, Network* net, int thr_no) 	override { };
  void Compute_Weights(UnitVars*, Network* net, int thr_no) 	override { };

  // bp special functions
  void Compute_Error(BpUnitVars*, BpNetwork* net, int thr_no)  override { };
  void Compute_dEdA(BpUnitVars*, BpNetwork* net, int thr_no) override { };
  void Compute_dEdNet(BpUnitVars*, BpNetwork* net, int thr_no) override { }; //

  TA_SIMPLE_BASEFUNS(BpContextSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
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
  void Compute_Act(UnitVars* u, Network* net, int thr_no) override;
  void	Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) override;

  TA_BASEFUNS(LinearBpUnitSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
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

  void Compute_Act(UnitVars* u, Network* net, int thr_no) override;
  void	Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(ThreshLinBpUnitSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
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

  void Compute_Act(UnitVars* u, Network* net, int thr_no) override;

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
  void Compute_Act(UnitVars* u, Network* net, int thr_no) override;

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

  void Compute_Netin(UnitVars* u, Network* net, int thr_no) override;
  void Compute_Act(UnitVars* u, Network* net, int thr_no) override;
  void	Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(RBFBpUnitSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
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

  void Compute_Act(UnitVars* u, Network* net, int thr_no) override;
  void	Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(BumpBpUnitSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
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
  void Compute_Act(UnitVars* u, Network* net, int thr_no) override;
  void	Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) override;

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

  void Compute_Netin(UnitVars* u, Network* net, int thr_no) override { };
  // do nothing
  void Compute_Act(UnitVars* u, Network* net, int thr_no) override;
  void	Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) override;

  // don't update my weights
  void Compute_dWt(UnitVars*, Network* net, int thr_no)	override { };
  void Compute_Weights(UnitVars*, Network* net, int thr_no)	override { };

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
  bool	bp_to_inputs;	// #DEF_false backpropagate errors to input layers (faster if not done, which is the default)
  int   prev_epoch;     // #NO_SAVE #HIDDEN previous epoch counter -- for detecting changes
    
  virtual void	SetCurLrate_Thr(int thr_no);
  // #IGNORE set current learning rate, based on network epoch counter

  virtual void	Compute_dEdA_dEdNet_Thr(int thr_no);
  // #IGNORE compute derivatives of error with respect to activations & net inputs (backpropagate)
  virtual void	Compute_Error();
  // #IGNORE compute local error values, for display purposes only (only call when testing, not training)

  void  Init_Weights() override;
  void	Compute_dWt_Thr(int thr_no) override;
  void	Compute_Weights_Thr(int thr_no) override;
  
  virtual void  Trial_Run(); // #CAT_Bp run one trial of Bp: calls SetCurLrate, Compute_NetinAct, Compute_dEdA_dEdNet, and, if train_mode == TRAIN, Compute_dWt.  If you want to save some speed just for testing, you can just call Compute_NetinAct and skip the other two (esp Compute_dEdA_dEdNet, which does a full backprop and is expensive, but often useful for visualization & testing)
  virtual void  Trial_Run_Thr(int thr_no); // #IGNORE
  
  void	SetProjectionDefaultTypes(Projection* prjn) override;
  void  BuildNullUnit() override;

  TA_SIMPLE_BASEFUNS(BpNetwork);
protected:
  void UpdateAfterEdit_impl() override;
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
