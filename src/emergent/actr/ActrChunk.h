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

eTypeDef_Of(ActrActVals);

class E_API ActrActVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_ActR actr activation values associated with a given chunk
INHERITED(taOBase)
public:
  float                 act;     // overall chunk activation = act_base + act_spread + act_match + act_noise + inst_noise
  float                 base;   // base level activation = ln(n_act / (1-d)) - d * ln(time - t_new) where time = current time and d = decay parameter (optimized calculation)
  float                 spread; // spreading activation through associative strengths from chunks in active buffers
  float                 match;  // activation derived from partial matching
  float                 noise;  // permanent noise established when chunk created
  
  inline float    ComputeAct(float inst_noise) {
    act = base + spread + match + noise + inst_noise;
    return act;
  }
  // compute the overall activation from all of the activation components

  inline void     InitAct() {
    act = base = spread = match = noise = 0.0f;
  }
  // initialize activations
  
  TA_SIMPLE_BASEFUNS(ActrActVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(ActrActTimeVals);

class E_API ActrActTimeVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_ActR actr activation timing values associated with a given chunk
INHERITED(taOBase)
public:
  float                 n_act;      // number of times chunk has been activated
  float                 t_new;      // time when chunk was created
  float                 t_act;      // time when chunk was last activated
  
  TA_SIMPLE_BASEFUNS(ActrActTimeVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(ActrChunk);

class E_API ActrChunk : public taNBase {
  // ##INSTANCE ##EDIT_INLINE ##CAT_ActR ##SCOPE_ActrModel ##DEF_CHILD_slots ##DEF_CHILDNAME_SlotVals a single chunk of memory in ActR
INHERITED(taNBase)
public:
  enum ChunkFlags { // #BITS ActR chunk flags
    CF_NONE             = 0, // #NO_BIT
    RETRIEVED           = 0x0001, // this chunk was just retrieved on last retrieval
    ELIGIBLE            = 0x0002, // this chunk was eligible to be retrieved on last retrieval
    RECENT              = 0x0004, // this chunk was recently activated or retrieved -- i.e., has a finger-of-instantiation (finst)
    COND                = 0x0010, // this chunk lives in a condition of a production
    ACT                 = 0x0020, // this chunk lives in an action of a production
    ALL_STATE_FLAGS     = RETRIEVED | ELIGIBLE | RECENT, // #NO_BIT all the DM state flags
  };

  ActrChunkTypeRef      chunk_type; // the type of this chunk -- enforces our structure to match
  ChunkFlags            flags;      // #CONDSHOW_OFF_flags:COND,ACT #READ_ONLY #SHOW flags indicating state of the chunk
  ActrActVals           act;        // #CONDSHOW_OFF_flags:COND,ACT #READ_ONLY #SHOW activation values for the chunk
  ActrActTimeVals       time;       // #CONDSHOW_OFF_flags:COND,ACT #READ_ONLY #SHOW activation timing values for the chunk -- when created, accessed, etc
  ActrSlot_List         slots;      // #NO_EXPAND_ALL #SHOW_TREE the slot values -- same number as slots in chunk_type

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

  inline void   ChunkActivated() { time.n_act += 1.0f; }
  // #CAT_ActR chunk was activated -- update its activation count
  inline void   InitChunk(float cur_time) 
  { time.t_new = cur_time; time.n_act = 1.0f; time.t_act = -1.0f;
    ClearChunkFlag(ALL_STATE_FLAGS); }
  // #CAT_ActR initialize the chunk as a new chunk (e.g., for declarative memory)
  void          ResetChunk();
  // #CAT_ActR completely reset a chunk to null everything

  inline float ComputeBaseAct(float cur_t, float decay) {
    if(time.n_act == 0) act.base = 0.0f;
    else {
      act.base = std::log(time.n_act / (1.0f - decay))
        - decay * std::log(cur_t - time.t_new);
    }
    return act.base;
  }
  // #CAT_ActR compute the base-level activation as a function of current time and decay parameter

  virtual  bool         SetSlotValLiteral(const String& slot, const String& val);
  // #CAT_ActR set given slot to given literal value -- error if not a LITERAL type
  inline bool           SetSlotVal(const String& slot, const String& val)
  { return SetSlotValLiteral(slot, val); }
  // #CAT_ActR set given slot to given literal value -- error if not a LITERAL type
  virtual bool          SetSlotValChunk(const String& slot, ActrChunk* ck);
  // #CAT_ActR set given slot to given chunk value -- error if not a CHUNK type
  virtual ActrSlot*     FindSlot(const String& slot);
  // #CAT_ActR find slot by name -- issues error if not found
  virtual Variant       GetSlotVal(const String& slot);
  // #CAT_ActR get slot value as a variant -- either is a string or points to a chunk or is nil if not found (err emitted)
  virtual String        GetSlotValLiteral(const String& slot);
  // #CAT_ActR get slot value as a literal string value -- emits an error if slot type is a chunk pointer
  virtual ActrChunk*    GetSlotValChunk(const String& slot);
  // #CAT_ActR get slot value as a pointer to a chunk -- emits error if slot type is a literal

  virtual ActrSlot*     NewSlot();
  // #BUTTON make a new slot in this chunk
  virtual void          SetChunkType(ActrChunkType* ck_type);
  // #MENU #MENU_ON_Actions #DROP1 #DYN1 set chunk type for this chunk (can use drag-and-drop and call on a set of selected items)
  virtual void          SetChunkTypeName(const String& ck_type);
  // #MENU #MENU_ON_Actions set chunk type for this chunk to given name

  virtual bool          MatchesProd(ActrProduction& prod, ActrChunk* cmp,
                                    bool exact, bool why_not = false);
  // #CAT_ActR for production firing purposes: does this chunk match against the target comparison -- 'this' is assumed to be the production LHS 
  virtual bool          MatchesMem(ActrChunk* cmp, bool exact, bool why_not = false);
  // #CAT_ActR for memory matching purposes: does this chunk match against the target comparison
  virtual bool          MergeVals(ActrChunk* ck);
  // #CAT_ActR merge values from other chunk into this one (all non-nil from other chunk overwrite our values)
  virtual void          CopyName(ActrChunk* cp);
  // #CAT_ActR copy name from other chunk -- uses special new name for copy

  virtual bool          UpdateFromType();
  // ensure that this chunk is formatted properly based on the chunk_type -- automatically called in update-after-edit -- returns true if any changes were made -- preserves existing content even if slots change order etc, to greatest extent possible

  inline bool          InheritsFromCT(ActrChunkType* par)
  { if(!chunk_type) return false; return chunk_type->InheritsFromCT(par); }
  // #CAT_ActR determine if this chunk type inherits from given parent chunk type, including being the same type itself
  inline bool          InheritsFromCTName(const String& chunk_type_nm)
  { if(!chunk_type) return false; return chunk_type->InheritsFromCTName(chunk_type_nm); }
  // #CAT_ActR determine if this chunk type inherits from given parent chunk type name, including being the same type itself
  virtual ActrChunkType* CommonChunkType(ActrChunkType* other)
  { if(!chunk_type) return NULL; return chunk_type->CommonChunkType(other); }
  // #CAT_ActR returns the highest common chunk type between this chunk type and the other -- NULL if they are not related


  override taList_impl*	children_() {return &slots;}	
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const
  { return slots.Elem(idx, mode); }
  override String& Print(String& strm, int indent = 0) const;
  override String GetDisplayName() const;
  override String GetDesc() const;
  override String GetTypeDecoKey() const { return "ProgVar"; }
  override int    GetSpecialState() const;

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(ActrChunk);
protected:
  void  UpdateAfterEdit_impl();

private:
  SIMPLE_COPY(ActrChunk);
  void Initialize();
  void Destroy();
};

#endif // ActrChunk_h
