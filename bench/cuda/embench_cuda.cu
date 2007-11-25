/* Example of integrating CUDA functions into an existing 
 * application / framework.
 * Host part of the device code.
 * Compiled with Cuda compiler.
 */

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "embench_cuda.h"

/* Common elements (send and recv)
*/

uint	h_n_units;
__constant__
float		c_acts[MAX_UNITS]; // only [h_n_units] init'ed/used
float*		d_nets; // [h_n_units];


extern "C" {

int cuCpHD_Acts(const float* acts) {
  CUDA_SAFE_CALL(cudaMemcpyToSymbol("c_acts", acts, h_n_units * sizeof(float)));
  return 0;
}

int cuCpDH_Nets(float* nets) {
  CUDA_SAFE_CALL(cudaMemcpy((void*)nets, (void*)d_nets,
    h_n_units * sizeof(float), cudaMemcpyDeviceToHost));
  return 0;
}

}; // extern "C"


/* Receiver-based

Description of Algorithm

This is a receiver-based algorithm.
Each thread computes partial d_nets inputs for a single Unit. 
Each such partial computation is called a "chunk" -- there
are 32 weights per chunk; therefore, each projection is
represented in atoms of chunks; any unused are set to 0:0.f
(i.e. Unit 0 is also a dummy.)

The Warp size (RCV_N_THREADS) is used to make sure that different
units are processed in parallel-- the allocation algorithm 
insures that each warp uses a different d_nets unit, so the write
updates do not need to be syncronized. 
*/

// Types

typedef struct con {
  int	snd_idx;
  float	wt;
} con_t;


typedef int blk_map_t[RCV_N_THREADS];
//typedef con_t con_blk_t[RCV_CON_CHUNK_SZ][RCV_N_THREADS];
typedef int con_si_blk_t[RCV_CON_CHUNK_SZ][RCV_N_THREADS];
typedef float con_wt_blk_t[RCV_CON_CHUNK_SZ][RCV_N_THREADS];

// Global data structures

//__device__
uint	n_con_chunks; // just for reference
//__device__
uint	n_blks; // number of [RCV_N_THREADS] blocks needed

blk_map_t*	d_blk_map; // [n_blks][RCV_N_THREADS]
//con_blk_t*	d_con_blks; // [n_blks][RCV_N_THREADS]
con_si_blk_t*	d_con_si_blks; // [n_blks][RCV_N_THREADS]
con_wt_blk_t*	d_con_wt_blks; // [n_blks][RCV_N_THREADS]

__global__ 
void kRecv_Netin(
  float* d_nets,
  blk_map_t* d_blk_map,
  con_si_blk_t* d_con_si_blks,
  con_wt_blk_t* d_con_wt_blks
) {
  // block/thread indexes, for clarity
  int blk = blockIdx.x;
  int tx = threadIdx.x;
  int un_idx = d_blk_map[blk][tx];
  float tnet = 0.0f;
  for (int ci = 0; ci < RCV_CON_CHUNK_SZ; ci++) {
/*    con_t& con = d_con_blks[blk][ci][tx];
    tnet += c_acts[con.snd_idx] * con.wt; */
    tnet += c_acts[d_con_si_blks[blk][ci][tx]] * d_con_wt_blks[blk][ci][tx]; 
  }
  d_nets[un_idx] += tnet; 
  __syncthreads();
}

extern "C" {

int cuRecv_AllocMem(uint n_units_, uint silo_sz_) {
  if ((n_units_ == 0) || (silo_sz_ == 0))
    return 1;
    
  // round up number of blks needed to even processable number
  
   // round up the number of chunks needed to an even processable number
  n_con_chunks = (silo_sz_ * RCV_N_THREADS); // for reference
//printf("cuAllocMem: n_con_chunks=%d\n",n_con_chunks); 
  if ((n_units_ > MAX_UNITS) || (silo_sz_ > RCV_MAX_SILO_SZ))
    return 2;
    
  h_n_units = n_units_;
  n_blks = silo_sz_; // s/b min 1
  
  CUT_DEVICE_INIT();
  
  uint mem_size;
  // init used acts to 0 
  mem_size = n_units_ * sizeof(float);
  void* mem = calloc(n_units_, sizeof(float));
  CUDA_SAFE_CALL(cudaMemcpyToSymbol("c_acts", mem, mem_size));
  free(mem);
  // alloc nets (no init, since overwritten anyway)
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_nets, mem_size));
  
  
//printf("cuAllocMem: n_blks=%d\n",n_blks); 
  // block map
  mem_size = n_blks * sizeof(blk_map_t);
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_blk_map, mem_size));
  CUDA_SAFE_CALL(cudaMemset(d_blk_map, 0, mem_size));
    
