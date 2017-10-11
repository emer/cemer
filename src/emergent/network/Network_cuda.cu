// Copyright 2016, Regents of the University of Colorado,
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
  NetFree();
}

void Network_cuda::Initialize() {
  strms_created = false;

  min_threads = 0;
  max_threads = 0;
  cons_per_thread = 0;
  n_threads = 0;
  
  unit_state_size = 0;
  n_units_built = 0;
  n_layers_built = 0;
  n_ungps_built = 0;
  units_mem_h = NULL;
  units_mem_d = NULL;
  lay_unit_idxs_h = NULL;
  lay_unit_idxs_d = NULL;
  ungp_unit_idxs_h = NULL;
  ungp_unit_idxs_d = NULL;
  n_lay_stats = 0;
  n_lay_stats_vars = 0;
  lay_stats_h = NULL;
  lay_stats_d = NULL;
  recv_owns_cons = true;
  con_state_size = 0;
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
  
  n_unit_specs = 0;
  unit_spec_mem_tot = 0;
  unit_spec_mem_h = NULL;
  unit_spec_mem_d = NULL;

  n_con_specs = 0;
  con_spec_mem_tot = 0;
  con_spec_mem_h = NULL;
  con_spec_mem_d = NULL;
}

void Network_cuda::NetFree() {
  if(recv_cons_mem_d)
    cudaFree(recv_cons_mem_d);
  if(send_cons_mem_d)
    cudaFree(send_cons_mem_d);

  if(recv_cgp_mem_d)
    cudaFree(recv_cgp_mem_d);
  if(send_cgp_mem_d)
    cudaFree(send_cgp_mem_d);
  if(recv_cgp_mem_h)
    free(recv_cgp_mem_h);
  if(send_cgp_mem_h)
    free(send_cgp_mem_h);
  if(recv_cgp_start_d)
    cudaFree(recv_cgp_start_d);
  if(send_cgp_start_d)
    cudaFree(send_cgp_start_d);
  
  if(units_n_recv_cgps_d)
    cudaFree(units_n_recv_cgps_d);
  if(units_n_send_cgps_d)
    cudaFree(units_n_send_cgps_d);

  if(lay_stats_d)
    cudaFree(lay_stats_d);
  
  if(lay_unit_idxs_d)
    cudaFree(lay_unit_idxs_d);
  if(ungp_unit_idxs_d)
    cudaFree(ungp_unit_idxs_d);
  
  if(units_mem_d)
    cudaFree(units_mem_d);

  if(unit_spec_mem_d)
    cudaFree(unit_spec_mem_d);
  if(unit_spec_mem_h)
    free(unit_spec_mem_h);

  if(con_spec_mem_d)
    cudaFree(con_spec_mem_d);
  if(con_spec_mem_h)
    free(con_spec_mem_h);

  if(strms_created) {
    cudaStreamDestroy(strm_memcpy_cons);
    cudaStreamDestroy(strm_memcpy_units);
    cudaStreamDestroy(strm_compute_netin);
    cudaStreamDestroy(strm_compute_dwt);
    cudaStreamDestroy(strm_compute_dwt_bias);
    cudaStreamDestroy(strm_compute_weights);
    cudaStreamDestroy(strm_compute_weights_bias);
    strms_created = false;
  }

  Initialize();
}

void Network_cuda::NetAlloc
( int    uvs,
  int    nub,
  int    nlb,
  int    nugb,
  char*  umh,
  int*   luih,
  int*   uuih,

  int    nls,
  int    nlsv,
  float* lsh,

  bool   roc,
  int*   unrch,
  int*   unsch,
  int    nrcg,
  int    nscg,
  int*   rcsh,
  int*   scsh,

  cudabigint rcc,
  cudabigint scc,
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
    cudaStreamCreate(&strm_compute_dwt_bias);
    cudaStreamCreate(&strm_compute_weights);
    cudaStreamCreate(&strm_compute_weights_bias);
    strms_created = true;
  }
  
  unit_state_size = uvs;
  n_units_built = nub;
  n_layers_built = nlb;
  n_ungps_built = nugb;

  units_mem_h = umh;
  cudaSafeCall(cudaMalloc(&units_mem_d, n_units_built * unit_state_size));

  lay_unit_idxs_h = luih;
  cudaSafeCall(cudaMalloc(&lay_unit_idxs_d, n_layers_built * 2 * sizeof(int)));
  ungp_unit_idxs_h = uuih;
  cudaSafeCall(cudaMalloc(&ungp_unit_idxs_d, n_ungps_built * 2 * sizeof(int)));
  
  n_lay_stats = nls;
  n_lay_stats_vars = nlsv;
  lay_stats_h = lsh;
  cudaSafeCall(cudaMalloc(&lay_stats_d, n_lay_stats * n_lay_stats_vars * n_layers_built * sizeof(float)));

  recv_owns_cons = roc;
  
  units_n_recv_cgps_h = unrch;
  cudaSafeCall(cudaMalloc(&units_n_recv_cgps_d, n_units_built * sizeof(int)));
  units_n_send_cgps_h = unsch;
  cudaSafeCall(cudaMalloc(&units_n_send_cgps_d, n_units_built * sizeof(int)));

  n_recv_cgps = nrcg;
  n_send_cgps = nscg;

  con_state_size = sizeof(ConState_cuda);
  recv_cgp_mem_h = (char*)malloc(n_recv_cgps * con_state_size);
  cudaSafeCall(cudaMalloc(&recv_cgp_mem_d, n_recv_cgps * con_state_size));
  send_cgp_mem_h = (char*)malloc(n_send_cgps * con_state_size);
  cudaSafeCall(cudaMalloc(&send_cgp_mem_d, n_send_cgps * con_state_size));
  
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
}

