// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef V2BordOwnSpec_h
#define V2BordOwnSpec_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API V2BordOwnSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v2 border ownership cells, which compute T and L junctions on top of V1 inputs
INHERITED(taOBase)
public:
  int		lat_itrs;	// #DEF_10 how many iterations of lateral propagation to perform?
  float		lat_dt;		// #DEF_0.5 integration rate for lateral propagation
  float		act_thr;	// #DEF_0.1 general activity threshold for doing V2 BO computations -- if below this value, just set ambiguously -- also threshold for using T,L junction output to drive BO
  float		ambig_gain;	// #DEF_0.2 gain multiplier for ambiguous length sum activation -- also serves as a threshold for communicating ambiguity
  float		l_t_inhib_thr;	// #DEF_0.2 threshold on max L-junction activity within a group to then inhibit T junctions within the same group -- don't want weak L's just weakening the T's
  int		depths_out;	// #MIN_1 number of depth replications of the V2 BO output -- just replicates the output this many times in the y dimension of the unit group output
  int		depth_idx;	// output only this specific depth index -- -1 = all

  float		ffbo_gain;	// #READ_ONLY 1-ambig_gain

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V2BordOwnSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

#endif // V2BordOwnSpec_h
