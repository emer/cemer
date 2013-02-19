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

#ifndef LeabraTICtxtConSpec_h
#define LeabraTICtxtConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraTICtxtConSpec);

class E_API LeabraTICtxtConSpec : public LeabraConSpec {
  // leabra TI (temporal integration) context con spec -- use for SELF projection in a layer to implement LeabraTI context activation and learning 
INHERITED(LeabraConSpec)
public:

  inline void C_Send_CtxtNetin_Thread(Connection* cn, float* send_netin_vec,
                                      LeabraUnit* ru, const float su_act_eff) {
    send_netin_vec[ru->flat_idx] += cn->wt * su_act_eff;
  }
  // #IGNORE

  inline void C_Send_CtxtNetin_NoThread(Connection* cn, LeabraUnit* ru,
                                        const float su_act_eff) {
    ru->net_ctxt += cn->wt * su_act_eff;
  }
  // #IGNORE

  inline void Send_CtxtNetin(LeabraSendCons* cg, LeabraNetwork* net,
                             int thread_no, float su_act) {
    const float su_act_eff = cg->scale_eff * su_act;
    if(thread_no < 0) {
      CON_GROUP_LOOP(cg, C_Send_CtxtNetin_NoThread(cg->OwnCn(i), (LeabraUnit*)cg->Un(i),
                                                   su_act_eff));
    }
    else {
      float* send_netin_vec = net->send_netin_tmp.el
        + net->send_netin_tmp.FastElIndex(0, thread_no);
      CON_GROUP_LOOP(cg, C_Send_CtxtNetin_Thread(cg->OwnCn(i), send_netin_vec,
                                                 (LeabraUnit*)cg->Un(i), su_act_eff));
    }
  }
  // #CAT_Activation sender-based activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // todo: turn off contribution to regular netins
  // and impl new learning rule..

  TA_SIMPLE_BASEFUNS(LeabraTICtxtConSpec);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // LeabraTICtxtConSpec_h
