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

#ifndef TiledGpMapConvergePrjnSpec_h
#define TiledGpMapConvergePrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:
#include <taVector2i>

// declare all other types mentioned but not required to include:

TypeDef_Of(TiledGpMapConvergePrjnSpec);

class EMERGENT_API TiledGpMapConvergePrjnSpec : public ProjectionSpec {
  // generates a converging map of the units within a sending layer with unit groups, using tiled overlapping receptive fields within each unit group -- each recv unit receives from the corresponding unit in all of the sending unit groups, with the recv units organized into unit groups that each recv from one tiled subset of sending units within all the sending unit groups -- there must be the same number of recv unit groups as tiled subsets within the sending unit groups
INHERITED(ProjectionSpec)
public:
  taVector2i	send_tile_size;		// number of units in one tile of the sending unit group units
  taVector2i	send_tile_skip;		// number of units to skip when moving the tiling over to the next position (typically 1/2 of the size for nice overlap)
  bool		wrap;			// if true, then connectivity has a wrap-around structure so it starts at -tile_skip (wrapped to right/top) and goes +tile_skip past the right/top edge (wrapped to left/bottom) -- this produces more uniform overlapping coverage of the space
  bool		reciprocal;		// if true, make the appropriate reciprocal connections for a backwards projection from recv to send

  taVector2i 	 trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer gp geometry -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  taVector2i 	 trg_send_geom;	// #READ_ONLY #SHOW target sending layer *unit group* geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  override void Connect_impl(Projection* prjn);
  virtual void 	Connect_Reciprocal(Projection* prjn);
  int 	ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt = 0.0f);
  virtual void	Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
				  int rgpidx, int suidx, int alloc_loop);
  // #IGNORE connect one recv unit group to all sending unit groups -- rgpidx = recv unit group idx, suidx = send unit idx within subgroups

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending unit group geometry (size of one unit group within sending layer) -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(TiledGpMapConvergePrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // TiledGpMapConvergePrjnSpec_h
