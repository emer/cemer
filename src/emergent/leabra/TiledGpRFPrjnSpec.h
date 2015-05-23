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
#include <int_Array>

// declare all other types mentioned but not required to include:

eTypeDef_Of(GaussInitWtsSpec);

class E_API GaussInitWtsSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Projection parameters for initializing gaussian projection weights
INHERITED(taOBase)
public:
  bool          on;             // initialize gaussian weights at this level scale
  float		sigma;		// #CONDSHOW_ON_on gaussian sigma (width) in normalized units where entire distance across relevant dimension is 1.0 -- typical useful values range from .3 to 1.5
  bool          wrap_wts;       // #CONDSHOW_ON_on wrap the gaussian around on other sides of the receptive field -- this removes strict topography but ensures a more uniform distribution of weight values so edge units don't have weaker overall weights -- often useful to use for unit-group level but not for full level
  float         ctr_mv;         // #CONDSHOW_ON_on #DEF_0.8;1 how much the center of the gaussian moves with respect to the position of the receiving unit within its unit group -- 1.0 = centers span the entire range of the receptive field -- typically want to use 1.0 for wrap_wts = true, and 0.8 for wrap_wts = false

  TA_SIMPLE_BASEFUNS(GaussInitWtsSpec);
private:
  void 	Initialize();
  void	Destroy() { };
};

eTypeDef_Of(TiledGpRFPrjnSpec);

class E_API TiledGpRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec for entirely group-to-group connections: connects entire receiving layer unit groups with overlapping tiled regions of sending layer groups -- if init_wts is on, gaussian topographic weights are initialized
INHERITED(ProjectionSpec)
public:
  taVector2i	send_gp_size;		// number of groups in the sending receptive field
  taVector2i	send_gp_skip;		// number of groups to skip per each recv group (typically 1/2 of the size for nice overlap)
  taVector2i	send_gp_start;		// starting offset for sending groups -- for wrapping this was previously automatically set to -send_gp_skip (else 0), but you can now set this to anything you want
  bool		wrap;			// if true, then connectivity has a wrap-around structure so it starts at -gp_skip (wrapped to right/top) and goes +gp_skip past the right/top edge (wrapped to left/bottom)
  bool		reciprocal;		// if true, make the appropriate reciprocal connections for a backwards projection from recv to send
  bool          share_cons;             // units in unit groups after the first unit group share connection values (weights etc) with those in the first unit group -- generally only works if wrap is true so that all units have same scope and order of connectivity
  GaussInitWtsSpec full_gauss;          // #CONDSHOW_ON_init_wts parameters for gaussian initial weight distribution for the full width of the projection across all sending unit groups
  GaussInitWtsSpec gp_gauss;            // #CONDSHOW_ON_init_wts parameters for gaussian initial weight distribution for each individual sending unit group
  bool          full_gpwise;            // #CONDSHOW_ON_init_wts&&full_gauss.on make the full-field gaussian weights apply uniformly across entire unit groups -- does not use relative position of the receiving unit -- otherwise uses full unit-level coordinates relative to position of receiving unit to compute a moving topological gaussian
  MinMaxRange	wt_range;               // #CONDSHOW_ON_init_wts range of weakest (min) to strongest (max) weight values generated -- typically want to center this around .5, and often fairly subtle differences (.4 - .6) produce reasonably strong effects on Leabra networks

  taVector2i 	trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer gp geometry -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  taVector2i 	trg_send_geom;	// #READ_ONLY #SHOW target sending layer geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  
  void	Init_Weights_Prjn(Projection* prjn, ConGroup* cg, Network* net,
                          int thr_no) override;

  virtual void	Init_Weights_Gaussian(Projection* prjn, ConGroup* cg, Network* net,
                                      int thr_no);
  // gaussian initial weights

  void  Connect_impl(Projection* prjn, bool make_cons) override;

  virtual void	Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
				  int rgpidx, int sgpidx, bool make_cons);
  // #IGNORE connect one unit group to another -- rgpidx = recv unit group idx, sgpidx = send unit group idx

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(TiledGpRFPrjnSpec);
protected:
  void  UpdateAfterEdit_impl() override;
  
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // TiledGpRFPrjnSpec_h
