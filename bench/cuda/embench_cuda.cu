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

/*
Description of Algorithm

This is a receiver-based algorithm.
Each thread computes partial net inputs for a single Unit. 
Each such partial computation is called a "chunk" -- there
are 32 weights per chunk; therefore, each projection is
represented in atoms of chunks; any unused are set to 0.

The Warp size is used to make sure that different units
are processed -- the allocation algorithm insures that 
each warp used a different net unit, so the write updates
do not need to be syncronized. 
*/

// Types

typedef struct con {
  int	snd_idx;
  float	wt;
} con_t;


typedef con_t con_chunk_t[CON_CHUNK_SZ];

// Global data structures

__device__
uint	n_units;
//__device__
//int	n_con_blks;
//__device__
uint	n_blks; // number of blocks needed

__constant__
float		acts[MAX_UNITS];
__device__
float		net[MAX_UNITS];

__device__
int*		blk_map[N_BLOCKS][N_THREADS];
__device__
con_chunk_t*	con_blks[N_BLOCKS][N_THREADS];


__global__ 
void kRecv_Netin() {
  // block/thread indexes, for clarity
  int blk = blockIdx.y;
  int bx = blockIdx.x;
  int tx = threadIdx.x;
//TODO: do i need to copy the chunks to local memory???  
//  int un_idx = blk_map[blk][bx][tx];
//TEMP: actually use a real idx
int un_idx = bx*N_THREADS+tx;
  con_chunk_t& con_chunk = con_blks[blk][bx][tx];
  float tnet = 0.0f;
  for (int ci = 0; ci < CON_CHUNK_SZ; ci++) {
    tnet += acts[con_chunk[ci].snd_idx] * con_chunk[ci].wt; 
  }
//  net[un_idx] += tnet; 
//TEMP
net[un_idx] += __int_as_float(tx);
}

extern "C" {

int cuAllocUnits(uint n_units_, uint n_con_chunks_) {
  if ((n_units_ == 0) || (n_con_chunks_ == 0))
    return 1;
   // round up the number of chunks needed to an even processable number
  n_con_chunks_ = (n_con_chunks_ + ((N_BLOCKS*N_THREADS) - 1)) & ~(N_BLOCKS*N_THREADS);
  if ((n_units_ > MAX_UNITS) || (n_con_chunks_ > MAX_CON_CHUNKS))
    return 2;
    
  n_units = n_units_;
  n_blks = n_con_chunks_ / (N_BLOCKS*N_THREADS); // s/b min 1
//printf("cuAllocUnits: n_blks=%d\n",n_blks); 
  uint mem_size = sizeof(con_blks) * n_blks;
  CUDA_SAFE_CALL(cudaMalloc((void**) &con_blks, mem_size));
  CUDA_SAFE_CALL(cudaMemset(con_blks, 0, mem_size));
  
  mem_size = sizeof(blk_map) * n_blks;
  CUDA_SAFE_CALL(cudaMalloc((void**) &blk_map, mem_size));
  CUDA_SAFE_CALL(cudaMemset(blk_map, 0, mem_size));
    
  // init acts to 0 
  void* mem = calloc(n_units_, sizeof(float));
  CUDA_SAFE_CALL(cudaMemcpyToSymbol("acts", mem, n_units_ * sizeof(float)));
  free(mem);
  
  //TODO: error?
  return 0;
}

int cuCpHD_Acts(float* acts) {
  CUDA_SAFE_CALL(cudaMemcpyToSymbol("acts", acts, n_units * sizeof(float)));
  return 0;
}

int cuCpHD_Cons(uint un_idx, uint n_cons, cbGetCon GetCon) {
    return 0;
}

int cuCpDH_Nets(float* nets) {
  CUDA_SAFE_CALL(cudaMemcpy((void*)nets, &net,
    n_units * sizeof(float), cudaMemcpyDeviceToHost));
  return 0;
}

void cuRecv_Netin()
{
//    CUT_DEVICE_INIT();

    // setup execution parameters
    dim3 grid(N_BLOCKS, n_blks, 1);
    dim3 threads(N_THREADS, 1, 1);
    
    // clear nets
    CUDA_SAFE_CALL(cudaMemset(&net, 0, n_units * sizeof(float)));
    
    // execute the kernel
    kRecv_Netin<<< grid, threads >>>();

    // check if kernel execution generated and error
    CUT_CHECK_ERROR("Kernel execution failed");
}


};
