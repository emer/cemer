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
#include "ta_time.h"
#include "ta_dmem.h"
#include "ta_engine.h"

#include "emergent_base.h"
#include "spec.h"

// pre-declare:
class Connection;
class ConSpec;
class ConArray;
class UnitPtrList;
class RecvCons;
class RecvCons_List;
class ConPtrList;
class SendCons;
class SendCons_List;
class UnitSpec;
class Unit;
SmartRef_Of(Unit,TA_Unit); // UnitRef
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
  // ##CAT_Network A schedule for parameters that vary over time
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
  float 	wt;		// weight of connection
  float		dwt;		// #NO_VIEW #NO_SAVE resulting net weight change

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
  for(int i=0; i<cg->cons.size; i++) \
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
  float		min;		// #CONDEDIT_OFF_type:NONE,LT_MAX minimum weight value (if applicable)
  float		max;		// #CONDEDIT_OFF_type:NONE,GT_MIN maximum weight value (if applicable)
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
  Random	rnd;		// #CAT_ConSpec Weight randomization specification.  Note that NONE means no value at all, not the mean, and should be used if some other source is setting the weights, e.g., from a projectionspec or loading from a file etc
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
  inline virtual void 	Init_Weights(RecvCons* cg, Unit* ru);
  // #CAT_Learning initialize state variables (ie. at beginning of training)
  inline virtual void	C_Init_Weights_post(RecvCons*, Connection*, Unit*, Unit*) { };
  // #IGNORE
  inline virtual void 	Init_Weights_post(RecvCons* cg, Unit* ru);
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
  override bool 	CheckType_impl(TypeDef* td);
  override bool		CheckObjectType_impl(taBase* obj); // don't do checking on 1st con group in units
private:
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
};

SpecPtr_of(ConSpec);


/////////////////////////////////////////////////////////////////////////////////
//		RecvCons


class EMERGENT_API  ConArray : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network a physically contiguous array of connections, for receiving con group -- only one alloc of connections is allowed (to preserve validity of links to existing connections)
INHERITED(taOBase)
public:
  int		con_size;	// #READ_ONLY #EXPERT #NO_SAVE sizeof() connection object being stored
  TypeDef*	con_type;	// type of connection object being stored
  int 		size;		// #NO_SAVE #READ_ONLY #SHOW number of elements in the array
  int		alloc_size;	// #READ_ONLY #NO_SAVE #EXPERT allocated (physical) size, in con_size units -- this is not incrementally allocated -- must be done in advance of making connections!
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
  // #CAT_Modify allocate storage for exactly the given size -- frees any existing connections!
  void			Free();
  // #CAT_Modify deallocate all storage
  void			SetSize(int sz);
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

  override String 	GetTypeDecoKey() const { return "Connection"; }

  void	CutLinks();
  void	Copy_(const ConArray& cp);
  TA_BASEFUNS(ConArray);
private:
  void 	Initialize();
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

class EMERGENT_API RecvCons : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network receiving connections: owns all the connection objects
INHERITED(taOBase)
public:
  static int	no_idx;			// #HIDDEN pass to find if you don't want one

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
  Projection*	prjn;
  // #CAT_Structure pointer the the projection which created this Group
  int		send_idx;
  // #READ_ONLY index into sending unit's send. list of SendCons

  Connection* 	Cn(int i) const { return cons.FastEl(i); }
  // #CAT_Structure gets the connection at the given index
  Unit*		Un(int i) const { return units.FastEl(i); }
  // #CAT_Structure gets the unit at the given index

  inline ConSpec* GetConSpec() const { return m_con_spec; }
  // #CAT_Structure get the con spec for this connection group -- this is controlled entirely by the projection con_spec
  inline void	SetConSpec(ConSpec* cs) { m_con_spec = cs; }
  // #CAT_Structure set the con spec to given value -- no ref counting or other checking is done -- should generally only be called by the Projection

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

  virtual void	MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this set of receiving connections

  virtual Connection*	FindConFrom(Unit* un, int& idx=no_idx) const;
  // #MENU #MENU_ON_Actions #USE_RVAL #ARGC_1 #CAT_Structure find connection from given unit
  static Connection* 	FindRecipRecvCon(Unit* su, Unit* ru, Layer* ru_lay);
  // #CAT_Structure find the reciprocal for sending unit su to this receiving unit ru
  static Connection* 	FindRecipSendCon(Unit* ru, Unit* su, Layer* su_lay);
  // #CAT_Structure find the reciprocal for receiving unit ru from this sending unit su

  ////////////////////////////////////////////////////////////////////////////////
  //	Below are the primary computational interface to the Network Objects
  //	for performing algorithm-specific activation and learning

  void 	Init_Weights(Unit* ru)	 	{ GetConSpec()->Init_Weights(this,ru); }
  // #CAT_Learning initialize weights for group
  void 	C_Init_Weights(Connection* cn, Unit* ru, Unit* su)
  { GetConSpec()->C_Init_Weights(this, cn, ru, su); }
  // #CAT_Learning initialize weights for single connection
  void	Init_Weights_post(Unit* ru) 	{ GetConSpec()->Init_Weights_post(this,ru); }
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
  virtual int	LoadWeights_strm(istream& strm, Unit* ru, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary format) -- rval is taMisc::ReadTagStatus, TAG_END if successful
  static int 	SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
				 bool quiet = false);
  // #IGNORE skip over saved weights (to keep the file in sync) -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void	SaveWeights(const String& fname="", Unit* ru = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int	LoadWeights(const String& fname="", Unit* ru = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File  #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary format) (leave fname empty to pull up file chooser)

  int 	Dump_Save_Value(ostream& strm, taBase* par=NULL, int indent = 0);
  int	Dump_Load_Value(istream& strm, taBase* par=NULL);

  virtual void	Copy_Weights(const RecvCons* src);
  // #CAT_ObjectMgmt copies weights from other con_group

  virtual void 	LinkSendCons(Unit* ru);
  // #CAT_Structure make connection links in all the sending units (assumes that these are initially empty, as after loading or copying)

  int 	UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  int	UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  int 	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);
  bool	ChangeMyType(TypeDef* new_type);
  
  override String 	GetTypeDecoKey() const { return "Connection"; }

  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const RecvCons& cp);
  TA_BASEFUNS(RecvCons);
