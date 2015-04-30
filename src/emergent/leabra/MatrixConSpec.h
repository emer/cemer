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

#ifndef MatrixConSpec_h
#define MatrixConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>
#include <MatrixCon>

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixLearnSpec);

class E_API MatrixLearnSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for learning in the matrix 
INHERITED(SpecMemberBase)
public:
  bool          reset_any_da;   // reset on any dopamine value
  float         da_reset_tr;    // amount of dopamine to completely reset the trace
  bool          use_thal;       // include thalamic modulation in the trace value -- this should be true for PBWM use of matrix con specs, but other cases may not use thalamic gating, and should have this off
  float         otr_lrate;      // #CONDSHOW_ON_use_thal #MIN_0 #DEF_0.5 learning rate associated with other non-gated activations (only avail when using thalamic gating) -- should generally be less in proportion to average number gating / total stripes

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(MatrixLearnSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(MatrixConSpec);

class E_API MatrixConSpec : public LeabraConSpec {
  // Learning of matrix input connections based on sender * receiver activation product and (optionally) thal gating activation signal, which accumulate in an ongoing synaptic trace over time, until they are multiplied by a later dopamine dav value that is typically driven by primary value (US) outcome at end of a sequence of actions -- dwt = dav * tr; tr = [thal] * su * ru - otr_lrate * su * ru, representing a contrast between gated activations that go one way, and other non-gated activations that go the opposite way (which supports engagement of alternative gating strategies, and avoids overall reductions in weights) -- the trace is reset when this weight change is computed, as a result of an over-threshold level of dopamine.  Patch units shunt dopamine from actively maintaining stripes / information processing channels, to prevent this clearing.
INHERITED(LeabraConSpec)
public:
  enum MtxConVars {
    NTR = SWT+1,           // new trace -- drives updates to trace value -- thal * ru * su
    TR,                    // current ongoing trace of activations, which drive learning -- adds ntr and clears after learning on current values -- includes both thal gated (+ and other nongated, - inputs)
  };
  bool                  nogo;    // are these nogo con specs -- if so, flip the sign of the dopamine signal
  MatrixLearnSpec       matrix;  // parameters for special matrix learning dynamics

  inline void Init_Weights(ConGroup* cg, Network* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);

    if(rnd.type != Random::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i], thr_no);
        C_Init_dWt(dwts[i]);
        ntrs[i] = 0.0f;
        trs[i] = 0.0f;
      }
    }
  }

  // TODO: need to build in a temporal asymmetry here, so that gating activations
  // register before dopamine when it makes sense for them to?
  
  inline void C_Compute_dWt_Matrix_ThalAnyDa
    (float& dwt, float& ntr, float& tr, const float otr_lr, const float mtx_da,
     const float ru_thal, const float ru_act, const float su_act) {

    if(fabsf(mtx_da) > matrix.da_reset_tr) {
      dwt += cur_lrate * mtx_da * tr;
      tr = 0.0f;
    }
    else {
      if(ru_thal > 0.0f) {              // gated
        ntr = ru_thal * ru_act * su_act;
      }
      else {
        ntr = otr_lr * ru_act * su_act; // other alternative non-gated
      }
      tr += ntr;                       // just keep accumulating..
    }
  }
  // #IGNORE

  inline void C_Compute_dWt_Matrix_Thal
    (float& dwt, float& ntr, float& tr, const float otr_lr, const float mtx_da,
     const float ru_thal, const float ru_act, const float su_act) {

    dwt += cur_lrate * mtx_da * tr;
    float reset_factor = (matrix.da_reset_tr - fabs(mtx_da)) / matrix.da_reset_tr;
    if(reset_factor < 0.0f) reset_factor = 0.0f;
    tr *= reset_factor;

    if(ru_thal > 0.0f) {              // gated
      ntr = ru_thal * ru_act * su_act;
    }
    else {
      ntr = otr_lr * ru_act * su_act; // other alternative non-gated
    }
    tr += ntr;                       // just keep accumulating..
  }
  // #IGNORE

  inline void C_Compute_dWt_Matrix_NoThal
    (float& dwt, float& ntr, float& tr, const float otr_lr, const float mtx_da,
     const float ru_act, const float su_act) {

    dwt += cur_lrate * mtx_da * tr;
    float reset_factor = (matrix.da_reset_tr - fabs(mtx_da)) / matrix.da_reset_tr;
    if(reset_factor < 0.0f) reset_factor = 0.0f;
    tr *= reset_factor;

    ntr = ru_act * su_act;
    tr += ntr;                       // just keep accumulating..
  }
  // #IGNORE

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (ignore_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);

    float* dwts = cg->OwnCnVar(DWT);
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);

    const float otr_lr = -matrix.otr_lrate;
    
    const int sz = cg->size;
    if(matrix.use_thal) {
      if(matrix.reset_any_da) {
        for(int i=0; i<sz; i++) {
          LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
          float dav = ((nogo) ? -ru->dav : ru->dav);
          C_Compute_dWt_Matrix_ThalAnyDa(dwts[i], ntrs[i], trs[i], otr_lr,
                                         dav, ru->thal, ru->act_eq, su->act_eq);
        }
      }
      else {
        for(int i=0; i<sz; i++) {
          LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
          float dav = ((nogo) ? -ru->dav : ru->dav);
          C_Compute_dWt_Matrix_Thal(dwts[i], ntrs[i], trs[i], otr_lr,
                                    dav, ru->thal, ru->act_eq, su->act_eq);
        }
      }
    }
    else {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
        float dav = ((nogo) ? -ru->dav : ru->dav);
        C_Compute_dWt_Matrix_NoThal(dwts[i], ntrs[i], trs[i], otr_lr,
                                    dav, ru->act_eq, su->act_eq);
      }
    }
  }

  TA_SIMPLE_BASEFUNS(MatrixConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MatrixConSpec_h
