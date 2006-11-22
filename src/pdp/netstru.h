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

#include "ta_script.h"
#include "datatable.h"

#include "spec.h"

#include "pdp_base.h"

#ifdef TA_GUI
//#include "ta_qtbrowse_def.h"
#include "t3viewer.h"
//#include "fontspec.h"
#endif

#include <math.h>

#include "ta_dmem.h"

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
// ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##CAT_Math Specifies a Sigmoid 1 / [1 + exp(-(x - off) * gain)]
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
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Network one element of a schedule
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
  // ##CAT_Network A schedule for parameters that vary over time
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


// the connection is managed fully by the ConSpec and the RecvCons
// don't put any functions on the connection itself

// note: v4.0 connection stuff is not backwards compatible with v3.2

// note: connection must be just a bag of bits with *no* functions of its own
// especially no virtual functions!  it is managed with raw bit copy ops
// although the sending connections have pointers to connection objects
// it is up to the connection management code to ensure that when a
// connection is removed, its associated sending link is also removed

class PDP_API Connection {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network base connection between two units
public:
  float 	wt;		// weight of connection
  float		dwt;		// #NO_VIEW #NO_SAVE resulting net weight change

  Connection() { wt = dwt = 0.0f; }
};

// SmartRef_Of(Connection); // ConnectionSRef

// the ConSpec has 2 versions of every function: one to go through the group
// and the other to apply to a single connection.
// The C_ denotes the con one which is non-virtual so that it is inlined
// this requires the group level one to be re-defined whenever the con
// level one is.  This is necessary given the speed penalty for a virtual
// function call at the connection level.

// The exception to the non-virtual rule is the C_Init functions!

// The following macro makes this process easier:

#define	CON_GROUP_LOOP(cg, expr) \
  for(int i=0; i<cg->cons.size; i++) \
    expr

class PDP_API WeightLimits : public taBase {
  // ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##CAT_Network specifies weight limits for connections
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
  // ##CAT_Spec Connection specs: for processing over a set of connections all from the same projection 
public:
  TypeDef*	min_con_type;
  // #HIDDEN #NO_SAVE #TYPE_Connection mimimal con type required for spec (obj is con group)
  Random	rnd;		// #CAT_ConSpec Weight randomization specification
  WeightLimits	wt_limits;	// #CAT_ConSpec limits on weight sign, symmetry

  inline void		C_ApplyLimits(Connection* cn, Unit*, Unit*)
  { wt_limits.ApplyLimits(cn->wt); }
  // #CAT_Learning apply weight limits to single connection
  inline virtual void	ApplyLimits(RecvCons* cg, Unit* ru);
  // #CAT_Learning apply weight limits (sign, magnitude)

  virtual void		ApplySymmetry(RecvCons* cg, Unit* ru);
  // #CAT_Learning apply weight symmetrizing between reciprocal units

  inline virtual void	C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su);
  inline virtual void 	Init_Weights(RecvCons* cg, Unit* ru);
  // #CAT_Learning initialize state variables (ie. at beginning of training)
  inline virtual void	C_Init_Weights_post(RecvCons*, Connection*, Unit*, Unit*) { };
  // #IGNORE
  inline virtual void 	Init_Weights_post(RecvCons* cg, Unit* ru);
  // #IGNORE post-initialize state variables (ie. for scaling symmetrical weights, etc)
  inline virtual void 	C_Init_dWt(RecvCons*, Connection* cn, Unit*, Unit*)
  { cn->dwt=0.0f; }
  // #CAT_Learning initialize weight-change variables on a single connecdtion
  inline virtual void 	Init_dWt(RecvCons* cg, Unit* ru);
  // #CAT_Learning initialize weight-change variables for whole set

  inline float 		C_Compute_Netin(Connection* cn, Unit* ru, Unit* su);
  inline virtual float 	Compute_Netin(RecvCons* cg, Unit* ru);
  // #CAT_Activation compute net input for weights in this con group
  inline void 		C_Send_Netin(Connection* cn, Unit* ru, Unit* su);
  inline virtual void 	Send_Netin(SendCons* cg, Unit* su);
  // #CAT_Activation sender-based net input for con group (send net input to receivers)
  inline float 		C_Compute_Dist(Connection* cn, Unit* ru, Unit* su);
  inline virtual float 	Compute_Dist(RecvCons* cg, Unit* ru);
  // #CAT_Activation compute net distance for con group (ie. euclidean distance)
  inline void		C_Compute_dWt(Connection*, Unit*, Unit*)	{ };
  inline virtual void	Compute_dWt(RecvCons* cg, Unit* ru);
  // #CAT_Learning compute the delta-weight change
  inline void 		C_Compute_Weights(Connection*, Unit*, Unit*)	{ };
  inline virtual void 	Compute_Weights(RecvCons* cg, Unit* ru);
  // #CAT_Learning update weights (ie. add delta-wt to wt, zero delta-wt)

  bool	CheckObjectType_impl(taBase* obj); // don't do checking on 1st con group in units

  virtual bool  	CheckConfig_RecvCons(RecvCons* cg, bool quiet=false);
  // check for for misc configuration settings required by different algorithms

  virtual int		UseCount();
  // #CAT_ObjectMgmt return number of times this spec is used

  virtual  void  Init_Weights_Net();
  // #BUTTON #CAT_Learning initializes all weights in the network

  virtual bool	 DMem_AlwaysLocal() { return false; }
  // #CAT_DMem overload this function to prevent this projection from being pruned for non-local units under dmem processing (for "special" connection types)

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ConSpec& cp);
  COPY_FUNS(ConSpec, BaseSpec);
  TA_BASEFUNS(ConSpec);
};

SpecPtr_of(ConSpec);


/////////////////////////////////////////////////////////////////////////////////
//		RecvCons


class PDP_API  ConArray : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network a physically contiguous array of connections, for receiving con group
  INHERITED(taOBase)
public:
  int		con_size;	// sizeof() connection object being stored
  TypeDef*	con_type;	// type of connection object being stored
  int 		size;		// #NO_SAVE #READ_ONLY number of elements in the array
  int		alloc_size;	// #READ_ONLY #NO_SAVE #DETAIL allocated (physical) size, in con_size units
  char*		cons;		// #IGNORE the connection memory, alloc_size * con_size

  inline bool		InRange(int idx) const {return ((idx < size) && (idx >= 0));}
  // #CAT_Access is index in range?
  inline Connection*	SafeEl(int idx) const
  { if(!InRange(idx)) return NULL; return (Connection*)&(cons[con_size * idx]); }
  // #CAT_Access safely access connection at given index, consumer must cast to appropriate sub-type (for type safety, check con_type)
  inline Connection*	FastEl(int idx) const
  { return (Connection*)&(cons[con_size * idx]); }
  // #CAT_Access fast access (no range checking) connection at given index, consumer must cast to appropriate type (for type safety, check con_type)

  void			SetType(TypeDef* cn_tp);
  // #CAT_Modify set new connection type -- resets any existing conections
  void			Alloc(int n);
  // #CAT_Modify allocate storage for at least the given size
  void			Free();
  // #CAT_Modify deallocate all storage
  inline void		SetSize(int sz)
  { if(sz > size) { Alloc(sz); bzero((void*)FastEl(size), (sz - size) * con_size); }
    size = sz; }
  // #CAT_Modify set size of array to given number of elements, with new items initialized to zero
  inline void		New(int n) { SetSize(size + n); }
  // #CAT_Modify add n new connections (initialized to all zeros)
  inline void		Reset()	{ SetSize(0); }
  // #CAT_Modify reset size of array to zero (does not free underlying memory)

  inline void		Add(const Connection* it)
  { SetSize(size + 1); memcpy((void*)FastEl(size-1), (void*)it, con_size); }
  // #CAT_Modify add a connection 
  bool			RemoveIdx(int i);
  // #CAT_Modify remove connection at given index, moving others down to fill in

  inline void		CopyCons_impl(const ConArray& cp)
  { SetSize(cp.size); if(size > 0) memcpy(cons, (char*)cp.cons, size * con_size); }
  // #IGNORE copy connections from other con array, ASSUMES they are both of same type
  inline bool		CopyCons(const ConArray& cp)
  { if(con_type != cp.con_type) return false; CopyCons_impl(cp); return true; }
  // #CAt_Modify copy connections from other con array, checking to make sure they are the same type (false if not)

  void 	Initialize();
  void 	Destroy();
  void	CutLinks();
  void	Copy_(const ConArray& cp);
  COPY_FUNS(ConArray, inherited);
  TA_BASEFUNS(ConArray);
};

class PDP_API UnitPtrList: public taPtrList<Unit> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network list of unit pointers, for sending connections
public:
  ~UnitPtrList()             { Reset(); }
};

