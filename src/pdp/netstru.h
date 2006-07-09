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



// netstru.h

#ifndef netstru_h
#define netstru_h

#include "tarandom.h"
#include "ta_script.h"
#include "datatable.h"

#include "spec.h"
#include "aggregate.h"

#include "pdpbase.h"

#ifdef TA_GUI
#include "ta_qtbrowse_def.h"
//#include "fontspec.h"
#endif

#include <math.h>

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

// forwards this file
class Unit_Group; //

// on functions in the spec:
// only those functions that relate to the computational processing done by
// a given object should be defined in the spec, while the "structural" or
// other "administrative" functions should be defined in the object itself
// so that the spec can be invariant with respect to these kinds of differences
// with the idea being that the spec defines _functional_ aspects while
// the object defines various implementational aspects of an object's function
// (i.e. how the connections are arranged, etc..)

// note that with the projection spec, its job is to define connectivity, so
// it does have structural functions in the spec..


const float SIGMOID_MAX_VAL = 0.999999f; // max eval value
const float SIGMOID_MIN_VAL = 0.000001f; // min eval value
const float SIGMOID_MAX_NET = 13.81551f;	// maximium net input value

class PDP_API SigmoidSpec : public taBase {
// ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER Specifies a Sigmoid 1 / [1 + exp(-(x - off) * gain)]
public:
  float		off;		// offset for .5 point
  float		gain;		// gain

  static float	Clip(float y)
  { y = MAX(y,SIGMOID_MIN_VAL); y = MIN(y,SIGMOID_MAX_VAL); return y; }
  static float	ClipNet(float x)
  { x = MAX(x,-SIGMOID_MAX_NET); x = MIN(x,SIGMOID_MAX_NET); return x; }
  float		Eval(float x)
  { return Clip(1.0f / (1.0f + expf(-ClipNet((x - off) * gain)))); }
  float		Deriv(float x)	{ x = Clip(x); return x * (1.0f - x) * gain; }
  float		Inverse(float y)	{ y=y+off; return logf(y / (1.0f - y)) / gain; }

  void 	Initialize()		{ off = 0.0f; gain = 1.0f; }
  void	Destroy()		{ };
  SIMPLE_COPY(SigmoidSpec);
  COPY_FUNS(SigmoidSpec, taBase);
  TA_BASEFUNS(SigmoidSpec);
};


class PDP_API SchedItem : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER one element of a schedule
public:
  int		start_ctr;	// ctr number to start at for this item
  float		start_val;	// starting value for item
  int		duration;	// #HIDDEN duration (from start_ctr)
  float		step;		// #HIDDEN linear step to take for each increment of ctr

  float		GetVal(int ctr)  { return start_val + step * (float)(ctr - start_ctr); }
  // get value for given ctr value

  void	Initialize();
  void	Destroy() 	{ };
  void 	Copy_(const SchedItem& cp);
  COPY_FUNS(SchedItem, taOBase);
  TA_BASEFUNS(SchedItem);
};

class PDP_API Schedule : public taList<SchedItem> {
  // A schedule for parameters that vary over time
public:
  int 		last_ctr;	// the last counter index called
  float		default_val;	// the default if group is empty
  bool		interpolate;	// use linear interpolation between points
  float		cur_val;	// #READ_ONLY the current val

  float		GetVal(int ctr);
  // #MENU #MENU_ON_Edit #USE_RVAL get current schedule val, based on counter

  void 	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  void	Copy_(const Schedule& cp);
  COPY_FUNS(Schedule, taList<SchedItem>);
  TA_BASEFUNS(Schedule);
};


// the connection is managed fully by the ConSpec and the Con_Group
// don't put any functions on the connection itself

class PDP_API Connection : public taBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER Generic Connections
public:
  float 	wt;		// weight of connection

  bool		ChangeMyType(TypeDef* new_type);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_Network); }
#endif
  void 	SetTypeDefaults()	{ }; // overload this to do nothing (faster)
  void	UpdateAfterEdit();	     // might want to override any default updates..
  void 	Initialize() 		{ wt = 0.0f; }
  void 	Destroy()		{ };
  void	Copy_(const Connection& cp)	{ wt = cp.wt; }
  COPY_FUNS(Connection, taBase);
  TA_BASEFUNS(Connection);
};

SmartRef_Of(Connection); // ConnectionSRef

// the ConSpec has 2 versions of every function: one to go through the group
// and the other to apply to a single connection.
// The C_ denotes the con one which is non-virtual so that it is inlined
// this requires the group level one to be re-defined whenever the con
// level one is.  This is necessary given the speed penalty for a virtual
// function call at the connection level.

// The exception to the non-virtual rule is the C_Init functions!

// The following macro makes this process easier:

#define	CON_GROUP_LOOP(cg, expr) \
  for(int i=0; i<cg->size; i++) \
    expr

class PDP_API WeightLimits : public taBase {
  // ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER specifies weight limits for connections
public:
  enum LimitType {
    NONE,			// no weight limitations
    GT_MIN,			// constrain weights to be greater than min value
    LT_MAX,			// constrain weights to be less than max value
    MIN_MAX 			// constrain weights to be within min and max values
  };
  LimitType	type;		// type of weight limitation to impose
  float		min;		// #CONDEDIT_OFF_type:NONE,LT_MAX minimum weight value (if applicable)
  float		max;		// #CONDEDIT_OFF_type:NONE,GT_MIN maximum weight value (if applicable)
  bool		sym;		// if true, also symmetrize with reciprocal connections

  void 	ApplyMinLimit(float& wt)	{ if(wt < min) wt = min; }
  void 	ApplyMaxLimit(float& wt)	{ if(wt > max) wt = max; }

  void	ApplyLimits(float& wt)
  { if(type == GT_MIN) 		ApplyMinLimit(wt);
    else if(type == LT_MAX)	ApplyMaxLimit(wt);
    else if(type == MIN_MAX)	{ ApplyMinLimit(wt); ApplyMaxLimit(wt); } }

  void 	Initialize()		{ type = NONE; min = -1.0f; max = 1.0f; sym = false; }
  void	Destroy()		{ };
  SIMPLE_COPY(WeightLimits);
  COPY_FUNS(WeightLimits, taBase);
  TA_BASEFUNS(WeightLimits);
};

class PDP_API ConSpec: public BaseSpec {
  // Connection Group Specs: for processing over connections
public:
  TypeDef*	min_con_type;
  // #HIDDEN #NO_SAVE #TYPE_Connection mimimal con type required for spec (obj is con group)
  Random	rnd;		// Weight randomization specification
  WeightLimits	wt_limits;	// limits on weight sign, symmetry

  inline void		C_ApplyLimits(Connection* cn, Unit*, Unit*)
  { wt_limits.ApplyLimits(cn->wt); }
  inline virtual void	ApplyLimits(Con_Group* cg, Unit* ru);
  // apply weight limits (sign, magnitude)

  virtual void		ApplySymmetry(Con_Group* cg, Unit* ru);
  // apply weight symmetrizing between reciprocal units

  inline virtual void	C_InitWtState(Con_Group* cg, Connection* cn, Unit* ru, Unit* su);
  inline virtual void 	InitWtState(Con_Group* cg, Unit* ru);
  // initialize state variables (ie. at beginning of training)
  inline virtual void	C_InitWtState_post(Con_Group*, Connection*, Unit*, Unit*) { };
  // #IGNORE
  inline virtual void 	InitWtState_post(Con_Group* cg, Unit* ru);
  // #IGNORE post-initialize state variables (ie. for scaling symmetrical weights, etc)
  inline virtual void 	C_InitWtDelta(Con_Group*, Connection*, Unit*, Unit*)	{ };
  inline virtual void 	InitWtDelta(Con_Group* cg, Unit* ru);
  // initialize variables that change every delta-weight computation

  inline float 		C_Compute_Net(Connection* cn, Unit* ru, Unit* su);
  inline virtual float 	Compute_Net(Con_Group* cg, Unit* ru);
  // compute net input for weights in this con group
  inline void 		C_Send_Net(Connection* cn, Unit* ru, Unit* su);
  inline virtual void 	Send_Net(Con_Group* cg, Unit* su);
  // sender-based net input for con group (send net input to receivers)
  inline float 		C_Compute_Dist(Connection* cn, Unit* ru, Unit* su);
  inline virtual float 	Compute_Dist(Con_Group* cg, Unit* ru);
  // compute net distance for con group (ie. euclidean distance)
  inline void		C_Compute_dWt(Connection*, Unit*, Unit*)	{ };
  inline virtual void	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute the delta-weight change
  inline void 		C_UpdateWeights(Connection*, Unit*, Unit*)	{ };
  inline virtual void 	UpdateWeights(Con_Group* cg, Unit* ru);
  // update weights (ie. add delta-wt to wt, zero delta-wt)

  bool	CheckObjectType_impl(TAPtr obj); // don't do checking on 1st con group in units

  virtual bool  	CheckConfig(Con_Group*, Layer*, Unit*, Network*, bool =false) { return true; }
  // check for for misc configuration settings required by different algorithms

  virtual int		UseCount(); // return number of times this spec is used
  void	ReplacePointersHook(TAPtr old);

  virtual  void  InitWeights();
  // #BUTTON initializes weights for all projections with this conspec

  virtual bool	 DMem_AlwaysLocal() { return false; }
  // overload this function to prevent this projection from being pruned for non-local units under dmem processing (for "special" connection types)
  virtual MemberDef* DMem_EpochShareDwtVar() { return min_con_type->members.FindName("dwt"); }
  // name of weight-change variable to share across dmem processors in BATCH mode learning
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_ConSpec); }
#endif
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ConSpec& cp);
  COPY_FUNS(ConSpec, BaseSpec);
  TA_BASEFUNS(ConSpec);
};

