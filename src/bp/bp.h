// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



#ifndef bp_h
#define bp_h

#include "pdp_base.h"
#include "netstru.h"
#include "pdp_project.h"

#include "bp_def.h"
#include "bp_TA_type.h"

// forwards this file
class BpConSpec;
class BpCon;
class BpCon_Group;
class BpUnit;
class BpUnitSpec;


class BP_API BpCon : public Connection {
  // Bp connection
public:
  float 		dEdW; 		// #NO_SAVE derivative of Error wrt weight

  void 	Initialize()		{ dEdW = 0.0f; }
  void 	Destroy()		{ };
  void	Copy_(const BpCon& cp)	{ dEdW = cp.dEdW; }
  COPY_FUNS(BpCon, Connection);
  TA_BASEFUNS(BpCon);
};

// todo: need to figure out why bp uses dEdW like others use dwt -- now that 
// dwt is moved to the base connection..

// ConSpec now has 3 versions of some functions, the two regular ones
// (one for the group and one for the connection), and now a third which is a
// virtual version of the C_ for use with the bias weights.
// every time a C_ version is overloaded, it is necessary to overload the B_
// version, where the new B_ just calls the new C_

class BP_API BpConSpec : public ConSpec {
  // Bp connection specifications
public:
  enum MomentumType {
    AFTER_LRATE,		// apply momentum after learning rate (old pdp style)
    BEFORE_LRATE,		// apply momentum before learning rate
    NORMALIZED 			// like BEFORE, but normalize direction to unit length
  };

  float 	lrate;		// learning rate
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  Schedule	lrate_sched;	// schedule of learning rate over training epochs (multiplies lrate!)
  float 	momentum;	// momentum
  MomentumType	momentum_type;	// #CONDEDIT_OFF_momentum:0 type of momentum function to use
  float		momentum_c;	// #READ_ONLY complement of momentum (for NORMALIZED)
  float 	decay;		// decay rate (before lrate and momentum)
  void 		(*decay_fun)(BpConSpec* spec, BpCon* cn, BpUnit* ru, BpUnit* su);
  // #LIST_BpConSpec_WtDecay #CONDEDIT_OFF_decay:0 the weight decay function to use

  void 		C_Init_dWt(Con_Group* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_dWt(cg, cn, ru, su); ((BpCon*)cn)->dEdW = 0.0f; }

  void 		C_Init_Weights(Con_Group* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_Weights(cg, cn, ru, su); ((BpCon*)cn)->dwt = 0.0f;}

  inline float		C_Compute_dEdA(BpCon* cn, BpUnit* ru, BpUnit* su);
  inline virtual float 	Compute_dEdA(BpCon_Group* cg, BpUnit* su);
  // get error from units I send to

  inline void 		C_Compute_dWt(BpCon* cn, BpUnit* ru, BpUnit* su);
  inline void 		Compute_dWt(Con_Group* cg, Unit* ru);
  inline virtual void	B_Compute_dWt(BpCon* cn, BpUnit* ru);
  // Compute dE with respect to the weights

  inline void 	C_Compute_WtDecay(BpCon* cn, BpUnit* ru, BpUnit* su);
  // call the decay function

  inline void	C_BEF_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su); // BEFORE_LRATE
  inline void	C_AFT_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su); // AFTER_LRATE
  inline void	C_NRM_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su); // NORMALIZED
  inline void	Compute_Weights(Con_Group* cg, Unit* ru);
  inline virtual void	B_Compute_Weights(BpCon* cn, BpUnit* ru);
  // for the bias unit

  virtual void	SetCurLrate(int epoch);
  // set current learning rate based on schedule given epoch

  // todo: this is no longer supported -- dwt is hard coded.  hmm.
  MemberDef* DMem_EpochShareDwtVar() { return min_con_type->members.FindName("dEdW"); }
  // name of weight-change variable to share across dmem processors in BATCH mode learning

  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(BpConSpec);
  COPY_FUNS(BpConSpec, ConSpec);
  TA_BASEFUNS(BpConSpec);
};

// the following functions are possible weight decay functions

// #REG_FUN
BP_API void Bp_Simple_WtDecay(BpConSpec* spec, BpCon* cn, BpUnit* ru, BpUnit* su)
// #LIST_BpConSpec_WtDecay Simple weight decay (subtract decay*wt)
     ;				// term here so scanner picks up comment
