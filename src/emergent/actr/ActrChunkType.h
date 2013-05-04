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

#ifndef ActrChunkType_h
#define ActrChunkType_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:
#include <String_Array>

#ifdef slots
#undef slots
#endif

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrChunkType);

class E_API ActrChunkType : public taNBase {
  // ##INSTANCE ##EDIT_INLINE ##CAT_ActR ##SCOPE_ActrModel a chunk type for defining ActR chunks
INHERITED(taNBase)
public:
  String               desc; // #EDIT_DIALOG #HIDDEN_INLINE description of this buffer
  String_Array         slots;  // the names of the slots within this chunk

  override String       GetTypeDecoKey() const { return "ProgType"; }

  TA_SIMPLE_BASEFUNS(ActrChunkType);
private:
  void Initialize();
  void Destroy();
};

SmartRef_Of(ActrChunkType); // ActrChunkTypeRef

#endif // ActrChunkType_h
