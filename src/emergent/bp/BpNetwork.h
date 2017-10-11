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

#ifndef BpNetwork_h
#define BpNetwork_h 1

// parent includes:
#include <Network>
#include <BpNetworkState_cpp>

#include <State_main>

// member includes:

// declare all other types mentioned but not required to include:
class BpConSpec; //

eTypeDef_Of(BpNetworkState_cpp);
eTypeDef_Of(BpUnitState_cpp);
eTypeDef_Of(BpConSpec_cpp);
eTypeDef_Of(BpUnitSpec_cpp);

eTypeDef_Of(BpNetwork);

class E_API BpNetwork : public Network {
  // #STEM_BASE ##CAT_Bp project for feedforward backpropagation networks (recurrent backprop is in RBpNetwork)
INHERITED(Network)
public:

#include <BpNetwork_core>
  
  void  Init_Weights() override;
 
  virtual void  Trial_Run(); // #CAT_Bp run one trial of Bp: calls SetCurLrate, Compute_NetinAct, Compute_dEdA_dEdNet, and, if train_mode == TRAIN, Compute_dWt.  If you want to save some speed just for testing, you can just call Compute_NetinAct and skip the other two (esp Compute_dEdA_dEdNet, which does a full backprop and is expensive, but often useful for visualization & testing)

  virtual void  SetCurLrate_Thr(int thr_no);
  
  inline void  Trial_Run_Thr(int thr_no)
  { ((BpNetworkState_cpp*)net_state)->Trial_Run_Thr(thr_no); }
  // #IGNORE

  NetworkState_cpp* NewNetworkState() const override;
  TypeDef*      NetworkStateType() const override;
  TypeDef* UnitStateType() const override;
  TypeDef* ConStateType() const override;
  
  void	SetProjectionDefaultTypes(Projection* prjn) override;
  void  BuildNullUnit() override;

  TA_SIMPLE_BASEFUNS(BpNetwork);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // BpNetwork_h
