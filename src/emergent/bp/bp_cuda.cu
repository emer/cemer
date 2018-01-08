// Copyright 2017-2016, Regents of the University of Colorado,
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

__global__ void Kernel_Compute_Netin_ConState
(const int st_ui, const int cgp_idx, int* recv_cgp_start, char* recv_cgp_mem,
 const int con_group_size, float* recv_cons_mem, char* units_mem, const int unit_vars_size) {

  extern __shared__ float temp_sums[]; // third arg to kernel specifies size of this!

  const int un_idx = st_ui + blockIdx.x; // blocks are *units*

  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
    
  ConState_cuda* cg = Network_cuda::GetUnConState
    (recv_cgp_mem, recv_cgp_start, con_group_size, un_idx, cgp_idx);

  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  const float* wts = cg->OwnCnVar(recv_cons_mem, ConState_cuda::WT);

  float sum = 0.0f;

  while(st < ed) {
    UnitState_cuda* su = cg->UnVars(recv_cons_mem, units_mem, unit_vars_size, st);
    sum += wts[st] * su->act;
    st++;
  }

  temp_sums[thr_no] = sum;

  __syncthreads();            // make sure all threads have written to temp_sums

  // if(thr_no == 0) {
  //   sum = 0.0f;
  //   for(int i=0; i<nthrs; i++) {
  //     sum += temp_sums[i];
  //   }
  //   cg->temp1 = sum;
  // }
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


__global__ void Kernel_Compute_Act_Bp
(const int st_ui, const int ed_ui, char* units_mem, const int unit_vars_size,
 int unit_spec_size, int* recv_cgp_start, int* units_n_recv_cgps, char* recv_cgp_mem,
 const int con_group_size)
{

  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x * nthrs + thr_no;
  if(un_idx < ed_ui) {

    UnitState_cuda* u = Network_cuda::GetUnitState(units_mem, unit_vars_size, un_idx);

    if(!(u->ext_flag & UnitState_cuda::EXT)) {
      // first step is to sum up the netins for this guy
      float netin = 0.0f;
      const int n_cgps = units_n_recv_cgps[un_idx];
      for(int i=0; i<n_cgps; i++) {
        ConState_cuda* cg = Network_cuda::GetUnConState
          (recv_cgp_mem, recv_cgp_start, con_group_size, un_idx, i);
        netin += cg->temp1;
      }

      netin += u->bias_wt;      // assume bias specs..
      
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
}

void Bp_cuda::Compute_NetinAct() {
  ExtInputToDevice(true);       // external input comes from host..
  // IMPORTANT: this means that these layers MUST be sync'd *back to the host* for any
  // important changes, e.g., bias weights..
      
  // copy unit spec mem to constant
  cudaMemcpyToSymbol(const_spec_mem, unit_spec_mem_d, unit_spec_mem_tot);
  
  const int nlay = n_layers_built;
  for(int li=0; li < nlay; li++) {
    const int st_ui = LayUnStart(lay_unit_idxs_h, li);
    const int ed_ui = LayUnEnd(lay_unit_idxs_h, li);
    const int nu = ed_ui - st_ui;

    if(nu > 0) {

      const int n_cgps = units_n_recv_cgps_h[st_ui]; // assume same for all..
      if(n_cgps > 0) {
        for(int cgi = 0; cgi < n_cgps; cgi++) {
          //  Invoke kernel -- 3rd arg is size of memory to allocate to shared
          // use act stream here so netin and act are fully sync'd automatically
          Kernel_Compute_Netin_ConState<<<nu, n_threads, n_threads * sizeof(float), strm_compute_act>>>
            (st_ui, cgi, recv_cgp_start_d, recv_cgp_mem_d, con_group_size,
             recv_cons_mem_d, units_mem_d, unit_vars_size);
        }
      }

      // b/c units are accessed each per a diff thread, the n blocks is divided by threads
      const int n_blocks = (int)ceil((float)nu / (float)n_threads);

      Kernel_Compute_Act_Bp<<<n_blocks, n_threads, 0, strm_compute_act>>>
        (st_ui, ed_ui, units_mem_d, unit_vars_size, unit_spec_size,
         recv_cgp_start_d, units_n_recv_cgps_d, recv_cgp_mem_d, con_group_size);
      cudaSafeCall(cudaStreamSynchronize(strm_compute_act));
    }
  }
}


//////////////////////////////////////////////////////////////////////////
//      layer-at-a-time dEdA and dEdNet


__global__ void Kernel_Compute_dEdA_ConState
(const int st_ui, const int cgp_idx, int* send_cgp_start, char* send_cgp_mem,
 const int con_group_size, float* send_cons_mem, char* units_mem, const int unit_vars_size,
 int* recv_cgp_start, char* recv_cgp_mem, float* recv_cons_mem)
{

  extern __shared__ float temp_sums[]; // third arg to kernel specifies size of this!

  const int un_idx = st_ui + blockIdx.x; // blocks are *units*

  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
    
  ConState_cuda* cg = Network_cuda::GetUnConState
    (send_cgp_mem, send_cgp_start, con_group_size, un_idx, cgp_idx);

  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  float sum = 0.0f;

  while(st < ed) {
    BpUnitState_cuda* ru = (BpUnitState_cuda*)cg->UnVars
      (send_cons_mem, units_mem, unit_vars_size, st);
    // this is super deadly slow:
    const float wt = cg->PtrCn
      (send_cons_mem, con_group_size, recv_cgp_start, recv_cgp_mem, recv_cons_mem, st,
       ConState_cuda::WT);
    sum += wt * ru->dEdNet;
    st++;
  }

  temp_sums[thr_no] = sum;

  __syncthreads();            // make sure all threads have written to temp_sums

  // if(thr_no == 0) {
  //   sum = 0;
  //   for(int i=0; i<nthrs; i++) {
  //     sum += temp_sums[i];
  //   }
  //   cg->temp1 = sum;
  // }
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
(const int st_ui, const int ed_ui, char* units_mem, const int unit_vars_size, int unit_spec_size,
 int* send_cgp_start, int* units_n_send_cgps, char* send_cgp_mem, const int con_group_size)
{
  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x * nthrs + thr_no;

  if(un_idx < ed_ui) {
    
    BpUnitState_cuda* u = (BpUnitState_cuda*)Network_cuda::GetUnitState
      (units_mem, unit_vars_size, un_idx);

    // first step is to sum up the dEdA's for this guy
    float dEdA = 0.0f;
    const int n_cgps = units_n_send_cgps[un_idx];
    for(int i=0; i<n_cgps; i++) {
      ConState_cuda* cg = Network_cuda::GetUnConState
        (send_cgp_mem, send_cgp_start, con_group_size, un_idx, i);
      dEdA += cg->temp1;
    }

    BpUnitSpec_cuda* us = (BpUnitSpec_cuda*)Network_cuda::GetUnitSpec
      (const_spec_mem, unit_spec_size, u->cuda_unit_spec_idx);
    
    u->dEdA = dEdA;

    // compute error
    if(u->ext_flag & UnitState_cuda::TARG) {
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
}

void Bp_cuda::Compute_dEdA_dEdNet() {
  // copy unit spec mem to constant
  cudaMemcpyToSymbol(const_spec_mem, unit_spec_mem_d, unit_spec_mem_tot);
    
  const int nlay = n_layers_built;
  for(int li = nlay-1; li >= 0; li--) { // go in reverse order!
    const int st_ui = LayUnStart(lay_unit_idxs_h, li);
    const int ed_ui = LayUnEnd(lay_unit_idxs_h, li);
    const int nu = ed_ui - st_ui;

    if(nu > 0) {
      BpUnitState_cuda* u = (BpUnitState_cuda*)Network_cuda::GetUnitState
        (units_mem_h, unit_vars_size, st_ui);
      if(u->ext_flag & UnitState_cuda::EXT)
        continue;                 // skip it!  todo: could have bp to inputs but..

      const int n_cgps = units_n_send_cgps_h[st_ui]; // assume same for all..
      if(n_cgps > 0) {
        for(int cgi = 0; cgi < n_cgps; cgi++) {
          // Invoke kernel -- 3rd arg is size of memory to allocate to shared
          // use "act" stream to keep deda and dednet synchronized..
          Kernel_Compute_dEdA_ConState<<<nu, n_threads, n_threads * sizeof(float), strm_compute_act>>>
            (st_ui, cgi, send_cgp_start_d, send_cgp_mem_d, con_group_size,
             send_cons_mem_d, units_mem_d, unit_vars_size,
             recv_cgp_start_d, recv_cgp_mem_d, recv_cons_mem_d);
        }
        cudaSafeCall(cudaStreamSynchronize(strm_compute_netin));
      }

      // b/c units are accessed each per a diff thread, the n blocks is divided by threads
      const int n_blocks = (int)ceil((float)nu / (float)n_threads);

      Kernel_Compute_Err_dEdNet<<<n_blocks, n_threads, 0, strm_compute_act>>>
        (st_ui, ed_ui, units_mem_d, unit_vars_size, unit_spec_size,
         send_cgp_start_d, units_n_send_cgps_d, send_cgp_mem_d, con_group_size);
      cudaSafeCall(cudaStreamSynchronize(strm_compute_act));
    }
  }
}



//////////////////////////////////////////////////////////////////////////
//      Compute_dWt -- straight by congroups


__global__ void Kernel_Compute_dWt_Bp
(char* recv_cgp_mem, const int con_group_size, float* recv_cons_mem,
 char* units_mem, const int unit_vars_size) {

  const int cgp_idx = blockIdx.x; // blocks are connection groups
  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
    
  ConState_cuda* cg = Network_cuda::GetConState_Flat(recv_cgp_mem, con_group_size, cgp_idx);

  BpUnitState_cuda* ru = (BpUnitState_cuda*)cg->OwnUnVars(units_mem, unit_vars_size);

  const float ru_dEdNet = ru->dEdNet;
  
  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  float* dwts = cg->OwnCnVar(recv_cons_mem, ConState_cuda::DWT);

  while(st < ed) {
    UnitState_cuda* su = cg->UnVars(recv_cons_mem, units_mem, unit_vars_size, st);
    dwts[st] += su->act * ru_dEdNet;
    st++;
  }
}

__global__ void Kernel_Compute_dWt_Bp_Bias
(const int st_ui, const int ed_ui, char* units_mem, const int unit_vars_size)
{

  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x * nthrs + thr_no;
  if(un_idx < ed_ui) {
    BpUnitState_cuda* u = (BpUnitState_cuda*)Network_cuda::GetUnitState
      (units_mem, unit_vars_size, un_idx);
    u->bias_dwt += u->dEdNet;
  }
}

void Bp_cuda::Compute_dWt(bool sync) {
  // just throw the whole set of recv_cgps at it!

  int n_units_blocks = (int)ceil((float)n_units_built / (float)n_threads);
  
  Kernel_Compute_dWt_Bp<<<n_recv_cgps, n_threads, 0, strm_compute_dwt>>>
    (recv_cgp_mem_d, con_group_size, recv_cons_mem_d, units_mem_d, unit_vars_size);

  Kernel_Compute_dWt_Bp_Bias<<<n_units_blocks, n_threads, 0, strm_compute_dwt_bias>>>
    (0, n_units_built, units_mem_d, unit_vars_size);

  if(sync) {  
    cudaSafeCall(cudaStreamSynchronize(strm_compute_dwt));
    cudaSafeCall(cudaStreamSynchronize(strm_compute_dwt_bias));
  }
}


//////////////////////////////////////////////////////////////////////////
//      Compute_Weights -- straight by congroups


__global__ void Kernel_Compute_Weights_Bp_dWtOnly
(char* recv_cgp_mem, const int con_group_size, float* recv_cons_mem, int con_spec_size) {

  const int cgp_idx = blockIdx.x; // blocks are connection groups
  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
    
  ConState_cuda* cg = Network_cuda::GetConState_Flat(recv_cgp_mem, con_group_size, cgp_idx);

  BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
    (const_spec_mem, con_spec_size, cg->con_spec_idx);
    
  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  float* wts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::WT);
  float* dwts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::DWT);

  while(st < ed) {
    wts[st] += cs->cur_lrate * dwts[st];
    dwts[st] = 0.0f;
    st++;
  }
}

__global__ void Kernel_Compute_Weights_Bp_Bias_dWtOnly
(const int st_ui, const int ed_ui, char* units_mem, const int unit_vars_size,
 const int unit_spec_size, const int con_spec_mem_tot, const int con_spec_size)
{
  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x * nthrs + thr_no;
  if(un_idx < ed_ui) {
    BpUnitState_cuda* u = (BpUnitState_cuda*)Network_cuda::GetUnitState
      (units_mem, unit_vars_size, un_idx);

    BpUnitSpec_cuda* us = (BpUnitSpec_cuda*)Network_cuda::GetUnitSpec
      (const_spec_mem + con_spec_mem_tot, unit_spec_size, u->cuda_unit_spec_idx);

    if(us->bias_spec_idx >= 0) {
      BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
        (const_spec_mem, con_spec_size, us->bias_spec_idx);
      
      u->bias_wt += cs->cur_lrate * u->bias_dwt;
      u->bias_dwt = 0.0f;
    }
  }
}

__global__ void Kernel_Compute_Weights_Bp_SimpleDecay
(char* recv_cgp_mem, const int con_group_size, float* recv_cons_mem, int con_spec_size) {

  const int cgp_idx = blockIdx.x; // blocks are connection groups
  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
    
  ConState_cuda* cg = Network_cuda::GetConState_Flat(recv_cgp_mem, con_group_size, cgp_idx);

  BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
    (const_spec_mem, con_spec_size, cg->con_spec_idx);
    
  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  float* wts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::WT);
  float* dwts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::DWT);

  while(st < ed) {
    wts[st] += cs->cur_lrate * (dwts[st] - cs->decay * wts[st]);
    dwts[st] = 0.0f;
    st++;
  }
}

__global__ void Kernel_Compute_Weights_Bp_Bias_SimpleDecay
(const int st_ui, const int ed_ui, char* units_mem, const int unit_vars_size,
 const int unit_spec_size, const int con_spec_mem_tot, const int con_spec_size)
{
  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x * nthrs + thr_no;
  if(un_idx < ed_ui) {
    BpUnitState_cuda* u = (BpUnitState_cuda*)Network_cuda::GetUnitState
      (units_mem, unit_vars_size, un_idx);

    BpUnitSpec_cuda* us = (BpUnitSpec_cuda*)Network_cuda::GetUnitSpec
      (const_spec_mem + con_spec_mem_tot, unit_spec_size, u->cuda_unit_spec_idx);

    if(us->bias_spec_idx >= 0) {
      BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
        (const_spec_mem, con_spec_size, us->bias_spec_idx);
      
      u->bias_wt += cs->cur_lrate * (u->bias_dwt - cs->decay * u->bias_wt);
      u->bias_dwt = 0.0f;
    }
  }
}

__global__ void Kernel_Compute_Weights_Bp_Elimination
(char* recv_cgp_mem, const int con_group_size, float* recv_cons_mem, int con_spec_size) {

  const int cgp_idx = blockIdx.x; // blocks are connection groups
  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
    
  ConState_cuda* cg = Network_cuda::GetConState_Flat(recv_cgp_mem, con_group_size, cgp_idx);

  BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
    (const_spec_mem, con_spec_size, cg->con_spec_idx);
    
  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  float* wts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::WT);
  float* dwts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::DWT);

  while(st < ed) {
    const float wt = wts[st];
    float denom = (1.0f + wt * wt);
    wts[st] += cs->cur_lrate * (dwts[st] - ((cs->decay * wt) / (denom * denom)));
    dwts[st] = 0.0f;
    st++;
  }
}

