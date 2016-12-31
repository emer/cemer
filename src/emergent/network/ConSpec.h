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
class ConGroup; //
class Unit; //
class UnitVars; //
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
  const int sz = cg->size; \
  for(int i=0; i<sz; i++) \
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
  bool          sym;            // if true, also symmetrize weights with those in reciprocal connections, during weight initialization process -- this is automatically turned off if the random variance (rnd.var) in the weights is set to 0 (e.g., for fixed weight patterns)
  bool          sym_fm_top;     // #CONDSHOW_ON_sym if symmetrizing, and this is true, then top-down weights end up driving the symmetrized weights -- otherwise the bottom-up end of driving (default)

  inline void  ApplyMinLimit(float& wt)        { if(wt < min) wt = min; }
  inline void  ApplyMaxLimit(float& wt)        { if(wt > max) wt = max; }

  inline void  ApplyLimits(float& wt)
  { if(type == GT_MIN)          ApplyMinLimit(wt);
    else if(type == LT_MAX)     ApplyMaxLimit(wt);
    else if(type == MIN_MAX)    { ApplyMinLimit(wt); ApplyMaxLimit(wt); } }

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(WeightLimits);
private:
  void  Initialize();
  void  Destroy()               { };
};


eTypeDef_Of(ConSpec);

class E_API ConSpec: public BaseSpec {
  // ##CAT_Spec Connection specs: for processing over a set of connections all from the same projection -- all ConGroup functions should be called on one that owns the connections
INHERITED(BaseSpec)
public:
  enum ConVars {                // Connection variables -- must align with Connection obj
    WT,                         // the synaptic weight of connection
    DWT,                        // change in synaptic weight as computed by learning mechanism
    N_CON_VARS,                 // #IGNORE number of basic connection variables -- use this as the starting index for any new connection variables
  };

  RandomSpec    rnd;            // #CAT_ConSpec Weight randomization specification.  Note that NONE means no value at all, not the mean, and should be used if some other source is setting the weights, e.g., from a projectionspec or loading from a file etc
  WeightLimits  wt_limits;      // #CAT_ConSpec limits on weight sign, symmetry

  inline float&         C_ApplyLimits(float& wt)
  { wt_limits.ApplyLimits(wt); return wt; }
  // #IGNORE #CAT_Learning apply weight limits to single connection
  inline virtual void   ApplyLimits(ConGroup* cg, Network* net, int thr_no);
  // #IGNORE #CAT_Learning apply weight limits (sign, magnitude) -- automatically enforced during Init_Weights -- this is if needed outside of that