SpecPtr_of(ConSpec);

// assumes no USE_TEMPLATE_GROUPS

class PDP_API Unit_List: public taList<Unit> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(Unit_List);
};

class PDP_API Con_Group: public taBase_Group {
  // ##NO_TOKENS ##NO_UPDATE_AFTER Group of connections, controlls processing over them
  // entire group must have same connection object type
public:
  // note: follwing must be coordinated with the Network enum
  enum WtSaveFormat {
    TEXT,			// weights are saved as ascii text representation of digits (completely portable)
    TEXT_IDX,			// text format plus unit indexes (useful for partially-connected networks where connections might change)
    BINARY,			// weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
    BINARY_IDX 			// binary format plus unit indexes (useful for partially-connected networks where connections might change)
  };

  Unit_List	units;
  // #NO_FIND #NO_SAVE pointers to units that correspond (by index) to cons
  ConSpec_SPtr 	spec;		// specification for connections
  Projection*	prjn;
  // pointer the the projection which created this Group
  int		other_idx;
  // #READ_ONLY index of other side of con group (for recv_gp = send_idx, send_gp = recv_idx)
  bool		own_cons;
  // #READ_ONLY true if this group "owns" the connections (must be receiver!)

  Connection* 	Cn(int i) const { return (Connection*)FastEl(i); }
  // gets the connection at the given index
  Unit*		Un(int i) const { return units.FastEl(i); }
  // gets the unit at the given index

  virtual void	Copy_Weights(const Con_Group* src);
  // copies weights from other con_group
  virtual void	WriteWeights(ostream& strm, Unit* ru, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_strm_wts #COMPRESS write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual void	ReadWeights(istream& strm, Unit* ru, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #EXT_strm_wts #COMPRESS read weight values in from a simple ordered list of weights (optionally in binary format)

  // overload some functions to manage both connections and units
  // leave most ops separate for flexibility
  virtual Connection*	NewCon(TypeDef* typ, Unit* un);
  // create a connection of given type to given unit
  virtual void		LinkCon(Connection* cn, Unit* un);
  // make a link connection from given connection, unit (for sending groups)
  virtual bool		RemoveCon(Unit* un);
  // remove connection from given unit
  virtual void		AllocCon(int no, TypeDef* typ);
  // allocate given number of new connections
  virtual Connection*	FindConFrom(Unit* un, int& idx=Idx) const;
  // #MENU #MENU_ON_Actions #USE_RVAL #ARGC_1 find connection from given unit

  void		Alloc(int sz);	// allocate both lists
  TAPtr		NewEl(int n_els=0, TypeDef* typ=NULL);
  TAPtr		New(int n_ojbs=0, TypeDef* typ=NULL);
  bool		Remove(const char* it)	{ return taBase_Group::Remove(it); }
  bool		Remove(TAPtr it)	{ return taBase_Group::Remove(it); }
  bool		Remove(int i);		// remove item

  // projection-related functions for operations on sub-groups of the group
  virtual Con_Group*	NewPrjn(Projection* prjn, bool own=false);
  // create a new sub_group from given projection, with given ownership (own_cons)
  virtual Con_Group*	FindPrjn(Projection* prjn, int& idx=Idx) const;
  // find sub group associated with given projection
  virtual Con_Group*	FindFrom(Layer* from, int& idx=Idx) const;
  // #MENU #USE_RVAL #ARGC_1 find sub group that receives from given layer
  virtual Con_Group*	FindTypeFrom(TypeDef* prjn_typ, Layer* from, int& idx=Idx) const;
  // #MENU #USE_RVAL #ARGC_2 find sub group that recvs prjn of given type from layer
  virtual Con_Group*	FindLayer(Layer* lay, int& idx=Idx) const;
  // find sub group where projection is in the given layer
  virtual bool		RemovePrjn(Projection* prjn);
  // remove sub group associated with given projection
  virtual bool		RemoveFrom(Layer* from);
  // #MENU remove sub group that receives from given layer

  virtual Connection* FindRecipRecvCon(Unit* su, Unit* ru);
  // find the reciprocal for sending unit su to this receiving unit ru
  virtual Connection* FindRecipSendCon(Unit* ru, Unit* su);
  // find the reciprocal for receiving unit ru from this sending unit su

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU add noise to weights using given noise specification
  virtual int	PruneCons(Unit* un, const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(Unit* un, float p_lesion, bool permute=true);
  // #MENU #USE_RVAL remove weights with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	CheckTypes();
  // #MENU #USE_RVAL check that the object and spec types are all ok
  virtual bool CheckOtherIdx_Recv(); // check validity of other_idx for recv con groups
  virtual bool CheckOtherIdx_Send();// check validity of other_idx for send con groups

  virtual void	ConValuesToArray(float_RArray& ary, const char* variable);
  // adds values of variable from the connections into the given array
  virtual void	ConValuesFromArray(float_RArray& ary, const char* variable);
  // sets values of variable in the connections from the given array

  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // switch any connections using old_sp to using new_sp
  virtual void	CopyNetwork(Network* net, Network* cn, Con_Group* cp); // #IGNORE copy entire network
  virtual void	CopyPtrs(Con_Group* cp); // #IGNORE copy the pointers directly!

  // these are convenience functions for those defined in the spec
  void 	InitWtState(Unit* ru)	 	{ spec->InitWtState(this,ru); }
  void 	C_InitWtState(Connection* cn, Unit* ru, Unit* su)
  { spec->C_InitWtState(this, cn, ru, su); }
  void	InitWtState_post(Unit* ru) 	{ spec->InitWtState_post(this,ru); } // #IGNORE
  void 	InitWtDelta(Unit* ru)	 	{ spec->InitWtDelta(this,ru); }

  float Compute_Net(Unit* ru)	 	{ return spec->Compute_Net(this,ru); }
  void 	Send_Net(Unit* su)		{ spec->Send_Net(this, su); }
  float Compute_Dist(Unit* ru)	 	{ return spec->Compute_Dist(this,ru); }
  void 	UpdateWeights(Unit* ru)	 	{ spec->UpdateWeights(this,ru); }
  void  Compute_dWt(Unit* ru)	 	{ spec->Compute_dWt(this,ru); }

  bool  CheckConfig(Layer* lay, Unit* ru, Network* net, bool quiet=false)
  { return spec->CheckConfig(this, lay, ru, net, quiet); }

  int 	Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  int	Dump_SaveR(ostream& strm, TAPtr par=NULL, int indent = 0);
  int	Dump_Save_PathR(ostream& strm, TAPtr par=NULL, int indent = 0);
  int	Dump_Load_Value(istream& strm, TAPtr par=NULL);

  bool	ChangeMyType(TypeDef* new_type);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_Network); }
#endif
  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const Con_Group& cp);
  void	Copy(const Con_Group& cp);
  TA_BASEFUNS(Con_Group);
};

class PDP_API UnitSpec: public BaseSpec { // Generic Unit Specification
#ifndef __MAKETA__
typedef BaseSpec inherited;
#endif
  static Con_Group*	rcg_rval; // return value for connecting
  static Con_Group*	scg_rval; // return value for connecting
public:
  MinMaxRange	act_range;		// range of activation for units
  TypeDef*	bias_con_type;
  // #TYPE_Connection #NULL_OK type of bias connection to make
  ConSpec_SPtr 	bias_spec;
  // con spec that controls the bias connection on the unit

  virtual void 	InitState(Unit* u);	// initialize unit state variables
  virtual void 	ModifyState(Unit*) { };// modify unit state variables
  virtual void 	InitWtDelta(Unit* u); 	// init weight delta variables
  virtual void 	InitWtState(Unit* u); 	// init weight state variables
  virtual void	InitWtState_post(Unit* u); // #IGNORE run after init wt state (ie. to scale wts)

  virtual void 	Compute_Net(Unit* u);
  virtual void 	Send_Net(Unit* u); // send net input to all units I send to
  virtual void 	Send_NetToLay(Unit* u, Layer* tolay); // send net input only to units in given layer (not used by default)
  virtual void 	Compute_Act(Unit* u);

  virtual void 	Compute_dWt(Unit* u); 	// compute change in weights
  virtual void 	UpdateWeights(Unit* u);	// update weights from deltas

  virtual void	BuildBiasCons();
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE build the bias connections according to specified type

  virtual bool  CheckConfig(Unit* un, Layer* lay, Network* net, bool quiet=false);
  // check for for misc configuration settings required by different algorithms

  virtual int	UseCount(); // return number of times this spec is used
  void	ReplacePointersHook(TAPtr old);

#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_UnitSpec); }
//  virtual Unit_So*	MakeSo(Unit* unit, Layer_So* par);
#endif

  void  UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const UnitSpec& cp);
  COPY_FUNS(UnitSpec, BaseSpec);
  TA_BASEFUNS(UnitSpec);
};

SpecPtr_of(UnitSpec);

class PDP_API Unit: public taNBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##DMEM_SHARE_SETS_3 Generic unit
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
protected:
  static Con_Group*	rcg_rval; // return value for connecting
  static Con_Group*	scg_rval; // return value for connecting