__global__ void Kernel_Compute_Weights_Bp_Bias_Elimination
(const int st_ui, const int ed_ui, char* units_mem, const int unit_vars_size,
 const int unit_spec_size, const int con_spec_mem_tot, const int con_spec_size)
{
  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x * nthrs + thr_no;
  if(un_idx < ed_ui) {
    BpUnitState_cuda* u = (BpUnitState_cuda*)Network_cuda::GetUnitState
      (units_mem, unit_vars_size, un_idx);

    BpUnitSpec_cuda* us = (BpUnitSpec_cuda*)Network_cuda::GetUnitSpec
      (const_spec_mem + con_spec_mem_tot, unit_spec_size, u->cuda_unit_spec_idx);

    if(us->bias_spec_idx >= 0) {
      BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
        (const_spec_mem, con_spec_size, us->bias_spec_idx);
      
      const float wt = u->bias_wt;
      float denom = (1.0f + wt * wt);
      u->bias_wt += cs->cur_lrate * (u->bias_dwt - ((cs->decay * wt) / (denom * denom)));
      u->bias_dwt = 0.0f;
    }
  }
}

__global__ void Kernel_Compute_Weights_Bp_Moment
(char* recv_cgp_mem, const int con_group_size, float* recv_cons_mem, int con_spec_size) {

  const int cgp_idx = blockIdx.x; // blocks are connection groups
  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
    
  ConState_cuda* cg = Network_cuda::GetConState_Flat(recv_cgp_mem, con_group_size, cgp_idx);

  BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
    (const_spec_mem, con_spec_size, cg->con_spec_idx);
    
  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  float* wts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::WT);
  float* dwts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::DWT);
  float* pdws = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::PDW);

  while(st < ed) {
    pdws[st] = cs->cur_lrate * dwts[st] + cs->momentum * pdws[st];
    wts[st] += pdws[st];
    dwts[st] = 0.0f;
    st++;
  }
}

