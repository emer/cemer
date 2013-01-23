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

#ifndef taGroup_h
#define taGroup_h 1

// parent includes:
#include <taGroup_impl>

// member includes:

// declare all other types mentioned but not required to include:


#ifndef __MAKETA__

template<typename T, typename Ref, typename Ptr, typename GpPtr, typename GpIt>
class IteratorTaGroupLeaf
{
public:
  typedef IteratorTaGroupLeaf               Self;
  typedef std::bidirectional_iterator_tag   iterator_category;
  typedef T                                 value_type;
  typedef std::size_t                       difference_type;
  typedef Ptr                               pointer;
  typedef Ref                               reference;

  IteratorTaGroupLeaf(GpPtr group, const GpIt &gp_it)
    : group_(group)
    , gp_it_(gp_it)
    , el_idx_(0)
  {
  }
  IteratorTaGroupLeaf()
    : group_(0)
    , gp_it_(0)
    , el_idx_(0)
  {
  }
  bool operator==(const Self &it) const
  {
    // Default constructed iterators are singular, not equal to any
    // other iterator.
    return group_ && group_ == it.group_ && gp_it_ == it.gp_it_ && el_idx_ == it.el_idx_;
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
    assert(group_);
    GpIt gp_end = group_->leaf_gp_end();
    // If already past-the-end, don't increment anything.
    // Otherwise, increment the element index.
    if (gp_it_ != gp_end && ++el_idx_ >= gp_it_->size) {
      // We incremented past the last element in the subgroup,
      // so move on to the next non-empty subgroup.  This could
      // end up iterating all the way to the end.
      while (++gp_it_ != gp_end && gp_it_->size == 0) {}
      el_idx_ = 0;
    }
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
    assert(group_);
    GpIt gp_begin = group_->leaf_gp_begin();
    // Decrement the element index, unless we're already "past-the-begin".
    if (!(gp_it_ == gp_begin && el_idx_ < 0) && --el_idx_ < 0) {
      // We decremented past the first element in the subgroup, so move back
      // to the previous non-empty subgroup.  This could iterate all the way
      // back to the beginning (but not beyond).
      while (gp_it_ != gp_begin && (--gp_it_)->size == 0) {}
      // Now, either gp_it_ == begin, or gp_it_ has elements, or both.  Set
      // the index to the last element in the group, or -1 if no elements.
      // Allow decrementing to "past-the-begin" so --i; ++i; is a null op.
      el_idx_ = gp_it_->size - 1;
    }
    return *this;
  }
  Self operator--(int) // postfix
  {
    Self ret(*this);
    operator--();
    return ret;
  }

private:
  pointer ptr() const
  {
    assert(group_);
    // Cast from void* returned by SafeEl_().
    pointer item = (pointer) gp_it_->SafeEl_(el_idx_);
    assert(item);
    return item;
  }

  // A pointer to the taGroup<> object this leaf-iterator was created on.
  GpPtr group_;
  // An iterator into group_.leaf_gp indicating the current group position.
  GpIt gp_it_;
  // The index into the leaf group.
  int el_idx_;
};

#endif // __MAKETA__

TypeDef_Of(taGroup);

template<class T>
class taGroup : public taGroup_impl {
  // #INSTANCE #NO_UPDATE_AFTER
  INHERITED(taGroup_impl)
public:
  #ifndef __MAKETA__
  // Iterators for immediate elements of this group.
  typedef IteratorTaPtrListImpl<T, T&, T*>                       el_iterator;
  typedef IteratorTaPtrListImpl<T, const T&, const T*>     const_el_iterator;
  typedef std::reverse_iterator<el_iterator>             reverse_el_iterator;
  typedef std::reverse_iterator<const_el_iterator> const_reverse_el_iterator;

                el_iterator el_begin()         { return               el_iterator(this, 0); }
          const_el_iterator el_begin()   const { return         const_el_iterator(this, 0); }
          const_el_iterator el_cbegin()  const { return         const_el_iterator(this, 0); }
                el_iterator el_end()           { return               el_iterator(this, size); }
          const_el_iterator el_end()     const { return         const_el_iterator(this, size); }
          const_el_iterator el_cend()    const { return         const_el_iterator(this, size); }
        reverse_el_iterator el_rbegin()        { return       reverse_el_iterator(      el_iterator(this, size)); }
  const_reverse_el_iterator el_rbegin()  const { return const_reverse_el_iterator(const_el_iterator(this, size)); }
  const_reverse_el_iterator el_crbegin() const { return const_reverse_el_iterator(const_el_iterator(this, size)); }
        reverse_el_iterator el_rend()          { return       reverse_el_iterator(      el_iterator(this, 0)); }
  const_reverse_el_iterator el_rend()    const { return const_reverse_el_iterator(const_el_iterator(this, 0)); }
  const_reverse_el_iterator el_crend()   const { return const_reverse_el_iterator(const_el_iterator(this, 0)); }

