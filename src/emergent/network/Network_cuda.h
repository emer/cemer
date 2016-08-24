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
// this is automatically enforced

////////////////////////////////////////////////////////////////////////
//      Memory layout and overall strategy, etc
//
// threads = connections with a connection group --
//           each thread divides up the list of connections by the total num of threads:
//
//   key thread-level code to divvy up sz number of connections:
//
//   const int nth = blockDim.x; // number of threads
//   int th = threadIdx.x;
//   const float cn_per_th = ((float)sz / (float)nth);
//   int st = __float2int_rn((float)th * cn_per_th);
//   int ed = __float2int_rn((float)(th+1) * cn_per_th);
//   while(st < ed) {
//     <process connection number st>
//     st++;
//   }
//
// TODO: docs say the while loop can be a problem if it diverges
// could also experiment with various other ways of allocating
// e.g., padding connections so all same length and having fixed iters, etc
// NOTE: might be faster to keep all threads at an even workload EXCEPT the last one?
// ie.., don't use the round float thing, but just use sz / nthr

// blocks = units x con-groups-per-unit = total list of connection groups to process
//
//   code:
//
//   const int cgp_idx = blockIdx.x;  // ConGroup index in list of congroups to process
//
//   then just access the relevant data structures from that -- the ConGroup_cuda
//   structure has the owning unit index in it, so relevant unit-level data can
//   be accessed from there
//
//   also, all conspec parameters and unit-level variables (avg_s, avg_m etc) need to be 
//   consolidated into float* arrays and copied up to the device as needed.
//

