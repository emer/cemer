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



// netstru.h

#ifndef netstru_h
#define netstru_h

#include "ta_group.h"
#include "ta_math.h"
#include "ta_geometry.h"
#include "minmax.h"
#include "ta_time.h"
#include "ta_dmem.h"
#include "ta_thread.h"
#include "ta_datatable.h"

#include "emergent_base.h"
#include "spec.h"

// pre-declare:
class Connection;
class ConSpec;
class BaseCons;
class RecvCons;
class RecvCons_List;
class SendCons;
class SendCons_List;
class UnitSpec;
class Unit;
SmartRef_Of(Unit,TA_Unit); // UnitRef
class UnitPtrList;
class ProjectionSpec;
class Projection;
SmartRef_Of(Projection,TA_Projection); // ProjectionRef
class Unit_Group;
class Layer;
SmartRef_Of(Layer,TA_Layer); // LayerRef
class Network;
SmartRef_Of(Network,TA_Network); // NetworkRef

// externals
class DataTable;
class NetMonitor;
class NetView; //

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

class EMERGENT_API SigmoidSpec : public taOBase {
// ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##CAT_Math Specifies a Sigmoid 1 / [1 + exp(-(x - off) * gain)]
INHERITED(taOBase) //
public:
#if (defined(_MSC_VER) && !defined(__MAKETA__))
  // msvc doesn't allow floats here, and doesn't seem to handle static const right
  static const float SIGMOID_MAX_VAL; // #READ_ONLY #HIDDEN max eval value
  static const float SIGMOID_MIN_VAL; // #READ_ONLY #HIDDEN min eval value
  static const float SIGMOID_MAX_NET; // #READ_ONLY #HIDDEN maximium net input value
#else
  static const float SIGMOID_MAX_VAL = 0.999999f; // #READ_ONLY #HIDDEN max eval value
  static const float SIGMOID_MIN_VAL = 0.000001f; // #READ_ONLY #HIDDEN min eval value
  static const float SIGMOID_MAX_NET = 13.81551f; // #READ_ONLY #HIDDEN maximium net input value
#endif
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

  SIMPLE_COPY(SigmoidSpec);
  TA_BASEFUNS(SigmoidSpec);
private:
  void 	Initialize()		{ off = 0.0f; gain = 1.0f; }
  void	Destroy()		{ };
};


class EMERGENT_API SchedItem : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Network one element of a schedule
INHERITED(taOBase)
public:
  int		start_ctr;	// ctr number to start at for this item
  float		start_val;	// starting value for item
  int		duration;	// #HIDDEN duration (from start_ctr)
  float		step;		// #HIDDEN linear step to take for each increment of ctr

  float		GetVal(int ctr)  { return start_val + step * (float)(ctr - start_ctr); }
  // get value for given ctr value

  override String	GetDesc() const;

  TA_SIMPLE_BASEFUNS(SchedItem);
private:
  void	Initialize();
  void	Destroy() 	{ };
};

class EMERGENT_API Schedule : public taList<SchedItem> {
  // ##CAT_Network #NO_EXPAND_ALL A schedule for parameters that vary over time
INHERITED(taList<SchedItem>)
public:
  int 		last_ctr;	// the last counter index called
  float		default_val;	// the default if group is empty
  bool		interpolate;	// use linear interpolation between points
  float		cur_val;	// #READ_ONLY the current val

  float		GetVal(int ctr);
  // #MENU #MENU_ON_Edit #USE_RVAL get current schedule val, based on counter

  TA_SIMPLE_BASEFUNS(Schedule);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};


// the connection is managed fully by the ConSpec and the RecvCons
// don't put any functions on the connection itself

// note: v4.0 connection stuff is not backwards compatible with v3.2

// note: connection must be just a bag of bits with *no* functions of its own
// especially no virtual functions!  it is managed with raw bit copy ops
// although the sending connections have pointers to connection objects
// it is up to the connection management code to ensure that when a
// connection is removed, its associated sending link is also removed

class EMERGENT_API Connection {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network base connection between two units
public:
  float 	wt;		// #VIEW_HOT weight of connection
  float		dwt;		// #VIEW #NO_SAVE resulting net weight change

  Connection() { wt = dwt = 0.0f; }
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

class EMERGENT_API WeightLimits : public taOBase {
  // ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##CAT_Network specifies weight limits for connections
INHERITED(taOBase)
public:
  enum LimitType {
    NONE,			// no weight limitations
    GT_MIN,			// constrain weights to be greater than min value
    LT_MAX,			// constrain weights to be less than max value
    MIN_MAX 			// constrain weights to be within min and max values
  };
  LimitType	type;		// type of weight limitation to impose
  float		min;		// #CONDSHOW_OFF_type:NONE,LT_MAX minimum weight value (if applicable)
  float		max;		// #CONDSHOW_OFF_type:NONE,GT_MIN maximum weight value (if applicable)
  bool		sym;		// if true, also symmetrize with reciprocal connections

  void 	ApplyMinLimit(float& wt)	{ if(wt < min) wt = min; }
  void 	ApplyMaxLimit(float& wt)	{ if(wt > max) wt = max; }

  void	ApplyLimits(float& wt)
  { if(type == GT_MIN) 		ApplyMinLimit(wt);
    else if(type == LT_MAX)	ApplyMaxLimit(wt);
    else if(type == MIN_MAX)	{ ApplyMinLimit(wt); ApplyMaxLimit(wt); } }

  override String 	GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(WeightLimits);
  TA_BASEFUNS(WeightLimits);
private:
  void 	Initialize()		{ type = NONE; min = -1.0f; max = 1.0f; sym = false; }
  void	Destroy()		{ };
};

class EMERGENT_API ConSpec: public BaseSpec {
  // ##CAT_Spec Connection specs: for processing over a set of connections all from the same projection 
INHERITED(BaseSpec)
public:
  RandomSpec	rnd;		// #CAT_ConSpec Weight randomization specification.  Note that NONE means no value at all, not the mean, and should be used if some other source is setting the weights, e.g., from a projectionspec or loading from a file etc
  WeightLimits	wt_limits;	// #CAT_ConSpec limits on weight sign, symmetry

  inline void		C_ApplyLimits(Connection* cn, Unit*, Unit*)
  { wt_limits.ApplyLimits(cn->wt); }
  // #CAT_Learning apply weight limits to single connection
  inline virtual void	ApplyLimits(RecvCons* cg, Unit* ru);
  // #CAT_Learning apply weight limits (sign, magnitude)

  virtual void		ApplySymmetry(RecvCons* cg, Unit* ru);
  // #CAT_Learning apply weight symmetrizing between reciprocal units

  ////////////////////////////////////////////////////////////////////////////////
  //	Below are the primary computational interface to the Network Objects
  //	for performing algorithm-specific activation and learning

  inline virtual void	C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su);
  // #CAT_Learning initialize weight state variables (ie. at beginning of training)
  inline virtual void	C_AddRndWeights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su);
  // #CAT_Learning add random noise to existing weight variables -- for add_rnd_wts after prjn spec init_wts based initialization
  inline virtual void 	Init_Weights(RecvCons* cg, Unit* ru);
  // #CAT_Learning initialize weight state variables (ie. at beginning of training)
  inline virtual void	C_Init_Weights_post(BaseCons*, Connection*, Unit*, Unit*) { };
  // #IGNORE
  inline virtual void 	Init_Weights_post(BaseCons* cg, Unit* ru);
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)
  inline virtual void 	C_Init_dWt(RecvCons*, Connection* cn, Unit*, Unit*)
  { cn->dwt=0.0f; }
  // #CAT_Learning initialize weight-change variables on a single connecdtion
  inline virtual void 	Init_dWt(RecvCons* cg, Unit* ru);
  // #CAT_Learning initialize weight-change variables for whole set

  inline float 		C_Compute_Netin(Connection* cn, Unit* ru, Unit* su);
  inline virtual float 	Compute_Netin(RecvCons* cg, Unit* ru);
  // #CAT_Activation compute net input for weights in this con group

  inline void 		C_Send_Netin(Connection* cn, float* send_netin_vec, Unit* ru,
				     float su_act);
  inline virtual void 	Send_Netin(SendCons* cg, Network* net, int thread_no, Unit* su);
  // #CAT_Activation sender-based net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units

  inline float 		C_Compute_Dist(Connection* cn, Unit* ru, Unit* su);
  inline virtual float 	Compute_Dist(RecvCons* cg, Unit* ru);
  // #CAT_Activation compute net distance for con group (ie. euclidean distance)
  inline void		C_Compute_dWt(Connection*, Unit*, Unit*)	{ };
  inline virtual void	Compute_dWt(RecvCons* cg, Unit* ru);
  // #CAT_Learning compute the delta-weight change
  inline void 		C_Compute_Weights(Connection*, Unit*, Unit*)	{ };
  inline virtual void 	Compute_Weights(RecvCons* cg, Unit* ru);
  // #CAT_Learning update weights (ie. add delta-wt to wt, zero delta-wt)

  ////////////////////////////////////////////////////////////////////////////////
  //	The following are misc functionality not required for primary computing

  virtual bool  	CheckConfig_RecvCons(RecvCons* cg, bool quiet=false);
  // check for for misc configuration settings required by different algorithms

  virtual  void  Init_Weights_Net();
  // #BUTTON #CAT_Learning initializes all weights in the network

  virtual bool	 DMem_AlwaysLocal() { return false; }
  // #CAT_DMem overload this function to prevent this projection from being pruned for non-local units under dmem processing (for "special" connection types)

  override String 	GetTypeDecoKey() const { return "ConSpec"; }

  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ConSpec& cp);
  TA_BASEFUNS(ConSpec);
protected:
  SPEC_DEFAULTS;
  override bool 	CheckType_impl(TypeDef* td);
  override bool		CheckObjectType_impl(taBase* obj); // don't do checking on 1st con group in units
private:
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	Defaults_init() 	{ };
};

SpecPtr_of(ConSpec);


//////////////////////////////////////////////////////////////////////////////////////
//	Base Cons -- fully integrated connection management with dynamic con ownership

class EMERGENT_API BaseCons : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE #VIRT_BASE ##CAT_Network base connection manager class -- manages one projection's worth of connections at a unit level -- inherited by RecvCons and SendCons
INHERITED(taOBase)
public:
  // note: follwing must be coordinated with the Network enum
  enum WtSaveFormat {
    TEXT,			// weights are saved as ascii text representation of digits (completely portable)
    BINARY,			// weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
  };

  // note: following use base_flags so have high values to avoid conflicts
  enum BaseConsFlags {
    OWN_CONS = 0x01000000,	// this guy owns the connections -- else gets links to others
    RECV_CONS = 0x02000000,	// we are a recv con group -- else a send con group
  };

  int 		size;		// #CAT_Structure #READ_ONLY #NO_SAVE #SHOW number of connections currently active
  int		alloc_size;	// #CAT_Structure #READ_ONLY #NO_SAVE #SHOW allocated size -- no more than this number of connections may be created -- it is a hard limit set by the alloc function
  TypeDef*	con_type;  	// #CAT_Structure #READ_ONLY #SHOW type of connection objects to make -- this can only be set before things have been allocated
  Projection*	prjn; 		// #CAT_Structure #READ_ONLY #SHOW #NO_SET_POINTER pointer the the projection which created this Group
  int		other_idx;      // #CAT_Structure #READ_ONLY #SHOW index into other direction's list of cons objects (i.e., send_idx for RecvCons and recv_idx for SendCons)

protected:
  ConSpec* 	m_con_spec;	// con spec that we use: controlled entirely by the projection!

  int		con_size;	// size of the connection object -- set if we own cons and built them
#ifndef __MAKETA__
  union {
    char*		cons_own;	// if we own the cons, this is their physical memory: alloc_size * con_size
    Connection**	cons_ptr;	// if we don't own the cons, these are pointers to the cons (alloc_size)
  };
#endif
  Unit**		units;		// list of units on the other side of the connection, in index association with the connections

public:

  ////////////////////////////////////////////////////////////////////////////////
  //	Primary infrastructure management routines

  bool	OwnCons() const	{ return HasBaseFlag(OWN_CONS); }
  // #CAT_Structure do we own the connections?  else just point to them
  bool	PtrCons() const	{ return !HasBaseFlag(OWN_CONS); }
  // #CAT_Structure do we have pointers to connections?  else we own them
  bool	IsRecv() const	{ return HasBaseFlag(RECV_CONS); }
  // #CAT_Structure is this a receiving con group?  else sending
  bool	IsSend() const	{ return !HasBaseFlag(RECV_CONS); }
  // #CAT_Structure is this a sending con group?  else receiving

  inline bool		InRange(int idx) const {return ((idx < size) && (idx >= 0));}
  // #CAT_Access is index in range?

#ifdef DEBUG
  inline Connection*	OwnCn(int idx) const
  { if(!OwnCons()) { taMisc::Error("owncn err"); return NULL; } 
    return (Connection*)&(cons_own[con_size * idx]); }
#else
  inline Connection*	OwnCn(int idx) const { return (Connection*)&(cons_own[con_size * idx]); }
#endif
  // #CAT_Access fast access (no range or own_cons checking) to owned connection at given index, consumer must cast to appropriate type (for type safety, check con_type) -- compute algorithms should use this, as they know whether the connections are owned
#ifdef DEBUG
  inline Connection*	PtrCn(int idx) const
  { if(!PtrCons()) { taMisc::Error("ptrcn err"); return NULL; }
    return cons_ptr[idx]; }
#else
  inline Connection*	PtrCn(int idx) const  { return cons_ptr[idx]; }
