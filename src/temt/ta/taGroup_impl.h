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

#ifndef taGroup_impl_h
#define taGroup_impl_h 1

// parent includes:
#include <taList_impl>

// member includes:
#include <taSubGroup>
#include <taLeafItr>

// declare all other types mentioned but not required to include:
class MemberDef; // 
class ControlPanel; // 
class taBase; // 
class TypeDef; // 

#ifdef SC_DEFAULT
#undef SC_DEFAULT
#endif

taTypeDef_Of(taGroup_impl);

class TA_API taGroup_impl : public taList_impl {
  // #INSTANCE #STEM_BASE implementation of a group
INHERITED(taList_impl)
public:
  static bool    def_nw_item; // #IGNORE default
  virtual taGroup_impl* GetSuperGp_();                 // #IGNORE Parent super-group, or NULL
  virtual void   UpdateLeafCount_(int no);      // #IGNORE updates the leaves count

public:
  int           leaves;         // #READ_ONLY #NO_SAVE #CAT_taList total number of leaves
  taSubGroup    gp;             // #NO_SHOW #NO_FIND #NO_SAVE #CAT_taList sub-groups within this one
  taGroup_impl*        super_gp;       // #READ_ONLY #NO_SHOW #NO_SAVE #NO_SET_POINTER #CAT_taList super-group above this
  taGroup_impl*        root_gp;        // #READ_ONLY #NO_SHOW #NO_SAVE #NO_SET_POINTER #CAT_taList the root group, 'this' for root group itself; never NULL

  bool          IsEmpty() const { return (leaves == 0) ? true : false; }
  bool          IsRoot() const  { return (root_gp == this); } // 'true' if this is the root

  ////////////////////////////////////////////////
  //    functions that return the type          //
  ////////////////////////////////////////////////

#ifndef __MAKETA__

  taGroup_impl* Gp_(int i) const { // #IGNORE
    return gp.SafeEl(i);
  }

  taGroup_impl* FastGp_(int i) const { // #IGNORE
    return gp.FastEl(i);
  }

  virtual taBase* Leaf_(int idx) const; // #IGNORE DFS through all subroups for leaf i

  taGroup_impl* FastLeafGp_(int gp_idx) const { // #IGNORE the flat leaf group, note: 0 is "this"
    if (gp_idx == 0)
      return const_cast<taGroup_impl*>(this);
    if (!leaf_gp)
      InitLeafGp();
    return (taGroup_impl*)leaf_gp->el[gp_idx];
  }

  taGroup_impl* SafeLeafGp_(int gp_idx) const; // #IGNORE the flat leaf group, note: 0 is "this"

  // iterator-like functions
  taGroup_impl* FirstGp_(int& g) const { // #IGNORE first sub-gp
    g = 0;
    if (leaf_gp == NULL)
      InitLeafGp();
    return leaf_gp->SafeEl(0);
  }

  taGroup_impl* LastGp_(int& g) const { // #IGNORE last sub-gp (for rev iter)
    if (leaf_gp == NULL)
      InitLeafGp();
    g = leaf_gp->size - 1;
    return leaf_gp->Peek();
  }

  taGroup_impl* NextGp_(int& g) const { // #IGNORE next sub-gp
    return leaf_gp->SafeEl(++g);
  }

  int LeafGpCount() const { // #IGNORE count of leaf groups **including self**; optimized for no subgroups
    if (gp.size == 0)
      return 1;
    if (leaf_gp == NULL)
      InitLeafGp();
    return leaf_gp->size;
  }

  taBase* FirstEl_(taLeafItr& lf) const { // #IGNORE first leaf iter init
    taBase* rval=NULL;
    lf.i = 0;
    lf.cgp = FirstGp_(lf.g);
    if (lf.cgp != NULL)
      rval = (taBase*)lf.cgp->el[0];
    return rval;
  }

  inline taBase* FirstEl(taLeafItr& lf) const { // #IGNORE
    return FirstEl_(lf);
  }

  taBase* NextEl_(taLeafItr& lf) const { // #IGNORE next leaf
    if (!lf.cgp) {
      // then do what?? return NULL?
    }
    if (++lf.i >= lf.cgp->size) {
      lf.i = 0;
      lf.cgp = leaf_gp ? leaf_gp->SafeEl(++lf.g) : 0;
      if (!lf.cgp) return NULL;
    }
    return (taBase*)lf.cgp->el[lf.i];
  }

  inline taBase* NextEl(taLeafItr& lf) const { // #IGNORE
    return NextEl_(lf);
  }

  taBase* LastEl_(taLeafItr& lf) const { // #IGNORE last leaf iter init
    lf.cgp = LastGp_(lf.g);
    if (!lf.cgp) {
      return NULL;
    }
    lf.i = lf.cgp->size - 1;
    return (taBase*)lf.cgp->el[lf.i];
  }

  inline taBase* LastEl(taLeafItr& lf) const { // #IGNORE
    return LastEl_(lf);
  }

