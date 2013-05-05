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

#ifndef ActrAction_h
#define ActrAction_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <ActrBuffer>
#include <ActrChunk>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrAction);

class E_API ActrAction : public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_ActR one action that is performed when an Act-R production fires
INHERITED(taOBase)
public:
  enum ActType {
    UPDATE,              // update buffer state of ActR chunk in a buffer with chunk info
    REQUEST,             // request info from buffer (and associated module) according to chunk info
    CLEAR,               // clear chunk from buffer
  };

  ActType       action;         // what type of action to perform
  ActrBufferRef buffer;         // what buffer to operate on
  ActrChunk     chunk;          // chunk information for the action

  override String       GetTypeDecoKey() const { return "Function"; }

  TA_SIMPLE_BASEFUNS(ActrAction);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

SmartRef_Of(ActrAction); // ActrActionRef

#endif // ActrAction_h
