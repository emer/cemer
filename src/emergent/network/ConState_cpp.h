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


  bool          CopyCons(const ConState_cpp& cp);
  // #CAT_State copy connections from other connections

  // int           FindConFromNameIdx(const String& unit_nm, NetworkState_cpp* net) const;
  // // #CAT_Structure find index of connection from given unit name

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
  int   PruneCons(NetworkState_cpp* net, UnitState_cpp* un, const SimpleMathSpec& pre_proc,
                  Relation::Relations rel, float cmp_val);
  // #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  int   LesionCons(NetworkState_cpp* net, UnitState_cpp* un, float p_lesion, bool permute=true);
  // #CAT_Structure remove weights with prob p_lesion (permute = fixed no. lesioned)

  bool  ConValuesToArray(NetworkState_cpp* net, float_Array& ary, const String& variable);
  // #CAT_Structure adds values of variable from the connections into the given array (false if var not found)
  bool  ConValuesToMatrix(NetworkState_cpp* net, float_Matrix& mat, const String& variable);
  // #CAT_Structure sets values of variable from the connections into the given matrix (uses flat index of cons to set: 0..size-1), returns false if matrix is not appropriately sized
  bool  ConValuesFromArray(NetworkState_cpp* net, float_Array& ary, const String& variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found) -- must call Init_Weights_post at network level after running this!
  bool  ConValuesFromMatrix(NetworkState_cpp* net, float_Matrix& mat, const String& variable);
  // #CAT_Structure sets values of variable in the connections from the given array (false if var not found) -- uses flat index of cons to set: 0..size-1 -- must call Init_Weights_post at network level after running this!

  DataTable*    ConVarsToTable(DataTable* dt, UnitState_cpp* ru, NetworkState_cpp* net,
           const String& var1, const String& var2 = "",
           const String& var3 = "", const String& var4 = "", const String& var5 = "",
           const String& var6 = "", const String& var7 = "", const String& var8 = "",
           const String& var9 = "", const String& var10 = "", const String& var11 = "",
           const String& var12 = "", const String& var13 = "", const String& var14 = "");
  // #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var"

  ConState_cpp() { Initialize_core(); }
};

#endif // ConState_h