#endif
  // #CAT_Access fast access (no range or own_cons checking) to connection pointer at given index, consumer must cast to appropriate type (for type safety, check con_type) -- compute algorithms should use this, as they know whether the connections are owned
  inline bool		SetPtrCn(int idx, Connection* cn)
  { if(!InRange(idx) || OwnCons()) return false; cons_ptr[idx] = cn; return true; }
  // #CAT_Modify set unit pointer at given index -- returns false if out of range

  inline Connection*	Cn(int idx) const
  { if(OwnCons()) return OwnCn(idx); return PtrCn(idx); }
  // #CAT_Access generic access of connection at given index, regardless of whether it is owned or a pointer -- no range checking -- consumer must cast to appropriate sub-type (for type safety, check con_type) -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn or PtrCn)
  inline Connection*	SafeCn(int idx) const
  { if(!InRange(idx)) return NULL; return Cn(idx); }
  // #CAT_Access fully safe generic access of connection at given index, regardless of whether it is owned or a pointer -- consumer must cast to appropriate sub-type (for type safety, check con_type) -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn or PtrCn)

  inline Unit*		Un(int idx) const  { return units[idx]; }
  // #CAT_Access fast access (no range checking) to unit pointer at given index
  inline bool		SetUn(int idx, Unit* un)
  { if(!InRange(idx)) return false; units[idx] = un; return true; }
  // #CAT_Modify set unit pointer at given index -- returns false if out of range

  Connection*		ConnectUnits(Unit* our_un, Unit* oth_un, BaseCons* oth_cons,
				     bool ignore_alloc_errs = false);
  // #CAT_Modify add a new connection betwee our unit and an other unit and its appropriate cons -- does appropriate things depending on who owns the connects, etc.  enough room must already be allocated on both sides  (flag will turn off err msg)
    Connection*		ConnectUnOwnCn(Unit* un, bool ignore_alloc_errs = false);
    // #CAT_Modify add a new connection from given unit for OwnCons case -- returns NULL if no more room relative to alloc_size (flag will turn off err msg)
    bool		ConnectUnPtrCn(Unit* un, Connection* cn, bool ignore_alloc_errs = false);
    // #CAT_Modify add a new connection from given unit and connection pointer for PtrCons case -- returns false if no more room, else true 

  void			ConnectAllocInc(int inc_n = 1);
  // #CAT_Modify use this for dynamically figuring out how many connections to allocate, if it is not possible to compute directly -- increments size by given number -- later call AllocConsFmSize to allocate connections based on the size value
  void			AllocConsFmSize();
  // #CAT_Structure allocate storage for given number of connections (and Unit pointers) based on the size value, and reset size to 0 -- for dynamic allocation with ConnectAllocInc

  virtual bool		SetConType(TypeDef* cn_tp);
  // #CAT_Structure set new connection type -- will fail (err msg, return false) if connections are already allocated -- can only set prior to allocation
  virtual void		AllocCons(int n);
  // #CAT_Structure allocate storage for given number of connections (and Unit pointers) -- this MUST be called prior to making any new connections
  virtual void		FreeCons();
  // #CAT_Structure deallocate all connection-level storage (cons and units)
  virtual bool		CopyCons(const BaseCons& cp);
  // #CAT_Structure copy connections (unit ptrs and cons) from other con array, checking to make sure they are the same type (false if not) -- does not do any alloc -- just copies values -- source must have same OwnCons status as us

  virtual bool		RemoveConIdx(int i);
  // #CAT_Modify remove connection (cons and units) at given index, moving others down to fill in
  virtual bool		RemoveConUn(Unit* un);
  // #CAT_Modify remove connection from given unit
  virtual void		RemoveAll() 	{ FreeCons(); }
  // #CAT_Modify remove all conections -- frees all associated memory
  virtual void		Reset() 	{ FreeCons(); }
  // #CAT_Modify remove all conections -- frees all associated memory

  inline ConSpec* 	GetConSpec() const { return m_con_spec; }
  // #CAT_Structure get the con spec for this connection group -- this is controlled entirely by the projection con_spec
  inline void		SetConSpec(ConSpec* cs) { m_con_spec = cs; }
  // #CAT_Structure set the con spec to given value -- no ref counting or other checking is done -- should generally only be called by the Projection

  virtual void		MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this set of receiving connections

  virtual int		FindConFromIdx(Unit* un) const;
  // #CAT_Structure find index of connection from given unit
  virtual int		FindConFromNameIdx(const String& unit_nm) const;
  // #CAT_Structure find index of connection from given unit name

  virtual Connection*	FindConFrom(Unit* un) const;
  // #MENU #MENU_ON_Actions #USE_RVAL #CAT_Structure find connection from given unit
  virtual Connection*	FindConFromName(const String& unit_nm) const;
  // #MENU #MENU_ON_Actions #USE_RVAL #CAT_Structure find connection from given unit name

  virtual SendCons*	GetPrjnSendCons(Unit* su) const;
  // #CAT_Structure safely and robustly get the SendCons from given sending unit that belongs to the same projection as this recv cons (only applicable if this IsRecv())
  virtual RecvCons*	GetPrjnRecvCons(Unit* ru) const;
  // #CAT_Structure safely and robustly get the RecvCons from given recv unit that belongs to the same projection as this send cons (only applicable if this IsSend())

  static Connection* 	FindRecipRecvCon(Unit* su, Unit* ru, Layer* ru_lay);
  // #CAT_Structure find the reciprocal for sending unit su to this receiving unit ru
  static Connection* 	FindRecipSendCon(Unit* ru, Unit* su, Layer* su_lay);
  // #CAT_Structure find the reciprocal for receiving unit ru from this sending unit su

  ////////////////////////////////////////////////////////////////////////////////
  //	The following are computational functions needed for basic infrastructure

  void	Init_Weights_post(Unit* ru) 	{ if(GetConSpec()) GetConSpec()->Init_Weights_post(this,ru); }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  ////////////////////////////////////////////////////////////////////////////////
  //	The following are misc functionality not required for primary computing

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(Unit* un, const SimpleMathSpec& pre_proc,
			     Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(Unit* un, float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove weights with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	ConValuesToArray(float_Array& ary, const String& variable);
  // #CAT_Structure adds values of variable from the connections into the given array (false if var not found)
  virtual bool	ConValuesToMatrix(float_Matrix& mat, const String& variable);
  // #CAT_Structure sets values of variable from the connections into the given matrix (uses flat index of cons to set: 0..size-1), returns false if matrix is not appropriately sized
  virtual bool	ConValuesFromArray(float_Array& ary, const String& variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found)
  virtual bool	ConValuesFromMatrix(float_Matrix& mat, const String& variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found) -- uses flat index of cons to set: 0..size-1

  static int	LoadWeights_StartTag(istream& strm, const String& tag, String& val, bool quiet);
  // #IGNORE read in a start tag -- makes sure it matches tag, returns TAG_GOT if got it
  static int	LoadWeights_EndTag(istream& strm, const String& trg_tag, String& cur_tag, int& stat, bool quiet);
  // #IGNORE read in an end tag -- makes sure it matches trg_tag, cur_tag, stat are current read_tag & status (if !END_TAG, will try to read end)

  virtual void	SaveWeights_strm(ostream& strm, Unit* ru, BaseCons::WtSaveFormat fmt = BaseCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, Unit* ru, BaseCons::WtSaveFormat fmt = BaseCons::TEXT, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary format) -- rval is taMisc::ReadTagStatus, TAG_END if successful -- the connections for both sides must already be allocated, but it can rearrange connections based on save unit indexes for random connectivity etc
  static int 	SkipWeights_strm(istream& strm, BaseCons::WtSaveFormat fmt = BaseCons::TEXT,
				 bool quiet = false);
  // #IGNORE skip over saved weights (to keep the file in sync) -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void	SaveWeights(const String& fname="", Unit* ru = NULL, BaseCons::WtSaveFormat fmt = BaseCons::TEXT);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="", Unit* ru = NULL, BaseCons::WtSaveFormat fmt = BaseCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File  #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary format) (leave fname empty to pull up file chooser)

  virtual void	Copy_Weights(const BaseCons* src);
  // #CAT_ObjectMgmt copies weights from other con_group

  virtual void	ConVarsToTable(DataTable* dt, Unit* ru, const String& var1, const String& var2 = "",
	       const String& var3 = "", const String& var4 = "", const String& var5 = "",
	       const String& var6 = "", const String& var7 = "", const String& var8 = "",
	       const String& var9 = "", const String& var10 = "", const String& var11 = "",
	       const String& var12 = "", const String& var13 = "", const String& var14 = "");
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var"

  override int 	UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  override int	UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  override int 	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);
  override bool	ChangeMyType(TypeDef* new_type);

  virtual void 	LinkPtrCons(Unit* ru);
  // #IGNORE link pointer connections from the corresponding owned connections -- only needed after a Copy -- this requires that the unit pointers are updated and set properly (through UpdatePointers), and just does the connections which cannot otherwise be updated
  
  override String GetTypeDecoKey() const { return "Connection"; }

  override int	Dump_Save_PathR(ostream& strm, taBase* par=NULL, int indent=0);
  override int	Dump_Load_Value(istream& strm, taBase* par=NULL);
  // the dump system saves the alloc_size during the first 'path' stage of dumping, and then during loading does a full AllocCons for everything, building it all anew prior to the second 'value' stage of loading, which can then do ConnectFrom to setup connections, and set weights etc

  // the cons versions below have support for loading an "old" format file (prior to 4.1.0), which does not have the pre-alloc during the first path load phase: they save the load string into user data on the Unit, and then the Network::Dump_Load_Value goes through and reads those in after doing a manual Connect, so that everything is allocated

  virtual int 	Dump_Save_Cons(ostream& strm, int indent);
  // #CAT_FILE save just the connection values out to given stream -- call this in Dump_Save_Value after default guy to actually save connections (in RecvCons)
  virtual int 	Dump_Load_Cons(istream& strm, bool old_2nd_load = false);
  // #CAT_FILE load just the connection values from given stream -- call this in Dump_Load_Value to actually load connections (in RecvCons)

  void	CutLinks();
  void	Copy_(const BaseCons& cp);
  TA_BASEFUNS(BaseCons);
protected:
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void 	Initialize();
  void 	Destroy();
};


/////////////////////////////////////

class EMERGENT_API RecvCons : public BaseCons {
  // receiving connections base class -- one projection's worth of receiving connections 
INHERITED(BaseCons)
public:
  // following is for backward compatibility with 4.0.x -- todo remove at some point?
#ifdef __MAKETA__
  int		other_idx;      // #AKA_send_idx #CAT_Structure #READ_ONLY #SHOW index into other direction's list of cons objects (i.e., send_idx for RecvCons and recv_idx for SendCons)
#endif

  inline int		send_idx() { return other_idx; }
  // #READ_ONLY index into sending unit's send. list of SendCons

  ////////////////////////////////////////////////////////////////////////////////
  //	Below are the primary computational interface to the Network Objects
  //	for performing algorithm-specific activation and learning

  void 	Init_Weights(Unit* ru)	 	{ if(GetConSpec()) GetConSpec()->Init_Weights(this,ru); }
  // #CAT_Learning initialize weights for group
  void 	C_Init_Weights(Connection* cn, Unit* ru, Unit* su)
  { GetConSpec()->C_Init_Weights(this, cn, ru, su); }
  // #CAT_Learning initialize weights for single connection
  void	Init_Weights_post(Unit* ru) 	{ if(GetConSpec()) GetConSpec()->Init_Weights_post(this,ru); }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)
  void 	Init_dWt(Unit* ru)	 	{ GetConSpec()->Init_dWt(this,ru); }
  // #CAT_Learning  initialize weight change variables

  float Compute_Netin(Unit* ru)	 	{ return GetConSpec()->Compute_Netin(this,ru); }
  // #CAT_Activation compute net input (receiver based; recv group)
  float Compute_Dist(Unit* ru)	 	{ return GetConSpec()->Compute_Dist(this,ru); }
  // #CAT_Activation compute net input as distance between activation and weights
  void  Compute_dWt(Unit* ru)	 	{ GetConSpec()->Compute_dWt(this,ru); }
  // #CAT_Learning compute weight changes (the fundamental learning problem)
  void 	Compute_Weights(Unit* ru)	{ GetConSpec()->Compute_Weights(this,ru); }
  // #CAT_Learning update weight values from deltas


  override int 	Dump_Save_Value(ostream& strm, taBase* par=NULL, int indent = 0);
  override int	Dump_Load_Value(istream& strm, taBase* par=NULL);

  virtual int	Dump_Load_Old_Cons(Unit* ru, int recv_gp_idx);
  // #IGNORE load old connection values if a user-data string is present to this effect -- removes the user data after loading!

  TA_BASEFUNS_NOCOPY(RecvCons);
protected:
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void 	Initialize();
  void 	Destroy()	{ };
};

class EMERGENT_API RecvCons_List: public taList<RecvCons> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network ##NO_EXPAND_ALL list of receiving connections, one per projection
INHERITED(taList<RecvCons>)
public:
  virtual RecvCons*	NewPrjn(Projection* prjn);
  // #CAT_Structure create a new recv cons from given projection, with given ownership (own_cons)

  virtual int		FindPrjnIdx(Projection* prjn) const;
  // #CAT_Structure find index of recv cons associated with given projection
  virtual RecvCons*	FindPrjn(Projection* prjn) const;
  // #CAT_Structure find recv cons associated with given projection

  virtual int		FindFromIdx(Layer* from) const;
  // #CAT_Structure find index of recv cons that receives from given layer
  virtual RecvCons*	FindFrom(Layer* from) const;
  // #CAT_Structure find recv cons that receives from given layer

  virtual int		FindFromNameIdx(const String& fm_nm) const;
  // #CAT_Structure find index of recv cons that receives from given layer named fm_nm
  virtual RecvCons*	FindFromName(const String& fm_nm) const;
  // #CAT_Structure find recv cons that receives from given layer named fm_nm

  virtual int		FindTypeFromIdx(TypeDef* prjn_typ, Layer* from) const;
  // #CAT_Structure find index of recv cons that recvs prjn of given type from layer
  virtual RecvCons*	FindTypeFrom(TypeDef* prjn_typ, Layer* from) const;
  // #CAT_Structure find recv cons that recvs prjn of given type from layer

  virtual bool		RemovePrjn(Projection* prjn);
  // #CAT_Structure remove recv cons associated with given projection
  virtual bool		RemoveFrom(Layer* from);
  // #MENU #CAT_Structure remove recv cons that receives from given layer

  override String 	GetTypeDecoKey() const { return "Connection"; }

  NOCOPY(RecvCons_List)
  TA_BASEFUNS(RecvCons_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_RecvCons); }
  void 	Destroy()		{ };
};

/////////////////////////////////////////////////////////////////////////////////
//		SendCons

class EMERGENT_API SendCons : public BaseCons {
  // sending connections base class -- one projection's worth of sending connections 
INHERITED(BaseCons)
public:
  // following is for backward compatibility with 4.0.x -- todo remove at some point?
#ifdef __MAKETA__
  int		other_idx;      // #AKA_recv_idx #CAT_Structure #READ_ONLY #SHOW index into other direction's list of cons objects (i.e., send_idx for RecvCons and recv_idx for SendCons)
#endif

  inline int		recv_idx() { return other_idx; }
  // #READ_ONLY index into recv unit's recv. list of RecvCons

  ////////////////////////////////////////////////////////////////////////////////
  //	Below are the primary computational interface to the Network Objects
  //	for performing algorithm-specific activation and learning

  // note: if using send_netin, it probably makes sense to have the sender own the connections
  // and add all the basic functionality from the sender perspective -- see Leabra for example

  void 	Send_Netin(Network* net, int thread_no, Unit* su)
  { GetConSpec()->Send_Netin(this, net, thread_no, su); }
  // #CAT_Activation sender-based net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units
  
  TA_BASEFUNS_NOCOPY(SendCons);
protected:
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void 	Initialize();
  void 	Destroy()	{ }
};

class EMERGENT_API SendCons_List: public taList<SendCons> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network ##NO_EXPAND_ALL list of sending connections, one per projection
INHERITED(taList<SendCons>)
public:
  // projection-related functions for operations on sub-groups of the group
  virtual SendCons*	NewPrjn(Projection* prjn);
  // #CAT_Structure create a new sub_group from given projection, with given ownership (own_cons)

  virtual int		FindPrjnIdx(Projection* prjn) const;
  // #CAT_Structure find index of send cons associated with given projection
  virtual SendCons*	FindPrjn(Projection* prjn) const;
  // #CAT_Structure find send cons associated with given projection

  virtual int		FindToIdx(Layer* to) const;
  // #CAT_Structure find index of send cons that sends to given layer
  virtual SendCons*	FindTo(Layer* to) const;
  // #CAT_Structure find send cons that sends to given layer

  virtual int		FindToNameIdx(const String& to_nm) const;
  // #CAT_Structure find index of send cons that sends to given layer named to_nm
  virtual SendCons*	FindToName(const String& to_nm) const;
  // #CAT_Structure find send cons that sends to given layer named to_nm

  virtual int		FindTypeToIdx(TypeDef* prjn_typ, Layer* to) const;
  // #CAT_Structure find index of send cons that sends prjn of given type to layer
  virtual SendCons*	FindTypeTo(TypeDef* prjn_typ, Layer* to) const;
  // #CAT_Structure find send cons that sends prjn of given type to layer

  virtual bool		RemovePrjn(Projection* prjn);
  // #CAT_Structure remove sending connections associated with given projection
  virtual bool		RemoveTo(Layer* to);
  // #MENU #CAT_Structure remove sending connections to given layer

  override String 	GetTypeDecoKey() const { return "Connection"; }

  TA_BASEFUNS_NOCOPY(SendCons_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_SendCons); }
  void 	Destroy()		{ };
};



//   ConPtrList	cons;
  // #NO_FIND #NO_SAVE #CAT_Structure list of pointers to receiving connections, in index correspondence with units;



//////////////////////////////////////////////////////////////////////////////
//		Units and UnitSpec

class EMERGENT_API UnitSpec: public BaseSpec {
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
  // #CAT_Statistic tolerance for computing sum-squared error and other error metrics on a per-unit basis

  ////////////////////////////////////////////////////////////////////////////////
  //	Below are the primary computational interface to the Network Objects
  //	for performing algorithm-specific activation and learning
  //	Many functions operate directly on the units via threads, and then
  //	call through to the layers for any layer-level subsequent processing
  // 	units typically call spec versions except for basic stuff

  // 	Init functions are NOT threaded, while Compute functions are