protected:
  ConSpec* 	m_con_spec;	// con spec that we use: controlled entirely by the projection!

  override void UpdateAfterEdit_impl();
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

class EMERGENT_API RecvCons_List: public taList<RecvCons> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network ##NO_EXPAND_ALL list of receiving connections, one per projection
INHERITED(taList<RecvCons>)
public:
  virtual RecvCons*	NewPrjn(Projection* prjn);
  // #CAT_Structure create a new sub_group from given projection, with given ownership (own_cons)
  virtual RecvCons*	FindPrjn(Projection* prjn, int& idx=no_idx) const;
  // #CAT_Structure find sub group associated with given projection
  virtual RecvCons*	FindFrom(Layer* from, int& idx=no_idx) const;
  // #MENU #USE_RVAL #ARGC_1 #CAT_Structure find sub group that receives from given layer
  virtual RecvCons*	FindFromName(const String& fm_nm, int& idx=no_idx) const;
  // #MENU #USE_RVAL #ARGC_1 #CAT_Structure find sub group that receives from given layer named fm_nm
  virtual RecvCons*	FindTypeFrom(TypeDef* prjn_typ, Layer* from, int& idx=no_idx) const;
  // #MENU #USE_RVAL #ARGC_2 #CAT_Structure find sub group that recvs prjn of given type from layer
  virtual RecvCons*	FindLayer(Layer* lay, int& idx=no_idx) const;
  // #CAT_Structure find sub group where projection is in the given layer
  virtual bool		RemovePrjn(Projection* prjn);
  // #CAT_Structure remove sub group associated with given projection
  virtual bool		RemoveFrom(Layer* from);
  // #MENU #CAT_Structure remove sub group that receives from given layer

  override String 	GetTypeDecoKey() const { return "Connection"; }

  NOCOPY(RecvCons_List)
  TA_BASEFUNS(RecvCons_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_RecvCons); }
  void 	Destroy()		{ };
};

/////////////////////////////////////////////////////////////////////////////////
//		SendCons

class EMERGENT_API ConPtrList: public taPtrList<Connection> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network list of connection pointers, for sending connections
public:
  ~ConPtrList()             { Reset(); }
};


class EMERGENT_API SendCons : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network sending connections: points to receiving connections
INHERITED(taOBase)
public:
  static int	no_idx;			// #HIDDEN pass to find if you don't want one

  TypeDef*	con_type;
  // #CAT_Structure #AKA_el_typ type of connections to make
  ConPtrList	cons;
  // #NO_FIND #NO_SAVE #CAT_Structure list of pointers to receiving connections, in index correspondence with units;
  UnitPtrList	units;
  // #NO_FIND #NO_SAVE #CAT_Structure pointers to the receiving units of this connection, in index correspondence with cons
  Projection*	prjn;
  // #CAT_Structure pointer the the projection which created this Group
  int		recv_idx;
  // #READ_ONLY index into recv unit's list of RecvCons for this projection

  Connection* 	Cn(int i) const { return cons.FastEl(i); }
  // #CAT_Structure gets the connection at the given index
  Unit*		Un(int i) const { return units.FastEl(i); }
  // #CAT_Structure gets the unit at the given index

  inline ConSpec* GetConSpec() const { return m_con_spec; }
  // #CAT_Structure get the con spec for this connection group -- this is controlled entirely by the projection con_spec
  inline void	SetConSpec(ConSpec* cs) { m_con_spec = cs; }
  // #CAT_Structure set the con spec to given value -- no ref counting or other checking is done -- should generally only be called by the Projection

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
  virtual Connection*	FindConFrom(Unit* un, int& idx=no_idx) const;
  // #MENU #MENU_ON_Actions #USE_RVAL #ARGC_1 #CAT_Structure find connection from given unit

  ////////////////////////////////////////////////////////////////////////////////
  //	Below are the primary computational interface to the Network Objects
  //	for performing algorithm-specific activation and learning

  void 	Send_Netin(Network* net, int thread_no, Unit* su)
  { GetConSpec()->Send_Netin(this, net, thread_no, su); }
  // #CAT_Activation sender-based net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units
  
  ////////////////////////////////////////////////////////////////////////////////
  //	The following are misc functionality not required for primary computing

  virtual bool	ConValuesToArray(float_Array& ary, const char* variable);
  // #CAT_Structure adds values of variable from the connections into the given array (false if var not found)
  virtual bool	ConValuesToMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure sets values of variable from the connections into the given matrix (uses flat index of cons to set: 0..size-1), returns false if matrix is not appropriately sized
  virtual bool	ConValuesFromArray(float_Array& ary, const char* variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found)
  virtual bool	ConValuesFromMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found) -- uses flat index of cons to set: 0..size-1

  virtual void	MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this set of connections

  int 	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);
  bool	ChangeMyType(TypeDef* new_type);
  
  override String 	GetTypeDecoKey() const { return "Connection"; }

  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const SendCons& cp);
  TA_BASEFUNS(SendCons);