public: //
  enum ExtType {// #BITS indicates type of external input; some flags used in Layer to control usage
    DEFAULT 		= 0x00,	// #NO_BIT (Layer ctrl param only) set default layer flags based on parameter in data
    NO_EXTERNAL 	= 0x00,	// #NO_BIT no input
    TARG 		= 0x01,	// #LABEL_Target as a target value
    EXT 		= 0x02,	// #LABEL_External as an external input value
    COMP		= 0x04,	// #LABEL_Comparison as a comparison value
    TARG_EXT	 	= 0x03,	// #NO_BIT as both external input and target value
    COMP_TARG		= 0x05,	// #NO_BIT as a comparision and target layer
    COMP_EXT		= 0x06,	// #NO_BIT as a comparison and external input layer
    COMP_TARG_EXT	= 0x07,	// #NO_BIT as a comparison, target, and external input layer
    NO_UNIT_FLAGS	= 0x10, 
      // #LABEL_no_Unit_flags (Layer + Layer ctrl param) don't set unit flags at all
    NO_LAYER_FLAGS	= 0x20, 
      // #LABEL_no_Layer_flags (Layer ctrl param only) don't set layer flags at all
    EXT_FLAGS_MASK	= 0x0F, // #NO_BIT mask for setting layer/unit flags
    LAYER_FLAGS_MASK	= 0x1F // #NO_BIT mask for setting layer flags
  };

  UnitSpec_SPtr spec;		// unit specification
  ExtType	ext_flag;	// #READ_ONLY #SHOW tells what kind of external input unit received
  float 	targ;		// target pattern
  float 	ext;		// external input
  float 	act;		// #DMEM_SHARE_SET_2 activation
  float 	net;		// #DMEM_SHARE_SET_1 net input
  Con_Group 	recv;		// Receiving Connection Groups
  Con_Group 	send;		// Sending Connection Groups
  Connection*	bias;		// #OWN_POINTER bias weight (type set in unit spec)
  int		n_recv_cons;	// #DMEM_SHARE_SET_0 total number of receiving connections
#ifdef DMEM_COMPILE
  static int	dmem_this_proc;	// #IGNORE processor rank for this processor RELATIVE TO COMMUNICATOR for the network
  int 		dmem_local_proc; // #IGNORE processor on which these units are local
  virtual bool 	DMem_IsLocalProc(int proc)   	{ return dmem_local_proc == proc; } // #IGNORE
  virtual bool 	DMem_IsLocal()       		{ return dmem_local_proc == dmem_this_proc; }  // #IGNORE
  virtual int 	DMem_GetLocalProc() 		{ return dmem_local_proc; } // #IGNORE
  virtual void 	DMem_SetLocalProc(int lproc) 	{ dmem_local_proc = lproc; } // #IGNORE
  virtual void 	DMem_SetThisProc(int proc) 	{ dmem_this_proc = proc; } // #IGNORE
#endif
  TDCoord       pos;		// position in space relative to owning group, layer

  Unit_Group*	ugrp() {return GET_MY_OWNER(Unit_Group);}
  virtual void	Copy_Weights(const Unit* src, Projection* prjn = NULL);
  // copies weights from other unit (incl wts assoc with unit bias member)
  virtual void	WriteWeights(ostream& strm, Projection* prjn = NULL, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_strm_wts #COMPRESS write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual void	ReadWeights(istream& strm, Projection* prjn = NULL, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #EXT_strm_wts #COMPRESS read weight values in from a simple ordered list of weights (optionally in binary fmt)

  // #MENU #MENU_ON_Actions initialize unit external input variables
  void		SetExtFlag(ExtType flg) { ext_flag = (ExtType)(ext_flag | flg); }
  void		UnSetExtFlag(ExtType flg) { ext_flag = (ExtType)(ext_flag & ~flg); }

  virtual void 	InitExterns()	{ ext = targ = 0.0f; ext_flag = NO_EXTERNAL; }
  virtual void 	InitDelta()	{ net = 0.0f; }

  // these are convenience functions for those defined in the spec
  void 	InitState()		{ spec->InitState(this); }
  // #MENU initialize unit state variables
  void 	ModifyState()		{ spec->ModifyState(this); }
  // #MENU modify unit state variables (algorithm specific, e.g. decay)
  void 	InitWtDelta()		{ spec->InitWtDelta(this); }
  // #MENU Initialze weight changes
  void 	InitWtState()		{ spec->InitWtState(this); }
  // #MENU Initialize weight values
  void	InitWtState_post() 	{ spec->InitWtState_post(this); } // #IGNORE
  void 	Compute_Net()		{ spec->Compute_Net(this); }
  void 	Send_Net()		{ spec->Send_Net(this); }
  void 	Send_NetToLay(Layer* tolay)	{ spec->Send_NetToLay(this, tolay); }
  void 	Compute_Act()		{ spec->Compute_Act(this); }
  void 	UpdateWeights()		{ spec->UpdateWeights(this); }
  void 	Compute_dWt()		{ spec->Compute_dWt(this); }

  bool  CheckConfig(Layer* lay, Network* net, bool quiet=false)
  { return spec->CheckConfig(this, lay, net, quiet); }

  void 		ApplyValue(float val, ExtType act_ext_flags, Random* ran = NULL);
  // called by layer to apply input or target pattern
  virtual bool	Build();
  // build unit: make sure bias connection is created and right type
  virtual bool	CheckBuild();
  // check if build needs to be performed (true if yes, false if no)
  TAPtr		New(int n_objs=0, TypeDef* type=NULL);
  // create a bias weight of the specified type (for loading existing nets)
  virtual void	RemoveCons();
  // #IGNORE remove all of unit's sending and receiving connections
  // since this doesn't affect other units, it should not be called individually
  virtual void		ConnectAlloc(int no, Projection* prjn, Con_Group*& cgp = rcg_rval);
  // pre-allocate given no of connections (for better memory layout)
  virtual Connection* 	ConnectFrom(Unit* su, Projection* prjn, Con_Group*& recv_gp = rcg_rval,
				    Con_Group*& send_gp = scg_rval);
  // make a recv connection from given unit to this unit using given projection
  virtual Connection* 	ConnectFromLink(Unit* su, Projection* prjn, Connection* src_con,
					Con_Group*& recv_gp = rcg_rval,
					Con_Group*& send_gp = scg_rval);
  // make linked recv con from given unit to this using prjn and given src connection
  virtual Connection* 	ConnectFromCk(Unit* su, Projection* prjn, Con_Group*& recv_gp = rcg_rval,
				      Con_Group*& send_gp = scg_rval);
  // does ConnectFrom but checks for an existing connection to prevent double-connections!
  virtual Connection* 	ConnectFromLinkCk(Unit* su, Projection* prjn, Connection* src_con,
					  Con_Group*& recv_gp = rcg_rval,
					  Con_Group*& send_gp = scg_rval);
  // does ConnectFromLink but checks for an existing connection to prevent double-connections!
  virtual bool	DisConnectFrom(Unit* su, Projection* prjn=NULL);
  // remove connection from given unit (projection is optional)
  virtual void	DisConnectAll();
  // #MENU #MENU_ON_Actions disconnect unit from all other units
  virtual int	CountRecvCons();
  // count total number of receiving connections

  virtual void	TransformWeights(const SimpleMathSpec& trans, Projection* prjn = NULL);
  // #MENU #MENU_SEP_BEFORE apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec, Projection* prjn = NULL);
  // #MENU add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc, CountParam::Relation rel,
			     float cmp_val, Projection* prjn = NULL);
  // #MENU #USE_RVAL remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(float p_lesion, bool permute=true, Projection* prjn = NULL);
  // #MENU #USE_RVAL remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	SetConSpec(ConSpec* con_spec);
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE set all recv conspecs to con_spec
  virtual bool	CheckTypes();
  // #MENU #USE_RVAL check that the object and spec types are all ok

  virtual int	ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // switch any units/layers using old_sp to using new_sp
  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // switch any connections/projections using old_sp to using new_sp

  virtual void	CopyNetwork(Network* anet, Network* cn, Unit* cp); // #IGNORE copy network
  virtual void	CopyPtrs(Unit* cp); // #IGNORE copy the pointers directly
  void	ReplacePointersHook(TAPtr old);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_Network); }
#endif
  void  UpdateAfterEdit();
  void	Initialize();
  void 	Destroy();
  void  InitLinks();
  void	CutLinks();
  void	Copy_(const Unit& cp);
  COPY_FUNS(Unit, taNBase);
  TA_BASEFUNS(Unit);
};

// Projections are abrevieated prjn (as a oppesed to proj = project or proc = process)
// ProjectionSpec does the connectivity, and optionally the weight init

class PDP_API ProjectionSpec : public BaseSpec {
  // #VIRT_BASE Specifies the connectivity between layers (ie. full vs. partial)
public:
  bool		self_con;	// whether to create self-connections or not (if applicable)
  bool		init_wts;	// whether this projection spec does weight init (else conspec)

  virtual void 	RemoveCons(Projection* prjn);	// deletes any existing connections
  virtual void	PreConnect(Projection* prjn);	// Prepare to connect (init con_groups)
  virtual void	Connect_impl(Projection*) { };
  // actually implements specific connection code
  virtual void 	Connect(Projection* prjn);
  // connects the network, first removing existing cons, and inits weights
  virtual void 	ReConnect_Load(Projection* prjn);
  // #IGNORE re-connects the network after loading based on recv groups only
  virtual int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);
  // probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
  virtual void 	InitWtDelta(Projection* prjn);	// initializes the delta
  virtual void 	InitWtState(Projection* prjn);	// initializes the wt`s state
  virtual void 	InitWtState_post(Projection* prjn);
  // #IGNORE after wt init (ie. for wt. scaling)

  virtual void	C_InitWtState(Projection* prjn, Con_Group* cg, Unit* ru);
  // custom initialize weights in this con group for given receiving unit ru

  virtual void 	CopyNetwork(Network* net, Network* cn, Projection* prjn, Projection* cp);
  virtual int	UseCount(); // return number of times this spec is used
  void	ReplacePointersHook(TAPtr old);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_ProjectionSpec); }
