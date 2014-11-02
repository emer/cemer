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

#include "LeabraConSpec_cuda.h"

//  lldb print commands -- use memory read with format
//  me r -rf f -l148 -c148 send_netin_tmp_h

LeabraConSpecCuda::LeabraConSpecCuda() {
  Initialize();
}

LeabraConSpecCuda::~LeabraConSpecCuda() {
  FreeCudaArrays();
}

void LeabraConSpecCuda::Initialize() {
  n_units = 0;
  own_cons_max_size = 0;
  thread_chunk_sz = 8;
  max_threads = 0;
  n_threads = 256;
  own_cons_cnt = 0;
  ptr_cons_cnt = 0;
  own_units_x_cons = 0;
  ptr_units_x_cons = 0;

  strms_created = false;

  own_cons_mem_h = NULL;
  own_cons_mem_d = NULL;
  ptr_cons_mem_h = NULL;
  ptr_cons_mem_d = NULL;

  units_h = NULL;
  units_d = NULL;
  con_mem_idxs_h = NULL;
  con_mem_idxs_d = NULL;
  con_allocs_h = NULL;
  con_allocs_d = NULL;
  con_sizes_h = NULL;
  con_sizes_d = NULL;
  con_recv_idxs_h = NULL;
  con_recv_idxs_d = NULL;
  unit_starts_h = NULL;

  con_params_h = NULL;
  con_params_d = NULL;
  wt_sig_fun_h = NULL;
  wt_sig_fun_d = NULL;

  cur_units_x_cons_n = 0;
  cur_units_x_cons_h = NULL;
  cur_units_x_cons_d = NULL;
  send_net_acts_h = NULL;
  send_net_acts_d = NULL;
  send_net_max_prjns = 1;
  send_netin_tmp_h = NULL;
  send_netin_tmp_d = NULL;
  send_d5bnet_tmp_h = NULL;
  send_d5bnet_tmp_d = NULL;

  unit_vec_vars_h = NULL;
  unit_vec_vars_d = NULL;
}

void LeabraConSpecCuda::FreeCudaArrays() {
  if(own_cons_mem_d)
    cudaFree(own_cons_mem_d);
  if(ptr_cons_mem_d)
    cudaFree(ptr_cons_mem_d);

  if(units_h)
    free(units_h);
  if(units_d)
    cudaFree(units_d);

  if(con_mem_idxs_h)
    free(con_mem_idxs_h);
  if(con_mem_idxs_d)
    cudaFree(con_mem_idxs_d);

  if(con_allocs_h)
    free(con_allocs_h);
  if(con_allocs_d)
    cudaFree(con_allocs_d);

  if(con_sizes_h)
    free(con_sizes_h);
  if(con_sizes_d)
    cudaFree(con_sizes_d);

  if(con_recv_idxs_h)
    free(con_recv_idxs_h);
  if(con_recv_idxs_d)
    cudaFree(con_recv_idxs_d);

  if(unit_starts_h)
    free(unit_starts_h);

  if(con_params_h)
    free(con_params_h);
  if(con_params_d)
    cudaFree(con_params_d);

  if(wt_sig_fun_d)
    cudaFree(wt_sig_fun_d);

  if(cur_units_x_cons_h)
    free(cur_units_x_cons_h);
  if(cur_units_x_cons_d)
    cudaFree(cur_units_x_cons_d);

  if(send_net_acts_h)
    free(send_net_acts_h);
  if(send_net_acts_d)
    cudaFree(send_net_acts_d);

  if(send_netin_tmp_d)
    cudaFree(send_netin_tmp_d);

  if(send_d5bnet_tmp_d)
    cudaFree(send_d5bnet_tmp_d);

  if(unit_vec_vars_d)
    cudaFree(unit_vec_vars_d);

  if(strms_created) {
    cudaStreamDestroy(strm_updt_cons);
    cudaStreamDestroy(strm_send_netin);
    cudaStreamDestroy(strm_compute_dwt);
    cudaStreamDestroy(strm_compute_wt);
    strms_created = false;
  }

  Initialize();
}