protected:
  ConSpec* 	m_con_spec;	// con spec that we use: controlled entirely by the projection!

  override void  UpdateAfterEdit_impl();
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

class EMERGENT_API SendCons_List: public taList<SendCons> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network ##NO_EXPAND_ALL list of sending connections, one per projection
INHERITED(taList<SendCons>)
public:
  // projection-related functions for operations on sub-groups of the group
  virtual SendCons*	NewPrjn(Projection* prjn);
  // #CAT_Structure create a new sub_group from given projection, with given ownership (own_cons)
  virtual SendCons*	FindPrjn(Projection* prjn, int& idx=no_idx) const;
  // #CAT_Structure find sending connections associated with given projection
  virtual SendCons*	FindFrom(Layer* from, int& idx=no_idx) const;
  // #MENU #USE_RVAL #ARGC_1 #CAT_Structure find sending connections that receive from given layer
  virtual SendCons*	FindFromName(const String& fm_nm, int& idx=no_idx) const;
  // #MENU #USE_RVAL #ARGC_1 #CAT_Structure find sending connections that receive from given layer named fm_nm
  virtual SendCons*	FindTypeFrom(TypeDef* prjn_typ, Layer* from, int& idx=no_idx) const;
  // #MENU #USE_RVAL #ARGC_2 #CAT_Structure find sending connections that recvs prjn of given type from layer
  virtual SendCons*	FindLayer(Layer* lay, int& idx=no_idx) const;
  // #CAT_Structure find sending connections where projection is in the given layer
  virtual bool		RemovePrjn(Projection* prjn);
  // #CAT_Structure remove sending connections associated with given projection
  virtual bool		RemoveFrom(Layer* from);
  // #MENU #CAT_Structure remove sending connections from given layer

  override String 	GetTypeDecoKey() const { return "Connection"; }

  TA_BASEFUNS_NOCOPY(SendCons_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_SendCons); }
  void 	Destroy()		{ };
};


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
  // #CAT_Statistic tolerance for computing sum-squared error on a per-unit basis

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
  // #CAT_Statistic compute sum squared error for this unit


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
  override void  	UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& ok);
  override bool 	CheckType_impl(TypeDef* td);
  override bool 	CheckObjectType_impl(TAPtr obj);
private:
  void 	Initialize();
  void 	Destroy()		{ };
};

SpecPtr_of(UnitSpec);

class EMERGENT_API Unit: public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##DMEM_SHARE_SETS_3 ##CAT_Network Generic unit -- basic computational unit of a neural network (e.g., a neuron-like processing unit)
INHERITED(taOBase)
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

  String	name;		// name of unit (empty by default)
  ExtType	ext_flag;
  // #GUI_READ_ONLY #SHOW #CAT_Activation tells what kind of external input unit received
  float 	targ;
  // #CAT_Activation target value: drives learning to produce this activation value
  float 	ext;
  // #CAT_Activation external input: drives activation of unit from outside influences (e.g., sensory input)
  float 	act;
  // #DMEM_SHARE_SET_2 #CAT_Activation activation value -- what the unit communicates to others
  float 	net;
  // #DMEM_SHARE_SET_1 #CAT_Activation net input value -- what the unit receives from others (typically sum of sending activations times the weights)
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
  // #CAT_Structure position in space relative to owning group, layer
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

  
  ////////////////////////////////////////////////////////////////////////////////
  //	The following are misc functionality not required for primary computing

  virtual bool	BuildUnits();
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

  virtual void 	LinkSendCons();
  // #CAT_Structure link sending connections based on recv cons (after load, copy)

  override int	GetIndex() { return idx; }
  override void	SetIndex(int i) { idx = i; }
  virtual int	GetMyLeafIndex();
  // compute leaf index from my individual index in an efficient manner
  TwoDCoord 		GetMyAbsPos()
    {TwoDCoord r; GetLayerAbsPos(r); return r;}
  // #NO_SHOW #OBSOLETE *use GetLayerAbsPos* get the absolute position of this unit relative to the layer, taking into account any unit groups
  void			GetAbsPos(TDCoord& abs_pos)
    {abs_pos = pos; AddRelPos(abs_pos);}
    // get absolute pos, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void			GetLayerAbsPos(TwoDCoord& lay_abs_pos); 
  // get coords of this Unit in the Layer (NOT for any kind of layout/3D use, only for Unit-in-Layer usage)
  void			AddRelPos(TDCoord& rel_pos); 
  // #IGNORE add relative pos, which factors in offsets from above
  
  override String 	GetTypeDecoKey() const { return "Unit"; }

  override bool 	SetName(const String& nm)    	{ name = nm; return true; }
  override String	GetName() const			{ return name; }

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

// Projections are abrevieated prjn (as a oppesed to proj = project or proc = process)
// ProjectionSpec does the connectivity, and optionally the weight init

class EMERGENT_API ProjectionSpec : public BaseSpec {
  // #STEM_BASE #VIRT_BASE ##CAT_Spec Specifies the connectivity between layers (ie. full vs. partial)
INHERITED(BaseSpec)
public:
  bool		self_con;	// #CAT_Structure whether to create self-connections or not (if applicable)
  bool		init_wts;	// #CAT_Structure whether this projection spec does weight init (else conspec)