#include <ConGroup_cuda.h>
#include <UnitVars_cuda.h>


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
  char*         units_mem_h;  // array of char[n_units_built * unit_vars_size], containing ALL units  -- this is the primary memory allocation of units
  char*         units_mem_d;
  int*          lay_unit_idxs_h; // allocation of units to layers -- arrays of int[n_layers_built * 2], containing start and end unit indexes of units processed by a given thread and a given layer
  int*          lay_unit_idxs_d;
  int*          ungp_unit_idxs_h; // allocation of units to unit groups by threads -- array of int**[n_thrs_built], pointing to arrays of int[n_ungps_built * 2], containing  start and end thr_un_idx indexes of units processed by a given thread and a given unit group
  int*          ungp_unit_idxs_d;

  int           n_lay_stats;     // #DEF_6 number of thread-specific layer-level statistics that require variable memory storage
  int           n_lay_stats_vars; // #DEF_6 number of thread-specific layer-level statistic variables, per stat, available for stats algorithms
  float*        lay_stats_h;       // layer-level stats variables available for stats routines to do efficient initial pre-computation across units at the thread level, followed by a main-thread integration of the thread-specific values --  float[n_lay_stats * n_lay_stats_vars * n_layers_built] -- n_lay_stats_vars is accessed as the inner dimension, then n_layers_built, then n_lay_stats as outer
  float*        lay_stats_d;

  int*          units_n_recv_cgps_h;  // number of receiving connection groups per unit (flat_idx unit indexing, starts at 1)
  int*          units_n_recv_cgps_d;
  
  int*          units_n_send_cgps_h;  // number of sending connection groups per unit (flat_idx unit indexing, starts at 1)
  int*          units_n_send_cgps_d;
  
  int           n_recv_cgps; // total number of units * recv con groups
  int           n_send_cgps; // total number of units * send con groups

  char*         recv_cgp_mem_h; // memory allocation for ConGroup for all recv connection group objects, array of char[n_recv_cgps * con_group_size], containing the recv ConGroup processed -- this is the primary memory allocation of recv ConGroups
  char*         recv_cgp_mem_d;
  char*         send_cgp_mem_h; // memory allocation for ConGroup for all send connection group objects, array of char[n_send_cgps * con_group_size], containing the send ConGroup processed -- this is the primary memory allocation of send ConGroups
  char*         send_cgp_mem_d;
  
  int*          recv_cgp_start_h; // starting indexes into recv_cgp_mem, per unit -- array of int[n_units_built] -- contains 0 for units that have none
  int*          recv_cgp_start_d;
  int*          send_cgp_start_h; // starting indexes into send_cgp_mem, per unit -- array of int[n_units_built] -- contains 0 for units that have none 
  int*          send_cgp_start_d;

  bigint        recv_cons_cnt; // number of floats to allocate to recv_cons_mem
  bigint        send_cons_cnt; // number of floats to allocate to send_cons_mem
  float*        recv_cons_mem_h; // bulk memory allocated for all of the recv connections, array of float[recv_cons_cnt]
  float*        recv_cons_mem_d;
  float*        send_cons_mem_h; // bulk memory allocated for all of the send connections, array of float[send_cons_cnt]
  float*        send_cons_mem_d;

  int           own_cons_max_size; // maximum alloc_size of any owning connection group -- for allocating temp structures..
  bigint        own_cons_tot_size; // total number of owned connections
  bigint        own_cons_tot_size_nonshared; // total number of owned connections, by thread, non-shared
  int           own_cons_avg_size; // average size of any owning connection group -- for optimizing computation
  int           own_cons_max_vars; // maximum NConVars of any owning connection group -- for allocating temp structures..

  float*        con_params_h; // host array of sending units * sending con groups -- size own_units_x_cons * N_CON_PARAMS -- parameters for each con group
  float*        con_params_d; // device array of sending units * sending con groups -- size own_units_x_cons * N_CON_PARAMS -- parameters for each con group

  ///////////////////////////////////////////////////////////////////////////////
  //  Key Accessor Routines -- static with all args explicitly passed for use
  //   inside a kernel
  
  static inline UnitVars_cuda*  GetUnitVars(char* net_units_mem, const int unit_vars_size,
                                            const int unit_idx)
  { return (UnitVars_cuda*)(net_units_mem[unit_idx * unit_vars_size]); }
  // #CAT_Structure unit variables for unit at given unit at unit flat_idx 

  static inline int GetNConGroups(const int* net_n_recv_cgps,
                                        const int unit_idx)
  { return net_n_recv_cgps[unit_idx]; }
  // #CAT_Structure number of congroups for given unit index -- pass in appropriate recv or send memory

  static inline ConGroup_cuda*  GetUnConGroup
  (char* net_cgp_mem, const int* net_cgp_start, const int con_group_size,
   const int unit_idx, const int cgp_idx)
  { return (ConGroup_cuda*)
      (net_cgp_mem[(net_cgp_start[unit_idx] + cgp_idx) * con_group_size]); }
  // #CAT_Structure connection group for given unit flat index, con group index, pass in appropriate recv or send cgp_mem and cgp_start from network

  static inline ConGroup_cuda*  GetConGroup_Flat
  (char* net_cgp_mem, const int con_group_size, const int cgp_idx)
  { return (ConGroup_cuda*)(net_cgp_mem[cgp_idx * con_group_size]); }
  // #CAT_Structure connection group for given global con group index, pass in appropriate recv or send cgp_mem and cgp_start from network
  
  // once you have the con group, use accessor routines there for further con access
  
  static inline int  LayUnStart(const int* net_lay_unit_idxs, const int lay_no)
  { return net_lay_unit_idxs[2*lay_no]; }
  // #CAT_Structure starting unit index for given layer (from active_layers list)
  static inline int  LayUnEnd(const int* net_lay_unit_idxs, const int lay_no)
  { return net_lay_unit_idxs[2*lay_no + 1]; }
  // #CAT_Structure ending unit index for given layer (from active_layers list) -- this is like the max in a for loop -- valid indexes are < end


  // can't define this here b/c of the __float2int_rn function which only exists on
  // device
  // static inline void GetThreadCons(const int nthrs, const int thr_no,
  //                                  const int n_cons, int& st, int& ed)
  // { const float cn_per_th = ((float)n_cons / (float)nthrs);
  //   st = __float2int_rn((float)thr_no * cn_per_th);
  //   ed = __float2int_rn((float)(thr_no+1) * cn_per_th);
  //   ed = ed < n_cons ? ed : n_cons;     // max of n_cons
  // }
  // Get starting and ending connection index numbers to process within a kernel operating
  
  //////////////////////////////////////////////////////
  //            Main Constructor and memory copying
  
  void  AllocCudaArrays
  (
   int    con_group_size,   
   int    unit_vars_size,  
   int    n_units_built,  
   int    n_layers_built, 
   int    n_ungps_built,  
   char*  units_mem_h,
   int*   lay_unit_idxs_h,
   int*   ungp_unit_idxs_h,

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


  //////////////////////////////////////////////////////
  //    Computation routines
  
  void  Compute_NetinAct();

  // void  Compute_dWt(bool sync = true);
  // void  Compute_Weights(bool sync = false);

  Network_cuda();
  ~Network_cuda();
  void Initialize();
};

#endif // Network_cuda_h
