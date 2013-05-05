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
#include <taNBase>

// member includes:
#include <ActrChunk_List>
#include <ActrModuleRef>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrBuffer);

class E_API ActrBuffer : public taNBase {
  // ##INSTANCE ##CAT_ActR ##SCOPE_ActrModel a named buffer in ActR -- holds active chunk(s)
INHERITED(taNBase)
public:
  enum BufferState {
    BS_EMPTY,                   // buffer is empty
    BS_FULL,                    // buffer is full
    BS_REQ,                     // buffer is requested
    BS_UNREQ,                   // buffer is unrequested
  };

  String                desc;      // #EDIT_DIALOG #HIDDEN_INLINE description of this buffer
  ActrChunk_List        active;    // active chunk(s) in the buffer
  ActrModuleRef         module;    // module that this buffer belongs to
  float                 act_total; // total activation that this buffer can contribute 
  BufferState           state;     // #READ_ONLY #SHOW current state of the buffer

  // todo: figure out spreading activation stuff..

  bool  IsEmpty()       { return state == BS_EMPTY; }
  bool  IsFull()        { return state == BS_FULL && active.size >= 1; }
  
  ActrChunk*            CurChunk() 
  { if(active.size == 0) return NULL; return active.FastEl(0); }

  bool  Matches(const String& query);
  // does state of buffer or owning module match given query value -- must be: buffers: full, empty, requested, unrequested  modules: busy, free, error

  virtual void          UpdateState();

  override String       GetDesc() const {return desc;}
  override String 	GetTypeDecoKey() const { return "DataTable"; }

  TA_SIMPLE_BASEFUNS(ActrBuffer);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

SmartRef_Of(ActrBuffer); // ActrBufferRef

#ifdef __TA_COMPILE__
#include <ActrModule>
#endif

#endif // ActrBuffer_h