  taBase* PrevEl_(taLeafItr& lf) const { // #IGNORE prev leaf -- delete item safe
    if (--lf.i < 0) {
      if (leaf_gp == NULL) {
        InitLeafGp(); // in case we did a delete of an item
      }
      lf.cgp = leaf_gp->SafeEl(--lf.g);
      if (!lf.cgp) {
        return NULL;
      }
      lf.i = lf.cgp->size - 1;
    }
    return (taBase*)lf.cgp->el[lf.i];
  }

  inline taBase* PrevEl(taLeafItr& lf) const { // #IGNORE
    return PrevEl_(lf);
  }

#endif

  taBase* ElemLeaf(int leaf_idx) const override { return Leaf_(leaf_idx); }
  int     ElemCount() const            override { return leaves; }

  virtual taGroup_impl*  NewGp_(int no, TypeDef* typ=NULL, const String& name_ = "");
    // #IGNORE create sub groups
  virtual taBase* NewEl_(int no, TypeDef* typ=NULL);    // #IGNORE create items

  taGroup_impl*          NewGp_gui(int n_gps=1, TypeDef* typ=NULL,
                                   const String& name="");
  // #BUTTON #MENU #MENU_ON_Object #MENU_CONTEXT #TYPE_this #NULL_OK_typ #NULL_TEXT_SameType #LABEL_NewGroup #NO_SAVE_ARG_VAL #CAT_Modify Create and add n_gps new sub group(s) of given type (typ=NULL: same type as this group)

  virtual taBase* FindLeafName_(const String& it) const;        // #IGNORE
  virtual taBase* FindLeafNameContains_(const String& it) const;        // #IGNORE
  virtual taBase* FindLeafType_(TypeDef* it) const;     // #IGNORE
  virtual taBase* FindLeafNameType_(const String& it) const;    // #IGNORE

  virtual int   FindLeafNameIdx(const String& item_nm) const;
  // #CAT_Access Find element anywhere in full group and subgroups with given name (item_nm)
  virtual int   FindLeafNameContainsIdx(const String& item_nm) const;
  // #MENU #MENU_ON_Edit #USE_RVAL #ARGC_1 #LABEL_Find #CAT_Access Find anywhere in full group and subgroups first element whose name contains given name (item_nm)
  virtual int   FindLeafTypeIdx(TypeDef* item_tp) const;
  // #CAT_Access find anywhere in full group and subgroups given type leaf element (NULL = not here)
  virtual int   FindLeafNameTypeIdx(const String& item_nm) const;
  // #CAT_Access Find anywhere in full group and subgroups element with given object name or type name (item_nm)

  virtual taGroup_impl* FindMakeGpName(const String& gp_nm, TypeDef* typ=NULL,
    bool& nw_item=def_nw_item);
  // #IGNORE find subgroup of given name; if it doesn't exist, then make it (using type if specified, else default type for subgroup)

  ////////////////////////////////////////////////
  // functions that don't depend on the type    //
  ////////////////////////////////////////////////

  virtual void  InitLeafGp() const;
  // #CAT_Access Initialize the leaf group iter list, always ok to call
  virtual void  InitLeafGp_impl(taGroup_List* lg) const; // #IGNORE impl of init leaf gp
  virtual void  AddOnly_(void* it);             // #IGNORE update leaf count

  virtual bool  RemoveLeafEl(taBase* item);
  // #CAT_Modify remove given leaf element
  virtual bool  RemoveLeafName(const char* item_nm);
  // #CAT_Modify remove given named leaf element
  virtual bool  RemoveLeafIdx(int idx);
  // #CAT_Modify Remove leaf element at leaf index
  virtual void  RemoveAll();
  // #CAT_Modify Remove all elements of the group

  virtual bool  RemoveGpIdx(int idx)
  { return gp.RemoveIdx(idx); }
  // #CAT_Modify remove group at given index
  virtual bool  RemoveGpEl(taGroup_impl* group)
  { return gp.RemoveEl(group); }
  // #MENU #FROM_GROUP_gp #MENU_ON_Edit #CAT_Modify remove given group
  virtual taGroup_List* EditSubGps()
  { return &gp; }
  // #MENU #USE_RVAL #CAT_Access edit the list of sub-groups (e.g., so you can move around subgroups)

  virtual void  EnforceLeaves(int sz);
  // #CAT_Modify ensure that sz leaves exits by adding new ones to top group and removing old ones from end
  void  EnforceSameStru(const taGroup_impl& cp);
  // #CAT_Modify enforce this group to have same structure as cp

  int   ReplaceType(TypeDef* old_type, TypeDef* new_type) override;

  virtual int    FindLeafEl(taBase* item) const;
  // #CAT_Access find given leaf element -1 = not here.

  void  SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;

  void* FindMembeR(const String& nm, MemberDef*& ret_md) const override;

