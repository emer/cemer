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

#ifndef FgBoWedgeGpPrjnSpec_h
#define FgBoWedgeGpPrjnSpec_h 1

// parent includes:
#include "leabra_def.h"
#include <TiledGpRFPrjnSpec>

// member includes:
#include <float_Matrix>
#include <taVector2i>

// declare all other types mentioned but not required to include:

TypeDef_Of(FgBoWedgeGpPrjnSpec);

class LEABRA_API FgBoWedgeGpPrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec connectvity with initial weights (when init_wts is set) configured in pattern of 4 1/4 circle wedges, and 4 90 degree straight segments with different border prefs, onto V2 border ownership connections -- recv group size must be 8 x depth where depth is number of depths represented in V2 layer -- 4 units are each quadrant of the wedge
INHERITED(TiledGpRFPrjnSpec)
public:
  float		dist_sigma;	// #CONDEDIT_ON_init_wts #DEF_0.8 sigma for gaussian for distance from center of circle representing the wedge
  float		ang_sigma;	// #CONDEDIT_ON_init_wts #DEF_1 sigma for gaussian around target angle -- how widely to connect units around the target angle given by the perpendicular to the radius line -- controls how tightly tuned the angles are
  float		wt_base;	// #CONDEDIT_ON_init_wts #DEF_0.25 base weight value for all connections (except from other depth, which are not even connected)
  float		wt_range;	// #CONDEDIT_ON_init_wts #DEF_0.5 range of weight values assigned by the gaussian functions of distance and angle, on top of wt_base
  
  float_Matrix	fgbo_weights;  // #READ_ONLY #NO_SAVE weights for FgBo projection -- serves as a stencil for the connection

  virtual float	ConWt_Wedge(int wedge, taVector2i& suc, taVector2i& su_geo, int sang_dx, int sdir);
  // connection weight in terms of send unit group coord (suc), sending angle index (0-3 in 45 deg incr), and bo direction (0-1) -- used for creating stencil
  virtual float	ConWt_Line(int line, taVector2i& suc, taVector2i& su_geo, int sang_dx, int sdir);
  // connection weight in terms of send unit group coord (suc), sending angle index (0-3 in 45 deg incr), and bo direction (0-1) -- used for creating stencil

  virtual void	CreateStencil();
  // create stencil -- always done as first step in connection function

  override void Connect_impl(Projection* prjn);
  override void	Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
				  int rgpidx, int sgpidx, int alloc_loop);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  TA_SIMPLE_BASEFUNS(FgBoWedgeGpPrjnSpec);
protected:
  override void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // FgBoWedgeGpPrjnSpec_h