__global__ void Kernel_Compute_Weights_Bp_Bias_Moment
(const int st_ui, const int ed_ui, char* units_mem, const int unit_vars_size,
 const int unit_spec_size, const int con_spec_mem_tot, const int con_spec_size)
{
  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x * nthrs + thr_no;
  if(un_idx < ed_ui) {
    BpUnitState_cuda* u = (BpUnitState_cuda*)Network_cuda::GetUnitState
      (units_mem, unit_vars_size, un_idx);

    BpUnitSpec_cuda* us = (BpUnitSpec_cuda*)Network_cuda::GetUnitSpec
      (const_spec_mem + con_spec_mem_tot, unit_spec_size, u->cuda_unit_spec_idx);

    if(us->bias_spec_idx >= 0) {
      BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
        (const_spec_mem, con_spec_size, us->bias_spec_idx);

      u->bias_pdw = cs->cur_lrate * u->bias_dwt + cs->momentum * u->bias_pdw;
      u->bias_wt += u->bias_pdw;
      u->bias_dwt = 0.0f;
    }
  }
}

__global__ void Kernel_Compute_Weights_Bp_Moment_Simple
(char* recv_cgp_mem, const int con_group_size, float* recv_cons_mem, int con_spec_size) {

  const int cgp_idx = blockIdx.x; // blocks are connection groups
  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
    
  ConState_cuda* cg = Network_cuda::GetConState_Flat(recv_cgp_mem, con_group_size, cgp_idx);

  BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
    (const_spec_mem, con_spec_size, cg->con_spec_idx);
    
  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  float* wts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::WT);
  float* dwts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::DWT);
  float* pdws = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::PDW);

  while(st < ed) {
    pdws[st] = cs->cur_lrate * (dwts[st] - cs->decay * wts[st]) + cs->momentum * pdws[st];
    wts[st] += pdws[st];
    dwts[st] = 0.0f;
    st++;
  }
}

