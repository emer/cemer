// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef ObjDiffRec_h
#define ObjDiffRec_h 1

// parent includes:
#include <taBase>

// member includes:
#include <taSmartRefT>

// declare all other types mentioned but not required to include:
class ObjDiffRec_List; //
class FlatTreeEl; //

taTypeDef_Of(ObjDiffRec);

class TA_API ObjDiffRec : public taBase {
  // ##NO_TOKENS #CAT_ObjDiff object difference record 
INHERITED(taBase)
public:
  enum DiffFlags {              // #BITS
    DF_NONE     = 0x000000,     // nothing
    A_NOT_B     = 0x000001,     // elements exist in A but not B -- valid edits are to delete A or add from A into B
    B_NOT_A     = 0x000002,     // elements exist in B but not A -- valid edits are to delete B or add from B into A
    A_B_DIFF    = 0x000004,     // elements are in both A and B, but they are different -- valid edits are copy from A to B or B to A
    DIFF_PAR    = 0x000020,     // parent of lower-level diff
    DIFF_CTXT   = 0x000040,     // context for another diff
    ACT_DEL_A   = 0x000100,     // action to take: delete obj from a
    ACT_DEL_B   = 0x000200,     // action to take: delete obj from b
    ACT_ADD_A   = 0x000400,     // action to take: add obj to a after last b
    ACT_ADD_B   = 0x000800,     // action to take: add obj to b after last a
    ACT_COPY_AB = 0x001000,     // action to take: copy from a to paired diff_odr b
    ACT_COPY_BA = 0x002000,     // action to take: copy from paired diff_odr b to a
    DIFF_MASK = A_NOT_B | B_NOT_A | A_B_DIFF, // #NO_BIT
    DIFF_ADDEL = A_NOT_B | B_NOT_A,           // #NO_BIT
    ACT_MASK = ACT_DEL_A | ACT_DEL_B | ACT_ADD_A | ACT_ADD_B | ACT_COPY_AB | ACT_COPY_BA, // #NO_BIT
    SUB_NO_ACT = 0x100000,      // this is a sub-object of an add or delete and thus not something that an action can be driven from (just follows whatever the parent has selected)
    VAL_PATH_REL = 0x200000,    // value is a path relative to tab_obj, not a global path
  };

  DiffFlags     flags;          // flags for diff status
  int           diff_no;        // difference number -- indexes which original diff record this diff belongs to
  int           a_idx;          // index into ObjDiff.a_tree for corresponding source record -- or last good index if not here in case of B_NOT_A
  int           b_idx;          // index into ObjDiff.b_tree for corresponding source record -- or last good index if not here in case of A_NOT_B
  int           nest_level;     // our computed nest level
  FlatTreeEl*   a_src;          // source element for A (ObjDiff.a_tree record at a_idx)
  FlatTreeEl*   b_src;          // source element for B (ObjDiff.b_tree record at b_idx)
  ObjDiffRec*   par_rec;        // parent diff rec -- this is immediate parent of this record

  int           n_diffs;        // total number of diffs for this item and everything below it
  int           diff_no_start;  // for parent objects, first difference number this object participates in
  int           diff_no_end;    // for parent objects, ending difference number this object participates in
  void*         widget;         // points to the widget associated with this record

  inline void   SetDiffFlag(DiffFlags flg)   { flags = (DiffFlags)(flags | flg); }
  // #CAT_ObjectMgmt set data column flag state on
  inline void   ClearDiffFlag(DiffFlags flg) { flags = (DiffFlags)(flags & ~flg); }
  // #CAT_ObjectMgmt clear data column flag state (set off)
  inline bool   HasDiffFlag(DiffFlags flg) const { return 0 != (flags & flg);  }
  // #CAT_ObjectMgmt check if data column flag is set
  inline void   SetDiffFlagState(DiffFlags flg, bool on)
  { if(on) SetDiffFlag(flg); else ClearDiffFlag(flg); }
  // #CAT_ObjectMgmt set data column flag state according to on bool (if true, set flag, if false, clear it)

  inline bool   IsAnotB() const { return HasDiffFlag(A_NOT_B); }
  inline bool   IsBnotA() const { return HasDiffFlag(B_NOT_A); }
  inline bool   IsABDiff() const { return HasDiffFlag(A_B_DIFF); }
  inline bool   IsDiffPar() const { return HasDiffFlag(DIFF_PAR); }
  inline bool   IsDiffCtxt() const { return HasDiffFlag(DIFF_CTXT); }
  inline bool   IsDiff() const { return HasDiffFlag(DIFF_MASK); }
  inline bool   IsADDel() const { return HasDiffFlag(DIFF_ADDEL); }
  inline bool   HasAcl() const { return HasDiffFlag(ACT_MASK); }

  virtual bool  IsParentOf(FlatTreeEl* ael, FlatTreeEl* bel);
  // robust checking if this record is the parent of given a/b records -- either of these can be null if not relevant
  
  virtual bool  IsObj() const;
  // is either A or B record an object (obj.ptr() is set)
  virtual bool  IsNonMemberObj() const;
  // is either A or B record a sparate, non-member object
  
  virtual FlatTreeEl* Source() const;
  // returns source for A record unless B_NOT_A in which case it is B -- not definitive but if you just need to check a source
  
  virtual int   SrcNestLevel() const;
  // get nest level of this record according to sources -- uses A level when avail except for BnotA case

  virtual bool  NameContains(const String& nm);
  // does either a or b name contain given string
  virtual bool  ValueContains(const String& nm);
  // does either a or b value contain given string
  
  virtual bool  ActionAllowed() const;
  // is an action allowed for this item?  checks flags and types to make sure

  virtual bool  GetCurAction(int a_or_b, String& lbl) const;
  // get currently set action for this guy, depending on its flag status, and a_or_b (a=0, b=1) -- also fills in label describing action
  virtual void  SetCurAction(int a_or_b, bool on_off);
  // set action for this guy, depending on its flag status, and a_or_b (a=0, b=1)

  // ObjDiffRec* GetOwnTaBaseRec();
  // // find the closest record with a tabref set, starting with this record, and moving up the parents
  // taBase*       GetOwnTaBase();
  // // get the actual ta base pointer that owns this record

  String        GetDisplayName() const override;

  TA_BASEFUNS(ObjDiffRec);
private:
  void          Initialize();
  void          Destroy() { };
  void          Copy_(const ObjDiffRec& cp);
};

#endif // ObjDiffRec_h
