// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef Network_cuda_h
#define Network_cuda_h 1

// parent includes:
#include "ta_cuda.h"

// member includes:

// declare all other types mentioned but not required to include:

// important: nvcc cannot compile Qt, etc, so we have to define everything
// completely separately in cuda space
// regular files can include this file, and fill in values here..

// IMPORTANT: for indexing into the own_cons_mem, for large nets, we really 
// should use int64_t -- BUT this does NOT seem to work, at least on latest
// mac pro retina late 2014 NVIDIA GeForce GT 750M chip!  should be 3.0 compute 
// capability but still doesn't work reliably!
typedef int bigint;
// typedef int64_t bigint;
// typedef long long bigint;


// CUDA ONLY WORKS WITH THREADS=1 ON NETWORK -- this makes the memory isomorphic!

////////////////////////////////////////////////////////////////////////
//      Memory layout and overall strategy, etc
//
// threads = connections with a sending connection group
// blocks =  units x cons = sending connection groups
//           (one for each sending group in each sending unit)
//
// cons_sizes for example is organized like this, for small layers with diff numbers
//      of sending groups:
// idx  unit con_group
//  0   1    0  # first layer, 2 units, 1 send con group per unit
//  1   2    0
//  2   3    0  # next layer, 2 units, 2 send con groups per unit
//  3   3    1
//  4   4    0
//  5   4    1
//  6   5    0
//  7   5    1
// ... and so on..
//
// A routine like Network::Cuda_Send_NetinDelta will fill in the "ucidx" (idx above)
// into cur_units_x_cons_h by going through the network and finding the sending units
// and sending con groups within them that should be sent on this cycle -- the cuda
// side will then copy this _h -> _d and call the kernel with blocks = number of 
// items in cur_units_x_cons and threads = pre-computed threads based on max number of
// connections in any connection group in the layer (todo: need more params on that)
// the kernel then looks up all the info needed to access the own_cons_mem raw memory
// bank of connection data, and does the computation
//
// All conspec parameters and unit-level variables (avg_s, avg_m etc) need to be 
// consolidated into float* arrays and copied up to the device as needed.
//
// Overall, we ONLY use CUDA for the connection-level processing, due to the need
// to rewrite everything outside of the standard C++ implementation.
// CRITICALLY, to avoid excessive memory transfers, EVERYTHING at the connection
// level must be implemented in CUDA -- the connection memory is only transfered
// back to the host for display or saving the weights at the end.
// 
// *This means that all possible special-function ConSpecs must be implemented here!*
// *Thus, CUDA only supports a subset of what might be avail outside of it*
// 
// The only real interface between connections and the unit-level code is the 
// net input, and then the activations coming back to influence learning, so
// those vectors of size n_units do need to be transferred, but that is far far
// less transfer than the full set of connections.  Overall, memory transfer is 
// a relatively minor cost of the computation.
// 
// The major limitations as far as we can tell are in the memory bandwidth required
// to feed the computations..
//

#include <ConGroup_cuda.h>
#include <UnitVars_core.h>

class Network_cuda {
  // NVIDIA CUDA support for calling LeabraConSpec functions
public:
  enum ConSpecParams { // connection spec params needed for compute dwt, compute weights
    CUR_LRATE,
    N_CON_PARAMS,
  };

  int           min_threads;       // from Network_cudaSpec::min_threads
  int           max_threads;       // from Network_cudaSpec::max_threads
  int           cons_per_thread;   // from Network_cudaSpec::cons_per_thread
  int           n_threads;         // actual number of threads to use -- multiple of 32

  bool          strms_created;     // status flag for streams
  cudaStream_t  strm_memcpy_cons;  // for copying cons
  cudaStream_t  strm_memcpy_units;  // for copying units
  cudaStream_t  strm_compute_netin; // for Compute_Netin
  cudaStream_t  strm_compute_act;  // for Compute_Act
  cudaStream_t  strm_compute_dwt; // for Compute_dWt
  cudaStream_t  strm_compute_wt;  // for Compute_Wt