// #REG_FUN
BP_API void Bp_WtElim_WtDecay(BpConSpec* spec, BpCon* cn, BpUnit* ru, BpUnit* su)
// #LIST_BpConSpec_WtDecay Weight Elimination (Rumelhart) weight decay
     ;				// term here so scanner picks up comment

class BP_API BpCon_Group : public Con_Group {
  // group of Bp connections
public:
  // these are "convenience" functions for those defined in the spec

  void	SetCurLrate(int epoch) { ((BpConSpec*)spec.spec)->SetCurLrate(epoch); }
  float Compute_dEdA(BpUnit* su) { return ((BpConSpec*)spec.spec)->Compute_dEdA(this, su); }

  void	Initialize();
  void 	Destroy()		{ };
  TA_BASEFUNS(BpCon_Group);
};


class BpUnit;

class BP_API BpUnitSpec : public UnitSpec {
  // specifications for Bp units
public:
  SigmoidSpec	sig;		// sigmoid activation parameters
  float		err_tol;	// error tolerance (no error signal if |t-o|<err_tol)
  void 		(*err_fun)(BpUnitSpec* spec, BpUnit* u);
  // #LIST_BpUnit_Error this points to the error fun, set appropriately

  // generic unit functions
  void		Init_Acts(Unit* u);
  void 		Compute_Act(Unit* u);		// activation from net input (sigmoid)
  void 		Compute_dWt(Unit* u); 		// for all of my recv weights
  void 		Compute_Weights(Unit* u);		// modify to update bias weight

  // bp special functions
  virtual void 	Compute_Error(BpUnit* u); 	// call the error function (testing only)
  virtual void 	Compute_dEdA(BpUnit* u); 	// error wrt unit activation
  virtual void 	Compute_dEdNet(BpUnit* u); 	// error wrt net input

  virtual void	SetCurLrate(BpUnit* u, int epoch);
  // set current learning rate based on epoch

//TODO  virtual void	GraphActFun(GraphLog* graph_log, float min = -5.0, float max = 5.0);
  // #BUTTON #NULL_OK graph the activation function (NULL = new graph log)

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const BpUnitSpec& cp);
  COPY_FUNS(BpUnitSpec, UnitSpec);
  TA_BASEFUNS(BpUnitSpec);
};

// the following functions are possible error functions.

// #REG_FUN
BP_API void Bp_Squared_Error(BpUnitSpec* spec, BpUnit* u)
// #LIST_BpUnit_Error Squared error function for bp
     ;				// term here so scanner picks up comment
// #REG_FUN
BP_API void Bp_CrossEnt_Error(BpUnitSpec* spec, BpUnit* u)
// #LIST_BpUnit_Error Cross entropy error function for bp
     ;				// term here so scanner picks up comment


class BP_API BpUnit : public Unit {
  // ##DMEM_SHARE_SETS_4 standard feed-forward Bp unit
public:
  float 	err; 		// this is E, not dEdA
  float 	dEdA;		// #LABEL_dEdA #DMEM_SHARE_SET_3 error wrt activation
  float 	dEdNet;		// #LABEL_dEdNet error wrt net input

  // these are "convenience" functions for those defined in the spec
  void SetCurLrate(int epoch)   { ((BpUnitSpec*)spec.spec)->SetCurLrate(this, epoch); }
  void Compute_Error()		{ ((BpUnitSpec*)spec.spec)->Compute_Error(this); }
  void Compute_dEdA()		{ ((BpUnitSpec*)spec.spec)->Compute_dEdA(this); }
  void Compute_dEdNet()		{ ((BpUnitSpec*)spec.spec)->Compute_dEdNet(this); }
  void Compute_dEdA_dEdNet() 	{ Compute_dEdA(); Compute_Error(); Compute_dEdNet(); }

  void 	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const BpUnit& cp);
  COPY_FUNS(BpUnit, Unit);
  TA_BASEFUNS(BpUnit);
};

// inline functions (for speed)

inline float BpConSpec::C_Compute_dEdA(BpCon* cn, BpUnit* ru, BpUnit*) {
  return cn->wt * ru->dEdNet;
}
inline float BpConSpec::Compute_dEdA(BpCon_Group* cg, BpUnit* su) {
  float rval = 0.0f;
  CON_GROUP_LOOP(cg,rval += C_Compute_dEdA((BpCon*)cg->Cn(i), (BpUnit*)cg->Un(i), su));
  return rval;
}


