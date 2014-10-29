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
  unit_starts_h = NULL;

  cur_send_net_n = 0;
  cur_send_net_h = NULL;
  cur_send_net_d = NULL;
  send_net_acts_h = NULL;
  send_net_acts_d = NULL;
  send_netin_tmp_h = NULL;
  send_netin_tmp_d = NULL;
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

  if(unit_starts_h)
    free(unit_starts_h);

  if(cur_send_net_h)
    free(cur_send_net_h);
  if(cur_send_net_d)
    cudaFree(cur_send_net_d);

  if(send_net_acts_h)
    free(send_net_acts_h);
  if(send_net_acts_d)
    cudaFree(send_net_acts_d);

  if(send_netin_tmp_d)
    cudaFree(send_netin_tmp_d);

  Initialize();
}

void LeabraConSpecCuda::AllocCudaArrays
(int n_un, int own_cons_max_sz, int64_t own_cnt, int64_t ptr_cnt,
 int own_units_x, int ptr_units_x, 
 float* own_cons_mem, float* ptr_cons_mem, float* send_netin_tmp) {
  if(n_units != n_un || own_units_x != own_units_x_cons) {
    FreeCudaArrays();
  }

  if(n_un == 0 || own_units_x == 0)
    return;

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

  units_h = (int*)malloc(own_units_x_cons * sizeof(int));
  cudaMalloc(&units_d, own_units_x_cons * sizeof(int));

  con_mem_idxs_h = (int64_t*)malloc(own_units_x_cons * sizeof(int64_t));
  cudaMalloc(&con_mem_idxs_d, own_units_x_cons * sizeof(int64_t));

  con_allocs_h = (int*)malloc(own_units_x_cons * sizeof(int));
  cudaMalloc(&con_allocs_d, own_units_x_cons * sizeof(int));

  con_sizes_h = (int*)malloc(own_units_x_cons * sizeof(int));
  cudaMalloc(&con_sizes_d, own_units_x_cons * sizeof(int));

  unit_starts_h = (int*)malloc((n_units+1) * sizeof(int));

  cur_send_net_h = (int*)malloc(own_units_x_cons * sizeof(int));
  cudaMalloc(&cur_send_net_d, own_units_x_cons * sizeof(int));

  send_net_acts_h = (float*)malloc(own_units_x_cons * sizeof(float));
  cudaMalloc(&send_net_acts_d, own_units_x_cons * sizeof(float));

  cudaMalloc(&send_netin_tmp_d, (n_units+1) * sizeof(float));

  cudaMalloc(&own_cons_mem_d, own_cons_cnt * sizeof(float));

  // conserve memory: not needed..
  //   cudaMalloc(&ptr_cons_mem_d, ptr_cons_cnt);
}

void LeabraConSpecCuda::UpdateOwnCons() {
  if(own_cons_mem_h && own_cons_mem_d) {
    cudaMemcpy(own_cons_mem_d, own_cons_mem_h, own_cons_cnt * sizeof(float),
               cudaMemcpyHostToDevice);
  }
}

void LeabraConSpecCuda::UpdateUnitsXCons() {
  if(!units_h) return;

  float sz =  own_units_x_cons * sizeof(float);

  cudaMemcpy(units_d, units_h, sz, cudaMemcpyHostToDevice);
  cudaMemcpy(con_mem_idxs_d, con_mem_idxs_h, sz, cudaMemcpyHostToDevice);
  cudaMemcpy(con_allocs_d, con_allocs_h, sz, cudaMemcpyHostToDevice);
  cudaMemcpy(con_sizes_d, con_sizes_h, sz, cudaMemcpyHostToDevice);
}

__global__ void Kernel_Send_NetinDelta
(int cur_send_net_n, int* cur_send_net_d, float* send_net_acts_d, float* send_netin_tmp_d,
 float* own_cons_mem_d, int64_t* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d) {
  int csni = blockIdx.x;
  int nth = blockDim.x;
  if (csni < cur_send_net_n) {
    int ucidx = cur_send_net_d[csni];
    float send_eff = send_net_acts_d[csni];
    const int sz = con_sizes_d[ucidx];
    const float* wts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
      (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::WT));
    const int* ridxs = ((int*)own_cons_mem_d) + con_mem_idxs_d[ucidx];
    int th = threadIdx.x;
    int cn_per_th = (sz / nth) + 1; // round up
    int st = th * cn_per_th;
    int ed = st + cn_per_th;
    ed = ed < sz ? ed : sz;     // max of sz
    while(st < ed) {
      int ridx = ridxs[st];
      atomicAdd(&(send_netin_tmp_d[ridx]), wts[st] * send_eff);
      st++;
    }
  }
}

void Test_Kernel_Send_NetinDelta
(int blockIdx, int blockDim, int threadIdx,
 int cur_send_net_n, int* cur_send_net_d, float* send_net_acts_d, float* send_netin_tmp_d,
 float* own_cons_mem_d, int64_t* con_mem_idxs_d, int* con_allocs_d, int* con_sizes_d) {
  int csni = blockIdx;
  int nth = blockDim;
  if (csni < cur_send_net_n) {
    int ucidx = cur_send_net_d[csni];
    float send_eff = send_net_acts_d[csni];
    const int sz = con_sizes_d[ucidx];
    const float* wts = own_cons_mem_d + con_mem_idxs_d[ucidx] +
      (con_allocs_d[ucidx] * (1 + LeabraConSpecCuda::WT));
    const int* ridxs = ((int*)own_cons_mem_d) + con_mem_idxs_d[ucidx];
    int th = threadIdx;
    int cn_per_th = (sz / nth) + 1; // round up
    int st = th * cn_per_th;
    int ed = st + cn_per_th;
    ed = ed < sz ? ed : sz;     // max of sz
    while(st < ed) {
      int ridx = ridxs[st];
      send_netin_tmp_d[ridx] += wts[st] * send_eff;
      st++;
    }
  }
}


void LeabraConSpecCuda::Send_NetinDelta() {
  if(cur_send_net_n == 0) return;

  float sz = cur_send_net_n * sizeof(float);

  cudaMemcpy(cur_send_net_d, cur_send_net_h, sz, cudaMemcpyHostToDevice);
  cudaMemcpy(send_net_acts_d, send_net_acts_h, sz, cudaMemcpyHostToDevice);

  cudaMemset(send_netin_tmp_d, 0, (n_units+1) * sizeof(float));

  // manual testing for getting all the logic right
  // for(int i=0; i< cur_send_net_n; i++) {
  //   for(int j=0; j< n_threads; j++) {
  //     Test_Kernel_Send_NetinDelta
  //       (i, n_threads, j, cur_send_net_n, cur_send_net_h, send_net_acts_h,
  //        send_netin_tmp_h, own_cons_mem_h, con_mem_idxs_h, con_allocs_h, con_sizes_h);
  //   }
  // }

  //  Invoke kernel
  Kernel_Send_NetinDelta<<<cur_send_net_n, n_threads>>>
    (cur_send_net_n, cur_send_net_d, send_net_acts_d, send_netin_tmp_d,
     own_cons_mem_d, con_mem_idxs_d, con_allocs_d, con_sizes_d);

  cudaDeviceSynchronize();

  cudaMemcpy(send_netin_tmp_h, send_netin_tmp_d, (n_units+1) * sizeof(float),
             cudaMemcpyDeviceToHost);
  // get results back from device -- args are reversed here!
}