#endif
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  SIMPLE_COPY(ProjectionSpec);
  COPY_FUNS(ProjectionSpec, BaseSpec);
  TA_BASEFUNS(ProjectionSpec);
};

SpecPtr_of(ProjectionSpec);

class PDP_API Projection: public taNBase {
  // Projection describes connectivity between layers (from receivers perspective)
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  enum PrjnSource {
    NEXT,		// Recv from the next layer in network
    PREV,		// Recv from the previous layer in network
    SELF,		// Recv from the same layer
    CUSTOM 		// Recv from the layer spec'd in the projection
  };

  Layer* 		layer;    	// #READ_ONLY #NO_SAVE layer this prjn is in
  PrjnSource 		from_type;	// Source of the projections
  Layer*		from;		// layer receiving from (set this for custom)
  ProjectionSpec_SPtr	spec;		// spec for this item
  TypeDef*		con_type;	// #TYPE_Connection Type of connection
  TypeDef*		con_gp_type;	// #TYPE_Con_Group Type of connection group
  ConSpec_SPtr 		con_spec;	// conspec to use for creating connections
  int			recv_idx;	// #READ_ONLY receiving con_group index
  int			send_idx;	// #READ_ONLY sending con_group index
  int			recv_n;		// #READ_ONLY number of receiving con_groups
  int			send_n;		// #READ_ONLY number of sending con_groups

  bool			projected; 	 // #HIDDEN t/f if connected
/*#ifdef TA_GUI // TODO: prob obsolete -- not really used anywhere, except 2 funcs below
  Xform*		proj_points;     // #OWN_POINTER #HIDDEN projection points

  void			SetFromPoints(float x1, float y1);
  void			SetToPoints(float x1, float y1);
#endif */
  virtual void 	SetFrom();	// set where to receive from based on selections

  virtual void	SetCustomFrom(Layer* from_lay);
  // set a CUSTOM projection from given layer (if from_lay == layer, turns into SELF)

  virtual void	Copy_Weights(const Projection* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE copies weights from other projection
  virtual void	WriteWeights(ostream& strm, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #EXT_strm_wts #COMPRESS write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual void	ReadWeights(istream& strm, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #EXT_strm_wts #COMPRESS read weight values in from a simple ordered list of weights (optionally in binary fmt)

  // convenience functions for those defined in the spec
  void 	RemoveCons()		{ spec->RemoveCons(this); }
  // #MENU #MENU_ON_Actions #CONFIRM Reset all connections for this projection
  void 	PreConnect()		{ spec->PreConnect(this); }
  void 	Connect()		{ spec->Connect(this); }
  // #BUTTON #CONFIRM Make all connections for this projection (resets first)
  void 	Connect_impl()		{ spec->Connect_impl(this); }
  void 	ReConnect_Load();	// #IGNORE
  int 	ProbAddCons(float p_add_con, float init_wt = 0.0) { return spec->ProbAddCons(this, p_add_con, init_wt); }
  // #MENU #USE_RVAL probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
  void 	CopyNetwork(Network* net, Network* cn, Projection* cp) { spec->CopyNetwork(net,cn,this,cp); } // #IGNORE
  void 	InitWtDelta()		{ spec->InitWtDelta(this); }
  // #MENU #MENU_SEP_BEFORE Initialize weight changes for this projection
  void 	InitWtState()		{ spec->InitWtState(this); }
  // #BUTTON #CONFIRM Initialize weight state for this projection
  void 	InitWtState_post() 	{ spec->InitWtState_post(this); } // #IGNORE

  void 	C_InitWtState(Con_Group* cg, Unit* ru)  { spec->C_InitWtState(this, cg, ru); }
  // custom initialize weights in this con group for given receiving unit ru

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual bool 	SetConSpec(ConSpec* sp);
  // set the con spec for all connections in this prjn
  virtual bool 	ApplyConSpec();
  // #BUTTON apply the default conspec to all connections in this prjn
  virtual bool	CheckTypes();
  // #BUTTON #USE_RVAL check that the existing con and con gp types are of the specified types
  virtual void	FixIndexes();
  // #MENU fix the indicies of the connection groups (other_idx)
  virtual bool	CheckTypes_impl(); // #IGNORE

  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // switch any connections/projections using old_sp to using new_sp
  virtual int	ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // switch any projections using old_sp to using new_sp

  virtual bool 	SetConType(TypeDef* td);
  // set the connection type for all connections in this prjn
  virtual bool 	SetConGpType(TypeDef* td);
  // set the connection group type for all connections in this prjn

  virtual void	GridViewWeights(GridLog* grid_log, bool use_swt=false, int un_x=-1, int un_y=-1, int wt_x=-1, int wt_y=-1);
  /* #MENU #MENU_SEP_BEFORE #NULL_OK display entire set of projection weights (use sending weights if use_swt) in grid log, -1 for x,y = use layer geometry
     (otherwise limits range, un= unit range, wt = weight (sending lay) range) */
  virtual void	WeightsToTable(DataTable* dt);
  // #MENU #NULL_OK TODO:define send entire set of projection weights to given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit

  void 	CopyPtrs(Projection* cp); // #IGNORE copy the pointers
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_Network); }
#endif
  void 	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Projection& cp);
  COPY_FUNS(Projection, taNBase);
  TA_BASEFUNS(Projection);
#ifdef TA_GUI
protected:
//  override taiDataLink*	ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type);
#endif
};

BaseGroup_of(Projection);

//////////////////////////////////////////////////////////
// 	Inline Connection-level functions (fast)	//
//////////////////////////////////////////////////////////

inline void ConSpec::ApplyLimits(Con_Group* cg, Unit* ru) {
  if(wt_limits.type != WeightLimits::NONE) {
    CON_GROUP_LOOP(cg, C_ApplyLimits(cg->Cn(i), ru, cg->Un(i)));
  }
}

inline void ConSpec::C_InitWtState(Con_Group*, Connection* cn, Unit* ru, Unit* su) {
  if(rnd.type != Random::NONE)	{ // don't do anything (e.g. so connect fun can do it)
    cn->wt = rnd.Gen();
  }
  else {
    Random::ZeroOne();		// keep random seeds syncronized for dmem
  }
  C_ApplyLimits(cn,ru,su);
}

inline void ConSpec::InitWtState(Con_Group* cg, Unit* ru) {
  if(cg->prjn->spec->init_wts) {
       cg->prjn->C_InitWtState(cg, ru);
  } else {
    CON_GROUP_LOOP(cg, C_InitWtState(cg, cg->Cn(i), ru, cg->Un(i)));
  }

  InitWtDelta(cg,ru);
  ApplySymmetry(cg,ru);
}

inline void ConSpec::InitWtState_post(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_InitWtState_post(cg, cg->Cn(i), ru, cg->Un(i)));
}

inline void ConSpec::InitWtDelta(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_InitWtDelta(cg, cg->Cn(i), ru, cg->Un(i)));
}

inline float ConSpec::C_Compute_Net(Connection* cn, Unit*, Unit* su) {
  return cn->wt * su->act;
}
inline float ConSpec::Compute_Net(Con_Group* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Net(cg->Cn(i), ru, cg->Un(i)));
  return rval;
}

inline void ConSpec::C_Send_Net(Connection* cn, Unit* ru, Unit* su) {
  ru->net += cn->wt * su->act;
}
inline void ConSpec::Send_Net(Con_Group* cg, Unit* su) {
CON_GROUP_LOOP(cg, C_Send_Net(cg->Cn(i),
		 cg->Un(i), su));
}

inline float ConSpec::C_Compute_Dist(Connection* cn, Unit*, Unit* su) {
  float tmp = su->act - cn->wt;
  return tmp * tmp;
}
inline float ConSpec::Compute_Dist(Con_Group* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Dist(cg->Cn(i), ru, cg->Un(i)));
  return rval;
}

inline void ConSpec::UpdateWeights(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_UpdateWeights(cg->Cn(i), ru, cg->Un(i)));
  ApplyLimits(cg,ru); // ApplySymmetry(cg,ru);  don't apply symmetry during learning..
}

inline void ConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt(cg->Cn(i), ru, cg->Un(i)));
}


class PDP_API Unit_Group: public taGroup<Unit> {
  // #NO_UPDATE_AFTER a group of units
INHERITED(taGroup<Unit>)
public:
  Layer*	own_lay;	// #READ_ONLY #NO_SAVE layer owner
  int		n_units;	// number of units to create in the group (0 = use layer n_units)
  PosTDCoord	pos;		// position of group relative to the layer
  PosTDCoord	geom;		// geometry of the group
  bool		units_lesioned;	// #READ_ONLY if units were lesioned in this group, don't complain about rebuilding!

