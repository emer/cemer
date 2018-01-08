// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef MemberDefBase_h
#define MemberDefBase_h 1

// parent includes:
#include <TypeItem>

// member includes:

// declare all other types mentioned but not required to include:
class MemberDefBase_List; //
class taiMember; //

taTypeDef_Of(MemberDefBase);

class TA_API MemberDefBase : public TypeItem {
  // #VIRT_BASE #NO_INSTANCE common subclass of MemberDef and PropertyDef
INHERITED(TypeItem)
public:
  enum MemberOpts { // #BITS member option settings -- cached, widely-used Opts values, mainly having to do with visibility etc
    HIDDEN        = 0x00000001, // hidden from the user by default -- for implementation or details the user does not otherwise need to see -- can still be seen by opening up the hidden items (for debugging implementation code) -- the gui version of protected: category -- cannot be combined with SHOW -- note, do NOT use NO_SHOW here -- use HIDDEN
    HIDDEN_INLINE = 0x00000002, // only hide this member during an inline edit -- when the members are being shown along one line of the editor in another class -- otherwise shown in a regular edit context
    READ_ONLY     = 0x00000004, // cannot be modified by the user in either the gui OR in css / programs -- by default ALSO hidden (see SHOW to override)
    GUI_READ_ONLY = 0x00000008, // only read-only for the gui -- CAN be modified in css / programs --  -- by default ALSO hidden (see SHOW to override)
    SHOW          = 0x00000010, // override default hidden state (for READ_ONLY cases) and show the item in the gui -- cannot be combined with HIDDEN
    EXPERT        = 0x00000020, // values that only an expert would care about -- these are generally hidden in a separate category from HIDDEN that can be optionally shown
    CONDSHOW      = 0x00000040, // has conditional showing logic where visibility depends on state of other members -- option starts with CONDSHOW_ -- CondShowCond() returns condition string
    CONDEDIT      = 0x00000080, // has conditional editability logic read-only vs. editable status depends on other members -- option starts with CONDEDIT_ -- CondEditCond() returns condition string
    TREE_HIDDEN   = 0x00000100, // hide this member from tree-browser views -- only applicable for taList_impl derived objects, which are the only ones shown in the tree by default
    TREE_SHOW     = 0x00000200, // show this member in tree-browser views when it otherwise would not be -- only applicable for taBase-derived objects -- only taList_impl shown by default so to show a non-taList object, you need this
    CONDTREE      = 0x00000400, // has conditional showing logic for tree browser -- option starts with CONDTREE_
    NO_SAVE       = 0x00000800, // do not save this member
    NO_FIND       = 0x00001000, // Don't search this member for the recursive FindMembeR function which searches recursively through objects (use if this ptr might point up in the hierarchy, which might cause an endless loop).
    NO_SEARCH     = 0x00002000, // Don't search this member for the recursive Search feature; typically used for .units and similar, with no significant findable properties and that would just clutter up search results
    NO_DIFF       = 0x00004000, // do not include in DiffCompare diff / patch functionality -- which also automatically excludes HIDDEN, READ_ONLY, NO_SAVE members
    OWN_POINTER   = 0x00008000, // we own the object pointed to by this pointer -- in general try to avoid doing this!
    NO_SET_POINTER= 0x00010000, // do not use the taBase::SetPointer method when setting this pointer
    
    IS_EDITOR_HIDDEN = 0x00040000, // computed value: has HIDDEN or [GUI_]READ_ONLY without SHOW
    IS_TREE_HIDDEN   = 0x00080000, // computed value: do not show in the tree -- note this is ON for DEF_CHILD members to prevent them from showing up in the tree an extra time
    IS_DEF_CHILD     = 0x00100000, // is this a default child member -- automatically visible
    IS_VISIBLE       = 0x00200000, // summary flag for whether this item is visible in some form or another, taking into account editor hidden, tree hidden and def child factors -- it should be used for filtering any kind of search or traversal process -- expert items ARE counted as visible so you need to check that separately if you want to exclude them
    OPTS_SET         = 0x10000000, // options have already been set -- don't redo!!
  };

  MemberDefBase_List*   owner;     // owning list -- owner of this list is a typedef
  TypeDef*              type;      // of this item
  String_PArray         inh_opts;  // inherited options ##xxx
  bool                  is_static; // true if this member is static
  MemberOpts            opt_flags; // cached values of option flags -- use these for more efficient access to commonly-used options
#ifdef TA_GUI
  taiMember*            im;        // gui structure for edit representation -- if this is a memberdef that is the storage for a property, then the im is assigned to the property
#endif

