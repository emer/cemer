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

#ifndef FgBoEllipseGpPrjnSpec_h
#define FgBoEllipseGpPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:
#include <float_Matrix>
#include <taVector2i>

// declare all other types mentioned but not required to include:

eTypeDef_Of(FgBoEllipseGpPrjnEl);

class E_API FgBoEllipseGpPrjnEl : public taNBase {
  // #AKA_FgBoGroupingPrjnEl ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a figure-ground border-ownership grouping projection spec -- contains parameters for a specific sized item
INHERITED(taNBase)
public:
  int		con_radius;	// maximum distance for how far to connect in any one direction (in unit group units) -- this then determines most other things
  float		wt_radius;	// distance at which the weight value is maximum -- the target weidth of the grouping detector -- specified as a normalized proportion of the con_radius
  float		dist_sig;	// #DEF_0.2:0.4 sigma for gaussian distance compared to wt_radius target -- in normalized units as a function of wt_radius
  float		ang_sig;	// #DEF_1 sigma for gaussian around target angle -- how widely to connect units around the target angle given by the perpendicular to the radius line
  float		ellipse_ratio;	// #MAX_1 #MIN_0.01 ratio of short over long side for ellpisoid shapes -- values less than 1 produce an ellipse -- long side is always con_radius 
  float		ellipse_angle;	// #CONDSHOW_OFF_ellipse_ratio:1 angle in degrees for the orientation of the long axis of the ellipse -- 0 = horizontal, 90 = vertical, etc.
  float		max_wt;		// magnitude multiplier for all weights -- determines the maximum weight value
  float		min_wt;		// #DEF_0.1 minimum weight value -- weights cannot go below this value -- this is applied after con_thr -- can be useful to retain some weight values to enable subsequent learning
  float		con_thr;	// #DEF_0.2 threshold for making a connection -- weight values below this are not connected -- set to a low value to allow learning

  ///////// use stencils to speed processing
  float_Matrix	fgbo_weights;  // #READ_ONLY #NO_SAVE weights for FgBo projection -- serves as a stencil for the connection

  virtual float	ConWt(taVector2i& suc, int sang_dx, int sdir);
  // connection weight in terms of send unit group coord (suc), sending angle index (0-3 in 45 deg incr), and bo direction (0-1) -- used for creating stencil

  virtual void	CreateStencil();
  // create stencil -- always done as first step in connection function

  TA_SIMPLE_BASEFUNS(FgBoEllipseGpPrjnEl);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(FgBoEllipseGpPrjnEl_List);

class E_API FgBoEllipseGpPrjnEl_List : public taList<FgBoEllipseGpPrjnEl> {
  // #AKA_FgBoGroupingPrjnEl_List ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec list of FgBoEllipseGpPrjnEl objects
INHERITED(taList<FgBoEllipseGpPrjnEl>)
public:
  TA_BASEFUNS_LITE_NOCOPY(FgBoEllipseGpPrjnEl_List);
private:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
};

eTypeDef_Of(FgBoEllipseGpPrjnSpec);

class E_API FgBoEllipseGpPrjnSpec : public ProjectionSpec {
  // #AKA_FgBoGroupingPrjnSpec figure-ground border-ownership grouping projection spec -- 
INHERITED(ProjectionSpec)
public:
  bool		wrap;		// #DEF_true wrap around layer coordinates (else clip at ends)
  bool		reciprocal; 	// set this for connections going the opposite direction, from grouping back to V2 Bo units
  FgBoEllipseGpPrjnEl_List	group_specs; // specifications for each grouping size

  virtual void	CreateStencils();
  // create stencil -- always done as first step in connection function

  override void	Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual FgBoEllipseGpPrjnEl* NewGroupSpec();
  // #BUTTON create a new group_specs item for specifying one grouping size

  TA_SIMPLE_BASEFUNS(FgBoEllipseGpPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // FgBoEllipseGpPrjnSpec_h
