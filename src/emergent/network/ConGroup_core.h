// Copyright 2017, Regents of the University of Colorado,
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
  int           alloc_size;     // #CAT_Structure #READ_ONLY #NO_SAVE #SHOW allocated size -- no more than this number of connections may be created -- it is a hard limit set by the alloc function
  int           other_idx;      // #CAT_Structure #READ_ONLY #SHOW index into other direction's list of cons objects (i.e., send_idx for RecvCons and recv_idx for SendCons)
  int           own_flat_idx;   // #IGNORE unit flat index of unit that owns us

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

};
  
#endif // ConGroup_core_h