class PDP_API RecvCons : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network receiving connections: owns all the connection objects
INHERITED(taOBase)
public:
  static int	Idx;			// #HIDDEN pass to find if you don't want one

  // note: follwing must be coordinated with the Network enum
  enum WtSaveFormat {
    TEXT,			// weights are saved as ascii text representation of digits (completely portable)
    BINARY,			// weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
  };

  // note: the cons and units are saved in an optimized fashion

  TypeDef*	con_type;
  // #CAT_Structure #AKA_el_typ type of connections to make
  ConArray	cons;
  // #NO_FIND #NO_SAVE #CAT_Structure the array of connections, in index correspondence with units
  UnitPtrList	units;
  // #NO_FIND #NO_SAVE #CAT_Structure pointers to the sending units of this connection (in index correspondence with cons)
  ConSpec_SPtr 	spec;
  // #CAT_Structure specification for connections
  Projection*	prjn;
  // #CAT_Structure pointer the the projection which created this Group
  int		send_idx;
  // #READ_ONLY index into sending unit's send. list of SendCons

  Connection* 	Cn(int i) const { return cons.FastEl(i); }
  // #CAT_Structure gets the connection at the given index
  Unit*		Un(int i) const { return units.FastEl(i); }
  // #CAT_Structure gets the unit at the given index

  virtual void		SetConType(TypeDef* cn_tp);
  // #CAT_Structure set the type of connection to make
  virtual void		AllocCons(int no);
  // #CAT_Structure allocate given number of new connections (if projection knows this in advance, it is more efficient than doing them incrementally)
  virtual Connection*	NewCon(Unit* un);
  // #CAT_Structure create a connection to given unit
  virtual bool		RemoveConIdx(int i);
  // #CAT_Structure remove connection at given index
  virtual bool		RemoveConUn(Unit* un);
  // #CAT_Structure remove connection from given unit
  virtual void		RemoveAll();
  // #CAT_Structure remove all conections
  virtual void		Reset() { RemoveAll(); }
  // #CAT_Structure remove all conections

  virtual Connection*	FindConFrom(Unit* un, int& idx=Idx) const;
  // #MENU #MENU_ON_Actions #USE_RVAL #ARGC_1 #CAT_Structure find connection from given unit
  static Connection* 	FindRecipRecvCon(Unit* su, Unit* ru);
  // #CAT_Structure find the reciprocal for sending unit su to this receiving unit ru
  static Connection* 	FindRecipSendCon(Unit* ru, Unit* su);
  // #CAT_Structure find the reciprocal for receiving unit ru from this sending unit su

  // these are convenience functions for those defined in the spec
  void 	Init_Weights(Unit* ru)	 	{ spec->Init_Weights(this,ru); }
  // #CAT_Learning initialize weights for group
  void 	C_Init_Weights(Connection* cn, Unit* ru, Unit* su)
  { spec->C_Init_Weights(this, cn, ru, su); }
  // #CAT_Learning initialize weights for single connection
  void	Init_Weights_post(Unit* ru) 	{ spec->Init_Weights_post(this,ru); }
  // #IGNORE
  void 	Init_dWt(Unit* ru)	 	{ spec->Init_dWt(this,ru); }
  // #CAT_Learning  initialize weight change variables

  float Compute_Netin(Unit* ru)	 	{ return spec->Compute_Netin(this,ru); }
  // #CAT_Activation compute net input (receiver based; recv group)
  float Compute_Dist(Unit* ru)	 	{ return spec->Compute_Dist(this,ru); }
  // #CAT_Activation compute net input as distance between activation and weights
  void  Compute_dWt(Unit* ru)	 	{ spec->Compute_dWt(this,ru); }
  // #CAT_Learning compute weight changes (the fundamental learning problem)
  void 	Compute_Weights(Unit* ru)	{ spec->Compute_Weights(this,ru); }
  // #CAT_Learning update weight values from deltas
  
  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(Unit* un, const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(Unit* un, float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove weights with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	ConValuesToArray(float_Array& ary, const char* variable);
  // #CAT_Structure adds values of variable from the connections into the given array (false if var not found)
  virtual bool	ConValuesToMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure sets values of variable from the connections into the given matrix (uses flat index of cons to set: 0..size-1), returns false if matrix is not appropriately sized
  virtual bool	ConValuesFromArray(float_Array& ary, const char* variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found)
  virtual bool	ConValuesFromMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found) -- uses flat index of cons to set: 0..size-1

  static int	LoadWeights_StartTag(istream& strm, const String& tag, String& val, bool quiet);
  // #IGNORE read in a start tag -- makes sure it matches tag, returns TAG_GOT if got it
  static int	LoadWeights_EndTag(istream& strm, const String& trg_tag, String& cur_tag, int& stat, bool quiet);
  // #IGNORE read in an end tag -- makes sure it matches trg_tag, cur_tag, stat are current read_tag & status (if !END_TAG, will try to read end)

  virtual void	SaveWeights_strm(ostream& strm, Unit* ru, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, Unit* ru, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = true);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary format) -- rval is taMisc::ReadTagStatus, TAG_END if successful
  static int 	SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = true);
  // #IGNORE skip over saved weights (to keep the file in sync) -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void	SaveWeights(const String& fname="", Unit* ru = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="", Unit* ru = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = true);
  // #MENU #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary format) (leave fname empty to pull up file chooser)

  int 	Dump_Save_Value(ostream& strm, taBase* par=NULL, int indent = 0);
  int	Dump_Load_Value(istream& strm, taBase* par=NULL);

  virtual void	CopyNetwork(Network* net, Network* cn, RecvCons* cp);
  // #IGNORE copy entire network
  virtual void	Copy_Weights(const RecvCons* src);
  // #CAT_ObjectMgmt copies weights from other con_group
  virtual bool	CheckTypes(bool quiet=false);
  // #MENU #USE_RVAL #CAT_ObjectMgmt check that the object and spec types are all ok
  virtual bool  CheckSendIdx();
  // #CAT_ObjectMgmt check validity of send_idx for recv con groups

  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_ObjectMgmt switch any connections using old_sp to using new_sp

  bool	ChangeMyType(TypeDef* new_type);
  
  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const RecvCons& cp);
  COPY_FUNS(RecvCons, inherited);
  TA_BASEFUNS(RecvCons);
protected:
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
};

// use this macro to replace the prevalent FOR_ITR_GP used to traverse connection groups
// in 3.2

#define	FOR_ITR_CON_GP(typ, ptr, gp, idx) \
  for(idx=0; idx<gp size; idx++) {   \
    typ* ptr = gp FastEl(idx);


class PDP_API RecvCons_List: public taList<RecvCons> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network list of receiving connections, one per projection
public:
  virtual RecvCons*	NewPrjn(Projection* prjn);
  // #CAT_Structure create a new sub_group from given projection, with given ownership (own_cons)
  virtual RecvCons*	FindPrjn(Projection* prjn, int& idx=Idx) const;
  // #CAT_Structure find sub group associated with given projection
  virtual RecvCons*	FindFrom(Layer* from, int& idx=Idx) const;
  // #MENU #USE_RVAL #ARGC_1 #CAT_Structure find sub group that receives from given layer
  virtual RecvCons*	FindFromName(const String& fm_nm, int& idx=Idx) const;
  // #MENU #USE_RVAL #ARGC_1 #CAT_Structure find sub group that receives from given layer named fm_nm
  virtual RecvCons*	FindTypeFrom(TypeDef* prjn_typ, Layer* from, int& idx=Idx) const;
  // #MENU #USE_RVAL #ARGC_2 #CAT_Structure find sub group that recvs prjn of given type from layer
  virtual RecvCons*	FindLayer(Layer* lay, int& idx=Idx) const;
  // #CAT_Structure find sub group where projection is in the given layer
  virtual bool		RemovePrjn(Projection* prjn);
  // #CAT_Structure remove sub group associated with given projection
  virtual bool		RemoveFrom(Layer* from);
  // #MENU #CAT_Structure remove sub group that receives from given layer

  void	Initialize() 		{ SetBaseType(&TA_RecvCons); }
  void 	Destroy()		{ };
  TA_BASEFUNS(RecvCons_List);
};

/////////////////////////////////////////////////////////////////////////////////
//		SendCons

class PDP_API ConPtrList: public taPtrList<Connection> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network list of connection pointers, for sending connections
public:
  ~ConPtrList()             { Reset(); }
};


class PDP_API SendCons : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network sending connections: points to receiving connections
INHERITED(taOBase)
public:
  static int	Idx;			// #HIDDEN pass to find if you don't want one

  TypeDef*	con_type;
  // #CAT_Structure #AKA_el_typ type of connections to make
  ConPtrList	cons;
  // #NO_FIND #NO_SAVE #CAT_Structure list of pointers to receiving connections, in index correspondence with units;
  UnitPtrList	units;
  // #NO_FIND #NO_SAVE #CAT_Structure pointers to the receiving units of this connection, in index correspondence with cons
  ConSpec_SPtr 	spec;
  // #CAT_Structure specification for connections
  Projection*	prjn;
  // #CAT_Structure pointer the the projection which created this Group
  int		recv_idx;
  // #READ_ONLY index into recv unit's list of RecvCons for this projection

  Connection* 	Cn(int i) const { return cons.FastEl(i); }
  // #CAT_Structure gets the connection at the given index
  Unit*		Un(int i) const { return units.FastEl(i); }
  // #CAT_Structure gets the unit at the given index

  virtual void		SetConType(TypeDef* cn_tp);
  // #CAT_Structure set the type of connection to make
  virtual void		LinkCon(Connection* cn, Unit* un);
  // #CAT_Structure make a link connection from given connection, unit (for sending groups)
  virtual bool		RemoveConIdx(int i);
  // #CAT_Structure remove connection at given index
  virtual bool		RemoveConUn(Unit* un);
  // #CAT_Structure remove connection from given unit
  virtual void		RemoveAll();
  // #CAT_Structure remove all conections
  virtual void		Reset() { RemoveAll(); }
  // #CAT_Structure remove all conections
  virtual Connection*	FindConFrom(Unit* un, int& idx=Idx) const;
  // #MENU #MENU_ON_Actions #USE_RVAL #ARGC_1 #CAT_Structure find connection from given unit

  void 	Send_Netin(Unit* su)		{ spec->Send_Netin(this, su); }
  // #CAT_Activation send net input (sender based; send group)
  
  virtual bool	ConValuesToArray(float_Array& ary, const char* variable);
  // #CAT_Structure adds values of variable from the connections into the given array (false if var not found)
  virtual bool	ConValuesToMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure sets values of variable from the connections into the given matrix (uses flat index of cons to set: 0..size-1), returns false if matrix is not appropriately sized
  virtual bool	ConValuesFromArray(float_Array& ary, const char* variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found)
  virtual bool	ConValuesFromMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found) -- uses flat index of cons to set: 0..size-1

  virtual void	CopyNetwork(Network* net, Network* cn, SendCons* cp);
  // #IGNORE copy entire network: doesn't actually copy here -- just resets and reconnect_load does the work..
  virtual bool	CheckTypes(bool quiet=false);
  // #MENU #USE_RVAL #CAT_ObjectMgmt check that the object and spec types are all ok
  virtual bool 	CheckRecvIdx();
  // #CAT_ObjectMgmt check validity of recv_idx for send con groups

  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_ObjectMgmt switch any connections using old_sp to using new_sp


  bool	ChangeMyType(TypeDef* new_type);
  
  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const SendCons& cp);
  COPY_FUNS(SendCons, inherited);
  TA_BASEFUNS(SendCons);
};

