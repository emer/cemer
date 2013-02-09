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

#ifndef ProjectionSpec_h
#define ProjectionSpec_h 1

// parent includes:
#include <BaseSpec>
#include <SpecPtr>

// smartptr, ref:
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:

// declare all other types mentioned but not required to include:
class Projection; //
class RecvCons; //
class Unit; //
class Layer; //

eTypeDef_Of(ProjectionSpec);

// Projections are abrevieated prjn (as a oppesed to proj = project or proc = process)
// ProjectionSpec does the connectivity, and optionally the weight init

class E_API ProjectionSpec : public BaseSpec {
  // #STEM_BASE #VIRT_BASE ##CAT_Projection Specifies the connectivity between layers (ie. full vs. partial)
INHERITED(BaseSpec)
public:
  bool          self_con;          // #CAT_Structure whether to create self-connections or not (if applicable)
  bool          init_wts;     	   // #CAT_Structure whether this projection spec does weight init (else conspec)
  bool          add_rnd_wts;  	   // #CONDSHOW_ON_init_wts if init_wts is set, use the random weight settings on the conspect to add random values to the weights set by the projection spec -- NOTE: this typically will work best by setting the rnd.mean value to 0
  float		add_rnd_wts_scale; // #CONDSHOW_ON_init_wts scales added random weight values by the projection spec -- don't need a custom spec!

  virtual void  Connect(Projection* prjn);
  // #CAT_Structure connects the network, doing PreConnect, Connect_impl, then Init_Weights -- generally do not override this function
    virtual void        PreConnect(Projection* prjn);
    // #CAT_Structure Prepare to connect (init con_groups)
    virtual void        Connect_impl(Projection*) { };
    // #CAT_Structure actually implements specific connection code

  virtual int   ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);
  // #CAT_Structure probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
    virtual int ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt = 0.0);
    // #CAT_Structure actual implementation: probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection


  virtual void  Init_dWt(Projection* prjn);
  // #CAT_Weights initializes the weight change variables
  virtual void  Init_Weights(Projection* prjn);
  // #CAT_Weights initializes the weiht values
  virtual void  Init_Weights_post(Projection* prjn);
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  virtual bool  CheckConnect(Projection* prjn, bool quiet=false);
  // #CAT_ObjectMgmt check if projection is connected

  virtual void  C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // #CAT_Weights custom initialize weights in this con group for given receiving unit ru -- any derived version MUST call the base inherited version so that other init weights variables are also initialized

  override String       GetTypeDecoKey() const { return "ProjectionSpec"; }

  void  InitLinks();
  SIMPLE_COPY(ProjectionSpec);
  TA_BASEFUNS(ProjectionSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()               { CutLinks(); }
  void  Defaults_init()         { };
};

TA_SMART_PTRS(ProjectionSpec)

SpecPtr_of(ProjectionSpec);

#endif // ProjectionSpec_h