  virtual void	Copy_Weights(const Unit_Group* src);
  // #MENU #MENU_ON_Object copies weights from other unit group (incl wts assoc with unit bias member)
  virtual void	WriteWeights(ostream& strm, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #EXT_strm_wts #COMPRESS write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual void	ReadWeights(istream& strm, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #EXT_strm_wts #COMPRESS read weight values in from a simple ordered list of weights (optionally in binary fmt)

  virtual bool	Build();
  // #MENU #MENU_ON_Actions for subgroups: build units to specs (true if changed)
  virtual bool	CheckBuild();
  // check if build needs to be performed (true if yes, false if no)
  virtual void	LayoutUnits(Unit* u = NULL);
  // for subgroups: redistribute units within the given geometry of the group
  virtual void	RecomputeGeometry();
  // re compute geometry based on parent layer

  virtual bool	SetUnitSpec(UnitSpec* unitspec);
  // #MENU #MENU_SEP_BEFORE set for all units in group
  virtual bool	SetConSpec(ConSpec* conspec);
  // #MENU set for all unit's connections in group

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL remove connections with prob p_lesion (permute = fixed no. lesioned)
  virtual int	LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL remove units with prob p_lesion (permute = fixed no. lesioned)

  virtual void	UnitValuesToArray(float_RArray& ary, const char* variable);
  // adds values of variable from the units into the given array
  virtual void	UnitValuesFromArray(float_RArray& ary, const char* variable);
  // sets unit values from values in the given array

  Unit*		FindUnitFmCoord(int x, int y);
  Unit* 	FindUnitFmCoord(const TwoDCoord& coord) {return FindUnitFmCoord(coord.x,coord.y);}
  // returns unit at given coordinates within layer

  void		RemoveAll();
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_Network); }
#endif
  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void  Copy_(const Unit_Group& cp);
  COPY_FUNS(Unit_Group, taGroup<Unit>);
  TA_BASEFUNS(Unit_Group);
#ifdef TA_GUI
protected:
//  override taiDataLink*	ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type);
#endif
};

class PDP_API LayerSpec : public BaseSpec {
  // generic layer specification
public:
  virtual int	UseCount(); // return number of times this spec is used
  void	ReplacePointersHook(TAPtr old);

#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_LayerSpec); }
//  virtual Layer_So*	MakeSo(Layer* layer, Network_So* par);
#endif

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(LayerSpec); //
};

/* 

  Layer SourceData
  act -- NxM float array, of the activation values
  
*/
class PDP_API Layer: public taNBase {
  // ##EXT_lay ##COMPRESS layer containing units
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  enum DMemDist {
    DMEM_DIST_DEFAULT,		// distribute units to different processors for distributed memory processing according to the default sequential scheme
    DMEM_DIST_UNITGP		// distribute units according to unit groups, which can be less even but allows for shared weights by unit group
  }; //
  
  
  //NOTE: LayerType bits are same as LayerWriter LayerFlags bits
  enum LayerType { // #BITS design hint on layer usage (but input/output always allowed)
    HIDDEN	= 0x00, 	// #NO_BIT layer probably won't be externally connected
    INPUT 	= 0x01,		// layer will (or may) receive external input
    OUTPUT 	= 0x02,		// layer will (or may) provide external output
    TARGET 	= 0x04		// layer will (or may) be supplied with target (training) data
  }; //
  
  int			numUnits() const; // count of N units, regardless of geom
  bool			isSparse() const; // true if N units doesn't fit evenly into geom or 4-d geom
  
  Network*		own_net;	// #READ_ONLY #NO_SAVE Network this layer is in
  LayerType		layer_type;	// design hint on layer usage (but input/output always allowed)
  int			n_units;
  // no. of units to create with Build command (0=use geometry)
  PosTDCoord		geom;
  // geometry (size) of units in layer (or of each subgroup if geom.z > 1)
  PosTDCoord		pos;		// position of layer
  PosTDCoord		gp_geom;	// #CONDEDIT_OFF_geom.z:1 geometry of sub-groups (if geom.z > 1)
  PosTDCoord		gp_spc;		// #CONDEDIT_OFF_geom.z:1 spacing between sub-groups (if geom.z > 1)
  TwoDCoord		flat_geom;	// #SHOW #READ_ONLY #NO_SAVE flat unit geometry (esp. when geom.z > 1)
  PosTDCoord		act_geom;	// #HIDDEN actual view geometry, incl spaces and with gps
  Projection_Group  	projections;	// group of receiving projections
  Projection_Group  	send_prjns;	// #HIDDEN #LINK_GROUP group of sending projections
  Unit_Group		units;		// units or groups of units
  UnitSpec_SPtr 	unit_spec;	// default unit specification for units in this layer
  bool			lesion;		// #DEF_false inactivate this layer from processing (reversable)
  Unit::ExtType		ext_flag;	// #READ_ONLY #SHOW indicates which kind of external input layer received
  int_Array		sent_already; 	// #READ_ONLY #NO_SAVE array of layer addresses for coordinating sending of net input to this layer
  DMemDist		dmem_dist; 	// how to distribute units across multiple distributed memory processors

  bool			uses_groups() {return (geom.z > 1);}
  
  ProjectBase*		project(); // this layer's project
  	
#ifdef DMEM_COMPILE
  DMemShare 	dmem_share_units;    	// #IGNORE the shared units
  virtual void	DMem_SyncNRecvCons();   // #IGNORE syncronize number of receiving connections (share set 0)
  virtual void	DMem_SyncNet();       	// #IGNORE syncronize just the netinputs (share set 1)
  virtual void	DMem_SyncAct();         // #IGNORE syncronize just the activations (share set 2)
  virtual void 	DMem_DistributeUnits();	// #IGNORE distribute units to different nodes (for this layer)
  virtual bool	DMem_DistributeUnits_impl(DMemShare& dms); // #IGNORE implementation: if true, a non-standard distribution was used (i.e., unit_groups)
#else
  virtual bool	DMem_DistributeUnits_impl(DMemShare&) { return false; } // #IGNORE to keep the ta file consistent..
#endif

  virtual void	Copy_Weights(const Layer* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE copies weights from other layer (incl wts assoc with unit bias member)
  virtual void	WriteWeights(ostream& strm, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #EXT_strm_wts #COMPRESS write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual void	ReadWeights(istream& strm, Con_Group::WtSaveFormat fmt = Con_Group::TEXT);
  // #MENU #EXT_strm_wts #COMPRESS read weight values in from a simple ordered list of weights (optionally in binary fmt)

  virtual void  Build();
  // #MENU #MENU_ON_Actions #CONFIRM build the units based on n_units, geom
  virtual void	RecomputeGeometry();
  // recompute the layer's geometry specifcations
  virtual void  LayoutUnits(Unit* u=NULL);
  // #ARGC_0 layout the units according to layer geometry
  virtual void  LayoutUnitGroups();
  // #MENU #CONFIRM layout the unit groups according to layer group geometry and spacing
  virtual void  ConnectFrom(Layer* lay);
  // #DYN12N connect one or more other layers to this layer
  virtual void  Connect();
  // #MENU #CONFIRM connect the layer
  virtual bool	CheckBuild();
  // check if build needs to be performed (true if yes, false if no)
  virtual bool	CheckConnect();
  // check if connect needs to be performed (true if yes, false if no)
  virtual void	RemoveCons();
  // #MENU #CONFIRM #MENU_SEP_BEFORE remove all connections in this layer
  virtual void	RemoveUnits();
  // #MENU #DYN1 remove all units in this layer (preserving groups)
  virtual void	RemoveUnitGroups();
  // #MENU #DYN1 remove all unit groups in this layer
  virtual void  PreConnect();	// prepare to connect the layer (create con_groups)
  virtual void	SyncSendPrjns();
  // synchronize sending projections with the recv projections so everyone's happy
  virtual void  ReConnect_Load();		// #IGNORE re-connect the layer after loading
  virtual void	DisConnect();
  // #MENU #CONFIRM disconnect layer from all others
  virtual int CountRecvCons();
  // count recv connections for all units in layer

  virtual void  InitExterns();	// Initializes external and target inputs
  virtual void  InitDelta();	// Initialize the unit deltas
  virtual void  InitState();	// Initialize the unit state variables
  virtual void	ModifyState(); 	// Alters state in an algorithm-specific way (e.g., decay)
  virtual void  InitWtDelta();	// Initialize the deltas
  virtual void  InitWtState();
  // #MENU #LABEL_Init_Weights #CONFIRM Initialize the weights
  virtual void	InitWtState_post(); // #IGNORE run after init wt state (ie. to scale wts..)

  virtual void	Compute_Net();	// Compute NetInput
  virtual void	Send_Net();	// sender-based compute net-input sending to all layers
  virtual void	Send_NetToLay(Layer* tolay);	// sender-based compute net-input sending only to tolay layer
  virtual void	Send_NetToMe();	// tell all layers that I receive from to send their net input to me
  virtual void	Compute_Act();	// Compute Activation
  virtual void	UpdateWeights(); // update weights for whole layer
  virtual void	Compute_dWt();	 // update weights for whole layer

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL remove weights that (after pre-proc) meet relation to compare val
  virtual int	ProbAddCons(float p_add_con, float init_wt = 0.0);
  // #MENU #USE_RVAL probabilistically add new connections (assuming prior pruning), init_wt = initial weight value of new connection
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL remove connectiosn with prob p_lesion (permute = fixed no. lesioned)
  virtual int	LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL remove units with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	SetLayerSpec(LayerSpec* layspec);
  // #MENU #MENU_SEP_BEFORE set the layer specification
  virtual LayerSpec* GetLayerSpec()		{ return (LayerSpec*)NULL; }
  virtual bool	SetUnitSpec(UnitSpec* unitspec);
  // #MENU set for all units in layer
  virtual void	SetUnitType(TypeDef* td);
  // #MENU #TYPE_Unit set unit type for all units in layer (created by Build)
  virtual bool	SetConSpec(ConSpec* conspec);
  // #MENU set for all unit's connections in layer
  virtual bool	CheckTypes();
  // #MENU #USE_RVAL check that the object and spec types are all ok
  virtual bool 	CheckConfig(Network* net, bool quiet=false);
  // check for for misc configuration settings required by different algorithms
  virtual void	FixPrjnIndexes();
  // #MENU fix the projection indicies of the connection groups (other_idx)

  virtual int	ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // switch any units/layers using old_sp to using new_sp
  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // switch any connections/projections using old_sp to using new_sp
  virtual int	ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // switch any projections using old_sp to using new_sp
  virtual int	ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp);
  // switch any layers using old_sp to using new_sp

  virtual void	GridViewWeights(GridLog* grid_log, Layer* send_lay, bool use_swt=false, int un_x=-1, int un_y=-1, int wt_x=-1, int wt_y=-1);
  /* #MENU #MENU_SEP_BEFORE #NULL_OK display entire set of weights from sending layer (use sending weights if use_swt) in grid log, -1 for x,y = use layer geometry
     (otherwise limits range, un= unit range, wt = weight (sending lay) range) */
  virtual void	WeightsToTable(DataTable* dt, Layer* send_lay);
  // #MENU #NULL_OK TODO:define send entire set of weights from sending layer to given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit

  void		SetExtFlag(int flg)   { ext_flag = (Unit::ExtType)(ext_flag | flg); }
  void		UnSetExtFlag(int flg) { ext_flag = (Unit::ExtType)(ext_flag & ~flg); }

  virtual void	ApplyData(taMatrix* data, int frame = -1, 
    Unit::ExtType ext_flags = Unit::DEFAULT,
    Random* ran = NULL, const PosTwoDCoord* offset = NULL);
    // apply the 2d, or 4d pattern to the network, optional random bias, and offsetting;\nuses a flat 2-d model where grouped layer or 4-d data are flattened to 2d;\nframe<0 means from end

  Unit*		FindUnitFmCoord(int x, int y);
  Unit*		FindUnitFmCoord(const TwoDCoord& coord) {return FindUnitFmCoord(coord.x, coord.y);}
  // get unit from coordinates, taking into account group geometry if present (subtracts any gp_spc -- as if it is not present).
  Unit*		FindUnitFmGpCoord(int gp_x, int gp_y, int un_x, int un_y);
  Unit*		FindUnitFmGpCoord(const TwoDCoord& gp_coord, const TwoDCoord& coord)
    {return FindUnitFmGpCoord(gp_coord.x,gp_coord.y, coord.x, coord.y);}
  // get unit from coordinates
  Unit_Group* 	FindUnitGpFmCoord(int gp_x, int gp_y);
  Unit_Group* 	FindUnitGpFmCoord(const TwoDCoord& coord) {return FindUnitGpFmCoord(coord.x,coord.y);}
  // get unit group from group coordinates (i.e., within gp_geom, not unit coordinates)
  virtual void	GetActGeomNoSpc(PosTDCoord& nospc_geom);
  // get the actual geometry of the layer, subtracting any gp_spc that might be present (as if there were no spaces between unit groups)

  void		SetDefaultPos(); // #IGNORE initialize position of layer

  virtual void	CopyNetwork(Network* net, Network* cn, Layer* cp); // #IGNORE copy entire network
  virtual void	CopyPtrs(Layer* cp); // #IGNORE the pointers
  void	ReplacePointersHook(TAPtr old);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_Network); }
