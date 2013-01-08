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

#ifndef DoGFilter_h
#define DoGFilter_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <float_Matrix>

// declare all other types mentioned but not required to include:
class DataTable; // 


class TA_API DoGFilter : public taNBase {
  // #STEM_BASE #INLINE ##CAT_Image defines a difference-of-gaussians (center minus surround or "mexican hat") filter that highlights contrast in an image
  INHERITED(taNBase)
public:
#ifdef __MAKETA__
  String	name;		// #HIDDEN_INLINE name of object
#endif

  int		filter_width;	// half-width of the filter (typically 2 * off_sigma)
  int		filter_size;	// #READ_ONLY size of the filter: 2 * width + 1
  float		on_sigma;	// width of the narrower central 'on' gaussian
  float		off_sigma;	// width of the wider surround 'off' gaussian (typically 2 * on_sigma)
  int		spacing;	// spacing between filters -- should be same as on_sigma
  bool		circle_edge;	// #DEF_true cut off the filter (to zero) outside a circle of radius filter_width -- makes the filter more radially symmetric
  float_Matrix	on_filter;	// #READ_ONLY #NO_SAVE #NO_COPY on-gaussian 
  float_Matrix	off_filter;	// #READ_ONLY #NO_SAVE #NO_COPY off-gaussian (values are positive)
  float_Matrix	net_filter;	// #READ_ONLY #NO_SAVE #NO_COPY net overall filter -- on minus off

  inline float	FilterPoint(int x, int y, float img_val) {
    return img_val * net_filter.FastEl(x+filter_width, y+filter_width);
  }
  // #CAT_DoGFilter apply filter at given x,y point (-filter_width..filter_width) given image value (which can be either luminance or color contrast
  float		InvertFilterPoint(int x, int y, float act) {
    return 0.5f * act * net_filter.FastEl(x+filter_width, (int)(y+filter_width + 0.5f));
  }
  // #CAT_DoGFilter invert filter at given x,y point with net activation, returning image value -- this is probably not very accurate

  virtual void	RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt, float_Matrix& net_flt);
  // #CAT_DoGFilter render filter into matrix
  virtual void	UpdateFilter();
  // #CAT_DoGFilter make our personal filter (RenderFilter(filter)) according to current params

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a graph
  virtual void	GridFilter(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a grid view (reset any existing data first)

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGFilter);
protected:
  void	UpdateAfterEdit_impl();
};

#endif // DoGFilter_h
