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

#ifndef V1BinocularSpec_h
#define V1BinocularSpec_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API V1BinocularSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 binocular cells -- specifies basic disparity coding system -- number and spacing of disparity tuned cells
INHERITED(taOBase)
public:
  bool		mot_in;		// use motion v1m_maxout signals as the input to disparity computation -- only moving objects will have depth mapping -- a quick and dirty way to drive saccade control, for example -- only valid if motion_frames > 1, and must not set r_only in v1s_motion specs (all these conditions are enforced!)
  int		n_disps;	// #DEF_0:2 number of different disparities encoded in each direction away from the focal plane (e.g., 1 = -1 near, 0 = focal, +1 far) -- each disparity tuned cell responds to a range of actual disparities around a central value, defined by disp * disp_off
  float		disp_range_pct;  // #DEF_0.05:0.1 range (half width) of disparity tuning around central offset value for each disparity cell -- expressed as proportion of total V1S image width -- total disparity tuning width for each cell is 2*disp_range + 1, and activation is weighted by gaussian tuning over this range (see gauss_sig)
  float		gauss_sig; 	// #DEF_0.7:1.5 gaussian sigma for weighting the contribution of different disparities over the disp_range -- expressed as a proportion of disp_range -- last disparity on near/far ends does not come back down from peak gaussian value (ramp to plateau instead of gaussian)
  float		disp_spacing;	// #DEF_2:2.5 spacing between different disparity detector cells in terms of disparity offset tunings -- expressed as a multiplier on disp_range -- this should generally remain the default value of 2, so that the space is properly covered by the different disparity detectors, but 2.5 can also be useful to not have any overlap between disparities to prevent ambiguous activations (e.g., for figure-ground segregation)
  int		end_extra;	// #DEF_2 extra disparity detecting range on the ends of the disparity spectrum (nearest and farthest detector cells) -- adds beyond the disp_range -- to extend out and capture all reasonable disparities -- expressed as a multiplier on disp_range 
  bool		fix_horiz;	// #DEF_true fix horizontal disparities by anchoring to values at the ends of horizontal line segments, which are the only places that have reliable disparity signals in this case
  float		horiz_thr;	// #DEF_0.2 threshold activity level for counting a horizontal line

  int		disp_range; 	// #READ_ONLY #SHOW range (half width) of disparity tuning around central offset value for each disparity cell -- integer value computed from disp_range_pct -- total disparity tuning width for each cell is 2*disp_range + 1, and activation is weighted by gaussian tuning over this range (see gauss_sig)
  int		disp_spc;	// #READ_ONLY #SHOW integer value of spacing between different disparity detector cells -- computed from disp_spacing and disp_range
  int		end_ext;	// #READ_ONLY #SHOW integer value of extra disparity detecting range on the ends of the disparity spectrum (nearest and farthest detector cells) -- adds beyond the disp_range -- to extend out and capture all reasonable disparities

  int		tot_disps;	// #READ_ONLY total number of disparities coded: 1 + 2 * n_disps
  int		max_width;	// #READ_ONLY maximum total width (1 + 2 * disp_range + end_ext)
  int		max_off;	// #READ_ONLY maximum possible offset -- furthest point out in any of the stencils
  int		tot_offs;	// #READ_ONLY 1 + 2 * max_off
  float		ambig_wt;	// #READ_ONLY disparity activation weight for ambiguous locations

  virtual void	UpdateFmV1sSize(int v1s_img_x) {
    disp_range = (int)((disp_range_pct * (float)v1s_img_x) + 0.5f);
    disp_spc = (int)(disp_spacing * (float)disp_range);
    end_ext = end_extra * disp_range;
    max_width = 1 + 2*disp_range + end_ext;
    max_off = n_disps * disp_spc + disp_range + end_ext;
    tot_offs = 1 + 2 * max_off;
  }

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1BinocularSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

#endif // V1BinocularSpec_h
