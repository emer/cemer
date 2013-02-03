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

#ifndef LeabraV1LayerSpec_h
#define LeabraV1LayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:
#include <float_Matrix>
#include <int_Matrix>

// declare all other types mentioned but not required to include:
class LeabraUnit_Group; //

TypeDef_Of(V1FeatInhibSpec);

class LEABRA_API V1FeatInhibSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies inhibition parameters for feature inhibition
INHERITED(SpecMemberBase)
public:
  bool		on;		// #DEF_true whether to use neighborhood inhibition
  int		n_angles;	// #CONDSHOW_ON_on #DEF_4 number of different angles represented in the layer unit groups -- should be equivalent to x-axis dimension of number of units in the unit group
  int		inhib_d; 	// #CONDSHOW_ON_on #DEF_1 distance of neighborhood for inhibition to apply to same feature in neighboring locations spreading out on either side along the orthogonal direction relative to the orientation tuning
  float		inhib_g;	// #CONDSHOW_ON_on #DEF_0.8 gain factor for feature-specific inhibition from neighbors -- this proportion of the neighboring feature's threshold-inhibition value (used in computing kwta) is spread among neighbors according to inhib_d distance
  bool		wrap;		// #CONDSHOW_ON_on #DEF_true wrap around effective connectivity on sides and top/bottom 

  int		tot_ni_len;	// #READ_ONLY total length of neighborhood inhibition stencils = 2 * neigh_inhib_d + 1

  TA_SIMPLE_BASEFUNS(V1FeatInhibSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

TypeDef_Of(LeabraV1LayerSpec);

class LEABRA_API LeabraV1LayerSpec : public LeabraLayerSpec {
  // LayerSpec that implements competition both within unit groups and among features in nearby unit groups, where a feature is defined as a specific unit position within the unit groups (layer must have unit groups) -- inhibition is oriented according to x axis of feature position, assuming that this encodes orientation angle (0 = horiz), and acts just like the gp_i, gp_g form of inhibition
INHERITED(LeabraLayerSpec)
public:
  enum XY {	   // x, y component of stencils etc -- for clarity in code
    X,
    Y,
  };
  enum LnOrtho {   // line, orthogonal to the line -- for v1s_ang_slopes
    LINE,	   // along the direction of the line
    ORTHO,	   // orthogonal to the line
  };

  V1FeatInhibSpec	feat_inhib; // feature-level inhibition parameters
  float_Matrix		v1s_ang_slopes; // #READ_ONLY #NO_SAVE angle slopes [dx,dy][line,ortho][angles] -- dx, dy slopes for lines and orthogonal lines for each of the angles
  int_Matrix		v1s_ni_stencils; // #READ_ONLY #NO_SAVE stencils for neighborhood inhibition [x,y][tot_ni_len][angles]

  virtual void	UpdateStencils();
  // update the stencils that determine the topology of neighborhood connections based on orientation

  virtual void	Compute_FeatGpActive(LeabraLayer* lay, LeabraUnit_Group* fug, LeabraNetwork* net);
  // compute active units in active_buf for given feature unit group

  override void	Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  override bool CheckConfig_Layer(Layer* lay, bool quiet=false);
  
  TA_SIMPLE_BASEFUNS(LeabraV1LayerSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // LeabraV1LayerSpec_h