void Network_cuda::NetToDevice() {
  if(!lay_unit_idxs_h) return;

  cudaSafeCall
    (cudaMemcpy(lay_unit_idxs_d, lay_unit_idxs_h, n_layers_built * 2 * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(ungp_unit_idxs_d, ungp_unit_idxs_h, n_ungps_built * 2 * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(units_n_recv_cgps_d, units_n_recv_cgps_h, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(units_n_send_cgps_d, units_n_send_cgps_h, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(recv_cgp_start_d, recv_cgp_start_h, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(send_cgp_start_d, send_cgp_start_h, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(recv_cgp_mem_d, recv_cgp_mem_h, n_recv_cgps * con_state_size,
                          cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(send_cgp_mem_d, send_cgp_mem_h, n_send_cgps * con_state_size,
                          cudaMemcpyHostToDevice));

  if(!recv_owns_cons) {         // copy the NON-owned cons -- just structural -- no data
    cudaSafeCall
      (cudaMemcpy(recv_cons_mem_d, recv_cons_mem_h, recv_cons_cnt * sizeof(float),
                  cudaMemcpyHostToDevice));
  }
  else {
    cudaSafeCall
      (cudaMemcpy(send_cons_mem_d, send_cons_mem_h, send_cons_cnt * sizeof(float),
                  cudaMemcpyHostToDevice));
  }
}

int Network_cuda::SetCudaParams(int min_th, int max_th, int cons_per_th,
                                int own_cons_avg_size) {
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
  return n_threads;
}

void Network_cuda::OwnCons_HostToDevice(bool sync) {
  if(recv_owns_cons) {
    if(!(recv_cons_mem_h && recv_cons_mem_d)) return;
    cudaSafeCall(cudaMemcpyAsync(recv_cons_mem_d, recv_cons_mem_h,
                                 recv_cons_cnt * sizeof(float),
                                 cudaMemcpyHostToDevice, strm_memcpy_cons));
    if(sync) {
      cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));
    }
  }
  else {
    if(!(send_cons_mem_h && send_cons_mem_d)) return;
    cudaSafeCall(cudaMemcpyAsync(send_cons_mem_d, send_cons_mem_h,
                                 send_cons_cnt * sizeof(float),
                                 cudaMemcpyHostToDevice, strm_memcpy_cons));
    if(sync) {
      cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));
    }
  }
}

void Network_cuda::OwnCons_DeviceToHost(bool sync) {
  if(recv_owns_cons) {
    if(!(recv_cons_mem_h && recv_cons_mem_d)) return;
    cudaSafeCall(cudaMemcpyAsync(recv_cons_mem_h, recv_cons_mem_d,
                                 recv_cons_cnt * sizeof(float),
                                 cudaMemcpyDeviceToHost, strm_memcpy_cons));
    if(sync) {
      cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));
    }
  }
  else {
    if(!(send_cons_mem_h && send_cons_mem_d)) return;
    cudaSafeCall(cudaMemcpyAsync(send_cons_mem_h, send_cons_mem_d,
                                 send_cons_cnt * sizeof(float),
                                 cudaMemcpyDeviceToHost, strm_memcpy_cons));
    if(sync) {
      cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));
    }
  }
}

