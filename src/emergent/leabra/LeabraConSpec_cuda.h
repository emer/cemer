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

class LeabraConSpecCuda {
  // NVIDIA CUDA support for calling LeabraConSpec functions
public:
  enum ConVars {                // Connection variables -- must align with Connection obj
    WT,                         // the synaptic weight of connection
    DWT,                        // change in synaptic weight as computed by learning mechanism
    FWT,                        // fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, "wt", via sigmoidal contrast enhancement (wt_sig)
    SWT,                        // slow learning linear (underlying) weight value -- learns more slowly from weight changes than fast weights, and fwt decays down to swt over time
  };

  // static LeabraConSpecCuda* cur; // current instance of this CUDA object

  int           n_units;          // total number of units in network flat list
  int64_t       own_cons_cnt;   // number of floats to allocate to own_cons
  int64_t       ptr_cons_cnt;   // number of floats to allocate to ptr_cons
  int           own_units_x_cons; // number of owning units * con groups that are stored in own_cons_mem -- size of units, con_mem_idx, con_size arrays below
  int           ptr_units_x_cons; // number of pointer units * con groups that are stored in ptr_cons_mem

  float*        own_cons_mem_h;   // host version of own_cons_mem -- Network owns!!
  float*        own_cons_mem_d;   // device version of own_cons_mem
  float*        ptr_cons_mem_h;   // host version of ptr_cons_mem -- Network owns!!
  float*        ptr_cons_mem_d;   // device version of ptr_cons_mem -- only needs to be copied once
  int*          units_h;      // host array of sending units * sending con groups -- size own_units_x_cons -- the sending unit indexes
  int*          units_d;      // device array of sending units * sending con groups -- size own_units_x_cons -- the sending unit indexes
  int64_t*      con_mem_idxs_h; // host array of sending units * sending con groups starting memory indexes -- size own_units_x_cons -- the mem_idx into own_cons_mem
  int64_t*      con_mem_idxs_d; // device array of sending units * sending con groups  starting memory indexes -- size own_units_x_cons  -- the mem_idx into own_cons_mem
  int*          con_allocs_h; // host array of sending units * sending con groups number of cons -- size own_units_x_cons -- the number of cons allocated
  int*          con_allocs_d; // device array of sending units * sending con groups  number of cons -- size own_units_x_cons  -- the number of cons allocated
  int*          con_sizes_h; // host array of sending units * sending con groups number of cons -- size own_units_x_cons -- the number of cons in use
  int*          con_sizes_d; // device array of sending units * sending con groups  number of cons -- size own_units_x_cons  -- the number of cons in use
  int*          unit_starts_h; // host array where units start into above units x cons arrays -- size n_units

  int           cur_send_net_n; // number of items in cur_send_net* to send this time
  int*          cur_send_net_h; // host indexes into units, con_mem_idxs, etc for current ones to send this iteration -- size own_units_x_cons
  int*          cur_send_net_d; // device indexes into units, con_mem_idxs, etc for current ones to send this iteration -- size own_units_x_cons
  float*        send_net_acts_h; // sending delta acts * scale_eff to send netin -- size own_units_x_cons
  float*        send_net_acts_d; // sending delta acts * scale_eff to send netin -- size own_units_x_cons
  float*        send_netin_tmp_h;     // host netinputs -- Network owns!!
  float*        send_netin_tmp_d;     // device netinputs -- size = n_units (todo: support netin per prjn!)


  inline float*   OwnCnVar(int uncon, int var_no) const
  { return own_cons_mem_d + con_mem_idxs_d[uncon] + (con_allocs_d[uncon] * (1 + var_no)); }
  // for use in compute code: access connection variable for unit x con, variable

  inline int32_t  UnIdx(int uncon, int idx) const
  { return ((int32_t*)(own_cons_mem_d + con_mem_idxs_d[uncon]))[idx]; }
  // for use in compute code: access recv unit idx for unit x con, variable

  
  void  AllocCudaArrays(int n_un, int64_t own_cnt, int64_t ptr_cnt,
                        int own_units_x, int ptr_units_x, 
                        float* own_cons_mem, float* ptr_cons_mem, float* send_netin_tmp);
  // allocate arrays based on parameters from network, called after network modifications
  void  FreeCudaArrays();
  // free arrays..

  void  UpdateOwnCons();
  // call after weights are updated to copy new weights to device
  void  UpdateUnitsXCons();
  // call after units, com_mem_idxs, con_allocs, con_sizes all on host have been updated -- sends to device -- just once after build

  void  Send_NetinDelta();
  // AFTER filling in cur_send_net_n, cur_send_net_h, and send_net_acts_h, this will fill out send_netin_tmp_h with the deltas!

  LeabraConSpecCuda();
  ~LeabraConSpecCuda();
  void Initialize();
};
  
#endif // LeabraConSpec_cuda_h
