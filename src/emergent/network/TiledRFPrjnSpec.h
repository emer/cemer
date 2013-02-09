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

#ifndef TiledRFPrjnSpec_h
#define TiledRFPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:
#include <taVector2i>
#include <PosVector2i>
#include <taVector2f>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TiledRFPrjnSpec);

class E_API TiledRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec: connects entire receiving layer unit groups with overlapping tiled regions of sending layers
INHERITED(ProjectionSpec)
public:
  taVector2i	recv_gp_border;		// number of groups around edge of layer to not connect 
  taVector2i	recv_gp_ex_st; 		// start of groups to exclude (e.g., from the middle; -1 = no exclude)
  taVector2i	recv_gp_ex_n; 		// number of groups to exclude
  taVector2i	send_border;		// number of units around edge of sending layer to not connect
  taVector2i	send_adj_rfsz;		// adjust the total number of sending units by this amount in computing rfield size
  taVector2i	send_adj_sndloc;	// adjust the total number of sending units by this amount in computing sending locations
  float		rf_width_mult;		// multiplier factor on the receptive field width: 1.0 = exactly half overlap of RF's across groups.  Larger number = more overlap

  // computed values below
  taVector2i ru_geo;		// #READ_ONLY receiving unit geometry
  taVector2i recv_gp_ed;		// #READ_ONLY recv gp end
  taVector2i recv_gp_ex_ed;	// #READ_ONLY recv gp ex end
  PosVector2i su_act_geom;	// #READ_ONLY sending actual geometry
  taVector2i n_recv_gps;		// #READ_ONLY number of recv gps
  taVector2i n_send_units;	// #READ_ONLY number of sending units total 
  taVector2i rf_ovlp; 		// #READ_ONLY ovlp = send / (ng + 1)
  taVector2f rf_move;	// #READ_ONLY how much to move sending rf per recv group
  taVector2i rf_width;		// #READ_ONLY width of the sending rf 

  virtual bool	InitRFSizes(Projection* prjn); // initialize sending receptive field sizes

  override void Connect_impl(Projection* prjn);
  int 	ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt = 0.0f);

  virtual void	SelectRF(Projection* prjn);
  // #BUTTON select all sending and receiving units in the receptive field of this projection

  TA_SIMPLE_BASEFUNS(TiledRFPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // TiledRFPrjnSpec_h
