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

#ifndef V1LateralContourPrjnSpec_h
#define V1LateralContourPrjnSpec_h 1

// parent includes:
#include "leabra_def.h"
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(V1LateralContourPrjnSpec);

class LEABRA_API V1LateralContourPrjnSpec : public ProjectionSpec {
  // lateral projections within V1 layer to support continuation of contours -- helps make edges more robust to differences in bottom-up strength, based on principle of good continuation -- requires unit groups within layer where x dim represents angle
INHERITED(ProjectionSpec)
public:
  int		radius;		// #DEF_2:10 how far to connect in any one direction (in unit group units)
  bool		wrap;		// #DEF_true wrap around layer coordinates (else clip at ends)
  float		ang_pow;	// #DEF_4 wt = (angle - feature_angle)^ang_pow -- values > 1 result in a more focal distribution for close angles, and less weight at off-angles.
  float		dist_sigma;	// #DEF_1 sigma for gaussian function of distance -- how much the weight drops off as a function of distance (multiplies angle weights) -- in normalized units relative to the radius
  float		con_thr;	// #DEF_0.2 threshold for making a connection -- weight values below this are not even connected
  float		oth_feat_wt;	// #DEF_0.5 weight multiplier for units that have a different featural encoding (e.g., on-center vs. off-center or another color contrast) -- encoded by y axis of unit group

  override void	Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  TA_SIMPLE_BASEFUNS(V1LateralContourPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // V1LateralContourPrjnSpec_h
