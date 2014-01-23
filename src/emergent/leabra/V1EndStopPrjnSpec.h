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

#ifndef V1EndStopPrjnSpec_h
#define V1EndStopPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:
#include <float_Matrix>
#include <int_Matrix>

// declare all other types mentioned but not required to include:

eTypeDef_Of(V1EndStopPrjnSpec);

class E_API V1EndStopPrjnSpec : public ProjectionSpec {
  // end-stop detectors within V1 layer -- connectivity and weights that enable units to detect when one orientation terminates into another -- recv layer must have unit groups with one row of n_angles units, while sender has multiple rows of n_angles units (recv integrates over rows)
INHERITED(ProjectionSpec)
public:
  enum LnOrtho {   // line, orthogonal to the line -- for v1s_ang_slopes
    LINE,	   // along the direction of the line
    ORTHO,	   // orthogonal to the line
  };
  enum XY {	   // x, y component of stencils etc -- for clarity in code
    X,
    Y,
  };

  int		n_angles;	// #DEF_4 number of angles in both the input and recv layer
  int		end_stop_dist;	// #DEF_2 end-stop distance factor -- how far away from the central point should we look for opposing orientations
  float		adjang_wt;	// #DEF_0.2 weight for adjacent angles in the end stop computation -- adjacent angles are often activated for edges that are not exactly aligned with the gabor angles, so they can result in false positives
  bool		wrap;		// #DEF_true wrap around layer coordinates (else clip at ends) -- only wrap is currently supported and will be enforced automatically

  float_Matrix	v1s_ang_slopes; // #READ_ONLY #NO_SAVE angle slopes [dx,dy][line,ortho][angles] -- dx, dy slopes for lines and orthogonal lines for each fo the angles
  int_Matrix	v1c_es_stencils;  // #READ_ONLY #NO_SAVE stencils for complex end stop cells [x,y][sum_line=2][max_line=2][angles]
  float_Matrix	v1c_es_angwts;  // #READ_ONLY #NO_SAVE weights for different angles relative to a given angle [n_angles][n_angles]

  void	Connect_impl(Projection* prjn) CPP11_OVERRIDE;
  void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) CPP11_OVERRIDE;

  virtual void	InitStencils(Projection* prjn);
  // initialize stencils -- does not depend on prjn, only params (spec can be reused for any prjn)

  TA_SIMPLE_BASEFUNS(V1EndStopPrjnSpec);
protected:
  void UpdateAfterEdit_impl() CPP11_OVERRIDE;
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // V1EndStopPrjnSpec_h