  virtual void 	RemoveCons(Projection* prjn);
  // #CAT_Structure deletes any existing connections
  virtual void	PreConnect(Projection* prjn);
  // #CAT_Structure Prepare to connect (init con_groups)
  virtual void	Connect_impl(Projection*) { };
  // #CAT_Structure actually implements specific connection code
  virtual void 	Connect(Projection* prjn);
  // #CAT_Structure connects the network, first removing existing cons, and inits weights
  virtual int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);
  // #CAT_Structure probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
  virtual void 	Init_dWt(Projection* prjn);
  // #CAT_Weights initializes the weight change variables
  virtual void 	Init_Weights(Projection* prjn);
  // #CAT_Weights initializes the weiht values
  virtual void 	Init_Weights_post(Projection* prjn);
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  virtual bool	CheckConnect(Projection* prjn, bool quiet=false);
  // #CAT_ObjectMgmt check if projection is connected

  virtual void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // #CAT_Weights custom initialize weights in this con group for given receiving unit ru

  override String 	GetTypeDecoKey() const { return "ProjectionSpec"; }

  void 	InitLinks();
  SIMPLE_COPY(ProjectionSpec);
  TA_BASEFUNS(ProjectionSpec);
private:
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
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

  // convenience functions for those defined in the spec
  void 	RemoveCons()		{ if((bool)spec) spec->RemoveCons(this); }
  // #MENU #MENU_ON_Actions #CONFIRM #CAT_Structure Reset all connections for this projection
  void 	PreConnect()		{ spec->PreConnect(this); }
  // #CAT_Structure pre-configure connection state
  void 	Connect()		{ spec->Connect(this); }
  // #BUTTON #CONFIRM #CAT_Structure Make all connections for this projection (resets first)
  void 	Connect_impl()		{ spec->Connect_impl(this); }
  // #CAT_Structure actually do the connecting
  int 	ProbAddCons(float p_add_con, float init_wt = 0.0) { return spec->ProbAddCons(this, p_add_con, init_wt); }
  // #MENU #USE_RVAL #CAT_Structure probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
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
  // #BUTTON #CAT_Structure #INIT_ARGVAL_ON_spec.spec set the projection spec (connectivity pattern) for this projection
  virtual bool 	SetConSpec(ConSpec* sp);
  // #BUTTON #CAT_Structure #INIT_ARGVAL_ON_con_spec.spec set the con spec for all connections in this prjn
  virtual bool	CheckConnect(bool quiet=false) { return spec->CheckConnect(this, quiet); }
  // #CAT_Structure check if projection is connected
  virtual void	FixPrjnIndexes();
  // #MENU #CAT_Structure fix the indicies of the connection groups (recv_idx, send_idx)

  virtual int	ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int	ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp

  virtual bool 	SetConType(TypeDef* td);
  // #BUTTON #CAT_Structure #TYPE_Connection #INIT_ARGVAL_ON_con_type set the connection type for all connections in this prjn
  virtual bool 	SetRecvConsType(TypeDef* td);
  // #BUTTON #CAT_Structure #TYPE_RecvCons #INIT_ARGVAL_ON_recvcons_type set the receiving connection group type for all connections in this prjn
  virtual bool 	SetSendConsType(TypeDef* td);
  // #BUTTON #CAT_Structure #TYPE_SendCons #INIT_ARGVAL_ON_sendcons_type set the connection group type for all connections in this prjn

  virtual void	MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this projection

  virtual void	WeightsToTable(DataTable* dt, const String& col_nm = "");
  // #MENU #NULL_OK  #NULL_TEXT_0_NewTable #CAT_Structure copy entire set of projection weights to given table (e.g., for analysis), with one row per receiving unit, and one column (name is layer name if not otherwise specified) that has a float matrix cell of the geometry of the sending layer
  virtual void	VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information


  override String 	GetTypeDecoKey() const { return "Projection"; }

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
  // #NO_UPDATE_AFTER ##CAT_Network ##SCOPE_Network ##NO_EXPAND_ALL a group of units -- manages structural but not computational functions
