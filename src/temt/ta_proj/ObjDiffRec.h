// Copyright 2017, Regents of the University of Colorado,
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

taTypeDef_Of(ObjDiffRec);

class TA_API ObjDiffRec : public taBase {
  // ##NO_TOKENS #CAT_ObjDiff object difference record 
INHERITED(taBase)
public:
  enum DiffFlags {              // #BITS
    DF_NONE     = 0x000000,     // nothing
    OBJECTS     = 0x000001,     // diff about separate non-member objects on a list
    MEMBERS     = 0x000002,     // diff about members on an object
    VALUES      = 0x000004,     // diff about values -- from matrix, array
    PARENTS     = 0x000008,     // parents of diffs lower down
    CONTEXT_B2  = 0x000010,     // context for objects on lists, 2 items before diff
    CONTEXT_B1  = 0x000020,     // context for objects on lists, 1 item before diff
    CONTEXT_A1  = 0x000040,     // context for objects on lists, 1 item after diff
    CONTEXT_A2  = 0x000080,     // context for objects on lists, 1 item after diff
    A_NOT_B     = 0x000100,     // elements exist in A but not B -- valid edits are to delete A or add from A into B
    B_NOT_A     = 0x000200,     // elements exist in B but not A -- valid edits are to delete B or add from B into A
    A_B_DIFF    = 0x000400,     // elements are in both A and B, but they are different -- valid edits are copy from A to B or B to A
    ACT_DEL_A   = 0x001000,     // action: for AnotB, delete obj A from A
    ACT_DEL_B   = 0x002000,     // action: for BnotA, delete obj B from B
    ACT_ADD_A   = 0x004000,     // action: for AnotB, add A obj to B
    ACT_ADD_B   = 0x008000,     // action: for BnotA, add B obj to A
    ACT_COPY_AB = 0x010000,     // action: for ABdiff, copy from A to B
    ACT_COPY_BA = 0x020000,     // action: for ABdiff, copy from B to A
    DIFF_MASK = A_NOT_B | B_NOT_A | A_B_DIFF, // #NO_BIT
    DIFF_ADDEL = A_NOT_B | B_NOT_A,           // #NO_BIT
    ACT_MASK = ACT_DEL_A | ACT_DEL_B | ACT_ADD_A | ACT_ADD_B | ACT_COPY_AB | ACT_COPY_BA, // #NO_BIT
    CONTEXT = CONTEXT_B1 | CONTEXT_B2 | CONTEXT_A1 | CONTEXT_A2, // #NO_BIT
  };

  int           chunk;          // increasing index that is the same for a sequence of diffs affecting a sequential chunk of changes
  DiffFlags     flags;          // flags for diff status
  int           nest_level;     // our computed nest level
  int           a_idx;          // index on a, for list objects
  int           b_idx;          // index on b, for list objects
  taBase*       a_obj;          // A object -- this is immediate owner of member for MEMBERS, parent for PARENTS, and same as a_nonmemb_obj for OBJECTS
  taBase*       b_obj;          // B object -- this is immediate owner of member for MEMBERS, parent for PARENTS, and same as a_nonmemb_obj for OBJECTS
  MemberDef*    mdef;           // member def if member (can be NULL for DIFF_STRING!)
  String        a_val;          // value of a member
  String        b_val;          // value of b member
  taBaseRef     a_indep_obj;    // A independent, non-member object -- object that lives on a list and is not a member of another object
  taBaseRef     b_indep_obj;    // B independent, non-member object -- object that lives on a list and is not a member of another object
  ObjDiffRec*   par_rec;        // parent diff rec -- this is immediate parent of this record

  int           n_diffs;        // total number of diffs for this item and everything below it
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

  inline bool   IsObjects() const { return HasDiffFlag(OBJECTS); }
  inline bool   IsMembers() const { return HasDiffFlag(MEMBERS); }
  inline bool   IsValues() const { return HasDiffFlag(VALUES); }
  inline bool   IsParents() const { return HasDiffFlag(PARENTS); }
  inline bool   IsContext() const { return HasDiffFlag(CONTEXT); }
  inline bool   IsAnotB() const { return HasDiffFlag(A_NOT_B); }
  inline bool   IsBnotA() const { return HasDiffFlag(B_NOT_A); }
  inline bool   IsABDiff() const { return HasDiffFlag(A_B_DIFF); }
  inline bool   IsDiff() const { return HasDiffFlag(DIFF_MASK); }
  inline bool   IsADDel() const { return HasDiffFlag(DIFF_ADDEL); }
  inline bool   HasAct() const { return HasDiffFlag(ACT_MASK); }

  inline bool   IsAValid() const { return (IsBnotA() || (a_indep_obj.ptr() != NULL)); }
  // is the A record still valid (i.e., has the object not yet been deleted?)
  inline bool   IsBValid() const { return (IsAnotB() || (b_indep_obj.ptr() != NULL)); }
  // is the B record still valid (i.e., has the object not yet been deleted?)
  inline bool   IsValid()  const { return IsAValid() && IsBValid(); }
  // is the record still vailid -- both A and B components still valid

  inline bool   IsADisabled() const
  { return ((a_indep_obj.ptr() == NULL) || a_indep_obj->GetEnabled() == 0); }
  inline bool   IsBDisabled() const
  { return ((b_indep_obj.ptr() == NULL) || b_indep_obj->GetEnabled() == 0); }
  
  inline bool   IsIndepObj() const
  { return IsValid() && (IsObjects() || IsParents()) && mdef == NULL; }
  // is an independent object that can be viewed in viewer -- not a member of another obj
      
  virtual String AName() const;
  virtual String BName() const;
  virtual String AValue() const;
  virtual String BValue() const;
  virtual String ADecoKey() const;
  virtual String BDecoKey() const;
  
  virtual bool  NameContains(const String& nm) const;
  // does either a or b name contain given string
  virtual bool  ValueContains(const String& nm) const;
  // does either a or b value contain given string

  virtual bool  AMemberNoShow() const;
  virtual bool  BMemberNoShow() const;
  // is this a member field, and if so, does it have CONDSHOW OFF at the moment?
  virtual bool  AMemberNoEdit() const;
  virtual bool  BMemberNoEdit() const;
  // is this a member field, and if so, does it have CONDEDIT OFF at the moment?
  
  virtual bool  GetCurAction(int a_or_b, String& lbl) const;
  // get currently set action for this guy, depending on its flag status, and a_or_b (a=0, b=1) -- also fills in label describing action
  virtual void  SetCurAction(int a_or_b, bool on_off);
  // set action for this guy, depending on its flag status, and a_or_b (a=0, b=1)

  virtual bool  CompareRecObjs(const ObjDiffRec& cp) const;
  // compare two records in terms of object-level members -- return true if same, else false
  virtual bool  CompareRecsAll(const ObjDiffRec& cp) const;
  // compare two records on everything -- return true if same, else false
  
  String        GetDisplayName() const override;

  TA_BASEFUNS(ObjDiffRec);
private:
  void          Initialize();
  void          Destroy() { };
  void          Copy_(const ObjDiffRec& cp);
};

#endif // ObjDiffRec_h