  // Iterators for immediate subgroups of this group.
  // Also used for the list of leaf groups.
  typedef typename taList<taGroup<T> >::iterator                             gp_iterator;
  typedef typename taList<taGroup<T> >::const_iterator                 const_gp_iterator;
  typedef typename taList<taGroup<T> >::reverse_iterator             reverse_gp_iterator;
  typedef typename taList<taGroup<T> >::const_reverse_iterator const_reverse_gp_iterator;

                gp_iterator sub_gp_begin()         { return               gp_iterator(&gp, 0); }
          const_gp_iterator sub_gp_begin()   const { return         const_gp_iterator(&gp, 0); }
          const_gp_iterator sub_gp_cbegin()  const { return         const_gp_iterator(&gp, 0); }
                gp_iterator sub_gp_end()           { return               gp_iterator(&gp, gp.size); }
          const_gp_iterator sub_gp_end()     const { return         const_gp_iterator(&gp, gp.size); }
          const_gp_iterator sub_gp_cend()    const { return         const_gp_iterator(&gp, gp.size); }
        reverse_gp_iterator sub_gp_rbegin()        { return       reverse_gp_iterator(      gp_iterator(&gp, gp.size)); }
  const_reverse_gp_iterator sub_gp_rbegin()  const { return const_reverse_gp_iterator(const_gp_iterator(&gp, gp.size)); }
  const_reverse_gp_iterator sub_gp_crbegin() const { return const_reverse_gp_iterator(const_gp_iterator(&gp, gp.size)); }
        reverse_gp_iterator sub_gp_rend()          { return       reverse_gp_iterator(      gp_iterator(&gp, 0)); }
  const_reverse_gp_iterator sub_gp_rend()    const { return const_reverse_gp_iterator(const_gp_iterator(&gp, 0)); }
  const_reverse_gp_iterator sub_gp_crend()   const { return const_reverse_gp_iterator(const_gp_iterator(&gp, 0)); }

                gp_iterator leaf_gp_begin()         { InitLeafGp(); return               gp_iterator(leaf_gp, 0); }
          const_gp_iterator leaf_gp_begin()   const { InitLeafGp(); return         const_gp_iterator(leaf_gp, 0); }
          const_gp_iterator leaf_gp_cbegin()  const { InitLeafGp(); return         const_gp_iterator(leaf_gp, 0); }
                gp_iterator leaf_gp_end()           { InitLeafGp(); return               gp_iterator(leaf_gp, leaf_gp->size); }
          const_gp_iterator leaf_gp_end()     const { InitLeafGp(); return         const_gp_iterator(leaf_gp, leaf_gp->size); }
          const_gp_iterator leaf_gp_cend()    const { InitLeafGp(); return         const_gp_iterator(leaf_gp, leaf_gp->size); }
        reverse_gp_iterator leaf_gp_rbegin()        { InitLeafGp(); return       reverse_gp_iterator(      gp_iterator(leaf_gp, leaf_gp->size)); }
  const_reverse_gp_iterator leaf_gp_rbegin()  const { InitLeafGp(); return const_reverse_gp_iterator(const_gp_iterator(leaf_gp, leaf_gp->size)); }
  const_reverse_gp_iterator leaf_gp_crbegin() const { InitLeafGp(); return const_reverse_gp_iterator(const_gp_iterator(leaf_gp, leaf_gp->size)); }
        reverse_gp_iterator leaf_gp_rend()          { InitLeafGp(); return       reverse_gp_iterator(      gp_iterator(leaf_gp, 0)); }
  const_reverse_gp_iterator leaf_gp_rend()    const { InitLeafGp(); return const_reverse_gp_iterator(const_gp_iterator(leaf_gp, 0)); }
  const_reverse_gp_iterator leaf_gp_crend()   const { InitLeafGp(); return const_reverse_gp_iterator(const_gp_iterator(leaf_gp, 0)); }

