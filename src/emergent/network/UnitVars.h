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

#ifndef UnitVars_h
#define UnitVars_h 1

// parent includes:
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:
class UnitSpec; // 
class Unit; //
class Network; //
class ConGroup; //
class Projection; //

// to allow CUDA access to the core UnitVars information, we break out the
// core unitvars info in a separate header that is common..
#include <UnitVars_core>

eTypeDef_Of(UnitVars);

class E_API UnitVars : public UnitVars_core {
  // ##NO_TOKENS ##CAT_Network Generic unit variables -- basic computational unit of a neural network (e.g., a neuron-like processing unit) -- Unit contains all the structural information, and this class just contains the computationally-relevant variables
public:
  UnitSpec*  unit_spec;
  // UnitSpec that has all the methods for processing information within these variables

  inline int    ThrNo(Network* net) const;
  // #IGNORE get thread number that owns this unit -- all methods on this unitvar MUST use the correct thread number for the thread that owns it!  so if accessing things at random you must use this method to get the thr_no!
  inline Unit*  Un(Network* net, int thr_no) const;
  // #IGNORE #CAT_Structure get unit that corresponds with these unit variables

  inline int            NRecvConGps(Network* net, int thr_no) const;
  // #IGNORE #CAT_Structure get number of receiving connection groups (determined by number of active layer projections at time of build)
  inline int            NSendConGps(Network* net, int thr_no) const;
  // #IGNORE #CAT_Structure get number of sending connection groups (determined by number of active layer send_prjns at time of build)
  int                   NRecvConGpsSafe(Network* net, int thr_no) const;
  // #CAT_Structure get number of receiving connection groups (determined by number of active layer projections at time of build)
  int                   NSendConGpsSafe(Network* net, int thr_no) const;
  // #CAT_Structure get number of sending connection groups (determined by number of active layer send_prjns at time of build)
  inline ConGroup*      RecvConGroup(Network* net, int thr_no, int rcg_idx) const;
  // #IGNORE #CAT_Structure get receiving connection group at given index -- no safe range checking is applied to rcg_idx!
  inline ConGroup*      SendConGroup(Network* net, int thr_no, int scg_idx) const;
  // #IGNORE #CAT_Structure get sendingconnection group at given index -- no safe range checking is applied to scg_idx!
  ConGroup*             RecvConGroupSafe(Network* net, int thr_no, int rcg_idx) const;
  // #CAT_Structure get receiving connection group at given index
  ConGroup*             SendConGroupSafe(Network* net, int thr_no, int scg_idx) const;
  // #CAT_Structure get sendingconnection group at given index
  inline ConGroup*      RecvConGroupPrjn(Network* net, int thr_no, Projection* prjn) const;
  // #IGNORE #CAT_Structure get con group at given prjn->recv_idx -- if it is not in range, emits error message and returns NULL
  inline ConGroup*      SendConGroupPrjn(Network* net, int thr_no, Projection* prjn) const;
  // #IGNORE #CAT_Structure get con group at given prjn->send_idx -- if it is not in range, emits error message and returns NULL
};

#endif // UnitVars_h