__global__ void Kernel_Compute_Weights_Bp_Bias_Moment_Simple
(const int st_ui, const int ed_ui, char* units_mem, const int unit_vars_size,
 const int unit_spec_size, const int con_spec_mem_tot, const int con_spec_size)
{
  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x * nthrs + thr_no;
  if(un_idx < ed_ui) {
    BpUnitState_cuda* u = (BpUnitState_cuda*)Network_cuda::GetUnitState
      (units_mem, unit_vars_size, un_idx);

    BpUnitSpec_cuda* us = (BpUnitSpec_cuda*)Network_cuda::GetUnitSpec
      (const_spec_mem + con_spec_mem_tot, unit_spec_size, u->cuda_unit_spec_idx);

    if(us->bias_spec_idx >= 0) {
      BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
        (const_spec_mem, con_spec_size, us->bias_spec_idx);
      
      u->bias_pdw = cs->cur_lrate * (u->bias_dwt - cs->decay * u->bias_wt) +
        cs->momentum * u->bias_pdw;
      u->bias_wt += u->bias_pdw;
      u->bias_dwt = 0.0f;
    }
  }
}

__global__ void Kernel_Compute_Weights_Bp_Moment_Elim
(char* recv_cgp_mem, const int con_group_size, float* recv_cons_mem, int con_spec_size) {

  const int cgp_idx = blockIdx.x; // blocks are connection groups
  const int nthrs = blockDim.x; // threads are connections
  const int thr_no = threadIdx.x;
    
  ConState_cuda* cg = Network_cuda::GetConState_Flat(recv_cgp_mem, con_group_size, cgp_idx);

  BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
    (const_spec_mem, con_spec_size, cg->con_spec_idx);
    
  const int sz = cg->size;
  int st, ed;
  Network_cuda::GetThreadCons(nthrs, thr_no, sz, st, ed);
     
  float* wts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::WT);
  float* dwts = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::DWT);
  float* pdws = cg->OwnCnVar(recv_cons_mem, BpConSpec_cuda::PDW);

  while(st < ed) {
    const float wt = wts[st];
    float denom = (1.0f + wt * wt);
    pdws[st] += cs->cur_lrate * (dwts[st] - ((cs->decay * wt) / (denom * denom))) +
      cs->momentum * pdws[st];
    wts[st] += pdws[st];
    dwts[st] = 0.0f;
    st++;
  }
}

