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

// netstru.h

#ifndef netstru_h
#define netstru_h

#include "tarandom.h"
#include "ta_script.h"
#include "ta_data.h"
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

class SigmoidSpec : public taBase {
// ##NO_TOKENS #INLINE #NO_UPDATE_AFTER Specifies a Sigmoid 1 / [1 + exp(-(x - off) * gain)]
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


class SchedItem : public taOBase {
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

class Schedule : public taList<SchedItem> {
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

class Connection : public taBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER Generic Connections
public:
  float 	wt;		// weight of connection

  bool		ChangeMyType(TypeDef* new_type);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Network); }
#endif
  void 	SetTypeDefaults()	{ }; // overload this to do nothing (faster)
  void	UpdateAfterEdit();	     // might want to override any default updates..
  void 	Initialize() 		{ wt = 0.0f; }
  void 	Destroy()		{ };
  void	Copy_(const Connection& cp)	{ wt = cp.wt; }
  COPY_FUNS(Connection, taBase);
  TA_BASEFUNS(Connection);
};


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

class WeightLimits : public taBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER specifies weight limits for connections
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

class ConSpec : public BaseSpec {
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

  virtual bool  	CheckConfig(Con_Group*, Layer*, Unit*, TrialProcess*, bool =false) { return true; }
  // check for for misc configuration settings required by different algorithms, including settings on the processes

  virtual int		UseCount(); // return number of times this spec is used
  void	ReplacePointersHook(TAPtr old);

  virtual  void  InitWeights();
  // #BUTTON initializes weights for all projections with this conspec

  virtual bool	 DMem_AlwaysLocal() { return false; }
  // overload this function to prevent this projection from being pruned for non-local units under dmem processing (for "special" connection types)
  virtual MemberDef* DMem_EpochShareDwtVar() { return min_con_type->members.FindName("dwt"); }
  // name of weight-change variable to share across dmem processors in BATCH mode learning
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_ConSpec); }
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

class Unit_List : public taList<Unit> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(Unit_List);
};

class Con_Group : public taBase_Group {
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

  bool  CheckConfig(Layer* lay, Unit* ru, TrialProcess* tp, bool quiet=false) { return spec->CheckConfig(this, lay, ru, tp, quiet); }

  int 	Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  int	Dump_SaveR(ostream& strm, TAPtr par=NULL, int indent = 0);
  int	Dump_Save_PathR(ostream& strm, TAPtr par=NULL, int indent = 0);
  int	Dump_Load_Value(istream& strm, TAPtr par=NULL);

  bool	ChangeMyType(TypeDef* new_type);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Network); }
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

class UnitSpec : public BaseSpec { // Generic Unit Specification
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

  virtual bool  CheckConfig(Unit* un, Layer* lay, TrialProcess* tp, bool quiet=false);
  // check for for misc configuration settings required by different algorithms, including settings on the processes

  virtual int	UseCount(); // return number of times this spec is used
  void	ReplacePointersHook(TAPtr old);

#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_UnitSpec); }
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

class Unit : public taNBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##DMEM_SHARE_SETS_3 Generic unit
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
protected:
  static Con_Group*	rcg_rval; // return value for connecting
  static Con_Group*	scg_rval; // return value for connecting
public:
  enum ExtType {		// indicates type of external input unit received
    NO_EXTERNAL = 0x00,		// no input
    TARG 	= 0x01,		// target input (value is in targ)
    EXT 	= 0x02,		// external input (value is in ext)
    TARG_EXT 	= 0x03,		// target and external input
    COMP	= 0x04,		// comparison value (for error) (value is in targ)
    COMP_TARG	= 0x05,		// comparision and target
    COMP_EXT	= 0x06,		// comparison and external input
    COMP_TARG_EXT = 0x07 	// comparison, target, and external input
  };

  UnitSpec_SPtr spec;		// unit specification
  TDCoord       pos;		// position in space relative to owning group, layer
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
  int 		dmem_local_proc; // #IGNORE processor on which these units are local
  static int	dmem_this_proc;	// #IGNORE processor rank for this processor RELATIVE TO COMMUNICATOR for the network
  virtual bool 	DMem_IsLocalProc(int proc)   	{ return dmem_local_proc == proc; } // #IGNORE
  virtual bool 	DMem_IsLocal()       		{ return dmem_local_proc == dmem_this_proc; }  // #IGNORE
  virtual int 	DMem_GetLocalProc() 		{ return dmem_local_proc; } // #IGNORE
  virtual void 	DMem_SetLocalProc(int lproc) 	{ dmem_local_proc = lproc; } // #IGNORE
  virtual void 	DMem_SetThisProc(int proc) 	{ dmem_this_proc = proc; } // #IGNORE
#endif

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

  bool  CheckConfig(Layer* lay, TrialProcess* tp, bool quiet=false) { return spec->CheckConfig(this, lay, tp, quiet); }

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
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Network); }
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

class ProjectionSpec : public BaseSpec {
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
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_ProjectionSpec); }
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

class Projection : public taNBase {
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
#ifdef TA_GUI // TODO: prob obsolete -- not really used anywhere, except 2 funcs below
  Xform*		proj_points;     // #OWN_POINTER #HIDDEN projection points

  void			SetFromPoints(float x1, float y1);
  void			SetToPoints(float x1, float y1);
#endif
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
  virtual void	WeightsToEnv(Environment* env);
  // #MENU #NULL_OK send entire set of projection weights to given environment (e.g., for analysis), with one event per receiving unit, and the pattern in the event reflects the weights into that unit

  void 	CopyPtrs(Projection* cp); // #IGNORE copy the pointers
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Network); }
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