void LeabraConSpecCuda::AllocCudaArrays
(int n_un, int own_cons_max_sz, bigint own_cnt, bigint ptr_cnt,
 int own_units_x, int ptr_units_x, 
 float* own_cons_mem, float* ptr_cons_mem, float* send_netin_tmp, int send_net_max_prj,
 float* send_d5bnet_tmp, float* unit_vec_vars)
{
  if(n_un == n_units && own_units_x == own_units_x_cons && own_cnt == own_cons_cnt)
    return;                     // already allocated

  FreeCudaArrays();

  if(n_un == 0 || own_units_x == 0)
    return;

  cudaStreamCreate(&strm_updt_cons);
  cudaStreamCreate(&strm_send_netin);
  cudaStreamCreate(&strm_compute_dwt);
  cudaStreamCreate(&strm_compute_wt);
  strms_created = true;

  n_units = n_un;
  own_cons_max_size = own_cons_max_sz;
  thread_chunk_sz = 8;
  max_threads = own_cons_max_size / thread_chunk_sz;

  // docs on number of threads: http://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#execution-configuration-optimizations

  int mod32 = max_threads % 32;
  if(mod32 != 0)
    n_threads = ((max_threads / 32) + 1) * 32;
  else
    n_threads = max_threads;
  if(n_threads > 256)
    n_threads = 256;

  own_cons_cnt = own_cnt;
  ptr_cons_cnt = ptr_cnt;
  own_units_x_cons = own_units_x;
  ptr_units_x_cons = ptr_units_x;

  own_cons_mem_h = own_cons_mem;
  ptr_cons_mem_h = ptr_cons_mem;
  send_netin_tmp_h = send_netin_tmp;
  send_net_max_prjns = send_net_max_prj;
  send_d5bnet_tmp_h = send_d5bnet_tmp;
  unit_vec_vars_h = unit_vec_vars;

  units_h = (int*)malloc(own_units_x_cons * sizeof(int));
  cudaSafeCall(cudaMalloc(&units_d, own_units_x_cons * sizeof(int)));

  con_mem_idxs_h = (bigint*)malloc(own_units_x_cons * sizeof(bigint));
  cudaSafeCall(cudaMalloc(&con_mem_idxs_d, own_units_x_cons * sizeof(bigint)));

  con_allocs_h = (int*)malloc(own_units_x_cons * sizeof(int));
  cudaSafeCall(cudaMalloc(&con_allocs_d, own_units_x_cons * sizeof(int)));

  con_sizes_h = (int*)malloc(own_units_x_cons * sizeof(int));
  cudaSafeCall(cudaMalloc(&con_sizes_d, own_units_x_cons * sizeof(int)));

  con_recv_idxs_h = (int*)malloc(own_units_x_cons * sizeof(int));
  cudaSafeCall(cudaMalloc(&con_recv_idxs_d, own_units_x_cons * sizeof(int)));

  unit_starts_h = (int*)malloc((n_units+1) * sizeof(int));

  con_params_h = (float*)malloc(own_units_x_cons * N_CON_PARAMS * sizeof(float));
  cudaSafeCall(cudaMalloc(&con_params_d, own_units_x_cons * N_CON_PARAMS * sizeof(float)));

  cudaSafeCall(cudaMalloc(&wt_sig_fun_d, 10002 * sizeof(float)));

  cur_units_x_cons_h = (int*)malloc(own_units_x_cons * sizeof(int));
  cudaSafeCall(cudaMalloc(&cur_units_x_cons_d, own_units_x_cons * sizeof(int)));

  send_net_acts_h = (float*)malloc(own_units_x_cons * sizeof(float));
  cudaSafeCall(cudaMalloc(&send_net_acts_d, own_units_x_cons * sizeof(float)));

  cudaSafeCall(cudaMalloc(&send_netin_tmp_d,
                          (n_units+1) * send_net_max_prjns * sizeof(float)));

  cudaSafeCall(cudaMalloc(&send_d5bnet_tmp_d, (n_units+1) * sizeof(float)));

  cudaSafeCall(cudaMalloc(&own_cons_mem_d, own_cons_cnt * sizeof(float)));

  cudaSafeCall(cudaMalloc(&unit_vec_vars_d, (n_units+1) * N_VEC_VARS * sizeof(float)));

  // conserve memory: not needed..
  //   cudaMalloc(&ptr_cons_mem_d, ptr_cons_cnt);
}

void LeabraConSpecCuda::OwnCons_HostToDevice(bool sync) {
  if(own_cons_mem_h && own_cons_mem_d) {
    cudaSafeCall(cudaMemcpyAsync(own_cons_mem_d, own_cons_mem_h,
                                 own_cons_cnt * sizeof(float),
                                 cudaMemcpyHostToDevice, strm_updt_cons));
    if(sync) {
      cudaSafeCall(cudaStreamSynchronize(strm_updt_cons));
    }
  }
}

