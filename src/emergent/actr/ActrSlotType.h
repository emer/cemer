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

#ifndef ActrSlotType_h
#define ActrSlotType_h 1

// parent includes:
#include <taNBase>
#include "network_def.h"

// member includes:
#include <ActrChunkType>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrSlotType);

class E_API ActrSlotType : public taNBase {
  // ##INSTANCE ##EDIT_INLINE ##CAT_ActR ##SCOPE_ActrModel type information (name, value type) for defining ActR slots within chunks
INHERITED(taNBase)
public:
  enum SlotValType {
    LITERAL,                    // value is a literal value (string)
    CHUNK,                      // value is a pointer to another chunk
  };

  String                desc; // #EDIT_DIALOG #HIDDEN_INLINE description of this slot type
  SlotValType           val_type;       // what type of value fills this slot
  ActrChunkTypeRef      val_chunk_type; // #CONDSHOW_ON_val_type:CHUNK what type of chunk should fill this slot

  virtual bool  UpdateFromType(const ActrSlotType& typ);
  // #CAT_ActR update our slot type info from other slot type -- return true if changes occurred
  virtual void  SetChunkType(ActrChunkType* ct);
  // #CAT_ActR set chunk type to given type -- sets val_type and val_chunk_type
  virtual void  SetChunkTypeName(const String& ct_name);
  // #CAT_ActR set chunk type to given type name -- sets val_type and val_chunk_type


  override String GetDisplayName() const;
  override String GetDesc() const {return desc;}
  override String GetTypeDecoKey() const { return "ProgType"; }

  TA_SIMPLE_BASEFUNS(ActrSlotType);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrSlotType_h