class PDP_API SendCons_List: public taList<SendCons> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network list of sending connections, one per projection
public:
  // projection-related functions for operations on sub-groups of the group
  virtual SendCons*	NewPrjn(Projection* prjn);
  // #CAT_Structure create a new sub_group from given projection, with given ownership (own_cons)
  virtual SendCons*	FindPrjn(Projection* prjn, int& idx=Idx) const;
  // #CAT_Structure find sending connections associated with given projection
  virtual SendCons*	FindFrom(Layer* from, int& idx=Idx) const;
  // #MENU #USE_RVAL #ARGC_1 #CAT_Structure find sending connections that receive from given layer
  virtual SendCons*	FindFromName(const String& fm_nm, int& idx=Idx) const;
  // #MENU #USE_RVAL #ARGC_1 #CAT_Structure find sending connections that receive from given layer named fm_nm
  virtual SendCons*	FindTypeFrom(TypeDef* prjn_typ, Layer* from, int& idx=Idx) const;
  // #MENU #USE_RVAL #ARGC_2 #CAT_Structure find sending connections that recvs prjn of given type from layer
  virtual SendCons*	FindLayer(Layer* lay, int& idx=Idx) const;
  // #CAT_Structure find sending connections where projection is in the given layer
  virtual bool		RemovePrjn(Projection* prjn);
  // #CAT_Structure remove sending connections associated with given projection
  virtual bool		RemoveFrom(Layer* from);
  // #MENU #CAT_Structure remove sending connections from given layer

  void	Initialize() 		{ SetBaseType(&TA_SendCons); }
  void 	Destroy()		{ };
  TA_BASEFUNS(SendCons_List);
};


//////////////////////////////////////////////////////////////////////////////
//		Units and UnitSpec

class PDP_API UnitSpec: public BaseSpec {
  // ##CAT_Spec Generic Unit Specification
INHERITED(BaseSpec)
  static RecvCons*	rcg_rval; // return value for connecting
  static SendCons*	scg_rval; // return value for connecting
public:
  MinMaxRange	act_range;
  // #CAT_Activation range of activation for units
  TypeDef*	bias_con_type;
  // #HIDDEN #TYPE_Connection #NULL_OK #CAT_Structure type of bias connection to make
  ConSpec_SPtr 	bias_spec;
  // #CAT_Structure con spec that controls the bias connection on the unit
  float		sse_tol;
  // #CAT_Statistic tolerance for computing sum-squared error on a per-unit basis

  virtual void 	Init_Acts(Unit* u);
  // #CAT_Activation initialize unit activation state variables
  virtual void 	Init_dWt(Unit* u);
  // #CAT_Learning initialize the weight change variables
  virtual void 	Init_Weights(Unit* u);
  // #CAT_Learning init weight state variables
  virtual void	Init_Weights_post(Unit* u);
  // #IGNORE run after init wt state (ie. to scale wts)

  virtual void 	Compute_Netin(Unit* u);
  // #CAT_Activation compute net input: activations times weights (receiver based)
  virtual void 	Send_Netin(Unit* u);
  // #CAT_Activation send net input to all units I send to (sender based)
  virtual void 	Send_NetinToLay(Unit* u, Layer* tolay);
  // #CAT_Activation send net input only to units in given layer (not used by default)
  virtual void 	Compute_Act(Unit* u);
  // #CAT_Activation compute the activation value of the unit: what it sends to other units

  virtual void 	Compute_dWt(Unit* u);
  // #CAT_Learning compute change in weights: the mechanism of learning
  virtual void 	Compute_Weights(Unit* u);
  // #CAT_Learning update weights from deltas

  virtual float	Compute_SSE(Unit* u);
  // #CAT_Statistic compute sum squared error for this unit

  virtual void	BuildBiasCons();
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE #CAT_Structure build the bias connections according to specified type

  virtual bool  CheckConfig_Unit(Unit* un, bool quiet=false);
  // #CAT_ObjectMgmt check for for misc configuration settings required by different algorithms

  virtual int	UseCount();
  // #CAT_ObjectMgmt return number of times this spec is used

  void 	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const UnitSpec& cp);
  COPY_FUNS(UnitSpec, BaseSpec);
  TA_BASEFUNS(UnitSpec);
protected:
  override void  	UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& ok);
};

SpecPtr_of(UnitSpec);

class PDP_API Unit: public taNBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##DMEM_SHARE_SETS_3 ##CAT_Network Generic unit
INHERITED(taNBase)
protected:
  static RecvCons*	rcg_rval; // return value for connecting
  static SendCons*	scg_rval; // return value for connecting
public: //
  enum ExtType {// #BITS indicates type of external input; some flags used in Layer to control usage
    NO_EXTERNAL 	= 0x00,	// #NO_BIT no input
    TARG 		= 0x01,	// a target value used to train the network (value goes in targ field of unit)
    EXT 		= 0x02,	// an external input value that drives activations (value goes in ext field of unit)
    COMP		= 0x04,	// a comparison value used for computing satistics but not training the network (value goes in targ field of unit)
    TARG_EXT	 	= 0x03,	// #NO_BIT as both external input and target value
    COMP_TARG		= 0x05,	// #NO_BIT as a comparision and target layer
    COMP_EXT		= 0x06,	// #NO_BIT as a comparison and external input layer
    COMP_TARG_EXT	= 0x07	// #NO_BIT as a comparison, target, and external input layer
  };

  UnitSpec_SPtr spec;
  // #CAT_Structure unit specification: all the parameters that control unit function
  ExtType	ext_flag;
  // #GUI_READ_ONLY #SHOW #CAT_Activation tells what kind of external input unit received
  float 	targ;
  // #CAT_Activation target value: drives learning to produce this activation value
  float 	ext;
  // #CAT_Activation external input: drives activation of unit from outside influences (e.g., sensory input)
  float 	act;
  // #DMEM_SHARE_SET_2 #CAT_Activation activation value -- what the unit communicates to others
  float 	net;
  // #DMEM_SHARE_SET_1 #CAT_Activation net input value -- what the unit receives from others
  RecvCons_List	recv;
  // #CAT_Structure Receiving connections, one set of connections for each projection (collection of connections) received from other units
  SendCons_List send;
  // #CAT_Structure Sending connections, one set of connections for each projection (collection of connections) sent from other units
  RecvCons	bias;
  // #CAT_Structure bias weight connection (type determined in unit spec) -- provides intrinsic activation in absence of other inputs
  int		n_recv_cons;
  // #DMEM_SHARE_SET_0 #CAT_Structure total number of receiving connections
  TDCoord       pos;
  // #CAT_Structure position in space relative to owning group, layer
  int		idx;
  // #READ_ONLY #HIDDEN #CAT_Structure index of this unit within containing unit group

#ifdef DMEM_COMPILE
  static int	dmem_this_proc;	// #IGNORE processor rank for this processor RELATIVE TO COMMUNICATOR for the network
  int 		dmem_local_proc; // #IGNORE processor on which these units are local
  virtual bool 	DMem_IsLocalProc(int proc)   	{ return dmem_local_proc == proc; } // #IGNORE
  virtual bool 	DMem_IsLocal()       		{ return dmem_local_proc == dmem_this_proc; }  // #IGNORE
  virtual int 	DMem_GetLocalProc() 		{ return dmem_local_proc; } // #IGNORE
  virtual void 	DMem_SetLocalProc(int lproc) 	{ dmem_local_proc = lproc; } // #IGNORE
  virtual void 	DMem_SetThisProc(int proc) 	{ dmem_this_proc = proc; } // #IGNORE