INHERITED(taGroup<Unit>)
public:
  Layer*	own_lay;	// #READ_ONLY #NO_SAVE #NO_SHOW #NO_SET_POINTER layer owner
  PosTDCoord	pos;		// #CAT_Structure position of group relative to the layer
  bool		unique_geom;	// #DEF_false #CAT_Structure if true, this unit group has a unique geometry different from the layer geometry (otherwise, geom is always copied from lay->un_geom)
  XYNGeom	geom;		// #CONDEDIT_ON_unique_geom:true #CAT_Structure geometry of the group: layout and number of units
  bool		units_lesioned;	// #GUI_READ_ONLY if units were lesioned in this group, don't complain about rebuilding!

  String	output_name;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)

  int		n_units;	// #READ_ONLY #HIDDEN #NO_SAVE obsolete v3 compatibility: number of units to create in the group; updates v4 values and is reset to 0 if !=0

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

  virtual bool	BuildUnits();
  // #MENU #MENU_ON_Actions #CAT_Structure for subgroups: build units to specs (true if changed)
  virtual bool	CheckBuild(bool quiet=false);
  // #CAT_Structure check if network is built 
  virtual void	LayoutUnits(Unit* u = NULL);
  // #CAT_Structure for subgroups: redistribute units within the given geometry of the group
  virtual void	RecomputeGeometry();
  // #CAT_Structure re compute geometry based on parent layer

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

  virtual bool	UnitValuesToArray(float_Array& ary, const char* variable);
  // #CAT_Structure adds values of variable from the units into the given array
  virtual bool	UnitValuesToMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure adds values of variable from the units into the given matrix
  virtual bool	UnitValuesFromArray(float_Array& ary, const char* variable);
  // #CAT_Structure sets unit values from values in the given array
  virtual bool	UnitValuesFromMatrix(float_Matrix& mat, const char* variable);
  // #CAT_Structure sets unit values from values in the given array

  virtual void	VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information

  Unit* 	FindUnitFmCoord(const TwoDCoord& coord)
  { return FindUnitFmCoord(coord.x,coord.y); }
  // #CAT_Structure returns unit at given coordinates within layer
  Unit*		FindUnitFmCoord(int x, int y);
  // #CAT_Structure find unit from given set of x and y coordinates
  TwoDCoord	GetGpGeomPos();
  // #CAT_Structure returns unit group position in terms of layer unit group geometry gp_geom (pos is in unit coordinates and not unit group geometry)
  void		GetAbsPos(TDCoord& abs_pos)
    {abs_pos = pos; AddRelPos(abs_pos);}
    // get absolute pos, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void		AddRelPos(TDCoord& rel_pos); //  #IGNORE add relative pos, which factors in offsets from above
  virtual bool	SetUnitNames(taMatrix* mat);
  // #CAT_XpertStructure sets the unit names from the matrix mat -- should generally be 2d but we are permissive about dims, size, etc.
  virtual bool	GetUnitNames(taMatrix* mat);
  // #CAT_XpertStructure gets current unit names into matrix mat -- should generally be 2d but we are permissive about dims, size, etc.

  // implement save_rmv_units:
  override bool	Dump_QuerySaveChildren();

  void		RemoveAll();
  
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
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
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
    OUTPUT		// layer produces a visible output response but is not a target.  any external input serves as a comparison (COMP) against current activations.
  };
  
  enum LayerFlags { 			// #BITS flags for layer
    LF_NONE		= 0, 		// #NO_BIT
    LESIONED		= 0x0001,	// this layer is temporarily lesioned (inactivated for all network-level processing functions)
    ICONIFIED		= 0x0002, 	// only display a single unit showing icon_value (set in algorithm-specific manner)
    NO_ADD_SSE		= 0x0004,	// do NOT add this layer's sse value (sum squared error) to the overall network sse value: this is for all types of SSE computed for ext_flag = TARG (layer_type = TARGET) or ext_flag = COMP (layer_type = OUTPUT) layers
    NO_ADD_COMP_SSE	= 0x0008,	// do NOT add this layer's sse value (sum squared error) to the overall network sse value: ONLY for ext_flag = COMP (OUTPUT) flag settings (NO_ADD_SSE blocks all contributions) -- this is relevant if the layer type or ext_flags are switched dynamically and only TARGET errors are relevant
    PROJECT_WTS_NEXT    = 0x0010,	// #NO_SHOW this layer is next in line for weight projection operation
    PROJECT_WTS_DONE    = 0x0020,	// #NO_SHOW this layer is done with weight projection operation (prevents loops)
    SAVE_UNITS		= 0x0040,	// save this layer's units in the project file (even if Network::SAVE_UNITS off)
    NO_SAVE_UNITS	= 0x0080,	// don't save this layer's units in the project file (even if Network::SAVE_UNITS on)
  };

  Network*		own_net;        // #READ_ONLY #NO_SAVE #NO_SHOW #CAT_Structure #NO_SET_POINTER Network this layer is in
  LayerFlags		flags;		// flags controlling various aspects of layer funcdtion
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
  Unit_Group		units;		// #CAT_Structure #NO_SEARCH units or groups of units
  UnitSpec_SPtr 	unit_spec;	// #CAT_Structure default unit specification for units in this layer
  Unit::ExtType		ext_flag;	// #CAT_Activation #GUI_READ_ONLY #SHOW indicates which kind of external input layer received
  DMemDist		dmem_dist; 	// #CAT_DMem how to distribute units across multiple distributed memory processors

  LayerDistances	dist;		// #CAT_Structure distances from closest input/output layers to this layer

  String		output_name;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)
  float			sse;		// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW sum squared error over the network, for the current external input pattern
  float			icon_value;	// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW value to display if layer is iconified (algorithmically determined)
  int			units_flat_idx;	// #READ_ONLY #NO_SAVE starting index for this layer into the network units_flat list, used in threading
  String_Matrix		unit_names; // #SHOW_TREE set unit names from corresponding items in this matrix (dims=2 for no group layer or to just label main group, dims=4 for grouped layers, dims=0 to disable)

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
  
  virtual void  BuildUnits();
  // #MENU #MENU_ON_Actions #CONFIRM #CAT_Structure build the units based current geometry configuration
  virtual void 	BuildUnits_Threads(Network* net);
  // #IGNORE build unit-level thread information: flat list of units, etc -- this is called by network BuildUnits_Threads so that layers (and layerspecs) can potentially modify which units get added to the compute lists, and thus which are subject to standard computations -- default is all units in the layer
  virtual void	RecomputeGeometry();
  // #CAT_Structure recompute the layer's geometry specifcations
  virtual void  LayoutUnits(Unit* u=NULL);
  // #ARGC_0 #CAT_Structure layout the units according to layer geometry
  virtual void  LayoutUnitGroups();
  // #MENU #CONFIRM #CAT_Structure layout the unit groups according to layer group geometry and spacing
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
  virtual void  LinkSendCons();
  // #IGNORE re-connect the layer after loading
  virtual void	DisConnect();
  // #MENU #CONFIRM #CAT_Structure disconnect layer from all others
  virtual int 	CountRecvCons();
  // #CAT_Structure count recv connections for all units in layer

  void		SetExtFlag(int flg)   { ext_flag = (Unit::ExtType)(ext_flag | flg); }
  // #CAT_Activation set external input data flag
  void		UnSetExtFlag(int flg) { ext_flag = (Unit::ExtType)(ext_flag & ~flg); }
  // #CAT_Activation un-set external input data flag

  virtual void	SetLayUnitExtFlags(int flg);
  // #CAT_Activation set external input data flags for layer and all units in the layer

  virtual void	ApplyInputData(taMatrix* data, Unit::ExtType ext_flags = Unit::NO_EXTERNAL,
      Random* ran = NULL, const PosTwoDCoord* offset = NULL, bool na_by_range=false);
  // #CAT_Activation apply the 2d or 4d external input pattern to the network, optional random additional values, and offsetting; uses a flat 2-d model where grouped layer or 4-d data are flattened to 2d; frame<0 means from end; na_by_range means that values are not applicable if they fall outside act_range on unit spec, and thus don't have flags or values set

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
  // #CAT_Statistic compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value

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
  // #MENU #CAT_Display #MENU_CONTEXT iconify this layer in the network display
  virtual void	DeIconify();
  // #MENU #CAT_Display #MENU_CONTEXT de-iconify this layer in the network display
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
  // #BUTTON #DROP1 #CAT_Structure #INIT_ARGVAL_ON_spec.spec set the layer specification
  virtual LayerSpec* GetLayerSpec()		{ return (LayerSpec*)NULL; }
  // #CAT_Structure get the layer spec for this layer (if used)
  virtual bool	SetUnitSpec(UnitSpec* unitspec);
  // #BUTTON #DROP1 #CAT_Structure #INIT_ARGVAL_ON_unit_spec.spec set unit spec for all units in layer
  virtual void	SetUnitType(TypeDef* td);
  // #BUTTON #TYPE_Unit #CAT_Structure #INIT_ARGVAL_ON_units.el_typ set unit type for all units in layer (created by Build)
  virtual void	FixPrjnIndexes();
  // #CAT_Structure fix the projection indicies of the connection groups (other_idx)

  virtual void	MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this layer (can be a variable on the units or connections as well)
  virtual bool	Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #BUTTON #CAT_Statistic take a snapshot of given variable: assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.  

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

  Unit*		FindUnitFmCoord(const TwoDCoord& coord) {return FindUnitFmCoord(coord.x, coord.y);}
  // #CAT_Structure get unit from coordinates, taking into account group geometry if present (subtracts any gp_spc -- as if it is not present).
  Unit*		FindUnitFmCoord(int x, int y);
  // #CAT_Structure find unit within layer from given coordinates (layer relative coords), taking into account group geometry if present (subtracts any gp_spc -- as if it is not present).
  Unit*		FindUnitFmGpCoord(int gp_x, int gp_y, int un_x, int un_y);
  // #CAT_Structure find unit given both unit and unit group coordinates
  Unit*		FindUnitFmGpCoord(const TwoDCoord& gp_coord, const TwoDCoord& coord)
    {return FindUnitFmGpCoord(gp_coord.x,gp_coord.y, coord.x, coord.y);}
  // #CAT_Structure get unit from both unit and group coordinates
  Unit_Group* 	FindUnitGpFmCoord(int gp_x, int gp_y);
  // #CAT_Structure find the unit group at given group coordiantes (i.e., within gp_geom, not unit coordinates)
  Unit_Group* 	FindUnitGpFmCoord(const TwoDCoord& coord) {return FindUnitGpFmCoord(coord.x,coord.y);}
  // #CAT_Structure get unit group from group coordinates (i.e., within gp_geom, not unit coordinates)
  virtual void	GetActGeomNoSpc(TwoDCoord& nospc_geom);
  // #CAT_Structure get the actual geometry of the layer, subtracting any gp_spc that might be present (as if there were no spaces between unit groups)
  void		GetAbsPos(TDCoord& abs_pos)
    {abs_pos = pos; AddRelPos(abs_pos);}
    // get absolute pos, which factors in offsets from layer groups
  void		AddRelPos(TDCoord& rel_pos); //  #IGNORE add relative pos, which factors in offsets from above
  void		SetDefaultPos();
  // #IGNORE initialize position of layer

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

  override int	GetEnabled() const { return !lesioned(); }
  override String GetTypeDecoKey() const { return "Layer"; }

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md);
  void 	UpdateAfterEdit();
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
  virtual void		ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
			       Random* ran, const TwoDCoord& offs, bool na_by_range=false);
  // #IGNORE 2d data is always treated the same: FindUnitFmCoord deals with unit grouping
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

