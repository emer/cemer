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

#ifndef ConGroup_core_h
#define ConGroup_core_h 1

// define these in main C++ code to be Projection, ConSpec

#ifndef ConGroupProjectionType
#define ConGroupProjectionType void
#endif

#ifndef ConGroupConSpecType
#define ConGroupConSpecType void
#endif


class ConGroup_core {
  // this is the core data for ConGroup, which can be used between CUDA and standard C++ -- CUDA cannot parse Qt and other infrastructure
public:
  
  enum WtSaveFormat {   // note: must be coordinated with the Network enum
    TEXT,                       // weights are saved as ascii text representation of digits (completely portable)
    BINARY,                     // weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
  };

  // note: define new enums for other variables, typically in ConSpec, adding from DWT
  enum ConVars {                // Connection variables -- must align with Connection obj
    WT,                         // the synaptic weight of connection
    DWT,                        // change in synaptic weight as computed by learning mechanism
  };

  enum ConGroupFlags {  // #BITS flags for this connection group
    CG_0 = 0x0,         // #IGNORE 
    OWN_CONS = 0x0001,  // this guy owns the connections -- else gets links to others
    RECV_CONS = 0x0002, // we are a recv con group -- else a send con group
    IS_ACTIVE = 0x0004, // we are an active con group -- projection is active and size > 0
    SHARING = 0x0008,   // this OWN_CONS group is sharing connection objects from another con group
    CHUNKS_SAME_SUGP = 0x0010,  // for Leabra: chunks all have the same sender unit group -- important for optimization of netin
  };

  ConGroupFlags flags;          // #CAT_Structure #READ_ONLY #NO_SAVE #SHOW flags for this connection group
  int           size;           // #CAT_Structure #READ_ONLY #NO_SAVE #SHOW number of connections currently active
  int           vec_chunked_size; // #CAT_Structure #READ_ONLY #NO_SAVE #SHOW number of connections at start of list that are chunked according to vec_chunk_targ -- for sender-based, this means that the recv unit_idx's are sequential for each of the chunks (individually) -- between chunks can be non-sequential
  int           alloc_size;     // #CAT_Structure #READ_ONLY #NO_SAVE #SHOW allocated size -- no more than this number of connections may be created -- it is a hard limit set by the alloc function
  ConGroupProjectionType*   prjn; // #CAT_Structure #READ_ONLY #SHOW #NO_SET_POINTER pointer to the projection which created these connections -- has the source con_type and con spec information
  ConGroupConSpecType*      con_spec; // #IGNORE con spec that we use: controlled entirely by the projection!
  int           other_idx;      // #CAT_Structure #READ_ONLY #SHOW index into other direction's list of cons objects (i.e., send_idx for RecvCons and recv_idx for SendCons)
  int           share_idx;      // #CAT_Structure #READ_ONLY #SHOW index of other unit that this congroup shares connection objects with -- unit must be in same layer with same number of connections, and must be *earlier* in the layer (typically the first unit group) -- projection must set this during the initial pre-allocation phase of connecting -- if -1 then this congroup has its own unique connection objects

  int           n_con_vars;     // #IGNORE number of connection variables
  int           own_flat_idx;   // #IGNORE unit flat index of unit that owns us
  int           own_thr_idx;    // #IGNORE unit thread-specific index of unit that owns us
  float*        mem_start;      // #IGNORE pointer into Network allocated connection memory -- for owned, non-shared cons, it is (con_type->members.size + 1) * sizeof(float) * alloc_size, for shared cons it is just sizeof(float) * alloc_size (first slice is index of other unit), for ptr cons, it is 2 * sizeof(float) * alloc_size (first slice is index of other unit, second is connection number there to find con data)
  float*        cnmem_start;    // #IGNORE pointer to start of connection variable memory -- for shared connections this points to the memory in the source connection. it is of size (con_type->members.size + 1) * sizeof(float) * alloc_size
#ifdef CUDA_COMPILE
  int64_t       mem_idx;        // #IGNORE index into Network allocated thread-specific connection memory corresponding to mem_start -- this is float-sized (32bit) index into either recv_cons_mem or send_cons_mem -- only needed for CUDA
  int64_t       cnmem_idx;        // #IGNORE index into Network allocated thread-specific connection memory corresponding to cnmem_start -- this is float-sized (32bit) index into either recv_cons_mem or send_cons_mem -- only needed for CUDA
#endif


  inline bool           HasConGroupFlag(int flag) const 
  { return (flags & flag); }
  // #CAT_ObjectMgmt true if flag set, or if multiple, any set
  inline void           SetConGroupFlag(int flag)
  { flags = (ConGroupFlags)(flags | flag); }
  // #CAT_ObjectMgmt sets the flag(s)
  inline void           ClearConGroupFlag(int flag)
  { flags = (ConGroupFlags)(flags & ~flag); }
  // #CAT_ObjectMgmt clears the flag(s)
  void                  ChangeConGroupFlag(int flag, bool set)
    {if (set) SetConGroupFlag(flag); else ClearConGroupFlag(flag);}
  // #CAT_ObjectMgmt sets or clears the flag(s)

  bool  OwnCons() const { return HasConGroupFlag(OWN_CONS); }
  // #CAT_Structure do we own the connections?  else just point to them
  bool  PtrCons() const { return !HasConGroupFlag(OWN_CONS); }
  // #CAT_Structure do we have pointers to connections?  else we own them
  bool  IsRecv() const  { return HasConGroupFlag(RECV_CONS); }
  // #CAT_Structure is this a receiving con group?  else sending
  bool  IsSend() const  { return !HasConGroupFlag(RECV_CONS); }
  // #CAT_Structure is this a sending con group?  else receiving
  bool  IsActive() const { return HasConGroupFlag(IS_ACTIVE); }
  // #CAT_Structure is this an active connection group, with connections and an active projection?
  bool  NotActive() const { return !HasConGroupFlag(IS_ACTIVE); }
  // #CAT_Structure is this NOT an active connection group, with connections and an active projection?
  bool  Sharing() const { return HasConGroupFlag(SHARING); }
  // #CAT_Structure is this group sharing connections from another connection group?

  inline bool           InRange(int idx) const
  { return ((idx < size) && (idx >= 0)); }
  // #CAT_Access is index in range?

  inline int            NConVars() const  { return n_con_vars; }
  // #CAT_Access number of connection-level variables

  inline bool           VarInRange(int var_no) const {
    if(var_no < 0 || var_no >= NConVars()) return false;
    return true;
  }

};
  
#endif // ConGroup_core_h