__global__ void Kernel_Compute_Weights_Bp_Bias_Moment_Elim
(const int st_ui, const int ed_ui, char* units_mem, const int unit_vars_size,
 const int unit_spec_size, const int con_spec_mem_tot, const int con_spec_size)
{
  // each thread just gets a different unit -- doesn't do multiple units
  const int nthrs = blockDim.x;
  const int thr_no = threadIdx.x;
  const int un_idx = st_ui + blockIdx.x * nthrs + thr_no;
  if(un_idx < ed_ui) {
    BpUnitState_cuda* u = (BpUnitState_cuda*)Network_cuda::GetUnitState
      (units_mem, unit_vars_size, un_idx);

    BpUnitSpec_cuda* us = (BpUnitSpec_cuda*)Network_cuda::GetUnitSpec
      (const_spec_mem + con_spec_mem_tot, unit_spec_size, u->cuda_unit_spec_idx);

    if(us->bias_spec_idx >= 0) {
      BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
        (const_spec_mem, con_spec_size, us->bias_spec_idx);
      
      const float wt = u->bias_wt;
      float denom = (1.0f + wt * wt);
      u->bias_pdw = cs->cur_lrate * (u->bias_dwt - ((cs->decay * wt) / (denom * denom))) +
        cs->momentum * u->bias_pdw;
      u->bias_wt += u->bias_pdw;
      u->bias_dwt = 0.0f;
    }
  }
}