#endif

  Unit_Group*	ugrp() {return GET_MY_OWNER(Unit_Group);} // #IGNORE

  virtual void	Copy_Weights(const Unit* src, Projection* prjn = NULL);
  // #CAT_ObjectMgmt copies weights from other unit (incl wts assoc with unit bias member)
  virtual void	SaveWeights_strm(ostream& strm, Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = true);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus, TAG_END if successful
  static int	SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = true);
  // #IGNORE skip over saved weight values -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void	SaveWeights(const String& fname="", Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="", Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = true);
  // #MENU #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  void		SetExtFlag(ExtType flg) { ext_flag = (ExtType)(ext_flag | flg); }
  // #CAT_Activation set ext flag for what type of input data we receive
  void		UnSetExtFlag(ExtType flg) { ext_flag = (ExtType)(ext_flag & ~flg); }
  // #CAT_Activation un-set ext flag for what type of input data we receive

  virtual void 	Init_InputData()	{ ext = targ = 0.0f; ext_flag = NO_EXTERNAL; }
  // #MENU #MENU_ON_Actions initialize unit external input data variables
  virtual void 	Init_Netin()	{ net = 0.0f; }
  // #CAT_Activation initialize netinput state prior to computing it (for sender-based)

  // these are convenience functions for those defined in the spec
  void 	Init_Acts()		{ spec->Init_Acts(this); }
  // #MENU #CAT_Activation initialize unit state variables
  void 	Init_dWt()		{ spec->Init_dWt(this); }
  // #MENU #CAT_Learning initialze weight change variables
  void 	Init_Weights()		{ spec->Init_Weights(this); }
  // #MENU #CAT_Learning Initialize weight values
  void	Init_Weights_post() 	{ spec->Init_Weights_post(this); } // #IGNORE
  void 	Compute_Netin()		{ spec->Compute_Netin(this); }
  // #CAT_Activation compute net input from other units
  void 	Send_Netin()		{ spec->Send_Netin(this); }
  // #CAT_Activation send net input to other units
  void 	Send_NetinToLay(Layer* tolay)	{ spec->Send_NetinToLay(this, tolay); }
  // #CAT_Activation send net input to other units in given layer
  void 	Compute_Act()		{ spec->Compute_Act(this); }
  // #CAT_Activation compute activation value: what we send to others
  void 	Compute_dWt()		{ spec->Compute_dWt(this); }
  // #CAT_Learning compute weight changes: the essence of learning
  void 	Compute_Weights()		{ spec->Compute_Weights(this); }
  // #CAT_Learning update weight values from weight change variables

  float	Compute_SSE()		{ return spec->Compute_SSE(this); }
  // #CAT_Statistic compute sum-squared-error of activations versus target values (standard measure of performance)
  
  virtual void 	ApplyInputData(float val, ExtType act_ext_flags, Random* ran = NULL);
  // #CAT_Activation apply external input or target value to unit
  virtual bool	Build();
  // #CAT_Structure build unit: make sure bias connection is created and right type
  virtual bool	CheckBuild(bool quiet=false);
  // #CAT_Structure check if network is built 
  virtual void	RemoveCons();
  // #IGNORE remove all of unit's sending and receiving connections
  // since this doesn't affect other units, it should not be called individually
  virtual void		ConnectAlloc(int no, Projection* prjn, RecvCons*& cgp = rcg_rval);
  // #CAT_Structure pre-allocate given no of connections (for better memory layout)
  virtual Connection* 	ConnectFrom(Unit* su, Projection* prjn, RecvCons*& recv_gp = rcg_rval,
				    SendCons*& send_gp = scg_rval);
  // #CAT_Structure make a recv connection from given unit to this unit using given projection
  virtual Connection* 	ConnectFromCk(Unit* su, Projection* prjn, RecvCons*& recv_gp = rcg_rval,
				      SendCons*& send_gp = scg_rval);
  // #CAT_Structure does ConnectFrom but checks for an existing connection to prevent double-connections! -- note that this is expensive!
  virtual bool	DisConnectFrom(Unit* su, Projection* prjn=NULL);
  // #CAT_Structure remove connection from given unit (projection is optional)
  virtual void	DisConnectAll();
  // #MENU #MENU_ON_Actions #CAT_Structure disconnect unit from all other units
  virtual int	CountRecvCons();
  // #CAT_Structure count total number of receiving connections

  virtual void	TransformWeights(const SimpleMathSpec& trans, Projection* prjn = NULL);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec, Projection* prjn = NULL);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc, CountParam::Relation rel,
			     float cmp_val, Projection* prjn = NULL);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(float p_lesion, bool permute=true, Projection* prjn = NULL);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	SetConSpec(ConSpec* con_spec);
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE #CAT_Structure set all recv conspecs to con_spec
  virtual bool	CheckTypes(bool quiet=false);
  // #MENU #USE_RVAL #CAT_ObjectMgmt check that the object and spec types are all ok

  virtual int	ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // #CAT_Structure switch any units/layers using old_sp to using new_sp
  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp

  virtual void	CopyNetwork(Network* anet, Network* cn, Unit* cp); // #IGNORE copy network

  override int	GetIndex() { return idx; }
  override void	SetIndex(int i) { idx = i; }
  virtual int	GetMyLeafIndex();
  // compute leaf index from my individual index in an efficient manner
  
  void  UpdateAfterEdit();
  void	Initialize();
  void 	Destroy();
  void  InitLinks();
  void	CutLinks();
  void	Copy_(const Unit& cp);
  COPY_FUNS(Unit, taNBase);
  TA_BASEFUNS(Unit);

protected:
  override bool  CheckConfig_impl(bool quiet)
  { return spec->CheckConfig_Unit(this, quiet); }
};

// Projections are abrevieated prjn (as a oppesed to proj = project or proc = process)
// ProjectionSpec does the connectivity, and optionally the weight init

class PDP_API ProjectionSpec : public BaseSpec {
  // #VIRT_BASE ##CAT_Spec Specifies the connectivity between layers (ie. full vs. partial)
public:
  bool		self_con;	// #CAT_Projection whether to create self-connections or not (if applicable)
  bool		init_wts;	// #CAT_Projection whether this projection spec does weight init (else conspec)

  virtual void 	RemoveCons(Projection* prjn);
  // #CAT_Projection deletes any existing connections
  virtual void	PreConnect(Projection* prjn);
  // #CAT_Projection Prepare to connect (init con_groups)
  virtual void	Connect_impl(Projection*) { };
  // #CAT_Projection actually implements specific connection code
  virtual void 	Connect(Projection* prjn);
  // #CAT_Projection connects the network, first removing existing cons, and inits weights
  virtual void 	ReConnect_Load(Projection* prjn);
  // #IGNORE re-connects the network after loading based on recv groups only
  virtual int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);
  // #CAT_Projection probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
  virtual void 	Init_dWt(Projection* prjn);
  // #CAT_Weights initializes the weight change variables
  virtual void 	Init_Weights(Projection* prjn);
  // #CAT_Weights initializes the weiht values
  virtual void 	Init_Weights_post(Projection* prjn);
  // #IGNORE after weight init (ie. for weight scaling)

  virtual bool	CheckConnect(Projection* prjn, bool quiet=false);
  // #CAT_ObjectMgmt check if projection is connected

  virtual void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // #CAT_Weights custom initialize weights in this con group for given receiving unit ru

  virtual void 	CopyNetwork(Network* net, Network* cn, Projection* prjn, Projection* cp);
  // #CAT_ObjectMgmt copy projection from one network to the other
  virtual int	UseCount();
  // #CAT_ObjectMgmt return number of times this spec is used
  
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
  // ##CAT_Network Projection describes connectivity between layers (from receivers perspective)
INHERITED(taNBase)
public:
  enum PrjnSource {
    NEXT,		// Recv from the next layer in network
    PREV,		// Recv from the previous layer in network
    SELF,		// Recv from the same layer
    CUSTOM 		// Recv from the layer spec'd in the projection
  };

  Layer* 		layer;    	// #READ_ONLY #NO_SAVE layer this prjn is in
  PrjnSource 		from_type;	// #CAT_Projection Source of the projections
  Layer*		from;		// #CAT_Projection layer receiving from (set this for custom)
  ProjectionSpec_SPtr	spec;		// #CAT_Projection spec for this item
  TypeDef*		con_type;	// #TYPE_Connection #CAT_Projection Type of connection
  TypeDef*		recvcons_type;	// #TYPE_RecvCons #CAT_Projection Type of receiving connection group to make
  TypeDef*		sendcons_type;	// #TYPE_SendCons #CAT_Projection Type of sending connection group to make
  ConSpec_SPtr 		con_spec;	// #CAT_Projection conspec to use for creating connections
  int			recv_idx;	// #READ_ONLY #CAT_Projection receiving con_group index
  int			send_idx;	// #READ_ONLY #CAT_Projection sending con_group index
  int			recv_n;		// #READ_ONLY #CAT_Projection number of receiving con_groups
  int			send_n;		// #READ_ONLY #CAT_Projection number of sending con_groups

  bool			projected; 	 // #HIDDEN #CAT_Projection t/f if connected

  virtual void 	SetFrom();
  // #CAT_Projection set where to receive from based on selections

  virtual void	SetCustomFrom(Layer* from_lay);
  // #CAT_Projection set a CUSTOM projection from given layer (if from_lay == layer, turns into SELF)

  virtual void	Copy_Weights(const Projection* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_Weights copies weights from other projection
  virtual void	SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = true);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt)
  virtual void	SaveWeights(const String& fname="", RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="",
			    RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = true);
  // #MENU #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  // convenience functions for those defined in the spec
  void 	RemoveCons()		{ spec->RemoveCons(this); }
  // #MENU #MENU_ON_Actions #CONFIRM #CAT_Projection Reset all connections for this projection
  void 	PreConnect()		{ spec->PreConnect(this); }
  // #CAT_Projection pre-configure connection state
  void 	Connect()		{ spec->Connect(this); }
  // #BUTTON #CONFIRM #CAT_Projection Make all connections for this projection (resets first)
  void 	Connect_impl()		{ spec->Connect_impl(this); }
  // #CAT_Projection actually do the connecting
  void 	ReConnect_Load();	// #IGNORE
  int 	ProbAddCons(float p_add_con, float init_wt = 0.0) { return spec->ProbAddCons(this, p_add_con, init_wt); }
  // #MENU #USE_RVAL #CAT_Projection probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
  void 	CopyNetwork(Network* net, Network* cn, Projection* cp) { spec->CopyNetwork(net,cn,this,cp); } // #IGNORE
  void 	Init_dWt()		{ spec->Init_dWt(this); }
  // #MENU #MENU_SEP_BEFORE #CAT_Weights Initialize weight changes for this projection
  void 	Init_Weights()		{ spec->Init_Weights(this); }
  // #BUTTON #CONFIRM #CAT_Weights Initialize weight state for this projection
  void 	Init_Weights_post() 	{ spec->Init_Weights_post(this); } // #IGNORE

  void 	C_Init_Weights(RecvCons* cg, Unit* ru)  { spec->C_Init_Weights(this, cg, ru); }
  // #CAT_Weights custom initialize weights in this con group for given receiving unit ru

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Weights apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Weights add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Weights remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Projection remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual bool 	SetConSpec(ConSpec* sp);
  // #CAT_Projection set the con spec for all connections in this prjn
  virtual bool 	ApplyConSpec();
  // #BUTTON #CAT_Projection apply the default conspec to all connections in this prjn
  virtual bool	CheckConnect(bool quiet=false) { return spec->CheckConnect(this, quiet); }
  // #CAT_Projection check if projection is connected
  virtual bool	CheckTypes(bool quiet=false);
  // #BUTTON #USE_RVAL #CAT_ObjectMgmt check that the existing con and con gp types are of the specified types
  virtual void	FixIndexes();
  // #MENU #CAT_Projection fix the indicies of the connection groups (other_idx)
  virtual bool	CheckTypes_impl(bool quiet=false); // #IGNORE

  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Projection switch any connections/projections using old_sp to using new_sp
  virtual int	ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Projection switch any projections using old_sp to using new_sp

  virtual bool 	SetConType(TypeDef* td);
  // #CAT_Projection #TYPE_Connection set the connection type for all connections in this prjn
  virtual bool 	SetRecvConsType(TypeDef* td);
  // #CAT_Projection #TYPE_RecvCons set the receiving connection group type for all connections in this prjn
  virtual bool 	SetSendConsType(TypeDef* td);
  // #CAT_Projection #TYPE_SendCons set the connection group type for all connections in this prjn

