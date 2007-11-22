/* Example of integrating CUDA functions into an existing 
 * application / framework.
 * Host part of the device code.
 * Compiled with Cuda compiler.
 */

#ifndef EMBENCH_CUDA
#define EMBENCH_CUDA

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// includes, project
#include <cutil.h>

// note: low level defines, but needed so we can declare max's correctly
#define N_BLOCKS 32
#define N_THREADS 32

// high level defines
#define CON_CHUNK_SZ 32
#define MAX_UNIT_BLOCKS 15
#define MAX_UNITS (MAX_UNIT_BLOCKS * N_BLOCKS * N_THREADS)
#define MAX_CON_CHUNKS (376 * N_BLOCKS * N_THREADS * CON_CHUNK_SZ)

#ifndef uint
typedef unsigned int uint;
#endif

// connections are allocated in blocks of 32; unused will have un=wt=0

// callback functions
typedef float (*cbGetAct)(uint un_idx);
typedef void (*cbGetCon)(uint un_idx, uint con_idx, uint* snd_idx, float* wt);
typedef void (*cbSetNet)(uint un_idx, float net);

typedef void* hcuda; // opaque handle to CUDA-allocated memory


// all CUDA functions return 0 on success
extern "C" {
int cuAllocUnits(uint n_units, uint n_con_chunks);
// allocate space on device for n_units, with a total of n_con_chunks

int cuCpHD_Acts(cbGetAct GetAct);
  // transfer the acts from host to device

int cuCpHD_ConUns(uint un_idx, uint n_cons, cbGetAct GetCon);
  // transfer the connection units host to device

int cuCpDH_Nets(cbSetNet SetNet);
  // transfer the nets from device to host
  
void cuRecv_Netin();
  // calculate all netins 
}

#endif
