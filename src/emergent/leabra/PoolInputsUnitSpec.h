// Copyright 2017, Regents of the University of Colorado,
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

#ifndef PoolInputsUnitSpec_h
#define PoolInputsUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PoolInputsUnitSpec);

class E_API PoolInputsUnitSpec : public LeabraUnitSpec {
  // a unit that continuously computes the pooled activation value over its inputs -- does not participate in normal netinput -- use MarkerConSpecs for inputs -- does not use weights, just activations
INHERITED(LeabraUnitSpec)
public:
  enum PoolingFun {             // how to pool the inputs
    MAX_POOL,                   // our activation is max over our inputs
    AVG_POOL,                   // our activation is average over our inputs
  };

  PoolingFun    pool_fun;        // how to pool the inputs
 
  virtual void Compute_PooledAct(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // set current act from current inputs

  void	Compute_NetinInteg(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;

  void 	Compute_dWt(UnitState* u, Network* net, int thr_no) override { };
  void	Compute_Weights(UnitState* u, Network* net, int thr_no) override { };

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(PoolInputsUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // PoolInputsUnitSpec_h
