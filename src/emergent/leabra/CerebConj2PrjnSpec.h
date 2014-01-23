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

#ifndef CerebConj2PrjnSpec_h
#define CerebConj2PrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:
#include <taVector2i>
#include <taVector2f>

// declare all other types mentioned but not required to include:

eTypeDef_Of(CerebConj2PrjnSpec);

class E_API CerebConj2PrjnSpec : public ProjectionSpec {
  // cerebellar-inspired conjunctive projection spec, 2nd order conjunctions between two topographic input maps -- first one in layer prjn is outer group (across unit groups), 2nd one is inner group (within unit groups)
INHERITED(ProjectionSpec)
public:
  taVector2i 	 rf_width;	// size of the receptive field -- should be an even number
  taVector2f     rf_move;	// how much to move in input coordinates per each receiving increment (unit group or unit within group, depending on whether inner or outer) -- typically 1/2 rf_width
  float		gauss_sigma;	// #CONDEDIT_ON_init_wts gaussian width parameter for initial weight values to give a tuning curve
  bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -rf_move (wrapped to right/top) and goes +rf_move past the right/top edge (wrapped to left/bottom)

  taVector2i 	 trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer geometry (either gp or unit, depending on outer vs. inner) -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  taVector2i 	 trg_send_geom;	// #READ_ONLY #SHOW target sending layer geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  virtual  void Connect_Inner(Projection* prjn);
  // inner connect: unit position within the unit group determines sender location
  virtual  void Connect_Outer(Projection* prjn);
  // outer connect: unit_group position determines sender location

  void Connect_impl(Projection* prjn) override;
  void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) override;

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(CerebConj2PrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // CerebConj2PrjnSpec_h
