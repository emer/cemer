// Co2018ght 2006-2017, Regents of the University of Colorado,
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
#include <ta_vector_ops.h>
#include <SpecMemberBase>

#include "network_def.h"

// forwards this file
class BpConSpec;
class BpCon;
class BpUnitState;
class BpUnitSpec;
class BpNetwork;
//


//////////////////////////////////////////////////////////////
//	                Specialized types

// todo: re-enable once base classes are working!
#if 0

eTypeDef_Of(HebbBpConSpec);

class E_API HebbBpConSpec : public BpConSpec {
  // Simple Hebbian wt update (send act * recv act)
INHERITED(BpConSpec)
public:
  inline void C_Compute_dWt(float& dwt, const float ru_act, const float su_act)
  { dwt += su_act * ru_act; }

  inline void Compute_dWt(ConState* cg, Network* net, int thr_no) override {
    BpUnitState* ru = (BpUnitState*)cg->ThrOwnUnVars(net, thr_no);
    const float ru_act = ru->HasExtFlag(UnitState::TARG) ? ru->targ : ru->act;
    float* dwts = cg->OwnCnVar(DWT);
    CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_act, cg->UnVars(i,net)->act));
  }

  inline void B_Compute_dWt(UnitState* uv, Network* net, int thr_no) override {
    const float ru_act = uv->HasExtFlag(UnitState::TARG) ? uv->targ : uv->act;
    uv->bias_dwt += ru_act;            // is this really what we want?
  }

  TA_BASEFUNS_NOCOPY(HebbBpConSpec);
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
};


eTypeDef_Of(ErrScaleBpConSpec);

class E_API ErrScaleBpConSpec : public BpConSpec {
  // con spec that scales the error by given parameter -- can be used to differentially weight the learning impact of one projection relative to another
INHERITED(BpConSpec)
public:
  float		err_scale;	// the scaling parameter

  inline float 		C_Compute_dEdA(const float wt, const float ru_dEdNet)
  { return err_scale * wt * ru_dEdNet; }

  inline float 	Compute_dEdA(ConState* cg, Network* net, int thr_no) override {
    // this is ptr-con based and thus very slow..
    float rval = 0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_dEdA(cg->PtrCn(i,WT,net),
                                              ((BpUnitState*)cg->UnVars(i,net))->dEdNet));
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
  float 		lr; 		// #NO_SAVE local synapse-specific learning rate

  DeltaBarDeltaBpCon() { lr = 0.0f; }
};

eTypeDef_Of(DeltaBarDeltaBpConSpec);

class E_API DeltaBarDeltaBpConSpec : public BpConSpec {
  // delta-bar-delta has local learning rates for each connection
INHERITED(BpConSpec)
public:
  enum DBDBpConVars {
    LR = N_BP_CON_VARS,         // local learning rate
  };

  float		lrate_incr;	// rate of learning rate increase (additive)
  float		lrate_decr;	// rate of learning rate decrease (multiplicative)
  float		act_lrate_incr;	// #HIDDEN actual lrate increase (times lrate)

  inline void   Init_Weights(ConState* cg, Network* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);

    int eff_thr_no = net->HasNetFlag(Network::INIT_WTS_1_THREAD) ? 0 : thr_no;
    
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    float* lrs = cg->OwnCnVar(LR);

    if(rnd.type != Random::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i], eff_thr_no);
        C_Init_dWt(dwts[i]);
        pdws[i] = 0.0f;
        lrs[i] = lrate;
      }
    }
  }

  inline void	C_UpdateLrate(float& lr, const float dwt, const float pdw) {
    const float prod = pdw * dwt;
    if(prod > 0.0f)
      lr += act_lrate_incr;
    else if(prod < 0.0f)
      lr *= lrate_decr;
    // prod = 0 means first epoch, don't change lrate..
  }

  inline void C_Compute_Weights_DBD(float& wt, float& dwt, float& pdw, float& lr) {
    pdw = lr * dwt + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
    C_UpdateLrate(lr, dwt, pdw);
  }
  inline void C_Compute_Weights_DBD_Simple(float& wt, float& dwt, float& pdw, float& lr) {
    pdw = lr * (dwt - decay * wt) + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
    C_UpdateLrate(lr, dwt, pdw);
  }
  inline void C_Compute_Weights_DBD_Elim(float& wt, float& dwt, float& pdw, float &lr) {
    float denom = (1.0f + wt * wt);
    pdw = lr * (dwt - ((decay * wt) / (denom * denom))) + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
    C_UpdateLrate(lr, dwt, pdw);
  }
  
  inline void	Compute_Weights(ConState* cg, Network* net, int thr_no) override {
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    float* lrs = cg->OwnCnVar(LR);

    switch(decay_type) {
    case NO_DECAY: {
      CON_GROUP_LOOP(cg, C_Compute_Weights_DBD(wts[i], dwts[i], pdws[i], lrs[i]));
      break;
    }
    case SIMPLE_DECAY: {
      CON_GROUP_LOOP(cg, C_Compute_Weights_DBD_Simple(wts[i], dwts[i], pdws[i], lrs[i]));
      break;
    }
    case ELIMINATION: {
      CON_GROUP_LOOP(cg, C_Compute_Weights_DBD_Elim(wts[i], dwts[i], pdws[i], lrs[i]));
      break;
    }
    }
    ApplyLimits(cg, net, thr_no);
  }

  // bias weight is NOT suported for delta-bar-delta -- would require a new unit var -- not worth it..

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
  UnitState::ExtFlags unit_flags;	 // flags to set on the unit after copying value
  MemberDef*	var_md;		 // #IGNORE memberdef of variable

  void Init_Acts(UnitState* uv, Network* net, int thr_no) override;
  void Compute_Act(UnitState* uv, Network* net, int thr_no) override;
  // copy activation from corresponding unit in projection from layer

  // nullify all other functions..
  void Compute_Netin(UnitState*, Network* net, int thr_no) 	override { };
  void Init_dWt(UnitState*, Network* net, int thr_no) 	override { };
  void Compute_dWt(UnitState*, Network* net, int thr_no) 	override { };
  void Compute_Weights(UnitState*, Network* net, int thr_no) 	override { };

  // bp special functions
  void Compute_Error(BpUnitState*, BpNetwork* net, int thr_no)  override { };
  void Compute_dEdA(BpUnitState*, BpNetwork* net, int thr_no) override { };
  void Compute_dEdNet(BpUnitState*, BpNetwork* net, int thr_no) override { }; //

  TA_SIMPLE_BASEFUNS(BpContextSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // disabled above


eTypeDef_Of(BpNetwork);

class E_API BpNetwork : public Network {
  // #STEM_BASE ##CAT_Bp project for feedforward backpropagation networks (recurrent backprop is in RBpNetwork)
INHERITED(Network)
public:
  

#ifdef CUDA_COMPILE
  bool  Cuda_MakeCudaNet() override;

  void  Cuda_CopyUnitSpec(void* cuda_us, const UnitSpec* source) override;
  void  Cuda_CopyConSpec(void* cuda_cs, const ConSpec* source) override;

  virtual void  Cuda_Trial_Run();
  // #IGNORE
  virtual void  Cuda_Compute_NetinAct();
  // #IGNORE
  virtual void  Cuda_Compute_dEdA_dEdNet();
  // #IGNORE
  virtual void  Cuda_Compute_dWt();
  // #IGNORE
  virtual void  Cuda_Compute_Weights();
  // #IGNORE
#endif
  
  TA_SIMPLE_BASEFUNS(BpNetwork);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{}
};



#endif // bp.h
