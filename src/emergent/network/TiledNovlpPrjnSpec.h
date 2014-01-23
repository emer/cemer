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

#ifndef TiledNovlpPrjnSpec_h
#define TiledNovlpPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:
#include <taVector2i>
#include <PosVector2i>
#include <taVector2f>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TiledNovlpPrjnSpec);

class E_API TiledNovlpPrjnSpec : public ProjectionSpec {
  // Tiled non-overlapping projection spec: connects entire receiving layer unit groups with non-overlapping tiled regions of sending units
INHERITED(ProjectionSpec)
public:
  bool		reciprocal;	// if true, make the appropriate reciprocal connections for a backwards projection from recv to send

  taVector2i ru_geo;		// #READ_ONLY receiving unit geometry
  PosVector2i su_act_geom;	// #READ_ONLY sending actual geometry
  taVector2f rf_width;	// #READ_ONLY how much to move sending rf per recv group

  virtual bool	InitRFSizes(Projection* prjn); // initialize sending receptive field sizes

  void Connect_impl(Projection* prjn) override;
  virtual void 	Connect_Reciprocal(Projection* prjn);
//   int 	ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt = 0.0f);

//   virtual void	SelectRF(Projection* prjn);
  // #BUTTON select all sending and receiving units in the receptive field of this projection

  TA_SIMPLE_BASEFUNS(TiledNovlpPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // TiledNovlpPrjnSpec_h
