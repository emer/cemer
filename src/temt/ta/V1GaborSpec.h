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

#ifndef V1GaborSpec_h
#define V1GaborSpec_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:
class float_Matrix; // 
class DataTable; // 


class TA_API V1GaborSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 simple cells as gabor filters: 2d Gaussian envelope times a sinusoidal plane wave -- by default produces 2 phase asymmetric edge detector filters
INHERITED(taOBase)
public:
  float		gain;		// #DEF_2 overall gain multiplier applied after gabor filtering -- only relevant if not using renormalization (otherwize it just gets renormed away)
  int		n_angles;	// #DEF_4 number of different angles encoded -- currently only 4 is supported
  int		filter_size;	// #DEF_6;12;18;24 size of the overall filter -- number of pixels wide and tall for a square matrix used to encode the filter -- filter is centered within this square
  int		spacing;	// how far apart to space the centers of the gabor filters -- 1 = every pixel, 2 = every other pixel, etc -- high-res should be 1, lower res can be increments therefrom
  float		wvlen;		// #DEF_6;12;18;24 (values = filter_size work well) wavelength of the sine waves -- number of pixels over which a full period of the wave takes place (computation adds a 2 PI factor to translate into pixels instead of radians)
  float		gauss_sig_len;	// #DEF_0.25:0.3 gaussian sigma for the length dimension (elongated axis perpendicular to the sine waves) -- normalized as a function of filter_size
  float		gauss_sig_wd;	// #DEF_0.2 gaussian sigma for the width dimension (in the direction of the sine waves) -- normalized as a function of filter_size
  float		phase_off;	// #DEF_0;1.5708 offset for the sine phase -- can make it into a symmetric gabor by using PI/2 = 1.5708
  bool		circle_edge;	// #DEF_true cut off the filter (to zero) outside a circle of diameter filter_size -- makes the filter more radially symmetric

  virtual void	RenderFilters(float_Matrix& fltrs);
  // generate filters into the given matrix, which is formatted as: [filter_size][filter_size][n_angles]

  virtual void	GridFilters(float_Matrix& fltrs, DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filters into data table and generate a grid view (reset any existing data first)

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1GaborSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

#endif // V1GaborSpec_h
