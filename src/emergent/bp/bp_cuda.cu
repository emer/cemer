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

#include "bp_cuda.h"

Bp_cuda::Bp_cuda() {
  unit_spec_size = sizeof(BpUnitSpec_cuda);
  con_spec_size = sizeof(BpConSpec_cuda);
}

// this constant memory is used for holding the spec info:

__constant__ char const_spec_mem[Network_cuda::max_constant_mem];


///////////////////////////////////////////////////////////////////////////
//      layer-at-a-time netinput and act

__global__ void Kernel_Compute_Netin_ConGroup
(const int st_ui, const int cgp_idx, int* recv_cgp_start, char* recv_cgp_mem, const int con_group_size, float* recv_cons_mem, char* units_mem, const int unit_vars_size) {

  extern __shared__ float temp_sums[]; // third arg to kernel specifies size of this!

  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x; // blocks are *units*

  ConGroup_cuda* cg = Network_cuda::GetUnConGroup
    (recv_cgp_mem, recv_cgp_start, con_group_size, un_idx, cgp_idx);

  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  const float* wts = cg->OwnCnVar(recv_cons_mem, ConGroup_cuda::WT);

  float sum = 0.0f;

  while(st < ed) {
    const int32_t su_idx = cg->UnIdx(recv_cons_mem, st);
    UnitVars_cuda* su = Network_cuda::GetUnitVars(units_mem, unit_vars_size, su_idx);
    sum += wts[st] * su->act;
    st++;
  }

  temp_sums[thr_no] = sum;

  __syncthreads();            // make sure all threads have written to temp_sums

  int i = nthrs / 2;            // now use a binary tree aggregation of temp_sums
  while( i!=0 ) {
    if(thr_no < i) {
      temp_sums[thr_no] += temp_sums[thr_no + i]; // get from next up
    }
    __syncthreads();
    i /= 2;                     // binary tree -- only earlier and earlier threads get it
  }

  if(thr_no == 0) {
    cg->temp1 = temp_sums[0]; // first guy has it all, store into our con group for later summation
  }
}


__global__ void Kernel_Compute_BpAct
(const int un_st_idx, char* units_mem, const int unit_vars_size, int unit_spec_size,
 int* recv_cgp_start, int* units_n_recv_cgps, char* net_cgp_mem, const int con_group_size)
{

  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = un_st_idx + blockIdx.x * nthrs + thr_no; // un_st_idx = 1 at minimum..

  UnitVars_cuda* u = Network_cuda::GetUnitVars(units_mem, unit_vars_size, un_idx);

  if(!(u->ext_flag & UnitVars_cuda::EXT)) {
    // first step is to sum up the netins for this guy
    float netin = 0.0f;
    const int cgp_st_idx = recv_cgp_start[un_idx];
    if(cgp_st_idx > 0) {
      const int n_cgps = units_n_recv_cgps[un_idx];
      for(int i=0; i<n_cgps; i++) {
        ConGroup_cuda* cg = Network_cuda::GetConGroup_Flat(net_cgp_mem, con_group_size,
                                                           cgp_st_idx + i);
        netin += cg->temp1;
      }
    }

    BpUnitSpec_cuda* us = (BpUnitSpec_cuda*)Network_cuda::GetUnitSpec
      (const_spec_mem, unit_spec_size, u->cuda_unit_spec_idx);
    
    u->net = netin;
    u->act = us->ActFromNetin(netin);
    // todo: dropout.. noise..
  }
  else {
    u->act = u->ext;
  }
}

void Bp_cuda::Compute_NetinAct() {
  ExtInputToDevice(true);       // external input comes from host..
  
  const int nlay = n_layers_built;
  for(int li=0; li < nlay; li++) {
    const int st_ui = LayUnStart(lay_unit_idxs_h, li);
    const int ed_ui = LayUnEnd(lay_unit_idxs_h, li);
    const int nu = ed_ui - st_ui;

    int n_cgps = units_n_recv_cgps_h[st_ui]; // assume same for all..
    if(n_cgps > 0) {
      for(int cgi = 0; cgi < n_cgps; cgi++) {
        //  Invoke kernel -- 3rd arg is size of memory to allocate to shared
        Kernel_Compute_Netin_ConGroup<<<nu, n_threads, n_threads, strm_compute_netin>>>
          (st_ui, cgi, recv_cgp_start_d, recv_cgp_mem_d, con_group_size,
           recv_cons_mem_d, units_mem_d, unit_vars_size);
        cudaSafeCall(cudaStreamSynchronize(strm_compute_netin));
      }
      // then aggregate netins and compute activations
    }

    // copy unit spec mem to constant
    cudaMemcpyToSymbol(const_spec_mem, unit_spec_mem_d, unit_spec_mem_tot);
    
    Kernel_Compute_BpAct<<<nu, n_threads, 0, strm_compute_act>>>
      (st_ui, units_mem_d, unit_vars_size, unit_spec_size,
       recv_cgp_start_d, units_n_recv_cgps_d, recv_cgp_mem_d, con_group_size);
    cudaSafeCall(cudaStreamSynchronize(strm_compute_act));
  }
}


//////////////////////////////////////////////////////////////////////////
//      layer-at-a-time dEdA and dEdNet