//obs  virtual void	GridViewWeights(GridLog* grid_log, bool use_swt=false, int un_x=-1, int un_y=-1, int wt_x=-1, int wt_y=-1);
  /* #MENU #MENU_SEP_BEFORE #NULL_OK display entire set of projection weights (use sending weights if use_swt) in grid log, -1 for x,y = use layer geometry
     (otherwise limits range, un= unit range, wt = weight (sending lay) range) */
  virtual void	WeightsToTable(DataTable* dt);
  // #MENU #NULL_OK #CAT_Projection TODO:define send entire set of projection weights to given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit

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

class PDP_API Projection_Group: public taGroup<Projection> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network 
public:
  void	Initialize() 		{ SetBaseType(&TA_Projection); }
  void 	Destroy()		{ };
  TA_BASEFUNS(Projection_Group);
};

//////////////////////////////////////////////////////////
// 	Inline Connection-level functions (fast)	//
//////////////////////////////////////////////////////////

inline void ConSpec::ApplyLimits(RecvCons* cg, Unit* ru) {
  if(wt_limits.type != WeightLimits::NONE) {
    CON_GROUP_LOOP(cg, C_ApplyLimits(cg->Cn(i), ru, cg->Un(i)));
  }
}

inline void ConSpec::C_Init_Weights(RecvCons*, Connection* cn, Unit* ru, Unit* su) {
  if(rnd.type != Random::NONE)	{ // don't do anything (e.g. so connect fun can do it)
    cn->wt = rnd.Gen();
  }
  else {
    rnd.Gen();		// keep random seeds syncronized for dmem
  }
  C_ApplyLimits(cn,ru,su);
}

inline void ConSpec::Init_Weights(RecvCons* cg, Unit* ru) {
  if(cg->prjn->spec->init_wts) {
       cg->prjn->C_Init_Weights(cg, ru);
  } else {
    CON_GROUP_LOOP(cg, C_Init_Weights(cg, cg->Cn(i), ru, cg->Un(i)));
  }

  Init_dWt(cg,ru);
  ApplySymmetry(cg,ru);
}

inline void ConSpec::Init_Weights_post(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Init_Weights_post(cg, cg->Cn(i), ru, cg->Un(i)));
}

inline void ConSpec::Init_dWt(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Init_dWt(cg, cg->Cn(i), ru, cg->Un(i)));
}

inline float ConSpec::C_Compute_Netin(Connection* cn, Unit*, Unit* su) {
  return cn->wt * su->act;
}
inline float ConSpec::Compute_Netin(RecvCons* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin(cg->Cn(i), ru, cg->Un(i)));
  return rval;
}

inline void ConSpec::C_Send_Netin(Connection* cn, Unit* ru, Unit* su) {
  ru->net += cn->wt * su->act;
}
inline void ConSpec::Send_Netin(SendCons* cg, Unit* su) {
  CON_GROUP_LOOP(cg, C_Send_Netin(cg->Cn(i), cg->Un(i), su));
}

inline float ConSpec::C_Compute_Dist(Connection* cn, Unit*, Unit* su) {
  float tmp = su->act - cn->wt;
  return tmp * tmp;
}
inline float ConSpec::Compute_Dist(RecvCons* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Dist(cg->Cn(i), ru, cg->Un(i)));
  return rval;
}

inline void ConSpec::Compute_Weights(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_Weights(cg->Cn(i), ru, cg->Un(i)));
  ApplyLimits(cg,ru); // ApplySymmetry(cg,ru);  don't apply symmetry during learning..
}

inline void ConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt(cg->Cn(i), ru, cg->Un(i)));
}


class PDP_API Unit_Group: public taGroup<Unit> {
  // #NO_UPDATE_AFTER ##CAT_Network a group of units
INHERITED(taGroup<Unit>)
public:
  Layer*	own_lay;	// #READ_ONLY #NO_SAVE layer owner
  PosTDCoord	pos;		// #CAT_Structure position of group relative to the layer
  bool		unique_geom;	// #DEF_false #CAT_Structure if true, this unit group has a unique geometry different from the layer geometry (otherwise, geom is always copied from lay->un_geom)
  XYNGeom	geom;		// #CONDEDIT_ON_unique_geom:true #CAT_Structure geometry of the group: layout and number of units
  bool		units_lesioned;	// #GUI_READ_ONLY if units were lesioned in this group, don't complain about rebuilding!

  int		n_units;	// #READ_ONLY #HIDDEN #NO_SAVE obsolete v3 compatibility: number of units to create in the group; updates v4 values and is reset to 0 if !=0

  virtual void	Copy_Weights(const Unit_Group* src);
  // #MENU #MENU_ON_Object #CAT_ObjectMgmt copies weights from other unit group (incl wts assoc with unit bias member)
  virtual void	SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = true);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful
  static int	SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = true);
  // #EXT_wts #COMPRESS #CAT_File skip over weight values -- rval is taMisc::ReadTagStatus = END_TAG if successful

  virtual void	SaveWeights(const String& fname="", RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="",
			    RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = true);
  // #MENU #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  virtual bool	Build();
  // #MENU #MENU_ON_Actions #CAT_Structure for subgroups: build units to specs (true if changed)
  virtual bool	CheckBuild(bool quiet=false);
  // #CAT_Structure check if network is built 
  virtual void	LayoutUnits(Unit* u = NULL);
  // #CAT_Structure for subgroups: redistribute units within the given geometry of the group
  virtual void	RecomputeGeometry();
  // #CAT_Structure re compute geometry based on parent layer

  virtual bool	SetUnitSpec(UnitSpec* unitspec);
  // #MENU #MENU_SEP_BEFORE #CAT_Structure set for all units in group
  virtual bool	SetConSpec(ConSpec* conspec);
  // #MENU #CAT_Structure set for all unit's connections in group

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)
  virtual int	LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove units with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	UnitValuesToArray(float_Array& ary, const char* variable);
  // #CAT_Structure adds values of variable from the units into the given array
  virtual bool	UnitValuesToMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure adds values of variable from the units into the given matrix
  virtual bool	UnitValuesFromArray(float_Array& ary, const char* variable);
  // #CAT_Structure sets unit values from values in the given array
  virtual bool	UnitValuesFromMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure sets unit values from values in the given array

  Unit*		FindUnitFmCoord(int x, int y);
  // #CAT_Structure find unit from given set of x and y coordinates
  Unit* 	FindUnitFmCoord(const TwoDCoord& coord)
  { return FindUnitFmCoord(coord.x,coord.y); }
  // #CAT_Structure returns unit at given coordinates within layer

  // implement save_rmv_units:
  override bool	Dump_QuerySaveChildren();

  void		RemoveAll();
  
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
  virtual int	UseCount();
  // #CAT_ObjectMgmt return number of times this spec is used

  virtual bool		CheckConfig_Layer(Layer* lay, bool quiet = false)
    {return true;} // #CAT_ObjectMgmt This is ONLY for spec-specific stuff; the layer still does all its default checking (incl child checking)

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(LayerSpec); //
};

class PDP_API Layer : public taNBase {
  // ##EXT_lay ##COMPRESS ##CAT_Network layer containing units
INHERITED(taNBase)
public:
  enum DMemDist {
    DMEM_DIST_DEFAULT,		// distribute units to different processors for distributed memory processing according to the default sequential scheme
    DMEM_DIST_UNITGP		// distribute units according to unit groups, which can be less even but allows for shared weights by unit group
  }; //
  
  enum LayerType { // type of layer, used to determine various default settings
    HIDDEN, 	// layer does not receive external input of any form
    INPUT,	// layer receives external input (EXT) that drives activation states directly
    TARGET,	// layer receives a target input (TARG) that determines correct activation states, used for training
    OUTPUT	// layer produces a visible output response but is not a target.  any external input serves as a comparison (COMP) against current activations.
  };
  
  Network*		own_net;        // #READ_ONLY #NO_SAVE #CAT_Structure Network this layer is in
  LayerType		layer_type;     // #CAT_Activation type of layer: determines default way that external inputs are presented, and helps with other automatic functions (e.g., wizards)
  PosTDCoord		pos;		// #CAT_Structure position of layer relative to the overall network position (0,0,0 is lower left hand corner)
  XYNGeom		un_geom;        // #AKA_geom #CAT_Structure two-dimensional layout and number of units within the layer or each unit group within the layer 
  bool			unit_groups;	// #CAT_Structure organize units into subgroups within the layer, with each unit group having the geometry specified by un_geom
  XYNGeom		gp_geom;	// #CONDEDIT_ON_unit_groups:true #CAT_Structure geometry of sub-groups (if unit_groups)
  PosTwoDCoord		gp_spc;		// #CONDEDIT_ON_unit_groups:true #CAT_Structure spacing between sub-groups (if unit_groups)
  XYNGeom		flat_geom;	// #EXPERT #READ_ONLY #NO_SAVE #CAT_Structure geometry of the units flattening out over unit groups (same as un_geom if !unit_groups; un_geom * gp_geom otherwise)
  XYNGeom		act_geom;	// #HIDDEN #READ_ONLY #CAT_Structure actual view geometry, includes spaces and groups and everything: the full extent of units within the layer

