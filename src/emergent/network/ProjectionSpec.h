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
class ConGroup; //
class Unit; //
class Layer; //
class Network; //

eTypeDef_Of(RenormInitWtsSpec);

class E_API RenormInitWtsSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Projection parameters for renormalizing initial weight values
INHERITED(taOBase)
public:
  bool          on;             // renormalize initial weight values
  bool          mult_norm;      // #CONDSHOW_ON_on use multiplicative normalization to rescale the weight values to hit the target value, which is appropriate for all-positive weight values (e.g., Leabra) -- otherwise use addition to add/subtract a constant from all weights to hit the target value, which is approparite for pos/neg signed weight values (e.g., backprop)
  float		avg_wt;	        // #CONDSHOW_ON_on target average weight value per connection to renormalize to -- e.g., the sum across all weights will be adjusted so that it equals n_cons * avg_wt

  TA_SIMPLE_BASEFUNS(RenormInitWtsSpec);
private:
  void 	Initialize();
  void	Destroy() { };
};

eTypeDef_Of(ProjectionSpec);

// Projections are abrevieated prjn (as a oppesed to proj = project or proc = process)
// ProjectionSpec does the connectivity, and optionally the weight init

class E_API ProjectionSpec : public BaseSpec {
  // #STEM_BASE #VIRT_BASE ##CAT_Projection Specifies the connectivity between layers (ie. full vs. partial)
INHERITED(BaseSpec)
public:
  bool          self_con;          // #CAT_Structure whether to create self-connections or not (if applicable)
  bool          init_wts;     	   // #CAT_Structure whether this projection spec does weight init (else conspec)
  bool          set_scale;         // #CAT_Structure #CONDSHOW_ON_init_wts only for Leabra algorithm: if initializing the weights, set the connection scaling parameter in addition to intializing the weights -- this will for example set a gaussian scaling parameter on top of random initial weights, instead of just setting the initial weights to a gaussian weighted value
  bool          add_rnd_var;  	   // #AKA_add_rnd_wts #CONDSHOW_ON_init_wts if init_wts is set, use the random weight settings on the conspec to add random values to the weights set by the projection spec -- the mean of the random distribution is subtracted, so we're just adding variance, not any mean value
  RenormInitWtsSpec renorm_wts;    // #CAT_Structure renormalize initial weight values -- this can be done even if this projection does not have init_wts set -- if set_scale is set, then the scales are renormalized instead of the weights

  virtual void  Connect_Sizes(Projection* prjn);
  // #CAT_Structure first-pass connects the network, doing Connect_impl(false), ending up with target allocation sizes
    virtual void        Connect_impl(Projection*, bool make_cons) { };
    // #CAT_Structure actually implements specific connection code -- called in two passes -- first with make_cons = false does allocation, and second with make_cons = true
  virtual void  Connect_Cons(Projection* prjn);
  // #CAT_Structure second pass connection -- actually makes the connections via Connect_impl(true), and then calls Init_Weights

  virtual int   ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);
  // #CAT_Structure probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
    virtual int ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt = 0.0);
    // #CAT_Structure actual implementation: probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection

  virtual void  SetCnWt(float wt_val, ConGroup* cg, int cn_idx, Network* net, int thr_no);
  // #CAT_Weights set given connection number in con group to given weight value -- this implements the add_rnd_var flag to add random variance to weights if set
  virtual void  SetCnWtRnd(ConGroup* cg, int cn_idx, Network* net, int thr_no);
  // #CAT_Weights set given connection number in con group to standard random weight value as specified in the connection spec
  virtual void  SetCnScale(float scale_val, ConGroup* cg, int cn_idx, Network* net, int thr_no);
  // #CAT_Weights set given connection number in con group to given scale value

  virtual void  Init_Weights_Prjn(Projection* prjn, ConGroup* cg, Network* net,
                                  int thr_no);
  // #CAT_Weights #IGNORE when init_wts flag is set, the projection spec sets weights for the entire set of connections, from a recv perspective (always use safe access for Cn that does not depend on who owns it) -- overload in subclasses that set weights

  virtual void  Init_Weights_renorm(Projection* prjn, ConGroup* cg, Network* net, int thr_no);
  // #CAT_Weights #IGNORE renormalize weights -- done as a second pass after Init_Weights and before Init_Weights_post

  virtual bool  CheckConnect(Projection* prjn, bool quiet=false);
  // #CAT_ObjectMgmt check if projection is connected

  String        GetTypeDecoKey() const override { return "ProjectionSpec"; }
  String        GetToolbarName() const override { return "prjn spec"; }

  void  InitLinks() override;
  SIMPLE_COPY(ProjectionSpec);
  TA_BASEFUNS(ProjectionSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
  
private:
  void  Initialize();
  void  Destroy()               { CutLinks(); }
  void  Defaults_init()         { };
};

SPECPTR_OF(ProjectionSpec);

#endif // ProjectionSpec_h
