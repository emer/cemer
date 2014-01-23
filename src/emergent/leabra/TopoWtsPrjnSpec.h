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
#include <SpecMemberBase>

// member includes:
#include <MinMaxRange>
#include <taVector2i>

// declare all other types mentioned but not required to include:

eTypeDef_Of(IndexByGps);

class E_API IndexByGps: public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra whether to us send, recv gps for indexing, and if so which dimension, or both?
  INHERITED(SpecMemberBase)
    public:

  bool on; 	// when true, uses unit groups for indexing layer
  bool x;		// #CONDSHOW_ON_on use unit groups for indexing in x-dimension, else flat indexing using units only
  bool y; 	// #CONDSHOW_ON_on use gps for indexing in y

  TA_SIMPLE_BASEFUNS(IndexByGps);
 protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();

 private:
  void Initialize() { };
  void Destroy() { };
  void Defaults_init() { Initialize(); };
};

eTypeDef_Of(TopoWtsPrjnSpec);

class E_API TopoWtsPrjnSpec: public FullPrjnSpec {
  // #AKA_FullTopolGradWtsPrjnSpec full connectivity, with user-definable topologically-defined gradient(s) for weight strengths (requires init_wts = true, otherwise is just like Full Prjn), where weights are strongest from sending units (unit groups) in the same topologically relative location as the receiving unit (unit groups), and fall off from there (either linearly or as a Gaussian; other functions can be added) -- if send and/or recv layer(s) have unit groups, then use_send_gps/use_recv_gps must be checked and it is the unit group position that counts -- all params then refer to un_gp position indices and all units within the send and/or recv group have the same connectivity (TODO: can this be overridden with use_recv_gps, use_send_grps flags? i.e., are these flags optional? depends on how the indexing works!!); subsets of units (un_gps) can also be designated for either sending layer, receiving layer, or both
  INHERITED(FullPrjnSpec)
    public:

  enum TopoPattern { 	// pattern of connectivity to establish
    X2X,			// map x-dimension of send layer to x-dim recv
    X2Y,			// x-dim of send to y-dim recv
    Y2Y,			// y send to y recv
    Y2X,			// y send to x recv
    X2X_Y2Y,	// map in both dimensions - x-dim of send to x-dim recv and y-to-y
    X2Y_Y2X,	// x-to-y and y-to-x
  };

  enum GradType {	// type of function to use to establish the gradient
    LINEAR,			// linear fall-off as a function of distance
    GAUSSIAN,		// gaussian fall-off as a function of distance
  };

  enum WrapReflect { 	// type of wt equalization to use at stripe edges
    NONE, 		// usually should be avoided as clipping will produce unequalized netins to recv units
    WRAP,			// compensates for clipping by wrapping around to the other side of layer/stripe
    REFLECT,	// compensates for clipped connections by double-counting mirror image of clipped cons
  };

  TopoPattern topo_pattern; // #CONDSHOW_ON_init_wts topological pattern of connectivity between send and recv layers

  MinMaxRange wt_range;	// #CONDSHOW_ON_init_wts range of weakest (min) to strongest (max) weight values generated

  bool invert;// #CONDSHOW_ON_init_wts invert the gradient, such that the min is located "below" the recv units, and the max is furthest away

  IndexByGps index_by_gps_send; // #CONDSHOW_ON_init_wts
  IndexByGps index_by_gps_recv; // #CONDSHOW_ON_init_wts

  bool custom_send_range;	// #CONDSHOW_ON_init_wts use custom (sub)range of send layer
  taVector2i send_range_start;// #CONDSHOW_ON_custom_send_range start coords used in send layer (either unit groups or units depending on use_send_gps setting)
  taVector2i send_range_end;// #CONDSHOW_ON_custom_send_range end coords used in send layer (either unit groups or units depending on use_send_gps setting) -- use -1 for last unit/group

  bool custom_recv_range;	// #CONDSHOW_ON_init_wts use custom (sub)range of recv layer?
  taVector2i recv_range_start;// #CONDSHOW_ON_custom_recv_range start coords used in recv layer (either unit groups or units depending on use_recv_gps setting)
  taVector2i recv_range_end;// #CONDSHOW_ON_custom_recv_range end coords used in recv layer (either unit groups or units depending on use_recv_gps setting) -- use -1 for last unit/group

  WrapReflect wrap_reflect; // #CONDSHOW_ON_init_wts how to deal with clipped edges

  GradType grad_type;	// #CONDSHOW_ON_init_wts type of gradient to make -- applies to both axes
  float gauss_sig;		// #CONDSHOW_ON_grad_type:GAUSSIAN gaussian sigma (width), in normalized units where entire distance across sending layer is 1.0

  void C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) CPP11_OVERRIDE;
  // calls one of the four InitWeights fns below according to use of unit groups
  void Connect_impl(Projection* prjn) CPP11_OVERRIDE;
  // enables flexibility whether to use unit group indexing (default) or not
  virtual void InitWeights_SendFlatRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru);
  // uses flat idx'ing of both send and recv layers (just unit positions)
  virtual void InitWeights_SendGpsRecvGps(Projection* prjn, RecvCons* cg, Unit* ru);
  // gp idx'ing of both send and recv layers
  virtual void InitWeights_SendGpsRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru);
  // gp idx'ing of send layer, flat idx'ing of recv
  virtual void InitWeights_SendFlatRecvGps(Projection* prjn, RecvCons* cg, Unit* ru);
  // flat idx'ing of send layer, gp idx'ing of recv
  virtual void SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist, int cg_idx, bool dbl_add);
  // actually set the weight value from distance value -- used by above four main routines -- can overload to implement different gradient functions -- cg_idx is index within con group, and dist is computed normalized distance value (0-1)

  virtual float ComputeTopoDist(Projection* prjn, RecvCons* cg, Unit* ru, int i, float ri_x, float ri_y,
				taVector2i srs, taVector2i sre, taVector2i rrs, taVector2i rre, taVector2i ri_pos);
  // computes the normalized Euclidean distance between idx'd send unit and current recv unit -- used by above four main routines

  virtual bool ReflectClippedWt(Projection* prjn, RecvCons* cg, Unit* ru, int i, taVector2i ri_pos,
				taVector2i srs, taVector2i sre, taVector2i rrs, taVector2i rre, float ri_x, float ri_y);
  // returns dbl_add = true if a particular sending wt is clipped by recv lay edge

  bool TestWarning(bool test, const char* fun_name, const char* a,
		   const char* b = 0, const char* c = 0, const char* d = 0,
		   const char* e = 0, const char* f = 0, const char* g = 0,
		   const char* h = 0) const;

  TA_SIMPLE_BASEFUNS(TopoWtsPrjnSpec);
 protected:
  SPEC_DEFAULTS;

  int already_warned;		// don't keep warning beyond first n..

 private:
  void Initialize();
  void Destroy() { };
  void Defaults_init();
};

#endif // TopoWtsPrjnSpec_h
