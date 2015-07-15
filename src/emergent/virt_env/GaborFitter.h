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

#ifndef GaborFitter_h
#define GaborFitter_h 1

// parent includes:
#include <GaborFilter>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(GaborFitter);

class TA_API GaborFitter : public GaborFilter {
  // ##CAT_Image fits a gabor filter from image data
INHERITED(GaborFilter)
public:
  float		fit_dist;	// #READ_ONLY #SHOW fit distance

  virtual float ParamDist(const GaborFilter& oth);
  // #CAT_GaborFilter return euclidian distance between parameters for this spec and the other one

//   virtual float	FitData(float_Matrix& data_vals, bool use_cur_vals = false);
  // find best-fitting parameters for given data.  use_cur_vals = use current values as initial reasonable guess (skip first-pass search)
//   virtual float	FitData_firstpass(float_Matrix& data_vals);
//   virtual float	SquaredDist(float_Matrix& data_vals);

//   virtual float	TestFit();	// #BUTTON test the fitting function
 
  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborFitter);
};

#endif // GaborFitter_h
