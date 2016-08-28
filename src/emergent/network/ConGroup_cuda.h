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

#ifndef ConGroup_cuda_h
#define ConGroup_cuda_h 1

// parent includes:
#include <ta_cuda.h>
#include <ConGroup_core>

// member includes:

// declare all other types mentioned but not required to include:

class UnitVars_cuda;


class  ConGroup_cuda : public ConGroup_core {
  // NVIDIA CUDA version of ConGroup -- has same memory layout and cuda-usable accessors
public:
  cudabigint    mem_idx;        // #IGNORE index into Network allocated thread-specific connection memory corresponding to mem_start -- this is float-sized (32bit) index into either recv_cons_mem or send_cons_mem -- only needed for CUDA
  cudabigint    cnmem_idx;        // #IGNORE index into Network allocated thread-specific connection memory corresponding to cnmem_start -- this is float-sized (32bit) index into either recv_cons_mem or send_cons_mem -- only needed for CUDA
  int           con_spec_idx;      // index into cuda_conspecs list of unique conspec parameters, compiled at build time by Cuda_MakeConSpecs
  float         temp1;            // temporary compute value -- e.g., net input

  CUDAFUN inline float*         MemBlock(float* cons_mem, int mem_block) const
  { return cons_mem + (mem_idx + alloc_size * mem_block); }
  // access given memory block relative to appropriate network cons_mem block -- just increments of alloc_size from mem_start -- for low-level routines
  CUDAFUN inline float*         CnMemBlock(float* cons_mem, int mem_block) const
  { return cons_mem + (cnmem_idx + alloc_size * mem_block); }
  // access given connection memory block relative to appropriate network cons_mem block -- just increments of alloc_size from cnmem_start -- for low-level routines

  CUDAFUN inline UnitVars_cuda* OwnUnVars(char* units_mem, const int unit_vars_size)
  { return (UnitVars_cuda*)(units_mem + ((own_flat_idx-1) * unit_vars_size)); }
  // #IGNORE #CAT_Access our own unit variables -- for the unit that owns these connections (could be sending or recv unit depending on type of connection group) -- needs network units_mem_block, which is thread-indexed, not flat, so we subtract one

  CUDAFUN inline float*         OwnCnVar(float* cons_mem, int var_no) const
  { return cons_mem + (cnmem_idx + (alloc_size * var_no)); }
  // #CAT_Access fastest access (no range checking) to owned connection variable value -- get this float* and then index it directly with loop index -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::SCALE) -- needs appropriate network cons_mem block
  
  CUDAFUN inline float&          OwnCn(float* cons_mem, int idx, int var_no) const
  { return cons_mem[cnmem_idx + (alloc_size * var_no) + idx]; }
  // #CAT_Access fast access (no range checking) to owned connection variable value at given index -- OwnCnVar with index in loop is preferred for fastest access -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)

  CUDAFUN inline int32_t        UnIdx(float* cons_mem, int idx) const
  { return ((int32_t*)cons_mem)[mem_idx + idx] -1; }
  // #CAT_Access fast access (no range checking) to unit *cuda* index at given connection index (subtracts 1 automatically from stored flat index!)

  CUDAFUN inline UnitVars_cuda* UnVars(float* cons_mem, int idx, char* units_mem,
                                       const int unit_vars_size)
  { return (UnitVars_cuda*)(units_mem + (UnIdx(cons_mem, idx) * unit_vars_size)); }
  // #IGNORE #CAT_Access fast access (no range checking) to unit pointer at given connection index (goes through flat index at network level) -- this is the unit on the other end of this connection 

  CUDAFUN inline int32_t        PtrCnIdx(float* cons_mem, int idx) const
  { return ((int32_t*)cons_mem)[mem_idx + alloc_size + idx]; }
  // #CAT_Access fast access (no range checking) to index of connection within unit cons on other side of connection -- needs appropriate network cons_mem block

  CUDAFUN inline ConGroup_cuda*  UnCons
  (float* cons_mem, char* cgp_mem, const int con_group_size,
   int* oth_cgp_start, int idx) const 
  { return (ConGroup_cuda*)
      (cgp_mem + ((oth_cgp_start[UnIdx(cons_mem, idx)] + other_idx) *
                      con_group_size)); }
  // get ConGroup for this projection in unit at given index at other end of this connection
  
  CUDAFUN inline float&  PtrCn
  (float* cons_mem, char* cgp_mem, const int con_group_size,
   int* oth_cgp_start, int idx, float* oth_cons_mem, int var_no) const {
    return UnCons(cons_mem, cgp_mem, con_group_size, oth_cgp_start, idx)->
      OwnCn(oth_cons_mem, PtrCnIdx(cons_mem, idx), var_no);
  }
  // #CAT_Access fast access (no range or own_cons checking) to connection value at given index -- this is MUCH slower than OwnCn due to several index accesses, so where ever possible computations should be performed on the side that owns the connections -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)
  
};

#endif // ConGroup_cuda_h