void Bp_cuda::Compute_Weights(bool sync) {
  // copy con spec mem to constant
  cudaMemcpyToSymbol(const_spec_mem, con_spec_mem_d, con_spec_mem_tot);
  // copy unit spec mem to constant, position AFTER con specs (4th arg)
  cudaMemcpyToSymbol(const_spec_mem, unit_spec_mem_d, unit_spec_mem_tot, con_spec_mem_tot);
  
  BpConSpec_cuda* cs = (BpConSpec_cuda*)Network_cuda::GetConSpec
    (con_spec_mem_h, con_spec_size, 0);

  int n_units_blocks = (int)ceil((float)n_units_built / (float)n_threads);
  
  switch(cs->wt_updt) {
  case BpConSpec_cuda::WU_DWT_ONLY: {
    Kernel_Compute_Weights_Bp_dWtOnly<<<n_recv_cgps, n_threads, 0, strm_compute_weights>>>
      (recv_cgp_mem_d, con_group_size, recv_cons_mem_d, con_spec_size);
    Kernel_Compute_Weights_Bp_Bias_dWtOnly<<<n_units_blocks, n_threads, 0, strm_compute_weights_bias>>>
      (0, n_units_built, units_mem_d, unit_vars_size, unit_spec_size, con_spec_mem_tot,
       con_spec_size);
    break;
  }
  case BpConSpec_cuda::WU_SIMPLE_DECAY: {
    Kernel_Compute_Weights_Bp_SimpleDecay<<<n_recv_cgps, n_threads, 0, strm_compute_weights>>>
      (recv_cgp_mem_d, con_group_size, recv_cons_mem_d, con_spec_size);
    Kernel_Compute_Weights_Bp_Bias_SimpleDecay<<<n_units_blocks, n_threads, 0, strm_compute_weights_bias>>>
      (0, n_units_built, units_mem_d, unit_vars_size, unit_spec_size, con_spec_mem_tot,
       con_spec_size);
    break;
  }
  case BpConSpec_cuda::WU_ELIMINATION: {
    Kernel_Compute_Weights_Bp_Elimination<<<n_recv_cgps, n_threads, 0, strm_compute_weights>>>
      (recv_cgp_mem_d, con_group_size, recv_cons_mem_d, con_spec_size);
    Kernel_Compute_Weights_Bp_Bias_Elimination<<<n_units_blocks, n_threads, 0, strm_compute_weights_bias>>>
      (0, n_units_built, units_mem_d, unit_vars_size, unit_spec_size, con_spec_mem_tot,
       con_spec_size);
    break;
  }
  case BpConSpec_cuda::WU_MOMENT: {
    Kernel_Compute_Weights_Bp_Moment<<<n_recv_cgps, n_threads, 0, strm_compute_weights>>>
      (recv_cgp_mem_d, con_group_size, recv_cons_mem_d, con_spec_size);
    Kernel_Compute_Weights_Bp_Bias_Moment<<<n_units_blocks, n_threads, 0, strm_compute_weights_bias>>>
      (0, n_units_built, units_mem_d, unit_vars_size, unit_spec_size, con_spec_mem_tot,
       con_spec_size);
    break;
  }
  case BpConSpec_cuda::WU_MOMENT_SIMPLE: {
    Kernel_Compute_Weights_Bp_Moment_Simple<<<n_recv_cgps, n_threads, 0, strm_compute_weights>>>
      (recv_cgp_mem_d, con_group_size, recv_cons_mem_d, con_spec_size);
    Kernel_Compute_Weights_Bp_Bias_Moment_Simple<<<n_units_blocks, n_threads, 0, strm_compute_weights_bias>>>
      (0, n_units_built, units_mem_d, unit_vars_size, unit_spec_size, con_spec_mem_tot,
       con_spec_size);
    break;
  }
  case BpConSpec_cuda::WU_MOMENT_ELIM: {
    Kernel_Compute_Weights_Bp_Moment_Elim<<<n_recv_cgps, n_threads, 0, strm_compute_weights>>>
      (recv_cgp_mem_d, con_group_size, recv_cons_mem_d, con_spec_size);
    Kernel_Compute_Weights_Bp_Bias_Moment_Elim<<<n_units_blocks, n_threads, 0, strm_compute_weights_bias>>>
      (0, n_units_built, units_mem_d, unit_vars_size, unit_spec_size, con_spec_mem_tot,
       con_spec_size);
    break;
  }
  }

  if(sync) {                    // generally doesn't have to be sync..
    cudaSafeCall(cudaStreamSynchronize(strm_compute_weights));
    cudaSafeCall(cudaStreamSynchronize(strm_compute_weights_bias));
  }
}