inline Layer* Unit::own_lay() const {return ((Unit_Group*)owner)->own_lay;}
inline bool Unit::lesioned() const {return own_lay()->lesioned(); }

class EMERGENT_API Layer_Group : public taGroup<Layer> {
  // ##CAT_Network ##SCOPE_Network group of layers 
INHERITED(taGroup<Layer>)
public:
  TDCoord	pos;		// Position of Group of layers relative to network

  void		GetAbsPos(TDCoord& abs_pos)
    {abs_pos = pos; AddRelPos(abs_pos);}
    // get absolute pos, which factors in offsets from layer groups
  void		AddRelPos(TDCoord& rel_pos); // add relative pos, which factors in offsets from layer groups
  
  virtual void		BuildLayers(); // #CAT_Structure create any algorithmically specified layers
  virtual void		BuildPrjns(); // #CAT_Structure create any algorithmically specified prjns
  virtual void		Clean();
    // #MENU #MENU_CONTEXT #CAT_Structure remove any algorithmically specified layers/prjns etc.
    
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
  void  Copy_(const Layer_Group& cp)	{ pos = cp.pos; }
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
  float		chunk_pct;	// #MIN_0 #MAX_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  proportion (0-1) of units to process in a chunked fashion, where units are allocated in (interdigitated) chunks to threads to exclusively process -- after this, each available thread works nibbling a unit at a time on the remaining list of units.  A middle value around .5 is typically best, as it balances between the efficiency of pre-allocating the load, and the load balancing of the nibbling which adapts automatically to effective processor loads.
  int		nibble_chunk;	// #MIN_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  how many units does each thread grab to process while nibbling?  Too small a value results in increased contention and inefficiency, while too large a value results in poor load balancing across processors.
  float		compute_thr;	// #MIN_0 #MAX_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  threshold value for amount of computation in a given function to actually deploy on threads, as opposed to just running it on main thread -- value is normalized (0-1) with 1 being the most computationally intensive task, and 0 being the least -- as with min_units, it may not be worth it to parallelize very lightweight computations.  See Thread_Params page on emergent wiki for relevant comparison values.
  int		min_units;	// #MIN_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  minimum number of units required to use threads at all -- for feedforward algorithms requiring layer-level syncing, this applies to each layer -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.
  bool		send_netin;	// #NO_SAVE NOTE: not saved -- initialized from user prefs.  should the Send_Netin call be threaded or not?  this can actually be slower on some machiness due to memory dispersion issues, and it also results in small numerical differences from the single-threaded case.
  bool		interleave;	// #DEF_true #EXPERT deploy threads in an interleaved fashion over units, which improves load balancing as neighboring units are likely to have similar compute demands, but it costs in cache coherency as the memory access per processor is more distributed -- only affects network level (non lay_sync) processes
  bool		ignore_lay_sync;// #DEF_false ignore need to sync at the layer level for feedforward algorithms that require this (e.g., backprop) -- results in faster but less accurate processing
  int		nibble_i;	// #IGNORE current nibble index -- atomic incremented by working threads to nibble away the rest..
  int 		nibble_stop;	// #IGNORE nibble stopping value
  bool	       	using_threads;	// #READ_ONLY #NO_SAVE are we currently using threads for a computation or not -- also useful for just after a thread call to see if threads were used

