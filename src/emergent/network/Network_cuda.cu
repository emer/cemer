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

#include "Network_cuda.h"

//  lldb print commands -- use memory read with format
//  me r -rf f -l148 -c148 send_netin_tmp_h

Network_cuda::Network_cuda() {
  Initialize();
}

Network_cuda::~Network_cuda() {
  FreeCudaArrays();
}

void Network_cuda::Initialize() {
  strms_created = false;

  con_group_size = 0;
  unit_vars_size = 0;
  n_units_built = 0;
  n_layers_built = 0;
  n_ungps_built = 0;
  units_mem_h = NULL;
  units_mem_d = NULL;
  n_lay_stats = 0;
  n_lay_stats_vars = 0;
  lay_stats_h = NULL;
  lay_stats_d = NULL;
  units_n_recv_cgps_h = NULL;
  units_n_recv_cgps_d = NULL;
  units_n_send_cgps_h = NULL;
  units_n_send_cgps_d = NULL;
  n_recv_cgps = 0;
  n_send_cgps = 0;
  recv_cgp_mem_h = NULL;
  recv_cgp_mem_d = NULL;
  send_cgp_mem_h = NULL;
  send_cgp_mem_d = NULL;
  recv_cgp_start_h = NULL;
  recv_cgp_start_d = NULL;
  send_cgp_start_h = NULL;
  send_cgp_start_d = NULL;
  recv_cons_cnt = 0;
  send_cons_cnt = 0;
  recv_cons_mem_h = NULL;
  recv_cons_mem_d = NULL;
  send_cons_mem_h = NULL;
  send_cons_mem_d = NULL;
  own_cons_max_size = 0;
  own_cons_tot_size = 0;
  own_cons_tot_size_nonshared = 0;
  own_cons_avg_size = 0;
  own_cons_max_vars;

  con_params_h = NULL;
  con_params_d = NULL;
}

void Network_cuda::FreeCudaArrays() {
  if(recv_cons_mem_d)
    cudaFree(recv_cons_mem_d);
  if(send_cons_mem_d)
    cudaFree(send_cons_mem_d);

  if(recv_cgp_mem_d)
    cudaFree(recv_cgp_mem_d);
  if(send_cgp_mem_d)
    cudaFree(send_cgp_mem_d);
  if(recv_cgp_start_mem_d)
    cudaFree(recv_cgp_start_mem_d);
  if(send_cgp_start_mem_d)
    cudaFree(send_cgp_start_mem_d);
  
  if(units_n_recv_cgps_d)
    cudaFree(units_n_recv_cgps_d);
  if(units_n_send_cgps_d)
    cudaFree(units_n_send_cgps_d);

  if(lay_stats_d)
    cudaFree(lay_stats_d);
  
  if(units_mem_d)
    cudaFree(units_mem_d);

  if(con_params_h)
    free(con_params_h);
  if(con_params_d)
    cudaFree(con_params_d);

   if(strms_created) {
    cudaStreamDestroy(strm_memcpy_cons);
    cudaStreamDestroy(strm_memcpy_units);
    cudaStreamDestroy(strm_compute_netin);
    cudaStreamDestroy(strm_compute_dwt);
    cudaStreamDestroy(strm_compute_wt);
    strms_created = false;
  }

  Initialize();
}