inline void BpConSpec::C_Compute_dWt(BpCon* cn, BpUnit* ru, BpUnit* su) {
  cn->dEdW += su->act * ru->dEdNet;
}
inline void BpConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg,C_Compute_dWt((BpCon*)cg->Cn(i), (BpUnit*)ru, (BpUnit*)cg->Un(i)));
}
inline void BpConSpec::B_Compute_dWt(BpCon* cn, BpUnit* ru) {
  cn->dEdW += ru->dEdNet;
}

inline void BpConSpec::C_Compute_WtDecay(BpCon* cn, BpUnit* ru, BpUnit* su) {
  if(decay_fun != NULL)
    (*decay_fun)(this, cn, ru, su);
}
inline void BpConSpec::C_AFT_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su) {
  C_Compute_WtDecay(cn, ru, su); // decay goes into dEdW...
  cn->dwt = cur_lrate * cn->dEdW + momentum * cn->dwt;
  cn->wt += cn->dwt;
  cn->dEdW = 0.0f;
}
inline void BpConSpec::C_BEF_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su) {
  C_Compute_WtDecay(cn, ru, su);
  cn->dwt = cn->dEdW + momentum * cn->dwt;
  cn->wt += cur_lrate * cn->dwt;
  cn->dEdW = 0.0f;
}
inline void BpConSpec::C_NRM_Compute_Weights(BpCon* cn, BpUnit* ru, BpUnit* su) {
  C_Compute_WtDecay(cn, ru, su);
  cn->dwt = momentum_c * cn->dEdW + momentum * cn->dwt;
  cn->wt += cur_lrate * cn->dwt;
  cn->dEdW = 0.0f;
}

inline void BpConSpec::Compute_Weights(Con_Group* cg, Unit* ru) {
  if(momentum_type == AFTER_LRATE) {
    CON_GROUP_LOOP(cg, C_AFT_Compute_Weights((BpCon*)cg->Cn(i),
					   (BpUnit*)ru, (BpUnit*)cg->Un(i)));
  }
  else if(momentum_type == BEFORE_LRATE) {
    CON_GROUP_LOOP(cg, C_BEF_Compute_Weights((BpCon*)cg->Cn(i),
					   (BpUnit*)ru, (BpUnit*)cg->Un(i)));
  }
  else {
    CON_GROUP_LOOP(cg, C_NRM_Compute_Weights((BpCon*)cg->Cn(i),
					   (BpUnit*)ru, (BpUnit*)cg->Un(i)));
  }
  ApplyLimits(cg, ru);
}

inline void BpConSpec::B_Compute_Weights(BpCon* cn, BpUnit* ru) {
  if(momentum_type == AFTER_LRATE)
    C_AFT_Compute_Weights(cn, ru, NULL);
  else if(momentum_type == BEFORE_LRATE)
    C_BEF_Compute_Weights(cn, ru, NULL);
  else
    C_NRM_Compute_Weights(cn, ru, NULL);
  C_ApplyLimits(cn, ru, NULL);
}

//////////////////////////////////////////
//	Additional ConSpec Types	//
//////////////////////////////////////////

class BP_API HebbBpConSpec : public BpConSpec {
  // Simple Hebbian wt update (send act * recv act)
public:
  inline void 		C_Compute_dWt(BpCon* cn, BpUnit* ru, BpUnit* su);
  inline void 		Compute_dWt(Con_Group* cg, Unit* ru);

  inline void		B_Compute_dWt(BpCon* cn, BpUnit* ru);

  void	Initialize()		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(HebbBpConSpec);
};

inline void HebbBpConSpec::C_Compute_dWt(BpCon* cn, BpUnit* ru, BpUnit* su) {
  cn->dEdW += ((ru->ext_flag & Unit::TARG) ? ru->targ : ru->act) * su->act;
}

inline void HebbBpConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg,C_Compute_dWt((BpCon*)cg->Cn(i), (BpUnit*)ru, (BpUnit*)cg->Un(i)));
}

inline void HebbBpConSpec::B_Compute_dWt(BpCon* cn, BpUnit* ru) {
  cn->dEdW +=  ((ru->ext_flag & Unit::TARG) ? ru->targ : ru->act);
}

class BP_API ErrScaleBpConSpec : public BpConSpec {
  // con spec that scales the error by given parameter
public:
  float		err_scale;	// the scaling parameter