  typedef IteratorTaGroupLeaf<T, T&, T*, taGroup<T> *, gp_iterator>                               iterator;
  typedef IteratorTaGroupLeaf<T, const T&, const T*, const taGroup<T> *, const_gp_iterator> const_iterator;
  typedef std::reverse_iterator<iterator>                                                 reverse_iterator;
  typedef std::reverse_iterator<const_iterator>                                     const_reverse_iterator;

//typename<T, G>
//struct GroupSubclass {
//  typedef IteratorTaGroupLeaf<T, T&, T*, G*, typename taList<G>::iterator> iterator;
//  static iterator begin(G* g,
//}

                iterator begin()         { return               iterator(this, leaf_gp_begin()); }
          const_iterator begin()   const { return         const_iterator(this, leaf_gp_cbegin()); }
          const_iterator cbegin()  const { return         const_iterator(this, leaf_gp_cbegin()); }
                iterator end()           { return               iterator(this, leaf_gp_end()); }
          const_iterator end()     const { return         const_iterator(this, leaf_gp_cend()); }
          const_iterator cend()    const { return         const_iterator(this, leaf_gp_cend()); }
        reverse_iterator rbegin()        { return       reverse_iterator(      iterator(this, leaf_gp_end())); }
  const_reverse_iterator rbegin()  const { return const_reverse_iterator(const_iterator(this, leaf_gp_cend())); }
  const_reverse_iterator crbegin() const { return const_reverse_iterator(const_iterator(this, leaf_gp_cend())); }
        reverse_iterator rend()          { return       reverse_iterator(      iterator(this, leaf_gp_begin())); }
  const_reverse_iterator rend()    const { return const_reverse_iterator(const_iterator(this, leaf_gp_cbegin())); }
  const_reverse_iterator crend()   const { return const_reverse_iterator(const_iterator(this, leaf_gp_cbegin())); }

  #endif // __MAKETA__

  ////////////////////////////////////////////////
  //    functions that return the type          //
  ////////////////////////////////////////////////

  // operators
  T*            SafeEl(int idx) const           { return (T*)SafeEl_(idx); }
  // #CAT_Access get element at index
  T*            FastEl(int i) const             { return (T*)el[i]; }
  // #CAT_Access fast element (no checking)
  T*            operator[](int i) const         { return (T*)el[i]; }

  T*            DefaultEl() const               { return (T*)DefaultEl_(); }
  // #CAT_Access returns the element specified as the default for this group

