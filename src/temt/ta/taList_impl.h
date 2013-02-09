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

#ifndef taList_impl_h
#define taList_impl_h 1

// parent includes:
#include <taNBase>
#include <taPtrList_base>

// for smart ptrs need these:
#include <taSmartPtrT>
#include <taSmartRefT>

// member includes:
#include <cassert>

// declare all other types mentioned but not required to include:
class taHashTable; // 
class TypeDef; // 
class taMatrix; // 
class taBaseItr; // 
class MemberDef; // 
class taObjDiff_List; // 
class taDoc; // 
class SelectEdit; // 
class MethodDef; // 
class UserDataItem; // 
class UserDataItem_List; // 
class UserDataItemBase; // 


taTypeDef_Of(taList_impl);

class TA_API taList_impl : public taNBase, public taPtrList_base<taBase> {
  // #INSTANCE #NO_TOKENS #STEM_BASE ##MEMB_HIDDEN_EDIT ##HIDDEN_INLINE implementation for a taBase list class
#ifndef __MAKETA__
private:
typedef taNBase inherited; // for the boilerplate code
typedef taNBase inherited_taBase;
typedef taPtrList_base<taBase> inherited_taPtrList;
#endif
public:
  TypeDef*      el_base;        // #EXPERT #NO_SHOW_TREE #READ_ONLY_GUI #NO_SAVE #CAT_taList Base type for objects in group
  TypeDef*      el_typ;         // #TYPE_ON_el_base #NO_SHOW_TREE #CAT_taList Default type for objects in group
  int           el_def;         // #EXPERT #CAT_taList Index of default element in group
  taBasePtr     el_view;        // #EXPERT #NO_SAVE #CAT_taList matrix with indicies providing view into items in this list, if set -- determines the items and the order in which they are presented for the iteration operations -- otherwise ignored in other contexts
  IndexMode     el_view_mode;   // #EXPERT #NO_SAVE #CAT_taList what kind of information is present in el_view to determine view mode -- only valid cases are IDX_COORDS and IDX_MASK

  override TypeDef*     GetElType() const {return el_typ;}
  // #IGNORE Default type for objects in group
  override TypeDef*     El_GetType_(void* it) const
  { return ((taBase*)it)->GetTypeDef(); } // #IGNORE
  override taList_impl* children_() {return this;}

  virtual Variant       VarEl(int idx) const;
  // #CAT_Access #EXPERT get element at index as a Variant -- does safe range checking -- if index is negative, access is from the back of the list (-1 = last item, -2 = second to last, etc) - this uses LeafElem interface and is thus fully generic
  virtual taBase*       ElemLeaf(int leaf_idx) const { return (taBase*)SafeEl_(leaf_idx); }
  // #CAT_Access #EXPERT return a terminal (leaf) element of the list -- this is interface used for iteration and elem generic accessor functionality
  virtual void          LinkCopyLeaves(const taList_impl& cp);
  // #CAT_ObjectMgmt #EXPERT create links in this list to all terminal leaf items in source list (i.e., Borrow) -- this is used for creating shallow container copies with different views -- does NOT copy full hierarchical substructure or anything -- just links leaves for accessor routines

  override bool         IsContainer()   { return true; }
  override taMatrix*    ElView() const  { return (taMatrix*)el_view.ptr(); }
  override IndexMode    ElViewMode() const  { return el_view_mode; }
  override int          ElemCount() const { return size; }
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const;
  override Variant      IterElem(taBaseItr* itr) const;
  override taBaseItr*   Iter() const;
  virtual bool          SetElView(taMatrix* view_mat, IndexMode md = IDX_COORDS);
  // #CAT_Access #EXPERT set el view to given new case -- just sets the members
  virtual taList_impl*  NewElView(taMatrix* view_mat, IndexMode md = IDX_COORDS) const;
  // #CAT_Access #EXPERT make a new view of this list -- returns a new pointer list with view set