  Projection_Group  	projections;	// #CAT_Structure group of receiving projections
  Projection_Group  	send_prjns;	// #CAT_Structure #HIDDEN #LINK_GROUP group of sending projections
  Unit_Group		units;		// #CAT_Structure units or groups of units
  UnitSpec_SPtr 	unit_spec;	// #CAT_Structure default unit specification for units in this layer
  bool			lesion;		// #DEF_false #CAT_Structure inactivate this layer from processing (reversable)
  Unit::ExtType		ext_flag;	// #CAT_Activation #GUI_READ_ONLY #SHOW indicates which kind of external input layer received
  int_Array		sent_already; 	// #CAT_Activation #READ_ONLY #NO_SAVE array of layer addresses for coordinating sending of net input to this layer
  DMemDist		dmem_dist; 	// #CAT_DMem how to distribute units across multiple distributed memory processors

  int			n_units;
  // #HIDDEN #READ_ONLY #NO_SAVE obsolete v3 specification of number of units in layer -- do not use!!

  ProjectBase*		project(); // #IGNORE this layer's project
  	
  virtual void	Copy_Weights(const Layer* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_ObjectMgmt copies weights from other layer (incl wts assoc with unit bias member)
  virtual void	SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
			         bool quiet = true);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful
  static int	SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
			         bool quiet = true);
  // #EXT_wts #COMPRESS #CAT_File skip over weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful

  virtual void	SaveWeights(const String& fname="", RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="",
			    RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = true);
  // #MENU #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  virtual void  Build();
  // #MENU #MENU_ON_Actions #CONFIRM #CAT_Structure build the units based current geometry configuration
  virtual void	RecomputeGeometry();
  // #CAT_Structure recompute the layer's geometry specifcations
  virtual void  LayoutUnits(Unit* u=NULL);
  // #ARGC_0 #CAT_Structure layout the units according to layer geometry
  virtual void  LayoutUnitGroups();
  // #MENU #CONFIRM #CAT_Structure layout the unit groups according to layer group geometry and spacing
  virtual void  ConnectFrom(Layer* lay);
  // #DYN12N #CAT_Structure connect one or more other layers to this layer
  virtual void  Connect();
  // #MENU #CONFIRM #CAT_Structure connect the layer
  virtual bool	CheckBuild(bool quiet=false);
  // #CAT_Structure check if network is built 
  virtual bool	CheckConnect(bool quiet=false);
  // #CAT_Structure check if network is connected
  virtual void	RemoveCons();
  // #MENU #CONFIRM #MENU_SEP_BEFORE #CAT_Structure remove all connections in this layer
  virtual void	RemoveUnits();
  // #MENU #DYN1 #CAT_Structure remove all units in this layer (preserving groups)
  virtual void	RemoveUnitGroups();
  // #MENU #DYN1 #CAT_Structure remove all unit groups in this layer
  virtual void  PreConnect();
  // #CAT_Structure prepare to connect the layer (create con_groups)
  virtual void	SyncSendPrjns();
  // #CAT_Structure synchronize sending projections with the recv projections so everyone's happy
  virtual void  ReConnect_Load();
  // #IGNORE re-connect the layer after loading
  virtual void	DisConnect();
  // #MENU #CONFIRM #CAT_Structure disconnect layer from all others
  virtual int 	CountRecvCons();
  // #CAT_Structure count recv connections for all units in layer

  virtual void  Init_InputData();
  // #CAT_Activation Initializes external and target inputs
  virtual void  Init_Netin();
  // #CAT_Activation Initialize the net-input state variable(s) (prior to Send_Netin)
  virtual void  Init_Acts();
  // #CAT_Activation Initialize the unit state variables
  virtual void  Init_dWt();
  // #CAT_Learning Initialize the weight change variables
  virtual void  Init_Weights();
  // #MENU #CONFIRM #CAT_Learning Initialize the weights
  virtual void	Init_Weights_post();
  // #IGNORE run after init wt state (ie. to scale wts..)

  void		SetExtFlag(int flg)   { ext_flag = (Unit::ExtType)(ext_flag | flg); }
  // #CAT_Activation set external input data flag
  void		UnSetExtFlag(int flg) { ext_flag = (Unit::ExtType)(ext_flag & ~flg); }
  // #CAT_Activation un-set external input data flag

  virtual void	ApplyInputData(taMatrix* data, Unit::ExtType ext_flags = Unit::NO_EXTERNAL,
    Random* ran = NULL, const PosTwoDCoord* offset = NULL);
  // #CAT_Activation apply the 2d or 4d external input pattern to the network, optional random additional values, and offsetting; uses a flat 2-d model where grouped layer or 4-d data are flattened to 2d; frame<0 means from end

  virtual void	Compute_Netin();
  // #CAT_Activation Compute NetInput -- weighted activations from other units
  virtual void	Send_Netin();
  // #CAT_Activation sender-based compute net-input sending to all layers
  virtual void	Send_NetinToLay(Layer* tolay);
  // #CAT_Activation sender-based compute net-input sending only to tolay layer
  virtual void	Send_NetinToMe();
  // #CAT_Activation tell all layers that I receive from to send their net input to me
  virtual void	Compute_Act();
  // #CAT_Activation compute activation: the value I send to other units
  virtual void	Compute_dWt();
  // #CAT_Learning compute weight changes: the essence of learning
  virtual void	Compute_Weights();
  // #CAT_Learning update weights based on previous weight changes

  virtual float	Compute_SSE();
  // #CAT_Statistic compute sum squared error of activation vs targt over the entire layer

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	ProbAddCons(float p_add_con, float init_wt = 0.0);
  // #MENU #USE_RVAL #CAT_Structure probabilistically add new connections (assuming prior pruning), init_wt = initial weight value of new connection
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connectiosn with prob p_lesion (permute = fixed no. lesioned)
  virtual int	LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove units with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	SetLayerSpec(LayerSpec* layspec);
  // #MENU #MENU_SEP_BEFORE #CAT_Structure set the layer specification
  virtual LayerSpec* GetLayerSpec()		{ return (LayerSpec*)NULL; }
  // #CAT_Structure get the layer spec for this layer (if used)
  virtual bool	SetUnitSpec(UnitSpec* unitspec);
  // #MENU #CAT_Structure set for all units in layer
  virtual void	SetUnitType(TypeDef* td);
  // #MENU #TYPE_Unit #CAT_Structure set unit type for all units in layer (created by Build)
  virtual bool	SetConSpec(ConSpec* conspec);
  // #MENU #CAT_Structure set for all unit's connections in layer
  virtual bool	CheckTypes(bool quiet=false);
  // #MENU #USE_RVAL #CAT_Structure check that the object and spec types are all ok
  virtual void	FixPrjnIndexes();
  // #MENU #CAT_Structure fix the projection indicies of the connection groups (other_idx)

  virtual int	ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // #CAT_Structure switch any units/layers using old_sp to using new_sp
  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int	ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp
  virtual int	ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp);
  // #CAT_Structure switch any layers using old_sp to using new_sp

//obs  virtual void	GridViewWeights(GridLog* grid_log, Layer* send_lay, bool use_swt=false, int un_x=-1, int un_y=-1, int wt_x=-1, int wt_y=-1);
  /* #MENU #MENU_SEP_BEFORE #NULL_OK display entire set of weights from sending layer (use sending weights if use_swt) in grid log, -1 for x,y = use layer geometry
     (otherwise limits range, un= unit range, wt = weight (sending lay) range) */
  virtual void	WeightsToTable(DataTable* dt, Layer* send_lay);
  // #MENU #NULL_OK #CAT_Structure TODO:define send entire set of weights from sending layer to given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit

  Unit*		FindUnitFmCoord(int x, int y);
  // #CAT_Structure find unit within layer from given coordinates (layer relative coords), taking into account group geometry if present (subtracts any gp_spc -- as if it is not present).
  Unit*		FindUnitFmCoord(const TwoDCoord& coord) {return FindUnitFmCoord(coord.x, coord.y);}
  // #CAT_Structure get unit from coordinates, taking into account group geometry if present (subtracts any gp_spc -- as if it is not present).
  Unit*		FindUnitFmGpCoord(int gp_x, int gp_y, int un_x, int un_y);
  // #CAT_Structure find unit given both unit and unit group coordinates
  Unit*		FindUnitFmGpCoord(const TwoDCoord& gp_coord, const TwoDCoord& coord)
    {return FindUnitFmGpCoord(gp_coord.x,gp_coord.y, coord.x, coord.y);}
  // #CAT_Structure get unit from both unit and group coordinates
  Unit_Group* 	FindUnitGpFmCoord(int gp_x, int gp_y);
  // #CAT_Structure find the unit group at given group coordiantes (i.e., within gp_geom, not unit coordinates)
  Unit_Group* 	FindUnitGpFmCoord(const TwoDCoord& coord) {return FindUnitGpFmCoord(coord.x,coord.y);}
  // #CAT_Structure get unit group from group coordinates (i.e., within gp_geom, not unit coordinates)
  virtual void	GetActGeomNoSpc(PosTwoDCoord& nospc_geom);
  // #CAT_Structure get the actual geometry of the layer, subtracting any gp_spc that might be present (as if there were no spaces between unit groups)

  void		SetDefaultPos();
  // #IGNORE initialize position of layer

  virtual void	CopyNetwork(Network* net, Network* cn, Layer* cp);
  // #IGNORE copy entire network
  
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
  virtual void		ApplyLayerFlags(Unit::ExtType act_ext_flags);
  // #IGNORE set layer flag to reflect the kind of input received
  virtual void		ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
					       Random* ran, const TwoDCoord& offs);
  // #IGNORE 2d data is always treated the same: FindUnitFmCoord deals with unit grouping
  virtual void		ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
					       Random* ran, const TwoDCoord& offs);
  // #IGNORE flat layer, 4d data
  virtual void		ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags,
					     Random* ran);
  // #IGNORE grouped layer, 4d data -- note this cannot have offsets..

  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  // #IGNORE this is the guy that *additionally* delegates to the Spec
  override void		CheckChildConfig_impl(bool quiet, bool& rval);// #IGNORE 
};

