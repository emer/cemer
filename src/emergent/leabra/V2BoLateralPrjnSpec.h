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

#ifndef V2BoLateralPrjnSpec_h
#define V2BoLateralPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:
#include <float_Matrix>
#include <taVector2i>

// declare all other types mentioned but not required to include:

eTypeDef_Of(V2BoLateralPrjnSpec);

class E_API V2BoLateralPrjnSpec : public ProjectionSpec {
  // lateral projections within V2 layer to support border ownership computation
INHERITED(ProjectionSpec)
public:
  int		radius;		// #DEF_2:10 how far to connect in any one direction (in unit group units)
  bool		wrap;		// #DEF_true wrap around layer coordinates (else clip at ends)
  bool		t_on;		// #DEF_true turn on the special T junction detector weights -- only for a 90 degree angle perpendicular, behind the border edge
  bool		opp_on;		// #DEF_true make connections from opponent border unit (same orientation, opposite BO coding) -- can help to resolve long rectalinear elements
  float		ang_sig;	// #DEF_0.5 sigma for gaussian around target angle -- same for all
  float		dist_sig_line;	// #DEF_0.8 sigma for gaussian distance -- for linear continuation case (delta-angle = 0) -- should in general go longer than for the off-angle cases
  float		dist_sig_oth;	// #DEF_0.5 sigma for gaussian distance -- for other angles (delta-angle != 0) -- should in general go shorter than for the linear case
  float		line_mag;	// #DEF_0.8 magnitude for the linear continuation case -- can actually make this a bit weaker b/c it is non-descriminative
  float		weak_mag;	// #DEF_0.5 weaker magnitude -- applies to acute angle intersections
  float		con_thr;	// #DEF_0.2 threshold for making a connection -- weight values below this are not even connected
  int		test_ang;	// #DEF_-1 #MIN_-1 #MAX_7 set to 0..7, computes only that delta-angle's worth of connectivity (0 = same angle between send and recv, 1 = 45 delta, 2 = 90 delta, 3 = 135 delta) -- useful for visually tuning the receptive field parameters separately

  ///////// use stencils to speed processing
  float_Matrix	v2ffbo_weights;  // #READ_ONLY #NO_SAVE weights for V2 feedforward border ownership inputs from length sum

  virtual float	ConWt(taVector2i& suc, int rang_dx, int sang_dx, int rdir, int sdir);
  // connection weight in terms of send unit group coord (suc), recv angle index (0-3 in 45 deg incr), and send angle index (0-3), and r/s bo direction (0-1) -- used for creating stencil
  virtual void	CreateStencils();
  // create stencil -- always done as first step in connection function

  void	Connect_impl(Projection* prjn) override;
  void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) override;

  TA_SIMPLE_BASEFUNS(V2BoLateralPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // V2BoLateralPrjnSpec_h