  ///////////////////////////////////////////////////////////////////////////
  //    Network memory vars, copied directly from Network.h -- keep updated!
  //    CUDA ONLY WORKS WITH THREADS=1 ON NETWORK -- this makes the memory isomorphic!
  //    All our _h = host memory is for thr_no=0, 
  
  int           con_group_size;  // #NO_SAVE #READ_ONLY #CAT_Structure size in *bytes* of con group objects actually built 
  int           unit_vars_size;  // #NO_SAVE #READ_ONLY #CAT_Threads size in *bytes* of the unit_vars_built UnitVars
  int           n_units_built;  // #NO_SAVE #READ_ONLY #CAT_Threads number of units built -- actually the n+1 size of units_flat
  int           n_layers_built; // #NO_SAVE #READ_ONLY #CAT_Threads number of active layers when built -- size of active_layers array
  int           n_ungps_built;  // #NO_SAVE #READ_ONLY #CAT_Threads number of active unit groups when built -- size of active_ungpss array
  char*         units_mem_h;  // #IGNORE array of char[n_units_built * unit_vars_size], containing ALL units  -- this is the primary memory allocation of units
  char*         units_mem_d;

  int          n_lay_stats;     // #IGNORE #DEF_6 number of thread-specific layer-level statistics that require variable memory storage
  int          n_lay_stats_vars; // #IGNORE #DEF_6 number of thread-specific layer-level statistic variables, per stat, available for stats algorithms
  float*       lay_stats_h;       // #IGNORE layer-level stats variables available for stats routines to do efficient initial pre-computation across units at the thread level, followed by a main-thread integration of the thread-specific values --  float[n_lay_stats * n_lay_stats_vars * n_layers_built] -- n_lay_stats_vars is accessed as the inner dimension, then n_layers_built, then n_lay_stats as outer
  float*       lay_stats_d;

  int*          units_n_recv_cgps_h;  // #IGNORE number of receiving connection groups per unit (flat_idx unit indexing, starts at 1)
  int*          units_n_recv_cgps_d;
  
  int*          units_n_send_cgps_h;  // #IGNORE number of sending connection groups per unit (flat_idx unit indexing, starts at 1)
  int*          units_n_send_cgps_d;
  
  int           n_recv_cgps; // #IGNORE total number of units * recv con groups
  int           n_send_cgps; // #IGNORE total number of units * send con groups

  char*         recv_cgp_mem_h; // #IGNORE memory allocation for ConGroup for all recv connection group objects, array of char[n_recv_cgps * con_group_size], containing the recv ConGroup processed -- this is the primary memory allocation of recv ConGroups
  char*         recv_cgp_mem_d;
  char*         send_cgp_mem_h; // #IGNORE memory allocation for ConGroup for all send connection group objects, array of char[n_send_cgps * con_group_size], containing the send ConGroup processed -- this is the primary memory allocation of send ConGroups
  char*         send_cgp_mem_d;
  
  int*          recv_cgp_start_h; // #IGNORE starting indexes into recv_cgp_mem, per unit -- array of int[n_units_built] -- contains 0 for units that have none
  int*          recv_cgp_start_d;
  int*          send_cgp_start_h; // #IGNORE starting indexes into send_cgp_mem, per unit -- array of int[n_units_built] -- contains 0 for units that have none 
  int*          send_cgp_start_d;

  bigint        recv_cons_cnt; // #IGNORE number of floats to allocate to recv_cons_mem
  bigint        send_cons_cnt; // #IGNORE number of floats to allocate to send_cons_mem
  float*        recv_cons_mem_h; // #IGNORE bulk memory allocated for all of the recv connections, array of float[recv_cons_cnt]
  float*        recv_cons_mem_d;
  float*        send_cons_mem_h; // #IGNORE bulk memory allocated for all of the send connections, array of float[send_cons_cnt]
  float*        send_cons_mem_d;

  int           own_cons_max_size; // #IGNORE maximum alloc_size of any owning connection group -- for allocating temp structures..
  bigint        own_cons_tot_size; // #IGNORE total number of owned connections
  bigint        own_cons_tot_size_nonshared; // #IGNORE total number of owned connections, by thread, non-shared
  int           own_cons_avg_size; // #IGNORE average size of any owning connection group -- for optimizing computation
  int           own_cons_max_vars; // #IGNORE maximum NConVars of any owning connection group -- for allocating temp structures..

