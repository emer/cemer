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
  Init_Timers();
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
//              Cuda Init


NetworkState_cuda* NetworkState_cuda::NewCudaState() {
  return new NetworkState_cuda;
}

__global__ void Kernel_NewCudaState(void** dev_st_pt) {
  *dev_st_pt = NewCudaState();
}
  
void NetworkState_cuda::CudaInit() {
  CreateCudaStreams();
  strms_created = true;         // do this outside of above fun so it can call inherited
  NetStateMalloc(&dev_state_ptr, sizeof(void*));
  Kernel_NewCudaState<<<1, 1, 0, strm_memcpy_idxs>>>((void**)dev_state_ptr);
  dev_state = (NetworkState_cuda*)*dev_state_ptr; // get the new pointer -- in dev mem

  // todo: at some point need to copy everything to dev_state, except its virtual table!
  // get address of first item and last item??
}

bool NetworkState_cuda::CreateCudaStreams() {
  if(!strms_created) {
    cudaStreamCreate(&strm_memcpy_idxs);
    cudaStreamCreate(&strm_memcpy_cons);
    cudaStreamCreate(&strm_memcpy_units);
    cudaStreamCreate(&strm_memcpy_layers);
    cudaStreamCreate(&strm_memcpy_prjns);
    cudaStreamCreate(&strm_memcpy_net);
    cudaStreamCreate(&strm_compute_netin);
    cudaStreamCreate(&strm_compute_act);
    cudaStreamCreate(&strm_compute_dwt);
    cudaStreamCreate(&strm_compute_dwt_bias);
    cudaStreamCreate(&strm_compute_weights);
    cudaStreamCreate(&strm_compute_weights_bias);
    return true;
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////
//              Building

// these are for allocating on device!

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

void NetworkState_cuda::NetIndexes_CppToDevice(NetworkState_cpp* netc) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(ungp_lay_idxs, netc->ungp_lay_idxs, n_ungps_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(lay_send_pjrns, netc->lay_send_prjns, n_prjns_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(units_lays, netc->units_lays, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(units_ungps, netc->units_ungps, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(units_thrs, netc->units_thrs, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(units_thr_un_idxs, netc->units_thr_un_idxs, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(thrs_n_units, netc->thrs_n_units, n_thrs_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));

  cudaSafeCall
    (cudaMemcpyAsync(units_n_recv_cgps, netc->units_n_recv_cgps, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(units_n_send_cgps, netc->units_n_send_cgps, n_units_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));

  cudaSafeCall
    (cudaMemcpyAsync(thrs_n_recv_cgps, netc->thrs_n_recv_cgps, n_thrs_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(thrs_n_send_cgps, netc->thrs_n_send_cgps, n_thrs_built * sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(n_recv_cgps, netc->n_recv_cgps, sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(n_send_cgps, netc->n_send_cgps, sizeof(int),
                cudaMemcpyHostToDevice, strm_memcpy_idxs));
  
  for(int thr_no=0; thr_no < n_thrs_built; thr_no++) {
    cudaSafeCall
      (cudaMemcpyAsync(thrs_unit_idxs[thr_no], netc->thrs_unit_idxs[thr_no],
                  max_thr_n_units * sizeof(int), cudaMemcpyHostToDevice, strm_memcpy_idxs));
    cudaSafeCall
      (cudaMemcpyAsync(thrs_lay_unit_idxs[thr_no], netc->thrs_lay_unit_idxs[thr_no],
                  2 * n_layers_built * sizeof(int), cudaMemcpyHostToDevice, strm_memcpy_idxs));
    cudaSafeCall
      (cudaMemcpyAsync(thrs_ungp_unit_idxs[thr_no], netc->thrs_ungp_unit_idxs[thr_no],
                  2 * n_ungps_built * sizeof(int), cudaMemcpyHostToDevice, strm_memcpy_idxs));
    cudaSafeCall
      (cudaMemcpyAsync(thrs_units_n_recv_cgps[thr_no], netc->thrs_units_n_recv_cgps[thr_no],
                  max_thr_n_units * sizeof(int), cudaMemcpyHostToDevice, strm_memcpy_idxs));
    cudaSafeCall
      (cudaMemcpyAsync(thrs_units_n_send_cgps[thr_no], netc->thrs_units_n_send_cpgs[thr_no],
                  max_thr_n_units * sizeof(int), cudaMemcpyHostToDevice, strm_memcpy_idxs));
    cudaSafeCall
      (cudaMemcpyAsync(thrs_recv_cgp_start[thr_no], netc->thrs_recv_cgp_start[thr_no],
                  max_thr_n_units * sizeof(int), cudaMemcpyHostToDevice, strm_memcpy_idxs));
    cudaSafeCall
      (cudaMemcpyAsync(thrs_send_cgp_start[thr_no], netc->thrs_send_cgp_start[thr_no],
                  max_thr_n_units * sizeof(int), cudaMemcpyHostToDevice, strm_memcpy_idxs));
  }

  cudaSafeCall
    (cudaMemcpyAsync(thrs_recv_cons_cnt, netc->thrs_recv_cons_cnt,
                     n_thrs_built * sizeof(int64_t),
                     cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(thrs_send_cons_cnt, netc->thrs_send_cons_cnt,
                     n_thrs_built * sizeof(int64_t),
                     cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(thrs_own_cons_max_size, netc->thrs_own_cons_max_size,
                     n_thrs_built * sizeof(int),
                     cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(thrs_own_cons_tot_size, netc->thrs_own_cons_tot_size,
                n_thrs_built * sizeof(int64_t), cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(thrs_own_cons_tot_size_nonshared, netc->thrs_own_cons_tot_size_nonshared,
                n_thrs_built * sizeof(int64_t), cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(thrs_own_cons_avg_size, netc->thrs_own_cons_avg_size,
                     n_thrs_built * sizeof(int),
                     cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(thrs_own_cons_max_vars, netc->thrs_own_cons_max_vars,
                     n_thrs_built * sizeof(int),
                     cudaMemcpyHostToDevice, strm_memcpy_idxs));
  cudaSafeCall
    (cudaMemcpyAsync(thrs_pct_cons_vec_chunked, netc->thrs_pct_cons_vec_chunked,
                n_thrs_built * sizeof(float), cudaMemcpyHostToDevice, strm_memcpy_idxs));

  cudaSafeCall(cudaStreamSynchronize(strm_memcpy_idxs)); // final sync
}

  // may need this later:
  // cudaSafeCall
  //   (cudaMemcpyAsync(thrs_lay_stats[0], netc->thrs_lay_stats[0], n_lay_stats * n_layers_built * n_lay_stats_vars * sizeof(float),
  //               cudaMemcpyHostToDevice, strm_memcpy_idxs));

void NetworkState_cuda::LayerState_CppToDevice(NetworkState_cpp* netc, bool sync) {
  if(!(layers_mem && netc->layers_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(layers_mem, netc->layers_mem,
                     n_layers_built * layer_state_size,
                     cudaMemcpyHostToDevice, strm_memcpy_layers));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_layers));
  }
}

void NetworkState_cuda::LayerState_DeviceToCpp(NetworkState_cpp* netc, bool sync) {
  if(!(layers_mem && netc->layers_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(netc->layers_mem, layers_mem,
                     n_layers_built * layer_state_size,
                     cudaMemcpyDeviceToHost, strm_memcpy_layers));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_layers));
  }
}


void NetworkState_cuda::PrjnState_CppToDevice(NetworkState_cpp* netc, bool sync) {
  if(!(prjns_mem && netc->prjns_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(prjns_mem, netc->prjns_mem,
                     n_prjns_built * prjn_state_size,
                     cudaMemcpyHostToDevice, strm_memcpy_prjns));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_prjns));
  }
}

void NetworkState_cuda::PrjnState_DeviceToCpp(NetworkState_cpp* netc, bool sync) {
  if(!(prjns_mem && netc->prjns_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(netc->prjns_mem, prjns_mem,
                     n_prjns_built * prjn_state_size,
                     cudaMemcpyDeviceToHost, strm_memcpy_prjns));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_prjns));
  }
}


void NetworkState_cuda::UnGpState_CppToDevice(NetworkState_cpp* netc, bool sync) {
  if(!(ungps_mem && netc->ungps_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(ungps_mem, netc->ungps_mem,
                     n_ungps_built * ungp_state_size,
                     cudaMemcpyHostToDevice, strm_memcpy_ungps));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_ungps));
  }
}

void NetworkState_cuda::UnGpState_DeviceToCpp(NetworkState_cpp* netc, bool sync) {
  if(!(ungps_mem && netc->ungps_mem)) return;
  cudaSafeCall
    (cudaMemcpyAsync(netc->ungps_mem, ungps_mem,
                     n_ungps_built * ungp_state_size,
                     cudaMemcpyDeviceToHost, strm_memcpy_ungps));
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_ungps));
  }
}


void NetworkState_cuda::UnitState_CppToDevice(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  for(int thr_no=0; thr_no < n_thrs_built; thr_no++) {
    cudaSafeCall
      (cudaMemcpyAsync(thrs_units_mem[thr_no], netc->thrs_units_mem[thr_no],
                       n_units_built * unit_state_size,
                       cudaMemcpyHostToDevice, strm_memcpy_units));
  }
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

void NetworkState_cuda::UnitState_DeviceToCpp(NetworkState_cpp* netc, bool sync) {
  if(!(thrs_units_mem && netc->thrs_units_mem)) return;
  for(int thr_no=0; thr_no < n_thrs_built; thr_no++) {
    cudaSafeCall
      (cudaMemcpyAsync(netc->thrs_units_mem[thr_no], thrs_units_mem[thr_no],
                       n_units_built * unit_state_size,
                       cudaMemcpyDeviceToHost, strm_memcpy_units));
  }
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_units));
  }
}

__global__ void Kernel_CacheMemStart(NetworkState_cuda* net) {
  const int nthrs = blockDim.x;
  const int cuth = threadIdx.x;
  const int cg_idx = blockIdx.x * nthrs + cuth;

  for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
    const int nrcg = net->ThrNRecvConGps(thr_no);
    if(cg_idx < nrcg) {
      CON_STATE* rcg = ThrRecvConState(thr_no, cg_idx);
      rcg->CacheMemStart(net, thr_no);
    }
    const int nscg = net->ThrNSendConGps(thr_no);
    if(cg_idx < nscg) {
      CON_STATE* scg = ThrSendConState(thr_no, cg_idx);
      scg->CacheMemStart(net, thr_no);
    }
  }
}