#endif
  bool	Dump_QuerySaveMember(MemberDef* md);
  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const Layer& cp);
  COPY_FUNS(Layer, taNBase);
  TA_BASEFUNS(Layer); //
  

#ifdef TA_GUI
//protected:
//  override taiDataLink*	ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type);
#endif

protected:
  class TxferDataStruct { // #
  public:
    taMatrix*		data;
    int			frame;
    Unit::ExtType	ext_flags;
    Random* 		ran;
    int			offs_x;
    int			offs_y;
    TxferDataStruct(
      taMatrix*		data_,
      int		frame_,
      Unit::ExtType	ext_flags_,
      Random* 		ran_,
      const PosTwoDCoord* offs
    ) 
#ifndef __MAKETA__
    :data(data_), frame(frame_), ext_flags(ext_flags_), ran(ran_)
#endif
    {if (offs) {offs_x=offs->x; offs_y=offs->y;} else {offs_x=0; offs_y=0;}} // maketa can't parse :syntax
  };
  
  virtual void		ApplyLayerFlags(Unit::ExtType act_ext_flags);
    // #IGNORE set layer flag to reflect the kind of input received
  virtual void		ApplyData_Flat2d(const TxferDataStruct& ads);
    // #IGNORE flat layer, 2d data
  virtual void		ApplyData_Flat4d(const TxferDataStruct& ads);
    // #IGNORE flat layer, 4d data
  virtual void		ApplyData_Gp2d(const TxferDataStruct& ads);
    // #IGNORE grouped layer, 2d data
  virtual void		ApplyData_Gp4d(const TxferDataStruct& ads);
    // #IGNORE grouped layer, 4d data
};

PosGroup_of(Layer);
SmartRef_Of(Layer); // LayerRef

//class Network : public WinMgr {
class PDP_API Network : public taNBase {
  // ##EXT_net ##COMPRESS A network, containing layers, units, etc..
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  enum LayerLayout {		// Visual mode of layer position/view
    TWO_D,			// #LABEL_2D all z = 0, no skew
    THREE_D 	    		// #LABEL_3D z = layer index, default skew
  };

  enum Usr1SaveFmt {		// how to save network on -USR1 signal
    FULL_NET,			// save the full network (dump file)
    JUST_WEIGHTS		// just do a 'write weights' command
  };

  enum DMem_SyncLevel {
    DMEM_SYNC_NETWORK,		// synchronize the entire network at a time
    DMEM_SYNC_LAYER		// synchronize only layer-by-layer
  };

  enum WtSaveFormat {
    TEXT,			// weights are saved as ascii text representation of digits (completely portable)
    TEXT_IDX,			// text format plus unit indexes (useful for partially-connected networks where connections might change)
    BINARY,			// weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
    BINARY_IDX 			// binary format plus unit indexes (useful for partially-connected networks where connections might change)
  };
  
  enum WtUpdate {
    ON_LINE,			// update weights on-line (after every event) -- this is not viable for dmem processing and is automatically switched to small_batch
    SMALL_BATCH, 		// update weights every batch_n events 
    BATCH			// update weights in batch (after every epoch)
  };

 enum NetContext { // an extensible enum (ex. NetContext) used to control train/test contexts
   TEST	= 0,	 		// indicates outputs are not clamped, net being tested or run
   TRAIN = 1	  		// usually indicates patterns are being clamped on outs etc.
 };
 
 enum NetCounter { // the hierarchical counters, used to specify from where to clear
   NC_CYCLE,
   NC_PHASE,
   NC_TRIAL,
   NC_EPOCH,
   NC_BATCH
 };

  Layer_Group	layers;		// Layers or Groups of Layers
  int		context;	// #READ_ONLY_IV used by programs to provide context modality to algorithms
  WtUpdate	wt_update;	// #READ_ONLY_IV determines weight update mode
  int		batch_n;	// #CONDEDIT_ON_wt_update:SMALL_BATCH number of events for small_batch learning mode (specifies how often weight changes are synchronized in dmem)
  int		batch_n_eff;	// #READ_ONLY #NO_SAVE effective batch_n value = batch_n except for dmem when it = (batch_n / epc_nprocs) >= 1
  int		batch;		// #READ_ONLY_IV batch counter (updated by program)
  int		epoch;		// #READ_ONLY_IV epoch counter (updated by program)
  int		trial;		// #READ_ONLY_IV trial counter (updated by program)
  int		phase;		// #READ_ONLY_IV phase counter, used by some algos (updated by program)
  int		cycle;		// #READ_ONLY_IV cycle counter, used by some algos (updated by program)	
  bool		re_init;	// should net be initialized (InitWtState) by process?
  DMem_SyncLevel dmem_sync_level; // at what level of network structure should information be synchronized across processes?
  int		dmem_nprocs;	// number of processors to use in distributed memory computation of connection-level processing (actual number may be less, depending on processors requested!)
  int		dmem_nprocs_actual; // #READ_ONLY #NO_SAVE actual number of processors being used
  int		dmem_gp;	// #IGNORE the group for the network communicator
  Usr1SaveFmt	usr1_save_fmt;	// save network for -USR1 signal: full net or weights
  WtSaveFormat	wt_save_fmt;	// format to save weights in if saving weights
  LayerLayout	lay_layout;	// Visual mode of layer position/view

  int		n_units;	// #READ_ONLY #SHOW total number of units in the network
  int		n_cons;		// #READ_ONLY #SHOW total number of connections in the network
  PosTDCoord	max_size;	// #READ_ONLY #SHOW maximum size in each dimension of the net

  ProjectBase*	proj;		// #READ_ONLY #NO_SAVE ProjectBase this network is in
//obs  bool		net_will_updt;	// #HIDDEN #NO_SAVE if true, network will do update of display so don't do at lower level

#ifdef DMEM_COMPILE
  DMemShare 	dmem_share_units;    	// #IGNORE the shared units
  virtual void	DMem_SyncNRecvCons();   // syncronize number of receiving connections (share set 0)
  virtual void	DMem_SyncNet();       	// #IGNORE synchronize just the netinputs (share set 1)
  virtual void	DMem_SyncAct();         // #IGNORE synchronize just the activations (share set 2)
  virtual void 	DMem_DistributeUnits();	// distribute units to different nodes
  virtual void 	DMem_PruneNonLocalCons(); // #IGNORE prune non-local connections from all units: units only have their own local connections
  virtual void  DMem_SyncWts(MPI_Comm comm, bool sum_dwts = false); // #IGNORE sync weights by averaging together across comm (sum_dwts = sum all dwts instead)
  virtual void	DMem_SymmetrizeWts(); 	// #IGNORE symmetrize the weights (if necessary) by sharing weight values across processors
#else
  virtual void	DMem_SyncNRecvCons() { };   // syncronize number of receiving connections (share set 0)
  virtual void 	DMem_DistributeUnits() { };	// distribute units to different nodes
#endif