void Network_cuda::AllocCudaArrays
( int    cgs,
  int    uvs,
  int    nub,
  int    nlb,
  int    nugb,
  char*  umh,

  int    nls,
  int    nlsv,
  float* lsh,
  int*   unrch,
  int*   unsch,
  
  int    nrcg,
  int    nscg,

  char*  rcgmh,
  char*  scgmh,
  int*   rcsh,
  int*   scsh,

  bigint rcc,
  bigint scc,
  float* rcmh,
  float* scmh
  )
{
  if(!strms_created) {
    cudaStreamCreate(&strm_memcpy_cons);
    cudaStreamCreate(&strm_memcpy_units);
    cudaStreamCreate(&strm_compute_netin);
    cudaStreamCreate(&strm_compute_act);
    cudaStreamCreate(&strm_compute_dwt);
    cudaStreamCreate(&strm_compute_wt);
    strms_created = true;
  }
  
  min_threads = min_th;
  max_threads = max_th;
  cons_per_thread = cons_per_th;

  // docs on number of threads: http://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#execution-configuration-optimizations

  n_threads = (int)round((float)own_cons_avg_size / (float)cons_per_thread);
  int mod32 = n_threads % 32;
  if(mod32 > 15)                // round up
    n_threads = ((n_threads / 32) + 1) * 32;
  else
    n_threads = (n_threads / 32) * 32;
  if(n_threads < min_threads)
    n_threads = min_threads;
  if(n_threads > max_threads)
    n_threads = max_threads;

  con_group_size = cgs;
  unit_vars_size = uvs;
  n_units_built = nub;
  n_layers_built = nlb;
  n_ungps_built = nugb;

  units_mem_h = umh;
  cudaSafeCall(cudaMalloc(&units_mem_d, n_units_built * sizeof(char)));

  n_lay_stats = nls;
  n_lay_stats_vars = nlsv;
  lay_stats_h = lsh;
  cudaSafeCall(cudaMalloc(&lay_stats_d, n_lay_stats * n_lay_stats_vars * n_layers_built * sizeof(float)));

  units_n_recv_cgps_h = unrch;
  cudaSafeCall(cudaMalloc(&units_n_recv_cgps_d, n_units_built * sizeof(int)));
  units_n_send_cgps_h = unsch;
  cudaSafeCall(cudaMalloc(&units_n_send_cgps_d, n_units_built * sizeof(int)));

  n_recv_cgps = nrcg;
  n_send_cgps = nscg;

  recv_cgp_mem_h = rcgmh;
  cudaSafeCall(cudaMalloc(&recv_cgp_mem_d, n_recv_cgps * con_group_size * sizeof(char)));
  send_cgp_mem_h = scgmh;
  cudaSafeCall(cudaMalloc(&send_cgp_mem_d, n_send_cgps * con_group_size * sizeof(char)));
  
  recv_cgp_start_h = rcsh;
  cudaSafeCall(cudaMalloc(&recv_cgp_start_d, n_units_built * sizeof(int)));
  send_cgp_start_h = scsh;
  cudaSafeCall(cudaMalloc(&send_cgp_start_d, n_units_built * sizeof(int)));

  recv_cons_cnt = rcc;
  send_cons_cnt = scc;
  
  recv_cons_mem_h = rcmh;
  cudaSafeCall(cudaMalloc(&recv_cons_mem_d, recv_cons_cnt * sizeof(float)));
  send_cons_mem_h = scmh;
  cudaSafeCall(cudaMalloc(&send_cons_mem_d, send_cons_cnt * sizeof(float)));
  
  con_params_h = (float*)malloc(n_recv_cgps * N_CON_PARAMS * sizeof(float));
  cudaSafeCall(cudaMalloc(&con_params_d, n_recv_cgps * N_CON_PARAMS * sizeof(float)));
}

void Network_cuda::RecvCons_HostToDevice(bool sync) {
  if(recv_cons_mem_h && recv_coms_mem_d) {
    cudaSafeCall(cudaMemcpyAsync(recv_cons_mem_d, recv_cons_mem_h,
                                 recv_cons_cnt * sizeof(float),
                                 cudaMemcpyHostToDevice, strm_memcpy_cons));
    if(sync) {
      cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));
    }
  }
}

void Network_cuda::RecvCons_DeviceToHost(bool sync) {
  if(recv_cons_mem_h && recv_cons_mem_d) {
    cudaSafeCall(cudaMemcpyAsync(recv_cons_mem_h, recv_cons_mem_d,
                                 recv_cons_cnt * sizeof(float),
                                 cudaMemcpyDeviceToHost, strm_memcpy_cons));
    if(sync) {
      cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));
    }
  }
}

void Network_cuda::UpdateUnitsXCons() {
  if(!con_mem_idxs_h) return;

  int sz =  recv_units_x_cons * sizeof(int);

  cudaSafeCall(cudaMemcpy(units_d, units_h, sz, cudaMemcpyHostToDevice));
  cudaSafeCall(cudaMemcpy(con_mem_idxs_d, con_mem_idxs_h, sz, cudaMemcpyHostToDevice));
  cudaSafeCall(cudaMemcpy(con_allocs_d, con_allocs_h, sz, cudaMemcpyHostToDevice));
  cudaSafeCall(cudaMemcpy(con_sizes_d, con_sizes_h, sz, cudaMemcpyHostToDevice));
  cudaSafeCall(cudaMemcpy(con_recv_idxs_d, con_recv_idxs_h, sz, cudaMemcpyHostToDevice));
}

void Network_cuda::UpdateConParams() {
  if(!con_params_h) return;

  int sz =  own_units_x_cons * N_CON_PARAMS * sizeof(float);

  cudaSafeCall(cudaMemcpy(con_params_d, con_params_h, sz, cudaMemcpyHostToDevice));

  if(wt_sig_fun_h) {
    cudaSafeCall(cudaMemcpy(wt_sig_fun_d, wt_sig_fun_h, 10002 * sizeof(float),
                            cudaMemcpyHostToDevice));
  }
}

__global__ void Kernel_Compute_Netin
(int* cur_units_x_cons_d, float* send_net_acts_d, float* compute_netin_tmp_d,
 float* own_cons_mem_d, bigint* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d) {
  const int csni = blockIdx.x;
  const int nth = blockDim.x;
  const int ucidx = cur_units_x_cons_d[csni];
  const float send_eff = send_net_acts_d[csni];
  const int sz = con_sizes_d[ucidx];
  const float* wts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + Network_cuda::WT));
  const int* ridxs = ((int*)own_cons_mem_d) + con_mem_idxs_d[ucidx];
  const int th = threadIdx.x;
  const float cn_per_th = ((float)sz / (float)nth);
  int st = __float2int_rn((float)th * cn_per_th);
  int ed = __float2int_rn((float)(th+1) * cn_per_th);
  ed = ed < sz ? ed : sz;     // max of sz
  while(st < ed) {
    int ridx = ridxs[st];
    atomicAdd(&(compute_netin_tmp_d[ridx]), wts[st] * send_eff);
    // compute_netin_tmp_d[ridx] += wts[st] * send_eff; // determine effect of atomic -- not much penalty there
    st++;
  }
}