void Network_cuda::UnitState_HostToDevice(bool sync) {
  if(!(units_mem_h && units_mem_d)) return;
  cudaSafeCall
    (cudaMemcpyAsync(units_mem_d, units_mem_h, n_units_built * unit_state_size,
                     cudaMemcpyHostToDevice, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

void Network_cuda::UnitState_DeviceToHost(bool sync) {
  if(!(units_mem_h && units_mem_d)) return;
  cudaSafeCall
    (cudaMemcpyAsync(units_mem_h, units_mem_d, n_units_built * unit_state_size,
                     cudaMemcpyDeviceToHost, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

void Network_cuda::ExtInputToDevice(bool sync) {
  if(!(units_mem_h && units_mem_d)) return;
  for(int li=0; li < n_layers_built; li++) {
    int st_ui = LayUnStart(lay_unit_idxs_h, li);
    int ed_ui = LayUnEnd(lay_unit_idxs_h, li);
    int nu = ed_ui - st_ui;

    UnitState_cuda* u = (UnitState_cuda*)Network_cuda::GetUnitState
      (units_mem_h, unit_state_size, st_ui);
    
    if(!((u->ext_flag & UnitState_cuda::EXT) || (u->ext_flag & UnitState_cuda::TARG)))
      continue;
    
    cudaSafeCall
      (cudaMemcpyAsync(units_mem_d + st_ui * unit_state_size,
                       units_mem_h + st_ui * unit_state_size,
                       nu * unit_state_size,
                       cudaMemcpyHostToDevice, strm_memcpy_units));
  }
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

void Network_cuda::TargUnitsToHost(bool sync) {
  if(!(units_mem_h && units_mem_d)) return;
  for(int li=0; li < n_layers_built; li++) {
    int st_ui = LayUnStart(lay_unit_idxs_h, li);
    int ed_ui = LayUnEnd(lay_unit_idxs_h, li);
    int nu = ed_ui - st_ui;

    UnitState_cuda* u = (UnitState_cuda*)Network_cuda::GetUnitState
      (units_mem_h, unit_state_size, st_ui);
    
    if(!(u->ext_flag & UnitState_cuda::TARG))
      continue;
    
    cudaSafeCall
      (cudaMemcpyAsync(units_mem_h + st_ui * unit_state_size,
                       units_mem_d + st_ui * unit_state_size,
                       nu * unit_state_size,
                       cudaMemcpyDeviceToHost, strm_memcpy_units));
  }
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

bool Network_cuda::AllocUnitSpecs(int n_us) {
  if(unit_spec_mem_h)
    free(unit_spec_mem_h);
  if(unit_spec_mem_d)
    cudaFree(unit_spec_mem_d);

  n_unit_specs = n_us;
  if(n_us == 0)                 // can't really happen..
    return false;

  unit_spec_mem_tot = n_unit_specs * unit_spec_size;
  if(unit_spec_mem_tot > max_constant_mem)
    return false;
  
  unit_spec_mem_h = (char*)malloc(unit_spec_mem_tot);
  cudaSafeCall(cudaMalloc(&unit_spec_mem_d, unit_spec_mem_tot));
  return true;
}

void Network_cuda::UnitSpecs_HostToDevice() {
  if(!(unit_spec_mem_h && unit_spec_mem_d)) return;
  
  cudaSafeCall
    (cudaMemcpy(unit_spec_mem_d, unit_spec_mem_h, unit_spec_mem_tot,
                cudaMemcpyHostToDevice));
}

bool Network_cuda::AllocConSpecs(int n_us) {
  if(con_spec_mem_h)
    free(con_spec_mem_h);
  if(con_spec_mem_d)
    cudaFree(con_spec_mem_d);

  n_con_specs = n_us;
  if(n_us == 0)                 // can't really happen..
    return false;

  con_spec_mem_tot = n_con_specs * con_spec_size;
  if(con_spec_mem_tot > max_constant_mem)
    return false;
  
  con_spec_mem_h = (char*)malloc(con_spec_mem_tot);
  cudaSafeCall(cudaMalloc(&con_spec_mem_d, con_spec_mem_tot));

  // host-side updated the con_spec_idx settings in all the congroups -- need to
  // re-copy that over again
  cudaSafeCall
    (cudaMemcpy(recv_cgp_mem_d, recv_cgp_mem_h, n_recv_cgps * con_state_size,
                          cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(send_cgp_mem_d, send_cgp_mem_h, n_send_cgps * con_state_size,
                          cudaMemcpyHostToDevice));
  return true;
}

void Network_cuda::ConSpecs_HostToDevice() {
  if(!(con_spec_mem_h && con_spec_mem_d)) return;
  
  cudaSafeCall
    (cudaMemcpy(con_spec_mem_d, con_spec_mem_h, con_spec_mem_tot,
                cudaMemcpyHostToDevice));
}