  virtual void          ApplySymmetry_r(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Learning apply weight symmetrizing between reciprocal units -- recv owns cons version
  virtual void          ApplySymmetry_s(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Learning apply weight symmetrizing between reciprocal units -- send owns cons version

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning

  virtual void  LoadWeightVal(float wtval, ConGroup* cg, int cidx, Network* net);
  // #CAT_Structure set loaded weight value for given connection index
  
  inline void     C_Init_Weight_Rnd(float& wt, int thr_no) {
    wt = rnd.Gen(thr_no);
    C_ApplyLimits(wt);
  }
  // #CAT_Learning initialize the weight according to the random number specs, and apply weight limits (not symmetry)
  inline void   C_Init_Weight_AddRndVar(float& wt, int thr_no) {
    wt += rnd.Gen(thr_no) - rnd.mean; // subtract mean
  }
  // #CAT_Learning initialize the weight by adding number according to the random number specs, and apply weight limits (not symmetry)
  inline void   C_Init_dWt(float& dwt)
  { dwt = 0.0f; }
  // #CAT_Learning initialize weight-change variable to 0

  virtual void          SetConScale(float scale, ConGroup* cg, int cidx, Network* net, int thr_no) { }
  // #CAT_Learning set the connection scale parameter, for given connection index, for algorithms that support connection scale parameters (Leabra)
  
  inline void           Init_Weights_symflag(Network* net, int thr_no);
  // #IGNORE must be called during Init_Weights to update net flag for weight symmetrizing

  inline virtual void   Init_Weights(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Learning initialize connection weights based on random parameter settings (ie. at beginning of training)
  
  // NOTE: it is ESSENTIAL that Init_Weights ONLY does wt, dwt, and scale -- all other vars
  // MUST be initialized in _post -- projections with topo weights ONLY do these specific
  // variables but no others..

  inline virtual void   Init_Weights_scale(ConGroup* cg, Network* net, int thr_no,
                                           float init_wt_val = 1.0f) { };
  // #CAT_Learning only for Leabra: initialize connection weights by setting scale multiplier values to random values, and setting adaptive weights to given constant initial value (weights end up as product of scale * weight)

  inline virtual void   Init_Weights_sym_r(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Structure apply symmetry after weight init, recv based
  inline virtual void   Init_Weights_sym_s(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Structure apply symmetry after weight init, sender based
  inline virtual void   Init_Weights_post(ConGroup* cg, Network* net, int thr_no)
  { };
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  inline virtual void   Init_dWt(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Learning initialize weight-change variables for all cons

  virtual void   RenormWeights(ConGroup* cg, Network* net, int thr_no,
                               bool mult_norm, float avg_wt);
  // #CAT_Structure renormalize the weight values using either multiplicative (for positive-only weight values such as Leabra) or subtractive normalization (for pos/neg weight values, such as backprop) to hit the given average weight value -- only affects wt value -- need to call Init_Weights_post afterward at appropriate level! -- receiver based but uses generic, slow interace so can be called either way
  virtual void   RenormScales(ConGroup* cg, Network* net, int thr_no,
                              bool mult_norm, float avg_wt)  { };
  // #CAT_Structure renormalize the connection scale values using either multiplicative (for positive-only weight values such as Leabra) or subtractive normalization (for pos/neg weight values, such as backprop) to hit the given average weight value -- only affects scale value -- only for algorithms that support scale (Leabra) -- need to call Init_Weights_post afterward at appropriate level! -- receiver based but uses generic, slow interace so can be called either way

  inline float          C_Compute_Netin(const float wt, const float su_act)
  { return wt * su_act; }
  // #IGNORE 
  inline virtual float  Compute_Netin(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Activation compute net input for weights in this con group

  inline void           C_Send_Netin(const float wt, float* send_netin_vec,
                                     const int ru_idx, const float su_act)
  { send_netin_vec[ru_idx] += wt * su_act; }
  // #IGNORE 
  inline virtual void   Send_Netin(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Activation sender-based net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units
  inline virtual void   Send_Netin_PerPrjn(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Activation sender-based net input, keeping projections separate, for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units

  inline float          C_Compute_Dist(const float wt, const float su_act)
  { const float tmp = su_act - wt; return tmp * tmp; }
  // #IGNORE 
  inline virtual float  Compute_Dist(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Activation compute net distance for con group (ie. euclidean distance)
  inline void           C_Compute_dWt(float& wt, float& dwt, const float ru_act,
                                      const float su_act)
  { dwt += ru_act * su_act; }
  // #IGNORE define in subclass to take proper args -- this is just for demo -- best to take all the vals as direct floats
  inline virtual void   Compute_dWt(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Learning compute the delta-weight change -- recv owns cons version

  inline void           C_Compute_Weights(float& wt, float& dwt)
  { wt += dwt; dwt = 0.0f; }
  // #IGNORE define in subclass to take proper args -- this is just for demo -- best to take all the vals as direct floats
  inline virtual void   Compute_Weights(ConGroup* cg, Network* net, int thr_no);
  // #CAT_Learning update weights (ie. add delta-wt to wt, zero delta-wt)

  ////////////////////////////////////////////////////////////////////////////////
  //    Bias-Weight versions, operate on UnitVars

  inline virtual void   B_Init_Weights(UnitVars* uv, Network* net, int thr_no);
  // #CAT_Learning bias weight: initialize weight state variables (ie. at beginning of training)
  inline virtual void   B_Init_dWt(UnitVars* uv, Network* net, int thr_no);
  // #CAT_Learning bias weight: initialize weight-change variables for all cons
  inline virtual void   B_Init_Weights_post(UnitVars* uv, Network* net, int thr_no)
  { };
  // #CAT_Learning bias weight: post-weight init
  inline virtual void   B_Compute_dWt(UnitVars* uv, Network* net, int thr_no);
  // #CAT_Learning bias weight: compute the delta-weight change -- recv owns cons version
  inline virtual void   B_Compute_Weights(UnitVars* uv, Network* net, int thr_no);
  // #CAT_Learning bias weight: update weights (ie. add delta-wt to wt, zero delta-wt)

  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  virtual bool          SaveConVarToWeights(MemberDef* md);
  // should given connection variable be saved to the weights file?  default method here just checks md->HasOption("SAVE") -- derived classes can determine based on other options
  
  virtual void          GetPrjnName(Projection& prjn, String& nm) { };
  // add anything special for this type of connection to the projection name -- default name (FM_from) is provided as a nm value, which this function can modify in any way

  virtual bool          CheckConfig_RecvCons(Projection* prjn, bool quiet=false);
  // check for for misc configuration settings required by different algorithms

  virtual  void         Init_Weights_Net();
  // #BUTTON #CAT_Learning initializes all weights in the network

  String          GetTypeDecoKey() const override { return "ConSpec"; }
  String          GetToolbarName() const override { return "con spec"; }

  void  InitLinks() override;
  void  CutLinks() override;
  void  Copy_(const ConSpec& cp);
  TA_BASEFUNS(ConSpec);
protected:
  SPEC_DEFAULTS;
  bool  CheckType_impl(TypeDef* td) override;
  bool  CheckObjectType_impl(taBase* obj) override; // don't do checking on 1st con group in units
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()               { CutLinks(); }
  void  Defaults_init()         { };
};

SPECPTR_OF(ConSpec);

#endif // ConSpec_h