void LeabraConSpecCuda::OwnCons_DeviceToHost(bool sync) {
  if(own_cons_mem_h && own_cons_mem_d) {
    cudaSafeCall(cudaMemcpyAsync(own_cons_mem_h, own_cons_mem_d,
                                 own_cons_cnt * sizeof(float),
                                 cudaMemcpyDeviceToHost, strm_updt_cons));
    if(sync) {
      cudaSafeCall(cudaStreamSynchronize(strm_updt_cons));
    }
  }
}

void LeabraConSpecCuda::UpdateUnitsXCons() {
  if(!con_mem_idxs_h) return;

  int sz =  own_units_x_cons * sizeof(int);

  cudaSafeCall(cudaMemcpy(units_d, units_h, sz, cudaMemcpyHostToDevice));
  cudaSafeCall(cudaMemcpy(con_mem_idxs_d, con_mem_idxs_h, sz, cudaMemcpyHostToDevice));
  cudaSafeCall(cudaMemcpy(con_allocs_d, con_allocs_h, sz, cudaMemcpyHostToDevice));
  cudaSafeCall(cudaMemcpy(con_sizes_d, con_sizes_h, sz, cudaMemcpyHostToDevice));
  cudaSafeCall(cudaMemcpy(con_recv_idxs_d, con_recv_idxs_h, sz, cudaMemcpyHostToDevice));
}

void LeabraConSpecCuda::UpdateConParams() {
  if(!con_params_h) return;

  int sz =  own_units_x_cons * N_CON_PARAMS * sizeof(float);

  cudaSafeCall(cudaMemcpy(con_params_d, con_params_h, sz, cudaMemcpyHostToDevice));

  if(wt_sig_fun_h) {
    cudaSafeCall(cudaMemcpy(wt_sig_fun_d, wt_sig_fun_h, 10002 * sizeof(float),
                            cudaMemcpyHostToDevice));
  }
}

__global__ void Kernel_Send_NetinDelta
(int* cur_units_x_cons_d, float* send_net_acts_d, float* send_netin_tmp_d,
 float* own_cons_mem_d, bigint* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d) {
  const int csni = blockIdx.x;
  const int nth = blockDim.x;
  const int ucidx = cur_units_x_cons_d[csni];
  const float send_eff = send_net_acts_d[csni];
  const int sz = con_sizes_d[ucidx];
  const float* wts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::WT));
  const int* ridxs = ((int*)own_cons_mem_d) + con_mem_idxs_d[ucidx];
  const int th = threadIdx.x;
  const float cn_per_th = ((float)sz / (float)nth);
  int st = __float2int_rn((float)th * cn_per_th);
  int ed = __float2int_rn((float)(th+1) * cn_per_th);
  ed = ed < sz ? ed : sz;     // max of sz
  while(st < ed) {
    int ridx = ridxs[st];
    atomicAdd(&(send_netin_tmp_d[ridx]), wts[st] * send_eff);
    // send_netin_tmp_d[ridx] += wts[st] * send_eff; // determine effect of atomic -- not much penalty there
    st++;
  }
}

__global__ void Kernel_Send_NetinDelta_per_prj
(int* cur_units_x_cons_d, float* send_net_acts_d, float* send_netin_tmp_d,
 float* own_cons_mem_d, bigint* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d,
 int* con_recv_idxs_d, const int nu) {
  const int csni = blockIdx.x;
  const int nth = blockDim.x;
  const int ucidx = cur_units_x_cons_d[csni];
  const float send_eff = send_net_acts_d[csni];
  const int sz = con_sizes_d[ucidx];
  const int recv_idx = con_recv_idxs_d[ucidx];
  float* send_netin_vec = send_netin_tmp_d + recv_idx * nu; // nu = n_units+1
  const float* wts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::WT));
  const int* ridxs = ((int*)own_cons_mem_d) + con_mem_idxs_d[ucidx];
  const int th = threadIdx.x;
  const float cn_per_th = ((float)sz / (float)nth);
  int st = __float2int_rn((float)th * cn_per_th);
  int ed = __float2int_rn((float)(th+1) * cn_per_th);
  //  ed = ed < sz ? ed : sz;     // max of sz
  while(st < ed) {
    int ridx = ridxs[st];
    atomicAdd(&(send_netin_vec[ridx]), wts[st] * send_eff);
    st++;
  }
}