  Network*	network() 	{ return (Network*)owner; }

  void		InitAll();	// initialize threads and tasks

  void		Run(ThreadUnitCall* unit_call, float comp_load,
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
//		Net View

class EMERGENT_API NetViewFontSizes : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Display network display font sizes
INHERITED(taOBase)
public:
  float	 net_name;	// #DEF_0.05 network name
  float	 net_vals;	// #DEF_0.05 network values (counters, stats)
  float	 layer;		// #DEF_0.04 layer names
  float	 layer_vals;	// #DEF_0.03 layer values (stats)
  float  prjn;		// #DEF_0.01 projection names and values
  float	 unit;		// #DEF_0.02 unit names and values
  int	 un_nm_len;	// #DEF_3 unit name length -- used to compute output name font size

  override String 	GetTypeDecoKey() const { return "Network"; }

  SIMPLE_COPY(NetViewFontSizes);
  TA_BASEFUNS(NetViewFontSizes);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class EMERGENT_API NetViewParams : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Display misc parameters for the network display
INHERITED(taOBase)
public:
  enum PrjnDisp {		// how to display projections
    L_R_F,			// all in front: sender is at left of layer, receiver is right
    L_R_B,			// all in back: sender is at left of layer, receiver is right
    B_F,			// sender is at back of layer, receiver is front
  };

  bool		xy_square;	// keep the x and y dimensions of the network square (same) -- makes the units square
  float		unit_spacing;	// #DEF_0.05 spacing between units (as a proportion of total space available to render the unit)
  PrjnDisp	prjn_disp;	// how to arrange projection arrows to convey sender/receiver info
  bool		prjn_name;	// #DEF_false whether to display the projection name
  float		prjn_width;	// #DEF_0.002 width of the projection arrows
  float		prjn_trans;	// #DEF_0.5 transparency of the projection arrows
  float		lay_trans;	// #DEF_0.5 transparency of the layer border
  float		unit_trans;	// #DEF_0.6 transparency of the units

  override String 	GetTypeDecoKey() const { return "Network"; }

  TA_SIMPLE_BASEFUNS(NetViewParams);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

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
  String	obj_fname;	// #CONDEDIT_ON_obj_type:OBJECT #FILE_DIALOG_LOAD #EXT_iv,wrl #FILETYPE_OpenInventor file name of Open Inventor file that contains the 3d geometry of the object
  String	text;		// #CONDEDIT_ON_obj_type:TEXT text to display for text type of object
  float		font_size;	// #CONDEDIT_ON_obj_type:TEXT font size to display text in, in normalized units (the entire network is 1x1x1, so this should typically be a smaller fraction like .05)
  bool		set_color;	// if true, we directly set our own color (otherwise it is whatever the object defaults to)
  taColor	color; 		// #CONDEDIT_ON_set_color default color if not otherwise defined (a=alpha used for transparency)

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
  // ##FILETYPE_Network ##EXT_net ##COMPRESS ##CAT_Network ##DEF_NAME_ROOT_Network A network, containing layers, units, etc..
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
    MANUAL_POS	 	= 0x0004, // disables the automatic cleanup/positioning of layers  (turn on to use Layer_Groups to position) 
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
  WtUpdate	wt_update;	// #CAT_Learning #CONDEDIT_ON_train_mode:TRAIN weight update mode: when are weights updated (only applicable if train_mode = TRAIN)
  int		small_batch_n;	// #CONDEDIT_ON_wt_update:SMALL_BATCH #CAT_Learning number of events for small_batch learning mode (specifies how often weight changes are synchronized in dmem)
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

  float	       	cur_sum_sse;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current sum_sse -- used during computation of sum_sse
  int	       	avg_sse_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic number of times cur_sum_sse updated: for computing avg_sse
  float	       	cur_cnt_err;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current cnt_err -- used for computing cnt_err

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
  PosTDCoord	max_size;	// #READ_ONLY #EXPERT #CAT_Structure maximum size in each dimension of the net

  NetViewFontSizes font_sizes;   // #CAT_Display default size of display labels when a new view is made (can be overriden in specific views)
  NetViewParams	view_params;   // #CAT_Display misc netview parameters

  ProjectBase*	proj;		// #IGNORE ProjectBase this network is in

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

  virtual void	Copy_Weights(const Network* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_ObjectMgmt copies weights from other network (incl wts assoc with unit bias member)
  virtual void	SaveWeights_strm(ostream& strm, WtSaveFormat fmt = NET_FMT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual bool	LoadWeights_strm(istream& strm, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (fmt is read from file)
  virtual void	SaveWeights(const String& fname="", WtSaveFormat fmt = NET_FMT);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual bool	LoadWeights(const String& fname="", bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (fmt is read from file) (leave fname empty to pull up file chooser)
//NOTE: if any of the Build or Connect are to be extended, the code must be rewritten by
//  calling an inner extensible virtual _impl

  void  Build();
  // #BUTTON #CAT_Structure Build the network units and Connect them (calls BuildLayers/Units/Prjns and Connect)
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

  virtual void	ConnectUnits(Unit* u_to, Unit* u_from=NULL, bool record=true,
			     ConSpec* conspec=NULL);
  // #CAT_Structure connect u1 so that it recieves from u2. Create projection if necessary

  virtual void	RemoveCons();
  // #MENU #MENU_ON_Structure #CONFIRM #MENU_SEP_BEFORE #CAT_Structure Remove all connections in network
  virtual void	RemoveUnits();
  // #MENU #CONFIRM #CAT_Structure Remove all units in network (preserving unit groups)
  virtual void	RemoveUnitGroups();
  // #MENU #CONFIRM #CAT_Structure Remove all unit groups in network
  virtual void	LayoutUnitGroups();
  // #MENU #CONFIRM #CAT_Structure layout all the layer's unit groups according to layer group geometry and spacing
  virtual void	PreConnect();
  // #CAT_Structure Prepare to connect this network (make con_groups)

  virtual void	LinkSendCons();
  // #IGNORE link the sending connections (after loading or copying)
  virtual void	FixPrjnIndexes();
  // #CAT_Structure fix the projection indicies of the connection groups (recv_idx, send_idx)

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
#endif

  virtual NetViewObj* NewViewText(const String& txt);
  // #MENU_BUTTON #MENU_ON_NetView #MENU_SEP_BEFORE #CAT_Display add a new text label to the network view objects -- this is an arbitrary fixed text label that can be placed anywhere in the display for annotating the model or other view elements
  virtual NetViewObj* NewGlassBrain();
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display add a new glass brain (as two separate hemispheres) to netview objects -- useful for situating biologically-based network models
  virtual void	PlaceNetText(NetTextLoc net_text_loc, float scale = 1.0f);
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display locate the network text data display (counters, statistics -- typically shown at bottom of network) in a new standard location (it can also be dragged anywhere in the net view, turn on lay_mv button and click on red arrow) -- can also change the scaling
  virtual void	NetTextUserData();
  // #IGNORE auto-called in InitLinks -- enable the filtering of what information is shown in the network text data display (typically shown at bottom of network, though see PlaceNetText for options on where to locate) -- this function creates entries for each of the viewable items in the UserData for this network -- just click on the UserData button to edit which items to display.

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

  ////////////////////////////////////////////////////////////////////////////////
  //	The following are misc functionality not required for primary computing

  virtual void	Compute_TrialStats();
  // #CAT_Statistic compute trial-level statistics (SSE and others defined by specific algorithms)
  virtual void	DMem_ShareTrialData(DataTable* dt, int n_rows = 1);
  // #CAT_DMem share trial data from given datatable across the trial-level dmem communicator (outer loop) -- each processor gets data from all other processors; if called every trial with n_rows = 1, data will be identical to non-dmem; if called at end of epoch with n_rows = -1 data will be grouped by processor but this is more efficient

  virtual void	Compute_EpochSSE();
  // #CAT_Statistic compute epoch-level sum squared error and related statistics
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

  virtual void	ProjectUnitWeights(Unit* un, float wt_thr = 0.5f, bool swt = false);
  // #CAT_Statistic project given unit's weights (receiving unless swt = true) through all layers (without any loops) -- results stored in anal1 on each unit (anal2 is used as a sum variable)  wt_thr is threshold on max-normalized weights (max=1) for following a given weight value to accumulate (so weaker weights are excluded).  values are always normalized at each layer to prevent exponential decrease/increase effects, so results are only relative indications of influence

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

  override void	Dump_Load_pre();
  override int	Dump_Load_Value(istream& strm, taBase* par=NULL);
  override int 	Save_strm(ostream& strm, TAPtr par=NULL, int indent=0);

  override String 	GetTypeDecoKey() const { return "Network"; }

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
  // ##FILETYPE_Network ##EXT_net ##COMPRESS ##CAT_Network a group of networks
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

inline void ConSpec::C_Send_Netin(Connection* cn, float* send_netin_vec, Unit* ru, float su_act) {
  send_netin_vec[ru->flat_idx] += cn->wt * su_act;
}
inline void ConSpec::Send_Netin(SendCons* cg, Network* net, int thread_no, Unit* su) {
  const float su_act = su->act;
  float* send_netin_vec = net->send_netin_tmp.el + net->send_netin_tmp.FastElIndex(0, thread_no);
  CON_GROUP_LOOP(cg, C_Send_Netin(cg->Cn(i), send_netin_vec, cg->Un(i), su_act));
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


#endif /* netstru_h */
