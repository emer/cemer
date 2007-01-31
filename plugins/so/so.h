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

#include "ta_plugin.h"
#include "netstru.h"
#include "pdp_project.h"

#include "so_TA_type.h"

// will need to be defined for Windows
#define SO_API 

class SoCon;
class SoConSpec;
class SoRecvCons;
class SoUnit;
class SoUnitSpec;
class SoLayer;
class SoLayerSpec;

class SoPlugin; //

#ifndef __MAKETA__
// The SoPlugin class allows this to be a plugin. It usually won't
// need to be majorly modified. It is not seen by `maketa' as it
// cannot grok many Qt c++ constructs.

class SO_API SoPlugin : public QObject, 
                           public IPlugin
{ // #NO_CSS #NO_MEMBERS
  Q_OBJECT

  // Tells Qt which interfaces are implemented by this class
  Q_INTERFACES(IPlugin)
public:
  static const taVersion	version;
  
  SoPlugin(QObject* par = NULL);

 public: // IPlugin interface
  void*		This() {return this;}
  TypeDef*	GetTypeDef() {return &TA_SoPlugin;} 
  const char*	desc() {return "So algorithm for PDP++";}
  const char*	name() {return "So";}
  const char*	uniqueId() {return "so.ccnlab.psych.colorado.edu";}
  const char*	url();
  
  int		NotifyTacssVersion(const taVersion& tav, bool& is_ok) {return 0;}
    // we pass ta/css version; set is_ok false if this version is no good for plugin
  int		GetVersion(taVersion& tav) {tav = version; return 0;}
  int 		InitializeTypes();
  int 		InitializePlugin();
};

// Associates a string with the interface SoPlugin
Q_DECLARE_INTERFACE(SoPlugin, "pdp.SoPlugin/1.0")
#endif // !__MAKETA__

class SO_API SoCon : public Connection {
  // generic self-organizing algorithm connection
public:
  float		dwt;		// #NO_VIEW #NO_SAVE resulting net weight change
  float		pdw;		// #NO_SAVE previous delta-weight change

  SoCon() { dwt = pdw = 0.0f; }
};

class SO_API SoConSpec : public ConSpec {
  // #VIRT_BASE generic self-organizing algorithm connection specification
public:
  enum AvgInActSource {
    LAYER_AVG_ACT,		// from layer's average activation
    COMPUTE_AVG_ACT 		// compute the avg_in_act directly from my inputs
  };

  float		lrate;		// learning rate
  AvgInActSource avg_act_source; // source of average input actviation value

  void 		C_Init_dWt(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) 
  { ConSpec::C_Init_dWt(cg, cn, ru, su); ((SoCon*)cn)->dwt=0.0f; }

  void 		C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) 
  { ConSpec::C_Init_Weights(cg, cn, ru, su); ((SoCon*)cn)->pdw=0.0f; }

  inline void	C_UpdateWeights(SoCon* cn, Unit* ru, Unit* su); 
  inline void	Compute_Weights(RecvCons* cg, Unit* ru);

  inline virtual void	Compute_AvgInAct(SoRecvCons* cg, Unit* ru);
  // compute the average input activation 

  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(SoConSpec);
  COPY_FUNS(SoConSpec, ConSpec);
  TA_BASEFUNS(SoConSpec);
};

class SO_API SoRecvCons : public RecvCons {
  // self-organizing connection group
public:
  float		avg_in_act;	// average input activation
  float		sum_in_act;	// summed input activation

  void		Compute_AvgInAct(Unit* ru)
  { ((SoConSpec*)spec.spec)->Compute_AvgInAct(this, ru); }
  // compute the average (and sum) input activation

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(SoRecvCons);
  COPY_FUNS(SoRecvCons, RecvCons);
  TA_BASEFUNS(SoRecvCons);
};

class SO_API SoUnitSpec : public UnitSpec {
  // generic self-organizing unit spec: linear act of std dot-product netin
INHERITED(UnitSpec)
public:
  void		Init_Acts(Unit* u);

  void		Compute_Act(Unit* u);

  virtual void	Compute_AvgInAct(Unit* u); //

//TODO  virtual void	GraphActFun(GraphLog* graph_log, float min = -5.0, float max = 5.0);
  // #BUTTON #NULL_OK graph the activation function (NULL = new graph log)

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(SoUnitSpec);
};

class SO_API ThreshLinSoUnitSpec : public SoUnitSpec {
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
  
class SO_API SoUnit : public Unit {
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


class SO_API SoLayerSpec : public LayerSpec {
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

  virtual void	Compute_Netin(SoLayer* lay);
  virtual void	Compute_Act(SoLayer* lay);
  virtual void	Compute_AvgAct(SoLayer* lay);
  virtual void	Compute_dWt(SoLayer* lay);
  virtual void	Compute_Weights(SoLayer* lay);

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(SoLayerSpec);
  COPY_FUNS(SoLayerSpec, LayerSpec);
  TA_BASEFUNS(SoLayerSpec);
};

SpecPtr_of(SoLayerSpec);

class SO_API SoLayer : public Layer {
  // generic self-organizing layer
public:
  SoLayerSpec_SPtr	spec;	// the spec for this layer
  float		avg_act;	// average activation over layer
  float		sum_act;	// summed activation over layer
  Unit*		winner;		// #READ_ONLY winning unit

  // the spec now does all of the updating..
  void		Compute_Netin()		{ spec->Compute_Netin(this); }
  void		Compute_Act()		{ spec->Compute_Act(this); }
  void		Compute_AvgAct()	{ spec->Compute_AvgAct(this); }
  void		Compute_dWt()		{ spec->Compute_dWt(this); }
  void		Compute_Weights()		{ spec->Compute_Weights(this); }

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
inline void SoConSpec::Compute_Weights(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_UpdateWeights((SoCon*)cg->Cn(i), ru, cg->Un(i)));
  ApplyLimits(cg, ru);
}

inline void SoConSpec::Compute_AvgInAct(SoRecvCons* cg, Unit*) {
  if(avg_act_source == LAYER_AVG_ACT) {
    cg->avg_in_act = ((SoLayer*)cg->prjn->from)->avg_act;
    cg->sum_in_act = ((SoLayer*)cg->prjn->from)->sum_act;
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
public:
  inline void	C_Compute_dWt(SoCon* cn, SoRecvCons* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(RecvCons* cg, Unit* ru);
  // compute weight change according to simple hebb function

  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(HebbConSpec);
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


class PDP_API SoNetwork : public Network {
  // project for feedforward backpropagation networks (recurrent backprop is in RBPNetwork)
INHERITED(Network)
public:

  override void		Compute_Act();

  override bool	CheckBuild(bool quiet=false);  
  virtual void		Trial_Run(); // run one trial of So
  
  void	Initialize();
  void 	Destroy()		{}
  TA_BASEFUNS(SoNetwork);
};

class PDP_API SoProject : public ProjectBase {
  // project for so networks
INHERITED(ProjectBase)
public:

  void	Initialize();
  void 	Destroy()		{}
  TA_BASEFUNS(SoProject);
};


#endif // so_h

