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

uint	h_n_units; // this is the requested # units
uint	h_n_units_adj; // adjusted number of units (only used by us)
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
Each thread computes the d_net input for a single Unit. 

The Warp size (RCV_N_THREADS) is used to make sure that different
units are processed in parallel-- the allocation algorithm 
insures that each warp uses a different d_nets unit, so the write
updates do not need to be syncronized. 
*/

// Types

typedef int con_si_blk_t[RCV_N_THREADS];
typedef float con_wt_blk_t[RCV_N_THREADS];

// Global data structures

uint	n_cons_pu; 
uint	n_blks; // number of [RCV_N_THREADS] blocks needed

con_si_blk_t*	d_con_si_blks; // [n_blks][n_cons_pu][RCV_N_THREADS]
con_wt_blk_t*	d_con_wt_blks; // [n_blks][n_cons_pu][RCV_N_THREADS]

// can compute act in place, in the net var
__global__ 
void kComputeActs(
  const float* d_nets,
  float* d_acts
) {
  int un_idx = blockIdx.x * blockDim.x + threadIdx.x;
  float tact =  1.0f / (1.0f + expf(-d_acts[un_idx]));
  __syncthreads();
  d_acts[un_idx] = tact; 
}

__global__ 
void kRecv_Netin(
  int n_cons_pu,
  float* d_nets,
  con_si_blk_t* d_con_si_blks,
  con_wt_blk_t* d_con_wt_blks
) {
  // block/thread indexes, for clarity
  int blk = blockIdx.x;
  int tx = threadIdx.x;
  int blki = blk * n_cons_pu;
  int un_idx = blk * blockDim.x + tx;
  float tnet = 0.0f;
  for (int ci = 0; ci < n_cons_pu; ci++, blki++) {
    tnet += c_acts[d_con_si_blks[blki][tx]] * d_con_wt_blks[blki][tx]; 
  }
  d_nets[un_idx] = tnet; 
}

extern "C" {

int cuRecv_AllocMem(uint n_units_, uint n_cons_pu_) {
  if ((n_units_ == 0) || (n_cons_pu_ == 0))
    return 1;
    
  // round up number of units needed to even processable number
  h_n_units_adj = (n_units_ + (RCV_N_THREADS - 1)) & ~(RCV_N_THREADS - 1);
  if ((n_units_ > MAX_UNITS) || (n_cons_pu_ > RCV_MAX_CON_SZ))
    return 2;
    
  h_n_units = n_units_;
  n_cons_pu = n_cons_pu_;
  n_blks = h_n_units / RCV_N_THREADS; // s/b min 1
  
  CUT_DEVICE_INIT();
  
  uint mem_size;
  // init used acts to 0 
  mem_size = n_units_ * sizeof(float);
  void* mem = calloc(n_units_, sizeof(float));
  CUDA_SAFE_CALL(cudaMemcpyToSymbol("c_acts", mem, mem_size));
  free(mem);
  // alloc nets (no init, since overwritten anyway)
  // we allocate dummys if needed
  mem_size = h_n_units * sizeof(float);
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_nets, mem_size));
  
  // allocate cons, including for dummys (just have si=0,wt=0)
  mem_size = n_blks * n_cons_pu * sizeof(con_si_blk_t);
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_con_si_blks, mem_size));
  CUDA_SAFE_CALL(cudaMemset(d_con_si_blks, 0, mem_size));
  mem_size = n_blks * n_cons_pu * sizeof(con_wt_blk_t);
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_con_wt_blks, mem_size));
  CUDA_SAFE_CALL(cudaMemset(d_con_wt_blks, 0, mem_size));
  
  //TODO: error?
  return 0;
}

int cuRecv_FreeMem() {
  CUDA_SAFE_CALL(cudaFree((void*) d_con_wt_blks));
  d_con_wt_blks = NULL;
  CUDA_SAFE_CALL(cudaFree((void*) d_con_si_blks));
  d_con_si_blks = NULL;
  CUDA_SAFE_CALL(cudaFree((void*) d_nets));
  d_nets = NULL;

  return 0;
}