  // note that the following is just to get this on the menu (it doesn't actually edit)
  T*            Edit_El(T* item) const          { return SafeEl(FindEl((taBase*)item)); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARG_ON_OBJ #CAT_Access Edit given group item

  taGroup<T>*   SafeGp(int idx) const           { return (taGroup<T>*)gp.SafeEl(idx); }
  // #CAT_Access get group at index
  taGroup<T>*   FastGp(int i) const             { return (taGroup<T>*)gp.FastEl(i); }
  // #CAT_Access the sub group at index
  taGroup<T>*   FastLeafGp(int gp_idx) const    { return (taGroup<T>*)FastLeafGp_(gp_idx); }
  // #CAT_Access the leaf sub group at index, note: 0 is always "this"
  taGroup<T>*   SafeLeafGp(int gp_idx) const    { return (taGroup<T>*)SafeLeafGp_(gp_idx); }
  // #CAT_Access the leaf sub group at index, note: 0 is always "this"

  T*            Leaf(int idx) const             { return (T*)Leaf_(idx); }
  // #CAT_Access get leaf element at index
  taGroup<T>*   RootGp() const                  { return (taGroup<T>*)root_gp;  }
  // #CAT_Access the root group ('this' for the root group)

  // iterator-like functions
  inline T*     FirstEl(taLeafItr& lf) const    { return (T*)FirstEl_(lf); }
  // #CAT_Access returns first leaf element and inits indexes
  inline T*     NextEl(taLeafItr& lf) const     { return (T*)NextEl_(lf); }
  // #CAT_Access returns next leaf element and incs indexes
  inline T*     LastEl(taLeafItr& lf) const     { return (T*)LastEl_(lf); }
  // #CAT_Access returns first leaf element and inits indexes
  inline T*     PrevEl(taLeafItr& lf) const     { return (T*)PrevEl_(lf); }
  // #CAT_Access returns next leaf element and incs indexes

  taGroup<T>*   FirstGp(int& g) const           { return (taGroup<T>*)FirstGp_(g); }
  // #CAT_Access returns first leaf group and inits index
  taGroup<T>*   NextGp(int& g) const            { return (taGroup<T>*)NextGp_(g); }
  // #CAT_Access returns next leaf group and incs index

  virtual T*    NewEl(int n_els=1, TypeDef* typ=NULL) { return (T*)NewEl_(n_els, typ);}
  // #CAT_Modify Create and add n_els new element(s) of given type to the group (NULL = default type, el_typ)
  virtual taGroup<T>* NewGp(int n_gps=1, TypeDef* typ=NULL,
    const String& name="") { return (taGroup<T>*)NewGp_(n_gps, typ, name);}
  // #CAT_Modify Create and add n_gps new sub group(s) of given type (NULL = same type as this group)

  T*            FindName(const String& item_nm)  const
  { return (T*)FindName_(item_nm); }
  // #CAT_Access Find element in top-level list with given name (nm) (NULL = not here)
  virtual T*    FindNameContains(const String& item_nm) const
  { return (T*)FindNameContains_(item_nm); }
  // #CAT_Access Find (first) element in top-level list whose name contains given string (NULL = not here)
  virtual T*    FindType(TypeDef* item_tp) const
  { return (T*)FindType_(item_tp); }
  // #CAT_Access find in top-level list given type element (NULL = not here)
  T*            FindNameType(const String& item_nm) const
  { return (T*)FindNameType_(item_nm); }
  // #CAT_Access Find element in top-level list with given object name or type name (item_nm)

  T*            Pop()                           { return (T*)Pop_(); }
  // #CAT_Modify pop the last element off the stack
  virtual T*    Peek()                          { return (T*)Peek_(); }
  // #CAT_Access peek at the last element on the stack

  virtual T*    AddUniqNameOld(T* item)         { return (T*)AddUniqNameOld_((void*)item); }
  // #CAT_Modify add so that name is unique, old used if dupl, returns one used
  virtual T*    LinkUniqNameOld(T* item)        { return (T*)LinkUniqNameOld_((void*)item); }
  // #CAT_Modify link so that name is unique, old used if dupl, returns one used

//   virtual bool       MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
//   // #CAT_Modify move item so that it appears just before the target item trg in the list
//   virtual bool       MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
//   // #CAT_Modify move item so that it appears just after the target item trg in the list

  T*            FindLeafName(const String& item_nm) const
  { return (T*)FindLeafName_(item_nm); }
  // #CAT_Access Find element anywhere in full group and subgroups with given name (item_nm)
  T*            FindLeafNameContains(const String& item_nm) const
  { return (T*)FindLeafNameContains_(item_nm); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARGC_1 #LABEL_Find #CAT_Access Find anywhere in full group and subgroups first element whose name contains given name (item_nm)
  T*            FindLeafType(TypeDef* item_tp) const
  { return (T*)FindLeafType_(item_tp);}
  // #CAT_Access find anywhere in full group and subgroups given type leaf element (NULL = not here)
  T*            FindLeafNameType(const String& item_nm) const
  { return (T*)FindLeafNameType_(item_nm); }
  // #CAT_Access Find anywhere in full group and subgroups element with given object name or type name (item_nm)

  TA_TMPLT_BASEFUNS(taGroup,T);
protected:
  taGroup<T>*   LeafGp(int leaf_idx) const              { return (taGroup<T>*)LeafGp_(leaf_idx); }
  // the group containing given leaf; NOTE: **don't confuse this with the Safe/FastLeafGp funcs*** -- moved here to try to flush out any use, since it is so confusing and nonstandard and likely to be mixed up with the XxxLeafGp funcs
private:
  TMPLT_NOCOPY(taGroup,T)
  void Initialize()     { SetBaseType(T::StatTypeDef(1));}
  void  Destroy () {}
};

// do not use this macro, since you typically will want ##NO_TOKENS, #NO_UPDATE_AFTER
// which cannot be put inside the macro!
//
// #define taGroup_of(T)
// class T ## _Group : public taGroup<T> {
// public:
//   void Initialize()  { };
//   void Destroy()     { };
//   TA_BASEFUNS(T ## _Group);
// }

// use taBase_Group.h as a template instead

#endif // taGroup_h
