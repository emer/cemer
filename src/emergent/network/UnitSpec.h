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

#ifndef UnitSpec_h
#define UnitSpec_h 1

// parent includes:
#include <BaseSpec>
#include <SpecPtr>

// member includes:
#include <MinMaxRange>
#include <ConSpec>

// declare all other types mentioned but not required to include:
class RecvCons; //
class SendCons; //
class Unit; //
class Network; //

eTypeDef_Of(UnitSpec);

class E_API UnitSpec: public BaseSpec {
  // ##CAT_Spec Generic Unit Specification
INHERITED(BaseSpec)
  static RecvCons*      rcg_rval; // return value for connecting
  static SendCons*      scg_rval; // return value for connecting
public:

  MinMaxRange   act_range;
  // #CAT_Activation range of activation for units
  TypeDef*      bias_con_type;
  // #HIDDEN #TYPE_Connection #NULL_OK #CAT_Structure type of bias connection to make
  ConSpec_SPtr  bias_spec;
  // #CAT_Structure con spec that controls the bias connection on the unit
  float         sse_tol;
  // #CAT_Statistic tolerance for computing sum-squared error and other error metrics on a per-unit basis

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning
  //    Many functions operate directly on the units via threads, and then
  //    call through to the layers for any layer-level subsequent processing
  //    units typically call spec versions except for basic stuff

  //    Init functions are NOT threaded, while Compute functions are

  virtual void  Init_Acts(Unit* u, Network* net);
  // #CAT_Activation initialize unit activation state variables
  virtual void  Init_dWt(Unit* u, Network* net);
  // #CAT_Learning initialize the weight change variables
  virtual void  Init_Weights(Unit* u, Network* net);
  // #CAT_Learning init weight state variables
  virtual void  Init_Weights_post(Unit* u, Network* net);
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  virtual void  Compute_Netin(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Activation compute net input: activations times weights (receiver based)
  virtual void  Send_Netin(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Activation send net input to all units I send to (sender based)
  virtual void  Compute_SentNetin(Unit* u, Network* net, float sent_netin);
  // #CAT_Activation called by network-level Send_Netin function to integrate sent netin value with current net input value -- default is just to set to net val + bias wt if avail
  virtual void  Compute_Act(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Activation compute the activation value of the unit: what it sends to other units

  virtual void  Compute_dWt(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Learning compute change in weights: the mechanism of learning
  virtual void  Compute_Weights(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Learning update weights from deltas

  virtual float Compute_SSE(Unit* u, Network* net, bool& has_targ);
  // #CAT_Statistic compute sum squared error for this unit -- uses sse_tol so error is 0 if within tolerance -- has_targ indicates if there is actually a target value (else the return value is 0)
  virtual bool  Compute_PRerr(Unit* u, Network* net, float& true_pos, float& false_pos, float& false_neg);
  // #CAT_Statistic compute precision and recall error statistics for this unit -- true positive, false positive, and false negative -- returns true if unit actually has a target value specified (otherwise everything is 0) -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance


  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  virtual void  BuildBiasCons();
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE #CAT_Structure build the bias connections according to specified type

  virtual bool  CheckConfig_Unit(Unit* un, bool quiet=false);
  // #CAT_ObjectMgmt check for for misc configuration settings required by different algorithms

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const UnitSpec& cp);
  TA_BASEFUNS(UnitSpec);
protected:
  SPEC_DEFAULTS;
  void         UpdateAfterEdit_impl() override;
  void         CheckThisConfig_impl(bool quiet, bool& ok) override;
  bool         CheckType_impl(TypeDef* td) override;
  bool         CheckObjectType_impl(taBase* obj) override;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init()         { };
};

SpecPtr_of(UnitSpec);

#endif // UnitSpec_h