  virtual int	GetDefaultX();
  virtual int	GetDefaultY();
  virtual int	GetDefaultZ();
#ifdef TA_GUI
  virtual void	OpenViewer();
  // #MENU #MENU_SEP_BEFORE #MENU_CONTEXT open a viewer on this network
#endif
  void		ClearCounters(NetCounter down_from = NC_BATCH); // clear the network counters down from indicate point, default clears all
  virtual void	Copy_Weights(const Network* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE copies weights from other network (incl wts assoc with unit bias member)
  virtual void	WriteWeights(ostream& strm, WtSaveFormat fmt = TEXT);
  // #MENU #EXT_strm_wts #COMPRESS write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual void	ReadWeights(istream& strm);
  // #MENU #EXT_strm_wts #COMPRESS read weight values in from a simple ordered list of weights (fmt is read from file)
  virtual void  ReadOldPDPNet(istream& strm,bool skip_dots=true);
  /* #MENU #ARGC_2 #EXT_strm_net #UPDATE_MENUS
     Read old style PDP .net file into network. If skip_dots then ignore
     '.' values in network connectivity matrix, otherwise create a zero weight */

  virtual void  Build();
  // #MENU #MENU_ON_Actions Build the network according to geometry
  virtual void	Connect();
  // #MENU #MENU_ON_Actions Connect this network according to projections on Layers
  virtual bool	CheckBuild();
  // check if build needs to be performed (true if yes, false if no)
  virtual bool	CheckConnect();
  // check if connect needs to be performed (true if yes, false if no)
  virtual void	UpdtAfterNetMod();
  // update network after any network modification (calls appropriate functions)
  virtual void	SyncSendPrjns();
  // synchronize sending projections with the recv projections so everyone's happy
  virtual void 	CountRecvCons();
  // count recv connections for all units in network

  virtual void	ConnectUnits(Unit* u_to, Unit* u_from=NULL, bool record=true,
			     ConSpec* conspec=NULL);
  // connect u1 so that it recieves from u2. Create projection if necessary

  virtual void	RemoveCons();
  // #MENU #MENU_ON_Actions #CONFIRM #MENU_SEP_BEFORE Remove all connections in network
  virtual void	RemoveUnits();
  // #MENU #CONFIRM Remove all units in network (preserving unit groups)
  virtual void	RemoveUnitGroups();
  // #MENU #CONFIRM Remove all unit groups in network
  virtual void	LayoutUnitGroups();
  // #MENU #CONFIRM layout all the layer's unit groups according to layer group geometry and spacing
  virtual void	PreConnect();	// Prepare to connect this network (make con_groups)
  virtual void	ReConnect_Load(); // #IGNORE ReConnect network after loading
  virtual void	CopyNetwork(Network* net);  // #IGNORE copy entire network

  virtual bool	CheckTypes();
  // #MENU #MENU_ON_Actions #USE_RVAL #MENU_SEP_BEFORE check that the object and spec types are all ok
  virtual bool	CheckConfig(bool quiet=false);
  // check for for misc configuration settings required by different algorithms
  virtual void	FixPrjnIndexes();
  // #MENU fix the projection indicies of the connection groups (other_idx)

  virtual void	RemoveMonitors();
  // Remove monitoring of all objects in all processes associated with parent project
  virtual void	UpdateMonitors();
  // Update monitoring of all objects in all processes associated with parent project

  virtual void  InitExterns();	// Initializes external and target inputs
  virtual void  InitDelta();	// Initialize the unit deltas
  virtual void  InitState();
  // #MENU #MENU_SEP_BEFORE Initialize the unit state variables
  virtual void	ModifyState();  // Alters the state in an algorithm-specify way
  virtual void  InitWtDelta();	// Initialize the Weight deltas
  virtual void  InitWtState();
  // #MENU #CONFIRM Initialize the weights, reset the epoch ctr to 0
  virtual void	InitWtState_post(); // #IGNORE run after init wt state (ie. to scale wts..)

  virtual void	Compute_Net();	// Compute NetInput
  virtual void	Send_Net();	// sender-based computation of net input
  virtual void	Compute_Act() {Compute_Act_default();}
  	// Compute Activation; subtypes may replace with their own algorithm
  virtual void	Compute_Act_default(); // default version, may be replaced or extended
  virtual void	UpdateWeights(); // update weights for whole net
  virtual void	Compute_dWt(); // update weights for whole net

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL remove weights that (after pre-proc) meet relation to compare val
  virtual int	ProbAddCons(float p_add_con, float init_wt = 0.0);
  // #MENU #USE_RVAL probabilistically add new connections (assuming prior pruning), init_wt = initial weight value of new connection
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL remove connections with prob p_lesion (permute = fixed no. lesioned)
  virtual int	LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL remove units with prob p_lesion (permute = fixed no. lesioned)

  virtual void	TwoD_Or_ThreeD(LayerLayout layout_type);
  // #MENU #MENU_SEP_BEFORE Set 2d or 3d and reposition and redraw layers

  virtual void	GridViewWeights(GridLog* grid_log, Layer* recv_lay, Layer* send_lay, bool use_swt=false, int un_x=-1, int un_y=-1, int wt_x=-1, int wt_y=-1);
  /* #MENU #MENU_SEP_BEFORE #NULL_OK display entire set of weights from sending layer to recv_lay (use sending weights if use_swt) in grid log, -1 for x,y = use layer geometry
     (otherwise limits range, un= unit range, wt = weight (sending lay) range) */
  virtual void	WeightsToTable(DataTable* dt, Layer* recv_lay, Layer* send_lay);
  // #MENU #NULL_OK send entire set of weights from sending layer to recv layer in given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit

  virtual int	ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // switch any units/layers using old_sp to using new_sp
  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // switch any connections/projections using old_sp to using new_sp
  virtual int	ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // switch any projections using old_sp to using new_sp
  virtual int	ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp);
  // switch any layers using old_sp to using new_sp

  // wizard construction functions:
  virtual Layer* FindMakeLayer(const char* nm, TypeDef* td = NULL, bool& nw_itm = nw_itm_def_arg, const char* alt_nm = NULL);
  // find a given layer and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
  virtual Layer* FindLayer(const char* nm) { return (Layer*)layers.FindName(nm); }
  virtual Projection* FindMakePrjn(Layer* recv, Layer* send, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // find a projection between two layers using given specs, make it if not found; if existing prjn between layers exists, it will be modified with current specs
  virtual Projection* FindMakePrjnAdd(Layer* recv, Layer* send, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // find a projection between two layers using given specs, make it if not found; if existing prjn between layers exists but has diff specs, a new prjn is made
  virtual Projection* FindMakeSelfPrjn(Layer* recv, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // find a self projection using given specs, make it if not found; if existing self prjn exists, it will be modified with current specs
  virtual Projection* FindMakeSelfPrjnAdd(Layer* recv, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // find a self projection using given specs, make it if not found; if existing self prjn exists but has diff specs, a new prjn is made
  virtual bool   RemovePrjn(Layer* recv, Layer* send, ProjectionSpec* ps = NULL, ConSpec* cs = NULL);
  // remove a projection between two layers, if it exists
  virtual bool   RemoveLayer(const char* nm) { return layers.RemoveName(nm); }
  // remove layer with given name, if it exists

  virtual void	FixLayerViews(Layer* lay=NULL);
  // #IGNORE update the layer on all the nets views

  virtual void	UpdateMax();	// #IGNORE

//TODO???  TypeDef*	GetDefaultView() { return &TA_NetView; } // #IGNORE default view
  int		Dump_Load_Value(istream& strm, TAPtr par=NULL);
  // call reconnect afterwards
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_Network); }
#endif
  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void 	Copy_(const Network& cp);
  COPY_FUNS(Network, taNBase);
  TA_BASEFUNS(Network);
#ifdef TA_GUI
protected:
//  override taiDataLink*	ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type);
#endif
};

//note: Network_Group name is for compatability with v3.2 files
class PDP_API Network_Group : public taGroup<Network> {
public:
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_Network); }
#endif
  void	Initialize() 		{SetBaseType(&TA_Network);}
  void 	Destroy()		{ };
  TA_BASEFUNS(Network_Group); //
};

class PDP_API LayerRWBase: public taOBase  {
  // #VIRT_BASE #NO_INSTANCE #NO_TOKENS helper object to read/write data to/from layers
friend class LayerRWBase_List;
friend class LayerWriter_List;
INHERITED(taOBase)
public:
  DataBlockRef		data_block; // source or sink of the data (as appropriate)
  String		chan_name; // name of the chan/col to use (we'll cache its col_idx for efficiency)
  LayerRef 		layer;	// the Layer that will get read or written
  PosTwoDCoord		offset;	// offset in layer or unit group at which to start reading/writing
  
  void  UpdateAfterEdit();
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const LayerRWBase& cp); 
  COPY_FUNS(LayerRWBase, taOBase);
  TA_BASEFUNS(LayerRWBase);
  
protected:
  int			chan_idx; // cached col, -1 = not looked up, or invalid
  
  int			GetChanIdx(bool force_lookup = false); // recalcs if needed