  override String       GetPath_Long(taBase* ta=NULL, taBase* par_stop = NULL) const;
  override String       GetPath(taBase* ta=NULL, taBase* par_stop = NULL) const;
  override String       GetPathNames(taBase* ta=NULL, taBase* par_stop=NULL) const;

  override void*        FindMembeR(const String& nm, MemberDef*& ret_md) const;

  override void Close();
  override bool Close_Child(taBase* obj);
  override bool CloseLater_Child(taBase* obj);
  override void ChildUpdateAfterEdit(taBase* child, bool& handled);
  override void SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);

  override taBase* CopyChildBefore(taBase* src, taBase* child_pos);

  override String&      Print(String& strm, int indent = 0) const;

  override String GetValStr(void* par = NULL, MemberDef* md = NULL,
                            TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                            bool force_inline = false) const;
  override bool  SetValStr(const String& val, void* par = NULL, MemberDef* md = NULL,
                           TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                           bool force_inline = false);
  override int  ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                              void* par = NULL, TypeDef* par_typ=NULL, MemberDef* md = NULL,
                              TypeDef::StrContext sc = TypeDef::SC_DEFAULT);

  override taObjDiffRec* GetObjDiffVal(taObjDiff_List& odl, int nest_lev,
                                       MemberDef* memb_def=NULL, const void* par=NULL,
                                       TypeDef* par_typ=NULL, taObjDiffRec* par_od=NULL) const;

#ifndef __MAKETA__
  override void Dump_Save_GetPluginDeps();
  override int  Dump_SaveR(std::ostream& strm, taBase* par=NULL, int indent=0);
  override int  Dump_Save_PathR(std::ostream& strm, taBase* par=NULL, int indent=0);
  virtual int   Dump_Save_PathR_impl(std::ostream& strm, taBase* par, int indent); // #IGNORE
  override void Dump_Load_pre();
  override taBase* Dump_Load_Path_parent(const String& el_path, TypeDef* ld_el_typ);
  override int  Dump_Load_Value(std::istream& strm, taBase* par=NULL);
#endif

  override void Search_impl(const String& srch, taBase_PtrList& items,
                            taBase_PtrList* owners = NULL,
                            bool contains = true, bool case_sensitive = false,
                            bool obj_name = true, bool obj_type = true,
                            bool obj_desc = true, bool obj_val = true,
                            bool mbr_name = true, bool type_desc = false);
  override void CompareSameTypeR(Member_List& mds, TypeSpace& base_types,
                           voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                           taBase* cp_base,
                           int show_forbidden=TypeItem::NO_HIDDEN,
                           int show_allowed=TypeItem::SHOW_CHECK_MASK, 
                           bool no_ptrs = true);
  override int  UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  override int  UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  override int  UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);
  override int  UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr);

  taBase*       DefaultEl_() const      { return (taBase*)SafeEl_(el_def); } // #IGNORE

  virtual int   SetDefaultElType(TypeDef* it);
  // #CAT_Access set the default element to be item with given type
  virtual int   SetDefaultElName(const String& nm);
  // #CAT_Access set the default element to be item with given name
  virtual int   SetDefaultEl(taBase* it);
  // #CAT_Access set the default element to be given item

  taBase*       New_gui(int n_objs=1, TypeDef* typ=NULL,
    const String& name="(default name)");
  // #BUTTON #MENU_CONTEXT #MENU #NO_SAVE_ARG_VAL #TYPE_ON_1_el_base #INIT_ARGVAL_ON_1_el_typ #LABEL_New #CAT_Modify create n_objs new objects of given type in list (typ=NULL: default type, el_typ;)
  virtual void  SetSize(int sz);
  // #MENU #MENU_ON_Edit #CAT_Modify add or remove elements to force list to be of given size

  override bool RemoveIdx(int idx);
  override bool Transfer(taBase* item);

  virtual void  EnforceType();
  // #CAT_Modify enforce current type (all elements have to be of this type)
  void  EnforceSameStru(const taList_impl& cp);
  // #CAT_Modify make the two lists identical in terms of size and types of objects

  virtual bool  ChangeType(int idx, TypeDef* new_type);
  // change type of item at index
  virtual bool  ChangeType(taBase* itm, TypeDef* new_type);
  // #MENU #ARG_ON_OBJ #CAT_Modify #TYPE_ON_el_base change type of item to new type, copying current info
  virtual int   ReplaceType(TypeDef* old_type, TypeDef* new_type);
  // #MENU #USE_RVAL #CAT_Modify #TYPE_ON_el_base replace all items of old type with new type (returns number changed)

  virtual int   FindTypeIdx(TypeDef* item_tp) const;
  // #CAT_Access find index of (first) element that inherits from given type (-1 = not found)
  virtual int   FindNameContainsIdx(const String& item_nm) const;
  // #CAT_Access Find index of (first) element whose name contains given name sub-string (-1 = nonot found)
  virtual int   FindNameTypeIdx(const String& item_nm) const;
  // #CAT_Access Find index of (first) element with given object name or type name (item_nm) (-1 if not found)

  virtual taBase* FindType_(TypeDef* item_tp) const;    // #IGNORE
  virtual taBase* FindNameContains_(const String& item_nm) const;
  // #IGNORE
  virtual taBase* FindNameType_(const String& item_nm) const;
  // #IGNORE

  void  SetBaseType(TypeDef* it); // #CAT_Modify set base (and default) type to given td

  virtual bool  MakeElNamesUnique();
  // #CAT_taList make all of the element names in the list unique -- returns true if all unique already
  virtual bool  MakeElNameUnique(taBase* itm);
  // #CAT_taList make sure name of given item on this list is unique -- will change only the name of this item to ensure uniqueness (does not check other items against other items)