int cuRecv_CpHD_Cons(cbGetCon GetCon) {
  int n_con_blks = n_blks * n_cons_pu;
  con_si_blk_t* con_si_buf = (con_si_blk_t*)calloc(n_con_blks, sizeof(con_si_blk_t));
  con_wt_blk_t* con_wt_buf = (con_wt_blk_t*)calloc(n_con_blks, sizeof(con_wt_blk_t));

// we work one silo at a time, each silo is a warp thread
  for (int silo = 0; silo < RCV_N_THREADS; silo++) {
    int blk = 0; // block x
    // note: only iterate actual units, dummys are defaulted to 0,0
    for (int un_idx = silo; un_idx < h_n_units; un_idx += RCV_N_THREADS, blk++) {
      int blki = blk*n_cons_pu;
      for (int con_idx = 0; con_idx < n_cons_pu; con_idx++, blki++) {
        int* con_snd_idx = &(con_si_buf[blki][silo]);
        float* con_wt = &(con_wt_buf[blki][silo]);
        GetCon(un_idx, con_idx, con_snd_idx, con_wt);
      }
    }
  }
  CUDA_SAFE_CALL(cudaMemcpy(d_con_si_blks, con_si_buf,
     sizeof(con_si_blk_t) * n_con_blks, cudaMemcpyHostToDevice));
  CUDA_SAFE_CALL(cudaMemcpy(d_con_wt_blks, con_wt_buf,
     sizeof(con_wt_blk_t) * n_con_blks, cudaMemcpyHostToDevice));
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
    
    // execute the kernel
    kRecv_Netin<<< grid, threads >>>(
      n_cons_pu, d_nets, d_con_si_blks, d_con_wt_blks);

    // check if kernel execution generated and error
    CUT_CHECK_ERROR("Kernel execution failed");
}


void cuComputeActs(float* acts)
{
    // setup execution parameters
    dim3 grid(n_blks, 1, 1);
    dim3 threads(RCV_N_THREADS, 1, 1);
    
    // execute the kernel -- uses d_nets as the result (allowed)
    kComputeActs<<< grid, threads >>>(
      d_nets, d_nets);

  CUDA_SAFE_CALL(cudaMemcpy((void*)acts, (void*)d_nets,
    h_n_units * sizeof(float), cudaMemcpyDeviceToHost));
    
    // check if kernel execution generated and error
    CUT_CHECK_ERROR("Kernel execution failed");
}



};


/* Sender-based

Description of Algorithm

This is a sender-based algorithm -- it is significantly more
complex than the receiver-algorithm.

Computation is organized around layers, and then smaller
groups of units from within a layer called a unitgroup (this
is not the same as a UnitGroup in Emergent, but the concept
is similar.)

The sending connections of a unit are organized into a list,
by target unitgroup. Each fixed-size item of this list is
a conchunk.



Each block is dedicated to calculating nets for a set of
unitgroups. Shared memory is used to hold the net values for
that set.

Each block processes a single sending unit's
connections at one time -- since the unit will have a disjoint
set of targets, this insures that each thread will be writing
to a different shared memory net value, so there can be
no conflicts or need for synchronization.
 
The block will loop until all the units that send values to
any of its nets have been finished.

Note that there is not a full mapping from sending unit to 
unitgroup -- for full connections, there will be, but for
partial connection patterns, some ugs will only have some
sending conns from a unit -- in this case there will either
need be dummy targets -- by convention, the dummy neurons
are the topmost ones (at end of act array)

by:
bx:

tx: con_idx

Data:

Runtime layout:

nets [blks][SND_UG_SZ]
*/
/* TODO
// CONSTANTS

#define SND_UG_SZ 

#define NETS_PER_BLOCK 
// Types


// Global data structures

uint	n_cons_pu; 
uint	n_blks; // number of [RCV_N_THREADS] blocks needed

con_si_blk_t*	d_con_si_blks; // [n_blks][n_cons_pu][RCV_N_THREADS]
con_wt_blk_t*	d_con_wt_blks; // [n_blks][n_cons_pu][RCV_N_THREADS]

__global__ 
void kSend_Netin(
  int n_cons_pu,
  float* d_nets,
  con_si_blk_t* d_con_si_blks,
  con_wt_blk_t* d_con_wt_blks
) {
  extern __shared__ float s_nets[]; //[SND_UG_SZ]
  
  // block/thread indexes, for clarity
  int blk = blockIdx.x;
  int tx = threadIdx.x;
  
  // init net
  s_nets[tx] = 0;
  
  int blki = blk * n_cons_pu;
  int un_idx = blk * blockDim.x + tx;
  float tnet = 0.0f;
  for (int ci = 0; ci < n_cons_pu; ci++, blki++) {
    tnet += c_acts[d_con_si_blks[blki][tx]] * d_con_wt_blks[blki][tx]; 
  }
  d_nets[un_idx] = tnet; 
}

*/
