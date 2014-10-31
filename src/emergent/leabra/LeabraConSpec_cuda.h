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

#ifndef LeabraConSpec_cuda_h
#define LeabraConSpec_cuda_h 1

// parent includes:
#include "ta_cuda.h"

// member includes:

// declare all other types mentioned but not required to include:

// important: nvcc cannot compile Qt, etc, so we have to define everything
// completely separately in cuda space
// regular files can include this file, and fill in values here..

// IMPORTANT: for indexing into the own_cons_mem, for large nets, we really 
// should use int64_t -- BUT this does NOT seem to work, at least on latest
// mac pro retina late 2014 NVIDIA GeForce GT 750M chip!  should be 3.0 compute 
// capability but still doesn't work reliably!
typedef int bigint;
// typedef int64_t bigint;
// typedef long long bigint;

class LeabraConSpecCuda {
  // NVIDIA CUDA support for calling LeabraConSpec functions
public:
  enum ConVars {                // Connection variables -- must align with Connection obj
    WT,                         // the synaptic weight of connection
    DWT,                        // change in synaptic weight as computed by learning mechanism
    FWT,                        // fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, "wt", via sigmoidal contrast enhancement (wt_sig)
    SWT,                        // slow learning linear (underlying) weight value -- learns more slowly from weight changes than fast weights, and fwt decays down to swt over time
  };

  // IMPORTANT: coordinate this with LeabraNetwork.h
  enum UnitVecVars {            // unit variables that have special vectorized storage, encoded at end of trial, in time for compute_dwt function
    AVG_S,
    AVG_M,
    AVG_L,
    THAL,
    COS_DIFF_LMIX,              // from recv layer
    N_VEC_VARS,
  };
  // ACT_M, // note: could add these to unit vec vars if needed
  // ACT_P,

  enum ConSpecParams {           // connection spec params needed for compute dwt, compute weights
    S_MIX,
    M_MIX,
    THR_L_MIX,
    THR_MAX,
    CUR_LRATE,
    N_CON_PARAMS,
  };

  int           n_units;          // total number of units in network flat list
  int           own_cons_max_size; // helps determine number of threads
  int           thread_chunk_sz;  // should be same as BaseCons::vec_chunked_size = 8
  int           max_threads;       // own_cons_max_size / thread_chunk_size
  int           n_threads;         // actual number of threads to use -- multiple of 32
  bigint        own_cons_cnt;   // number of floats to allocate to own_cons
  bigint        ptr_cons_cnt;   // number of floats to allocate to ptr_cons
  int           own_units_x_cons; // number of owning units * con groups that are stored in own_cons_mem -- size of units, con_mem_idx, con_size arrays below
  int           ptr_units_x_cons; // number of pointer units * con groups that are stored in ptr_cons_mem

  bool          strms_created;    // status flag for streams
  cudaStream_t  strm_updt_cons;   // for UpdateOwnCons method -- memcopy own_cons_mem to device
  cudaStream_t  strm_send_netin;  // for Send_NetinDelta
  cudaStream_t  strm_compute_dwt; // for Compute_dWt
  cudaStream_t  strm_compute_wt;  // for Compute_Wt

  float*        own_cons_mem_h;   // host version of own_cons_mem -- Network owns!!
  float*        own_cons_mem_d;   // device version of own_cons_mem
  float*        ptr_cons_mem_h;   // host version of ptr_cons_mem -- Network owns!!
  float*        ptr_cons_mem_d;   // device version of ptr_cons_mem -- only needs to be copied once
  int*          units_h;      // host array of sending units * sending con groups -- size own_units_x_cons -- the sending unit indexes
  int*          units_d;      // device array of sending units * sending con groups -- size own_units_x_cons -- the sending unit indexes
  bigint*       con_mem_idxs_h; // host array of sending units * sending con groups starting memory indexes -- size own_units_x_cons -- the mem_idx into own_cons_mem
  bigint*       con_mem_idxs_d; // device array of sending units * sending con groups  starting memory indexes -- size own_units_x_cons  -- the mem_idx into own_cons_mem
  int*          con_allocs_h; // host array of sending units * sending con groups -- size own_units_x_cons -- the number of cons allocated
  int*          con_allocs_d; // device array of sending units * sending con groups -- size own_units_x_cons  -- the number of cons allocated
  int*          con_sizes_h; // host array of sending units * sending con groups -- size own_units_x_cons -- the number of cons in use
  int*          con_sizes_d; // device array of sending units * sending con groups -- size own_units_x_cons  -- the number of cons in use
  int*          unit_starts_h; // host array where units start into above units x cons arrays -- size n_units