#if defined(TA_GUI) && !defined(__MAKETA__)
  override const QPixmap* GetDataNodeBitmap(int bmf, int& flags_supported) const;
#endif
  override int          NumListCols() const {return 3;}
  // #IGNORE number of columns in a default list view for this list type
  override const        KeyString GetListColKey(int col) const;
  // #IGNORE get the key for the default list view
  override String       GetColHeading(const KeyString& key) const;
  // #IGNORE header text for the indicated column
  override String       GetColText(const KeyString& key, int itm_idx = -1) const;
  override String       ChildGetColText(void* child, TypeDef* typ, const KeyString& key,
                                        int itm_idx = -1) const;        // #IGNORE
  override int          SelectForEditSearch(const String& memb_contains, SelectEdit*& editor);

  void  CutLinks();
  void  UpdateAfterEdit(); // we skip the taOBase version, and inherit only taBase (DPF: what does that mean?)
  TA_BASEFUNS(taList_impl);

protected:
  int           m_trg_load_size; // #IGNORE target load size -- set during Dump_Load -- used to enforce size to that which was saved, in case of load-over

  String        GetListName_() const    { return name; }
  void          El_SetIndex_(void* it, int idx) {((taBase*)it)->SetIndex(idx);}
  void          El_SetDefaultName_(void*, int idx); // sets default name if child has DEF_NAME_LIST
  String        El_GetName_(void* it) const { return ((taBase*)it)->GetName(); }
  void          El_SetName_(void* it, const String& nm)  {((taBase*)it)->SetName(nm);}
  taPtrList_impl*        El_GetOwnerList_(void* it) const
  { return dynamic_cast<taList_impl*>(((taBase*)it)->GetOwner()); }
  void*         El_GetOwnerObj_(void* it) const { return ((taBase*)it)->GetOwner(); }
  void*         El_SetOwner_(void* it)  { ((taBase*)it)->SetOwner(this); return it; }
  bool          El_FindCheck_(void* it, const String& nm) const
  {  if (((taBase*)it)->FindCheck(nm)) {
      taPtrList_impl* own = El_GetOwnerList_(it);
      return ((!own) || (own == (taPtrList_impl*)this));  }
    return false; }

  void*         El_Ref_(void* it)       { taBase::Ref((taBase*)it); return it; }
  void*         El_unRef_(void* it)     { taBase::unRef((taBase*)it); return it; }
  void          El_Done_(void* it)      { taBase::Done((taBase*)it); }
  void*         El_Own_(void* it)       { taBase::Own((taBase*)it,this); return it; }
  void          El_disOwn_(void* it)
  { if(El_GetOwnerList_(it) == this) {((taBase*)it)->Destroying(); ((taBase*)it)->CutLinks();}
    El_Done_(El_unRef_(it)); }
  // cut links to other objects when removed from owner group

  void*         El_MakeToken_(void* it) { return (void*)((taBase*)it)->MakeToken(); }
  void*         El_Copy_(void* trg, void* src)
  { ((taBase*)trg)->UnSafeCopy((taBase*)src); return trg; }
  void*         El_CopyN_(void* to, void* fm); // wrap in an update bracket

