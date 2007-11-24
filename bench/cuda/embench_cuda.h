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
#define N_THREADS 32

// each thread defines a "silo"; a unit is always processed in its own
// silo, defined as unit_idx % N_THREADS

// high level defines
#define CON_CHUNK_SZ 32 // note: this is independent of N_THREADS
#define MAX_UNITS (65536 / sizeof(float)) // note: assumes we can use entire constant mem
// following, assume 512M, 8-byte chunks
#define MAX_CON_CHUNKS (0x20000000 / (CON_CHUNK_SZ * 8))
#define MAX_SILO_SZ (MAX_CON_CHUNKS / N_THREADS)

#ifndef uint
typedef unsigned int uint;
#endif

// connections are allocated in blocks of 32; unused will have un=wt=0

// callback functions
typedef bool (*cbGetCon)(int un_idx, int con_idx, int* snd_idx, float* wt);
  // get the indicated weight, return true when done (when con_idx > max_idx)


// all CUDA functions return 0 on success
extern "C" {
int cuAllocMem(uint n_units, uint silo_sz_);
// allocate space on device for n_units, with a total of silo_sz con chunks per silo

int cuFreeMem();
// free previously alloc'ed mem

int cuCpHD_Acts(const float* acts);
  // transfer the acts from host to device

int cuCpHD_Cons(cbGetCon GetCon);
  // transfer the connection units host to device

int cuCpDH_Nets(float* nets);
  // transfer the nets from device to host
  
void cuRecv_Netin();
  // calculate all netins 
}

#endif
