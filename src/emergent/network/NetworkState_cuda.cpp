// Copyright 2017, Regents of the University of Colorado,
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

#include "NetworkState_cuda.h"

#include <LayerState_cuda>
#include <UnGpState_cuda>
#include <UnitState_cuda>

#include <LayerSpec_cuda>
#include <UnitSpec_cuda>
#include <ProjectionSpec_cuda>

// yikes -- need them all here, for the NewPrjnSpec method
#include <AllProjectionSpecs_cuda>

// common impl code from NetworkState_core

#include <State_cuda>

#include "NetworkState_core.cpp"


////////////////////////////////////////////////////////////
//              Regular

void NetworkState_cuda::StateError
(const char* a, const char* b, const char* c, const char* d, const char* e, const char* f,
 const char* g, const char* h, const char* i) const {
}

void NetworkState_cuda::StateErrorVals
  (const char* msg, const char* var1, float val1, const char* var2, float val2,
   const char* var3, float val3, const char* var4, float val4) const {
}

void NetworkState_cuda::ThreadSyncSpin(int thr_no, int sync_no) {
}

void NetworkState_cuda::Init_InputData() {
  Init_InputData_Layers();
  NET_THREAD_CALL(NetworkState_cuda::Init_InputData_Thr);
}

void NetworkState_cuda::Init_Acts() {
  NET_THREAD_CALL(NetworkState_cuda::Init_Acts_Thr);
}

void NetworkState_cuda::Init_dWt(){
  NET_THREAD_CALL(NetworkState_cuda::Init_dWt_Thr);
}

void NetworkState_cuda::Init_Weights() {
  needs_wt_sym = false;          // will get set to true if needed

  if(HasNetFlag(INIT_WTS_1_THREAD)) {
    Init_Weights_1Thr();
    Init_Weights_renorm();
    if(needs_wt_sym) {
      Init_Weights_sym();
    }
  }
  else {
    NET_THREAD_CALL(NetworkState_cuda::Init_Weights_Thr);
    Init_Weights_renorm();
    if(needs_wt_sym) {
      Init_Weights_sym();
    }
  }
  
  Init_Weights_post();
  Init_Weights_Layers();

  Init_Acts();                  // also re-init state at this point..
  Init_Metrics();
}

void NetworkState_cuda::Init_Weights_renorm() {
  NET_THREAD_CALL(NetworkState_cuda::Init_Weights_renorm_Thr);
}

void NetworkState_cuda::Init_Weights_sym() {
  NET_THREAD_CALL(NetworkState_cuda::Init_Weights_sym_Thr);
}

void NetworkState_cuda::Init_Weights_post() {
  NET_THREAD_CALL(NetworkState_cuda::Init_Weights_post_Thr);
}

void NetworkState_cuda::Init_Metrics() {
  Init_Counters();
  Init_Stats();                 // in NetworkState_core.cuda -- calls layer stats too
  // Init_Timers();
}

// void NetworkState_cuda::Init_Counters() {
//   Init_Counters_impl();         // does all the work
//   Init_Counters_State();
// }
  
void NetworkState_cuda::Init_Timers() {
  // todo: figure out timers at some point
  // train_time.ResetUsed();
  // epoch_time.ResetUsed();
  // trial_time.ResetUsed();
  // settle_time.ResetUsed();
  // cycle_time.ResetUsed();
  // wt_sync_time.ResetUsed();
  // misc_time.ResetUsed();
}

void NetworkState_cuda::Compute_Netin() {
  NET_THREAD_CALL(NetworkState_cuda::Compute_Netin_Thr);
}

void NetworkState_cuda::Send_Netin() {
  NET_THREAD_CALL(NetworkState_cuda::Send_Netin_Thr);

  Send_Netin_Integ();           // integrate from temp buff
}

void NetworkState_cuda::Compute_Act() {
  NET_THREAD_CALL(NetworkState_cuda::Compute_Act_Thr);
}

void NetworkState_cuda::Compute_NetinAct() {
  NET_THREAD_CALL(NetworkState_cuda::Compute_NetinAct_Thr);
}

void NetworkState_cuda::Compute_dWt() {
  NET_THREAD_CALL(NetworkState_cuda::Compute_dWt_Thr);
}

void NetworkState_cuda::Compute_Weights() {
  NET_THREAD_CALL(NetworkState_cuda::Compute_Weights_Thr);
}