void NetworkState_cuda::ConState_CppToDevice(NetworkState_cpp* netc) {
  int max_cgps = 0;
  for(int thr_no=0; thr_no < n_thrs_built; thr_no++) {
    cudaSafeCall
      (cudaMemcpyAsync(thrs_recv_cgp_mem[thr_no], netc->thrs_recv_cgp_mem[thr_no],
                       thrs_n_recv_cgps[thr_no] * con_state_size,
                       cudaMemcpyHostToDevice, strm_memcpy_cons));
    cudaSafeCall
      (cudaMemcpyAsync(thrs_send_cgp_mem[thr_no], netc->thrs_send_cgp_mem[thr_no],
                       thrs_n_send_cgps[thr_no] * con_state_size,
                       cudaMemcpyHostToDevice, strm_memcpy_cons));
    const int nrcg = ThrNRecvConGps(thr_no);
    const int nscg = ThrNSendConGps(thr_no);
    max_cgps = MAX(nrcg, max_cgps);
    max_cgps = MAX(nscg, max_cgps);
  }
  cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));

  // need to update cached memory pointers on device
  const int n_blocks = (int)ceil((float)max_cgps / (float)cuda.n_threads);
  Kernel_CacheMemStart<<<n_blocks, cuda.n_threads, 0, strm_memcpy_cons>>>(dev_state);
}