protected:
  override void CanCopy_impl(const taBase* cp_fm, bool quiet,
    bool& ok, bool virt) const;
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override String ChildGetColText_impl(taBase* child, const KeyString& key, int itm_idx = -1) const;
  override taBase* New_impl(int n_objs, TypeDef* typ, const String& nm);
private:
  void  Copy_(const taList_impl& cp);
  void  Initialize();
  void  Destroy();
};

#ifndef __MAKETA__
template<typename T, typename Ref, typename Ptr>
class IteratorTaPtrListImpl
{
public:
  typedef IteratorTaPtrListImpl             Self;
  typedef std::random_access_iterator_tag   iterator_category;
  typedef T                                 value_type;
  typedef std::size_t                       difference_type;
  typedef Ptr                               pointer;
  typedef Ref                               reference;

  IteratorTaPtrListImpl(const taPtrList_impl *list, int idx)
    : list_(list)
    , idx_(idx)
  {
  }
  IteratorTaPtrListImpl()
    : list_(0)
    , idx_(0)
  {
  }
  bool operator==(const Self &it) const
  {
    // Default constructed iterators are singular, not equal to any
    // other iterator.
    return list_ && list_ == it.list_ && idx_ == it.idx_;
  }
  bool operator!=(const Self &it) const
  {
    return !(*this == it);
  }
  reference operator*() const
  {
    return *ptr();
  }
  pointer operator->() const
  {
    return ptr();
  }
  Self & operator++() // prefix
  {
    assert(list_);
    ++idx_;
    return *this;
  }
  Self operator++(int) // postfix
  {
    Self ret(*this);
    operator++();
    return ret;
  }
  Self & operator--() // prefix
  {
    assert(list_);
    --idx_;
    return *this;
  }
  Self operator--(int) // postfix
  {
    Self ret(*this);
    operator--();
    return ret;
  }
  Self & operator+=(difference_type n)
  {
    assert(list_);
    idx_ += n;
    return *this;
  }
  Self & operator-=(difference_type n)
  {
    assert(list_);
    idx_ -= n;
    return *this;
  }
  Self operator+(difference_type n) const
  {
    Self ret(*this);
    ret += n;
    return ret;
  }
  Self operator-(difference_type n) const
  {
    Self ret(*this);
    ret -= n;
    return ret;
  }
  friend Self operator+(difference_type n, const Self &it)
  {
    return it + n;
  }
  difference_type operator-(const Self &it) const
  {
    assert(list_);
    assert(list_ == it.list_);
    return idx_ - it.idx_;
  }
  reference operator[](difference_type n) const
  {
    return *(*this + n);
  }
  bool operator<(const Self &it) const
  {
    assert(list_);
    assert(list_ == it.list_);
    return idx_ < it.idx_;
  }

private:
  pointer ptr() const
  {
    assert(list_);
    // Cast from void* returned by SafeEl_().
    pointer item = (pointer) list_->SafeEl_(idx_);
    assert(item);
    return item;
  }

  // The list this iterator indexes into.  If null, the iterator is singular.
  const taPtrList_impl *list_;
  // The index into the list; only valid if list_ non-null.
  int idx_;
};
#endif // __MAKETA__


#endif // taList_impl_h