  virtual void 	Init_Acts(Unit* u, Network* net);
  // #CAT_Activation initialize unit activation state variables
  virtual void 	Init_dWt(Unit* u, Network* net);
  // #CAT_Learning initialize the weight change variables
  virtual void 	Init_Weights(Unit* u, Network* net);
  // #CAT_Learning init weight state variables
  virtual void	Init_Weights_post(Unit* u, Network* net);
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  virtual void 	Compute_Netin(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Activation compute net input: activations times weights (receiver based)
  virtual void 	Send_Netin(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Activation send net input to all units I send to (sender based)
  virtual void 	Compute_SentNetin(Unit* u, Network* net, float sent_netin);
  // #CAT_Activation called by network-level Send_Netin function to integrate sent netin value with current net input value -- default is just to set to net val + bias wt if avail
  virtual void 	Compute_Act(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Activation compute the activation value of the unit: what it sends to other units

  virtual void 	Compute_dWt(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Learning compute change in weights: the mechanism of learning
  virtual void 	Compute_Weights(Unit* u, Network* net, int thread_no=-1);
  // #CAT_Learning update weights from deltas

  virtual float	Compute_SSE(Unit* u, Network* net, bool& has_targ);
  // #CAT_Statistic compute sum squared error for this unit -- uses sse_tol so error is 0 if within tolerance -- has_targ indicates if there is actually a target value (else the return value is 0)
  virtual bool	Compute_PRerr(Unit* u, Network* net, float& true_pos, float& false_pos, float& false_neg);
  // #CAT_Statistic compute precision and recall error statistics for this unit -- true positive, false positive, and false negative -- returns true if unit actually has a target value specified (otherwise everything is 0) -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance 


  ////////////////////////////////////////////////////////////////////////////////
  //	The following are misc functionality not required for primary computing

  virtual void	BuildBiasCons();
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE #CAT_Structure build the bias connections according to specified type

  virtual bool  CheckConfig_Unit(Unit* un, bool quiet=false);
  // #CAT_ObjectMgmt check for for misc configuration settings required by different algorithms

  override String 	GetTypeDecoKey() const { return "UnitSpec"; }

  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const UnitSpec& cp);
  TA_BASEFUNS(UnitSpec);
protected:
  SPEC_DEFAULTS;
  override void  	UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& ok);
  override bool 	CheckType_impl(TypeDef* td);
  override bool 	CheckObjectType_impl(taBase* obj);
private:
  void 	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

SpecPtr_of(UnitSpec);

class EMERGENT_API Unit: public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##DMEM_SHARE_SETS_3 ##CAT_Network Generic unit -- basic computational unit of a neural network (e.g., a neuron-like processing unit)
INHERITED(taOBase)
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

  String	name;		// name of unit (empty by default)
  ExtType	ext_flag;
  // #GUI_READ_ONLY #SHOW #CAT_Activation tells what kind of external input unit received
  float 	targ;
  // #VIEW_HOT #CAT_Activation target value: drives learning to produce this activation value
  float 	ext;
  // #VIEW_HOT #CAT_Activation external input: drives activation of unit from outside influences (e.g., sensory input)
  float 	act;
  // #DMEM_SHARE_SET_2 #VIEW_HOT #CAT_Activation activation value -- what the unit communicates to others
  float 	net;
  // #DMEM_SHARE_SET_1 #VIEW_HOT #CAT_Activation net input value -- what the unit receives from others (typically sum of sending activations times the weights)
  float		wt_prjn;
  // #NO_SAVE #CAT_Statistic weight projection value -- computed by Network::ProjectUnitWeights (triggered in GUI by setting wt prjn variable in netview control panel to point to a layer instead of NULL) -- represents weight values projected through any intervening layers from source unit (selected unit in netview or passed to ProjectUnitWeights function directly)
  float		snap;
  // #NO_SAVE #CAT_Statistic current snapshot value, as computed by the Snapshot function -- this can be displayed as a border around the units in the netview
  float		tmp_calc1;
  // #NO_SAVE #READ_ONLY #HIDDEN #CAT_Statistic temporary calculation variable (used for computing wt_prjn and prossibly other things)

  RecvCons_List	recv;
  // #CAT_Structure Receiving connections, one set of connections for each projection (collection of connections) received from other units
  SendCons_List send;
  // #CAT_Structure Sending connections, one set of connections for each projection (collection of connections) sent from other units
  RecvCons	bias;
  // #CAT_Structure bias weight connection (type determined in unit spec) -- provides intrinsic activation in absence of other inputs

  int		n_recv_cons;
  // #DMEM_SHARE_SET_0 #READ_ONLY #EXPERT #CAT_Structure total number of receiving connections
  TDCoord       pos;
  // #CAT_Structure display position in space relative to owning group or layer
  int		idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_Structure index of this unit within containing unit group
  int		flat_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_Structure index of this unit in a flat array of units (used by parallel threading) -- 0 is special null case -- real idx's start at 1

#ifdef DMEM_COMPILE
  static int	dmem_this_proc;	// #IGNORE processor rank for this processor RELATIVE TO COMMUNICATOR for the network
  int 		dmem_local_proc; // #IGNORE processor on which these units are local
  virtual bool 	DMem_IsLocalProc(int proc)   	{ return dmem_local_proc == proc; } // #IGNORE
  virtual bool 	DMem_IsLocal()       		{ return dmem_local_proc == dmem_this_proc; }  // #IGNORE
  virtual int 	DMem_GetLocalProc() 		{ return dmem_local_proc; } // #IGNORE
  virtual void 	DMem_SetLocalProc(int lproc) 	{ dmem_local_proc = lproc; } // #IGNORE
  virtual void 	DMem_SetThisProc(int proc) 	{ dmem_this_proc = proc; } // #IGNORE
#endif

  inline bool	lesioned() const;
  // #IGNORE refers to the layer-level LESIONED flag -- important for thread code
  inline Layer*	own_lay() const;
  // #CAT_Structure get the owning layer of this unit
  inline Unit_Group* own_subgp() const;
  // #CAT_Structure get the owning subgroup of this unit -- NULL if unit lives directly within the layer and not in a subgroup
  inline int	UnitGpIdx() const;
  // #CAT_Structure get unit's subgroup index -- returns -1 if layer does not have unit groups -- directly from info avail on unit itself
  
  inline UnitSpec* GetUnitSpec() const { return m_unit_spec; }
  // #CAT_Structure get the unit spec for this unit -- this is controlled entirely by the layer and all units in the layer have the same unit spec
  inline void	SetUnitSpec(UnitSpec* us) { m_unit_spec = us; if(us) bias.SetConSpec(us->bias_spec.SPtr()); }
  // #CAT_Structure set the unit spec to given value -- no ref counting or other checking is done

  virtual void	Copy_Weights(const Unit* src, Projection* prjn = NULL);
  // #CAT_ObjectMgmt copies weights from other unit (incl wts assoc with unit bias member)
  virtual void	SaveWeights_strm(ostream& strm, Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus, TAG_END if successful
  static int	SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = false);
  // #IGNORE skip over saved weight values -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void	SaveWeights(const String& fname="", Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="", Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  void		SetExtFlag(ExtType flg) { ext_flag = (ExtType)(ext_flag | flg); }
  // #CAT_Activation set ext flag for what type of input data we receive
  void		UnSetExtFlag(ExtType flg) { ext_flag = (ExtType)(ext_flag & ~flg); }
  // #CAT_Activation un-set ext flag for what type of input data we receive
  bool		HasExtFlag(int flg) { return ext_flag & flg; }
  // #CAT_Activation check if has given ext flag value

  virtual void 	ApplyInputData(float val, ExtType act_ext_flags, Random* ran = NULL,
			       bool na_by_range=false);
  // #CAT_Activation apply external input or target value to unit

  ////////////////////////////////////////////////////////////////////////////////
  //	Below are the primary computational interface to the Network Objects
  //	for performing algorithm-specific activation and learning
  //	Many functions operate directly on the units via threads, and then
  //	call through to the layers for any layer-level subsequent processing
  // 	units typically call spec versions except for basic stuff

  // 	Init functions are NOT threaded, while Compute functions are

  virtual void 	Init_InputData() { ext = targ = 0.0f; ext_flag = NO_EXTERNAL; }
  // #MENU #MENU_ON_Actions initialize unit external input data variables

  void 	Init_Acts(Network* net)	{ GetUnitSpec()->Init_Acts(this, net); }
  // #MENU #CAT_Activation initialize unit state variables
  void 	Init_dWt(Network* net)	{ GetUnitSpec()->Init_dWt(this, net); }
  // #MENU #CAT_Learning initialze weight change variables
  void 	Init_Weights(Network* net) { GetUnitSpec()->Init_Weights(this, net); }
  // #MENU #CAT_Learning Initialize weight values
  void	Init_Weights_post(Network* net) { GetUnitSpec()->Init_Weights_post(this, net); }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  void 	Compute_Netin(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Netin(this, net, thread_no); }
  // #CAT_Activation compute net input from other units
  void 	Send_Netin(Network* net, int thread_no=-1)
  { GetUnitSpec()->Send_Netin(this, net, thread_no); }
  // #CAT_Activation send net input to other units
  void 	Compute_SentNetin(Network* net, float sent_netin)
  { GetUnitSpec()->Compute_SentNetin(this, net, sent_netin); }
  // #CAT_Activation compute net input for unit based on sent_netin value from Send_Netin
  void 	Compute_Act(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Act(this, net, thread_no); }
  // #CAT_Activation compute activation value: what we send to others
  void 	Compute_NetinAct(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Netin(this, net, thread_no);
    GetUnitSpec()->Compute_Act(this, net, thread_no); }
  // #CAT_Activation compute net input from other units and then our own activation value based on that -- use this for feedforward networks to propagate activation through network in one compute cycle

  void 	Compute_dWt(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_dWt(this, net, thread_no); }
  // #CAT_Learning compute weight changes: the essence of learning
  void 	Compute_Weights(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Weights(this, net, thread_no); }
  // #CAT_Learning update weight values from weight change variables

  float	Compute_SSE(Network* net, bool& has_targ)
  { return GetUnitSpec()->Compute_SSE(this, net, has_targ); }
  // #CAT_Statistic compute sum-squared-error of activations versus target values (standard measure of performance) -- not threadable due to integration requirements at higher levels
  bool	Compute_PRerr(Network* net, float& true_pos, float& false_pos, float& false_neg)
  { return GetUnitSpec()->Compute_PRerr(this, net, true_pos, false_pos, false_neg); }
  // #CAT_Statistic compute precision and recall error statistics for this unit -- true positive, false positive, and false negative -- returns true if unit actually has a target value specified (otherwise everything is 0) -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance 


  ////////////////////////////////////////////////////////////////////////////////
  //	The following are misc functionality not required for primary computing

  virtual bool	BuildUnits();
  // #CAT_Structure build unit: make sure bias connection is created and right type
  virtual bool	CheckBuild(bool quiet=false);
  // #CAT_Structure check if network is built 
  virtual void	RemoveCons();
  // #IGNORE remove all of unit's sending and receiving connections -- since this doesn't affect other units, it should not be called individually
  virtual void	RecvConsPreAlloc(int no, Projection* prjn);
  // #CAT_Structure pre-allocate given no of receiving connections -- sufficient connections must be allocated in advance of making specific connections
  virtual void	SendConsPreAlloc(int no, Projection* prjn);
  // #CAT_Structure pre-allocate given no of sending connections -- sufficient connections must be allocated in advance of making specific connections
  virtual void	SendConsAllocInc(int no, Projection* prjn);
  // #CAT_Structure increment size by given no of sending connections -- later call SendConsPostAlloc to actually allocate connections
  virtual void	RecvConsAllocInc(int no, Projection* prjn);
  // #CAT_Structure increment size by given no of recv connections -- later call RecvConsPostAlloc to actually allocate connections
  virtual void	SendConsPostAlloc(Projection* prjn);
  // #CAT_Structure post-allocate given no of sending connections (calls AllocConsFmSize on send con group) -- if connections were initially made using the alloc_send = true, then this must be called to actually allocate connections -- then routine needs to call ConnectFrom again to make the connections
  virtual void	RecvConsPostAlloc(Projection* prjn);
  // #CAT_Structure post-allocate given no of recv connections (calls AllocConsFmSize on recv con group) -- if connections were initially made using the alloc_send = true, then this must be called to actually allocate connections -- then routine needs to call ConnectFrom again to make the connections
  virtual Connection*	ConnectFrom(Unit* su, Projection* prjn, bool alloc_send = false, 
				    bool ignore_alloc_errs = false);
  // #CAT_Structure make a recv connection from given unit to this unit using given projection -- requires both recv and sender to have sufficient connections allocated already, unless alloc_send is true, then it only allocates connections on the sender -- does NOT make any connection on the receiver -- use this in a loop that runs connections twice, with first pass as allocation (then call SendConstPostAlloc) and second pass as actual connection making
  virtual Connection* 	ConnectFromCk(Unit* su, Projection* prjn, 
				      bool ignore_alloc_errs = false);
  // #CAT_Structure does ConnectFrom but checks for an existing connection to prevent double-connections -- note that this is expensive -- only use if there is a risk of multiple connections.  This does not support alloc_send option -- can call in 2nd pass if needed
  virtual bool	DisConnectFrom(Unit* su, Projection* prjn=NULL);
  // #CAT_Structure remove connection from given unit (projection is optional)
  virtual void	DisConnectAll();
  // #MENU #MENU_ON_Actions #CAT_Structure disconnect unit from all other units
  virtual int	CountRecvCons();
  // #CAT_Structure count total number of receiving connections

  virtual void	GetLocalistName();
  // #CAT_Structure look for a receiving projection from a single unit, which has a name: if found, set our name to that name

  virtual void	TransformWeights(const SimpleMathSpec& trans, Projection* prjn = NULL);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec, Projection* prjn = NULL);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc, Relation::Relations rel,
			     float cmp_val, Projection* prjn = NULL);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(float p_lesion, bool permute=true, Projection* prjn = NULL);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual void	MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this unit
  virtual bool	Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #BUTTON #CAT_Statistic take a snapshot of given variable: assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.  

  virtual void	VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual void	ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
	       const String& var3 = "", const String& var4 = "", const String& var5 = "",
	       const String& var6 = "", const String& var7 = "", const String& var8 = "",
	       const String& var9 = "", const String& var10 = "", const String& var11 = "",
	       const String& var12 = "", const String& var13 = "", const String& var14 = "",
	       Projection* prjn=NULL);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var" -- prjn restricts to that prjn

  virtual void 	LinkPtrCons();
  // #IGNORE link pointer connections from the corresponding owned connections -- only needed after a Copy

  override int	GetIndex() const { return idx; }
  override void	SetIndex(int i) { idx = i; }
  virtual int	GetMyLeafIndex();
  // #CAT_Structure compute leaf index from my individual index in an efficient manner
  void		GetAbsPos(TDCoord& abs_pos)  { abs_pos = pos; AddRelPos(abs_pos); }
  // #CAT_Structure get absolute pos, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void		GetLayerAbsPos(TwoDCoord& lay_abs_pos); 
  // #CAT_Structure get coords of this Unit in the Layer (NOT for any kind of layout/3D use, only for Unit-in-Layer usage)
  void		AddRelPos(TDCoord& rel_pos); 
  // #IGNORE add relative pos, which factors in offsets from above
  
  override String 	GetTypeDecoKey() const { return "Unit"; }

  override bool 	SetName(const String& nm)    	{ name = nm; return true; }
  override String	GetName() const			{ return name; }

  override bool	ChangeMyType(TypeDef* new_type);

  void  InitLinks();
  void	CutLinks();
  void	Copy_(const Unit& cp);
  TA_BASEFUNS(Unit);

protected:
  UnitSpec*	m_unit_spec;	// unit spec that we use: controlled entirely by the layer!

  override void  UpdateAfterEdit_impl();
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
  override void	 CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void 	Destroy();
};


class EMERGENT_API UnitPtrList: public taPtrList<Unit> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network list of unit pointers, for sending connections
public:
  int UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  int UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  int UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);

  ~UnitPtrList()             { Reset(); }
};


// Projections are abrevieated prjn (as a oppesed to proj = project or proc = process)
// ProjectionSpec does the connectivity, and optionally the weight init

class EMERGENT_API ProjectionSpec : public BaseSpec {
  // #STEM_BASE #VIRT_BASE ##CAT_Projection Specifies the connectivity between layers (ie. full vs. partial)
INHERITED(BaseSpec)
public:
  bool		self_con;	// #CAT_Structure whether to create self-connections or not (if applicable)
  bool		init_wts;	// #CAT_Structure whether this projection spec does weight init (else conspec)
  bool		add_rnd_wts;	// #CONDSHOW_ON_init_wts if init_wts is set, use the random weight settings on the conspect to add random values to the weights set by the projection spec -- NOTE: this typically will work best by setting the rnd.mean value to 0

  virtual void 	Connect(Projection* prjn);
  // #CAT_Structure connects the network, doing PreConnect, Connect_impl, then Init_Weights -- generally do not override this function
    virtual void	PreConnect(Projection* prjn);
    // #CAT_Structure Prepare to connect (init con_groups)
    virtual void	Connect_impl(Projection*) { };
    // #CAT_Structure actually implements specific connection code

  virtual int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);
  // #CAT_Structure probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
    virtual int	ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt = 0.0);
    // #CAT_Structure actual implementation: probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection


  virtual void 	Init_dWt(Projection* prjn);
  // #CAT_Weights initializes the weight change variables
  virtual void 	Init_Weights(Projection* prjn);
  // #CAT_Weights initializes the weiht values
  virtual void 	Init_Weights_post(Projection* prjn);
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  virtual bool	CheckConnect(Projection* prjn, bool quiet=false);
  // #CAT_ObjectMgmt check if projection is connected

  virtual void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // #CAT_Weights custom initialize weights in this con group for given receiving unit ru -- any derived version MUST call the base inherited version so that other init weights variables are also initialized

  override String 	GetTypeDecoKey() const { return "ProjectionSpec"; }

  void 	InitLinks();
  SIMPLE_COPY(ProjectionSpec);
  TA_BASEFUNS(ProjectionSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	Defaults_init() 	{ };
};
TA_SMART_PTRS(ProjectionSpec)
SpecPtr_of(ProjectionSpec);


class EMERGENT_API Projection: public taNBase {
  // #STEM_BASE ##CAT_Network ##SCOPE_Network Projection describes connectivity between layers (from receivers perspective)
INHERITED(taNBase)
public:
  enum PrjnSource {
    NEXT,		// Recv from the next layer in network
    PREV,		// Recv from the previous layer in network
    SELF,		// Recv from the same layer
    CUSTOM, 		// Recv from the layer spec'd in the projection
    INIT		// #IGNORE used during create to skip err msgs
  };

  enum PrjnDirection {	// which direction does this projection come from
    FM_INPUT,		// from layer is closer to input signals than recv layer
    FM_OUTPUT,		// from layer is closer to output signals than recv layer
    LATERAL,		// from layer is same distance from input and output signals as this layer
    DIR_UNKNOWN,	// direction not set
  };

#ifdef __MAKETA__
  String		name;		// #READ_ONLY #SHOW name of the projection -- this is generated automatically based on the from name
#endif

