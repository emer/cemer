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

#ifndef taObjDiffRec_h
#define taObjDiffRec_h 1

// parent includes:
#include <taRefN>

// member includes:
#include <taString>
#include <taHashEl>

// declare all other types mentioned but not required to include:
class taObjDiff_List; //
class taBase; //

class TA_API taObjDiffRec : public taRefN {
  // ##INSTANCE ##NO_TOKENS ##NO_MEMBERS ##NO_CSS ##MEMB_NO_SHOW_TREE TA object difference record -- records information about objects for purposes of diffing object structures
INHERITED(taRefN)
public:
  enum DiffFlags {              // #BITS
    DF_NONE     = 0x000000,     // nothing
    SRC_A       = 0x000001,     // comes from a items
    SRC_B       = 0x000002,     // comes from b items
    DIFF_DEL    = 0x000004,     // diff edit = delete from a, also marks corresponding point in b where a del starts
    DIFF_ADD    = 0x000008,     // diff edit = add from b to a, also marks corresponding point in a where b add starts
    DIFF_CHG    = 0x000010,     // diff edit = change from a to b
    DIFF_PAR    = 0x000020,     // parent of lower-level diff
    DIFF_CTXT   = 0x000040,     // context for another diff
    ACT_DEL_A   = 0x000100,     // action to take: delete obj from a
    ACT_DEL_B   = 0x000200,     // action to take: delete obj from b
    ACT_ADD_A   = 0x000400,     // action to take: add obj to a after paired diff_odr
    ACT_ADD_B   = 0x000800,     // action to take: add obj to b after paired diff_odr
    ACT_COPY_AB = 0x001000,     // action to take: copy from a to paired diff_odr b
    ACT_COPY_BA = 0x002000,     // action to take: copy from paired diff_odr b to a
#ifndef __MAKETA__
    DIFF_MASK = DIFF_DEL | DIFF_ADD | DIFF_CHG,
    DIFF_ADDEL = DIFF_DEL | DIFF_ADD,
    ACT_MASK = ACT_DEL_A | ACT_DEL_B | ACT_ADD_A | ACT_ADD_B | ACT_COPY_AB | ACT_COPY_BA,
#endif
    SUB_NO_ACT = 0x100000,      // this is a sub-object of an add or delete and thus not something that an action can be driven from (just follows whatever the parent has selected)
    VAL_PATH_REL = 0x200000,    // value is a path relative to tab_obj, not a global path
  };

  DiffFlags     flags;          // flags for diff status

  taObjDiff_List* owner;        // the owner of this one
  int           idx;            // the index number in owning list
  int           nest_level;     // how deeply nested or embedded is this object in the obj hierarchy
  int           n_diffs;        // total number of diffs for this item and everything below it
  int           diff_no;        // difference number -- indexes which original diff record this diff belongs to
  int           diff_no_start;  // for parent objects, first difference number this object participates in
  int           diff_no_end;    // for parent objects, ending difference number this object participates in
  String        name;           // object name (member name, object type) -- this is used in diffing and is not always best for display -- see GetDisplayName()
  String        value;          // string representation of this object
  taHashVal     hash_code;      // hash-code of name&value + nest_level -- this is what diff is based on
  TypeDef*      type;           // type of this object (same as mdef->type if a member) -- not type of parent
  MemberDef*    mdef;           // memberdef if this is a member of a parent object
  void*         addr;           // address in memory of this object
  void*         par_addr;       // address in memory of parent of this object, if a member of a containing object
  TypeDef*      par_type;       // type of parent object, if a member of a containing object
  taObjDiffRec* par_odr;        // parent diff record
  taObjDiffRec* diff_odr;       // paired diff record from other source
  void*         tabref;         // when this is used, it is a taBaseRef*
  void*         widget;         // points to the widget associated with this record

  inline void           SetDiffFlag(DiffFlags flg)   { flags = (DiffFlags)(flags | flg); }
  // #CAT_ObjectMgmt set data column flag state on
  inline void           ClearDiffFlag(DiffFlags flg) { flags = (DiffFlags)(flags & ~flg); }
  // #CAT_ObjectMgmt clear data column flag state (set off)
  inline bool           HasDiffFlag(DiffFlags flg) const {
    return 0 != (flags & flg);
  }
  // #CAT_ObjectMgmt check if data column flag is set
  inline void           SetDiffFlagState(DiffFlags flg, bool on)
  { if(on) SetDiffFlag(flg); else ClearDiffFlag(flg); }
  // #CAT_ObjectMgmt set data column flag state according to on bool (if true, set flag, if false, clear it)

  String        GetDisplayName();
  // returns a name suitable for gui display purposes -- taBase->GetDisplayName else name

  bool          ActionAllowed();
  // is an action allowed for this item?  checks flags and types to make sure

  bool          GetCurAction(int a_or_b, String& lbl);
  // get currently set action for this guy, depending on its flag status, and a_or_b (a=0, b=1) -- also fills in label describing action
  void          SetCurAction(int a_or_b, bool on_off);
  // set action for this guy, depending on its flag status, and a_or_b (a=0, b=1)

  void          GetValue(taObjDiff_List& odl);
  // gets the value and hash code (and name) fields based on the other information already set -- also uses information on overall obj diff list (tab_obj_a for paths)

  void          ComputeHashCode();
  // computes the hash code based on name + & + value + nest_level -- called by GetValue, but call this manually if anything changes

#ifndef NO_TA_BASE
  taObjDiffRec* GetOwnTaBaseRec();
  // find the closest record with a tabref set, starting with this record, and moving up the parents
  taBase*       GetOwnTaBase();
  // get the actual ta base pointer that owns this record
  String        GetTypeDecoKey();
  // get the decoration key for coloring this record -- from ta base
#endif

  taObjDiffRec();
  taObjDiffRec(taObjDiff_List& odl, int nest, TypeDef* td, MemberDef* md, void* adr, void* par_adr = NULL,
               TypeDef* par_typ = NULL, taObjDiffRec* par_od = NULL);
  // this is the main interface for making a new item -- sets info and calls GetValue

  taObjDiffRec(const taObjDiffRec& cp); // copy constructor
  ~taObjDiffRec();

  taObjDiffRec* Clone()         { return new taObjDiffRec(*this); }
  taObjDiffRec* MakeToken()     { return new taObjDiffRec(); }
  void          Copy(const taObjDiffRec& cp);

private:
  void          Initialize();
  void          Copy_(const taObjDiffRec& cp);
};

#endif // taObjDiffRec_h