  // IO routines
  String GetValStr(void* par = NULL, MemberDef* md = NULL,
                            TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                            bool force_inline = false) const override;
  bool  SetValStr(const String& val, void* par = NULL, MemberDef* md = NULL,
                           TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                           bool force_inline = false) override;
  int   ReplaceValStr(const String& srch, const String& repl,
                              const String& mbr_filt,
                              void* par = NULL, TypeDef* par_typ=NULL,
                              MemberDef* md = NULL,
                              TypeDef::StrContext sc = TypeDef::SC_DEFAULT) override;

  taObjDiffRec* GetObjDiffVal(taObjDiff_List& odl, int nest_lev,
                              MemberDef* memb_def=NULL, const void* par=NULL,
                              TypeDef* par_typ=NULL, taObjDiffRec* par_od=NULL) const override;

  void Dump_Save_GetPluginDeps() override; // note: in ta_dump.cpp
#ifndef __MAKETA__
  int  Dump_SaveR(std::ostream& strm, taBase* par=NULL, int indent=0) override;
  int  Dump_Save_PathR(std::ostream& strm, taBase* par=NULL, int indent=0) override;
  int  Dump_Save_PathR_impl(std::ostream& strm, taBase* par=NULL, int indent=0) override;
#endif

  void Search_impl(const String& srch, taBase_PtrList& items,
                            taBase_PtrList* owners = NULL,
                            bool contains = true, bool case_sensitive = false,
                            bool obj_name = true, bool obj_type = true,
                            bool obj_desc = true, bool obj_val = true,
                            bool mbr_name = true, bool type_desc = false) override;
  void CompareSameTypeR(Member_List& mds, TypeSpace& base_types,
                           voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                           taBase* cp_base,
                           int show_forbidden=TypeItem::NO_HIDDEN,
                           int show_allowed=TypeItem::SHOW_CHECK_MASK, 
                           bool no_ptrs = true) override;
  int  UpdatePointers_NewPar(taBase* old_par, taBase* new_par) override;
  int  UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) override;
  int  UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) override;
  int  UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr) override;
  int  AddToControlPanelSearch(const String& memb_contains, ControlPanel*& editor) override; //

  bool         ChildCanDuplicate(const taBase* chld, bool quiet = true) const override;
  taBase*      ChildDuplicate(const taBase* chld) override;

  virtual void  Duplicate(const taGroup_impl& cp);
  virtual void  DupeUniqNameOld(const taGroup_impl& cp);
  virtual void  DupeUniqNameNew(const taGroup_impl& cp);

  virtual void  Borrow(const taGroup_impl& cp);
  virtual void  BorrowUnique(const taGroup_impl& cp);
  virtual void  BorrowUniqNameOld(const taGroup_impl& cp);
  virtual void  BorrowUniqNameNew(const taGroup_impl& cp);

  virtual void  Copy_Common(const taGroup_impl& cp);
  virtual void  Copy_Duplicate(const taGroup_impl& cp);
  virtual void  Copy_Borrow(const taGroup_impl& cp);

  String& Print(String& strm, int indent=0) const;

  void  InitLinks();            // inherit the el_typ from parent group..
  void  CutLinks();
  TA_BASEFUNS(taGroup_impl);
private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const taGroup_impl& cp);
protected:
  mutable taGroup_List*  leaf_gp; // #READ_ONLY #NO_SAVE cached 'flat' list of leaf-containing-gps for iter
  void         CanCopy_impl(const taBase* cp_fm, bool quiet,
    bool& ok, bool virt) const override;
  void         CheckChildConfig_impl(bool quiet, bool& rval) override;
  void         ItemRemoved_() override; // update the leaf counts (supercursively)
  taBase*      New_impl(int n_objs, TypeDef* typ, const String& name_) override;

  virtual taGroup_impl* LeafGp_(int leaf_idx) const; // #IGNORE the leaf group containing leaf item -- **NONSTANDARD FUNCTION** put here to try to flush out any use
#ifdef TA_GUI
protected: // clip functions
  void ChildQueryEditActions_impl(const MemberDef* md,
    const taBase* child, const taiMimeSource* ms,
    int& allowed, int& forbidden) override;
  virtual void  ChildQueryEditActionsG_impl(const MemberDef* md,
    taGroup_impl* subgrp, const taiMimeSource* ms,
    int& allowed, int& forbidden);
  int  ChildEditAction_impl(const MemberDef* md, taBase* child,
    taiMimeSource* ms, int ea) override;
    // if child or ms is a group, dispatch to new G version
  virtual int   ChildEditActionGS_impl(const MemberDef* md, taGroup_impl* subgrp, int ea);
  virtual int   ChildEditActionGD_impl_inproc(const MemberDef* md,
    taGroup_impl* subgrp, taiMimeSource* ms, int ea);
  virtual int   ChildEditActionGD_impl_ext(const MemberDef* md,
    taGroup_impl* subgrp, taiMimeSource* ms, int ea);
#endif
};

#endif // taGroup_impl_h