  inline float 		C_Compute_dEdA(BpCon* cn, BpUnit* ru, BpUnit* su);
  inline float 		Compute_dEdA(BpCon_Group* cg, BpUnit* su);

  void 	Initialize()	{ err_scale = 1.0f; }
  void 	Destroy()	{ };
  SIMPLE_COPY(ErrScaleBpConSpec);
  COPY_FUNS(ErrScaleBpConSpec, BpConSpec);
  TA_BASEFUNS(ErrScaleBpConSpec);
};

inline float ErrScaleBpConSpec::C_Compute_dEdA(BpCon* cn, BpUnit* ru, BpUnit*) {
  return err_scale * cn->wt * ru->dEdNet;
}
inline float ErrScaleBpConSpec::Compute_dEdA(BpCon_Group* cg, BpUnit* su) {
  float rval = 0.0f;
  CON_GROUP_LOOP(cg,rval += C_Compute_dEdA((BpCon*)cg->Cn(i), (BpUnit*)cg->Un(i), su));
  return rval;
}

class BP_API DeltaBarDeltaBpCon : public BpCon {
  // delta-bar-delta connection object with local learning rate
public:
  float 		lrate; 		// #NO_SAVE local learning rate

  void 	Initialize()		{ lrate = 0.0f; }
  void 	Destroy()		{ };
  void	Copy_(const DeltaBarDeltaBpCon& cp)	{ lrate = cp.lrate; }
  COPY_FUNS(DeltaBarDeltaBpCon, BpCon);
  TA_BASEFUNS(DeltaBarDeltaBpCon);
};

class BP_API DeltaBarDeltaBpConSpec : public BpConSpec {
  // delta-bar-delta has local learning rates for each connection
public:
  float		lrate_incr;	// rate of learning rate increase (additive)
  float		lrate_decr;	// rate of learning rate decrease (multiplicative)
  float		act_lrate_incr;	// #HIDDEN actual lrate increase (times lrate)

  void 		C_Init_Weights(Con_Group* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_Weights(cg, cn, ru, su); ((DeltaBarDeltaBpCon*)cn)->lrate = lrate;}
  // set initial learning rate

  inline void	C_UpdateLrate(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su);
  inline void	C_BEF_Compute_Weights(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su);
  inline void	C_AFT_Compute_Weights(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su);
  inline void	C_NRM_Compute_Weights(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su);
  inline virtual void	Compute_Weights(Con_Group* cg, Unit* ru);
  inline virtual void	B_Compute_Weights(BpCon* cn, BpUnit* ru);

  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const DeltaBarDeltaBpConSpec& cp);
  COPY_FUNS(DeltaBarDeltaBpConSpec, BpConSpec);
  TA_BASEFUNS(DeltaBarDeltaBpConSpec);
};

inline void DeltaBarDeltaBpConSpec::C_UpdateLrate
(DeltaBarDeltaBpCon* cn, BpUnit*, BpUnit*)
{
  float prod = cn->dwt * cn->dEdW;
  if(prod > 0.0f)
    cn->lrate += act_lrate_incr;
  else if(prod < 0.0f)
    cn->lrate *= lrate_decr;
  // prod = 0 means first epoch, don't change lrate..
}

inline void DeltaBarDeltaBpConSpec::C_AFT_Compute_Weights
(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su)
{
  C_Compute_WtDecay(cn, ru, su); // decay goes into dEdW...
  C_UpdateLrate(cn, ru, su);
  cn->dwt = cn->lrate * cn->dEdW + momentum * cn->dwt;
  cn->wt += cn->dwt;
  cn->dEdW = 0.0f;
}
inline void DeltaBarDeltaBpConSpec::C_BEF_Compute_Weights
(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su)
{
  C_Compute_WtDecay(cn, ru, su);
  C_UpdateLrate(cn, ru, su);
  cn->dwt = cn->dEdW + momentum * cn->dwt;
  cn->wt += cn->lrate * cn->dwt;
  cn->dEdW = 0.0f;
}
inline void DeltaBarDeltaBpConSpec::C_NRM_Compute_Weights
(DeltaBarDeltaBpCon* cn, BpUnit* ru, BpUnit* su)
{
  C_Compute_WtDecay(cn, ru, su);
  C_UpdateLrate(cn, ru, su);
  cn->dwt = momentum_c * cn->dEdW + momentum * cn->dwt;
  cn->wt += cn->lrate * cn->dwt;
  cn->dEdW = 0.0f;
}

