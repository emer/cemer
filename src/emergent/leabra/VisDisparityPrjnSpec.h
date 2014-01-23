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

#ifndef VisDisparityPrjnSpec_h
#define VisDisparityPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:
#include <int_Matrix>
#include <float_Matrix>

// declare all other types mentioned but not required to include:

eTypeDef_Of(VisDisparityPrjnSpec);

class E_API VisDisparityPrjnSpec : public ProjectionSpec {
  // visual disparity projection spec: receiving layer units within groups encode different offset disparities (near..far) from two sending layers (first prjn MUST be right eye, second MUST be left eye -- right is just one-to-one dominant driver) -- should have same gp_geom as sending layer gp_geom -- features within sending gps are replicated for each disparity -- MUST only have one of these per configuration of sending / recv layers, as local data is stored and cached from connection for use in initweights
INHERITED(ProjectionSpec)
public:
  int		n_disps;	// #DEF_0:3 number of different disparities encoded in each direction away from the focal plane (e.g., 1 = -1 near, 0 = focal, +1 far) -- each disparity tuned cell responds to a range of actual disparities around a central value, defined by disp * disp_off
  float		disp_range_pct;  // #DEF_0.02:0.1 range (half width) of disparity tuning around central offset value for each disparity cell -- expressed as proportion of total input image width -- total disparity tuning width for each cell is 2*disp_range + 1, and activation is weighted by gaussian tuning over this range (see gauss_sig)
  float		gauss_sig; 	// #CONDEDIT_ON_init_wts #DEF_0.7:1.5 gaussian sigma for weighting the contribution of different disparities over the disp_range -- expressed as a proportion of disp_range -- last disparity on near/far ends does not come back down from peak gaussian value (ramp to plateau instead of gaussian)
  float		disp_spacing;	// #DEF_2:2.5 spacing between different disparity detector cells in terms of disparity offset tunings -- expressed as a multiplier on disp_range -- this should generally remain the default value of 2, so that the space is properly covered by the different disparity detectors, but 2.5 can also be useful to not have any overlap between disparities to prevent ambiguous activations (e.g., for figure-ground segregation)
  int		end_extra;	// #DEF_2 extra disparity detecting range on the ends of the disparity spectrum (nearest and farthest detector cells) -- adds beyond the disp_range -- to extend out and capture all reasonable disparities -- expressed as a multiplier on disp_range 
  bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -rf_move (wrapped to right/top) and goes +rf_move past the right/top edge (wrapped to left/bottom)

  int		disp_range; 	// #READ_ONLY #SHOW range (half width) of disparity tuning around central offset value for each disparity cell -- integer value computed from disp_range_pct -- total disparity tuning width for each cell is 2*disp_range + 1, and activation is weighted by gaussian tuning over this range (see gauss_sig)
  int		disp_spc;	// #READ_ONLY #SHOW integer value of spacing between different disparity detector cells -- computed from disp_spacing and disp_range
  int		end_ext;	// #READ_ONLY #SHOW integer value of extra disparity detecting range on the ends of the disparity spectrum (nearest and farthest detector cells) -- adds beyond the disp_range -- to extend out and capture all reasonable disparities

  int		tot_disps;	// #READ_ONLY total number of disparities coded: 1 + 2 * n_disps
  int		max_width;	// #READ_ONLY maximum total width (1 + 2 * disp_range + end_ext)
  int		max_off;	// #READ_ONLY maximum possible offset -- furthest point out in any of the stencils
  int		tot_offs;	// #READ_ONLY 1 + 2 * max_off

  int_Matrix	v1b_widths; 	// #READ_ONLY #NO_SAVE width of stencils for binocularity detectors 1d: [tot_disps]
  float_Matrix	v1b_weights;	// #READ_ONLY #NO_SAVE v1 binocular gaussian weighting factors for integrating disparity values into v1b unit activations -- for each tuning disparity [max_width][tot_disps] -- only v1b_widths[disp] are used per disparity
  int_Matrix	v1b_stencils; 	// #READ_ONLY #NO_SAVE stencils for binocularity detectors, in terms of v1s location offsets per image: 2d: [XY][max_width][tot_disps]

  void Connect_impl(Projection* prjn) CPP11_OVERRIDE;
  void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) CPP11_OVERRIDE;

  virtual void InitStencils(Projection* prjn);
  // initialize the stencils for given geometry of layers for this projection -- called at Connect_LeftEye
  virtual void Connect_RightEye(Projection* prjn);
  // right eye is simple replicated one-to-one connectivity
  virtual void Connect_LeftEye(Projection* prjn);
  // left eye has all the disparity integration connections

  virtual void	UpdateFmV1sSize(int v1s_img_x) {
    disp_range = (int)((disp_range_pct * (float)v1s_img_x) + 0.5f);
    disp_spc = (int)(disp_spacing * (float)disp_range);
    end_ext = end_extra * disp_range;
    max_width = 1 + 2*disp_range + end_ext;
    max_off = n_disps * disp_spc + disp_range + end_ext;
    tot_offs = 1 + 2 * max_off;
  }

  TA_SIMPLE_BASEFUNS(VisDisparityPrjnSpec);
protected:
  void UpdateAfterEdit_impl() CPP11_OVERRIDE;
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // VisDisparityPrjnSpec_h