#ifdef DMEM_COMPILE

void NetworkState_cuda::DMem_SumDWts_ToTmp() {
  NET_THREAD_CALL(NetworkState_cuda::DMem_SumDWts_ToTmp_Thr);
}  
 
void NetworkState_cuda::DMem_SumDWts_FmTmp() {
  NET_THREAD_CALL(NetworkState_cuda::DMem_SumDWts_FmTmp_Thr);
}  

#endif

void NetworkState_cuda::Compute_SSE(bool unit_avg, bool sqrt) {
  NET_THREAD_CALL(NetworkState_cuda::Compute_SSE_Thr);
  Compute_SSE_Agg(unit_avg, sqrt);
}

void NetworkState_cuda::Compute_PRerr() {
  NET_THREAD_CALL(NetworkState_cuda::Compute_PRerr_Thr);
  Compute_PRerr_Agg();
}

void NetworkState_cuda::Compute_TrialStats() {
  Compute_SSE(stats.sse_unit_avg, stats.sse_sqrt);
  if(stats.prerr)
    Compute_PRerr();
}

void NetworkState_cuda::Compute_EpochStats() {
  Compute_EpochSSE();
  if(stats.prerr)
    Compute_EpochPRerr();
  Compute_EpochStats_Layers();
}

//////////////////////////////////////////////////////////////////////////
//              Building

bool NetworkState_cuda::NetStateMalloc(void** ptr, size_t sz) const {
  cudaSafeCall(cudaMalloc(ptr, sz));
  return true;
}
  
bool NetworkState_cuda::NetStateFree(void** ptr) const {
  if(ptr && *ptr) {
    cudaSafeCall(cudaFree(*ptr));
    *ptr = NULL;
    return true;
  }
  return false;
}

__global__ void Kernel_InitUnitState(NetworkState_cuda* net) {
  net->InitUnitState_Thr(0);
}

bool NetworkState_cuda::CreateCudaStreams() {
  if(!strms_created) {
    cudaStreamCreate(&strm_build);
    cudaStreamCreate(&strm_memcpy_cons);
    cudaStreamCreate(&strm_memcpy_units);
    cudaStreamCreate(&strm_compute_netin);
    cudaStreamCreate(&strm_compute_act);
    cudaStreamCreate(&strm_compute_dwt);
    cudaStreamCreate(&strm_compute_dwt_bias);
    cudaStreamCreate(&strm_compute_weights);
    cudaStreamCreate(&strm_compute_weights_bias);
    strms_created = true;
    return true;
  }
  return false;
}