inline void DeltaBarDeltaBpConSpec::Compute_Weights(Con_Group* cg, Unit* ru) {
  if(momentum_type == AFTER_LRATE) {
    CON_GROUP_LOOP(cg, C_AFT_Compute_Weights((DeltaBarDeltaBpCon*)cg->Cn(i),
					   (BpUnit*)ru, (BpUnit*)cg->Un(i)));
  }
  else if(momentum_type == BEFORE_LRATE) {
    CON_GROUP_LOOP(cg, C_BEF_Compute_Weights((DeltaBarDeltaBpCon*)cg->Cn(i),
					   (BpUnit*)ru, (BpUnit*)cg->Un(i)));
  }
  else {
    CON_GROUP_LOOP(cg, C_NRM_Compute_Weights((DeltaBarDeltaBpCon*)cg->Cn(i),
					   (BpUnit*)ru, (BpUnit*)cg->Un(i)));
  }
  ApplyLimits(cg, ru);
}

inline void DeltaBarDeltaBpConSpec::B_Compute_Weights(BpCon* cn, BpUnit* ru) {
  if(momentum_type == AFTER_LRATE)
    C_AFT_Compute_Weights((DeltaBarDeltaBpCon*)cn, ru, NULL);
  else if(momentum_type == BEFORE_LRATE)
    C_BEF_Compute_Weights((DeltaBarDeltaBpCon*)cn, ru, NULL);
  else
    C_NRM_Compute_Weights((DeltaBarDeltaBpCon*)cn, ru, NULL);
  C_ApplyLimits(cn, ru, NULL);
}


//////////////////////////////////////////
//	Additional Unit Types		//
//////////////////////////////////////////

class BP_API BpContextSpec : public BpUnitSpec {
  // for context units in simple recurrent nets (SRN), expects one-to-one prjn from layer it copies, must be AFTER that layer in .layers
public:
  float		hysteresis;	 // hysteresis factor: (1-hyst)*new + hyst*old
  float		hysteresis_c;	 // #READ_ONLY complement of hysteresis
  Random	initial_act;	 // initial activation value
  String	variable;	 // name of unit variable to copy into
  Unit::ExtType	unit_flags;	 // flags to set on the unit after copying value
  MemberDef*	var_md;		 // #IGNORE memberdef of variable

  void		Init_Acts(Unit* u);
  void 		Compute_Act(Unit* u);
  // copy activation from corresponding unit in projection from layer

  // nullify all other functions..
  void 		Compute_Netin(Unit*) 	{ };
  void 		Init_dWt(Unit*) 	{ };
  void 		Compute_dWt(Unit*) 	{ };
  void 		Compute_Weights(Unit*) 	{ };

  // bp special functions
  void 	Compute_Error(BpUnit*)		{ };
  void 	Compute_dEdA(BpUnit*)		{ };
  void 	Compute_dEdNet(BpUnit*)		{ }; //

//obs  bool  CheckConfig(Unit* un, Layer* lay, TrialProcess* tp);

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const BpContextSpec& cp);
  COPY_FUNS(BpContextSpec, BpUnitSpec);
  TA_BASEFUNS(BpContextSpec);
};

class BP_API LinearBpUnitSpec : public BpUnitSpec {
  // linear unit in Bp
public:
  void 		Compute_Act(Unit* u);
  void		Compute_dEdNet(BpUnit* u);

  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ };
  TA_BASEFUNS(LinearBpUnitSpec);
};

class BP_API ThreshLinBpUnitSpec : public BpUnitSpec {
  // thresholded linear unit in Bp
public:
  float		threshold;

  void 		Compute_Act(Unit* u);
  void		Compute_dEdNet(BpUnit* u);

  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ };
  SIMPLE_COPY(ThreshLinBpUnitSpec);
  COPY_FUNS(ThreshLinBpUnitSpec, BpUnitSpec);
  TA_BASEFUNS(ThreshLinBpUnitSpec);
};

class BP_API NoisyBpUnitSpec : public BpUnitSpec {
  // Bp with noisy output signal (act plus noise)
public:
  Random	noise;		// what kind of noise to add to activations

  void 		Compute_Act(Unit* u);

  void	Initialize();
  void 	Destroy()		{ };
  void 	InitLinks();
  SIMPLE_COPY(NoisyBpUnitSpec);
  COPY_FUNS(NoisyBpUnitSpec, BpUnitSpec);
  TA_BASEFUNS(NoisyBpUnitSpec);
};