  bool			off;		// #DEF_false turn this projection off -- useful for experimenting with projections while being able to keep the specifications in place
  Layer* 		layer;    	// #READ_ONLY #NO_SAVE #HIDDEN #NO_SET_POINTER layer this prjn is in
  PrjnSource 		from_type;	// #CAT_Structure Source of the projections
  LayerRef		from;		// #CAT_Structure #CONDEDIT_ON_from_type:CUSTOM layer receiving from (set this for custom)
  ProjectionSpec_SPtr	spec;		// #CAT_Structure spec for this item
  TypeDef*		con_type;	// #TYPE_Connection #CAT_Structure Type of connection
  TypeDef*		recvcons_type;	// #TYPE_RecvCons #CAT_Structure Type of receiving connection group to make
  TypeDef*		sendcons_type;	// #TYPE_SendCons #CAT_Structure Type of sending connection group to make
  ConSpec_SPtr 		con_spec;	// #CAT_Structure conspec to use for creating connections

  int			recv_idx;	// #READ_ONLY #CAT_Structure receiving con_group index
  int			send_idx;	// #READ_ONLY #CAT_Structure sending con_group index
  int			recv_n;		// #READ_ONLY #CAT_Structure #DEF_1 number of receiving con_groups allocated to this projection: almost always 1 -- some things won't work right if > 1 (e.g., copying)
  int			send_n;		// #READ_ONLY #CAT_Structure number of sending con_groups: almost always 1 -- some things won't work right if > 1 (e.g., copying)

  bool			projected; 	 // #HIDDEN #CAT_Structure t/f if connected

  PrjnDirection		direction; 	// #CAT_Structure which direction does this projection go (in terms of distance from input and output layers) -- auto computed by Compute_PrjnDirection or you can manually set; optionally used by only some algorithms
  
  virtual void 	SetFrom();
  // #CAT_Structure set where to receive from based on selections

  virtual void	SetCustomFrom(Layer* from_lay);
  // #CAT_Structure set a CUSTOM projection from given layer (if from_lay == layer, turns into SELF)

  virtual void 	RemoveCons();
  // #MENU #MENU_ON_Actions #CONFIRM #CAT_Structure Reset all connections for this projection

  virtual void	Copy_Weights(const Projection* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_Weights copies weights from other projection
  virtual void	SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt)
  virtual void	SaveWeights(const String& fname="", RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="",
			    RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  virtual void  CheckSpecs();
  // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig

  // convenience functions for those defined in the spec
  void 	PreConnect()		{ spec->PreConnect(this); }
  // #CAT_Structure pre-configure connection state
  void 	Connect()		{ spec->Connect(this); }
  // #BUTTON #CONFIRM #CAT_Structure Make all connections for this projection (resets first)
  void 	Connect_impl()		{ spec->Connect_impl(this); }
  // #CAT_Structure actually do the connecting
  int 	ProbAddCons(float p_add_con, float init_wt = 0.0) { return spec->ProbAddCons(this, p_add_con, init_wt); }
  // #MENU #MENU_ON_Actions #USE_RVAL #CAT_Structure probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
  void 	Init_dWt()		{ spec->Init_dWt(this); }
  // #MENU #MENU_SEP_BEFORE #CAT_Weights Initialize weight changes for this projection
  void 	Init_Weights()		{ spec->Init_Weights(this); }
  // #BUTTON #CONFIRM #CAT_Weights Initialize weight state for this projection
  void 	Init_Weights_post() 	{ spec->Init_Weights_post(this); }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  void 	C_Init_Weights(RecvCons* cg, Unit* ru)  { spec->C_Init_Weights(this, cg, ru); }
  // #CAT_Weights custom initialize weights in this con group for given receiving unit ru

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Weights apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Weights add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Weights remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	UpdateConSpecs(bool force = false);
  // #CAT_Structure update con specs for all connection groups for this projection in the network to use con_spec (only if changed from last update -- force = do regardless); returns true if changed and all cons can use given spec

  virtual bool 	SetPrjnSpec(ProjectionSpec* sp);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_spec.spec set the projection spec (connectivity pattern) for this projection
  virtual bool 	SetConSpec(ConSpec* sp);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_con_spec.spec set the con spec for all connections in this prjn
  virtual bool	CheckConnect(bool quiet=false) { return spec->CheckConnect(this, quiet); }
  // #CAT_Structure check if projection is connected
  virtual void	FixPrjnIndexes();
  // #MENU #CAT_Structure fix the indicies of the connection groups (recv_idx, send_idx)

  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int	ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp

  virtual bool 	SetConType(TypeDef* td);
  // #BUTTON #DYN1 #CAT_Structure #TYPE_Connection #INIT_ARGVAL_ON_con_type set the connection type for all connections in this prjn
  virtual bool 	SetRecvConsType(TypeDef* td);
  // #BUTTON #DYN1 #CAT_Structure #TYPE_RecvCons #INIT_ARGVAL_ON_recvcons_type set the receiving connection group type for all connections in this prjn
  virtual bool 	SetSendConsType(TypeDef* td);
  // #BUTTON #DYN1 #CAT_Structure #TYPE_SendCons #INIT_ARGVAL_ON_sendcons_type set the connection group type for all connections in this prjn

  virtual void	MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #DYN1 #CAT_Statistic monitor (record in a datatable) the given variable on this projection

  virtual void	WeightsToTable(DataTable* dt, const String& col_nm = "");
  // #MENU #NULL_OK  #NULL_TEXT_0_NewTable #CAT_Structure copy entire set of projection weights to given table (e.g., for analysis), with one row per receiving unit, and one column (name is layer name if not otherwise specified) that has a float matrix cell of the geometry of the sending layer
  virtual void	VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual void	ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
	       const String& var3 = "", const String& var4 = "", const String& var5 = "",
	       const String& var6 = "", const String& var7 = "", const String& var8 = "",
	       const String& var9 = "", const String& var10 = "", const String& var11 = "",
	       const String& var12 = "", const String& var13 = "", const String& var14 = "");
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var"

  override String 	GetTypeDecoKey() const { return "Projection"; }
  override int	  GetEnabled() const 	{ return !off; }
  override void	  SetEnabled(bool value) { off = !value; }

  override bool	ChangeMyType(TypeDef* new_type);

  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Projection& cp);
  TA_BASEFUNS(Projection);
protected:
  ConSpec*	m_prv_con_spec;	// previous con spec set for cons 

  override void UpdateAfterEdit_impl();
  override void UpdateAfterMove_impl(taBase* old_owner);
  override void CheckThisConfig_impl(bool quiet, bool& rval);
//  override taiDataLink*	ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type);
private:
  void 	Initialize();
  void 	Destroy();
};

class EMERGENT_API Projection_Group: public taGroup<Projection> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network group of projections
INHERITED(taGroup<Projection>)
public:
  bool	send_prjns;	// #NO_SAVE #HIDDEN if true, this is a sending projection group which is just links
  
  override String 	GetTypeDecoKey() const { return "Projection"; }

  void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  TA_SIMPLE_BASEFUNS(Projection_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_Projection); send_prjns = false; }
  void 	Destroy()		{ };
};

class EMERGENT_API Unit_Group: public taGroup<Unit> {
  // #NO_UPDATE_AFTER ##CAT_Network ##SCOPE_Network ##NO_EXPAND_ALL ##UNDO_BARRIER a group of units -- manages structural but not computational functions
INHERITED(taGroup<Unit>)
public:
  Layer*	own_lay;	// #READ_ONLY #NO_SAVE #NO_SHOW #NO_SET_POINTER layer owner
  PosTDCoord	pos;		// #CAT_Structure position of group relative to the layer -- for display purposes
  String	output_name;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)
  int		idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_Structure index of this unit_group within containing subgroup list

  //////////////////////////////////////////////////////////////////////////
  //	Unit access API -- for internal use only -- use layer-level access of units instead!!

  Unit* 	UnitAtCoord(const TwoDCoord& coord)
  { return UnitAtCoord(coord.x,coord.y); }
  // #CAT_XpertStructure returns unit at given coordinates within unit group
  Unit*		UnitAtCoord(int x, int y);
  // #CAT_XpertStructure get unit from given set of x and y coordinates within this group
  TwoDCoord	GpLogPos();
  // #CAT_XpertStructure returns unit group *logical* position in terms of layer unit group geometry gp_geom -- computed from idx -- only for subgroups

  void		GetAbsPos(TDCoord& abs_pos) { abs_pos = pos; AddRelPos(abs_pos); }
  // #CAT_XpertStructure get absolute pos, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void		AddRelPos(TDCoord& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above

  virtual void	Copy_Weights(const Unit_Group* src);
  // #MENU #MENU_ON_Object #CAT_ObjectMgmt copies weights from other unit group (incl wts assoc with unit bias member)
  virtual void	SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful
  static int	SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File skip over weight values -- rval is taMisc::ReadTagStatus = END_TAG if successful

  virtual void	SaveWeights(const String& fname="", RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="",
			    RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  virtual void	MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this unit group

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)
  virtual int	LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove units with prob p_lesion (permute = fixed no. lesioned)

  virtual bool	UnitValuesToArray(float_Array& ary, const String& variable);
  // #CAT_Structure adds values of variable from the units into the given array
  virtual bool	UnitValuesToMatrix(float_Matrix& mat, const String& variable);
  // #CAT_Structure adds values of variable from the units into the given matrix
  virtual bool	UnitValuesFromArray(float_Array& ary, const String& variable);
  // #CAT_Structure sets unit values from values in the given array
  virtual bool	UnitValuesFromMatrix(float_Matrix& mat, const String& variable);
  // #CAT_Structure sets unit values from values in the given array

  virtual void	VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual void	ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
	       const String& var3 = "", const String& var4 = "", const String& var5 = "",
	       const String& var6 = "", const String& var7 = "", const String& var8 = "",
	       const String& var9 = "", const String& var10 = "", const String& var11 = "",
	       const String& var12 = "", const String& var13 = "", const String& var14 = "",
	       Projection* prjn=NULL);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var" -- prjn restricts to that prjn

  virtual bool	VarToVarCopy(const String& dest_var, const String& src_var);
  // #CAT_Structure copy one unit variable to another (un->dest_var = un->src_var) for all units within this unit group (must be a float type variable)
  virtual bool	VarToVal(const String& dest_var, float val);
  // #CAT_Structure set variable to given value for all units within this unit group (must be a float type variable)

  virtual Unit* MostActiveUnit(int& idx);
  // #CAT_Activation Return the unit with the highest activation (act) value -- index of unit is returned in idx

  // implement save_rmv_units:
  override bool	Dump_QuerySaveChildren();
  override taObjDiffRec* GetObjDiffVal(taObjDiff_List& odl, int nest_lev,
				       MemberDef* memb_def=NULL, const void* par=NULL,
				       TypeDef* par_typ=NULL, taObjDiffRec* par_od=NULL) const;

  // we maintain our index in any owning list
  override int	GetIndex() const { return idx; }
  override void	SetIndex(int i) { idx = i; }

  override String 	GetTypeDecoKey() const { return "Unit"; }

  void	InitLinks();
  void	CutLinks();
  void  Copy_(const Unit_Group& cp);
  TA_BASEFUNS(Unit_Group);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
};

class EMERGENT_API LayerSpec : public BaseSpec {
  // generic layer specification
INHERITED(BaseSpec)
public:
  virtual bool		CheckConfig_Layer(Layer* lay, bool quiet = false)
    {return true;} // #CAT_ObjectMgmt This is ONLY for spec-specific stuff; the layer still does all its default checking (incl child checking)

  override String 	GetTypeDecoKey() const { return "LayerSpec"; }

  void 	InitLinks();
  void	CutLinks();
  TA_BASEFUNS_NOCOPY(LayerSpec); //
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	Defaults_init() 	{ };
};

class EMERGENT_API LayerDistances : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Network specifies distance from input/output layers
INHERITED(taOBase)
public:
  int	fm_input;		// how many layers between closest input layer and me (-1 if unknown)
  int	fm_output;		// how many layers between closest output layer and me (-1 if unknown)

  override String 	GetTypeDecoKey() const { return "Layer"; }

  SIMPLE_COPY(LayerDistances);
  TA_BASEFUNS_LITE(LayerDistances);
private:
  void 	Initialize()		{ fm_input = -1; fm_output = -1; }
  void	Destroy()		{ };
};

class EMERGENT_API PRerrVals : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Network contains precision and recall error values 
INHERITED(taOBase)
public:
  float		true_pos;	// true positive values -- e.g., how many active targets were correctly activated by the network (actually a sum over graded activations -- if(targ > act) act else targ)
  float		false_pos;	// false positive values -- e.g., how many inactive targets were incorrectly activated by the network (actually a sum over graded activations -- if(act > targ) act - targ)
  float		false_neg;	// false negative values -- e.g., how many active targets were incorrectly not activated by the network (actually a sum over graded activations -- if(targ > act) targ - act)
  float		precision;	// precision = true_pos / (true_pos + false_pos) -- how many of the positive responses were true positives -- i.e., of the responses the network made, how many were correct -- does not include any of the false negatives, so the network could be under-responding (see recall)
  float		recall;		// recall = true_pos / (true_pos + false_neg) -- how many true positive responses were there relative to the total number of positive targets -- if it did not respond to a number of cases where it should have, then recall will be low
  float		fmeasure;	// fmeasure = 2 * precision * recall / (precision + recall) -- harmonic mean of precision and recall -- is 1 when network is performing optimally -- this is the best overall value to look at for summary performance

  void		InitVals() { true_pos = false_pos = false_neg = precision = recall = fmeasure = 0.0f; }
  // initialize all values to 0

  void		IncrVals(const PRerrVals& vls)
  { true_pos += vls.true_pos; false_pos += vls.false_pos; false_neg += vls.false_neg; }
  // increment values from another set of values

  void		ComputePR() {
    if(true_pos > 0.0f) {
      precision = true_pos / (true_pos + false_pos); recall = true_pos / (true_pos + false_neg);
      fmeasure = 2 * precision * recall / (precision + recall);
    }
    else {
      precision = recall = fmeasure = 0.0f;
    }
  }
  // compute the precision, recall, and fmeasure values based on current raw stats values

  SIMPLE_COPY(PRerrVals);
  TA_BASEFUNS(PRerrVals);
private:
  void 	Initialize()		{ InitVals(); }
  void	Destroy()		{ };
};

class EMERGENT_API Layer : public taNBase {
  // ##EXT_lay ##COMPRESS ##CAT_Network ##SCOPE_Network layer containing units
INHERITED(taNBase)
public:
  enum DMemDist {
    DMEM_DIST_DEFAULT,		// distribute units to different processors for distributed memory processing according to the default sequential scheme
    DMEM_DIST_UNITGP		// distribute units according to unit groups, which can be less even but allows for shared weights by unit group
  }; //
  
  enum LayerType { 	// type of layer, used to determine various default settings
    HIDDEN, 		// layer does not receive external input of any form
    INPUT,		// layer receives external input (EXT) that drives activation states directly
    TARGET,		// layer receives a target input (TARG) that determines correct activation states, used for training
    OUTPUT,		// layer produces a visible output response but is not a target.  any external input serves as a comparison (COMP) against current activations.
  };
  
  enum LayerFlags { 			// #BITS flags for layer
    LF_NONE		= 0, 		// #NO_BIT
    LESIONED		= 0x0001,	// this layer is temporarily lesioned (inactivated for all network-level processing functions) -- IMPORTANT: use the Lesion and UnLesion functions to set this flag -- they provide proper updating after changes -- otherwise network dynamics will be wrong and the display will not be properly updated
    ICONIFIED		= 0x0002, 	// only display a single unit showing icon_value (set in algorithm-specific manner)
    NO_ADD_SSE		= 0x0004,	// do NOT add this layer's sse value (sum squared error) to the overall network sse value: this is for all types of SSE computed for ext_flag = TARG (layer_type = TARGET) or ext_flag = COMP (layer_type = OUTPUT) layers
    NO_ADD_COMP_SSE	= 0x0008,	// do NOT add this layer's sse value (sum squared error) to the overall network sse value: ONLY for ext_flag = COMP (OUTPUT) flag settings (NO_ADD_SSE blocks all contributions) -- this is relevant if the layer type or ext_flags are switched dynamically and only TARGET errors are relevant
    PROJECT_WTS_NEXT    = 0x0010,	// #NO_SHOW this layer is next in line for weight projection operation
    PROJECT_WTS_DONE    = 0x0020,	// #NO_SHOW this layer is done with weight projection operation (prevents loops)
    SAVE_UNITS		= 0x0040,	// save this layer's units in the project file (even if Network::SAVE_UNITS off)
    NO_SAVE_UNITS	= 0x0080,	// don't save this layer's units in the project file (even if Network::SAVE_UNITS on)
  };

  enum AccessMode { 	// how to access the units in the layer -- only relevant for layers with unit groups (otherwise modes are the same)
    ACC_LAY,		// access as a single layer-wide set of units
    ACC_GP,		// access via their subgroup structure, with group and unit index values
  };