class Unit_Group : public taGroup<Unit> {
  // #NO_UPDATE_AFTER a group of units
#ifndef __MAKETA__
typedef taGroup<Unit> inherited;
#endif
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

  virtual Unit* FindUnitFmCoord(const TwoDCoord& coord);
  // returns unit at given coordinates within layer

  void		RemoveAll();
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Network); }
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

class LayerSpec : public BaseSpec {
  // generic layer specification
public:
  virtual int	UseCount(); // return number of times this spec is used
  void	ReplacePointersHook(TAPtr old);

#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_LayerSpec); }
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
class Layer : public taNBase, public IDataSource {
  // ##EXT_lay ##COMPRESS layer containing units
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  enum DMemDist {
    DMEM_DIST_DEFAULT,		// distribute units to different processors for distributed memory processing according to the default sequential scheme
    DMEM_DIST_UNITGP		// distribute units according to unit groups, which can be less even but allows for shared weights by unit group
  };

  Network*		own_net;	// #READ_ONLY #NO_SAVE Network this layer is in
  int			n_units;
  // no. of units to create with Build command (0=use geometry)
  PosTDCoord		geom;
  // geometry (size) of units in layer (or of each subgroup if geom.z > 1)
  PosTDCoord		pos;		// position of layer
  PosTDCoord		gp_geom;	// #CONDEDIT_OFF_geom.z:1 geometry of sub-groups (if geom.z > 1)
  PosTDCoord		gp_spc;		// #CONDEDIT_OFF_geom.z:1 spacing between sub-groups (if geom.z > 1)
  PosTDCoord		act_geom;	// #HIDDEN actual geometry (if geom.z > 1)
  Projection_Group  	projections;	// group of receiving projections
  Projection_Group  	send_prjns;	// #HIDDEN #LINK_GROUP group of sending projections
  Unit_Group		units;		// units or groups of units
  UnitSpec_SPtr 	unit_spec;	// default unit specification for units in this layer
  bool			lesion;		// #DEF_false inactivate this layer from processing (reversable)
  Unit::ExtType		ext_flag;	// #READ_ONLY #SHOW indicates which kind of external input layer received
  int_Array		sent_already; 	// #READ_ONLY #NO_SAVE array of layer addresses for coordinating sending of net input to this layer
  DMemDist		dmem_dist; 	// how to distribute units across multiple distributed memory processors

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
  virtual bool 	CheckConfig(TrialProcess* tp, bool quiet=false);
  // check for for misc configuration settings required by different algorithms, including settings on the processes
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
  virtual void	WeightsToEnv(Environment* env, Layer* send_lay);
  // #MENU #NULL_OK send entire set of weights from sending layer to given environment (e.g., for analysis), with one event per receiving unit, and the pattern in the event reflects the weights into that unit

  void		SetExtFlag(int flg)   { ext_flag = (Unit::ExtType)(ext_flag | flg); }
  void		UnSetExtFlag(int flg) { ext_flag = (Unit::ExtType)(ext_flag & ~flg); }

  virtual Unit*	FindUnitFmCoord(const TwoDCoord& coord);
  // get unit from coordinates, taking into account group geometry if present (subtracts any gp_spc -- as if it is not present).
  virtual Unit_Group* FindUnitGpFmCoord(const TwoDCoord& coord);
  // get unit group from group coordinates (i.e., within gp_geom, not unit coordinates)
  virtual void	GetActGeomNoSpc(PosTDCoord& nospc_geom);
  // get the actual geometry of the layer, subtracting any gp_spc that might be present (as if there were no spaces between unit groups)

  void		SetDefaultPos(); // #IGNORE initialize position of layer

  virtual void	CopyNetwork(Network* net, Network* cn, Layer* cp); // #IGNORE copy entire network
  virtual void	CopyPtrs(Layer* cp); // #IGNORE the pointers
  void	ReplacePointersHook(TAPtr old);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Network); }
#endif
  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const Layer& cp);
  COPY_FUNS(Layer, taNBase);
  TA_BASEFUNS(Layer); //
  
public: // IDataSource i/f and impl
  SourceChannel_List  source_channels;
  // override bool	can_sequence_() const {return false;} // true if has a ISequencable interface
  // override ISequencable* sequencer_() {return NULL;} // sequencing interface, if sequencable
  override SourceChannel_List& source_channels_() {return source_channels;}
protected: // SourceChannel delegates -- designed for multi-class inheritance chain
  override void GetData_(SourceChannel* ch, ptaMatrix_impl& data, bool& handled);

#ifdef TA_GUI
//protected:
//  override taiDataLink*	ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type);
#endif
};

PosMGroup_of(Layer);

//class Network : public WinMgr {
class Network : public taNBase {
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

  Layer_MGroup	layers;		// Layers or Groups of Layers
  int		epoch;		// epoch counter (updated by process)
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

  Project*	proj;		// #READ_ONLY #NO_SAVE Project this network is in
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
  virtual void CountRecvCons();
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
  virtual bool	CheckConfig(TrialProcess* tp, bool quiet=false);
  // check for for misc configuration settings required by different algorithms, including settings on the processes
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
  virtual void	Compute_Act();	// Compute Activation
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
  virtual void	WeightsToEnv(Environment* env, Layer* recv_lay, Layer* send_lay);
  // #MENU #NULL_OK send entire set of weights from sending layer to recv layer in given environment (e.g., for analysis), with one event per receiving unit, and the pattern in the event reflects the weights into that unit

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
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Network); }
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

//note: Network_MGroup name is for compatability with v3.2 files
class Network_MGroup : public taGroup<Network> {
public:
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Network); }
#endif
  void	Initialize() 		{SetBaseType(&TA_Network);}
  void 	Destroy()		{ };
  TA_BASEFUNS(Network_MGroup);
};

#endif /* netstru_h */