void Network_cuda::Compute_Netin() {
  if(cur_units_x_cons_n == 0) return;

  // cudaSafeCall(cudaMemsetAsync(compute_netin_tmp_d, 0,
  //                              (n_units+1) * send_net_max_prjns * sizeof(float),
  //                              strm_compute_netin));

  cudaSafeCall(cudaMemcpyAsync(cur_units_x_cons_d, cur_units_x_cons_h,
                               cur_units_x_cons_n * sizeof(int),
                               cudaMemcpyHostToDevice, strm_compute_netin));
  cudaSafeCall(cudaMemcpyAsync(send_net_acts_d, send_net_acts_h,
                               cur_units_x_cons_n * sizeof(float),
                               cudaMemcpyHostToDevice, strm_compute_netin));

  //  Invoke kernel
  Kernel_Compute_NetinDelta<<<cur_units_x_cons_n, n_threads, 0, strm_compute_netin>>>
    (cur_units_x_cons_d, send_net_acts_d, compute_netin_tmp_d,
     own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d);

  cudaSafeCall(cudaMemcpyAsync(compute_netin_tmp_h, compute_netin_tmp_d,
                               (n_units+1) * send_net_max_prjns * sizeof(float),
                               cudaMemcpyDeviceToHost, strm_compute_netin));
  // get results back from device -- args are reversed here!

  cudaSafeCall(cudaStreamSynchronize(strm_compute_netin));
}


__global__ void Kernel_Compute_dWt
(int* cur_units_x_cons_d, float* unit_vec_vars_d, float* con_params_d, int* units_d,
 float* own_cons_mem_d, bigint* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d,
 const int nu) {
  const int csni = blockIdx.x;
  const int nth = blockDim.x;
  const int ucidx = cur_units_x_cons_d[csni];
  const int sidx = units_d[ucidx];

  const float su_avg_s = unit_vec_vars_d[Network_cuda::AVG_S * nu + sidx];
  const float su_avg_m = unit_vec_vars_d[Network_cuda::AVG_M * nu + sidx];

  const float s_mix = con_params_d[ucidx * Network_cuda::N_CON_PARAMS +
                                   Network_cuda::S_MIX];
  const float m_mix = con_params_d[ucidx * Network_cuda::N_CON_PARAMS +
                                   Network_cuda::M_MIX];
  const float thr_l_mix = con_params_d[ucidx * Network_cuda::N_CON_PARAMS +
                                       Network_cuda::THR_L_MIX];
  const float thr_max = con_params_d[ucidx * Network_cuda::N_CON_PARAMS +
                                     Network_cuda::THR_MAX];
  const float clrate = con_params_d[ucidx * Network_cuda::N_CON_PARAMS +
                                    Network_cuda::CUR_LRATE];

  const int sz = con_sizes_d[ucidx];
  float* dwts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + Network_cuda::DWT));
  const int* ridxs = ((int*)own_cons_mem_d) + con_mem_idxs_d[ucidx];
  int th = threadIdx.x;
  const float cn_per_th = ((float)sz / (float)nth);
  int st = __float2int_rn((float)th * cn_per_th);
  int ed = __float2int_rn((float)(th+1) * cn_per_th);
  //  ed = ed < sz ? ed : sz;     // max of sz
  while(st < ed) {
    int ridx = ridxs[st];
    const float ru_avg_s = unit_vec_vars_d[Network_cuda::AVG_S * nu + ridx];
    const float ru_avg_m = unit_vec_vars_d[Network_cuda::AVG_M * nu + ridx];
    const float ru_avg_l = unit_vec_vars_d[Network_cuda::AVG_L * nu + ridx];

    // unfortunately, cos_diff_lmix is on recv layer -- so this needs to be in the loop
    // whereas normally it is outside the loop.. would require a separate
    // var array just for it, at the unit_x_con level..  could look into it later
    const float cos_diff_lmix = unit_vec_vars_d[Network_cuda::COS_DIFF_LMIX * nu
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

void Network_cuda::Compute_dWt(bool sync) {
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

__global__ void Kernel_Compute_Weights
(float* own_cons_mem_d, bigint* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d,
 float* wt_sig_fun_d) {
  const int ucidx = blockIdx.x;  // full unit x con idx here
  const int nth = blockDim.x;

  const int sz = con_sizes_d[ucidx];
  float* wts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + Network_cuda::WT));
  float* dwts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + Network_cuda::DWT));
  float* fwts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + Network_cuda::FWT));
  float* swts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
    (con_allocs_d[ucidx] * (1 + Network_cuda::SWT));

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

void Network_cuda::Compute_Weights(bool sync) {
  //  Invoke kernel -- does all
  Kernel_Compute_Weights<<<own_units_x_cons, n_threads, 0, strm_compute_wt>>>
    (own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d, wt_sig_fun_d);

  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_compute_wt));
  }
}