__global__ void Kernel_Compute_dEdA_ConGroup
(const int st_ui, const int cgp_idx, int* send_cgp_start, char* send_cgp_mem,
 const int con_group_size, float* send_cons_mem, char* units_mem, const int unit_vars_size,
 int* recv_cgp_start, float* recv_cons_mem)
{

  extern __shared__ float temp_sums[]; // third arg to kernel specifies size of this!

  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x; // blocks are *units*

  ConGroup_cuda* cg = Network_cuda::GetUnConGroup
    (send_cgp_mem, send_cgp_start, con_group_size, un_idx, cgp_idx);

  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  float sum = 0.0f;

  while(st < ed) {
    const int32_t ru_idx = cg->UnIdx(send_cons_mem, st);
    BpUnitVars_cuda* ru = (BpUnitVars_cuda*)Network_cuda::GetUnitVars
      (units_mem, unit_vars_size, ru_idx);
    // this is super deadly slow:
    const float wt = cg->PtrCn
      (send_cons_mem, send_cgp_mem, con_group_size, recv_cgp_start, st,
       recv_cons_mem, ConGroup_cuda::WT);
    sum += wt * ru->dEdNet;
    st++;
  }

  temp_sums[thr_no] = sum;

  __syncthreads();            // make sure all threads have written to temp_sums

  int i = nthrs / 2;            // now use a binary tree aggregation of temp_sums
  while( i!=0 ) {
    if(thr_no < i) {
      temp_sums[thr_no] += temp_sums[thr_no + i]; // get from next up
    }
    __syncthreads();
    i /= 2;                     // binary tree -- only earlier and earlier threads get it
  }

  if(thr_no == 0) {
    cg->temp1 = temp_sums[0]; // first guy has it all, store into our con group for later summation
  }
}

__global__ void Kernel_Compute_Err_dEdNet
(const int un_st_idx, char* units_mem, const int unit_vars_size, int unit_spec_size,
 int* send_cgp_start, int* units_n_send_cgps, char* net_cgp_mem, const int con_group_size)
{
  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = un_st_idx + blockIdx.x * nthrs + thr_no; // un_st_idx = 1 at minimum..

  BpUnitVars_cuda* u = (BpUnitVars_cuda*)Network_cuda::GetUnitVars
    (units_mem, unit_vars_size, un_idx);

  // first step is to sum up the netins for this guy
  float dEdA = 0.0f;
  const int cgp_st_idx = send_cgp_start[un_idx];
  if(cgp_st_idx > 0) {
    const int n_cgps = units_n_send_cgps[un_idx];
    for(int i=0; i<n_cgps; i++) {
      ConGroup_cuda* cg = Network_cuda::GetConGroup_Flat(net_cgp_mem, con_group_size,
                                                         cgp_st_idx + i);
      dEdA += cg->temp1;
    }
  }

  BpUnitSpec_cuda* us = (BpUnitSpec_cuda*)Network_cuda::GetUnitSpec
    (const_spec_mem, unit_spec_size, u->cuda_unit_spec_idx);
    
  u->dEdA = dEdA;

  // compute error
  if(u->ext_flag & UnitVars_cuda::TARG) {
    float err = u->targ - u->act;
    if(fabsf(err) >= us->err_tol) {
      if(us->error_fun == BpUnitSpec_cuda::SQUARED_ERR
         || us->act_fun == BpUnitSpec_cuda::SOFTMAX) { // softmax always uses this!
        u->dEdA += err;
      }
      else { // CROSS_ENTROPY
        if(us->act_fun == BpUnitSpec_cuda::TANH) {
          err /= (u->act + 1.0f) * (1.0f - u->act) * 2.0f;
        }
        else {
          err /= u->act * (1.0f - u->act);
        }
        u->dEdA += err;
      }
    }
  }
    
  u->dEdNet = u->dEdA * us->ActDeriv(u->act);
}

void Bp_cuda::Compute_dEdA_dEdNet() {
  const int nlay = n_layers_built;
  for(int li = nlay-1; li >= 0; li--) { // go in reverse order!
    const int st_ui = LayUnStart(lay_unit_idxs_h, li);
    const int ed_ui = LayUnEnd(lay_unit_idxs_h, li);
    const int nu = ed_ui - st_ui;

    BpUnitVars_cuda* u = (BpUnitVars_cuda*)Network_cuda::GetUnitVars
      (units_mem_h, unit_vars_size, st_ui);
    if(u->ext_flag & UnitVars_cuda::EXT)
      continue;                 // skip it!  todo: could have bp to inputs but..

    int n_cgps = units_n_send_cgps_h[st_ui]; // assume same for all..
    if(n_cgps > 0) {
      for(int cgi = 0; cgi < n_cgps; cgi++) {
        //  Invoke kernel -- 3rd arg is size of memory to allocate to shared
        Kernel_Compute_dEdA_ConGroup<<<nu, n_threads, n_threads, strm_compute_netin>>>
          (st_ui, cgi, send_cgp_start_d, send_cgp_mem_d, con_group_size,
           send_cons_mem_d, units_mem_d, unit_vars_size,
           recv_cgp_start_d, recv_cons_mem_d);
        cudaSafeCall(cudaStreamSynchronize(strm_compute_netin));
      }
      // then aggregate netins and compute activations
    }

    // copy unit spec mem to constant
    cudaMemcpyToSymbol(const_spec_mem, unit_spec_mem_d, unit_spec_mem_tot);
    
    Kernel_Compute_Err_dEdNet<<<nu, n_threads, 0, strm_compute_act>>>
      (st_ui, units_mem_d, unit_vars_size, unit_spec_size,
       send_cgp_start_d, units_n_send_cgps_d, send_cgp_mem_d, con_group_size);
    cudaSafeCall(cudaStreamSynchronize(strm_compute_act));
  }

  TargUnitsToHost(true);       // send output layer data back to host for stats..
}

