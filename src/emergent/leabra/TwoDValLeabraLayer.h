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

#ifndef TwoDValLeabraLayer_h
#define TwoDValLeabraLayer_h 1

// parent includes:
#include <LeabraLayer>

// member includes:
#include <float_Matrix>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TwoDValLeabraLayer);

class E_API TwoDValLeabraLayer : public LeabraLayer {
  // represents one or more two-d value(s) using a coarse-coded distributed code over units.  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid -- uses separate matrix storage of x,y values (prev impl used first row of layer)
INHERITED(LeabraLayer)
public:
  enum TwoDXY {			// x-y two-d vals
    TWOD_X,			// the horizontal (X) value encoded in the layer
    TWOD_Y,			// the vertical (Y) value encoded in the layer
    TWOD_XY,			// number of xy vals (2)
  };

  enum TwoDValTypes {		// different values encoded in the twod_vals matrix
    TWOD_EXT,			// external inputs
    TWOD_TARG,			// target values
    TWOD_ACT,			// current activation
    TWOD_ACT_M,			// minus phase activations
    TWOD_ACT_P,			// plus phase activations
    TWOD_ACT_DIF,		// difference between plus and minus phase activations
    TWOD_ACT_M2,		// second minus phase activations
    TWOD_ACT_P2,		// second plus phase activations
    TWOD_ACT_DIF2,		// difference between second plus and minus phase activations
    TWOD_ERR,			// error from target: targ - act_m
    TWOD_SQERR,			// squared error from target: (targ - act_m)^2
    TWOD_N,			// number of val types to encode
  };
  
  float_Matrix		twod_vals; // #SHOW_TREE matrix of layer-encoded values, dimensions: [gp_y][gp_x][n_vals][TWOD_N][TWOD_XY] (outer to inner) -- gp_y and gp_x are group indices, size 1,1, for a layer with no unit groups

  inline float	GetTwoDVal(TwoDXY xy, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    return twod_vals.SafeElAsFloat(xy, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD get a two-d value encoded in the twod_vals data 
  inline void	GetTwoDVals(float& x_val, float& y_val, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    x_val = twod_vals.SafeElAsFloat(TWOD_X, val_typ, val_no, gp_x, gp_y);
    y_val = twod_vals.SafeElAsFloat(TWOD_Y, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD get a two-d value encoded in the twod_vals data 

  inline void	SetTwoDVal(const Variant& val, TwoDXY xy, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    twod_vals.SetFmVar(val, xy, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD set a two-d value encoded in the twod_vals data 
  inline void	SetTwoDVals(const Variant& x_val, const Variant& y_val, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    twod_vals.SetFmVar(x_val, TWOD_X, val_typ, val_no, gp_x, gp_y);
    twod_vals.SetFmVar(y_val, TWOD_Y, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD set both two-d values encoded in the twod_vals data 

  virtual void		UpdateTwoDValsGeom();
  // update the twod_vals geometry based on current layer and layer spec settings

  TA_SIMPLE_BASEFUNS(TwoDValLeabraLayer);
protected:
  override void	UpdateAfterEdit_impl();

  override void	ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
				  Random* ran, const taVector2i& offs, bool na_by_range=false);
  override void	ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
				      Random* ran, const taVector2i& offs, bool na_by_range=false);
  override void	ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags,
				    Random* ran, bool na_by_range=false);

private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // TwoDValLeabraLayer_h
