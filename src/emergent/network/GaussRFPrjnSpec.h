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

#ifndef GaussRFPrjnSpec_h
#define GaussRFPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(GaussRFPrjnSpec);

class EMERGENT_API GaussRFPrjnSpec : public ProjectionSpec {
  // a simple receptive-field (RF) projection spec with gaussian weight values over a receptive-field window onto the sending layer that moves as a function of the receiving unit's position (like TesselPrjnSpec and other RF prjn specs, but does NOT use unit groups) -- useful for reducing larger layers to smaller ones for example
INHERITED(ProjectionSpec)
public:
  taVector2i 	 rf_width;	// size of the receptive field -- should be an even number
  taVector2f     rf_move;	// how much to move in input coordinates per each receiving increment (unit group or unit within group, depending on whether inner or outer) -- typically 1/2 rf_width
  float		gauss_sigma;	// #CONDEDIT_ON_init_wts gaussian width parameter for initial weight values to give a tuning curve
  bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -rf_move (wrapped to right/top) and goes +rf_move past the right/top edge (wrapped to left/bottom)

  taVector2i 	 trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer geometry (either gp or unit, depending on outer vs. inner) -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  taVector2i 	 trg_send_geom;	// #READ_ONLY #SHOW target sending layer geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  override void Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(GaussRFPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // GaussRFPrjnSpec_h
