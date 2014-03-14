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

#ifndef ActrBuffer_h
#define ActrBuffer_h 1

// parent includes:
#include <ActrNBase>

// member includes:
#include <ActrChunk_List>
#include <ActrModuleRef>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrBuffer);

class E_API ActrBuffer : public ActrNBase {
  // a named buffer in ActR -- holds active chunk(s)
INHERITED(ActrNBase)
public:
  enum BufferFlags { // #BITS ActR buffer flags
    BF_NONE             = 0, // #NO_BIT
    FULL                = 0x0001, // buffer is full (else empty)
    REQ                 = 0x0002, // buffer has pending request (else not)
    STRICT_HARVEST      = 0x0004, // when a production matches against chunk in this buffer, it is harvested (cleared)
    DM_MERGE            = 0x0008, // when a chunk is cleared from harvesting or explicit clear event, does it merge into declarative memory?
    
    STD_FLAGS           = STRICT_HARVEST | DM_MERGE, // #NO_BIT standard buffer flags
  };

  String                desc;      // #EDIT_DIALOG #HIDDEN_INLINE description of this buffer
  BufferFlags           flags;     // current state of the buffer
  ActrChunk_List        active;    // active chunk(s) in the buffer
  ActrModuleRef         module;    // module that this buffer belongs to
  float                 act_total; // total activation that this buffer can contribute 

  // todo: figure out spreading activation stuff..

  inline void           SetBufferFlag(BufferFlags flg)
  { flags = (BufferFlags)(flags | flg); }
  // #CAT_Flags set flag state on
  inline void           ClearBufferFlag(BufferFlags flg)
  { flags = (BufferFlags)(flags & ~flg); }
  // #CAT_Flags clear flag state (set off)
  inline bool           HasBufferFlag(BufferFlags flg) const
  { return (flags & flg); }
  // #CAT_Flags check if flag is set
  inline void           SetBufferFlagState(BufferFlags flg, bool on)
  { if(on) SetBufferFlag(flg); else ClearBufferFlag(flg); }
  // #CAT_Flags set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           ToggleBufferFlag(BufferFlags flg)
  { SetBufferFlagState(flg, !HasBufferFlag(flg)); }
  // #CAT_Flags toggle flag

  bool  IsFull()        { return HasBufferFlag(FULL) && active.size >= 1; }
  bool  IsEmpty()       { return !IsFull(); }
  bool  IsReq()         { return HasBufferFlag(REQ); }
  bool  IsUnReq()       { return !IsReq(); }
  void  SetReq()        { SetBufferFlag(REQ); }
  void  ClearReq()      { ClearBufferFlag(REQ); }
  
  ActrChunk*            CurChunk() 
  { if(active.size == 0) return NULL; return active.FastEl(0); }
  // #CAT_ActR get the current chunk in buffer if there is one (returns NULL if not)

  virtual bool          HarvestChunk();
  // #CAT_ActR called in response to a BUFFER-READ-ACTION -- harvest (or not) any current chunk per flag settings for this buffer, merging into declarative if flagged
  virtual bool          ClearChunk();
  // #CAT_ActR called in response to a CLEAR-BUFFER -- clear any current chunk, merging into declarative if flagged
  virtual bool          ClearChunk_impl(bool dm_merge = true);
  // #CAT_ActR implementation function that does merge per arg, ignoring flags: clear any existing chunk from buffer, and merge into declarative memory (normal ACT-R behavior for any chunk that is cleared) -- also updates state -- returns false if error
  virtual ActrChunk*    UpdateChunk(ActrChunk* chunk);
  // #CAT_ActR update chunk from new item coming in -- if there is currently a chunk, it is updated with non-nil elements from new chunk (unless chunk types differ, in which case it replaces entirely) -- if no existing chunk, it makes a *copy* of the chunk and updates state -- returns new chunk
  virtual ActrChunk*    SetChunk(ActrChunk* chunk);
  // #CAT_ActR set chunk to be new one -- if there is currently a chunk, it is cleared, then it makes a *copy* of the chunk and updates state -- returns new chunk

  bool  QueryMatches(const String& query, bool why_not = false);
  // #CAT_ActR for production matching: does state of buffer match given query value -- must be: buffer: full, empty, requested, unrequested, module: busy, free, error

  virtual void          Init();
  // #CAT_ActR initialize buffer at start of run

  virtual void          UpdateState();
  // #CAT_ActR update buffer state -- call this whenver anything changes!

  String       GetDesc() const override {return desc;}
  String 	GetTypeDecoKey() const override { return "DataTable"; }

  TA_SIMPLE_BASEFUNS(ActrBuffer);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

SMARTREF_OF(ActrBuffer); // ActrBufferRef

#ifdef __TA_COMPILE__
#include <ActrModule>
#endif

#endif // ActrBuffer_h
