// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef BaseCons_h
#define BaseCons_h 1

// parent includes:
#include "network_def.h"
#include <taOBase>

// member includes:
#include <Connection>
#include <ConSpec>
#include <Relation>


// declare all other types mentioned but not required to include:
class NetMonitor; //
class float_Array; //
class float_Matrix; //
class SimpleMathSpec; //
class SendCons; //
class Projection; //
class Layer; //
class DataTable; //

eTypeDef_Of(BaseCons);

class E_API BaseCons : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE #VIRT_BASE ##CAT_Network base connection manager class -- manages one projection's worth of connections at a unit level -- inherited by RecvCons and SendCons
INHERITED(taOBase)
public:
  
  enum WtSaveFormat {   // note: must be coordinated with the Network enum
    TEXT,                       // weights are saved as ascii text representation of digits (completely portable)
    BINARY,                     // weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
  };

  
  enum BaseConsFlags {  // note: following use base_flags so have high values to avoid conflicts
    OWN_CONS = 0x01000000,      // this guy owns the connections -- else gets links to others
    RECV_CONS = 0x02000000,     // we are a recv con group -- else a send con group
  };

  // note: define new enums for other variables, typically in ConSpec, adding from DWT
  enum ConVars {                // Connection variables -- must align with Connection obj
    WT,                         // the synaptic weight of connection
    DWT,                        // change in synaptic weight as computed by learning mechanism
  };

  static float  null_rval;      // #IGNORE null return value for reference funs

  int           size;           // #CAT_Structure #READ_ONLY #NO_SAVE #SHOW number of connections currently active
  int           alloc_size;     // #CAT_Structure #READ_ONLY #NO_SAVE #SHOW allocated size -- no more than this number of connections may be created -- it is a hard limit set by the alloc function
  TypeDef*      con_type;       // #CAT_Structure #READ_ONLY #SHOW type of connection object currently allocated
  Projection*   prjn;           // #CAT_Structure #READ_ONLY #SHOW #NO_SET_POINTER pointer to the projection which created these connections -- has the source con_type and con spec information
  int           other_idx;      // #CAT_Structure #READ_ONLY #SHOW index into other direction's list of cons objects (i.e., send_idx for RecvCons and recv_idx for SendCons)

  ConSpec*      m_con_spec;     // #IGNORE con spec that we use: controlled entirely by the projection!

#ifndef __MAKETA__
  union {
    float**         cons_own;       // if we own the cons, this is their physical memory -- float* array of pointers to allocated float memory, corresponding exactly to con_type float* variables
    int32_t*        cons_idx;       // if we don't own the cons, these are indexes into the connections of the unit on the other side of the connection, for each connection
  };
#endif
  int32_t*          unit_idxs;      // #IGNORE list of unit flat_idx indexes on the other side of the connection, in index association with the connections

  ////////////////////////////////////////////////////////////////////////////////
  //    Primary infrastructure management routines

  bool  OwnCons() const { return HasBaseFlag(OWN_CONS); }
  // #CAT_Structure do we own the connections?  else just point to them
  bool  PtrCons() const { return !HasBaseFlag(OWN_CONS); }
  // #CAT_Structure do we have pointers to connections?  else we own them
  bool  IsRecv() const  { return HasBaseFlag(RECV_CONS); }
  // #CAT_Structure is this a receiving con group?  else sending
  bool  IsSend() const  { return !HasBaseFlag(RECV_CONS); }
  // #CAT_Structure is this a sending con group?  else receiving

  inline bool           InRange(int idx) const
  { return ((idx < size) && (idx >= 0)); }
  // #CAT_Access is index in range?

#ifdef DEBUG
  inline float*         OwnCnVar(int var_no) const
  { if(TestError(!OwnCons(), "OwnCnVar", "programmer error -- don't own cons"))
      return NULL; 
    return cons_own[var_no]; }
#else
  inline float*          OwnCnVar(int var_no) const
  { return cons_own[var_no]; }
