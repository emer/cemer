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

#ifndef SaliencyPrjnSpec_h
#define SaliencyPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:
#include <DoGFilter>

// declare all other types mentioned but not required to include:
class DataTable; // 

eTypeDef_Of(SaliencyPrjnSpec);

class E_API SaliencyPrjnSpec : public ProjectionSpec {
  // Saliency projection spec from V1 layer: receiving layer must have a unit group for each feature, with each unit group having the V1 unit group geometry -- gets excitatory connection from feature corresponding to group index, and from all-but that feature in surrounding sending areas, producing a contrast enhancement effect.  Competition within group and across whole layer produces pop-out dynamics
INHERITED(ProjectionSpec)
public:
  int		convergence;	// how many sending groups to count as a single entity from the perspective of the saliency computation -- values larger than 1 make the saliency layer smaller than the sending layer
  bool		reciprocal;	// opposite direction connection: feedback to the features 
  bool		feat_only;	// only get excitatory input from feature only -- no DoG or anything else -- just pure feature aggregation (for use with kwta-based popout dynamics)
  int		feat_gps;	// number of feature groups contained within V1 unit group -- surround connections are only for within-group connections
  DoGFilter	dog_wts;	// #CONDEDIT_OFF_feat_only Difference of Gaussians filter for setting the weights -- determines the width of the projection, etc -- where the net filter values are positive, receives excitation from the target feature, else from all-but the target feature
  float		wt_mult;	// #CONDEDIT_OFF_feat_only multiplier on weight values coming from the dog_wts 
  float		surr_mult;	// #CONDEDIT_OFF_feat_only multiplier on surround weight values -- DoG has very weak surround in general so it can be useful to increase that

  int		units_per_feat_gp; // #READ_ONLY #NO_SAVE #SHOW number of units per feature group (computed from sending layer)
  
  void 		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual void 	Connect_feat_only(Projection* prjn);
  virtual void 	Connect_full_dog(Projection* prjn);

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK plot the DoG center-surround weights into data table and generate a graph
  virtual void	GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK plot the DoG center-surround weights into data table and generate a grid view

  TA_SIMPLE_BASEFUNS(SaliencyPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // SaliencyPrjnSpec_h