void NetworkState_cuda::ConMem_CppToDevice(NetworkState_cpp* netc, bool sync) {
  for(int thr_no=0; thr_no < n_thrs_built; thr_no++) {
    if(thrs_recv_cons_cnt[thr_no] > 0) {
      cudaSafeCall
        (cudaMemcpyAsync(thrs_recv_cons_mem[thr_no], netc->thrs_recv_cons_mem[thr_no],
                         thrs_recv_cons_cnt[thr_no] * sizeof(float),
                         cudaMemcpyHostToDevice, strm_memcpy_cons));
    }
    if(thrs_send_cons_cnt[thr_no] > 0) {
      cudaSafeCall
        (cudaMemcpyAsync(thrs_send_cons_mem[thr_no], netc->thrs_send_cons_mem[thr_no],
                         thrs_send_cons_cnt[thr_no] * sizeof(float),
                         cudaMemcpyHostToDevice, strm_memcpy_cons));
    }
  }
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));
  }
}

void NetworkState_cuda::ConMem_CppToDevice(NetworkState_cpp* netc, bool sync) {
  for(int thr_no=0; thr_no < n_thrs_built; thr_no++) {
    if(thrs_recv_cons_cnt[thr_no] > 0) {
      cudaSafeCall
        (cudaMemcpyAsync(thrs_recv_cons_mem[thr_no], netc->thrs_recv_cons_mem[thr_no],
                         thrs_recv_cons_cnt[thr_no] * sizeof(float),
                         cudaMemcpyHostToDevice, strm_memcpy_cons));
    }
    if(thrs_send_cons_cnt[thr_no] > 0) {
      cudaSafeCall
        (cudaMemcpyAsync(thrs_send_cons_mem[thr_no], netc->thrs_send_cons_mem[thr_no],
                         thrs_send_cons_cnt[thr_no] * sizeof(float),
                         cudaMemcpyHostToDevice, strm_memcpy_cons));
    }
  }
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));
  }
}

