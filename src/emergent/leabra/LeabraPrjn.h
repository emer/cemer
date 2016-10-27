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

#ifndef LeabraPrjn_h
#define LeabraPrjn_h 1

// parent includes:
#include "network_def.h"
#include <Projection>

// member includes:
#include <DMemAggVars>

// declare all other types mentioned but not required to include:
class LeabraNetwork; // 

eTypeDef_Of(LeabraPrjn);


class E_API LeabraPrjn: public Projection {
  // #STEM_BASE ##CAT_Leabra leabra specific projection -- has special variables at the projection-level
INHERITED(Projection)
public:
  float		netin_avg;		// #NO_SAVE #READ_ONLY #SHOW #CAT_Statistic average netinput values for the recv projections into this layer
  float		netin_rel;		// #NO_SAVE #READ_ONLY #SHOW #CAT_Statistic relative netinput values for the recv projections into this layer

  float		avg_netin_avg;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Statistic average netinput values for the recv projections into this layer, averaged over an epoch
  float		avg_netin_avg_sum;	// #NO_SAVE #READ_ONLY #HIDDEN #DMEM_AGG_SUM #CAT_Statistic average netinput values for the recv projections into this layer, sum over an epoch
  float		avg_netin_rel;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Statistic relative netinput values for the recv projections into this layer, averaged over an epoch
  float		avg_netin_rel_sum;	// #NO_SAVE #READ_ONLY #HIDDEN #DMEM_AGG_SUM #CAT_Statistic relative netinput values for the recv projections into this layer, sum over an epoch (for computing average)
  int		avg_netin_n;		// #NO_SAVE #READ_ONLY #HIDDEN #DMEM_AGG_SUM #CAT_Statistic count for computing epoch-level averages
  float         bal_sum_max;            // #NO_SAVE #READ_ONLY #SHOW #CAT_Statistic maximum weight balance sum factor (sum of extent weights are over high threshold) across connections in this projection -- only computed if weight normalization is on, as part of that process
  float         bal_sum_avg;            // #NO_SAVE #READ_ONLY #SHOW #CAT_Statistic average weight balance sum factor (sum of extent weights are over high threshold) across connections in this projection -- only computed if weight normalization is on, as part of that process

  virtual void	Trial_Init_Specs(LeabraNetwork* net);
  // #CAT_Learning initialize specs and specs update network flags -- e.g., set current learning rate based on epoch
  virtual void	CheckInhibCons(LeabraNetwork* net);
  // #CAT_Structure check for inhibitory connections -- sets flag on network

  virtual void	Init_Stats();	// #CAT_Statistic intialize statistic counters

#ifdef DMEM_COMPILE
  DMemAggVars	dmem_agg_sum;		// #IGNORE aggregation of network variables using SUM op (currently only OP in use -- add others as needed)
  virtual void 	DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void	DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network variables across procs for trial-level dmem 
#endif

  void	Copy_(const LeabraPrjn& cp);
  TA_BASEFUNS(LeabraPrjn);
private:
  void 	Initialize();
  void 	Destroy();
};

#endif // LeabraPrjn_h