void LeabraConSpecCuda::Send_NetinDelta() {
  if(cur_units_x_cons_n == 0) return;

  cudaSafeCall(cudaMemsetAsync(send_netin_tmp_d, 0,
                               (n_units+1) * send_net_max_prjns * sizeof(float),
                               strm_send_netin));

  cudaSafeCall(cudaMemcpyAsync(cur_units_x_cons_d, cur_units_x_cons_h,
                               cur_units_x_cons_n * sizeof(int),
                               cudaMemcpyHostToDevice, strm_send_netin));
  cudaSafeCall(cudaMemcpyAsync(send_net_acts_d, send_net_acts_h,
                               cur_units_x_cons_n * sizeof(float),
                               cudaMemcpyHostToDevice, strm_send_netin));

  if(send_net_max_prjns > 1) {
    //  Invoke kernel
    Kernel_Send_NetinDelta_per_prj<<<cur_units_x_cons_n, n_threads, 0, strm_send_netin>>>
      (cur_units_x_cons_d, send_net_acts_d, send_netin_tmp_d,
       own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d, con_recv_idxs_d,
       n_units+1);
  }
  else {
    //  Invoke kernel
    Kernel_Send_NetinDelta<<<cur_units_x_cons_n, n_threads, 0, strm_send_netin>>>
      (cur_units_x_cons_d, send_net_acts_d, send_netin_tmp_d,
       own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d);
  }

  cudaSafeCall(cudaMemcpyAsync(send_netin_tmp_h, send_netin_tmp_d,
                               (n_units+1) * send_net_max_prjns * sizeof(float),
                               cudaMemcpyDeviceToHost, strm_send_netin));
  // get results back from device -- args are reversed here!

  cudaSafeCall(cudaStreamSynchronize(strm_send_netin));
}


void LeabraConSpecCuda::Send_Deep5bNetinDelta() {
  if(cur_units_x_cons_n == 0) return;

  cudaSafeCall(cudaMemsetAsync(send_d5bnet_tmp_d, 0, (n_units+1) * sizeof(float),
                               strm_send_netin));

  cudaSafeCall(cudaMemcpyAsync(cur_units_x_cons_d, cur_units_x_cons_h,
                               cur_units_x_cons_n * sizeof(int),
                               cudaMemcpyHostToDevice, strm_send_netin));
  cudaSafeCall(cudaMemcpyAsync(send_net_acts_d, send_net_acts_h,
                               cur_units_x_cons_n * sizeof(float),
                               cudaMemcpyHostToDevice, strm_send_netin));

  //  Invoke kernel
  Kernel_Send_NetinDelta<<<cur_units_x_cons_n, n_threads, 0, strm_send_netin>>>
    (cur_units_x_cons_d, send_net_acts_d, send_d5bnet_tmp_d,
     own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d);

  cudaSafeCall(cudaMemcpyAsync(send_d5bnet_tmp_h, send_d5bnet_tmp_d,
                               (n_units+1) * sizeof(float),
                               cudaMemcpyDeviceToHost, strm_send_netin));
  // get results back from device -- args are reversed here!

  cudaSafeCall(cudaStreamSynchronize(strm_send_netin));
}

void LeabraConSpecCuda::Send_TICtxtNetin() {
  if(cur_units_x_cons_n == 0) return;

  cudaSafeCall(cudaMemsetAsync(send_netin_tmp_d, 0, (n_units+1) * sizeof(float),
                               strm_send_netin));

  cudaSafeCall(cudaMemcpyAsync(cur_units_x_cons_d, cur_units_x_cons_h,
                               cur_units_x_cons_n * sizeof(int),
                               cudaMemcpyHostToDevice, strm_send_netin));
  cudaSafeCall(cudaMemcpyAsync(send_net_acts_d, send_net_acts_h,
                               cur_units_x_cons_n * sizeof(float),
                               cudaMemcpyHostToDevice, strm_send_netin));

  //  Invoke kernel
  Kernel_Send_NetinDelta<<<cur_units_x_cons_n, n_threads, 0, strm_send_netin>>>
    (cur_units_x_cons_d, send_net_acts_d, send_netin_tmp_d,
     own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d);

  cudaSafeCall(cudaMemcpyAsync(send_netin_tmp_h, send_netin_tmp_d,
                               (n_units+1) * sizeof(float),
                               cudaMemcpyDeviceToHost, strm_send_netin));
  // get results back from device -- args are reversed here!

  cudaSafeCall(cudaStreamSynchronize(strm_send_netin));
}


