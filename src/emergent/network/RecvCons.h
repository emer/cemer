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

#ifndef RecvCons_h
#define RecvCons_h 1

// parent includes:
#include <BaseCons>

// member includes:
#include <ConSpec>

// declare all other types mentioned but not required to include:

eTypeDef_Of(RecvCons);

class E_API RecvCons : public BaseCons {
  // receiving connections base class -- one projection's worth of receiving connections
INHERITED(BaseCons)
public:
  inline int            send_idx() { return other_idx; }
  // #READ_ONLY index into sending unit's send. list of SendCons

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning

  void  Init_Weights(Unit* ru, Network* net)
  { if(GetConSpec()) GetConSpec()->Init_Weights(this,ru,net); }
  // #CAT_Learning initialize weights for group
  void  C_Init_Weights(int idx, Unit* ru, Unit* su, Network* net)
  { GetConSpec()->C_Init_Weights(this, idx, ru, su, net); }
  // #CAT_Learning initialize weights for single connection
  void  Init_Weights_post(Unit* ru, Network* net)
  { if(GetConSpec()) GetConSpec()->Init_Weights_post(this,ru,net); }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)
  void  Init_dWt(Unit* ru, Network* net)
  { GetConSpec()->Init_dWt(this,ru,net); }
  // #CAT_Learning  initialize weight change variables

  float Compute_Netin(Unit* ru, Network* net)
  { return GetConSpec()->Compute_Netin(this,ru,net); }
  // #CAT_Activation compute net input (receiver based; recv group)
  float Compute_Dist(Unit* ru, Network* net)
  { return GetConSpec()->Compute_Dist(this,ru,net); }
  // #CAT_Activation compute net input as distance between activation and weights
  void  Compute_dWt(Unit* ru, Network* net)
  { GetConSpec()->Compute_dWt(this,ru,net); }
  // #CAT_Learning compute weight changes (the fundamental learning problem)
  void  Compute_Weights(Unit* ru, Network* net)
  { GetConSpec()->Compute_Weights(this,ru,net); }
  // #CAT_Learning update weight values from deltas

  int  Dump_Save_Value(std::ostream& strm, taBase* par=NULL, int indent = 0) override;
  int  Dump_Load_Value(std::istream& strm, taBase* par=NULL) override;

  virtual int   Dump_Load_Old_Cons(Unit* ru, int recv_gp_idx);
  // #IGNORE load old connection values if a user-data string is present to this effect -- removes the user data after loading!

  TA_BASEFUNS_NOCOPY(RecvCons);
protected:
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // RecvCons_h