/*  mem_size = n_blks * sizeof(con_blk_t);
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_con_blks, mem_size));
  CUDA_SAFE_CALL(cudaMemset(d_con_blks, 0, mem_size));*/
  mem_size = n_blks * sizeof(con_si_blk_t);
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_con_si_blks, mem_size));
  CUDA_SAFE_CALL(cudaMemset(d_con_si_blks, 0, mem_size));
  mem_size = n_blks * sizeof(con_wt_blk_t);
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_con_wt_blks, mem_size));
  CUDA_SAFE_CALL(cudaMemset(d_con_wt_blks, 0, mem_size));
  
  //TODO: error?
  return 0;
}

int cuRecv_FreeMem() {
  CUDA_SAFE_CALL(cudaFree((void*) d_nets));
  d_nets = NULL;
  CUDA_SAFE_CALL(cudaFree((void*) d_blk_map));
  d_blk_map = NULL;
/*  CUDA_SAFE_CALL(cudaFree((void*) d_con_blks));
  d_con_blks = NULL;*/
  CUDA_SAFE_CALL(cudaFree((void*) d_con_si_blks));
  d_con_si_blks = NULL;
  CUDA_SAFE_CALL(cudaFree((void*) d_con_wt_blks));
  d_con_wt_blks = NULL;

  return 0;
}

int cuRecv_CpHD_Cons(cbGetCon GetCon) {
  int chunks_copied = 0; // sanity check
//  con_blk_t* con_buf = (con_blk_t*)calloc(n_blks, sizeof(con_blk_t));
  con_si_blk_t* con_si_buf = (con_si_blk_t*)calloc(n_blks, sizeof(con_si_blk_t));
  con_wt_blk_t* con_wt_buf = (con_wt_blk_t*)calloc(n_blks, sizeof(con_wt_blk_t));
// we work one silo at a time, each silo is a warp thread
  for (int silo = 0; silo < RCV_N_THREADS; silo++) {
    int un_idx = silo;
    int blk = 0; // block x
    while (un_idx < h_n_units) {
      int con_idx = 0;
      bool done = false;
      while (!done) {
        int chunk_idx = 0; // note: only advanced if val read, so ==0 means none read
        while (chunk_idx < RCV_CON_CHUNK_SZ) {
//          con_t& con = con_buf[blk][chunk_idx][silo];
          int* con_snd_idx = &(con_si_buf[blk][chunk_idx][silo]);
          float* con_wt = &(con_wt_buf[blk][chunk_idx][silo]);
          done = GetCon(un_idx, con_idx, con_snd_idx, con_wt);
          if (done) break;
          con_idx++;
          chunk_idx++;
        }
        // chunk_idx is now # of cons read
        if (chunk_idx > 0) { // write chunk and blk map entry
          void* ptr;
          int flat_chunk_idx = (blk * RCV_N_THREADS) + silo;
          // block map guy -- points to this unit
          ptr = (void*)((char*)d_blk_map + (flat_chunk_idx * sizeof(int)));
          CUDA_SAFE_CALL(cudaMemcpy(ptr, &un_idx,
            sizeof(int), cudaMemcpyHostToDevice));

          // bump block pointers/counters
          chunks_copied++;
          blk++;
        }
      }
      un_idx += RCV_N_THREADS;
    }
  }
/*  CUDA_SAFE_CALL(cudaMemcpy(d_con_blks, con_buf,
     sizeof(con_blk_t) * n_blks, cudaMemcpyHostToDevice));*/
  CUDA_SAFE_CALL(cudaMemcpy(d_con_si_blks, con_si_buf,
     sizeof(con_si_blk_t) * n_blks, cudaMemcpyHostToDevice));
  CUDA_SAFE_CALL(cudaMemcpy(d_con_wt_blks, con_wt_buf,
     sizeof(con_wt_blk_t) * n_blks, cudaMemcpyHostToDevice));
//printf("Copied %d chunks (%d expected)\n", chunks_copied, n_con_chunks);  
    
//  free(con_buf);
  free(con_si_buf);
  free(con_wt_buf);
  return 0;
}

void cuRecv_Netin()
{

    // setup execution parameters
    dim3 grid(n_blks, 1, 1);
    dim3 threads(RCV_N_THREADS, 1, 1);
    
    // clear nets
    CUDA_SAFE_CALL(cudaMemset(d_nets, 0, h_n_units * sizeof(float)));
    
    // execute the kernel
//    kRecv_Netin<<< grid, threads >>>(d_nets, d_blk_map, d_con_blks);
    kRecv_Netin<<< grid, threads >>>(
      d_nets, d_blk_map, d_con_si_blks, d_con_wt_blks);

    // check if kernel execution generated and error
    CUT_CHECK_ERROR("Kernel execution failed");
}


};