  String		desc;		// #EDIT_DIALOG Description of this layer -- what functional role it plays, how it maps onto the brain, etc
  Network*		own_net;        // #READ_ONLY #NO_SAVE #NO_SHOW #CAT_Structure #NO_SET_POINTER Network this layer is in
  LayerFlags		flags;		// flags controlling various aspects of layer funcdtion
  LayerType		layer_type;     // #CAT_Activation type of layer: determines default way that external inputs are presented, and helps with other automatic functions (e.g., wizards)
  PosTDCoord		pos;		// #CAT_Structure position of layer relative to the overall network position (0,0,0 is lower left hand corner)
  float			disp_scale;	// #DEF_1 #CAT_Structure display scale factor for layer -- multiplies overall layer size -- 1 is normal, < 1 is smaller and > 1 is larger -- can be especially useful for shrinking very large layers to better fit with other smaller layers
  XYNGeom		un_geom;        // #AKA_geom #CAT_Structure two-dimensional layout and number of units within the layer or each unit group within the layer 
  bool			unit_groups;	// #CAT_Structure organize units into subgroups within the layer, with each unit group having the geometry specified by un_geom -- see virt_groups for whether there are actual unit groups allocated, or just virtual organization a flat list of groups 
  bool			virt_groups;	// #CONDSHOW_ON_unit_groups #CAT_Structure #DEF_true if true, do not allocate actual unit groups -- just organize a flat list of units into groups for display and computation purposes -- this is much more efficient and is the default behavior, but some rare cases require explicit unit groups still (e.g. unique positions for unit groups)
  XYNGeom		gp_geom;	// #CONDSHOW_ON_unit_groups #CAT_Structure geometry of unit sub-groups (if unit_groups) -- this is the layout of the groups, with gp_geom defining the layout of units within the groups
  PosTwoDCoord		gp_spc;		// #CONDSHOW_ON_unit_groups #CAT_Structure spacing between unit sub-groups (if unit_groups) -- this is *strictly* for display purposes, and does not affect anything else in terms of projection connectivity calculations etc.
  XYNGeom		flat_geom;	// #EXPERT #READ_ONLY #CAT_Structure geometry of the units flattening out over unit groups -- same as un_geom if !unit_groups; otherwise un_geom * gp_geom -- this is in logical (not display) sizes
  XYNGeom		disp_geom;	// #HIDDEN #READ_ONLY #CAT_Structure actual view geometry, includes spaces and groups and everything: the full extent of units within the layer
  XYNGeom		scaled_disp_geom; // #HIDDEN #READ_ONLY #CAT_Structure scaled actual view geometry: disp_scale * disp_geom -- use for view computations

  Projection_Group  	projections;	// #CAT_Structure group of receiving projections
  Projection_Group  	send_prjns;	// #CAT_Structure #HIDDEN #LINK_GROUP group of sending projections
  Unit_Group		units;		// #CAT_Structure #NO_SEARCH units or groups of units
  UnitSpec_SPtr 	unit_spec;	// #CAT_Structure default unit specification for units in this layer
  Unit::ExtType		ext_flag;	// #CAT_Activation #GUI_READ_ONLY #SHOW indicates which kind of external input layer received
  DMemDist		dmem_dist; 	// #CAT_DMem how to distribute units across multiple distributed memory processors

  LayerDistances	dist;		// #CAT_Structure distances from closest input/output layers to this layer

  String		output_name;	// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)
  String_Matrix		gp_output_names; // #HIDDEN #SHOW_TREE #CAT_Statistic #CONDSHOW_ON_unit_groups output_name's for unit subgroups -- name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)
  float			sse;		// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW sum squared error over the network, for the current external input pattern
  PRerrVals		prerr;		// #GUI_READ_ONLY #SHOW #CAT_Statistic precision and recall error values for this layer, for the current pattern
  float			icon_value;	// #GUI_READ_ONLY #HIDDEN #CAT_Statistic value to display if layer is iconified (algorithmically determined)
  int			units_flat_idx;	// #READ_ONLY #NO_SAVE starting index for this layer into the network units_flat list, used in threading
  bool			units_lesioned;	// #GUI_READ_ONLY if units were lesioned in this group, don't complain about rebuilding!
  bool			gp_unit_names_4d; // #CONDSHOW_ON_unit_groups if there are unit subgroups, create a 4 dimensional set of unit names which allows for distinct names for each unit in the layer -- otherwise a 2d set of names is created of size un_geom, all unit groups have the same repeated set of names
  String_Matrix		unit_names; 	// #HIDDEN #SHOW_TREE set unit names from corresponding items in this matrix (dims=2 for no group layer or to just label main group, dims=4 for grouped layers, dims=0 to disable)

  int			n_units;
  // #HIDDEN #READ_ONLY #NO_SAVE obsolete v3 specification of number of units in layer -- do not use!!
  bool			lesion_;	
  // #AKA_lesion #HIDDEN #READ_ONLY #NO_SAVE obsolete v3 flag to inactivate this layer from processing (reversable)

  ProjectBase*		project(); // #IGNORE this layer's project
  	
