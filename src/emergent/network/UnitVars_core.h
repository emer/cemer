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

#ifndef UnitVars_core_h
#define UnitVars_core_h 1


class UnitVars_core {
  // this is the core data for UnitVars, which can be used between CUDA and standard C++ -- CUDA cannot parse Qt and other infrastructure
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
    UN_FLAG_1           = 0x0100, // #READ_ONLY misc unit-level flag for use by algorithms to set binary state (e.g., used in backprop to mark units that have been dropped)
    UN_FLAG_2           = 0x0200, // #READ_ONLY misc unit-level flag for use by algorithms to set binary state
    UN_FLAG_3           = 0x0400, // #READ_ONLY misc unit-level flag for use by algorithms to set binary state
    UN_FLAG_4           = 0x0800, // #READ_ONLY misc unit-level flag for use by algorithms to set binary state
  };

  ExtFlags      ext_flag;
  // #GUI_READ_ONLY #SHOW #CAT_Activation tells what kind of external input unit received -- this is normally set by the ApplyInputData function -- it is not to be manipulated directly
  int           flat_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of this unit in a flat array of units (used by parallel threading) -- 0 is special null case -- real idx's start at 1
  int           thr_un_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE thread-based unit index where this unitvars lives
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
#ifdef CUDA_COMPILE
  int           cuda_unit_spec_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE for cuda build, this is index of unit spec -- can't use the unit spec pointer
#endif  

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
  
};

class UnitVars_gen : public UnitVars_core {
  // generic version of UnitVars -- for generic code that works for regular c++ or cuda
public:
  void*   unit_spec; // keep our unit vars aligned with C++ one..
};


#endif // UnitVars_core_h