__global__ void Kernel_Compute_dWt_cosdif
(int* cur_units_x_cons_d, float* unit_vec_vars_d, float* con_params_d, int* units_d,
 float* own_cons_mem_d, bigint* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d,
 const int nu) {
  const int csni = blockIdx.x;
  const int nth = blockDim.x;
  const int ucidx = cur_units_x_cons_d[csni];
  const int sidx = units_d[ucidx];

  const float su_avg_s = unit_vec_vars_d[LeabraConSpecCuda::AVG_S * nu + sidx];
  const float su_avg_m = unit_vec_vars_d[LeabraConSpecCuda::AVG_M * nu + sidx];

  const float s_mix = con_params_d[ucidx * LeabraConSpecCuda::N_CON_PARAMS +
                                   LeabraConSpecCuda::S_MIX];
  const float m_mix = con_params_d[ucidx * LeabraConSpecCuda::N_CON_PARAMS +
                                   LeabraConSpecCuda::M_MIX];
  const float thr_l_mix = con_params_d[ucidx * LeabraConSpecCuda::N_CON_PARAMS +
                                       LeabraConSpecCuda::THR_L_MIX];
  const float thr_max = con_params_d[ucidx * LeabraConSpecCuda::N_CON_PARAMS +
                                     LeabraConSpecCuda::THR_MAX];
  const float clrate = con_params_d[ucidx * LeabraConSpecCuda::N_CON_PARAMS +
                                    LeabraConSpecCuda::CUR_LRATE];

  const int sz = con_sizes_d[ucidx];
  float* dwts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::DWT));
  const int* ridxs = ((int*)own_cons_mem_d) + con_mem_idxs_d[ucidx];
  int th = threadIdx.x;
  const float cn_per_th = ((float)sz / (float)nth);
  int st = __float2int_rn((float)th * cn_per_th);
  int ed = __float2int_rn((float)(th+1) * cn_per_th);
  //  ed = ed < sz ? ed : sz;     // max of sz
  while(st < ed) {
    int ridx = ridxs[st];
    const float ru_avg_s = unit_vec_vars_d[LeabraConSpecCuda::AVG_S * nu + ridx];
    const float ru_avg_m = unit_vec_vars_d[LeabraConSpecCuda::AVG_M * nu + ridx];
    const float ru_avg_l = unit_vec_vars_d[LeabraConSpecCuda::AVG_L * nu + ridx];

    // unfortunately, cos_diff_lmix is on recv layer -- so this needs to be in the loop
    // whereas normally it is outside the loop.. would require a separate
    // var array just for it, at the unit_x_con level..  could look into it later
    const float cos_diff_lmix = unit_vec_vars_d[LeabraConSpecCuda::COS_DIFF_LMIX * nu
                                                + ridx];
    const float efflmix = thr_l_mix * cos_diff_lmix;
    const float effmmix = 1.0f - efflmix;
    const float su_act_mult = efflmix * su_avg_m;

    const float srs = ru_avg_s * su_avg_s;
    const float srm = ru_avg_m * su_avg_m;
    const float sm_mix = s_mix * srs + m_mix * srm;
    const float lthr = su_act_mult * ru_avg_l;
    float effthr = effmmix * srm + lthr;
    effthr = effthr < thr_max ? effthr : thr_max; // max = thr_max

    float rval;                 // xcal.dWtFun
    if(sm_mix < 0.0001f)        // d_thr = 0.0001
      rval = 0.0f;
    else if(sm_mix > effthr * 0.1f) // d_rev = 0.1
      rval = (sm_mix - effthr);
    else
      rval = sm_mix * -9.0f;    // d_rev_ratio = -9.0;
    dwts[st] += clrate * rval;
    st++;
  }
}

void LeabraConSpecCuda::Compute_dWt(bool sync) {
  if(cur_units_x_cons_n == 0) return;

  cudaSafeCall(cudaMemcpyAsync(cur_units_x_cons_d, cur_units_x_cons_h,
                               cur_units_x_cons_n * sizeof(int),
                               cudaMemcpyHostToDevice, strm_compute_dwt));
  cudaSafeCall(cudaMemcpyAsync(unit_vec_vars_d, unit_vec_vars_h,
                               (n_units+1) * N_VEC_VARS * sizeof(float),
                               cudaMemcpyHostToDevice, strm_compute_dwt));

  //  Invoke kernel
  Kernel_Compute_dWt_cosdif<<<cur_units_x_cons_n, n_threads, 0, strm_compute_dwt>>>
    (cur_units_x_cons_d, unit_vec_vars_d, con_params_d, units_d,
     own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d, n_units+1);

  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_compute_dwt));
  }
}

