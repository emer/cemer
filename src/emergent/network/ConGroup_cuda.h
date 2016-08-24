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
#include <ConGroup_core>

// member includes:

// declare all other types mentioned but not required to include:

class UnitVars_cuda;


class  ConGroup_cuda : public ConGroup_core {
  // NVIDIA CUDA version of ConGroup -- has same memory layout and cuda-usable accessors
public:

  inline float*         MemBlock(float* net_cons_mem, int mem_block) const
  { return net_cons_mem + (mem_idx + alloc_size * mem_block); }
  // access given memory block relative to appropriate network cons_mem block -- just increments of alloc_size from mem_start -- for low-level routines
  inline float*         CnMemBlock(float* net_cons_mem, int mem_block) const
  { return net_cons_mem + (cnmem_idx + alloc_size * mem_block); }
  // access given connection memory block relative to appropriate network cons_mem block -- just increments of alloc_size from cnmem_start -- for low-level routines

  inline UnitVars_cuda* OwnUnVars(char* net_units_mem, const int unit_vars_size)
  { return (UnitVars_cuda*)(net_units_mem[own_flat_idx * unit_vars_size]); }
  // #IGNORE #CAT_Access our own unit variables -- for the unit that owns these connections (could be sending or recv unit depending on type of connection group) -- needs network units_mem_block

  inline float*         OwnCnVar(float* net_cons_mem, int var_no) const
  { return net_cons_mem + (cnmem_idx + (alloc_size * var_no)); }
  // #CAT_Access fastest access (no range checking) to owned connection variable value -- get this float* and then index it directly with loop index -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::SCALE) -- needs appropriate network cons_mem block
  
  inline float&          OwnCn(float* net_cons_mem, int idx, int var_no) const
  { return net_cons_mem[cnmem_idx + (alloc_size * var_no) + idx]; }
  // #CAT_Access fast access (no range checking) to owned connection variable value at given index -- OwnCnVar with index in loop is preferred for fastest access -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)

  inline const int32_t& UnIdx(float* net_cons_mem, int idx) const
  { return ((int32_t*)net_cons_mem)[mem_idx + idx]; }
  // #CAT_Access fast access (no range checking) to unit flat index at given connection index
  inline int32_t&       UnIdx(float* net_cons_mem, int idx)
  { return ((int32_t*)net_cons_mem)[mem_idx + idx]; }
  // #CAT_Access fast access (no range checking) to unit flat index at given connection index

  inline const int32_t& PtrCnIdx(float* net_cons_mem, int idx) const
  { return ((int32_t*)net_cons_mem)[mem_idx + alloc_size + idx]; }
  // #CAT_Access fast access (no range checking) to index of connection within unit cons on other side of connection -- needs appropriate network cons_mem block
  inline int32_t&    PtrCnIdx(float* net_cons_mem, int idx)
  { return ((int32_t*)net_cons_mem)[mem_idx + alloc_size + idx]; }
  // #CAT_Access fast access (no range checking) to index of connection within unit cons on other side of connection -- needs appropriate network cons_mem block

  inline ConGroup_cuda*  UnCons
  (float* my_net_cons_mem, char* net_cgp_mem, const int con_group_size,
   int* net_cgp_start, int idx) const 
  { return (ConGroup_cuda*)
      (net_cgp_mem[(net_cgp_start[UnIdx(my_net_cons_mem, idx)] + other_idx) *
                   con_group_size]); }
  // get ConGroup for this projection in unit at given index at other end of this connection
  
  inline float&  PtrCn
  (float* my_net_cons_mem, char* net_cgp_mem, const int con_group_size,
   int* net_cgp_start, int idx, float* oth_net_cons_mem, int var_no) const {
    return UnCons(my_net_cons_mem, net_cgp_mem, con_group_size, net_cgp_start, idx)->
      OwnCn(oth_net_cons_mem, PtrCnIdx(my_net_cons_mem, idx), var_no);
  }
  // #CAT_Access fast access (no range or own_cons checking) to connection value at given index -- this is MUCH slower than OwnCn due to several index accesses, so where ever possible computations should be performed on the side that owns the connections -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)
  
};

#endif // ConGroup_cuda_h
