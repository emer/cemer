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

eTypeDef_Of(UnitVars);

class E_API UnitVars {
  // ##NO_TOKENS ##CAT_Network Generic unit variables -- basic computational unit of a neural network (e.g., a neuron-like processing unit) -- Unit contains all the structural information, and this class just contains the computationally-relevant variables
public:
  enum ExtFlags {        // #BITS indicates type of external input; some flags used in Layer to control usage
    NO_EXTERNAL         = 0x0000, // #NO_BIT no input
    TARG                = 0x0001, // a target value used to train the network (value goes in targ field of unit)
    EXT                 = 0x0002, // an external input value that drives activations (value goes in ext field of unit)
    COMP                = 0x0004, // a comparison value used for computing satistics but not training the network (value goes in targ field of unit)
    TARG_EXT            = 0x0003, // #NO_BIT as both external input and target value
    COMP_TARG           = 0x0005, // #NO_BIT as a comparision and target layer
    COMP_EXT            = 0x0006, // #NO_BIT as a comparison and external input layer
    COMP_TARG_EXT       = 0x0007, // #NO_BIT as a comparison, target, and external input layer
    LESIONED            = 0x0010, // #READ_ONLY unit is temporarily lesioned (inactivated for all network-level processing functions) -- copied from Unit -- should not be set directly here
  };

  ExtFlags      ext_flag;
  // #GUI_READ_ONLY #SHOW #CAT_Activation tells what kind of external input unit received -- this is normally set by the ApplyInputData function -- it is not to be manipulated directly
  UnitSpec*     unit_spec;
  // UnitSpec that has all the methods for processing information within these variables
  int           thr_un_idx;
  // thread-based unit index where this unitvars lives
  float         targ;
  // #VIEW_HOT #CAT_Activation target value: drives learning to produce this activation value
  float         ext;
  // #VIEW_HOT #CAT_Activation external input: drives activation of unit from outside influences (e.g., sensory input)
  float         act;
  // #VIEW_HOT #CAT_Activation activation value -- what the unit communicates to others
  float         net;
  // #VIEW_HOT #CAT_Activation net input value -- what the unit receives from others (typically sum of sending activations times the weights)
  float         bias_wt;
  // #VIEW_HOT #CAT_Bias bias weight value -- the bias weight acts like a connection from a unit that is always active with a constant value of 1 -- reflects intrinsic excitability from a biological perspective
  float         bias_dwt;
  // #VIEW_HOT #CAT_Bias change in bias weight value as computed by a learning mechanism

  inline void   SetExtFlag(int flg)   { ext_flag = (ExtFlags)(ext_flag | flg); }
  // set flag state on
  inline void   ClearExtFlag(int flg) { ext_flag = (ExtFlags)(ext_flag & ~flg); }
  // clear flag state (set off)
  inline bool   HasExtFlag(int flg) const { return (ext_flag & flg); }
  // check if flag is set
  inline void   SetExtFlagState(int flg, bool on)
  { if(on) SetExtFlag(flg); else ClearExtFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  inline bool   lesioned() const { return HasExtFlag(LESIONED); }
  // check if this unit is lesioned -- must check for all processing functions (threaded calls automatically exclude lesioned units)

  inline Unit*  Un(Network* net, int thr_no) const;
  // #IGNORE #CAT_Structure get unit that corresponds with these unit variables
  inline int    UnFlatIdx(Network* net, int thr_no) const;
  // #IGNORE #CAT_Structure get unit flat index that corresponds with these unit variables

  inline int            NRecvConGps(Network* net, int thr_no) const;
  // #IGNORE #CAT_Structure get number of receiving connection groups (determined by number of active layer projections at time of build)
  inline int            NSendConGps(Network* net, int thr_no) const;
  // #IGNORE #CAT_Structure get number of sending connection groups (determined by number of active layer send_prjns at time of build)
  inline ConGroup*      RecvConGroup(Network* net, int thr_no, int rcg_idx) const;
  // #IGNORE #CAT_Structure get receiving connection group at given index -- no safe range checking is applied to rcg_idx!
  inline ConGroup*      SendConGroup(Network* net, int thr_no, int scg_idx) const;
  // #IGNORE #CAT_Structure get sendingconnection group at given index -- no safe range checking is applied to scg_idx!
  inline ConGroup*      RecvConGroupPrjn(Network* net, int thr_no, Projection* prjn);
  // #IGNORE #CAT_Structure get con group at given prjn->recv_idx -- if it is not in range, emits error message and returns NULL
  inline ConGroup*      SendConGroupPrjn(Network* net, int thr_no, Projection* prjn);
  // #IGNORE #CAT_Structure get con group at given prjn->send_idx -- if it is not in range, emits error message and returns NULL
};

#endif // UnitVars_h
