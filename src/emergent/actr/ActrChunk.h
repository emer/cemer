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

#ifndef ActrChunk_h
#define ActrChunk_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ActrChunkRef>
#include <ActrChunkType>
#include <ActrSlot_List>

#ifndef __MAKETA__
# include <cmath>
#endif

// declare all other types mentioned but not required to include:
class ActrProduction; //

eTypeDef_Of(ActrChunk);

class E_API ActrChunk : public taNBase {
  // ##INSTANCE ##EDIT_INLINE ##CAT_ActR ##SCOPE_ActrModel ##DEF_CHILD_slots ##DEF_CHILDNAME_SlotVals a single chunk of memory in ActR
INHERITED(taNBase)
public:
  enum ChunkFlags { // #BITS ActR chunk flags
    CF_NONE             = 0, // #NO_BIT
    RETRIEVED           = 0x0001, // this chunk was just retrieved on last retrieval
    ELIGIBLE            = 0x0002, // this chunk was eligible to be retrieved on last retrieval
    RECENT              = 0x0004, // this chunk was recently retrieved -- some productions may want to ignore
  };

  ActrChunkTypeRef      chunk_type; // the type of this chunk -- enforces our structure to match
  ChunkFlags            flags;      // #READ_ONLY #SHOW flags indicating state of the chunk
  float                 n_act;      // #READ_ONLY #SHOW number of times chunk has been activated
  float                 t_new;      // #READ_ONLY #SHOW time when chunk was created
  float                 base_act;   // #READ_ONLY #SHOW base level activation = ln(n_act / (1-d)) - d * ln(time - t_new) where time = current time and d = decay parameter (optimized calculation)
  ActrSlot_List         slots;      // #NO_EXPAND_ALL the slot values -- same number as slots in chunk_type

  inline void           SetChunkFlag(ChunkFlags flg)
  { flags = (ChunkFlags)(flags | flg); }
  // #CAT_Flags set flag state on
  inline void           ClearChunkFlag(ChunkFlags flg)
  { flags = (ChunkFlags)(flags & ~flg); }
  // #CAT_Flags clear flag state (set off)
  inline bool           HasChunkFlag(ChunkFlags flg) const
  { return (flags & flg); }
  // #CAT_Flags check if flag is set
  inline void           SetChunkFlagState(ChunkFlags flg, bool on)
  { if(on) SetChunkFlag(flg); else ClearChunkFlag(flg); }
  // #CAT_Flags set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           ToggleChunkFlag(ChunkFlags flg)
  { SetChunkFlagState(flg, !HasChunkFlag(flg)); }
  // #CAT_Flags toggle flag

  inline float Compute_BaseAct(float time, float decay) {
    if(n_act == 0) base_act = 0.0f;
    else {
      base_act = std::log(n_act / (1.0f - decay)) - decay * std::log(time - t_new);
    }
    return base_act;
  }
  // #CAT_ActR compute the base-level activation as a function of current time and decay parameter

  virtual bool          SetSlotVal(const String& slot, const String& val);
  // set given slot to given value
  virtual ActrSlot*     FindSlot(const String& slot);
  // find slot by name -- issues error if not found

  virtual ActrSlot*     NewSlot();
  // #BUTTON make a new slot in this chunk
  virtual void          SetChunkType(ActrChunkType* ck_type);
  // #MENU #MENU_ON_Actions #DROP1 #DYN1 set chunk type for this chunk (can use drag-and-drop and call on a set of selected items)

  virtual bool          MatchesProd(ActrProduction& prod, ActrChunk* cmp,
                                    bool exact, bool why_not = false);
  // #CAT_ActR for production firing purposes: does this chunk match against the target comparison -- 'this' is assumed to be the production LHS 
  virtual bool          MatchesMem(ActrChunk* cmp, bool exact, bool why_not = false);
  // #CAT_ActR for memory matching purposes: does this chunk match against the target comparison
  virtual bool          MergeVals(ActrChunk* ck);
  // #CAT_ActR merge values from other chunk into this one (all non-nil from other chunk overwrite our values)
  virtual void          CopyName(ActrChunk* cp);
  // #CAT_ActR copy name from other chunk -- uses special new name for copy

  virtual bool  UpdateFromType();
  // ensure that this chunk is formatted properly based on the chunk_type -- automatically called in update-after-edit -- returns true if any changes were made -- preserves existing content even if slots change order etc, to greatest extent possible

  override taList_impl*	children_() {return &slots;}	
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const
  { return slots.Elem(idx, mode); }
  override String& Print(String& strm, int indent = 0) const;
  override String GetDisplayName() const;
  override String GetDesc() const;
  override String GetTypeDecoKey() const { return "ProgVar"; }
  override int    GetSpecialState() const;

  TA_SIMPLE_BASEFUNS(ActrChunk);
protected:
  void  UpdateAfterEdit_impl();

private:
  void Initialize();
  void Destroy();
};

#endif // ActrChunk_h
