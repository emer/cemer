// Co2018ght 2006-2017, Regents of the University of Colorado,
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

// so.h: self-organizing learning algorithms

#ifndef so_h
#define so_h

#include <Network>
#include <ProjectBase>
#include <Wizard>

#include "network_def.h"

class SoCon;
class SoConSpec;
class SoConState;
class SoUnitState;
class SoUnit;
class SoUnitSpec;
class SoLayer;
class SoLayerSpec;
class SoNetwork;
//

eTypeDef_Of(SoCon);

class E_API SoCon : public Connection {
  // #STEM_BASE ##CAT_So generic self-organizing algorithm connection
public:
  float		pdw;		// #NO_SAVE previous delta-weight change

  SoCon() { pdw = 0.0f; }
};

eTypeDef_Of(SoConSpec);

class E_API SoConSpec : public ConSpec {
  // #STEM_BASE ##CAT_So #VIRT_BASE generic self-organizing algorithm connection specification
INHERITED(ConSpec)
public:
  enum AvgInActSource {
    LAYER_AVG_ACT,		// from layer's average activation
    COMPUTE_AVG_ACT 		// compute the avg_in_act directly from my inputs (more computationally expensive but always accurate)
  };
  enum SoConVars {
    PDW = DWT+1,                // previous delta weight
  };

  float		lrate;		// learning rate
  AvgInActSource avg_act_source; // source of average input actviation value


  inline void   Init_dWt(ConState* cg, Network* net, int thr_no) override {
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    for(int i=0; i<cg->size; i++) {
      C_Init_dWt(dwts[i]);
      pdws[i] = 0.0f;
    }
  }

  inline void   Init_Weights_post(ConState* cg, Network* net, int thr_no) override {
    float* pdws = cg->OwnCnVar(PDW);
    for(int i=0; i<cg->size; i++) {
      pdws[i] = 0.0f;
    }
  }
  // all non-wt, pdw vars MUST be initialized in _post!

  inline void   B_Init_dWt(UnitState* ru, Network* net, int thr_no) override;

  inline void	C_Compute_Weights(float& wt, float& dwt, float& pdw)
  { pdw = dwt;  wt += lrate * dwt;  dwt = 0.0f; }
  inline void	Compute_Weights(ConState* cg, Network* net, int thr_no) override;

  inline virtual void	Compute_AvgInAct(SoConState* cg, SoNetwork* net, int thr_no);
  // compute the average input activation 

