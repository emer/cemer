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
#define RCV_N_THREADS 32
// following, assume 512M, 8-byte chunks
#define RCV_MAX_CON_SZ (0x20000000 / (MAX_UNITS * 8))

#ifndef uint
typedef unsigned int uint;
#endif

// connections are allocated in blocks of 32; unused will have un=wt=0

// callback functions
typedef bool (*cbGetCon)(int un_idx, int con_idx, int* snd_idx, float* wt);
  // get the indicated weight, return true when done (when con_idx > max_idx)

typedef bool (*cbGetUnitInfo)(int un_idx, int* n_cons);
  // get the requested values for the unit
typedef void (*cbGetLayerInfo)(int lay_idx, int* n_units);
  // get the requested values for the unit

// all CUDA functions return 0 on success
extern "C" {
// common
int cuCpHD_Acts(const float* acts);
  // transfer the acts from host to device
int cuCpDH_Nets(float* nets);
  // transfer the nets from device to host
void cuComputeActs(float* acts);
  // compute the acts (uses nets inside the device)
   
// Receiver-based
int cuRecv_AllocMem(uint n_units, uint con_sz_);
// allocate space on device for n_units, with connection size of con_sz per unit
int cuRecv_FreeMem();
// free previously alloc'ed mem
int cuRecv_CpHD_Cons(cbGetCon GetCon);
  // transfer the connection units host to device
void cuRecv_Netin();
  // calculate all netins 
  
// Sender-based

int cuSend_CpHD_Cons(int n_layers, cbGetUnitInfo GetUnitInfo, cbGetCon GetCon);
  // transfer the connection units host to device
void cuSend_Netin(uint n_units, int units[]);
  // process only the units list in the array (by flat_idx)
}

#endif