class PDP_API Layer_Group : public taGroup<Layer> {
  // ##CAT_Network group of layers
  INHERITED(taGroup<Layer>)
public:
  TDCoord	pos;		// Position of Group of layers relative to network

  void	Initialize() 		{ };
  void 	Destroy()		{ };
  void	InitLinks()		{ inherited::InitLinks(); taBase::Own(pos,this); }
  void  Copy_(const Layer_Group& cp)	{ pos = cp.pos; }
  COPY_FUNS(Layer_Group, inherited);
  TA_BASEFUNS(Layer_Group);
};

SmartRef_Of(Layer); // LayerRef

class PDP_API Network : public taFBase {
  // ##FILETYPE_Network ##EXT_net ##COMPRESS ##CAT_Network A network, containing layers, units, etc..
INHERITED(taFBase)
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
    BINARY,			// weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
    NET_FMT,			// use format specified on the network
  };
  
  enum WtUpdate {
    ON_LINE,			// update weights on-line (after every event) -- this is not viable for dmem processing across trials and is automatically switched to small_batch in that case
    SMALL_BATCH, 		// update weights every small_batch_n trials
    BATCH			// update weights in batch mode (after every epoch)
  };

  enum TrainMode {
    TEST,		 	// network is only being tested; no learning should occur
    TRAIN	  		// network is being trained: learning should occur
  };
 
  BaseSpec_Group specs; 	// #CAT_Structure Specifications for network parameters
  Layer_Group	layers;		// #CAT_Structure Layers or Groups of Layers
  TrainMode	train_mode;	// #CAT_Learning t
  WtUpdate	wt_update;	// #CAT_Learning weight update mode: when are weights updated
  int		small_batch_n;	// #CONDEDIT_ON_wt_update:SMALL_BATCH #CAT_Learning number of events for small_batch learning mode (specifies how often weight changes are synchronized in dmem)
  int		small_batch_n_eff; // #GUI_READ_ONLY #NO_SAVE #CAT_Learning effective batch_n value = batch_n except for dmem when it = (batch_n / epc_nprocs) >= 1

  int		batch;		// #GUI_READ_ONLY #SHOW #CAT_Counter batch counter: number of times network has been trained over a full sequence of epochs (updated by program)
  int		epoch;		// #GUI_READ_ONLY #SHOW #CAT_Counter epoch counter: number of times a complete set of training patterns has been presented (updated by program)
  int		trial;		// #GUI_READ_ONLY #SHOW #CAT_Counter trial counter: number of external input patterns that have been presented in the current epoch (updated by program)
  int		cycle;		// #GUI_READ_ONLY #SHOW #CAT_Counter cycle counter: number of iterations of activation updating (settling) on the current external input pattern (updated by program)	
  float		time;		// #GUI_READ_ONLY #SHOW #CAT_Counter the current time, relative to some established starting point, in algorithm-specific units (often miliseconds)
  String	group_name;	// #GUI_READ_ONLY #SHOW #CAT_Counter name associated with the current group of trials, if such a grouping is applicable
  String	trial_name;	// #GUI_READ_ONLY #SHOW #CAT_Counter name associated with the current trial (e.g., name of input pattern)

  float		sse;		// #GUI_READ_ONLY #SHOW #CAT_Statistic sum squared error over the network, for the current external input pattern
  float		sum_sse;	// #GUI_READ_ONLY #SHOW #CAT_Statistic total sum squared error over an epoch or similar larger set of external input patterns
  float		avg_sse;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average sum squared error over an epoch or similar larger set of external input patterns
  float		cnt_err_tol;	// #CAT_Statistic tolerance for computing the count of number of errors over current epoch
  float		cnt_err;	// #GUI_READ_ONLY #SHOW #CAT_Statistic count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns

  float	       	cur_sum_sse;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current sum_sse -- used during computation of sum_sse
  int	       	avg_sse_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic number of times cur_sum_sse updated: for computing avg_sse
  float	       	cur_cnt_err;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current cnt_err -- used for computing cnt_err

  DMem_SyncLevel dmem_sync_level; // #CAT_DMem at what level of network structure should information be synchronized across processes?
  int		dmem_nprocs;	// #CAT_DMem number of processors to use in distributed memory computation of connection-level processing (actual number may be less, depending on processors requested!)
  int		dmem_nprocs_actual; // #READ_ONLY #NO_SAVE actual number of processors being used

  Usr1SaveFmt	usr1_save_fmt;	// #CAT_File save network for -USR1 signal: full net or weights
  WtSaveFormat	wt_save_fmt;	// #CAT_File format to save weights in if saving weights
  LayerLayout	lay_layout;	// #CAT_Display Visual mode of layer position/view

  int		n_units;	// #READ_ONLY #DETAIL #CAT_Structure total number of units in the network
  int		n_cons;		// #READ_ONLY #DETAIL #CAT_Structure total number of connections in the network
  PosTDCoord	max_size;	// #READ_ONLY #DETAIL #CAT_Structure maximum size in each dimension of the net

  bool		copying; 	// #IGNORE if object is currently being copied
  ProjectBase*	proj;		// #IGNORE ProjectBase this network is in

  // todo: these seem pretty lame:
  virtual int	GetDefaultX();  // #IGNORE 
  virtual int	GetDefaultY();  // #IGNORE
  virtual int	GetDefaultZ();  // #IGNORE
#ifdef TA_GUI
  virtual void	ShowInViewer(T3DataViewFrame* fr = NULL);
  // #NULL_OK #MENU #MENU_SEP_BEFORE #MENU_CONTEXT #CAT_Display open a viewer on this network (NULL=use blank if any, else make new frame)