private:
  void	Initialize();
  void 	Destroy();
};


class PDP_API LayerRWBase_List: public taList<LayerRWBase> {
  // #VIRT_BASE #NO_INSTANCE list of individual LayerRWBase objects
INHERITED(taList<LayerRWBase>)
public:
  LayerRWBase*		FindByDataBlockLayer(DataBlock* db, Layer* lay);
    // find the item by source and target -- note: finds first if multiple the same
  void			FillFromDataBlock(DataBlock* db, Network* net, bool freshen_only);
    // #MENU_ON_Data #MENU #MENU_CONTEXT #BUTTON #MENU_SEP_BEFORE do a 'best guess' fill of items by matching up like-named Channels and Layers
  void			FillFromTable(DataTable* dt, Network* net, bool freshen_only);
    // #MENU #MENU_CONTEXT #BUTTON do a 'best guess' fill of items by matching up like-named Columns and Layers
    
  
  TA_ABSTRACT_BASEFUNS(LayerRWBase_List); //

protected:
  virtual void		FillFromDataBlock_impl(DataBlock* db, Network* net,
    bool freshen, Layer::LayerType lt){}
    
private:
  void	Initialize() {SetBaseType(&TA_LayerRWBase);}
  void 	Destroy() {}
};


class PDP_API LayerWriter: public LayerRWBase {
  // object that writes data from a DataSource to a layer
INHERITED(LayerRWBase)
public: 
  Unit::ExtType	ext_flags;	// how to flag the unit/layer's external input status
  Random	noise;		// noise optionally added to values when applied
  String_Array  value_names;	// display names of the individual pattern values

  virtual void 		ApplyData(int context);
    // convenience method for applying data in indicated context
  
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const LayerWriter& cp);
  COPY_FUNS(LayerWriter, LayerRWBase);
  TA_BASEFUNS(LayerWriter); //
  
private:
  void	Initialize();
  void 	Destroy();
};

class PDP_API LayerWriter_List: public LayerRWBase_List {
  // ##TOKENS #INSTANCE list of individual LayerWriter objects
INHERITED(LayerRWBase_List)
public:
  
  inline LayerWriter*	FastEl(int i) {return (LayerWriter*)FastEl_(i);}
  void			ApplyData(int context);
    // convenience method that iterates the writers and applies data
    
  TA_BASEFUNS(LayerWriter_List); //

protected:
  override void		FillFromDataBlock_impl(DataBlock* db, Network* net,
    bool freshen, Layer::LayerType lt);
    
private:
  void	Initialize() {SetBaseType(&TA_LayerWriter);}
  void 	Destroy() {}
};

/*TODO
class PDP_API LayerReader: public LayerRWBase {
  // object that reads data from a Layer to a DataSink 
INHERITED(LayerRWBase)
public:

  
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const LayerReader& cp);
  COPY_FUNS(LayerReader, LayerRWBase);
  TA_BASEFUNS(LayerReader);
  
private:
  void	Initialize();
  void 	Destroy();
};


class PDP_API LayerReader_List: public LayerRWBase_List {
  // ##TOKENS #INSTANCE list of LayerReader objects
INHERITED(LayerRWBase_List)
public:
  TA_BASEFUNS(LayerReader_List); //

protected:
  override void		FillFromDataBlock_impl(DataBlock* db, Network* net,
    bool freshen, Layer::LayerType lt);
    
private:
  void	Initialize() {SetBaseType(&TA_LayerReader);}
  void 	Destroy() {}
};
*/

class PDP_API NetMonItem: public taNBase {
  //  #NO_TOKENS #NO_UPDATE_AFTER used for monitoring the value of a net object:\nLayer, Projection, UnitGroup, Unit
INHERITED(taNBase)
public:
  static const String 	GetObjName(TAPtr obj, TAPtr own = NULL); 
   // get name of object for naming stats, etc. qualifies up to Layer; looks up own if NULL
  static const String	DotCat(const String& lhs, const String& rhs); 
    // cat with . except if either empty, just return the other
  
  taSmartRef 		object;		// #NO_SCOPE the network object being monitored
  String        	variable;	// Variable (member) to monitor
  ChannelSpec_List	val_specs;	// #SHOW #NO_SAVE specs of the values being monitored 
  MemberSpace   	members;	// #IGNORE memberdefs
  taBase_List		ptrs;     	// #HIDDEN #NO_SAVE actual ptrs to values
  SimpleMathSpec 	pre_proc_1;	// #EXPERT first step of pre-processing to perform
  SimpleMathSpec 	pre_proc_2;	// #EXPERT second step of pre-processing to perform
  SimpleMathSpec 	pre_proc_3;	// #EXPERT third step of pre-processing to perform
  

  void		SetMonVals(TAPtr obj, const String& var); 
    // set object and variable, and update appropriately
//TODO: add funcs for specific object types, and put in gui directives


  void			ScanObject();	// #IGNORE update the schema
//obs  void		NameStatVals();
  virtual void 		UpdateMonVals(DataBlock* db); // get all the values!
  void			ResetMonVals(); // deletes the cached vars
  
  String GetColText(int col, int itm_idx = -1);
  void  InitLinks();
  void	CutLinks();
  void	UpdateAfterEdit();
  void 	Copy_(const NetMonItem& cp);
  COPY_FUNS(NetMonItem, taNBase);
  TA_BASEFUNS(NetMonItem);//
  
protected:
  int			cell_num; // current cell number, when adding mon vals
  override void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
  override void		SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
    int dcr, void* op1_, void* op2_);

  ChannelSpec* 		AddScalarChan(const String& valname, ValType val_type);
  MatrixChannelSpec* 	AddMatrixChan(const String& valname, ValType val_type,
    const MatrixGeom* geom = NULL);
    // caller resp for somehow setting geom if NULL; clears cell_num
  bool 			AddCellName(const String& cellname);
  bool	 		GetMonVal(int i, Variant& rval); // get the value at i, true if exists
  // these are for finding the members and building the stat
  // out of the objects and the variable
  
/*  void 			ScanObject_InObject(TAPtr obj, String var);
  void			ScanObject_IterLayer(Layer* lay, String var); // #IGNORE
  void			ScanObject_IterUnitGroup(Unit_Group* ug, String var);	// #IGNORE
  void			ScanObject_IterGroup(taGroup_impl* gp, String var);	// #IGNORE
  void			ScanObject_IterList(taList_impl* list, String var);	// #IGNORE
*/  
  
  bool 			ScanObject_InObject(TAPtr obj, String var, 
    bool mk_col = false, TAPtr own = NULL);
  void			ScanObject_Network(Network* net, String var); // #IGNORE
  void			ScanObject_Layer(Layer* lay, String var); // #IGNORE
  void			ScanObject_Projection(Projection* p, String var); // #IGNORE
  void			ScanObject_UnitGroup(Unit_Group* ug, String var, 
    bool mk_col = false);	// #IGNORE
  void			ScanObject_Unit(Unit* u, String var,
    Projection* p = NULL, bool mk_col = false); // #IGNORE
  void			ScanObject_ConGroup(Con_Group* cg, String var,
    Projection* p = NULL); // #IGNORE note: always makes a col
  
// following return 'true' if the routine handled making the colspecs

private:
  void	Initialize();
  void 	Destroy() {CutLinks();}
};


class PDP_API NetMonItem_List: public taList<NetMonItem> { 
INHERITED(taList<NetMonItem>)
public:

  int	NumListCols() const {return 3;} 
  String GetColHeading(int col); // header text for the indicated column
  TA_BASEFUNS(NetMonItem_List);
  
private:
  void		Initialize() {SetBaseType(&TA_NetMonItem);}
  void		Destroy() {}
};

class PDP_API NetMonitor: public taNBase { // ##TOKENS #NO_UPDATE_AFTER used for monitoring values of network objects
INHERITED(taNBase)
public:
  NetMonItem_List	items; // the list of items being monitored
  DataTableRef		data; // the data table that will be used to hold the monitor data
  bool			rmv_orphan_cols; // #DEF_true remove orphan columns when updating table schema

  void		SetDataTable(DataTable* dt); // #MENU #MENU_ON_Action set the data table used
  void		AddNetwork(Network* net, const String& variable)
    {AddObject(net, variable);}
    // #MENU #MENU_ON_Action #MENU_SEP_BEFORE monitor a value in the Network or its subobjects
  void		AddLayer(Layer* lay, const String& variable)
    {AddObject(lay, variable);}
    // #MENU monitor a value in the Layer or its subobjects
  void		AddProjection(Projection* prj, const String& variable)
    {AddObject(prj, variable);}
    // #MENU monitor a value in the Projection or its subobjects
  void		AddUnitGroup(Unit_Group* ug, const String& variable)
    {AddObject(ug, variable);}
    // monitor a value in the UnitGroup or its subobjects
  void		AddUnit(Unit* un, const String& variable)
    {AddObject(un, variable);}
    // monitor a value in the Unit or its subobjects
  
  void		AddObject(TAPtr obj, const String& variable);
    // monitor a value in the object or its subobjects
  void 		UpdateMonitors(); // #MENU #MENU_SEP_BEFORE create or update the channels
  void 		UpdateMonVals(); // get all the values!
  
  void		RemoveMonitors(); //called by net to remove the objs from lists
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const NetMonitor& cp);
  void	UpdateAfterEdit();
  COPY_FUNS(NetMonitor, taNBase);
  TA_BASEFUNS(NetMonitor);
  
private:
  void		Initialize();
  void		Destroy() {CutLinks();}
};



#endif /* netstru_h */