  void	InitLinks() override;
  SIMPLE_COPY(SoConSpec);
  TA_BASEFUNS(SoConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

eTypeDef_Of(SoConState);

class E_API SoConState : public ConState {
  // #STEM_BASE ##CAT_So self-organizing connection group
INHERITED(ConState)
public:
  float		avg_in_act;	// average input activation
  float		sum_in_act;	// summed input activation
};

eTypeDef_Of(SoUnitSpec);

class E_API SoUnitSpec : public UnitSpec {
  // #STEM_BASE ##CAT_So generic self-organizing unit spec: linear act of std dot-product netin
INHERITED(UnitSpec)
public:
  void	Init_Acts(UnitState* u, Network* net, int thr_no) override;
  void	Compute_Act(UnitState* u, Network* net, int thr_no) override;

  virtual void	Compute_AvgInAct(SoUnitState* u, SoNetwork* net, int thr_no);
  // compute average input activations
  virtual void	GraphActFun(DataTable* graph_data, float min = -5.0, float max = 5.0);
  // #BUTTON #NULL_OK graph the activation function (NULL = new graph data)

  TypeDef*      MinBiasSpecType() const override { return &TA_SoConSpec; }
  
  void	InitLinks() override;
  TA_BASEFUNS_NOCOPY(SoUnitSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ThreshLinSoUnitSpec);

class E_API ThreshLinSoUnitSpec : public SoUnitSpec {
  // threshold-linear self-org unit spec
INHERITED(SoUnitSpec)
public:
  float		threshold;

  void	Compute_Act(UnitState* u, Network* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(ThreshLinSoUnitSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};
  
eTypeDef_Of(SoUnitState);

class E_API SoUnitState : public UnitState {
  // #STEM_BASE ##CAT_So generic self-organizing unit variables
INHERITED(UnitState)
public:
  float		bias_pdw;
  // #NO_SAVE the previous delta-weight (for momentum), bias
  float		act_i;
  // #VIEW_HOT independent activation of the unit (before layer-level rescaling)
};


eTypeDef_Of(SoUnit);

class E_API SoUnit : public Unit {
  // #STEM_BASE ##CAT_So generic self-organizing unit
INHERITED(Unit)
public:
  inline UnitState::ExtFlags ext_flag() { return GetUnitState()->ext_flag; }
  // #CAT_UnitVar external input flags -- determines whether the unit is receiving an external input (EXT), target (TARG), or comparison value (COMP)
  inline float& targ()  { return GetUnitState()->targ; }
  // #VIEW_HOT #CAT_UnitVar target value: drives learning to produce this activation value
  inline float& ext()   { return GetUnitState()->ext; }
  // #VIEW_HOT #CAT_UnitVar external input: drives activation of unit from outside influences (e.g., sensory input)
  inline float& act()   { return GetUnitState()->act; }
  // #VIEW_HOT #CAT_UnitVar activation value -- what the unit communicates to others
  inline float& net()   { return GetUnitState()->net; }
  // #VIEW_HOT #CAT_UnitVar net input value -- what the unit receives from others  (typically sum of sending activations times the weights)
  inline float& bias_wt() { return GetUnitState()->bias_wt; }
  // #VIEW_HOT #CAT_UnitVar bias weight value -- the bias weight acts like a connection from a unit that is always active with a constant value of 1 -- reflects intrinsic excitability from a biological perspective
  inline float& bias_dwt() { return GetUnitState()->bias_dwt; }
  // #VIEW_HOT #CAT_UnitVar change in bias weight value as computed by a learning mechanism

  inline float& bias_pdw() { return ((SoUnitState*)GetUnitState())->bias_pdw; }
  // #VIEW_HOT #CAT_UnitVar the previous delta-weight (for momentum), bias
  inline float& act_i() { return ((SoUnitState*)GetUnitState())->act_i; }
  // #VIEW_HOT #CAT_UnitVar independent activation of the unit (before layer-level rescaling)

  TA_BASEFUNS_NOCOPY(SoUnit);
private:
  void	Initialize();
  void	Destroy()	{ };
};


eTypeDef_Of(SoLayerSpec);

class E_API SoLayerSpec : public LayerSpec {
  // #STEM_BASE ##CAT_So generic self-organizing layer specification
INHERITED(LayerSpec)
public:
  enum	NetInType {		// type of netinput measure unit has
    MAX_NETIN_WINS,		// maximum net input wins (like dot product)
    MIN_NETIN_WINS		// minimum net input wins (like distance)
  };

  NetInType	netin_type;	// type of netinput measure for computing winner

  virtual SoUnit*	FindMaxNetIn(SoLayer* lay);
  // finds unit with the maximum net-input value in the layer (clears acts too)
  virtual SoUnit*	FindMinNetIn(SoLayer* lay);
  // finds unit with the minimum net-input value in the layer (clears acts too)
  virtual SoUnit*	FindWinner(SoLayer* lay);
  // finds the winning unit according to netin_type (clears acts too)

  virtual void	Compute_Act_post(SoLayer* lay, SoNetwork* net);
  // perform post-processing of activations, often amounting to full recomputing of them depending on layer-level constraints -- also calls AvgAct at the end
  virtual void	Compute_AvgAct(SoLayer* lay, SoNetwork* net);

  TA_SIMPLE_BASEFUNS(SoLayerSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

SPECPTR_OF(SoLayerSpec);

eTypeDef_Of(SoLayer);

class E_API SoLayer : public Layer {
  // #STEM_BASE ##CAT_So generic self-organizing layer
INHERITED(Layer)
public:
  SoLayerSpec_SPtr spec;	// the spec for this layer
  float		avg_act;	// average activation over layer
  float		sum_act;	// summed activation over layer
  SoUnit*	winner;		// #READ_ONLY #NO_SAVE winning unit

  void	Compute_Act_post(SoNetwork* net) { spec->Compute_Act_post(this, net); }
  void	Compute_AvgAct(SoNetwork* net)	{ spec->Compute_AvgAct(this, net); }

  bool		SetLayerSpec(LayerSpec* sp) override;
  LayerSpec*	GetLayerSpec()  override { return (LayerSpec*)spec.spec; }

  void	CheckSpecs() override;

  void	InitLinks() override;
  void	CutLinks() override;
  void	Copy_(const SoLayer& cp);
  TA_BASEFUNS(SoLayer);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

//////////////////////////////////
//	Simple SoftMax		//
//////////////////////////////////

eTypeDef_Of(SoftMaxLayerSpec);

class E_API SoftMaxLayerSpec : public SoLayerSpec {
  // soft competitive learning layer spec: does a softmax on the units
INHERITED(SoLayerSpec)
public:
  float		softmax_gain;	// gain of the softmax function

  void	Compute_Act_post(SoLayer* lay, SoNetwork* net) override;
  // set activation to be softmax of unit activations

  TA_SIMPLE_BASEFUNS(SoftMaxLayerSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(SoNetwork);

class E_API SoNetwork : public Network {
  // #STEM_BASE ##CAT_So network for self-organizing networks
INHERITED(Network)
public:

  void	 Compute_NetinAct() override;

  virtual void	 Trial_Run(); // run one trial of So
  
  void	SetProjectionDefaultTypes(Projection* prjn) override;
  void  BuildNullUnit() override;

  TA_BASEFUNS_NOCOPY(SoNetwork);
private:
  void	Initialize();
  void 	Destroy()		{}
};

//////////////////////////////////
//	Inline Functions	//
//////////////////////////////////

inline void SoConSpec::B_Init_dWt(UnitState* ru, Network* net, int thr_no) {
  C_Init_dWt(ru->bias_dwt);
  ((SoUnitState*)ru)->bias_pdw = 0.0f;
}

inline void SoConSpec::Compute_Weights(ConState* cg, Network* net, int thr_no) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  float* pdws = cg->OwnCnVar(PDW);
  CON_GROUP_LOOP(cg, C_Compute_Weights(wts[i], dwts[i], pdws[i]));
  ApplyLimits(cg, net, thr_no);
}

inline void SoConSpec::Compute_AvgInAct(SoConState* cg, SoNetwork* net, int thr_no) {
  if(avg_act_source == LAYER_AVG_ACT) {
    cg->avg_in_act = ((SoLayer*)cg->prjn->from.ptr())->avg_act;
    cg->sum_in_act = ((SoLayer*)cg->prjn->from.ptr())->sum_act;
  }
  else if(cg->size > 0) {
    float sum = 0.0f;
    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      sum += cg->UnVars(i,net)->act;
    }
    cg->sum_in_act = sum;
    cg->avg_in_act = sum / (float)cg->size;
  }
}


//////////////////////////////////
//	Simple Hebbian 		//
//////////////////////////////////

eTypeDef_Of(HebbConSpec);

class E_API HebbConSpec : public SoConSpec {
  // simple hebbian learning
INHERITED(SoConSpec)
public:
  inline void	C_Compute_dWt(float& dwt, const float ru_act, const float su_act) 
  { dwt += ru_act * su_act; }

  inline void 	Compute_dWt(ConState* cg, Network* net, int thr_no) override {
    SoUnitState* ru = (SoUnitState*)cg->ThrOwnUnVars(net, thr_no);
    float* dwts = cg->OwnCnVar(DWT);
    const float ru_act = ru->act;
    CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_act, cg->UnVars(i,net)->act));
  }
  // compute weight change according to simple hebb function

  TA_BASEFUNS(HebbConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////////////////////

eTypeDef_Of(SoProject);

class E_API SoProject : public ProjectBase {
  // #STEM_BASE ##CAT_So project for self-organizing networks
INHERITED(ProjectBase)
public:

  TA_BASEFUNS_NOCOPY(SoProject);
private:
  void	Initialize();
  void 	Destroy()		{}
};

//////////////////////////////////
//	So Wizard		//
//////////////////////////////////

eTypeDef_Of(SoWizard);

class E_API SoWizard : public Wizard {
  // #STEM_BASE ##CAT_So self-organizing networks specific wizard for automating construction of simulation objects
INHERITED(Wizard)
public:

  bool	StdProgs() override;
  bool	TestProgs(Program* call_test_from, bool call_in_loop=true, int call_modulus=1) override;

  TA_BASEFUNS_NOCOPY(SoWizard);
private:
  void 	Initialize();
  void 	Destroy()	{ };
};

#endif // so_h

