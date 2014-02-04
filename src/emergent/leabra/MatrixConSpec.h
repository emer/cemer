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

eTypeDef_Of(MatrixConSpec);

class E_API MatrixConSpec : public LeabraConSpec {
  // Learning of matrix input connections based on dopamine value and sender * receiver activation product (using act_mid acts at time of gating), only for units with LEARN flag set (credit assignment gating flag) -- for both Matrix_Go and NoGo connections
INHERITED(LeabraConSpec)
public:
  enum MtxConVars {
    NTR = SWT+1,           // new trace -- drives updates to trace value -- su * ru at time of gating
    TR,                    // current ongoing trace that drives learning -- adds ntr and decays after learning on current values
  };

  float         mnt_decay;      // rate of decay of the synaptic trace for units that have ongoing continued maintenance (signalled by the LEARN flag on ru)
  float         no_mnt_decay;    // rate of decay of the synaptic trace for units that do NOT have ongoing continued maintenance (signalled by the LEARN flag on ru)

  inline void Compute_NTr(LeabraRecvCons* cg, LeabraUnit* ru, LeabraNetwork* net) {
    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i,net);
      cg->PtrCn(i, NTR, net) = su->act_eq * ru->act_eq;
    }
  }
  // #IGNORE RECV-based save current sender activation states to sact_lrn for subsequent learning -- call this at time of gating

  inline void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su,
                            LeabraNetwork* net, const bool do_s) override {
    // do NOT do this under any circumstances!!
  }

  inline void C_Compute_dWt_Matrix_Tr(float& dwt, const float mtx_da, const float decay,
                                      float& tr, float& ntr) {
    dwt += cur_lrate * mtx_da * tr; // first learn based on cur trace (from prior trial)
    tr += ntr - decay * tr;         // then update trace to include new vals
    ntr = 0.0f;                     // clear new -- any new trace needs to be computed
  }
  // #IGNORE

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                       LeabraNetwork* net) override {
    if(ignore_unlearnable && net->unlearnable_trial) return;

    float* dwts = cg->OwnCnVar(DWT);
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
      const float decay = ru->HasUnitFlag(Unit::LEARN) ? mnt_decay : no_mnt_decay;
      C_Compute_dWt_Matrix_Tr(dwts[i], ru->dav, decay, trs[i], ntrs[i]);
    }
  }

  inline void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                    LeabraNetwork* net) override {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                      LeabraNetwork* net) override {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                        LeabraNetwork* net) override {
    Compute_Weights_CtLeabraXCAL(cg, su, net);
  }
  inline void Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                          LeabraNetwork* net) override {
    Compute_Weights_CtLeabraXCAL(cg, su, net);
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
