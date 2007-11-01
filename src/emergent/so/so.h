// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "emergent_base.h"
#include "netstru.h"
#include "emergent_project.h"

#include "so_def.h"
#include "so_TA_type.h"

class SoCon;
class SoConSpec;
class SoRecvCons;
class SoUnit;
class SoUnitSpec;
class SoLayer;
class SoLayerSpec; //

class SO_API SoCon : public Connection {
  // ##CAT_So generic self-organizing algorithm connection
public:
  float		pdw;		// #NO_SAVE previous delta-weight change

  SoCon() { pdw = 0.0f; }
};

class SO_API SoConSpec : public ConSpec {
  // ##CAT_So #VIRT_BASE generic self-organizing algorithm connection specification
INHERITED(ConSpec)
public:
  enum AvgInActSource {
    LAYER_AVG_ACT,		// from layer's average activation
    COMPUTE_AVG_ACT 		// compute the avg_in_act directly from my inputs (more computationally expensive but always accurate)
  };

  float		lrate;		// learning rate
  AvgInActSource avg_act_source; // source of average input actviation value

  void 		C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) 
  { ConSpec::C_Init_Weights(cg, cn, ru, su); ((SoCon*)cn)->pdw=0.0f; }

  inline void	C_Compute_Weights(SoCon* cn, Unit* ru, Unit* su); 
  inline void	Compute_Weights(RecvCons* cg, Unit* ru);

  inline virtual void	Compute_AvgInAct(SoRecvCons* cg, Unit* ru);
  // compute the average input activation 

  void	InitLinks();
  SIMPLE_COPY(SoConSpec);
  TA_BASEFUNS(SoConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class SO_API SoRecvCons : public RecvCons {
  // ##CAT_So self-organizing connection group
INHERITED(RecvCons)
public:
  float		avg_in_act;	// average input activation
  float		sum_in_act;	// summed input activation

  void		Compute_AvgInAct(Unit* ru)
  { ((SoConSpec*)GetConSpec())->Compute_AvgInAct(this, ru); }
  // compute the average (and sum) input activation

  void	Copy_(const SoRecvCons& cp);
  TA_BASEFUNS(SoRecvCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class SO_API SoSendCons : public SendCons {
  // ##CAT_So group of self-organizing sending connections
INHERITED(SendCons)
public:
  TA_BASEFUNS_NOCOPY(SoSendCons);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class SO_API SoUnitSpec : public UnitSpec {
  // ##CAT_So generic self-organizing unit spec: linear act of std dot-product netin
INHERITED(UnitSpec)
public:
  void		Init_Acts(Unit* u);
  void		Compute_Act(Unit* u);

  virtual void	Compute_AvgInAct(Unit* u);
  // compute average input activations
  virtual void	GraphActFun(DataTable* graph_data, float min = -5.0, float max = 5.0);
  // #BUTTON #NULL_OK graph the activation function (NULL = new graph data)

  void	InitLinks();
  TA_BASEFUNS_NOCOPY(SoUnitSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class SO_API ThreshLinSoUnitSpec : public SoUnitSpec {
  // threshold-linear self-org unit spec
INHERITED(SoUnitSpec)
public:
  float		threshold;

  void		Compute_Act(Unit* u);

  TA_SIMPLE_BASEFUNS(ThreshLinSoUnitSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};
  
class SO_API SoUnit : public Unit {
  // ##CAT_So generic self-organizing unit
INHERITED(Unit)
public:
  float		act_i;
  // independent activation of the unit (before layer-level rescaling)

  void		Compute_AvgInAct()
  { ((SoUnitSpec*)GetUnitSpec())->Compute_AvgInAct(this); }

  void Copy_(const SoUnit& cp) { act_i = cp.act_i; }
  TA_BASEFUNS(SoUnit);
private:
  void	Initialize();
  void	Destroy()	{ };
};


class SO_API SoLayerSpec : public LayerSpec {
  // ##CAT_So generic self-organizing layer specification
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

  virtual void	Compute_Netin(SoLayer* lay);
  virtual void	Compute_Act(SoLayer* lay);
  virtual void	Compute_AvgAct(SoLayer* lay);
  virtual void	Compute_dWt(SoLayer* lay);
  virtual void	Compute_Weights(SoLayer* lay);

  TA_SIMPLE_BASEFUNS(SoLayerSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

SpecPtr_of(SoLayerSpec);

class SO_API SoLayer : public Layer {
  // ##CAT_So generic self-organizing layer
INHERITED(Layer)
public:
  SoLayerSpec_SPtr	spec;	// the spec for this layer
  float		avg_act;	// average activation over layer
  float		sum_act;	// summed activation over layer
  Unit*		winner;		// #READ_ONLY #NO_SAVE winning unit

  // the spec now does all of the updating..
  void		Compute_Netin()		{ spec->Compute_Netin(this); }
  void		Compute_Act()		{ spec->Compute_Act(this); }
  void		Compute_AvgAct()	{ spec->Compute_AvgAct(this); }
  void		Compute_dWt()		{ spec->Compute_dWt(this); }
  void		Compute_Weights()		{ spec->Compute_Weights(this); }

  bool		SetLayerSpec(LayerSpec* sp);
  LayerSpec*	GetLayerSpec()		{ return (LayerSpec*)spec.spec; }

  void	InitLinks();
  void	CutLinks();
  void	Copy_(const SoLayer& cp);
  TA_BASEFUNS(SoLayer);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};


//////////////////////////////////
//	Inline Functions	//
//////////////////////////////////

inline void SoConSpec::C_Compute_Weights(SoCon* cn, Unit*, Unit*) {
  cn->pdw = cn->dwt;
  cn->wt += lrate * cn->dwt;
  cn->dwt = 0.0f;
}
inline void SoConSpec::Compute_Weights(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_Weights((SoCon*)cg->Cn(i), ru, cg->Un(i)));
  ApplyLimits(cg, ru);
}

inline void SoConSpec::Compute_AvgInAct(SoRecvCons* cg, Unit*) {
  if(avg_act_source == LAYER_AVG_ACT) {
    cg->avg_in_act = ((SoLayer*)cg->prjn->from.ptr())->avg_act;
    cg->sum_in_act = ((SoLayer*)cg->prjn->from.ptr())->sum_act;
  }
  else if(cg->cons.size > 0) {
    cg->sum_in_act = 0.0f;
    int i;
    for(i=0; i<cg->cons.size; i++)
      cg->sum_in_act += ((Unit*)cg->Un(i))->act;
    cg->avg_in_act = cg->sum_in_act / (float)cg->cons.size;
  }
}


//////////////////////////////////
//	Simple Hebbian 		//
//////////////////////////////////

class SO_API HebbConSpec : public SoConSpec {
  // simple hebbian learning
INHERITED(SoConSpec)
public:
  inline void	C_Compute_dWt(SoCon* cn, SoRecvCons* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(RecvCons* cg, Unit* ru);
  // compute weight change according to simple hebb function

  TA_BASEFUNS(HebbConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

inline void HebbConSpec::
C_Compute_dWt(SoCon* cn, SoRecvCons*, Unit* ru, Unit* su)
{
  cn->dwt += ru->act * su->act;
}

inline void HebbConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoRecvCons*)cg, ru, cg->Un(i)));
}

//////////////////////////////////
//	Simple SoftMax		//
//////////////////////////////////

class SO_API SoftMaxLayerSpec : public SoLayerSpec {
  // soft competitive learning layer spec: does a softmax on the units
INHERITED(SoLayerSpec)
public:
  float		softmax_gain;	// gain of the softmax function

  void		Compute_Act(SoLayer* lay);
  // set activation to be softmax of unit activations

  TA_SIMPLE_BASEFUNS(SoftMaxLayerSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class SO_API SoNetwork : public Network {
  // ##CAT_So network for self-organizing networks
INHERITED(Network)
public:

  override void		Compute_Act();

  virtual void		Trial_Run(); // run one trial of So
  
  override void	SetProjectionDefaultTypes(Projection* prjn);

  TA_BASEFUNS_NOCOPY(SoNetwork);
private:
  void	Initialize();
  void 	Destroy()		{}
};

class SO_API SoProject : public ProjectBase {
  // ##CAT_So project for self-organizing networks
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

class SO_API SoWizard : public Wizard {
  // ##CAT_So self-organizing networks specific wizard for automating construction of simulation objects
INHERITED(Wizard)
public:

  override bool	StdProgs();

  TA_BASEFUNS_NOCOPY(SoWizard);
private:
  void 	Initialize() 	{ };
  void 	Destroy()	{ };
};

#endif // so_h

