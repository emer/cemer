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

#ifndef TiledGpRFPrjnSpec_h
#define TiledGpRFPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:
#include <taVector2i>
#include <MinMaxRange>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TiledGpRFPrjnSpec);

class E_API TiledGpRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec for entirely group-to-group connections: connects entire receiving layer unit groups with overlapping tiled regions of sending layer groups -- if init_wts is on, gaussian topographic weights are initialized
INHERITED(ProjectionSpec)
public:
  taVector2i	send_gp_size;		// number of groups in the sending receptive field
  taVector2i	send_gp_skip;		// number of groups to skip per each recv group (typically 1/2 of the size for nice overlap)
  bool		wrap;			// if true, then connectivity has a wrap-around structure so it starts at -gp_skip (wrapped to right/top) and goes +gp_skip past the right/top edge (wrapped to left/bottom)
  bool		reciprocal;		// if true, make the appropriate reciprocal connections for a backwards projection from recv to send
  float		gauss_sig;		// gaussian sigma (width), in normalized units where entire distance across sending layer is 1.0
  MinMaxRange	wt_range;	        // #CONDSHOW_ON_init_wts range of weakest (min) to strongest (max) weight values generated

  taVector2i 	 trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer gp geometry -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  taVector2i 	 trg_send_geom;	// #READ_ONLY #SHOW target sending layer geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) override;
  void Connect_impl(Projection* prjn) override;
  virtual void 	Connect_Reciprocal(Projection* prjn);
  int 	ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt = 0.0f);
  virtual void	Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
				  int rgpidx, int sgpidx, int alloc_loop);
  // #IGNORE connect one unit group to another -- rgpidx = recv unit group idx, sgpidx = send unit group idx
  virtual void 	SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist,
                            int cg_idx);
  // actually set the weight value from distance value -- util used by both of above main routines -- can overload to implement different gradient functions -- cg_idx is index within con group, and dist is computed normalized distance value (0-1)

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(TiledGpRFPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // TiledGpRFPrjnSpec_h