#endif
  // #CAT_Access fastest access (no range checking) to owned connection variable value -- get this float* and then index it directly with loop index -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)

  inline float&          OwnCn(int idx, int var_no) const
  { return cons_own[var_no][idx]; }
  // #CAT_Access fast access (no range checking) to owned connection variable value at given index -- OwnCnVar with index in loop is preferred for fastest access -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)

  inline const int32_t& UnIdx(int idx) const
  { return unit_idxs[idx]; }
  // #CAT_Access fast access (no range checking) to unit flat index at given connection index
  inline int32_t&       UnIdx(int idx)
  { return unit_idxs[idx]; }
  // #CAT_Access fast access (no range checking) to unit flat index at given connection index
  inline Unit*          Un(int idx, Network* net) const;
  // #IGNORE #CAT_Access fast access (no range checking) to unit pointer at given connection index (goes through flat index at network level) -- this is the unit on the other end of this connection 
  inline Unit*          UnFmLst(int idx, Unit** flat_units) const
  { return flat_units[unit_idxs[idx]]; }
  // #IGNORE #CAT_Access fast access (no range checking) to unit pointer at given index (goes through flat index at network level) -- this is the unit on the other end of this connection 
  Unit*                 SafeUn(int idx) const;
  // #CAT_Access safe access (range checking) to unit pointer at given connection index (goes through flat index at network level) -- this is the unit on the other end of this connection -- mainly for program access
  inline bool           SetUn(int idx, Unit* un);
  // #IGNORE #CAT_Modify set unit pointer at given index -- returns false if out of range

  inline BaseCons*      UnCons(int idx, Network* net) const;
  // #IGNORE get BaseCons for this projection in unit at given index at other end of this connection
  inline BaseCons*      SafeUnCons(int idx, Network* net) const;
  // #IGNORE get BaseCons for this projection in unit at given index at other end of this connection -- uses safe access

  inline const int32_t& PtrCnIdx(int idx) const
  { return cons_idx[idx]; }
  // #CAT_Access fast access (no range checking) to index of connection within unit cons on other side of connection 
  inline int32_t&       PtrCnIdx(int idx)
  { return cons_idx[idx]; }
  // #CAT_Access fast access (no range checking) to index of connection within unit cons on other side of connection 

  inline float&         PtrCn(int idx, int var_no, Network* net) const
  { return UnCons(idx, net)->OwnCn(PtrCnIdx(idx), var_no); }
  // #IGNORE #CAT_Access fast access (no range or own_cons checking) to connection value at given index -- this is MUCH slower than OwnCn due to several index accesses, so where ever possible computations should be performed on the side that owns the connections -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)

  inline float&         Cn(int idx, int var_no, Network* net) const
  { if(OwnCons()) return OwnCn(idx, var_no); return PtrCn(idx, var_no, net); }
  // #IGNORE #CAT_Access generic access of connection variable value at given index, regardless of whether it is owned or a pointer -- no range checking -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)
  inline float&         SafeFastCn(int idx, int var_no, Network* net) const
  { if(OwnCons()) { if(InRange(idx)) return OwnCn(idx, var_no); return null_rval; }
    BaseCons* bc = SafeUnCons(idx, net);
    if(bc) return bc->SafeFastCn(PtrCnIdx(idx), var_no, net); return null_rval; }
  // #IGNORE #CAT_Access generic access of connection variable value at given index, regardless of whether it is owned or a pointer -- does range checking but doesn't issue messages, and is otherwise as fast as possible -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)
  float&                SafeCn(int idx, int var_no) const;
  // #CAT_Access fully safe generic access of connection variable value at given index, regardless of whether it is owned or a pointer -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- this is mainly for program access -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)
  float&                SafeCnName(int idx, const String& var_nm) const;
  // #CAT_Access generic safe access of connection variable value by name (e.g., wt, dwt, pdw, etc) at given index, regardless of whether it is owned or a pointer -- mainly for program access -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)

  bool                  SetCnVal(float val, int idx, int var_no);
  // #CAT_Access set connection variable to given value -- for use by programs, which cannot assign the value through the SafeCn function -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- 
  bool                  SetCnValName(float val, int idx, const String& var_nm);
  // #CAT_Access set connection variable (specified by name, e.g., wt, dwt, pdw) to given value -- for use by programs, which cannot assign the value through the SafeCn function 

  virtual int            ConnectUnits(Unit* our_un, Unit* oth_un, BaseCons* oth_cons,
                                     bool ignore_alloc_errs = false,
                                      bool set_init_wt = false, float init_wt = 0.0f);
  // #CAT_Modify add a new connection betwee our unit and an other unit and its appropriate cons -- does appropriate things depending on who owns the connects, etc.  enough room must already be allocated on both sides  (flag will turn off err msg) -- returns index of new connection (-1 if failed) -- can also optionally set initial weight value
  virtual int            ConnectUnOwnCn(Unit* un, bool ignore_alloc_errs = false,
                                       bool allow_null_unit = false);
  // #CAT_Modify add a new connection from given unit for OwnCons case -- returns -1 if no more room relative to alloc_size (flag will turn off err msg) -- default is to not allow connections from a unit with flat_idx = 0 (null_unit) but this can be overridden -- returns index of new connection (-1 if failed)
  virtual bool           ConnectUnPtrCn(Unit* un, int con_idx,
                                       bool ignore_alloc_errs = false);
    // #CAT_Modify add a new connection from given unit and connection index for PtrCons case -- returns false if no more room, else true

  virtual void           ConnectAllocInc(int inc_n = 1);
  // #CAT_Modify use this for dynamically figuring out how many connections to allocate, if it is not possible to compute directly -- increments size by given number -- later call AllocConsFmSize to allocate connections based on the size value
  virtual void           AllocConsFmSize();
  // #CAT_Structure allocate storage for given number of connections (and Unit pointers) based on the size value, and reset size to 0 -- for dynamic allocation with ConnectAllocInc

  virtual bool          SetConType(TypeDef* cn_tp);
  // #CAT_Structure set new connection type -- will fail (err msg, return false) if connections are already allocated -- can only set prior to allocation
  virtual void          AllocCons(int n);
  // #CAT_Structure allocate storage for given number of connections (and Unit pointers) -- this MUST be called prior to making any new connections
  virtual void          FreeCons();
  // #CAT_Structure deallocate all connection-level storage (cons and units)
  virtual bool          CopyCons(const BaseCons& cp);
  // #CAT_Structure copy connections (unit ptrs and cons) from other con array, checking to make sure they are the same type (false if not) -- does not do any alloc -- just copies values -- source must have same OwnCons status as us

  virtual bool          RemoveConIdx(int i, Unit* myun, Network* net);
  // #CAT_Modify remove connection (cons and units) at given index, moving others down to fill in, including fixing the PtrCn cons_idx index into OwnCons when one is removed for remaining connections
  virtual bool          RemoveConUn(Unit* un, Unit* myun, Network* net);
  // #CAT_Modify remove connection from given unit
  virtual void          RemoveAll()     { FreeCons(); }
  // #CAT_Modify remove all conections -- frees all associated memory
  virtual void          Reset()         { FreeCons(); }
  // #CAT_Modify remove all conections -- frees all associated memory

  inline ConSpec*       GetConSpec() const { return m_con_spec; }
  // #CAT_Structure get the con spec for this connection group -- this is controlled entirely by the projection con_spec
  inline void           SetConSpec(ConSpec* cs) { m_con_spec = cs; }
  // #CAT_Structure set the con spec to given value -- no ref counting or other checking is done -- should generally only be called by the Projection

  virtual void          MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this set of receiving connections

  virtual int           FindConFromIdx(Unit* un) const;
  // #CAT_Structure find index of connection from given unit
  virtual int           FindConFromNameIdx(const String& unit_nm, Network* net) const;
  // #CAT_Structure find index of connection from given unit name

  virtual SendCons*     GetPrjnSendCons(Unit* su) const;
  // #CAT_Structure safely and robustly get the SendCons from given sending unit that belongs to the same projection as this recv cons (only applicable if this IsRecv())
  virtual RecvCons*     GetPrjnRecvCons(Unit* ru) const;
  // #CAT_Structure safely and robustly get the RecvCons from given recv unit that belongs to the same projection as this send cons (only applicable if this IsSend())

  static RecvCons*      FindRecipRecvCon(int& con_idx, Unit* su, Unit* ru, Layer* ru_lay);
  // #CAT_Structure find the reciprocal recv con group and con index for sending unit su to this receiving unit ru
  static SendCons*      FindRecipSendCon(int& con_idx, Unit* ru, Unit* su, Layer* su_lay);
  // #CAT_Structure find the reciprocal send con group and con index for receiving unit ru from this sending unit su

  ////////////////////////////////////////////////////////////////////////////////
  //    The following are computational functions needed for basic infrastructure

  void  Init_Weights_post(Unit* ru, Network* net)
  { if(GetConSpec()) GetConSpec()->Init_Weights_post(this,ru,net); }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  virtual void  TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void  AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int   PruneCons(Unit* un, const SimpleMathSpec& pre_proc,
                             Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int   LesionCons(Unit* un, float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove weights with prob p_lesion (permute = fixed no. lesioned)

  virtual bool  ConValuesToArray(float_Array& ary, const String& variable);
  // #CAT_Structure adds values of variable from the connections into the given array (false if var not found)
  virtual bool  ConValuesToMatrix(float_Matrix& mat, const String& variable);
  // #CAT_Structure sets values of variable from the connections into the given matrix (uses flat index of cons to set: 0..size-1), returns false if matrix is not appropriately sized
  virtual bool  ConValuesFromArray(float_Array& ary, const String& variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found)
  virtual bool  ConValuesFromMatrix(float_Matrix& mat, const String& variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found) -- uses flat index of cons to set: 0..size-1

  static int    LoadWeights_StartTag(std::istream& strm, const String& tag,
                                     String& val, bool quiet);
  // #IGNORE read in a start tag -- makes sure it matches tag, returns TAG_GOT if got it
  static int    LoadWeights_EndTag(std::istream& strm, const String& trg_tag,
                                   String& cur_tag, int& stat, bool quiet);
  // #IGNORE read in an end tag -- makes sure it matches trg_tag, cur_tag, stat are current read_tag & status (if !END_TAG, will try to read end)

  virtual void  SaveWeights_strm(std::ostream& strm, Unit* ru, Network* net,
                                 BaseCons::WtSaveFormat fmt = BaseCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   LoadWeights_strm(std::istream& strm, Unit* ru, Network* net,
                BaseCons::WtSaveFormat fmt = BaseCons::TEXT, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary format) -- rval is taMisc::ReadTagStatus, TAG_END if successful -- the connections for both sides must already be allocated, but it can rearrange connections based on save unit indexes for random connectivity etc
  static int    SkipWeights_strm(std::istream& strm,
                                 BaseCons::WtSaveFormat fmt = BaseCons::TEXT,
                                 bool quiet = false);
  // #IGNORE skip over saved weights (to keep the file in sync) -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void  SaveWeights(const String& fname, Unit* ru, Network* net,
                            BaseCons::WtSaveFormat fmt = BaseCons::TEXT);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int   LoadWeights(const String& fname, Unit* ru, Network* net,
                  BaseCons::WtSaveFormat fmt = BaseCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File  #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary format) (leave fname empty to pull up file chooser)

  virtual void  Copy_Weights(const BaseCons* src, Network* net);
  // #CAT_ObjectMgmt copies weights from other con_group

  virtual DataTable*    ConVarsToTable(DataTable* dt, Unit* ru, Network* net,
            const String& var1, const String& var2 = "",
            const String& var3 = "", const String& var4 = "", const String& var5 = "",
            const String& var6 = "", const String& var7 = "", const String& var8 = "",
            const String& var9 = "", const String& var10 = "", const String& var11 = "",
            const String& var12 = "", const String& var13 = "", const String& var14 = "");
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var"

  // int  UpdatePointers_NewPar(taBase* old_par, taBase* new_par) override;
  // int  UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) override;
  // int  UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) override;
  bool ChangeMyType(TypeDef* new_type) override;

  String GetTypeDecoKey() const override { return "Connection"; }

  int  Dump_Save_PathR(std::ostream& strm, taBase* par=NULL, int indent=0) override;
  int  Dump_Load_Value(std::istream& strm, taBase* par=NULL) override;
  // the dump system saves the alloc_size during the first 'path' stage of dumping, and then during loading does a full AllocCons for everything, building it all anew prior to the second 'value' stage of loading, which can then do ConnectFrom to setup connections, and set weights etc

  // the cons versions below have support for loading an "old" format file (prior to 4.1.0), which does not have the pre-alloc during the first path load phase: they save the load string into user data on the Unit, and then the Network::Dump_Load_Value goes through and reads those in after doing a manual Connect, so that everything is allocated

  virtual int   Dump_Save_Cons(std::ostream& strm, int indent);
  // #CAT_FILE save just the connection values out to given stream -- call this in Dump_Save_Value after default guy to actually save connections (in RecvCons)
  virtual int   Dump_Load_Cons(std::istream& strm, bool old_2nd_load = false);
  // #CAT_FILE load just the connection values from given stream -- call this in Dump_Load_Value to actually load connections (in RecvCons)

  void  CutLinks();
  void  Copy_(const BaseCons& cp);
  TA_BASEFUNS(BaseCons);
protected:
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void  Initialize();
  void  Destroy();
};

#endif // BaseCons_h
