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
#include <ActrChunkTypeRef>
#include <taNBase>

// member includes:
#include <ActrSlotType_List>
#include <ActrChunkType_List>

#ifdef slots
#undef slots
#endif

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrChunkType);

class E_API ActrChunkType : public taNBase {
  // ##INSTANCE ##EDIT_INLINE ##CAT_ActR ##SCOPE_ActrModel ##DEF_CHILD_slots ##DEF_CHILDNAME_Slots a chunk type for defining ActR chunks
INHERITED(taNBase)
public:
  String               desc; // #EDIT_DIALOG #HIDDEN_INLINE description of this chunk type
  ActrChunkTypeRef     parent; // type that this one inherits from -- we automatically get the slots from the parent if set
  ActrSlotType_List    slots;  // #SHOW_TREE #NO_EXPAND_ALL the names and types of the slots within this chunk
  ActrChunkType_List   sub_types; // #HIDDEN #LINK_LIST child types that derive from this type

  virtual ActrSlotType* NewSlot(const String& slot_nm = _nilString);
  // #BUTTON #CAT_ActR #ARGC_0 make a new slot in this chunk type
  virtual void          MakeSlots(const String& slot_0,
                                  const String& slot_1 = _nilString,
                                  const String& slot_2 = _nilString,
                                  const String& slot_3 = _nilString,
                                  const String& slot_4 = _nilString,
                                  const String& slot_5 = _nilString,
                                  const String& slot_6 = _nilString,
                                  const String& slot_7 = _nilString,
                                  const String& slot_8 = _nilString,
                                  const String& slot_9 = _nilString,
                                  const String& slot_a = _nilString,
                                  const String& slot_b = _nilString);
  // #CAT_ActR mass create slots -- all will be LITERAL type by default
  virtual void          SetSlotChunkType(const String& slot_nm, const String& chunk_typ);
  // #CAT_ActR set given slot to be a pointer to another chunk of given type, instead of a literal value

  virtual void          UpdateAllChunks();
  // #BUTTON #CAT_ActR update all the chunks that use this chunk type to conform to any changes made in this type
  virtual bool          UpdateFromParent();
  // #CAT_ActR update our slot information from the parent -- returns true if updates occurred -- updates the parent first
  virtual bool          UpdateFromParent_impl();
  // #IGNORE actually do the update itself -- doesn't update the parent
  virtual bool          UpdateAllSubTypes();
  // #BUTTON update all the sub-types of this type -- called automatically in uae
  virtual void          SetParent(ActrChunkType* par);
  // #CAT_ActR set the parent of this chunk type and update our slots from parent

  virtual bool          InheritsFromCT(ActrChunkType* par);
  // #CAT_ActR determine if this chunk type inherits from given parent chunk type, including being the same type itself
  virtual bool          InheritsFromCTName(const String& chunk_type_nm);
  // #CAT_ActR determine if this chunk type inherits from given parent chunk type name, including being the same type itself
  virtual ActrChunkType* CommonChunkType(ActrChunkType* other);
  // #CAT_ActR returns the highest common chunk type between this chunk type and the other -- NULL if they are not related

  taList_impl*	children_() override {return &slots;}	
  Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const override
  { return slots.Elem(idx, mode); }
  String       GetDesc() const override {return desc;}
  String       GetTypeDecoKey() const override { return "ProgType"; }

  TA_SIMPLE_BASEFUNS(ActrChunkType);
protected:
  void         UpdateAfterEdit_impl() override;
  //  void	CheckChildConfig_impl(bool quiet, bool& rval) override;
private:
  void Initialize();
  void Destroy();
};

#endif // ActrChunkType_h