__global__ void Kernel_Compute_dWt_TICtxt
(int* cur_units_x_cons_d, float* unit_vec_vars_d, float* con_params_d, int* units_d,
 float* own_cons_mem_d, bigint* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d,
 const int nu) {
  const int csni = blockIdx.x;
  const int nth = blockDim.x;
  const int ucidx = cur_units_x_cons_d[csni];
  const int sidx = units_d[ucidx];

  const float su_act_q0 = unit_vec_vars_d[LeabraConSpecCuda::ACT_Q0 * nu + sidx];

  const float clrate = con_params_d[ucidx * LeabraConSpecCuda::N_CON_PARAMS +
                                    LeabraConSpecCuda::CUR_LRATE];

  const int sz = con_sizes_d[ucidx];
  float* dwts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::DWT));
  const int* ridxs = ((int*)own_cons_mem_d) + con_mem_idxs_d[ucidx];
  int th = threadIdx.x;
  const float cn_per_th = ((float)sz / (float)nth);
  int st = __float2int_rn((float)th * cn_per_th);
  int ed = __float2int_rn((float)(th+1) * cn_per_th);
  //  ed = ed < sz ? ed : sz;     // max of sz
  while(st < ed) {
    int ridx = ridxs[st];
    const float ru_avg_s = unit_vec_vars_d[LeabraConSpecCuda::AVG_S * nu + ridx];
    const float ru_avg_m = unit_vec_vars_d[LeabraConSpecCuda::AVG_M * nu + ridx];

    dwts[st] += clrate * (ru_avg_s - ru_avg_m) * su_act_q0;
    st++;
  }
}

void LeabraConSpecCuda::Compute_dWt_TICtxt(bool sync) {
  if(cur_units_x_cons_n == 0) return;

  cudaSafeCall(cudaMemcpyAsync(cur_units_x_cons_d, cur_units_x_cons_h,
                               cur_units_x_cons_n * sizeof(int),
                               cudaMemcpyHostToDevice, strm_compute_dwt));
  cudaSafeCall(cudaMemcpyAsync(unit_vec_vars_d, unit_vec_vars_h,
                               (n_units+1) * N_VEC_VARS * sizeof(float),
                               cudaMemcpyHostToDevice, strm_compute_dwt));

  //  Invoke kernel
  Kernel_Compute_dWt_TICtxt<<<cur_units_x_cons_n, n_threads, 0, strm_compute_dwt>>>
    (cur_units_x_cons_d, unit_vec_vars_d, con_params_d, units_d,
     own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d, n_units+1);

  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_compute_dwt));
  }
}



__global__ void Kernel_Compute_Weights
(float* own_cons_mem_d, bigint* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d,
 float* wt_sig_fun_d) {
  const int ucidx = blockIdx.x;  // full unit x con idx here
  const int nth = blockDim.x;

  const int sz = con_sizes_d[ucidx];
  float* wts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::WT));
  float* dwts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::DWT));
  float* fwts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::FWT));
  float* swts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::SWT));

  int th = threadIdx.x;
  const float cn_per_th = ((float)sz / (float)nth);
  int st = __float2int_rn((float)th * cn_per_th);
  int ed = __float2int_rn((float)(th+1) * cn_per_th);
  //  ed = ed < sz ? ed : sz;     // max of sz
  while(st < ed) {
    float& dwt = dwts[st];
    if(dwt != 0.0f) {
      float& wt = wts[st];
      float& fwt = fwts[st];
      float& swt = swts[st];
      if(dwt > 0.0f)  dwt *= (1.0f - fwt);
      else            dwt *= fwt;
      fwt += dwt;
      swt = fwt;                // keep sync'd -- not tech necc..

      int idx = __float2int_rd(fwt * 10000.0f); // sig_res_inv
      wt = wt_sig_fun_d[idx];

      dwt = 0.0f;
    }
    st++;
  }
}

void LeabraConSpecCuda::Compute_Weights(bool sync) {
  //  Invoke kernel -- does all
  Kernel_Compute_Weights<<<own_units_x_cons, n_threads, 0, strm_compute_wt>>>
    (own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d, wt_sig_fun_d);

  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_compute_wt));
  }
}


