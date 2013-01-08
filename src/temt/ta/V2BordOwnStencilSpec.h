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

#ifndef V2BordOwnStencilSpec_h
#define V2BordOwnStencilSpec_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API V2BordOwnStencilSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v2 border ownership stencils for neighborhood connectivity
INHERITED(taOBase)
public:
  float		gain;		// #DEF_4 gain on strength of ff bo inputs -- multiplies average netinput values from ffbo stencils
  int		radius;		// #DEF_3:8 for all curved angles, how far to connect in any one direction (in unit group units)
  bool		t_on;		// #DEF_true turn on the special T junction detector weights -- only for a 90 degree angle perpendicular, behind the border edge
  bool		opp_on;		// #DEF_false make connections from opponent border unit (same orientation, opposite BO coding) -- can help to resolve long rectalinear elements
  float		ang_sig;	// #DEF_0.5 sigma for gaussian around target angle -- same for all
  float		dist_sig;	// #DEF_0.8 sigma for gaussian distance -- for other angles (delta-angle != 0) -- should in general go shorter than for the linear case
  float		weak_mag;	// #DEF_0.5 weaker magnitude -- applies to acute angle intersections
  float		con_thr;	// #DEF_0.2 threshold for making a connection -- weight values below this are not even connected

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V2BordOwnStencilSpec);
};

#endif // V2BordOwnStencilSpec_h
