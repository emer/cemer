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

#ifndef SendCons_h
#define SendCons_h 1

// parent includes:
#include <BaseCons>

// member includes:
#include <ConSpec>

// declare all other types mentioned but not required to include:

eTypeDef_Of(SendCons);

class E_API SendCons : public BaseCons {
  // sending connections base class -- one projection's worth of sending connections
INHERITED(BaseCons)
public:

  inline int            recv_idx() { return other_idx; }
  // #READ_ONLY index into recv unit's recv. list of RecvCons

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning

  // note: if using send_netin, it probably makes sense to have the sender own the connections
  // and add all the basic functionality from the sender perspective -- see Leabra for example

  void  Init_Weights(Unit* su, Network* net)
  { if(GetConSpec()) GetConSpec()->Init_Weights(this,su,net); }
  // #CAT_Learning initialize weights for group -- must set net->needs_wt_sym if wt_limits.sym is set
  void  C_Init_Weight_Rnd(float& wt)
  { GetConSpec()->C_Init_Weight_Rnd(wt); }
  // #CAT_Learning initialize weight value according to random number specs
  void  Init_Weights_sym(Unit* su, Network* net)
  { if(GetConSpec()) GetConSpec()->Init_Weights_sym_s(this,su,net); }
  // #CAT_Structure symmetrize the weights
  void  Init_Weights_post(Unit* su, Network* net)
  { if(GetConSpec()) GetConSpec()->Init_Weights_post(this,su,net); }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)
  void  Init_dWt(Unit* su, Network* net)
  { GetConSpec()->Init_dWt(this,su,net); }
  // #CAT_Learning  initialize weight change variables

  void  Send_Netin(Network* net, int thread_no, Unit* su)
  { GetConSpec()->Send_Netin(this, net, thread_no, su); }
  // #CAT_Activation sender-based net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units
  void  Send_Netin_PerPrjn(Network* net, int thread_no, Unit* su)
  { GetConSpec()->Send_Netin_PerPrjn(this, net, thread_no, su); }
  // #CAT_Activation sender-based net input keeping prjns separate, for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units

  void  Compute_dWt(Unit* su, Network* net)
  { GetConSpec()->Compute_dWt(this,su,net); }
  // #CAT_Learning compute weight changes (the fundamental learning problem)
  void  Compute_Weights(Unit* su, Network* net)
  { GetConSpec()->Compute_Weights(this,su,net); }
  // #CAT_Learning update weight values from deltas

  TA_BASEFUNS_NOCOPY(SendCons);
protected:
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void  Initialize();
  void  Destroy()       { }
};

#endif // SendCons_h
