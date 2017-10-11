// Copyright 2017, Regents of the University of Colorado,
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

#ifndef ConState_cpp_h
#define ConState_cpp_h 1

// parent includes:
#include "network_def.h"

#include <State_main>

// member includes:
#include <taString>
#include <Connection>

#include <Random>
#include <SimpleMathSpec>
#include <Relation>

// declare all other types mentioned but not required to include:
class ConSpec; //
class Unit; //
class NetMonitor; //
class float_Array; //
class float_Matrix; //
class Projection; //
class Layer; //
class Network; //
class DataTable; //

#include <NetworkState_cpp>
#include <PrjnState_cpp>

#include <State_cpp>

// This object is allocated and controlled by the Network, on a per-thread basis
//
// Connection memory is allocated and controlled by the NetworkState, on a per-thread basis
// we have to use an index into this memory block to be bit-copy portable across devices (GPU)
// but then we cache this on each device after all the connections are established, to speed
// up repeated access, into mem_start, cnmem_start pointers

// mem_start below is the appropriate recv or send mem block + mem_idx

// Memory structure for connection owners =
// block of (con_type->members.size + 1) * sizeof(float) * alloc_size
// where sizeof(float) *better be* same size as int (4 bytes, 32 bits)
//
// int     unit_idxs = mem_start[0..alloc_size-1]  // we have to cast from the float*
// float   con_val1 =  mem_start[alloc_size .. 2*alloc_size -1]
// float   con_val2 =  mem_start[2*alloc_size .. 3*alloc_size -1] etc..

// to support shared connections, and slightly optimize the indexing, we store
// a separate cnmem_start pointer -- this points to the connection memory of the
// source we share from if shared, or else it is just mem_start + alloc_size:

// float   con_val1 =  cnmem_start[0 .. alloc_size -1]
// float   con_val2 =  cnmem_start[alloc_size .. 2*alloc_size -1]

// Memory for connection pointers = block of 2 * sizeof(int) * alloc_size
// int unit_idxs = mem_start[0..alloc_size-1]
// int con_idxs  = mem_start[alloc_size .. 2*alloc_size -1]


// TODO: at some point may want to make this code fully independent of TA / emergent framework
// and move the load / save etc kind of stuff to UnitSpec or something 


class ConState_cpp {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE ##CAT_Network connection group -- manages one projection's worth of connections at a unit level -- can be either Recv or Send
public:

#include <ConState_core>

  TypeDef* ConType(Network* net) const;
  // #IGNORE #CAT_Access connection type (only used for looking up variable names)

  bool   SetShareFrom(NetworkState_cpp* net, Unit* shu);
  // #CAT_Access set this connection group to share from given other unit -- checks to make sure this works -- returns false if not (will have already emitted warning message)

  Unit*          OwnUn(Network* net) const;
  // #CAT_Access get owning unit
  Unit*          Un(int idx, Network* net) const;
  // #CAT_Access get unit at given connection index (fast, no checking)
  Unit*          SafeUn(int idx, Network* net) const;
  // #CAT_Access get unit at given connection index, safely with range checking
  
  float&         SafeCn(Network* net, int idx, int var_no) const;
  // #CAT_Access fully safe generic access of connection variable value at given index, regardless of whether it is owned or a pointer -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- this is mainly for program access -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)
  float&         SafeCnName(Network* net, int idx, const String& var_nm) const;
  // #BUTTON #USE_RVAL #CAT_Access generic safe access of connection variable value by name (e.g., wt, dwt, pdw, etc) at given index, regardless of whether it is owned or a pointer -- mainly for program access -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)

  bool           SetCnVal(Network* net, float val, int idx, int var_no);
  // #CAT_Access set connection variable to given value -- for use by programs, which cannot assign the value through the SafeCn function -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- 
  bool           SetCnValName(Network* net, float val, int idx, const String& var_nm);
  // #CAT_Access set connection variable (specified by name, e.g., wt, dwt, pdw) to given value -- for use by programs, which cannot assign the value through the SafeCn function 

  int            ConnectUnits(Unit* our_un, Unit* oth_un, ConState_cpp* oth_cons,
                              bool ignore_alloc_errs = false,
                              bool set_init_wt = false, float init_wt = 0.0f);
  // #CAT_Modify add a new connection betwee our unit and an other unit and its appropriate cons -- does appropriate things depending on who owns the connects, etc.  enough room must already be allocated on both sides  (flag will turn off err msg) -- returns index of new connection (-1 if failed) -- can also optionally set initial weight value
  int            ConnectUnOwnCn(Unit* un, bool ignore_alloc_errs = false,
                                bool allow_null_unit = false);
  // #CAT_Modify add a new connection from given unit for OwnCons case -- returns -1 if no more room relative to alloc_size (flag will turn off err msg) -- default is to not allow connections from a unit with flat_idx = 0 (null_unit) but this can be overridden -- returns index of new connection (-1 if failed)
  bool           ConnectUnPtrCn(Unit* un, int con_idx, bool ignore_alloc_errs = false);
    // #CAT_Modify add a new connection from given unit and connection index for PtrCons case -- returns false if no more room, else true