void NetworkState_cuda::ConMem_DeviceToCpp(NetworkState_cpp* netc, bool sync) {
  for(int thr_no=0; thr_no < n_thrs_built; thr_no++) {
    if(thrs_recv_cons_cnt[thr_no] > 0) {
      cudaSafeCall
        (cudaMemcpyAsync(netc->thrs_recv_cons_mem[thr_no], thrs_recv_cons_mem[thr_no],
                         thrs_recv_cons_cnt[thr_no] * sizeof(float),
                         cudaMemcpyDeviceToHost, strm_memcpy_cons));
    }
    if(thrs_send_cons_cnt[thr_no] > 0) {
      cudaSafeCall
        (cudaMemcpyAsync(netc->thrs_send_cons_mem[thr_no], thrs_send_cons_mem[thr_no],
                         thrs_send_cons_cnt[thr_no] * sizeof(float),
                         cudaMemcpyDeviceToHost, strm_memcpy_cons));
    }
  }
  if(sync) {
    cudaSafeCall(cudaStreamSynchronize(strm_memcpy_cons));
  }
}

void NetworkState_cuda::BuildCudaFmCpp(NetworkState_cpp* netc) {
  // one-stop 
  
  // AllocLayerUnitMem has been called, and all the layer, prjn, ungp state initialized by this point

  CreateCudaStreams();
  
  AllocNetTimers();
  
  // this assigns units to threads, also counts up number of con states
  InitUnitIdxs();               // in _core

  LayoutUnits(); // _core
}

void NetworkState_cuda::BuildConState() {
  AllocConStateMem();           // _core
  
  // NET_THREAD_CALL(NetworkState_cuda::InitConState_Thr); // _core
}

void NetworkState_cuda::BuildSendNetinTmp() {
  AllocSendNetinTmpState();     // _core
  
  // NET_THREAD_CALL(NetworkState_cuda::InitSendNetinTmp_Thr); // _core
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