  float*        con_params_h; // host array of sending units * sending con groups -- size own_units_x_cons * N_CON_PARAMS -- parameters for each con group
  float*        con_params_d; // device array of sending units * sending con groups -- size own_units_x_cons * N_CON_PARAMS -- parameters for each con group
  float*        wt_sig_fun_h; // host wt_sig_fun.el lookup table for sigmoid contrast enhancement
  float*        wt_sig_fun_d; // device wt_sig_fun.el lookup table for sigmoid contrast enhancement -- length is 10,002 -- could be shared..

  int           cur_units_x_cons_n; // number of items in cur_units_x_cons* to send this time
  int*          cur_units_x_cons_h; // host indexes into units, con_mem_idxs, etc for current items to process this iteration -- size own_units_x_cons
  int*          cur_units_x_cons_d; // device indexes into units, con_mem_idxs, etc for current items to process this iteration -- size own_units_x_cons

  float*        send_net_acts_h; // sending delta acts * scale_eff to send netin -- size own_units_x_cons
  float*        send_net_acts_d; // sending delta acts * scale_eff to send netin -- size own_units_x_cons
  float*        send_netin_tmp_h;     // host netinputs -- Network owns!!
  float*        send_netin_tmp_d;     // device netinputs -- size = n_units (todo: support netin per prjn!)

  float*        unit_vec_vars_h; // host vectorized versions of unit variables -- 2d matrix outer dim is N_VEC_VARS, and inner is flat_units.size
  float*        unit_vec_vars_d; // device vectorized versions of unit variables -- 2d matrix outer dim is N_VEC_VARS, and inner is flat_units.size

  inline float*   OwnCnVar(int uncon, int var_no) const
  { return own_cons_mem_d + con_mem_idxs_d[uncon] + (con_allocs_d[uncon] * (1 + var_no)); }
  // illustration for how to access connection variable in compute code: access connection variable for unit x con, variable

  inline int32_t  UnIdx(int uncon, int idx) const
  { return ((int32_t*)(own_cons_mem_d + con_mem_idxs_d[uncon]))[idx]; }
  // illustration for how to access access recv unit idx for unit x con

  inline float&   ConParam_h(int uncon, int param_no) const
  { return con_params_h[uncon * N_CON_PARAMS + param_no]; }
  // connection parameter, on host, for given unit x con and parameter number

  
  void  AllocCudaArrays(int n_un, int own_cons_max_sz, bigint own_cnt,
                        bigint ptr_cnt, int own_units_x, int ptr_units_x, 
                        float* own_cons_mem, float* ptr_cons_mem, float* send_netin_tmp,
                        float* unit_vec_vars);
  // allocate arrays based on parameters from network, called after network modifications
  void  FreeCudaArrays();
  // free arrays..

  void  OwnCons_HostToDevice(bool sync = false);
  // copy all the own_cons_mem (weights etc) from host to device: after initializing weights -- optionally wait for copy to finish
  void  OwnCons_DeviceToHost(bool sync = false);
  // copy all the own_cons_mem (weights etc) from device to host: for visualization etc -- optionally wait for copy to finish

  void  UpdateUnitsXCons();
  // call after units, com_mem_idxs, con_allocs, con_sizes all on host have been updated -- sends to device -- just once after build
  void  UpdateConParams();
  // call whenever any connection parameters change -- copy con_params_h -> d and wt_sig_fun_h -> d (must set _h before this!)

  void  Send_NetinDelta();
  // AFTER filling in cur_units_x_cons_n, cur_units_x_cons_h, and send_net_acts_h, this will fill out send_netin_tmp_h with the deltas!

  void  Compute_dWt(bool sync = false);
  // AFTER filling in cur_units_x_cons_n, cur_units_x_cons_h, and unit_vec_vars_h, this will compute weight changes -- ONLY on CUDA side -- call OwnCons_DeviceToHost to get back
  void  Compute_Weights(bool sync = false);
  // compute weight updates -- ONLY on CUDA side -- call OwnCons_DeviceToHost to get back

  LeabraConSpecCuda();
  ~LeabraConSpecCuda();
  void Initialize();
};
  
#endif // LeabraConSpec_cuda_h
