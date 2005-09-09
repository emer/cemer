/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// so.h: self-organizing learning algorithms

#ifndef so_h
#define so_h

#include <so/so_TA_type.h>
#include <pdp/netstru.h>
#include <pdp/sched_proc.h>

class SoCon;
class SoConSpec;
class SoCon_Group;
class SoUnit;
class SoUnitSpec;
class SoLayer;
class SoLayerSpec;

class SoCon : public Connection {
  // generic self-organizing algorithm connection
public:
  float		dwt;		// #NO_VIEW #NO_SAVE resulting net weight change
  float		pdw;		// #NO_SAVE previous delta-weight change

  void 	Initialize()		{ dwt = pdw = 0.0f; }
  void	Destroy()		{ };
  void	Copy_(const SoCon& cp)	{ dwt = cp.dwt; pdw = cp.pdw; }
  COPY_FUNS(SoCon, Connection);
  TA_BASEFUNS(SoCon);
};

class SoConSpec : public ConSpec {
  // #VIRT_BASE generic self-organizing algorithm connection specification
public:
  enum AvgInActSource {
    LAYER_AVG_ACT,		// from layer's average activation
    COMPUTE_AVG_ACT 		// compute the avg_in_act directly from my inputs
  };

  float		lrate;		// learning rate
  AvgInActSource avg_act_source; // source of average input actviation value

  void 		C_InitWtDelta(Con_Group* cg, Connection* cn, Unit* ru, Unit* su) 
  { ConSpec::C_InitWtDelta(cg, cn, ru, su); ((SoCon*)cn)->dwt=0.0f; }

  void 		C_InitWtState(Con_Group* cg, Connection* cn, Unit* ru, Unit* su) 
  { ConSpec::C_InitWtState(cg, cn, ru, su); ((SoCon*)cn)->pdw=0.0f; }

  inline void	C_UpdateWeights(SoCon* cn, Unit* ru, Unit* su); 
  inline void	UpdateWeights(Con_Group* cg, Unit* ru);

  inline virtual void	Compute_AvgInAct(SoCon_Group* cg, Unit* ru);
  // compute the average input activation 

  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(SoConSpec);
  COPY_FUNS(SoConSpec, ConSpec);
  TA_BASEFUNS(SoConSpec);
};

class SoCon_Group : public Con_Group {
  // self-organizing connection group
public:
  float		avg_in_act;	// average input activation
  float		sum_in_act;	// summed input activation

  void		Compute_AvgInAct(Unit* ru)
  { ((SoConSpec*)spec.spec)->Compute_AvgInAct(this, ru); }
  // compute the average (and sum) input activation

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(SoCon_Group);
  COPY_FUNS(SoCon_Group, Con_Group);
  TA_BASEFUNS(SoCon_Group);
};

class SoUnitSpec : public UnitSpec {
  // generic self-organizing unit spec: linear act of std dot-product netin
public:
  void		InitState(Unit* u);

  void		Compute_Act(Unit* u);

  virtual void	Compute_AvgInAct(Unit* u);

  virtual void	GraphActFun(GraphLog* graph_log, float min = -5.0, float max = 5.0);
  // #BUTTON #NULL_OK graph the activation function (NULL = new graph log)

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(SoUnitSpec);
};

class ThreshLinSoUnitSpec : public SoUnitSpec {
  // threshold-linear self-org unit spec
public:
  float		threshold;

  void		Compute_Act(Unit* u);

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(ThreshLinSoUnitSpec);
  COPY_FUNS(ThreshLinSoUnitSpec, SoUnitSpec);
  TA_BASEFUNS(ThreshLinSoUnitSpec);
};
  
class SoUnit : public Unit {
  // generic self-organizing unit
public:
  float		act_i;
  // independent activation of the unit (before layer-level rescaling)

  void		Compute_AvgInAct()
  { ((SoUnitSpec*)spec.spec)->Compute_AvgInAct(this); }

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(SoUnit);
};


class SoLayerSpec : public LayerSpec {
  // generic self-organizing layer specification
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