  inline void		SetLayerFlag(LayerFlags flg)   { flags = (LayerFlags)(flags | flg); }
  // set flag state on
  inline void		ClearLayerFlag(LayerFlags flg) { flags = (LayerFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool		HasLayerFlag(LayerFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void		SetLayerFlagState(LayerFlags flg, bool on)
  { if(on) SetLayerFlag(flg); else ClearLayerFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  inline bool		lesioned() const { return HasLayerFlag(LESIONED); }
  // check if this layer is lesioned -- use in function calls
  
  inline UnitSpec* GetUnitSpec() const { return unit_spec.SPtr(); }
  // #CAT_Structure get the unit spec for this unit -- this is controlled entirely by the layer and all units in the layer have the same unit spec

  ////////////////////////////////////////////////////////////////////////////////
  // Unit access API -- all access of units should occur strictly through this API
  // and NOT via unit groups (layers can handle unit groups virtually or with real
  // allocated unit groups -- see virt_groups flag)

  Unit*		UnitAtCoord(const TwoDCoord& coord) const 
  { return UnitAtCoord(coord.x, coord.y); }
  // #CAT_Access get unit at given logical coordinates, taking into account group geometry if present -- this uses *logical* flat 2d coordinates, which exclude any consideration of gp_spc between units (i.e., as if there were no space -- space is only for display)
  Unit*		UnitAtCoord(int x, int y) const;
  // #CAT_Access get unit at given logical coordinates, taking into account group geometry if present -- this uses *logical* flat 2d coordinates, which exclude any consideration of gp_spc between units (i.e., as if there were no space -- space is only for display)

  inline Unit*	UnitAtUnGpIdx(int unidx, int gpidx) const {
    if(unit_groups && !virt_groups) {
      Unit_Group* ug = (Unit_Group*)units.gp.SafeEl(gpidx); if(!ug) return NULL;
      return ug->SafeEl(unidx);
    }
    return units.SafeEl(gpidx * un_geom.n + unidx);
  }
  // #CAT_Access get unit from group and unit indexes -- only valid group index is 0 if no unit groups -- useful for efficient access to units in computational routines 
  void		UnGpIdxFmUnitIdx(int uflat_idx, int& unidx, int& gpidx) const
  { gpidx = uflat_idx / un_geom.n;  unidx = uflat_idx % un_geom.n; }
  // #CAT_Access get unit and group index values from a flat unit index

  inline Unit*	UnitAccess(AccessMode mode, int unidx, int gpidx) const {
    if(mode == ACC_GP && unit_groups) {
      if(virt_groups) return units.SafeEl(gpidx * un_geom.n + unidx);
      Unit_Group* ug = (Unit_Group*)units.gp.SafeEl(gpidx); if(!ug) return NULL;
      return ug->SafeEl(unidx);
    }
    return units.Leaf(unidx);
  }
  // #CAT_Access abstracted access of units in layer depending on mode -- if layer-wide mode, unidx is index into full set of units (flat_geom.n items), else if in group mode, get from unit from group and unit indexes
  inline int	UnitAccess_NUnits(AccessMode mode) const {
    if(mode == ACC_GP && unit_groups) return un_geom.n;
    return flat_geom.n;
  }
  // #CAT_Access abstracted access of units in layer depending on mode -- number of units associated with this access mode
  inline int	UnitAccess_NGps(AccessMode mode) const {
    if(mode == ACC_GP && unit_groups) return gp_geom.n;
    return 1;
  }
  // #CAT_Access abstracted access of units in layer depending on mode -- number of groups associated with this access mode

  Unit*		UnitAtGpCoord(const TwoDCoord& gp_coord, const TwoDCoord& coord) const
  { return UnitAtGpCoord(gp_coord.x,gp_coord.y, coord.x, coord.y); }
  // #CAT_Access get unit given both unit and group coordinates -- only functional if unit_groups is on -- this uses logical 4d coordinates, relative to gp_geom and un_geom
  Unit*		UnitAtGpCoord(int gp_x, int gp_y, int un_x, int un_y) const;
  // #CAT_Access get unit given both unit and group coordinates -- only functional if unit_groups is on -- this uses logical 4d coordinates, relative to gp_geom and un_geom


  Unit_Group* 	UnitGpAtCoord(const TwoDCoord& coord) const
  { return UnitGpAtCoord(coord.x,coord.y); }
  // #CAT_XpertAccess get unit group at logical group coordinates (i.e., within gp_geom) -- note that if virt_groups is on, then there are no unit subgroups -- better to use UnitAtGpCoord to access units directly at the unit level
  Unit_Group* 	UnitGpAtCoord(int gp_x, int gp_y) const;
  // #CAT_XpertAccess get unit group at logical group coordinates (i.e., within gp_geom) -- note that if virt_groups is on, then there are no unit subgroups -- better to use UnitAtGpCoord to access units directly at the unit level

  void		UnitLogPos(Unit* un, TwoDCoord& upos) const
  { UnitLogPos(un, upos.x, upos.y); }
  // #CAT_Access get *logical* position for unit, relative to flat_geom (no display spacing) -- based on index within group/layer
  void		UnitLogPos(Unit* un, int& x, int& y) const;
  // #CAT_Access get *logical* position for unit, relative to flat_geom (no display spacing) -- based on index within group/layer

  TwoDCoord	UnitGpPosFmIdx(int gpidx) const
  { TwoDCoord rval; rval.x = gpidx % gp_geom.x; rval.y = gpidx / gp_geom.x; return rval; }
  // #CAT_Access get unit group *logical* position from index
  int		UnitGpIdxFmPos(TwoDCoord& pos) const
  { return pos.y * gp_geom.x + pos.x; }
  // #CAT_Access get unit group index from position
  bool		UnitGpIdxIsValid(int gpidx) const
  { return gpidx >= 0 && gpidx < gp_geom.n; }
  // #CAT_Access is the group index valid (within range)

  int		UnitGpIdx(Unit* u) const;
  // #CAT_Access get unit's subgroup index (unit must live in this layer, returns -1 if layer does not have unit groups), directly from info avail on unit itself

  ////////////	display coordinate versions

  Unit*		UnitAtDispCoord(int x, int y) const;
  // #CAT_Access get unit at given *display* coordinates relative to layer -- this takes into account spaces between groups etc
  void		UnitDispPos(Unit* un, TwoDCoord& upos) const
  { UnitDispPos(un, upos.x, upos.y); }
  // #CAT_Access get display position for unit, taking into account spacing, unit group positioning etc
  void		UnitDispPos(Unit* un, int& x, int& y) const;
  // #CAT_Access get display position for unit, taking into account spacing, unit group positioning etc


  ////////////////////////////////////////////////////////////////////////////////
  // obsolete versions -- do not use in new code

  Unit*		FindUnitFmCoord(const TwoDCoord& coord)
  { return UnitAtCoord(coord.x, coord.y); }
  // #CAT_zzzObsolete get unit at given coordinates, taking into account group geometry if present -- this uses *logical* flat 2d coordinates, which exclude any consideration of gp_spc between units (i.e., as if there were no space -- space is only for display)
  Unit*		FindUnitFmCoord(int x, int y)
  { return UnitAtCoord(x, y); }
  // #CAT_zzzObsolete get unit at given coordinates, taking into account group geometry if present -- this uses *logical* flat 2d coordinates, which exclude any consideration of gp_spc between units (i.e., as if there were no space -- space is only for display)
  Unit*		FindUnitFmGpCoord(const TwoDCoord& gp_coord, const TwoDCoord& coord)
  { return UnitAtGpCoord(gp_coord.x, gp_coord.y, coord.x, coord.y); }
  // #CAT_zzzObsolete get unit given both unit and group coordinates -- only functional if unit_groups is on -- this uses logical 4d coordinates, relative to gp_geom and un_geom
  Unit*		FindUnitFmGpCoord(int gp_x, int gp_y, int un_x, int un_y)
  { return UnitAtGpCoord(gp_x, gp_y, un_x, un_y); }
  // #CAT_zzzObsolete get unit given both unit and group coordinates -- only functional if unit_groups is on -- this uses logical 4d coordinates, relative to gp_geom and un_geom
  Unit_Group* 	FindUnitGpFmCoord(const TwoDCoord& coord)
  { return UnitGpAtCoord(coord.x,coord.y); }
  // #CAT_zzzObsolete get unit group at logical group coordinates (i.e., within gp_geom) -- note that if virt_groups is on, then there are no unit subgroups -- better to use UnitAtGpCoord to access units directly at the unit level
  Unit_Group* 	FindUnitGpFmCoord(int gp_x, int gp_y)
  { return UnitGpAtCoord(gp_x, gp_y); }
  // #CAT_zzzObsolete get unit group at logical group coordinates (i.e., within gp_geom) -- note that if virt_groups is on, then there are no unit subgroups -- better to use UnitAtGpCoord to access units directly at the unit level

  ////////////	layer display position computation

  void		GetAbsPos(TDCoord& abs_pos) { abs_pos = pos; AddRelPos(abs_pos); }
  // #CAT_Structure get absolute pos, which factors in offsets from layer groups
  void		AddRelPos(TDCoord& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above
  void		SetDefaultPos();
  // #IGNORE initialize position of layer

  virtual void	Copy_Weights(const Layer* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_ObjectMgmt copies weights from other layer (incl wts assoc with unit bias member)
  virtual void	SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int	LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
			         bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful
  static int	SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
			         bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File skip over weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful

  virtual void	SaveWeights(const String& fname="", RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="",
			    RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  virtual void	SetNUnits(int n_units);
  // #CAT_Structure set number of units in layer in the un_geom member -- attempts to lay out geometry in closest to a square that fits all the units evenly, if possible.  note: does NOT rebuild the network.  also does not make any changes if current number of units is same as arg, or arg <= 0 (e.g., for startup arg, just init n_units to -1 prior to getting arg val so it won't have any effect if arg not passed)
  virtual void	SetNUnitGroups(int n_groups);
  // #CAT_Structure set number of unit groups in layer in the gp_geom member -- attempts to lay out geometry in closest to a square that fits all the groups evenly, if possible.  note: does NOT rebuild the network.  also does not make any changes if current number of groups is same as arg, or arg <= 0 (e.g., for startup arg, just init n_groups to -1 prior to getting arg val so it won't have any effect if arg not passed)
  
  virtual void  CheckSpecs();
  // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig
  virtual void  BuildUnits();
  // #MENU #MENU_ON_Actions #CONFIRM #CAT_Structure build the units based current geometry configuration
  virtual void 	BuildUnits_Threads(Network* net);
  // #IGNORE build unit-level thread information: flat list of units, etc -- this is called by network BuildUnits_Threads so that layers (and layerspecs) can potentially modify which units get added to the compute lists, and thus which are subject to standard computations -- default is all units in the layer
  virtual void	RecomputeGeometry();
  // #CAT_Structure recompute the layer's geometry specifcations
  virtual void  LayoutUnits();
  // #CAT_Structure layout the units according to layer geometry
  virtual void  ConnectFrom(Layer* lay);
  // #DYN12N #CAT_Structure connect from one or more other layers to this layer (receive from other layer(s)) -- in network view, receiver is FIRST layer selected -- makes a new projection between layers
  virtual void  ConnectBidir(Layer* lay);
  // #DYN12N #CAT_Structure bidirectionally connect with one or more other layers to (receive from and send to other layer(s)) -- makes new projections between layers
  virtual void  ConnectSelf();
  // #MENU #CONFIRM #CAT_Structure #MENU_CONTEXT create a new self-connection within this layer (a projection to/from this layer)
  virtual void  Connect();
  // #MENU #CONFIRM #CAT_Structure connect the layer -- construct connections according to existing projections
  virtual bool	CheckBuild(bool quiet=false);
  // #CAT_Structure check if network is built 
  virtual bool	CheckConnect(bool quiet=false);
  // #CAT_Structure check if network is connected
  virtual void	RemoveCons();
  // #MENU #CONFIRM #MENU_SEP_BEFORE #CAT_Structure remove all connections in this layer
  virtual void	RemoveCons_Net();
  // #CAT_Structure remove all connections in this layer, in context of entire network removecons -- calls unit removecons only
  virtual void	RemoveUnits();
  // #MENU #DYN1 #CAT_Structure remove all units in this layer (preserving groups)
  virtual void	RemoveUnitGroups();
  // #MENU #DYN1 #CAT_Structure remove all unit groups in this layer
  virtual void  PreConnect();
  // #CAT_XpertStructure prepare to connect the layer (create con_groups)
  virtual void	SyncSendPrjns();
  // #CAT_XpertStructure synchronize sending projections with the recv projections so everyone's happy
  virtual void	RecvConsPreAlloc(int alloc_no, Projection* prjn);
  // #CAT_XpertStructure allocate given number of recv connections for all units in layer, for given projection
  virtual void	SendConsPreAlloc(int alloc_no, Projection* prjn);
  // #CAT_XpertStructure allocate given number of send connections for all units in layer, for given projection
  virtual void	SendConsPostAlloc(Projection* prjn);
  // #CAT_XpertStructure allocate sending connections based on those allocated previously 
  virtual void	RecvConsPostAlloc(Projection* prjn);
  // #CAT_XpertStructure allocate recv connections based on those allocated previously 
  virtual void  LinkPtrCons();
  // #IGNORE link pointer connections from the corresponding owned connections -- only needed after a Copy
  virtual void	DisConnect();
  // #MENU #CONFIRM #CAT_Structure disconnect layer from all others
  virtual int 	CountRecvCons();
  // #CAT_Structure count recv connections for all units in layer

  void		SetExtFlag(int flg)   { ext_flag = (Unit::ExtType)(ext_flag | flg); }
  // #CAT_Activation set external input data flag
  void		UnSetExtFlag(int flg) { ext_flag = (Unit::ExtType)(ext_flag & ~flg); }
  // #CAT_Activation un-set external input data flag
  bool		HasExtFlag(int flg)   { return ext_flag & flg; }
  // #CAT_Activation check if has given ext flag value set

  virtual void	SetLayUnitExtFlags(int flg);
  // #CAT_Activation set external input data flags for layer and all units in the layer

  virtual void	ApplyInputData(taMatrix* data, Unit::ExtType ext_flags = Unit::NO_EXTERNAL,
      Random* ran = NULL, const PosTwoDCoord* offset = NULL, bool na_by_range=false);
  // #CAT_Activation apply the 2d or 4d external input pattern to the network, optional random additional values, and offsetting; uses a flat 2-d model where grouped layer or 4-d data are flattened to 2d; frame<0 means from end; na_by_range means that values are not applicable if they fall outside act_range on unit spec, and thus don't have flags or values set
  virtual void	TriggerContextUpdate() {} // for algorithms/specs that suport context layers (copy of previous state) this manually triggers an update

  ////////////////////////////////////////////////////////////////////////////////
  //	Below are the primary computational interface to the Network Objects
  //	for performing algorithm-specific activation and learning
  //	Many functions operate directly on the units via threads, and then
  //	call through to the layers for any layer-level subsequent processing
  //	All functions take the pointer to the parent network, just for
  //	convenience and consistency with Unit levels etc

  virtual void  Init_InputData(Network* net);
  // #CAT_Activation Initializes external and target inputs
  virtual void  Init_Acts(Network* net);
  // #CAT_Activation Initialize the unit state variables
  virtual void  Init_dWt(Network* net);
  // #CAT_Learning Initialize the weight change variables
  virtual void  Init_Weights(Network* net);
  // #MENU #CONFIRM #CAT_Learning Initialize the weights
  virtual void	Init_Weights_post(Network* net);
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  virtual float	Compute_SSE(Network* net, int& n_vals, bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value -- uses sse_tol so error is 0 if within tolerance on a per unit basis
  virtual int	Compute_PRerr(Network* net);
  // #CAT_Statistic compute precision and recall error statistics over entire layer -- true positive, false positive, and false negative -- returns number of values entering into computation (depends on number of targets) -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance on a per unit basis -- results are stored in prerr values on layer

  ////////////////////////////////////////////////////////////////////////////////
  //	The following are misc functionality not required for primary computing

  virtual void	PropagateInputDistance();
  // #CAT_XpertStructure propagate my input distance (dist.fm_input) to layers I send to
  virtual void	PropagateOutputDistance();
  // #CAT_XpertStructure propagate my output distance (dist.fm_output) to layers I receive from
  virtual void	Compute_PrjnDirections();
  // #CAT_Structure compute the directions of projections based on the relative distances from input/output layers

  virtual bool	SetUnitNames(bool force_use_unit_names = false);
  // #MENU #MENU_SEP_BEFORE #CAT_Structure set unit names from unit_names matrix (called automatically on Build) -- also ensures unit_names fits geometry of layer -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
  virtual bool	SetUnitNamesFromDataCol(const DataCol* unit_names_col, int max_unit_chars=-1);
  // #MENU #CAT_Structure set unit names from unit names table column (string matrix with one row) -- max_unit_chars is max length of name to apply to unit (-1 = all)
  virtual bool	GetUnitNames(bool force_use_unit_names = true);
  // #MENU #CAT_Structure get unit_names matrix values from current unit name values -- also ensures unit_names fits geometry of layer -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
  virtual void	GetLocalistName();
  // #CAT_XpertStructure look for a receiving projection from a single unit, which has a name: if found, set our unit name to that name (also sets unit_names)

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	ProbAddCons(float p_add_con, float init_wt = 0.0);
  // #MENU #USE_RVAL #CAT_Structure probabilistically add new connections (assuming prior pruning), init_wt = initial weight value of new connection
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connectiosn with prob p_lesion (permute = fixed no. lesioned)
  virtual int	LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove units with prob p_lesion (permute = fixed no. lesioned)

  virtual void	Iconify();
  // #MENU #DYN1 #CAT_Display iconify this layer in the network display (shrink to size of 1 unit)
  virtual void	DeIconify();
  // #MENU #DYN1 #CAT_Display de-iconify this layer in the network display (make full size)
  inline void	SetDispScale(float disp_sc) 	{ disp_scale = disp_sc; UpdateAfterEdit(); }
  // #MENU #DYN1 #CAT_Display set the display scale for the layer -- can change how much space it takes up relative to other layers
  virtual void	Lesion();
  // #MENU #DYN1 #MENU_SEP_BEFORE #CAT_Structure set the lesion flag on layer -- removes it from all processing operations
  virtual void	UnLesion();
  // #MENU #DYN1 #CAT_Structure un-set the lesion flag on layer -- restores it to engage in normal processing

  virtual bool	Iconified() const 	{ return HasLayerFlag(ICONIFIED); }
  // convenience function for checking iconified flag

  virtual void	SetLayerUnitGeom(int x, int y, bool n_not_xy = false, int n = 0);
  // set layer unit geometry (convenience function for programs)
  virtual void	SetLayerUnitGpGeom(int x, int y, bool n_not_xy = false, int n = 0);
  // set layer unit group geometry (convenience function for programs)

  virtual bool	UpdateUnitSpecs(bool force = false);
  // #CAT_Structure update unit specs for all units in the layer to use unit_spec (only if changed from last update -- force = do regardless); returns true if changed and all units can use given spec
  virtual bool	UpdateConSpecs(bool force = false);
  // #CAT_Structure update connection specs for all projections in the layer (only if changed from last update -- force = do regardless)

  virtual bool	SetLayerSpec(LayerSpec* layspec);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_spec.spec set the layer specification
  virtual LayerSpec* GetLayerSpec()		{ return (LayerSpec*)NULL; }
  // #CAT_Structure get the layer spec for this layer (if used)
  virtual bool	SetUnitSpec(UnitSpec* unitspec);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_unit_spec.spec set unit spec for all units in layer
  virtual void	SetUnitType(TypeDef* td);
  // #BUTTON #DYN1 #TYPE_Unit #CAT_Structure #INIT_ARGVAL_ON_units.el_typ set unit type for all units in layer (created by Build)
  virtual void	FixPrjnIndexes();
  // #CAT_Structure fix the projection indicies of the connection groups (other_idx)

  virtual void	MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #DYN1 #CAT_Statistic monitor (record in a datatable) the given variable on this layer (can be a variable on the units or connections as well)
  virtual bool	Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #BUTTON #CAT_Statistic take a snapshot of given variable: assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.  
  virtual Unit* MostActiveUnit(int& idx);
  // #CAT_Statistic Return the unit with the highest activation (act) value -- index of unit is returned in idx

  virtual int	ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // #CAT_Structure switch any units/layers using old_sp to using new_sp
  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int	ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp
  virtual int	ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp);
  // #CAT_Structure switch any layers using old_sp to using new_sp

  virtual void	WeightsToTable(DataTable* dt, Layer* send_lay);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send entire set of weights from sending layer to given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit
  virtual void	VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for projection variables, specify prjns.; for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual void	ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
	       const String& var3 = "", const String& var4 = "", const String& var5 = "",
	       const String& var6 = "", const String& var7 = "", const String& var8 = "",
	       const String& var9 = "", const String& var10 = "", const String& var11 = "",
	       const String& var12 = "", const String& var13 = "", const String& var14 = "",
	       Projection* prjn=NULL);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var" -- prjn restricts to that prjn
  virtual bool	VarToVarCopy(const String& dest_var, const String& src_var);
  // #CAT_Structure copy one unit variable to another (un->dest_var = un->src_var) for all units within this layer (must be a float type variable)
  virtual bool	VarToVal(const String& dest_var, float val);
  // #CAT_Structure set variable to given value for all units within this layer (must be a float type variable)

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

  override String GetDesc() const 	{ return desc; }
  override int	  GetEnabled() const 	{ return !lesioned(); }
  override void	  SetEnabled(bool value) { SetLayerFlagState(LESIONED, !value); }
  override String GetTypeDecoKey() const { return "Layer"; }

  override bool	ChangeMyType(TypeDef* new_type);

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md);

  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const Layer& cp);
  TA_BASEFUNS(Layer); //
  
protected:
  UnitSpec*	m_prv_unit_spec; // previous unit spec set for units in layer

  override void 	UpdateAfterEdit_impl();
  override void 	UpdateAfterMove_impl(taBase* old_owner);
  virtual void		ApplyLayerFlags(Unit::ExtType act_ext_flags);
  // #IGNORE set layer flag to reflect the kind of input received
  virtual void		ApplyInputData_1d(taMatrix* data, Unit::ExtType ext_flags,
			       Random* ran, bool na_by_range=false);
  // #IGNORE 1d data -- just go in order -- offsets ignored
  virtual void		ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
			       Random* ran, const TwoDCoord& offs, bool na_by_range=false);
  // #IGNORE 2d data is always treated the same: UnitAtCoord deals with unit grouping
  virtual void		ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
			       Random* ran, const TwoDCoord& offs, bool na_by_range=false);
  // #IGNORE flat layer, 4d data
  virtual void		ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags,
   			       Random* ran, bool na_by_range=false);
  // #IGNORE grouped layer, 4d data -- note this cannot have offsets..

  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  // #IGNORE this is the guy that *additionally* delegates to the Spec
  override void		CheckChildConfig_impl(bool quiet, bool& rval);// #IGNORE 
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

////////////////////  
// Unit Inlines

inline Layer* Unit::own_lay() const {
  return ((Unit_Group*)owner)->own_lay;
}

inline bool Unit::lesioned() const {
  return own_lay()->lesioned();
}

inline Unit_Group* Unit::own_subgp() const {
  if(!owner || !owner->GetOwner()) return NULL;
  if(owner->GetOwner()->InheritsFrom(&TA_Layer)) return NULL; // we're owned by the layer really
  return (Unit_Group*)owner;
}

inline int Unit::UnitGpIdx() const {
  return own_lay()->UnitGpIdx((Unit*)this);
}


////////////////////  

class EMERGENT_API Layer_Group : public taGroup<Layer> {
  // ##CAT_Network ##SCOPE_Network group of layers -- this should be used in larger networks to organize subnetworks (e.g., in brain models, different brain areas)
INHERITED(taGroup<Layer>)
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  PosTDCoord	pos;		// Position of Group of layers relative to network
  PosTDCoord	max_disp_size;	// #READ_ONLY #SHOW #CAT_Structure maximum display size of the layer group -- computed automatically from the layers within the group

  void		GetAbsPos(TDCoord& abs_pos)
  { abs_pos = pos; AddRelPos(abs_pos); }
  // #CAT_Structure get absolute pos, which factors in offsets from layer groups
  void		AddRelPos(TDCoord& rel_pos);
  // #CAT_Structure add relative pos from layer groups, which factors in offsets from layer groups
  
  virtual void	BuildLayers();
  // #CAT_Structure create any algorithmically specified layers
  virtual void	BuildPrjns();
  // #CAT_Structure create any algorithmically specified prjns
  virtual void	UpdateMaxDispSize();
  // #IGNORE update max_disp_size of layer group based on current layer layout

  virtual void	LesionLayers();
  // #BUTTON #DYN1 #CAT_Structure set the lesion flag on all the layers within this group -- removes them from all processing operations
  virtual void	UnLesionLayers();
  // #BUTTON #DYN1 #CAT_Structure un-set the lesion flag on all the layers within this group -- restores them to engage in normal processing
  virtual void	IconifyLayers();
  // #BUTTON #DYN1 #CAT_Structure iconi
  virtual void	DeIconifyLayers();
  // #BUTTON #DYN1 #CAT_Structure un-set the lesion flag on all the layers within this group
  virtual void	DispScaleLayers(float disp_scale = 1.0f);
  // #BUTTON #DYN1 #CAT_Structure set the display scale on all the layers in the group (scales the size of the units -- 1 = normal, lower = smaller units, higher = larger units.

  virtual void	Clean();
  // #MENU #MENU_CONTEXT #CAT_Structure remove any algorithmically specified layers/prjns etc.

  virtual void	LayerPos_Cleanup();
  // #MENU #MENU_CONTEXT #CAT_Structure cleanup the layer positions relative to each other (prevent overlap etc)

  virtual Layer* FindMakeLayer(const String& nm, TypeDef* td = NULL,
			       bool& nw_itm = nw_itm_def_arg, const String& alt_nm = "");
  // #CAT_Structure find a given layer and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
  virtual Layer_Group* FindMakeLayerGroup(const String& nm, TypeDef* td = NULL,
			  bool& nw_itm = nw_itm_def_arg, const String& alt_nm = "");
  // #CAT_Structure find a given layer group and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)

  void		TriggerContextUpdate();
  // #CAT_Activation for context layers, manually triggers the update		
    
  override String GetTypeDecoKey() const { return "Layer"; }

  void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(Layer_Group);
protected:
  override void	UpdateAfterEdit_impl();
  virtual void		BuildLayers_impl(); 
  virtual void		BuildPrjns_impl();
  virtual void		Clean_impl() {}
private:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  void  Copy_(const Layer_Group& cp)	{ pos = cp.pos; max_disp_size = cp.max_disp_size; }
};

TA_SMART_PTRS(Layer_Group)

class EMERGENT_API Layer_PtrList : public taPtrList<Layer> {
  // ##IGNORE used in lookaside lists 
INHERITED(taPtrList<Layer>)
public:
  Layer_PtrList() {}
};

/////////////////////////////////////////////////////////////
//		Threading code

// this is the standard unit function call taking a network pointer arg 
// and the thread number int value
// all threaded unit-level functions MUST use this call signature!
#ifdef __MAKETA__
typedef void* ThreadUnitCall;
#else
typedef taTaskMethCall2<Unit, void, Network*, int> ThreadUnitCall;
typedef void (Unit::*ThreadUnitMethod)(Network*, int);
#endif

class UnitCallThreadMgr;

class EMERGENT_API UnitCallTask : public taTask {
INHERITED(taTask)
public:
  NetworkRef	network;	// the network we're operating on
  int		uidx_st;	// unit list number to start on
  int		uidx_ed;	// unit number to end before
  int		uidx_inc;	// how much to increment counter by
  ThreadUnitCall* unit_call;	// method to call on the unit

  override void run();
  // runs specified chunks and then nibbles on remainder

  UnitCallThreadMgr* mgr() { return (UnitCallThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(UnitCallTask);
private:
  void	Initialize();
  void	Destroy();
};

class EMERGENT_API UnitCallThreadMgr : public taThreadMgr {
  // #INLINE thread manager for UnitCall tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  float		alloc_pct;	// #MIN_0 #MAX_1 #DEF_0 #NO_SAVE NOTE: not saved -- initialized from user prefs.  proportion (0-1) of total to process by pre-allocating a set of computations to a given thread -- the remainder of the load is allocated dynamically through a nibbling mechanism, where each thread takes a nibble_chunk at a time until the job is done.  current experience is that this should be no greater than .2, unless the load is quite large, as there is a high degree of variability in thread start times, so the automatic load balancing of nibbling is important, and it has very little additional overhead.
  int		nibble_chunk;	// #MIN_1 #DEF_8 #NO_SAVE NOTE: not saved -- initialized from user prefs.  how many units does each thread grab to process while nibbling?  Too small a value results in increased contention and inefficiency, while too large a value results in poor load balancing across processors.
  float		compute_thr;	// #MIN_0 #MAX_1 #DEF_0.5 #NO_SAVE NOTE: not saved -- initialized from user prefs.  threshold value for amount of computation in a given function to actually deploy on threads, as opposed to just running it on main thread -- value is normalized (0-1) with 1 being the most computationally intensive task, and 0 being the least -- as with min_units, it may not be worth it to parallelize very lightweight computations.  See Thread_Params page on emergent wiki for relevant comparison values.
  int		min_units;	// #MIN_1 #DEF_3000 #NO_SAVE NOTE: not saved -- initialized from user prefs.  minimum number of units required to use threads at all -- for feedforward algorithms requiring layer-level syncing, this applies to each layer -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.
  bool		interleave;	// #DEF_true #EXPERT deploy threads in an interleaved fashion over units, which improves load balancing as neighboring units are likely to have similar compute demands, but it costs in cache coherency as the memory access per processor is more distributed -- only affects network level (non lay_sync) processes
  bool		ignore_lay_sync;// #DEF_false ignore need to sync at the layer level for feedforward algorithms that require this (e.g., backprop) -- results in faster but less accurate processing
  QAtomicInt	nibble_i;	// #IGNORE current nibble index -- atomic incremented by working threads to nibble away the rest..
  int 		nibble_stop;	// #IGNORE nibble stopping value
  bool	       	using_threads;	// #READ_ONLY #NO_SAVE are we currently using threads for a computation or not -- also useful for just after a thread call to see if threads were used

  Network*	network() 	{ return (Network*)owner; }

  override void	InitAll();	// initialize threads and tasks

  override void	Run(ThreadUnitCall* unit_call, float comp_load,
		    bool backwards=false, bool layer_sync=false);
  // #IGNORE run given function on all units, with specified level of computational load (0-1), and flags controlling order of processing and syncing: backwards = go through units in reverse order, and layer_sync = sync processing at each layer (else at network level) -- needed for feedforward network topologies (unfortunately)

  void		RunThread0(ThreadUnitCall* unit_call, bool backwards=false);
  // #IGNORE run only on thread 0 (the main thread) -- calls method with arg thread_no = -1 -- order matters but layer_sync is irrelevant here
  void		RunThreads_FwdNetSync(ThreadUnitCall* unit_call);
  // #IGNORE run on all threads -- calls method with arg thread_no -- forward order and network-level sync
  void		RunThreads_BkwdNetSync(ThreadUnitCall* unit_call);
  // #IGNORE run on all threads -- calls method with arg thread_no -- backward order and network-level sync
  void		RunThreads_FwdLaySync(ThreadUnitCall* unit_call);
  // #IGNORE run on all threads -- calls method with arg thread_no -- forward order and layer-level sync
  void		RunThreads_BkwdLaySync(ThreadUnitCall* unit_call);
  // #IGNORE run on all threads -- calls method with arg thread_no -- backward order and layer-level sync
  
  TA_BASEFUNS_NOCOPY(UnitCallThreadMgr);
protected:
  void	UpdateAfterEdit_impl();

  int		n_threads_prev;		// #IGNORE number of threads set previously in net build -- for update diffs

private:
  void	Initialize();
  void	Destroy();
};


/////////////////////////////////////////////////////////////
//		Net View Objs

class EMERGENT_API NetViewObj : public taNBase {
  // ##CAT_Network ##EXT_nvobj network view object (3d object or text) -- is displayed in network view
INHERITED(taNBase)
public:	
  enum ObjType {		// what type of object to create
    TEXT,			// text label
    OBJECT,			// 3d object loaded from an open inventor format 3d object file
  };

  String	desc;	   	// #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  FloatTDCoord	pos;  		// 3d position of object (can be moved within network view)
  FloatRotation	rot;  		// 3d rotation of body, specifying an axis and a rot along that axis in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  FloatTDCoord	scale; 		// 3d scaling of object along each dimension (applied prior to rotation)
  ObjType	obj_type;	// type of object to display
  String	obj_fname;	// #CONDSHOW_ON_obj_type:OBJECT #FILE_DIALOG_LOAD #EXT_iv,wrl #FILETYPE_OpenInventor file name of Open Inventor file that contains the 3d geometry of the object
  String	text;		// #CONDSHOW_ON_obj_type:TEXT text to display for text type of object
  float		font_size;	// #CONDSHOW_ON_obj_type:TEXT font size to display text in, in normalized units (the entire network is 1x1x1, so this should typically be a smaller fraction like .05)
  bool		set_color;	// if true, we directly set our own color (otherwise it is whatever the object defaults to)
  taColor	color; 		// #CONDSHOW_ON_set_color default color if not otherwise defined (a=alpha used for transparency)

  override String	GetDesc() const { return desc; }

  TA_SIMPLE_BASEFUNS(NetViewObj);
protected:
  override void 	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void  Destroy()	{ CutLinks(); }
};

SmartRef_Of(NetViewObj,TA_NetViewObj); // NetViewObjRef

class EMERGENT_API NetViewObj_Group : public taGroup<NetViewObj> {
  // ##CAT_Network a group of network view objects
INHERITED(taGroup<NetViewObj>)
public:
  TA_BASEFUNS_NOCOPY(NetViewObj_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_NetViewObj); }
  void 	Destroy()		{ };
};

/////////////////////////////////////////////////////////////
//		Network

class EMERGENT_API Network : public taFBase {
  // ##FILETYPE_Network ##EXT_net ##COMPRESS ##CAT_Network ##DEF_NAME_ROOT_Network ##EXPAND_DEF_2 A network, containing layers, units, etc..
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
 
  enum AutoBuildMode {
    AUTO_BUILD,			// automatically build the network after loading
    PROMPT_BUILD,		// prompt about building after loading (if run in -nogui mode, it is automatically built without prompting)
    NO_BUILD,			// do not build network after loading
  };

  enum NetFlags { 		// #BITS flags for network
    NF_NONE		= 0, 	// #NO_BIT
    SAVE_UNITS		= 0x0001, // save units with the project or other saves (specificaly saving just the network always saves the units)
    SAVE_UNITS_FORCE 	= 0x0002, // #NO_SHOW internal flag that forces the saving of units in cases where it is important to do so (e.g., saving just the network, or for a crash recover file)
    MANUAL_POS	 	= 0x0004, // disables the automatic cleanup/positioning of layers
  }; 

  enum NetTextLoc {
    NT_BOTTOM,			// standard bottom location below network -- extends network "foot" lower below to make text more visible
    NT_TOP_BACK,		// above top-most layer, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_LEFT_BACK,		// at left of network, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_RIGHT_BACK,		// at right of network, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_LEFT_MID,		// at left of network, in the middle depth-wise -- foot is raised as when no net text is visible
    NT_RIGHT_MID,		// at right of network, in the middle depth-wise -- foot is raised as when no net text is visible
  };

  BaseSpec_Group specs; 	// #CAT_Structure Specifications for network parameters
  Layer_Group	layers;		// #CAT_Structure Layers or Groups of Layers
  NetViewObj_Group view_objs;	// #CAT_Display objects to display in the network 3d view

  NetFlags	flags;		// #CAT_Structure flags controlling various aspects of network function

  AutoBuildMode	auto_build;     // #CAT_Structure whether to automatically build the network (make units and connections) after loading or not (if the SAVE_UNITS flag is not on, then auto building makes sense)

  TrainMode	train_mode;	// #CAT_Learning training mode -- determines whether weights are updated or not (and other algorithm-dependent differences as well).  TEST turns off learning
  WtUpdate	wt_update;	// #CAT_Learning #CONDSHOW_ON_train_mode:TRAIN weight update mode: when are weights updated (only applicable if train_mode = TRAIN)
  int		small_batch_n;	// #CONDSHOW_ON_wt_update:SMALL_BATCH #CAT_Learning number of events for small_batch learning mode (specifies how often weight changes are synchronized in dmem)
  int		small_batch_n_eff; // #GUI_READ_ONLY #EXPERT #NO_SAVE #CAT_Learning effective batch_n value = batch_n except for dmem when it = (batch_n / epc_nprocs) >= 1

  int		batch;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW batch counter: number of times network has been trained over a full sequence of epochs (updated by program)
  int		epoch;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW epoch counter: number of times a complete set of training patterns has been presented (updated by program)
  int		group;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW group counter: optional extra counter to record sequence-level information (sequence = group of trials)
  int		trial;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW trial counter: number of external input patterns that have been presented in the current epoch (updated by program)
  int		tick;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW tick counter: optional extra counter to record a level of organization below the trial level (for cases where trials have multiple component elements)
  int		cycle;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW cycle counter: number of iterations of activation updating (settling) on the current external input pattern (updated by program)	
  float		time;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW the current time, relative to some established starting point, in algorithm-specific units (often miliseconds)
  String	group_name;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name associated with the current group of trials, if such a grouping is applicable (typically set by a LayerWriter)
  String	trial_name;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name associated with the current trial (e.g., name of input pattern, typically set by a LayerWriter)
  String	output_name;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name for the output produced by the network (must be computed by a program)
  
  bool		sse_unit_avg;	// #CAT_Statistic compute sse as average sse over units (i.e., divide by total number of target units in layer)
  bool		sse_sqrt;	// #CAT_Statistic take the square root of the SSE, producing a Euclidian distance instead of raw sse (note this cannot technically be added across trials in a linear fashion, as raw sse can)
  float		sse;		// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW sum squared error over the network, for the current external input pattern
  float		sum_sse;	// #GUI_READ_ONLY #SHOW #CAT_Statistic total sum squared error over an epoch or similar larger set of external input patterns
  float		avg_sse;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average sum squared error over an epoch or similar larger set of external input patterns
  float		cnt_err_tol;	// #CAT_Statistic tolerance for computing the count of number of errors over current epoch
  float		cnt_err;	// #GUI_READ_ONLY #SHOW #CAT_Statistic count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns
  float		pct_err;	// #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns (= cnt_err / n)
  float		pct_cor;	// #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was below cnt_err_tol over an epoch or similar larger set of external input patterns (= 1 - pct_err -- just for convenience for whichever you want to plot)

  float	       	cur_sum_sse;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current sum_sse -- used during computation of sum_sse
  int	       	avg_sse_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic number of times cur_sum_sse updated: for computing avg_sse
  float	       	cur_cnt_err;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current cnt_err -- used for computing cnt_err

  bool		compute_prerr;	// #CAT_Statistic compute precision and recall error values over units, at same time as computing sum squared error (SSE)
  PRerrVals	prerr;		// #GUI_READ_ONLY #SHOW #CAT_Statistic precision and recall error values for the entire network, for the current external input pattern
  PRerrVals	sum_prerr;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic precision and recall error values for the entire network, over an epoch or similar larger set of external input patterns -- these are always up-to-date as the system is aggregating, given the additive nature of the statistics
  PRerrVals	epc_prerr;	// #GUI_READ_ONLY #SHOW #CAT_Statistic precision and recall error values for the entire network, over an epoch or similar larger set of external input patterns

  TimeUsed	train_time;	// #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing entire training (across epochs) (managed entirely by programs -- not always used)
  TimeUsed	epoch_time;	// #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing an epoch (managed entirely by programs -- not always used)
  TimeUsed	group_time;	// #GUI_READ_ONLY #EXPERT #CAT_Statistic #NO_SAVE time used for computing a group, when groups used (managed entirely by programs -- not always used)
  //TODO: remove the NO_SAVE for v.0.15
  TimeUsed	trial_time;	// #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a trial (managed entirely by programs -- not always used)
  TimeUsed	settle_time;	// #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a settling (managed entirely by programs -- not always used)
  TimeUsed	cycle_time;	// #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a cycle (managed entirely by programs -- not always used)
  TimeUsed	wt_sync_time;	// #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for the DMem_SumDWts operation (trial-level dmem, computed by network) 
  TimeUsed	misc_time;	// #GUI_READ_ONLY #EXPERT #CAT_Statistic misc timer for ad-hoc use by programs

  UnitCallThreadMgr threads;	// #CAT_Threads parallel threading of network computation
  UnitPtrList	units_flat;	// #READ_ONLY #NO_SAVE #CAT_Threads flat list of units for deploying in threads
  float_Matrix	send_netin_tmp; // #READ_ONLY #NO_SAVE #CAT_Threads temporary storage for threaded sender-based netinput computation -- dimensions are [un_idx][task] (inner = units, outer = task, such that units per task is contiguous in memory)

  DMem_SyncLevel dmem_sync_level; // #CAT_DMem at what level of network structure should information be synchronized across processes?
  int		dmem_nprocs;	// #CAT_DMem number of processors to use in distributed memory computation of connection-level processing (actual number may be less, depending on processors requested!)
  int		dmem_nprocs_actual; // #READ_ONLY #NO_SAVE actual number of processors being used

  Usr1SaveFmt	usr1_save_fmt;	// #CAT_File save network for -USR1 signal: full net or weights
  WtSaveFormat	wt_save_fmt;	// #CAT_File format to save weights in if saving weights
  LayerLayout	lay_layout;	// #CAT_Display Visual mode of layer position/view

  int		n_units;	// #READ_ONLY #EXPERT #CAT_Structure total number of units in the network
  int		n_cons;		// #READ_ONLY #EXPERT #CAT_Structure total number of connections in the network
  PosTDCoord	max_disp_size;	// #READ_ONLY #EXPERT #CAT_Structure maximum display size in each dimension of the net

  ProjectBase*	proj;		// #IGNORE ProjectBase this network is in
  bool		old_load_cons;	// #IGNORE #NO_SAVE special flag (can't use flags b/c it is saved, loaded!) for case when loading a project with old cons file format (no pre-alloc of cons)

  inline void		SetNetFlag(NetFlags flg)   { flags = (NetFlags)(flags | flg); }
  // set flag state on
  inline void		ClearNetFlag(NetFlags flg) { flags = (NetFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool		HasNetFlag(NetFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void		SetNetFlagState(NetFlags flg, bool on)
  { if(on) SetNetFlag(flg); else ClearNetFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  // todo: these seem pretty lame:
  virtual int	GetDefaultX();  // #IGNORE 
  virtual int	GetDefaultY();  // #IGNORE
  virtual int	GetDefaultZ();  // #IGNORE

  void  Build();
  // #BUTTON #CAT_Structure Build the network units and Connect them (calls CheckSpecs/BuildLayers/Units/Prjns and Connect)
    virtual void  CheckSpecs();
    // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig
    virtual void  BuildLayers();
    // #MENU #MENU_ON_Structure #CAT_Structure Build any network layers that are dynamically constructed
    virtual void  BuildUnits();
    // #MENU #CAT_Structure Build the network units in layers according to geometry
    virtual void 	BuildUnits_Threads();
    // #IGNORE build unit-level thread information: flat list of units, etc, and update thread structures
    virtual void  BuildPrjns();
    // #MENU #CAT_Structure Build any network prjns that are dynamically constructed
  void	Connect();
  // #MENU #CAT_Structure Connect this network according to projections on Layers

  virtual bool	CheckBuild(bool quiet=false);
  // #CAT_Structure check if network units are built 
  virtual bool	CheckConnect(bool quiet=false);
  // #CAT_Structure check if network is connected

  virtual void	UpdtAfterNetMod();
  // #CAT_ObjectMgmt update network after any network modification (calls appropriate functions)
  virtual void	SetUnitType(TypeDef* td);
  // #MENU #TYPE_Unit #CAT_Structure set unit type for all units in layer (created by Build)

  virtual void	SyncSendPrjns();
  // #CAT_Structure synchronize sending projections with the recv projections so everyone's happy
  virtual void 	CountRecvCons();
  // #CAT_Structure count recv connections for all units in network
  virtual bool 	RecvOwnsCons() { return true; }
  // #CAT_Structure does the receiver own the connections (default) or does the sender?

  virtual void	ConnectUnits(Unit* u_to, Unit* u_from=NULL, bool record=true,
			     ConSpec* conspec=NULL);
  // #CAT_Structure connect u1 so that it recieves from u2. Create projection if necessary

  virtual void	RemoveCons();
  // #MENU #MENU_ON_Structure #CONFIRM #MENU_SEP_BEFORE #CAT_Structure Remove all connections in network
  virtual void	RemoveUnits();
  // #MENU #CONFIRM #CAT_Structure Remove all units in network (preserving unit groups)
  virtual void	RemoveUnitGroups();
  // #MENU #CONFIRM #CAT_Structure Remove all unit groups in network

  virtual void	LinkPtrCons();
  // #IGNORE link pointer connections from the corresponding owned connections -- only needed after a Copy
  virtual void	FixPrjnIndexes();
  // #CAT_Structure fix the projection indicies of the connection groups (recv_idx, send_idx)

  virtual void	Copy_Weights(const Network* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_ObjectMgmt copies weights from other network (incl wts assoc with unit bias member)
  virtual void	SaveWeights_strm(ostream& strm, WtSaveFormat fmt = NET_FMT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual bool	LoadWeights_strm(istream& strm, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (fmt is read from file)
  virtual void	SaveWeights(const String& fname="", WtSaveFormat fmt = NET_FMT);
  // #BUTTON #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual bool	LoadWeights(const String& fname="", bool quiet = false);
  // #BUTTON #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (fmt is read from file) (leave fname empty to pull up file chooser)
//NOTE: if any of the Build or Connect are to be extended, the code must be rewritten by
//  calling an inner extensible virtual _impl

  virtual Layer* NewLayer();
  // #BUTTON create a new layer in the network, using default layer type

  virtual void	MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this network
  virtual void	RemoveMonitors();
  // #CAT_ObjectMgmt Remove monitoring of all objects in all processes associated with parent project
  virtual void	UpdateMonitors();
  // #CAT_ObjectMgmt Update monitoring of all objects in all processes associated with parent project
  virtual void	NetControlPanel(SelectEdit* editor, const String& extra_label = "",
				const String& sub_gp_nm = "");
  // #MENU #MENU_ON_SelectEdit #MENU_SEP_BEFORE #NULL_OK_0  #NULL_TEXT_0_NewEditor #CAT_Display add the key network counters and statistics to a select edit dialog (control panel) (if editor is NULL, a new one is created in .edits).  The extra label is prepended to each member name, and if sub_group, then all items are placed in a subgroup with the network's name.  NOTE: be sure to click update_after on NetCounterInit and Incr at appropriate program level(s) to trigger updates of select edit display (typically in Train to update epoch -- auto update of all after Step so only needed for continuous update during runnign)

  virtual bool	SnapVar();
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic take a snapshot of currently selected variable in netview -- copies this value to the snap unit variable
  virtual bool	SnapAnd();
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic do an AND-like MIN computation of the current snap unit variable and the current value of the variable shown in netview -- shows the intersection between current state and previously snap'd state
  virtual bool	SnapOr();
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic do an OR-like MAX computation of the current snap unit variable and the current value of the variable shown in netview -- shows the union between current state and previously snap'd state
  virtual bool	SnapThresh(float thresh_val = 0.5f);
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic take a snapshot of currently selected variable in netview -- copies this value to the snap unit variable, but also applies a thresholding such that values above the thresh_val are set to 1 and values below the thresh_val are set to 0
  virtual bool	Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic take a snapshot of given variable (if empty, currently viewed variable in netview is used): assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.  

#ifdef TA_GUI
  virtual NetView* NewView(T3DataViewFrame* fr = NULL);
  // #NULL_OK #NULL_TEXT_0_NewFrame #MENU_BUTTON #MENU_ON_NetView #CAT_Display make a new viewer of this network (NULL=use existing empty frame if any, else make new frame)
  virtual NetView* FindMakeView(T3DataViewFrame* fr = NULL);
  // #CAT_Display find existing or make a new viewer of this network (NULL=use existing empty frame if any, else make new frame)
  virtual NetView* FindView();
  // #CAT_Display find (first) existing viewer of this network
  virtual String GetViewVar();
  // #CAT_Display get the currently viewed variable name from netview
  virtual bool 	SetViewVar(const String& view_var);
  // #CAT_Display set the variable name to view in the netview
  virtual Unit* GetViewSrcU();
  // #CAT_Display get the currently picked source unit (for viewing weights) from netview
  virtual bool 	SetViewSrcU(Unit* src_u);
  // #CAT_Display set the picked source unit (for viewing weights) in netview
#endif

  virtual void	PlaceNetText(NetTextLoc net_text_loc, float scale = 1.0f);
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display locate the network text data display (counters, statistics -- typically shown at bottom of network) in a new standard location (it can also be dragged anywhere in the net view, turn on lay_mv button and click on red arrow) -- can also change the scaling
  virtual NetViewObj* NewViewText(const String& txt);
  // #MENU_BUTTON #MENU_ON_NetView #MENU_SEP_BEFORE #CAT_Display add a new text label to the network view objects -- this is an arbitrary fixed text label that can be placed anywhere in the display for annotating the model or other view elements
  virtual NetViewObj* NewGlassBrain();
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display add a new glass brain (as two separate hemispheres) to netview objects -- useful for situating biologically-based network models
  virtual void	NetTextUserData();
  // #IGNORE auto-called in InitLinks -- enable the filtering of what information is shown in the network text data display (typically shown at bottom of network, though see PlaceNetText for options on where to locate) -- this function creates entries for each of the viewable items in the UserData for this network -- just click on the UserData button to edit which items to display.
  virtual void		HistMovie(int x_size=640, int y_size=480, 
				  const String& fname_stub = "movie_img_");
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display record individual frames of the netview display from current position through to the end of the history buffer, as movie frames -- use mjpeg tools http://mjpeg.sourceforge.net/ (pipe png2yuv into mpeg2enc) to compile the individual PNG frames into an MPEG movie, which can then be transcoded (e.g., using VLC) into any number of other formats

  ////////////////////////////////////////////////////////////////////////////////
  //	Below are the primary computational interface to the Network Objects
  //	for performing algorithm-specific activation and learning
  //	Many functions operate directly on the units via threads, with
  //	optional call through to the layers for any layer-level subsequent processing

  virtual void  Init_InputData();
  // #CAT_Activation Initializes external and target inputs
  virtual void  Init_Acts();
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Activation initialize the unit activation state variables
  virtual void  Init_dWt();
  // #CAT_Learning Initialize the weight change variables
  virtual void  Init_Weights();
  // #MENU #CONFIRM #CAT_Learning Initialize the weights -- also inits acts, counters and stats
  virtual void	Init_Weights_post();
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  virtual void	Init_Metrics(); 
  // #CAT_Statistic this is an omnibus guy that initializes every metric: Counters, Stats, and Timers
  
  virtual void	Init_Counters();
  // #EXPERT #CAT_Counter initialize all counter variables on network (called in Init_Weights; except batch because that loops over inits!)
  virtual void	Init_Stats();
  // #EXPERT #CAT_Statistic initialize statistic variables on network
  virtual void	Init_Timers();
  // #EXPERT #CAT_Statistic initialize statistic variables on network

  virtual void	Init_Sequence()	{ };
  // #CAT_Activation called by NetGroupedDataLoop at the start of a sequence (group) of input data events -- some algorithms may want to have a flag to optionally initialize activations at this point
  
  virtual void	Compute_Netin();
  // #CAT_Activation Compute NetInput: weighted activation from other units
  virtual void	Send_Netin();
  // #CAT_Activation sender-based computation of net input: weighted activation from other units
  virtual void	Compute_Act();
  // #CAT_Activation Compute Activation based on net input
  virtual void	Compute_NetinAct();
  // #CAT_Activation compute net input from other units and then our own activation value based on that -- use this for feedforward networks to propagate activation through network in one compute cycle

  virtual void	Compute_dWt();
  // #CAT_Learning compute weight changes -- the essence of learning

  virtual bool	Compute_Weights_Test(int trial_no);
  // #CAT_Learning check to see if it is time to update the weights based on the given number of completed trials (typically trial counter + 1): if ON_LINE, always true; if SMALL_BATCH, only if trial_no % batch_n_eff == 0; if BATCH, never (check at end of epoch and run then)
  virtual void	Compute_Weights();
  // #CAT_Learning update weights for whole net: calls DMem_SumDWts before doing update if in dmem mode
  virtual void	Compute_Weights_impl();
  // #CAT_Learning just the weight update routine: update weights from delta-weight changes

  virtual void	Compute_SSE(bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activations vs targets over the entire network -- optionally taking the average over units, and square root of the final results
  virtual void	Compute_PRerr();
  // #CAT_Statistic compute precision and recall error statistics over entire network -- true positive, false positive, and false negative -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance on a per unit basis

  ////////////////////////////////////////////////////////////////////////////////
  //	The following are misc functionality not required for primary computing

  virtual void	Compute_TrialStats();
  // #CAT_Statistic compute trial-level statistics (SSE and others defined by specific algorithms)
  virtual void	DMem_ShareTrialData(DataTable* dt, int n_rows = 1);
  // #CAT_DMem share trial data from given datatable across the trial-level dmem communicator (outer loop) -- each processor gets data from all other processors; if called every trial with n_rows = 1, data will be identical to non-dmem; if called at end of epoch with n_rows = -1 data will be grouped by processor but this is more efficient

  virtual void	Compute_EpochSSE();
  // #CAT_Statistic compute epoch-level sum squared error and related statistics
  virtual void	Compute_EpochPRerr();
  // #CAT_Statistic compute epoch-level precision and recall statistics
  virtual void	Compute_EpochStats();
  // #CAT_Statistic compute epoch-level statistics; calls DMem_ComputeAggs (if dmem) and EpochSSE -- specific algos may add more


  virtual void	LayerZPos_Unitize();
  // #MENU #MENU_ON_Structure #CAT_Structure set layer z axis positions to unitary increments (0, 1, 2.. etc)
  virtual void	LayerPos_Cleanup();
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Structure cleanup the layer positions relative to each other (prevent overlap etc)

  virtual void	Compute_LayerDistances();
  // #MENU #MENU_ON_Structure #MENU_SEP_BEFORE #CONFIRM #CAT_Structure compute distances between layers and input/output layers
  virtual void	Compute_PrjnDirections();
  // #MENU #CONFIRM #CAT_Structure compute the directions of projections based on the relative distances from input/output layers (calls Compute_LayerDistances first)

  virtual void	SetUnitNames(bool force_use_unit_names = false);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Structure for all layers, set unit names from unit_names matrix (called automatically on Build) -- also ensures unit_names fits geometry of layer -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
  virtual void 	SetUnitNamesFromDataTable(DataTable* unit_names_table, int max_unit_chars=-1,
					  bool propagate_names=false);
  // #MENU #MENU_ON_State #CAT_Structure label units in the network based on unit names table -- also sets the unit_names matrix in the layer so they are persistent -- max_unit_chars is max length of name to apply to unit (-1 = all) -- if propagate_names is set, then names will be propagated along one-to-one projections to other layers that do not have names in the table (GetLocalistName)
  virtual void	GetUnitNames(bool force_use_unit_names = true);
  // #MENU #MENU_ON_State #CAT_Structure for all layers, get unit_names matrix values from current unit name values -- also ensures unit_names fits geometry of layer -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
  virtual void	GetLocalistName();
  // #CAT_Structure look for a receiving projection from a single unit, which has a name: if found, set our name to that name

  virtual void	TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void	AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int	PruneCons(const SimpleMathSpec& pre_proc,
			     Relation::Relations rel, float cmp_val);
  // #MENU #MENU_ON_Structure #MENU_SEP_BEFORE #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int	ProbAddCons(float p_add_con, float init_wt = 0.0);
  // #MENU #USE_RVAL #CAT_Structure probabilistically add new connections (assuming prior pruning), init_wt = initial weight value of new connection
  virtual int	LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)
  virtual int	LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove units with prob p_lesion (permute = fixed no. lesioned)

  virtual void	TwoD_Or_ThreeD(LayerLayout layout_type);
  // #MENU #MENU_ON_Structure #MENU_SEP_BEFORE #CAT_Display Set 2d or 3d and reposition and redraw layers

  virtual void	WeightsToTable(DataTable* dt, Layer* recv_lay, Layer* send_lay);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send entire set of weights from sending layer to recv layer in given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit
  virtual void	VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (if a network, one col, if a layer, number of layers, etc).  for projection data, specify: prjns.xxx  for weight values, specify r. or s. (e.g., r.wt) -- this uses a NetMonitor internally (just does AddNetwork with variable, then gets data), so see documentation there for more information
  virtual void	ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
	       const String& var3 = "", const String& var4 = "", const String& var5 = "",
	       const String& var6 = "", const String& var7 = "", const String& var8 = "",
	       const String& var9 = "", const String& var10 = "", const String& var11 = "",
	       const String& var12 = "", const String& var13 = "", const String& var14 = "");
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var"
  virtual bool	VarToVarCopy(const String& dest_var, const String& src_var);
  // #CAT_Structure copy one unit variable to another (un->dest_var = un->src_var) for all units within this network (must be a float type variable)
  virtual bool	VarToVal(const String& dest_var, float val);
  // #CAT_Structure set variable to given value for all units within this network (must be a float type variable)

  virtual void	ProjectUnitWeights(Unit* un, int top_k_un = 5, int top_k_gp=1, bool swt = false,
				   bool zero_sub_hiddens=false);
  // #CAT_Statistic project given unit's weights (receiving unless swt = true) through all layers (without any loops) -- results stored in wt_prjn on each unit (tmp_calc1 is used as a sum variable).  top_k_un (< 1 = all) is number of strongest units to allow to pass information further down the chain -- lower numbers generally make the info more interpretable.  top_k_gp is number of unit groups to process for filtering through, if layer has sub groups (< 1 = ignore subgroups). values are always normalized at each layer to prevent exponential decrease/increase effects, so results are only relative indications of influence -- if zero_sub_hiddens then intermediate hidden units (indicated by layer_type == HIDDEN) that have sub-threshold values zeroed

  virtual bool	UpdateUnitSpecs(bool force = false);
  // #CAT_Structure update unit specs for entire network (calls layer version of this function)
  virtual bool	UpdateConSpecs(bool force = false);
  // #CAT_Structure update con specs for entire network (calls layer version of this function)
  virtual bool	UpdateAllSpecs(bool force = false);
  // #CAT_Structure update all unit and con specs -- just calls above two functions

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
  virtual BaseSpec_Group* FindMakeSpecGp(const String& nm, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a given spec group and if not found, make it
  virtual BaseSpec* FindMakeSpec(const String& nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a given spec and if not found, make it
  virtual BaseSpec* FindSpecName(const String& nm);
  // #CAT_Structure find a given spec by name
  virtual BaseSpec* FindSpecType(TypeDef* td);
  // #CAT_Structure find a given spec by type

  virtual Layer* FindMakeLayer(const String& nm, TypeDef* td = NULL,
			       bool& nw_itm = nw_itm_def_arg, const String& alt_nm = NULL);
  // #CAT_Structure find a given layer and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
  virtual Layer_Group* FindMakeLayerGroup(const String& nm, TypeDef* td = NULL,
			    bool& nw_itm = nw_itm_def_arg, const String& alt_nm = NULL);
  // #CAT_Structure find a given layer group and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
  virtual Layer* FindLayer(const String& nm) { return (Layer*)layers.FindLeafName(nm); }
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
  virtual bool   RemoveLayer(const String& nm) { return layers.RemoveName(nm); }
  // #CAT_Structure remove layer with given name, if it exists

  virtual void	UpdateMaxDispSize();
  // #IGNORE update max_disp_size of network based on current layer layout

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
  virtual void 	DMem_UpdtWtUpdt();
  // #CAT_DMem update wt_update and small_batch parameters for dmem, depending on trl_comm.nprocs
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

  override int	Dump_Load_Value(istream& strm, taBase* par=NULL);
  override int 	Save_strm(ostream& strm, taBase* par=NULL, int indent=0);
  override int 	Dump_Save_impl(ostream& strm, taBase* par=NULL, int indent=0);

  override String 	GetTypeDecoKey() const { return "Network"; }

  override bool	ChangeMyType(TypeDef* new_type);

  void 	InitLinks();
  void	CutLinks();
  void 	Copy_(const Network& cp);
  TA_BASEFUNS(Network);
  
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void	CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void 	Initialize();
  void 	Destroy();
};

class EMERGENT_API Network_Group : public taGroup<Network> {
  // ##FILETYPE_Network ##EXT_net ##COMPRESS ##CAT_Network ##EXPAND_DEF_2 a group of networks
INHERITED(taGroup<Network>)
public:
  
  override String 	GetTypeDecoKey() const { return "Network"; }

  TA_BASEFUNS_NOCOPY(Network_Group); //
private:
  void	Initialize() 		{SetBaseType(&TA_Network);}
  void 	Destroy()		{ };
};

//////////////////////////////////////////////////////////
// 	Inline Connection-level functions (fast)	//
//////////////////////////////////////////////////////////

// NOTE: most computationally intensive of these are written in
// optimized form assuming that the recv group owns the connections
// which is the default for most algos -- if using sender-own, then
// DEFINITELY need to re-write!!

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

inline void ConSpec::C_AddRndWeights(RecvCons*, Connection* cn, Unit* ru, Unit* su) {
  if(rnd.type != Random::NONE)	{ // don't do anything (e.g. so connect fun can do it)
    cn->wt += rnd.Gen();
  }
  else {
    rnd.Gen();		// keep random seeds syncronized for dmem
  }
  C_ApplyLimits(cn,ru,su);
}

inline void ConSpec::Init_Weights(RecvCons* cg, Unit* ru) {
  Projection* prjn = cg->prjn;
  if(prjn->spec->init_wts) {
    prjn->C_Init_Weights(cg, ru); // NOTE: this must call PrjnSpec::C_Init_Weights which does basic ConSpec C_Init_Weights
    if(prjn->spec->add_rnd_wts) {
      CON_GROUP_LOOP(cg, C_AddRndWeights(cg, cg->Cn(i), ru, cg->Un(i)));
    }
  }
  else {
    CON_GROUP_LOOP(cg, C_Init_Weights(cg, cg->Cn(i), ru, cg->Un(i)));
  }

  Init_dWt(cg,ru);
  ApplySymmetry(cg,ru);
}

inline void ConSpec::Init_Weights_post(BaseCons* cg, Unit* ru) {
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
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin(cg->OwnCn(i), ru, cg->Un(i)));
  return rval;
}

inline void ConSpec::C_Send_Netin(Connection* cn, float* send_netin_vec, Unit* ru, float su_act) {
  send_netin_vec[ru->flat_idx] += cn->wt * su_act;
}
inline void ConSpec::Send_Netin(SendCons* cg, Network* net, int thread_no, Unit* su) {
  const float su_act = su->act;
  float* send_netin_vec = net->send_netin_tmp.el + net->send_netin_tmp.FastElIndex(0, thread_no);
  CON_GROUP_LOOP(cg, C_Send_Netin(cg->OwnCn(i), send_netin_vec, cg->Un(i), su_act));
}

inline float ConSpec::C_Compute_Dist(Connection* cn, Unit*, Unit* su) {
  float tmp = su->act - cn->wt;
  return tmp * tmp;
}
inline float ConSpec::Compute_Dist(RecvCons* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Dist(cg->OwnCn(i), ru, cg->Un(i)));
  return rval;
}

inline void ConSpec::Compute_Weights(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_Weights(cg->OwnCn(i), ru, cg->Un(i)));
  ApplyLimits(cg,ru); // ApplySymmetry(cg,ru);  don't apply symmetry during learning..
}

inline void ConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt(cg->OwnCn(i), ru, cg->Un(i)));
}


#endif /* netstru_h */
