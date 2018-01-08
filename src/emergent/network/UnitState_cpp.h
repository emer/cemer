// Copyright 2014-20188 Regents of the University of Colorado,
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

#ifndef UnitState_cpp_h
#define UnitState_cpp_h 1

#include <taString>

// parent includes:
#include <NetworkState_cpp>
#include <ConState_cpp>         // also includes prjnstate

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:
class SimpleMathSpec; //
class Random; //
class NetMonitor; //
class DataTable; //


class E_API UnitState_cpp {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE ##CAT_Network Generic unit variables -- basic computational unit of a neural network (e.g., a neuron-like processing unit) -- Unit contains all the structural information, and this class just contains the computationally-relevant variables
public:

#include <UnitState_core>

  // todo: ifdef all of these -- ok for emergent but not for standalone
  
  bool  SetUnValName(NetworkState_cpp* nnet, float val, const String& var_nm);
  // #CAT_Access set unit variable (specified by name, e.g., act, net, bias_wt) to given value -- for use by programs, which cannot assign the value through the direct variable access functions (e.g., act())
  float GetUnValName(NetworkState_cpp* nnet, const String& var_nm) const;
  // #CAT_Access get unit variable value (specified by name, e.g., act, net, bias_wt) -- for use by programs or other generic access via name variable
  bool  SetCnValName(NetworkState_cpp* nnet, float val, const Variant& prjn, int cn_idx, const String& var_nm, bool recv = true);
  // #CAT_Access set recv connection variable (specified by name, e.g., wt, dwt, pdw) in given projection (can be specified by name or index, defaults to recv -- send if false) at given connection index (cn_idx), to given value -- for use by programs, which cannot assign the value through the SafeCn function 
  float GetCnValName(NetworkState_cpp* nnet, const Variant& prjn, int cn_idx, const String& var_nm, bool recv = true) const;
  // #CAT_Access get recv connection variable value (specified by name, e.g., wt, dwt, pdw) in given projection (can be specified by name or index, defaults to recv -- send if false) at given connection index (cn_idx) -- for use by programs

  void  TransformWeights(NetworkState_cpp* nnet, const SimpleMathSpec& trans, PrjnState_cpp* prjn = NULL);
  // #CAT_Learning apply given transformation to weights -- must call Init_Weights_post at network level after running this!
  void  RenormWeights(NetworkState_cpp* nnet, bool mult_norm, float avg_wt, PrjnState_cpp* prjn = NULL);
  // #CAT_Learning renormalize the weight values using either multiplicative (for positive-only weight values such as Leabra) or subtractive normalization (for pos/neg weight values, such as backprop) to hit the given average weight value -- must call Init_Weights_post at network level after running this!
  void  RescaleWeights(NetworkState_cpp* nnet, const float rescale_factor, PrjnState_cpp* prjn = NULL);
  // #CAT_Learning rescale weights by multiplying by given factor -- must call Init_Weights_post at network level after running this!
  void  AddNoiseToWeights(NetworkState_cpp* nnet, const Random& noise_spec, PrjnState_cpp* prjn = NULL);
  // #CAT_Learning add noise to weights using given noise specification -- must call Init_Weights_post at network level after running this!
  int   PruneCons(NetworkState_cpp* nnet, const SimpleMathSpec& pre_proc, Relation::Relations rel,
                             float cmp_val, PrjnState_cpp* prjn = NULL);
  // #USE_RVAL #CAT_State remove weights that (after pre-proc) meet relation to compare val
  int   LesionCons(NetworkState_cpp* nnet, float p_lesion, bool permute=true, PrjnState_cpp* prjn = NULL);
  // #USE_RVAL #CAT_State remove connections with prob p_lesion (permute = fixed no. lesioned)

  bool  Snapshot(Network* nnet, const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #CAT_Statistic take a snapshot of given variable: assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.

  DataTable*    VarToTable(Network* nnet, DataTable* dt, const String& variable);
  // #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_State send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  DataTable*    ConVarsToTable(Network* nnet, DataTable* dt, const String& var1, const String& var2 = "",
               const String& var3 = "", const String& var4 = "", const String& var5 = "",
               const String& var6 = "", const String& var7 = "", const String& var8 = "",
               const String& var9 = "", const String& var10 = "", const String& var11 = "",
               const String& var12 = "", const String& var13 = "", const String& var14 = "",
               PrjnState_cpp* prjn=NULL);
  // #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var" -- prjn restricts to that prjn

  UnitState_cpp() { Initialize_core(); }
};


class E_API Unit : public UnitState_cpp {
  // Unit is an alias for UnitState_cpp for backward compatibility and simplicity in Programs
public:
};
  
#endif // UnitState_cpp_h