class BP_API StochasticBpUnitSpec : public BpUnitSpec {
  // Bp with a binary stochastic activation function
public:
  void				Compute_Act(Unit* u);
  void	Initialize()		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(StochasticBpUnitSpec);
};

class BP_API RBFBpUnitSpec : public BpUnitSpec {
  // Radial basis function (Gaussian) function units in Bp
public:
  float         var;            // variance of Gaussian
  float         norm_const;     // #HIDDEN normalization const for Gaussian
  float         denom_const;    // #HIDDEN denominator const for Gaussian

  void          Compute_Netin(Unit* u);
  void          Compute_Act(Unit* u);
  void          Compute_dEdNet(BpUnit* u);

  void  UpdateAfterEdit();
  void  Initialize();
  void  Destroy()               { };
  SIMPLE_COPY(RBFBpUnitSpec);
  COPY_FUNS(RBFBpUnitSpec, BpUnitSpec);
  TA_BASEFUNS(RBFBpUnitSpec);
};

class BP_API BumpBpUnitSpec : public BpUnitSpec {
  // bump function in Bp: Gaussian of std net input
public:
  float         mean;           // mean of Gaussian
  float         std_dev;        // std deviation of Gaussian
  float         std_dev_r;      // #HIDDEN reciprocal of std_dev

  void          Compute_Act(Unit* u);
  void          Compute_dEdNet(BpUnit* u);

  void  UpdateAfterEdit();
  void  Initialize();
  void  Destroy()               { };
  SIMPLE_COPY(BumpBpUnitSpec);
  COPY_FUNS(BumpBpUnitSpec, BpUnitSpec);
  TA_BASEFUNS(BumpBpUnitSpec);
};

class BP_API ExpBpUnitSpec : public BpUnitSpec {
  // exponential units in Bp: simple exponent of net input
public:
  void          Compute_Act(Unit* u);
  virtual void  Compute_dEdNet(BpUnit* u);      // error wrt unit activation

  void  Initialize()	{ };
  void  Destroy()	{ };
  TA_BASEFUNS(ExpBpUnitSpec);
};

class BP_API SoftMaxBpUnitSpec : public BpUnitSpec {
  /* SoftMax Units: first one-to-one prjn is from corresp exponential unit,
     second prjn is from single summing linear unit, this then divides two */
public:

  void		Compute_Netin(Unit*)	{ }; // do nothing
  void          Compute_Act(Unit* u);
  void          Compute_dEdNet(BpUnit* u);

  // don't update my weights
  void          Compute_dWt(Unit*)	{ };
  void          Compute_Weights(Unit*)	{ };

  void  Initialize()	{ };
  void  Destroy()	{ };
  TA_BASEFUNS(SoftMaxBpUnitSpec);
};

class BP_API BpLayer : public Layer {
  // A feedforward backpropagation layer
public:

  void	Initialize();
  void 	Destroy()		{ };
  TA_BASEFUNS(BpLayer);
};

class PDP_API BpNetwork : public Network {
  // project for feedforward backpropagation networks (recurrent backprop is in RBPNetwork)
INHERITED(Network)
public:
  bool			bp_to_inputs;	// #DEF_false backpropagate errors to input layers (faster if not done, which is the default)

  virtual void		SetCurLrate(); // set current learning rate, based on network epoch counter
  override void 	Compute_Act(); // compute activations, based on external input data that has already been applied
  virtual void		Compute_dEdA_dEdNet(); // compute derivatives of error with respect to activations & net inputs (backpropagate)
  virtual void		Compute_Error(); // compute local error values, for display purposes only (only call when testing, not training)
  
  virtual void		Trial_Run(); // run one trial of Bp: calls SetCurLrate, Compute_Act and Compute_dEdA_dEdNet.  If you want to save some speed just for testing, you can just call Compute_Act and skip the other two (esp Compute_dEdA_dEdNet, which does a full backprop and is expensive, but often useful for visualization & testing)
  
  override void	SetProjectionDefaultTypes(Projection* prjn);

  void	Initialize();
  void 	Destroy()		{}
  TA_BASEFUNS(BpNetwork);
};

class PDP_API BpProject : public ProjectBase {
  // project for backpropagation networks
INHERITED(ProjectBase)
public:

  void	Initialize();
  void 	Destroy()		{}
  TA_BASEFUNS(BpProject);
};



#endif // bp.h