void NetworkState_cuda::LayUnitIndexes_CppToDevice(NetworkState_cpp* netc) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpy(ungp_lay_idxs, netc->ungp_lay_idxs, n_ungps_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(lay_send_pjrns, netc->lay_send_prjns, n_prjns_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(units_lays, netc->units_lays, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(units_ungps, netc->units_ungps, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(units_thrs, netc->units_thrs, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(units_thr_un_idxs, netc->units_thr_un_idxs, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(thrs_n_units, netc->thrs_n_units, n_thrs_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(thrs_unit_idxs[0], netc->thrs_unit_idxs[0], max_thr_n_units * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(thrs_lay_unit_idxs[0], netc->thrs_lay_unit_idxs[0], 2 * n_layers_built * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(thrs_ungp_unit_idxs[0], netc->thrs_ungp_unit_idxs[0], 2 * n_ungps_built * sizeof(int),
                cudaMemcpyHostToDevice));
  // todo: this one needs to be broken out:
  cudaSafeCall
    (cudaMemcpy(thrs_lay_stats[0], netc->thrs_lay_stats[0], n_lay_stats * n_layers_built * n_lay_stats_vars * sizeof(float),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(thrs_units_n_recv_cgps[0], netc->thrs_units_n_recv_cgps[0], max_thr_n_units * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(thrs_units_n_send_cgps[0], netc->thrs_units_n_send_cpgs[0], max_thr_n_units * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(thrs_recv_cgp_start[0], netc->thrs_recv_cgp_start[0], max_thr_n_units * sizeof(int),
                cudaMemcpyHostToDevice));
  cudaSafeCall
    (cudaMemcpy(thrs_send_cgp_start[0], netc->thrs_send_cgp_start[0], max_thr_n_units * sizeof(int),
                cudaMemcpyHostToDevice));
  // cudaSafeCall
  //   (cudaMemcpy(, netc->, n__built * sizeof(int),
  //               cudaMemcpyHostToDevice));
}

void NetworkState_cuda::LayerState_CppToDevice(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(thrs_units_mem[0], netc->thrs_units_mem[0],
                     n_units_built * unit_state_size,
                     cudaMemcpyHostToDevice, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

void NetworkState_cuda::LayerState_DeviceToCpp(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(netc->thrs_units_mem[0], thrs_units_mem[0],
                     n_units_built * unit_state_size,
                     cudaMemcpyDeviceToHost, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}


void NetworkState_cuda::PrjnState_CppToDevice(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(thrs_units_mem[0], netc->thrs_units_mem[0],
                     n_units_built * unit_state_size,
                     cudaMemcpyHostToDevice, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

void NetworkState_cuda::PrjnState_DeviceToCpp(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(netc->thrs_units_mem[0], thrs_units_mem[0],
                     n_units_built * unit_state_size,
                     cudaMemcpyDeviceToHost, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}


void NetworkState_cuda::UnGpState_CppToDevice(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(thrs_units_mem[0], netc->thrs_units_mem[0],
                     n_units_built * unit_state_size,
                     cudaMemcpyHostToDevice, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

void NetworkState_cuda::UnGpState_DeviceToCpp(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(netc->thrs_units_mem[0], thrs_units_mem[0],
                     n_units_built * unit_state_size,
                     cudaMemcpyDeviceToHost, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}


void NetworkState_cuda::UnitState_CppToDevice(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(thrs_units_mem[0], netc->thrs_units_mem[0],
                     n_units_built * unit_state_size,
                     cudaMemcpyHostToDevice, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

void NetworkState_cuda::UnitState_DeviceToCpp(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(netc->thrs_units_mem[0], thrs_units_mem[0],
                     n_units_built * unit_state_size,
                     cudaMemcpyDeviceToHost, strm_memcpy_units));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}


void NetworkState_cuda::BuildUnitState() {
  // AllocLayerUnitMem has been called, and all the layer, prjn, ungp state initialized by this point

  CreateCudaStreams();
  
  AllocNetTimers();
  
  // this assigns units to threads, also counts up number of con states
  InitUnitIdxs();               // in _core

  // just using a single thread just to get this running on the device..?
  Kernel_InitUnitState<<<1, 1, 0, strm_build>>>(this);

  LayoutUnits(); // _core
}

void NetworkState_cuda::BuildConState() {
  AllocConStateMem();           // _core
  
  NET_THREAD_CALL(NetworkState_cuda::InitConState_Thr); // _core
}

void NetworkState_cuda::BuildSendNetinTmp() {
  AllocSendNetinTmpState();     // _core
  
  NET_THREAD_CALL(NetworkState_cuda::InitSendNetinTmp_Thr); // _core
}


void NetworkState_cuda::Connect() {
  // RemoveCons(); // -- has been called at Network level.

  Connect_Sizes();
  Connect_Alloc();
  NET_THREAD_CALL(NetworkState_cuda::Connect_CacheMemStart_Thr);
  Connect_Cons(1);              // first pass

  if(needs_prjn_pass2) {
    NET_THREAD_CALL(NetworkState_cuda::Connect_CacheUnitLoHiIdxs_Thr);
    Connect_Cons(2);              // second pass (symmetry)
  }

  NET_THREAD_CALL(NetworkState_cuda::Connect_CacheUnitLoHiIdxs_Thr);
  NET_THREAD_CALL(NetworkState_cuda::Connect_VecChunk_Thr);
  NET_THREAD_CALL(NetworkState_cuda::Connect_UpdtActives_Thr);

  CountCons();
}

void NetworkState_cuda::Connect_Alloc() {
  AllocConsCountStateMem();
  
  NET_THREAD_CALL(NetworkState_cuda::Connect_AllocSizes_Thr);

  AllocConnectionMem();
  
  NET_THREAD_CALL(NetworkState_cuda::Connect_Alloc_Thr); // allocate to con groups
}

void NetworkState_cuda::UnBuildState() {
  // threads.RemoveAll();
  FreeStateMem();
  n_units = 0;
}

