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

/////////////////////////////////////////////
//      layer-at-a-time netinput and act

__global__ void Kernel_Compute_Netin_OneLayer
(const int cgp_st_idx, char* net_cgp_mem, const int con_group_size, float* net_recv_cons_mem, char* net_units_mem, const int unit_vars_size) {

  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int cgp_idx = blockIdx.x + cgp_st_idx;

  extern __shared__ float temp_sums[]; // third arg to kernel specifies size of this!

  ConGroup_cuda* cg = Network_cuda::GetConGroup_Flat(net_cgp_mem, con_group_size, cgp_idx);

  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  const float* wts = cg->OwnCnVar(net_recv_cons_mem, ConGroup_cuda::WT);

  float sum = 0.0f;

  while(st < ed) {
    const int32_t su_idx = cg->UnIdx(net_recv_cons_mem, st);
    UnitVars_cuda* su = Network_cuda::GetUnitVars(net_units_mem, unit_vars_size, su_idx);
    sum += wts[st] * su->act;
    st++;
  }

  temp_sums[thr_no] = sum;

  __syncthreads();            // make sure all threads have written to temp_sums

  int i = nthrs / 2;            // now use a binary tree aggregation of temp_sums
  while( i!=0 ) {
    if(thr_no < i)
      temp_sums[thr_no] += temp_sums[thr_no + i]; // get from next up

    __syncthreads();
    i /= 2;                     // binary tree -- only earlier and earlier threads get it
  }

  if(thr_no == 0) {
    cg->temp1 = temp_sums[0]; // first guy has it all, store into our con group for later summation
  }
}

// this is just sample code showing how to roll up the netin and then compute act from
// that -- actual algorithm will have to do this with its own special code for act fun

__global__ void Kernel_Compute_Act_OneLayer
(const int un_st_idx, char* units_mem, const int unit_vars_size,
 int* recv_cgp_start, int* units_n_recv_cgps,
 char* net_cgp_mem, const int con_group_size) {

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

    u->net = netin;
    // exponential activation example:
    u->act = 1.0f / (1.0f + expf(netin));
  }
  else {
    u->act = u->ext;
  }
}

void Bp_cuda::Compute_NetinAct() {
  ExtInputToDevice(true);       // typically need the exts first!
  
  for(int i=0; i< n_layers_built; i++) {
    int st_ui = LayUnStart(lay_unit_idxs_h, i);
    int ed_ui = LayUnEnd(lay_unit_idxs_h, i);
    int nu = ed_ui - st_ui;

    int cgp_st_idx = recv_cgp_start_h[st_ui];
    if(cgp_st_idx > 0) {
      //  Invoke kernel -- 3rd arg is size of memory to allocate to shared
      Kernel_Compute_Netin_OneLayer<<<nu, n_threads, n_threads, strm_compute_netin>>>
        (cgp_st_idx, recv_cgp_mem_d, con_group_size, recv_cons_mem_d, units_mem_d,
         unit_vars_size);
    }
    cudaSafeCall(cudaStreamSynchronize(strm_compute_netin));
    // then aggregate netins and compute activations
    Kernel_Compute_Act_OneLayer<<<nu, n_threads, 0, strm_compute_netin>>>
      (st_ui, units_mem_d, unit_vars_size, recv_cgp_start_d, units_n_recv_cgps_d,
       recv_cgp_mem_d, con_group_size);
    cudaSafeCall(cudaStreamSynchronize(strm_compute_netin));
  }
}


