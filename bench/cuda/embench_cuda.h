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

// Common constants
#define MAX_UNITS (65536 / sizeof(float)) // note: assumes we can use entire constant mem

// Receiver-based constants

// each thread defines a "silo"; a unit is always processed in its own
// silo, defined as unit_idx % RCV_N_THREADS
#define RCV_N_THREADS 64
#define RCV_CON_CHUNK_SZ 64 // note: this is independent of RCV_N_THREADS
// following, assume 512M, 8-byte chunks
#define RCV_MAX_CON_CHUNKS (0x20000000 / (RCV_CON_CHUNK_SZ * 8))
#define RCV_MAX_SILO_SZ (RCV_MAX_CON_CHUNKS / RCV_N_THREADS)

#ifndef uint
typedef unsigned int uint;
#endif

// connections are allocated in blocks of 32; unused will have un=wt=0

// callback functions
typedef bool (*cbGetCon)(int un_idx, int con_idx, int* snd_idx, float* wt);
  // get the indicated weight, return true when done (when con_idx > max_idx)


// all CUDA functions return 0 on success
extern "C" {
// common
int cuCpHD_Acts(const float* acts);
  // transfer the acts from host to device
int cuCpDH_Nets(float* nets);
  // transfer the nets from device to host
 
// Receiver-based
int cuRecv_AllocMem(uint n_units, uint silo_sz_);
// allocate space on device for n_units, with a total of silo_sz con chunks per silo
int cuRecv_FreeMem();
// free previously alloc'ed mem
int cuRecv_CpHD_Cons(cbGetCon GetCon);
  // transfer the connection units host to device
void cuRecv_Netin();
  // calculate all netins 
  
// Sender-based

void cuSend_Netin(uint n_units, int units[]);
  // process only the units list in the array (by flat_idx)
}

#endif
