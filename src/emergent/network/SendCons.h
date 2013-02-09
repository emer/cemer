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

  void  Send_Netin(Network* net, int thread_no, Unit* su)
  { GetConSpec()->Send_Netin(this, net, thread_no, su); }
  // #CAT_Activation sender-based net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units
  void  Send_Netin_PerPrjn(Network* net, int thread_no, Unit* su)
  { GetConSpec()->Send_Netin_PerPrjn(this, net, thread_no, su); }
  // #CAT_Activation sender-based net input keeping prjns separate, for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units

  TA_BASEFUNS_NOCOPY(SendCons);
protected:
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void  Initialize();
  void  Destroy()       { }
};

#endif // SendCons_h
