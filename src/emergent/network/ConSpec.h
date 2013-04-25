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

#ifndef ConSpec_h
#define ConSpec_h 1

// parent includes:
#include <BaseSpec>
#include <SpecPtr>

// member includes:
#include <Connection>
#include <RandomSpec>

// declare all other types mentioned but not required to include:
class BaseCons; //
class RecvCons; //
class SendCons; //
class Unit; //
class Network; //
class Projection; //

// the ConSpec has 2 versions of every function: one to go through the group
// and the other to apply to a single connection.
// The C_ denotes the con one which is non-virtual so that it is inlined
// this requires the group level one to be re-defined whenever the con
// level one is.  This is necessary given the speed penalty for a virtual
// function call at the connection level.

// The exception to the non-virtual rule is the C_Init functions!

// The following macro makes this process easier:

#define CON_GROUP_LOOP(cg, expr) \
  for(int i=0; i<cg->size; i++) \
    expr

eTypeDef_Of(WeightLimits);

class E_API WeightLimits : public taOBase {
  // ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##CAT_Network specifies weight limits for connections
INHERITED(taOBase)
public:
  enum LimitType {
    NONE,                       // no weight limitations
    GT_MIN,                     // constrain weights to be greater than min value
    LT_MAX,                     // constrain weights to be less than max value
    MIN_MAX                     // constrain weights to be within min and max values
  };
  LimitType     type;           // type of weight limitation to impose
  float         min;            // #CONDSHOW_OFF_type:NONE,LT_MAX minimum weight value (if applicable)
  float         max;            // #CONDSHOW_OFF_type:NONE,GT_MIN maximum weight value (if applicable)
  bool          sym;            // if true, also symmetrize with reciprocal connections

  void  ApplyMinLimit(float& wt)        { if(wt < min) wt = min; }
  void  ApplyMaxLimit(float& wt)        { if(wt > max) wt = max; }

  void  ApplyLimits(float& wt)
  { if(type == GT_MIN)          ApplyMinLimit(wt);
    else if(type == LT_MAX)     ApplyMaxLimit(wt);
    else if(type == MIN_MAX)    { ApplyMinLimit(wt); ApplyMaxLimit(wt); } }

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(WeightLimits);
  TA_BASEFUNS(WeightLimits);
private:
  void  Initialize()            { type = NONE; min = -1.0f; max = 1.0f; sym = false; }
  void  Destroy()               { };
};

eTypeDef_Of(ConSpec);

class E_API ConSpec: public BaseSpec {
  // ##CAT_Spec Connection specs: for processing over a set of connections all from the same projection
INHERITED(BaseSpec)
public:
  RandomSpec    rnd;            // #CAT_ConSpec Weight randomization specification.  Note that NONE means no value at all, not the mean, and should be used if some other source is setting the weights, e.g., from a projectionspec or loading from a file etc
  WeightLimits  wt_limits;      // #CAT_ConSpec limits on weight sign, symmetry

  inline void           C_ApplyLimits(Connection* cn, Unit*, Unit*)
  { wt_limits.ApplyLimits(cn->wt); }
  // #CAT_Learning apply weight limits to single connection
  inline virtual void   ApplyLimits(RecvCons* cg, Unit* ru);
  // #CAT_Learning apply weight limits (sign, magnitude)

  virtual void          ApplySymmetry(RecvCons* cg, Unit* ru);
  // #CAT_Learning apply weight symmetrizing between reciprocal units

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning

  inline virtual void   C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su);
  // #CAT_Learning initialize weight state variables (ie. at beginning of training)
  inline virtual void   C_AddRndWeights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su, float scale);
  // #CAT_Learning add random noise to existing weight variables -- for add_rnd_wts after prjn spec init_wts based initialization
  inline virtual void   Init_Weights(RecvCons* cg, Unit* ru);
  // #CAT_Learning initialize weight state variables (ie. at beginning of training)
  inline virtual void   C_Init_Weights_post(BaseCons* cg, Connection* cn, Unit* ru, Unit* su)
  { };
  inline virtual void   Init_Weights_post(BaseCons* cg, Unit* ru);
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)
  inline virtual void   C_Init_dWt(RecvCons*, Connection* cn, Unit*, Unit*)
  { cn->dwt=0.0f; }
  // #CAT_Learning initialize weight-change variables on a single connection
  inline virtual void   Init_dWt(RecvCons* cg, Unit* ru);
  // #CAT_Learning initialize weight-change variables for whole set

  inline float          C_Compute_Netin(Connection* cn, Unit* ru, Unit* su);
  // #IGNORE 
  inline virtual float  Compute_Netin(RecvCons* cg, Unit* ru);
  // #CAT_Activation compute net input for weights in this con group

  inline void           C_Send_Netin(Connection* cn, float* send_netin_vec, Unit* ru,
                                     float su_act);
  // #IGNORE 
  inline virtual void   Send_Netin(SendCons* cg, Network* net, int thread_no, Unit* su);
  // #CAT_Activation sender-based net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units
  inline virtual void   Send_Netin_PerPrjn(SendCons* cg, Network* net, int thread_no, Unit* su);
  // #CAT_Activation sender-based net input, keeping projections separate, for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units

  inline float          C_Compute_Dist(Connection* cn, Unit* ru, Unit* su);
  // #IGNORE 
  inline virtual float  Compute_Dist(RecvCons* cg, Unit* ru);
  // #CAT_Activation compute net distance for con group (ie. euclidean distance)
  inline void           C_Compute_dWt(Connection*, Unit*, Unit*)        { };
  inline virtual void   Compute_dWt(RecvCons* cg, Unit* ru);
  // #CAT_Learning compute the delta-weight change
  inline void           C_Compute_Weights(Connection*, Unit*, Unit*)    { };
  inline virtual void   Compute_Weights(RecvCons* cg, Unit* ru);
  // #CAT_Learning update weights (ie. add delta-wt to wt, zero delta-wt)

  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  virtual void          GetPrjnName(Projection& prjn, String& nm) { };
  // add anything special for this type of connection to the projection name -- default name (FM_from) is provided as a nm value, which this function can modify in any way

  virtual bool          CheckConfig_RecvCons(RecvCons* cg, bool quiet=false);
  // check for for misc configuration settings required by different algorithms

  virtual  void  Init_Weights_Net();
  // #BUTTON #CAT_Learning initializes all weights in the network

  virtual bool   DMem_AlwaysLocal() { return false; }
  // #CAT_DMem overload this function to prevent this projection from being pruned for non-local units under dmem processing (for "special" connection types)

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const ConSpec& cp);
  TA_BASEFUNS(ConSpec);
protected:
  SPEC_DEFAULTS;
  override bool         CheckType_impl(TypeDef* td);
  override bool         CheckObjectType_impl(taBase* obj); // don't do checking on 1st con group in units
private:
  void  Initialize();
  void  Destroy()               { CutLinks(); }
  void  Defaults_init()         { };
};

SpecPtr_of(ConSpec);

#endif // ConSpec_h