#endif
  virtual void	Copy_Weights(const Network* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_ObjectMgmt copies weights from other network (incl wts assoc with unit bias member)
  virtual void	SaveWeights_strm(ostream& strm, WtSaveFormat fmt = NET_FMT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual bool	LoadWeights_strm(istream& strm, bool quiet = true);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (fmt is read from file)
  virtual void	SaveWeights(const String& fname="", WtSaveFormat fmt = NET_FMT);
  // #MENU #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual bool	LoadWeights(const String& fname="", bool quiet = true);
  // #MENU #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (fmt is read from file) (leave fname empty to pull up file chooser)

  virtual void  Build();
  // #MENU #MENU_ON_Actions #CAT_Structure Build the network according to geometry
  virtual void	Connect();
  // #MENU #MENU_ON_Actions #CAT_Structure Connect this network according to projections on Layers
  virtual bool	CheckBuild(bool quiet=false);
  // #CAT_Structure check if network is built 
  virtual bool	CheckConnect(bool quiet=false);
  // #CAT_Structure check if network is connected
  virtual bool	CheckTypes(bool quiet=false);
  // #MENU #MENU_ON_Actions #USE_RVAL #MENU_SEP_BEFORE #CAT_ObjectMgmt check that the object and spec types are all ok

  virtual void	UpdtAfterNetMod();
  // #CAT_ObjectMgmt update network after any network modification (calls appropriate functions)
  virtual void	SyncSendPrjns();
  // #CAT_Structure synchronize sending projections with the recv projections so everyone's happy
  virtual void 	CountRecvCons();
  // #CAT_Structure count recv connections for all units in network

  virtual void	ConnectUnits(Unit* u_to, Unit* u_from=NULL, bool record=true,
			     ConSpec* conspec=NULL);
  // #CAT_Structure connect u1 so that it recieves from u2. Create projection if necessary

  virtual void	RemoveCons();
  // #MENU #MENU_ON_Actions #CONFIRM #MENU_SEP_BEFORE #CAT_Structure Remove all connections in network
  virtual void	RemoveUnits();
  // #MENU #CONFIRM #CAT_Structure Remove all units in network (preserving unit groups)
  virtual void	RemoveUnitGroups();
  // #MENU #CONFIRM #CAT_Structure Remove all unit groups in network
  virtual void	LayoutUnitGroups();
  // #MENU #CONFIRM #CAT_Structure layout all the layer's unit groups according to layer group geometry and spacing
  virtual void	PreConnect();
  // #CAT_Structure Prepare to connect this network (make con_groups)
  virtual void	ReConnect_Load();
  // #IGNORE ReConnect network after loading
  virtual void	CopyNetwork(Network* net);
  // #IGNORE copy entire network

  virtual void	FixPrjnIndexes();
  // #CAT_Structure fix the projection indicies of the connection groups (other_idx)

  virtual void	RemoveMonitors();
  // #CAT_ObjectMgmt Remove monitoring of all objects in all processes associated with parent project
  virtual void	UpdateMonitors();
  // #CAT_ObjectMgmt Update monitoring of all objects in all processes associated with parent project

  virtual void  Init_InputData();
  // #CAT_Activation Initializes external and target inputs
  virtual void  Init_Netin();
  // #CAT_Activation Initialize the netinput variable(s) (prior to Send_Netin)
  virtual void  Init_Acts();
  // #MENU #MENU_SEP_BEFORE #CAT_Activation initialize the unit activation state variables
  virtual void  Init_dWt();
  // #CAT_Learning Initialize the weight change variables
  virtual void  Init_Weights();
  // #MENU #CONFIRM #CAT_Learning Initialize the weights -- also inits acts, counters and stats
  virtual void	Init_Weights_post();
  // #IGNORE run after init weight state (ie. to scale weights)

  virtual void	Init_Counters();
  // #CAT_Counter initialize all counter variables on network (called in Init_Weights; except batch because that loops over inits!)
  virtual void	Init_Stats();
  // #CAT_Statistic initialize statistic variables on network

  virtual void	Compute_Netin();
  // #CAT_Activation Compute NetInput: weighted activation from other units
  virtual void	Send_Netin();
  // #CAT_Activation sender-based computation of net input: weighted activation from other units
  virtual void	Compute_Act() { Compute_Act_default(); }
  // #CAT_Activation Compute Activation -- subtypes may replace with their own algorithm
  virtual void	Compute_Act_default();
  // #CAT_Activation default version, may be replaced or extended

  virtual void	Compute_dWt();
  // #CAT_Learning compute weight changes -- the essence of learning

  virtual bool	Compute_Weights_Test(int trial_no);
  // #CAT_Learning check to see if it is time to update the weights based on the given number of completed trials (typically trial counter + 1): if ON_LINE, always true; if SMALL_BATCH, only if trial_no % batch_n_eff == 0; if BATCH, never (check at end of epoch and run then)
  virtual void	Compute_Weights();
  // #CAT_Learning update weights for whole net: calls DMem_SumDWts before doing update if in dmem mode
  virtual void	Compute_Weights_impl();
  // #CAT_Learning just the weight update routine: update weights from delta-weight changes

  virtual void	Compute_SSE();
  // #CAT_Statistic compute sum squared error of activations vs targets over the entire network
  virtual void	Compute_TrialStats();
  // #CAT_Statistic compute trial-level statistics (SSE and others defined by specific algorithms)
  virtual void	DMem_ShareTrialData(DataTable* dt, int n_rows = 1);
  // #CAT_DMem share trial data from given datatable across the trial-level dmem communicator (outer loop) -- each processor gets data from all other processors; if called every trial with n_rows = 1, data will be identical to non-dmem; if called at end of epoch with n_rows = -1 data will be grouped by processor but this is more efficient

  virtual void	Compute_EpochSSE();
  // #CAT_Statistic compute epoch-level sum squared error and related statistics
  virtual void	Compute_EpochStats();
  // #CAT_Statistic compute epoch-level statistics; calls DMem_ComputeAggs (if dmem) and EpochSSE -- specific algos may add more

  virtual void	LayerZPos_Add(int add_to_z = 1);
  // #MENU #MENU_SEP_BEFORE #CAT_Structure Add add_to_z to layer vertical positions in proportion to current positions:\n new layer.pos.z += layer.pos.z * add_to_z -- makes display look better \n -- negative values will subtract or compact the layers
  virtual void	LayerZPos_Unitize();
  // #CAT_Structure set layer z axis positions to unitary increments (0, 1, 2.. etc)
  virtual void	LayerZPos_Auto(float y_mult_factor = .5f);
  // #MENU #CAT_Structure auto stretch out z positions in proportion to the maximum y axis size of the network

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     CountParam::Relation rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	ProbAddCons(float p_add_con, float init_wt = 0.0);
  // #MENU #USE_RVAL #CAT_Structure probabilistically add new connections (assuming prior pruning), init_wt = initial weight value of new connection
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)
  virtual int	LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove units with prob p_lesion (permute = fixed no. lesioned)

  virtual void	TwoD_Or_ThreeD(LayerLayout layout_type);
  // #MENU #MENU_SEP_BEFORE #CAT_Display Set 2d or 3d and reposition and redraw layers

//obs  virtual void	GridViewWeights(GridLog* grid_log, Layer* recv_lay, Layer* send_lay, bool use_swt=false, int un_x=-1, int un_y=-1, int wt_x=-1, int wt_y=-1);
  /* #MENU #MENU_SEP_BEFORE #NULL_OK display entire set of weights from sending layer to recv_lay (use sending weights if use_swt) in grid log, -1 for x,y = use layer geometry
     (otherwise limits range, un= unit range, wt = weight (sending lay) range) */
  virtual void	WeightsToTable(DataTable* dt, Layer* recv_lay, Layer* send_lay);
  // #MENU #NULL_OK #CAT_Structure send entire set of weights from sending layer to recv layer in given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit

  virtual void	ReplaceSpecs(BaseSpec* old_sp, BaseSpec* new_sp);
  // #CAT_Structure replace a spec of any kind, including iterating through any children of that spec
  virtual void	ReplaceSpecs_Gp(const BaseSpec_Group& old_spg, BaseSpec_Group& new_spg);
  // #CAT_Structure replace a specs on two matching spec groups, including iterating through any children of each spec
  virtual int	ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // #CAT_Structure switch any units/layers using old_sp to using new_sp
  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int	ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp
  virtual int	ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp);
  // #CAT_Structure switch any layers using old_sp to using new_sp

  // wizard construction functions:
  virtual BaseSpec_Group* FindMakeSpecGp(const char* nm, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a given spec group and if not found, make it
  virtual BaseSpec* FindMakeSpec(const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a given spec and if not found, make it
  virtual BaseSpec* FindSpecName(const char* nm);
  // #CAT_Structure find a given spec by name
  virtual BaseSpec* FindSpecType(TypeDef* td);
  // #CAT_Structure find a given spec by type

  virtual Layer* FindMakeLayer(const char* nm, TypeDef* td = NULL, bool& nw_itm = nw_itm_def_arg, const char* alt_nm = NULL);
  // #CAT_Structure find a given layer and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
  virtual Layer* FindLayer(const char* nm) { return (Layer*)layers.FindName(nm); }
  // #CAT_Structure find layer by name
  virtual Projection* FindMakePrjn(Layer* recv, Layer* send, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a projection between two layers using given specs, make it if not found; if existing prjn between layers exists, it will be modified with current specs
  virtual Projection* FindMakePrjnAdd(Layer* recv, Layer* send, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a projection between two layers using given specs, make it if not found; if existing prjn between layers exists but has diff specs, a new prjn is made
  virtual Projection* FindMakeSelfPrjn(Layer* recv, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a self projection using given specs, make it if not found; if existing self prjn exists, it will be modified with current specs
  virtual Projection* FindMakeSelfPrjnAdd(Layer* recv, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a self projection using given specs, make it if not found; if existing self prjn exists but has diff specs, a new prjn is made
  virtual bool   RemovePrjn(Layer* recv, Layer* send, ProjectionSpec* ps = NULL, ConSpec* cs = NULL);
  // #CAT_Structure remove a projection between two layers, if it exists
  virtual bool   RemoveLayer(const char* nm) { return layers.RemoveName(nm); }
  // #CAT_Structure remove layer with given name, if it exists

  virtual void	FixLayerViews(Layer* lay=NULL);
  // #IGNORE update the layer on all the nets views

  virtual void	UpdateMax();	// #IGNORE

  virtual void	SetProjectionDefaultTypes(Projection* prjn);
  // #IGNORE this is called by the projection InitLinks to set its default types: overload in derived algorithm-specific networks to provide appropriate default types

#ifdef DMEM_COMPILE
  DMemComm	dmem_net_comm;	// #IGNORE the dmem communicator for the network-level dmem computations (the inner subgroup of units, each of size dmem_nprocs_actual)
  DMemComm	dmem_trl_comm;	// #IGNORE the dmem communicator for the trial-level (each node processes a different set of trials) -- this is the outer subgroup
  DMemShare 	dmem_share_units;    	// #IGNORE the shared units
  DMemAggVars	dmem_agg_sum;		// #IGNORE aggregation of network variables using SUM op (currently only OP in use -- add others as needed)
  virtual void	DMem_SyncNRecvCons();
  // syncronize number of receiving connections (share set 0)
  virtual void	DMem_SyncNet();
  // #IGNORE synchronize just the netinputs (share set 1)
  virtual void	DMem_SyncAct();
  // #IGNORE synchronize just the activations (share set 2)
  virtual void 	DMem_DistributeUnits();
  // #CAT_DMem distribute units to different nodes
  virtual void 	DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void 	DMem_PruneNonLocalCons();
  // #IGNORE prune non-local connections from all units: units only have their own local connections
  virtual void  DMem_SumDWts(MPI_Comm comm);
  // #IGNORE sync weights across trial-level dmem by summing delta-weights across processors (prior to computing weight updates)
  virtual void  DMem_AvgWts(MPI_Comm comm);
  // #IGNORE sync weights across trial-level dmem by averaging weight values across processors (this is not mathematically equivalent to anything normally done, but it may be useful in some situations)
  virtual void	DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network variables across procs for trial-level dmem 
  virtual void	DMem_SymmetrizeWts();
  // #IGNORE symmetrize the weights (if necessary) by sharing weight values across processors
#else
  virtual void	DMem_SyncNRecvCons() { };
  // #CAT_DMem syncronize number of receiving connections (share set 0)
  virtual void 	DMem_DistributeUnits() { };
  // #CAT_DMem distribute units to different nodes
#endif

//TODO???  TypeDef*	GetDefaultView() { return &TA_NetView; } // #IGNORE default view
  int		Dump_Load_Value(istream& strm, taBase* par=NULL);
  override int 	Save_strm(ostream& strm, TAPtr par=NULL, int indent=0);

  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void 	Copy_(const Network& cp);
  COPY_FUNS(Network, inherited);
  TA_BASEFUNS(Network);
  
protected:
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void	CheckChildConfig_impl(bool quiet, bool& rval);
#ifdef TA_GUI
protected:
//  override taiDataLink*	ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type);
#endif
};

SmartRef_Of(Network)

class PDP_API Network_Group : public taGroup<Network> {
  // ##FILETYPE_Network ##EXT_net ##COMPRESS ##CAT_Network a group of networks
INHERITED(taGroup<Network>)
public:
  
  void	Initialize() 		{SetBaseType(&TA_Network);}
  void 	Destroy()		{ };
  TA_BASEFUNS(Network_Group); //
};

#endif /* netstru_h */
