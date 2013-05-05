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

#ifndef ActrSlot_h
#define ActrSlot_h 1

// parent includes:
#include <taOBase>
#include "network_def.h"

// member includes:
#include <ActrChunk>

// declare all other types mentioned but not required to include:
class ActrSlotType; // 

eTypeDef_Of(ActrSlot);

class E_API ActrSlot : public taOBase {
  // ##INSTANCE ##EDIT_INLINE ##CAT_ActR ##SCOPE_ActrModel One slot in an ActR memory chunk
INHERITED(taOBase)
public:
  enum SlotValType {            // slot value type -- must be same as in ActrSlotType
    LITERAL,                    // value is a literal value (string)
    CHUNK,                      // value is a pointer to another chunk
  };

  String        name;           // #READ_ONLY name of this slot (automatically set by chunk type)
  SlotValType   val_type;       // what type of value fills this slot
  ActrChunkRef  val_chunk;      // #CONDSHOW_ON_val_type:CHUNK the value as a pointer to another chunk
  String        val;            // #CONDSHOW_ON_val_type:LITERAL the value as a literal value -- empty or "nil" = not set

  virtual bool          IsNil();
  // #CAT_ActR is this item empty or not?
  virtual bool          Matches(ActrSlot* os);
  // #CAT_ActR does this match against other slot?  'this' is the LHS of production
  virtual String        WhyNot(ActrSlot* os);
  // #CAT_ActR why doesn't this match other slot?

  virtual bool          UpdateFromType(const ActrSlotType& typ);
  // initialize this slot from given slot type -- this is called by parent chunk based on chunk type -- returns true if updated

  override String       GetTypeDecoKey() const { return "ProgType"; }

  TA_SIMPLE_BASEFUNS(ActrSlot);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrSlot_h