  bool          CopyCons(const ConState_cpp& cp);
  // #CAT_Structure copy connections (unit ptrs and cons) from other con array, checking to make sure they are the same type (false if not) -- does not do any alloc -- just copies values -- source must have same OwnCons status as us

  void          MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this set of receiving connections

  int           FindConFromNameIdx(const String& unit_nm, Network* net) const;
  // #CAT_Structure find index of connection from given unit name

  ConState_cpp*     GetPrjnSendCons(Unit* su) const;
  // #CAT_Structure safely and robustly get the SendCons from given sending unit that belongs to the same projection as this recv cons (only applicable if this IsRecv())
  ConState_cpp*     GetPrjnRecvCons(Unit* ru) const;
  // #CAT_Structure safely and robustly get the RecvCons from given recv unit that belongs to the same projection as this send cons (only applicable if this IsSend())

  void          FixConPtrs_SendOwns(Network* net, int st_idx = 0);
  // #CAT_Structure only for sending cons that own the connections: fix all the pointer connections to our connections to be correct -- called after reorganizing the order of connections within this group -- called with our owning send unit
  void          FixConPtrs_RecvOwns(Network* net, int st_idx = 0);
  // #CAT_Structure only for sending cons that own the connections: fix all the pointer connections to our connections to be correct -- called after reorganizing the order of connections within this group -- called with our owning recv unit

  inline float  VecChunkPct()
  {  if(size > 0) return (float)vec_chunked_size / (float)size; return 0.0f; }
  // #CAT_Structure return percent of our cons that are vec chunked

  void          VecChunk_SendOwns(Network* net, int* tmp_chunks, int* tmp_not_chunks,
                                  float* tmp_con_mem);
  // #CAT_Structure chunks the connections in vectorizable units, for sender own case -- pass in our sending own unit, and temp scratch memory guaranteed to be >= alloc_size for doing the reorganization
  void          VecChunk_RecvOwns(Network* net, int* tmp_chunks, int* tmp_not_chunks,
                                  float* tmp_con_mem);
  // #CAT_Structure chunks the connections in vectorizable units, for recv own case -- pass in our recv own unit, and temp scratch memory guaranteed to be >= alloc_size for doing the reorganization

  int           VecChunk_impl(int* tmp_chunks, int* tmp_not_chunks,
                              float* tmp_con_mem);
  // #IGNORE impl -- returns first_change index for fixing con ptrs, and sets vec_chunked_size -- chunks the connections in vectorizable units, for sender own case -- gets our sending own unit, and temp scratch memory guaranteed to be >= alloc_size for doing the reorganization

  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  void  TransformWeights(NetworkState_cpp* net, const SimpleMathSpec& trans);
  // #CAT_Learning apply given transformation to weights -- must call Init_Weights_post at network level after running this, to keep other weight values synchronized
  void  RenormWeights(NetworkState_cpp* net, bool mult_norm, float avg_wt);
  // #CAT_Learning renormalize the weight values using either multiplicative (for positive-only weight values such as Leabra) or subtractive normalization (for pos/neg weight values, such as backprop) to hit the given average weight value -- must call Init_Weights_post at network level after running this!
  void  RescaleWeights(NetworkState_cpp* net, const float rescale_factor);
  // #CAT_Learning rescale the weight values by multiplying by rescaling factor -- must call Init_Weights_post at network level after running this, to keep other weight values synchronized
  void  AddNoiseToWeights(NetworkState_cpp* net, const Random& noise_spec);
  // #CAT_Learning add noise to weights using given noise specification -- must call Init_Weights_post at network level after running this!
  int   PruneCons(Unit* un, const SimpleMathSpec& pre_proc,
                  Relation::Relations rel, float cmp_val);
  // #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  int   LesionCons(Unit* un, float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove weights with prob p_lesion (permute = fixed no. lesioned)

  bool  ConValuesToArray(Network* net, float_Array& ary, const String& variable);
  // #CAT_Structure adds values of variable from the connections into the given array (false if var not found)
  bool  ConValuesToMatrix(Network* net, float_Matrix& mat, const String& variable);
  // #CAT_Structure sets values of variable from the connections into the given matrix (uses flat index of cons to set: 0..size-1), returns false if matrix is not appropriately sized
  bool  ConValuesFromArray(Network* net, float_Array& ary, const String& variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found) -- must call Init_Weights_post at network level after running this!
  bool  ConValuesFromMatrix(Network* net, float_Matrix& mat, const String& variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found) -- uses flat index of cons to set: 0..size-1 -- must call Init_Weights_post at network level after running this!

  DataTable*    ConVarsToTable(DataTable* dt, Unit* ru, Network* net,
           const String& var1, const String& var2 = "",
           const String& var3 = "", const String& var4 = "", const String& var5 = "",
           const String& var6 = "", const String& var7 = "", const String& var8 = "",
           const String& var9 = "", const String& var10 = "", const String& var11 = "",
           const String& var12 = "", const String& var13 = "", const String& var14 = "");
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var"

  ConState_cpp() { Initialize_core(); }
};

#endif // ConState_h