  // IMPORTANT: opt_flags is NOT valid during maketa process!
  
  inline void           SetOptFlag(MemberOpts typ)
  { opt_flags = (MemberOpts)(opt_flags | typ); }
  // set opt_flags state on
  inline void           ClearOptFlag(MemberOpts typ)
  { opt_flags = (MemberOpts)(opt_flags & ~typ); }
  // clear opt_flags state (set off)
  inline bool           HasOptFlag(MemberOpts typ) const
  { return (opt_flags & typ); }
  // check if opt_flags is set
  inline void           SetOptFlagsState(MemberOpts typ, bool on)
  { if(on) SetOptFlag(typ); else ClearOptFlag(typ); }
  // set opt_flags state according to on bool (if true, set opt_flags, if false, clear it)

  // NOTE: Has indicates a literal check for the option, while Is does the typical logic -- in general better to use Is cases
  inline bool  HasHidden() const { return HasOptFlag(HIDDEN); }
  inline bool  HasHiddenInline() const { return HasOptFlag(HIDDEN_INLINE); }
  inline bool  HasReadOnly() const { return HasOptFlag(READ_ONLY); }
  inline bool  HasGuiReadOnly() const { return HasOptFlag(GUI_READ_ONLY); }
  inline bool  HasShow() const { return HasOptFlag(SHOW); }
  inline bool  HasExpert() const { return HasOptFlag(EXPERT); }
  inline bool  HasNoSave() const { return HasOptFlag(NO_SAVE); }
  inline bool  HasTreeHidden() const { return HasOptFlag(TREE_HIDDEN); }
  inline bool  HasTreeShow() const { return HasOptFlag(TREE_SHOW); }
  inline bool  HasCondShow() const { return HasOptFlag(CONDSHOW); }
  inline bool  HasCondEdit() const { return HasOptFlag(CONDEDIT); }
  inline bool  HasCondTree() const { return HasOptFlag(CONDTREE); }
  inline bool  HasNoFind() const { return HasOptFlag(NO_FIND); }
  inline bool  HasNoSearch() const { return HasOptFlag(NO_SEARCH); }
  inline bool  HasNoDiff() const { return HasOptFlag(NO_DIFF); }
  inline bool  HasOwnPointer() const { return HasOptFlag(OWN_POINTER); }
  inline bool  HasNoSetPointer() const { return HasOptFlag(NO_SET_POINTER); }

  inline bool  IsGuiReadOnly() const { return HasReadOnly() || HasGuiReadOnly(); }
  inline bool  IsEditorHidden() const { return HasOptFlag(IS_EDITOR_HIDDEN); }
  inline bool  IsTreeHidden() const { return HasOptFlag(IS_TREE_HIDDEN); }
  inline bool  IsDefChild() const { return HasOptFlag(IS_DEF_CHILD); }
  inline bool  IsVisible() const { return HasOptFlag(IS_VISIBLE); }
  inline bool  IsInvisible() const { return !IsVisible(); }
  inline bool  IsStatic() const { return is_static; }
  
  void*        This() override {return this;}
  TypeDef*     GetTypeDef() const override {return &TA_MemberDefBase;}
  virtual bool ValIsDefault(const void* base) const = 0;
  // true if the member contains its default value, either DEF_ or the implicit default

  void          Copy(const MemberDefBase& cp);
  void          Copy(const MemberDefBase* cp); // this is a "pseudo-virtual" type guy, that will copy a like source (Member or Property)
  TypeDef*     GetOwnerType() const override;
  MemberDefBase();
  MemberDefBase(const String& nm);
  MemberDefBase(TypeDef* ty, const String& nm, const String& dsc, const String& op,
    const String& lis, bool is_stat = false);
  MemberDefBase(const MemberDefBase& cp);
  ~MemberDefBase();

  virtual const Variant GetValVar(const void* base) const = 0;
  virtual void  SetValVar(const Variant& val, void* base,
    void* par = NULL) = 0;

  bool          CheckList(const String_PArray& lst) const;
  // check if member has a list in common with given one

  virtual void  InitOptsFlags();
  // initialize options flags, computed during type initialization at startup

private:
  void          Initialize();
  void          Copy_(const MemberDefBase& cp);
};

#endif // MemberDefBase_h
