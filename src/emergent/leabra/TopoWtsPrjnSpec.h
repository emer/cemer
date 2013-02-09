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

#ifndef TopoWtsPrjnSpec_h
#define TopoWtsPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <FullPrjnSpec>

// member includes:
#include <MinMaxRange>
#include <taVector2i>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TopoWtsPrjnSpec);

class E_API TopoWtsPrjnSpec : public FullPrjnSpec {
  // #AKA_FullTopolGradWtsPrjnSpec full connectivity, with user-definable topologically-defined gradient of weight strengths (requires init_wts = true, otherwise is just like Full Prjn), where weights are strongest from sending units (unit groups) in the same topologically relative location as the receiving unit (unit groups), and fall off from there (either linearly or as a Gaussian; other functions can be added) -- if send and/or recv layer(s) have unit groups, then use_send_gps/use_recv_gps must be checked and it is the unit group position that counts -- all params then refer to un_gp position indices and all units within the send and/or recv group have the same connectivity (TODO: can this be overridden with use_recv_gps, use_send_grps flags? i.e., are these flags optional? depends on how the indexing works!!); subsets of units (un_gps) can also be designated for either sending layer, receiving layer, or both
INHERITED(FullPrjnSpec)
public:
  enum GradType {	// type of gradient to establish
    LINEAR,		// linear fall-off as a function of distance
    GAUSSIAN,		// gaussian fall-off as a function of distance
  };

  MinMaxRange	wt_range;	// #CONDSHOW_ON_init_wts range of weakest (min) to strongest (max) weight values generated

  bool		invert;			// #CONDSHOW_ON_init_wts invert the gradient, such that the min is located "below" the recv units, and the max is furthest away
  bool		grad_x;			// #CONDSHOW_ON_init_wts compute a gradient over the x dimension of the sending layer, based on x axis location of the send layer to x dimension of recv layer, unless grad_x_grad_y also true
  bool		grad_x_grad_y;		// #CONDSHOW_ON_grad_x map gradient from x dimension of send layer to y dimension of recv layer

  bool		grad_y;			// #CONDSHOW_ON_init_wts compute a gradient over the y dimension of the sending layer, based on y axis location of the send layer to y dimension of recv layer, unless grad_y_grad_x also
  bool		grad_y_grad_x;		// #CONDSHOW_ON_grad_y map gradient from y dimension of send layer to x dimension of recv layer

  bool		wrap;			// #CONDSHOW_ON_init_wts wrap weight values around relevant dimension(s) -- the closest location wins -- this ensures that all units have the same overall weight strengths
  bool		use_recv_gps;		// #CONDSHOW_ON_init_wts if recv layer has unit groups, use them for determining relative position to compare with sending unit locations (unit group information is not used for the sending layer unless use_send_gps also true)
  bool		use_send_gps;		// #CONDSHOW_ON_init_wts if send layer has unit groups, use them for determining relative position to compare with receiving unit locations (unit group information is not used for the receiving layer unless use_recv_gps also true)

  bool		custom_send_range;	// #CONDSHOW_ON_init_wts use custom (sub)range of send layer

  taVector2i 	send_range_start;	// #CONDSHOW_ON_custom_send_range start coords used in send layer (either unit groups or units depending on use_send_gps setting)
  taVector2i 	send_range_end;		// #CONDSHOW_ON_custom_send_range end coords used in send layer (either unit groups or units depending on use_send_gps setting) -- use -1 for last unit/group
  
  bool		custom_recv_range;	// #CONDSHOW_ON_init_wts use custom (sub)range of recv layer?

  taVector2i 	recv_range_start;	// #CONDSHOW_ON_custom_recv_range start coords used in recv layer (either unit groups or units depending on use_recv_gps setting)
  taVector2i 	recv_range_end;		// #CONDSHOW_ON_custom_recv_range end coords used in recv layer (either unit groups or units depending on use_recv_gps setting) -- use -1 for last unit/group
  
  GradType	grad_type;		// #CONDSHOW_ON_init_wts type of gradient to make -- applies to both axes
  float		gauss_sig;		// #CONDSHOW_ON_grad_type:GAUSSIAN gaussian sigma (width), in normalized units where entire distance across sending layer is 1.0

  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // calls one of the four InitWeights fns below according to use of unit groups
  override void Connect_impl(Projection* prjn);
  // enables flexibility whether to use unit group indexing (default) or not
  virtual void 	InitWeights_SendFlatRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru);
  // for flat recv layer case (just unit positions)
  virtual void	InitWeights_SendGpsRecvGps(Projection* prjn, RecvCons* cg, Unit* ru);
  // for both send and recv unit group case
  virtual void	InitWeights_SendGpsRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru);
  // for send unit group, flat recv case
  virtual void	InitWeights_SendFlatRecvGps(Projection* prjn, RecvCons* cg, Unit* ru);
  // for send flat, recv unit group case
  virtual void 	SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist, int cg_idx);
  // actually set the weight value from distance value -- util used by both of above main routines -- can overload to implement different gradient functions -- cg_idx is index within con group, and dist is computed normalized distance value (0-1)

  bool   TestWarning(bool test, const char* fun_name,
		     const char* a, const char* b=0, const char* c=0,
		     const char* d=0, const char* e=0, const char* f=0,
		     const char* g=0, const char* h=0) const;


  TA_SIMPLE_BASEFUNS(TopoWtsPrjnSpec);
protected:
  SPEC_DEFAULTS;

  int	already_warned;		// don't keep warning beyond first n..

private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};

#endif // TopoWtsPrjnSpec_h