  virtual void	Compute_Net(SoLayer* lay);
  virtual void	Compute_Act(SoLayer* lay);
  virtual void	Compute_AvgAct(SoLayer* lay);
  virtual void	Compute_dWt(SoLayer* lay);
  virtual void	UpdateWeights(SoLayer* lay);

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(SoLayerSpec);
  COPY_FUNS(SoLayerSpec, LayerSpec);
  TA_BASEFUNS(SoLayerSpec);
};

SpecPtr_of(SoLayerSpec);

class SoLayer : public Layer {
  // generic self-organizing layer
public:
  SoLayerSpec_SPtr	spec;	// the spec for this layer
  float		avg_act;	// average activation over layer
  float		sum_act;	// summed activation over layer
  Unit*		winner;		// #READ_ONLY winning unit

  // the spec now does all of the updating..
  void		Compute_Net()		{ spec->Compute_Net(this); }
  void		Compute_Act()		{ spec->Compute_Act(this); }
  void		Compute_AvgAct()	{ spec->Compute_AvgAct(this); }
  void		Compute_dWt()		{ spec->Compute_dWt(this); }
  void		UpdateWeights()		{ spec->UpdateWeights(this); }

  bool		SetLayerSpec(LayerSpec* sp);
  LayerSpec*	GetLayerSpec()		{ return (LayerSpec*)spec.spec; }

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const SoLayer& cp);
  COPY_FUNS(SoLayer, Layer);
  TA_BASEFUNS(SoLayer);
};


//////////////////////////////////
//	Inline Functions	//
//////////////////////////////////

inline void SoConSpec::C_UpdateWeights(SoCon* cn, Unit*, Unit*) {
  cn->pdw = cn->dwt;
  cn->wt += lrate * cn->dwt;
  cn->dwt = 0.0f;
}
inline void SoConSpec::UpdateWeights(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_UpdateWeights((SoCon*)cg->Cn(i), ru, cg->Un(i)));
  ApplyLimits(cg, ru);
}

inline void SoConSpec::Compute_AvgInAct(SoCon_Group* cg, Unit*) {
  if(avg_act_source == LAYER_AVG_ACT) {
    cg->avg_in_act = ((SoLayer*)cg->prjn->from)->avg_act;
    cg->sum_in_act = ((SoLayer*)cg->prjn->from)->sum_act;
  }
  else if(cg->size > 0) {
    cg->sum_in_act = 0.0f;
    int i;
    for(i=0; i<cg->size; i++)
      cg->sum_in_act += ((Unit*)cg->Un(i))->act;
    cg->avg_in_act = cg->sum_in_act / (float)cg->size;
  }
}

//////////////////////////
//	Processes	//
//////////////////////////

class SoTrial : public TrialProcess {
  // one self-organizing trial step
public:
  void		Loop();		
  bool 		Crit()		{ return true; } // executes loop only once

  virtual void	Compute_Act();
  virtual void	Compute_dWt();

  bool	CheckNetwork();
  
  void	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(SoTrial);
};


//////////////////////////////////
//	Simple Hebbian 		//
//////////////////////////////////

class HebbConSpec : public SoConSpec {
  // simple hebbian learning
public:
  inline void	C_Compute_dWt(SoCon* cn, SoCon_Group* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change according to simple hebb function

  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(HebbConSpec);
};

inline void HebbConSpec::
C_Compute_dWt(SoCon* cn, SoCon_Group*, Unit* ru, Unit* su)
{
  cn->dwt += ru->act * su->act;
}

inline void HebbConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoCon_Group*)cg, ru, cg->Un(i)));
}

//////////////////////////////////
//	Simple SoftMax		//
//////////////////////////////////

class SoftMaxLayerSpec : public SoLayerSpec {
  // soft competitive learning layer spec: does a softmax on the units
public:
  float		softmax_gain;	// gain of the softmax function

  void		Compute_Act(SoLayer* lay);
  // set activation to be softmax of unit activations

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(SoftMaxLayerSpec);
  COPY_FUNS(SoftMaxLayerSpec, SoLayerSpec);
  TA_BASEFUNS(SoftMaxLayerSpec);
};

#endif // so_h