  float*        con_params_h; // host array of sending units * sending con groups -- size own_units_x_cons * N_CON_PARAMS -- parameters for each con group
  float*        con_params_d; // device array of sending units * sending con groups -- size own_units_x_cons * N_CON_PARAMS -- parameters for each con group

  inline UnitVars_core*  UnitVars(char* net_units_mem, int flat_idx) const
  { return (UnitVars_core*)(net_units_mem[flat_idx * unit_vars_size]); }
  // #CAT_Structure unit variables for unit at given unit at flat_idx 

  inline ConGroup_cuda*  UnitVars(char* net_units_mem, int flat_idx) const
  { return (UnitVars*)(net_units_mem[flat_idx * unit_vars_size]); }
  // #CAT_Structure unit variables for unit at given unit at flat_idx 

  inline int    ThrLayUnStart(int thr_no, int lay_no)
  { return thrs_lay_unit_idxs[thr_no][2*lay_no]; }
  // #CAT_Structure starting thread-specific unit index for given layer (from active_layers list)
  inline int    ThrLayUnEnd(int thr_no, int lay_no)
  { return thrs_lay_unit_idxs[thr_no][2*lay_no + 1]; }
  // #CAT_Structure ending thread-specific unit index for given layer (from active_layers list) -- this is like the max in a for loop -- valid indexes are < end


  inline float*   RecvCnVar(int uncon, int var_no) const
  { return recv_cons_mem_d + con_mem_idxs_d[uncon] + (con_allocs_d[uncon] * (1 + var_no)); }
  // illustration for how to access connection variable in compute code: access connection variable for unit x con, variable

  inline int      UnIdx(int uncon, int idx) const
  { return ((int*)(own_cons_mem_d + con_mem_idxs_d[uncon]))[idx]; }
  // illustration for how to access access recv unit idx for unit x con

  inline float&   ConParam_h(int uncon, int param_no)
  { return con_params_h[uncon * N_CON_PARAMS + param_no]; }
  // connection parameter, on host, for given unit x con and parameter number

  
  void  AllocCudaArrays
  (
   int    con_group_size,   
   int    unit_vars_size,  
   int    n_units_built,  
   int    n_layers_built, 
   int    n_ungps_built,  
   char*  units_mem_h,  

   int    n_lay_stats,     
   int    n_lay_stats_vars, 
   float* lay_stats_h,       
   int*   units_n_recv_cgps_h,  
   int*   units_n_send_cgps_h,  
  
   int    n_recv_cgps, 
   int    n_send_cgps, 

   char*  recv_cgp_mem_h, 
   char*  send_cgp_mem_h, 
   int*   recv_cgp_start_h, 
   int*   send_cgp_start_h,  

   bigint recv_cons_cnt, 
   bigint send_cons_cnt, 
   float* recv_cons_mem_h, 
   float* send_cons_mem_h
   );

  void  FreeCudaArrays();
  // free arrays..

  void  RecvCons_HostToDevice(bool sync = false);
  // copy all the recv_cons_mem (weights etc) from host to device: after initializing weights -- optionally wait for copy to finish
  void  RecvCons_DeviceToHost(bool sync = false);
  // copy all the recv_cons_mem (weights etc) from device to host: for visualization etc -- optionally wait for copy to finish

  void  UpdateConParams();
  // call whenever any connection parameters change -- copy con_params_h -> d 

  void  Compute_Netin();
  // AFTER filling in cur_units_x_cons_n, cur_units_x_cons_h..

  void  Compute_dWt(bool sync = true);
  // AFTER filling in cur_units_x_cons_n, cur_units_x_cons_h, and unit_vec_vars_h, this will compute weight changes -- ONLY on CUDA side -- call OwnCons_DeviceToHost to get back
  void  Compute_Weights(bool sync = false);
  // compute weight updates -- ONLY on CUDA side -- call OwnCons_DeviceToHost to get back

  Network_cuda();
  ~Network_cuda();
  void Initialize();
};

#endif // Network_cuda_h
