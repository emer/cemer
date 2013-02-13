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

#include "taPtrList_impl.h"
#include <SigLinkSignal>

#include <taHashTable>
#include <taMisc>
#include <MTRnd>

taPtrList_impl taPtrList_impl::scratch_list;

void taPtrList_impl::InitList_() {
  hash_table = NULL;
//   alloc_size = 2;
//   el = (void**)calloc(alloc_size, sizeof(void*));
  // start out un-allocated
  alloc_size = 0;
  el = NULL;
  size = 0;
  SigEmit(SLS_LIST_INIT);
}

taPtrList_impl::~taPtrList_impl() {
  Reset();
  if(el != NULL) {
    free(el);
    el = NULL;
    alloc_size = 0;
  }
  if(hash_table) {
    delete hash_table;
    hash_table = NULL;
  }
}

// allocate by powers of two, minus the amount of overhead required by the
// memory manger

bool taPtrList_impl::Alloc(int sz) {
  if(alloc_size >= sz)  return true;    // no need to increase..
  int old_alloc_sz = alloc_size;
  sz = MAX(16-TA_ALLOC_OVERHEAD-1,sz);          // once allocating, use a minimum of 16
  alloc_size += TA_ALLOC_OVERHEAD; // increment to full power of 2
  while((alloc_size-TA_ALLOC_OVERHEAD) <= sz) alloc_size <<= 1;
  alloc_size -= TA_ALLOC_OVERHEAD;
  if(!el) {
    el = (void**)malloc(alloc_size * sizeof(void*));
    if(!el) {
      taMisc::Error("taPtrList_impl::Alloc -- malloc failed -- pointer list is too long! could be fatal.");
      alloc_size = 0;
      return false;
    }
  }
  else {
    void** nwel = (void**)realloc((char *) el, alloc_size * sizeof(void*));
    if(!nwel) {
      taMisc::Error("taPtrList_impl::Alloc -- realloc failed -- pointer list is too long! could be fatal.");
      alloc_size = old_alloc_sz;
      return false;
    }
    el = nwel;
  }
  return true;
}

bool taPtrList_impl::AllocExact(int sz) {
  int old_alloc_sz = alloc_size;
  alloc_size = MAX(sz, size);
  if(!el) {
    el = (void**)malloc(alloc_size * sizeof(void*));
    if(!el) {
      taMisc::Error("taPtrList_impl::AllocExact -- malloc failed -- pointer list is too long! could be fatal.");
      alloc_size = 0;
      return false;
    }
  }
  else {
    void** nwel = (void**)realloc((char *) el, alloc_size * sizeof(void*));
    if(!nwel) {
      taMisc::Error("taPtrList_impl::AllocExact -- realloc failed -- pointer list is too long! could be fatal.");
      alloc_size = old_alloc_sz;
      return false;
    }
    el = nwel;
  }
  return true;
}

void taPtrList_impl::BuildHashTable(int sz, KeyType key_typ) {
  if(!hash_table)
    hash_table = new taHashTable();
  hash_table->key_type = key_typ;

  if(!hash_table->Alloc(sz)) return;
  for(int i=0; i<size; i++) {
    hash_table->AddHash(El_GetHashVal_(el[i]), i, El_GetHashString_(el[i]));
  }
}

void taPtrList_impl::ReBuildHashTable() {
  if(!hash_table) return;
  hash_table->RemoveAll();
  for(int i=0; i<size; i++) {
    hash_table->AddHash(El_GetHashVal_(el[i]), i, El_GetHashString_(el[i]));
  }
}

taHashVal taPtrList_impl::El_GetHashVal_(void* it) const {
  int kt = 0;
  if(hash_table) kt = hash_table->key_type;
  switch (kt) {
  case taHashTable::KT_PTR:
    return taHashEl::HashCode_Ptr(it);
  case taHashTable::KT_NAME:
  default:
    return taHashEl::HashCode_String(El_GetName_(it));
  }
}

String taPtrList_impl::El_GetHashString_(void* it) const {
  return (hash_table->key_type == taHashTable::KT_NAME) ? El_GetName_(it) : _nilString;
}

int taPtrList_impl::FindEl_(const void* it) const {
  if (hash_table && (hash_table->key_type == taHashTable::KT_PTR))
    return hash_table->FindHashValPtr(it);

  int i;
  for(i=0; i < size; i++) {
    if(el[i] == it)
      return i;
  }
  return -1;
}

int taPtrList_impl::FindNameIdx(const String& nm) const {
  if (hash_table && (hash_table->key_type == taHashTable::KT_NAME))
    return hash_table->FindHashValString(nm);

  for(int i=0; i < size; i++) {
    if(El_FindCheck_(el[i], nm)) {
      return i;
    }
  }
  return -1;
}

void* taPtrList_impl::FindName_(const String& nm) const {
  int idx = FindNameIdx(nm);
  if(idx >= 0) return el[idx];
  return NULL;
}


void taPtrList_impl::Hijack(taPtrList_impl& src) {
  // normally only used when already empty, but this guarantees it!
  if (el) {
    Reset();
    el = (void**)realloc((char *) el, 0);
  }
  el = src.el;
  size = src.size;
  alloc_size = src.alloc_size;
  src.el = NULL;
  src.size = 0;
  src.alloc_size = 0;
}

void taPtrList_impl::UpdateIndex_(int idx) {
  if(el[idx] == NULL)
    return;
  if(El_GetOwnerList_(el[idx]) == this) {
    El_SetIndex_(el[idx], idx);
  }
  if(hash_table)
    hash_table->UpdateHashVal(El_GetHashVal_(el[idx]), idx, El_GetHashString_(el[idx]));
}

bool taPtrList_impl::MoveIdx(int fm, int to) {
  if (fm == to) return true; // nop
  if ((fm < 0) || (fm >= size) || (to < 0) || (to >= size)) return false;

  void* itm = el[fm];
  // algo is diff depending on whether fm is > or < to
  if (fm < to) {
    // 'to' gets pushed up, to make room for fm
    for (int j = fm; j < to; j++) {             // compact, if necc
      el[j] = el[j+1];
      UpdateIndex_(j);
    }
  } else { // fm > to
    for (int j = fm; j > to; j--) {
      el[j] = el[j-1];
      UpdateIndex_(j);
    }
  }
  el[to] = itm;
  UpdateIndex_(to);
  void* op2 = NULL;
  if (to > 0) op2 = FastEl_(to - 1);
  SigEmit(SLS_LIST_ITEM_MOVED, itm, op2);
  return true;
}

bool taPtrList_impl::MoveBeforeIdx(int fm, int to) {
// consistent api, used to move the item fm to before the to index
// to move to end, use to=-1 or size
// note that fm=to-1 is also a noop
  if ((fm < 0) || (fm >= size)) return false; // bad params
  if ((to < 0) || (to > size)) to = size;
  if ((fm == to) || ((fm + 1) == to)) return true; // nop

  void* itm = el[fm];
  void* op2 = NULL;
  // algo is diff depending on whether fm is > or < to
  if (fm < to) {
    for (int j = fm; j < (to - 1); j++) {
      el[j] = el[j+1];
      UpdateIndex_(j);
    }
    el[to-1] = itm;
    UpdateIndex_(to-1);
    if (to > 1) op2 = FastEl_(to - 2);
    SigEmit(SLS_LIST_ITEM_MOVED, itm, op2); // itm, itm_after, NULL at beg
  } else { // fm > to
    for (int j = fm; j > to; j--) {
      el[j] = el[j-1];
      UpdateIndex_(j);
    }
    el[to] = itm;
    UpdateIndex_(to);
    if (to > 0) op2 = FastEl_(to - 1);
    SigEmit(SLS_LIST_ITEM_MOVED, itm, op2); // itm, itm_after, NULL at beg
  }
  return true;
}

bool taPtrList_impl::MoveBefore_(void* trg, void* item) {
  int trgi = FindEl_(trg); // -1 if none, which means "last" for Before api
  int iti = FindEl_(item);
  return MoveBeforeIdx(iti, trgi); // validates all params
}

bool taPtrList_impl::MoveAfter_(void* trg, void* item) {
  int trgi = (trg) ? FindEl_(trg) + 1 : 0; // first if none
  int iti = FindEl_(item);
  if((trgi < 0) || (iti < 0) || (iti == trgi)) return false;
  return MoveIdx(iti, trgi);
}

bool taPtrList_impl::SwapIdx(int pos1, int pos2) {
  if ((pos1 < 0) || (pos2 < 0) || (pos1 >= size) || (pos2 >= size)) return false;
  if (pos1 == pos2) return true;
  void* tmp = el[pos1];
  el[pos1] = el[pos2];
  UpdateIndex_(pos1);
  el[pos2] = tmp;
  UpdateIndex_(pos2);
  SigEmit(SLS_LIST_ITEMS_SWAP, el[pos2], el[pos1]);
  return true;
}


/////////////////////////
//        Add          //
/////////////////////////

void taPtrList_impl::AddOnly_(void* it) {
//TODO: note, the logistics of the SigEmit are wrong, since ex. Add_
// causes the notification before it has set the index and owned the item, renaming it
  if(size+1 >= alloc_size) {
    if(!Alloc(size+1)) return;
  }
  el[size++] = it;
}

void taPtrList_impl::Add_(void* it, bool no_notify) {
  int idx = size; // the new item index, once added
  AddOnly_(it);
  if (it != NULL) {
    El_SetIndex_(El_Own_(it), idx);
    if (!taMisc::is_loading && !taMisc::is_duplicating && El_GetName_(it).empty()) {
      El_SetDefaultName_(it, idx);
    }
    if(hash_table)
      hash_table->AddHash(El_GetHashVal_(it), idx, El_GetHashString_(it));
  }
  if (no_notify) return;
  void* op2 = NULL;
  if (idx > 0) op2 = FastEl_(idx - 1);
  SigEmit(SLS_LIST_ITEM_INSERT, it, op2);
}

bool taPtrList_impl::AddUnique_(void* it) {
  // note: this should be valid but somehow it is not and causes problems if uncommented:
  // if(El_GetOwnerList_(it) == this) return false;
  if(FindEl_(it) >= 0)
    return false;
  Add_(it);
  return true;
}
bool taPtrList_impl::AddUniqNameNew_(void* it) {
  if(El_GetOwnerList_(it) == this) return false; // if we're already on the list, the replace causes delete and it is bad
  int i = FindNameIdx(El_GetName_(it));
  if(i >= 0) {
    ReplaceIdx_(i,it);
    return false;
  }
  Add_(it);
  return true;
}
void* taPtrList_impl::AddUniqNameOld_(void* it) {
  // note: this optimization is relatively new and could potentially cause problems..
  if(El_GetOwnerList_(it) == this) return it;
  int i = FindNameIdx(El_GetName_(it));
  if(i >= 0) {
    return el[i];
  }
  Add_(it);
  return it;
}

bool taPtrList_impl::RemoveIdx(int i) {
  if((size == 0) || (i >= size))
    return false;
  void* tel = el[i];
  //note: change in 4.0 - don't disown until after removed from list
  if(tel != NULL) {
    if(hash_table)
      hash_table->RemoveHash(El_GetHashVal_(tel), El_GetHashString_(tel));
  }
  int j;
  for(j=i; j < size-1; j++) {           // compact, if necc
    el[j] = el[j+1];
    UpdateIndex_(j);
  }
  --size;
  ItemRemoved_(); //NOTE: for Groups, we update leaf counts (supercursively) at this point
  SigEmit(SLS_LIST_ITEM_REMOVE, tel);
  if (tel) El_disOwn_(tel);
  //WARNING: other modules depends on no more code after this point! (ex. using El_Done to self-delete)
  return true;
}

bool taPtrList_impl::RemoveEl_(void* it) {
  int i = FindEl_(it);
  return (i < 0) ? false : RemoveIdx(i);
}
bool taPtrList_impl::RemoveName(const String& it) {
  int i = FindNameIdx(it);
  return (i < 0) ? false : RemoveIdx(i);
}
bool taPtrList_impl::RemoveLast() {
  return (size == 0) ? false : RemoveIdx(size - 1);
}

void taPtrList_impl::RemoveAll() {
  while (size > 0)
    RemoveLast();
}

bool taPtrList_impl::Insert_(void* it, int where, bool no_notify) {
  if((where >= size) || (where < 0)) {
    Add_(it, no_notify);
    return true;
  }
  AddOnly_(NULL);
  int i;
  for(i=size-1; i > where; i--) {
    el[i] = el[i-1];
    UpdateIndex_(i);
  }
  el[where] = it;
  if(it != NULL) {
    El_SetIndex_(El_Own_(it), where);
    if(hash_table)
      hash_table->AddHash(El_GetHashVal_(it), where, El_GetHashString_(it));
    void* op2 = NULL;
    if (where > 0) op2 = FastEl_(where - 1);
    if (!no_notify) SigEmit(SLS_LIST_ITEM_INSERT, it, op2);
  }
  return true;
}
bool taPtrList_impl::ReplaceEl_(void* ol, void* nw) {
  int i = FindEl_(ol);
  if (i < 0) return false;
  ReplaceIdx_(i, nw);
  return true;
}
bool taPtrList_impl::ReplaceName_(const String& ol, void* nw) {
  int i = FindNameIdx(ol);
  if(i < 0) return false;
  return ReplaceIdx_(i, nw);
}
bool taPtrList_impl::ReplaceIdx_(int ol, void* nw, bool no_notify_insert) {
  if((size == 0) || (ol >= size))
    return false;
  if(el[ol] != NULL) {
    if(hash_table)
      hash_table->RemoveHash(El_GetHashVal_(el[ol]), El_GetHashString_(el[ol]));
    SigEmit(SLS_LIST_ITEM_REMOVE, el[ol]);
    El_disOwn_(el[ol]);
  }
  el[ol] = nw;
  if(nw != NULL) {
    El_SetIndex_(El_Own_(nw), ol);
    if(hash_table)
      hash_table->AddHash(El_GetHashVal_(nw), ol, El_GetHashString_(nw));
    if (!no_notify_insert) {
      void* op2 = NULL;
      if (ol > 0) op2 = FastEl_(ol - 1);
      SigEmit(SLS_LIST_ITEM_INSERT, nw, op2);
    }
  }
  return true;
}

bool taPtrList_impl::Transfer_(void* it) {
  taPtrList_impl* old_own = El_GetOwnerList_(it);
  if (old_own == this)
    return false;
  El_Ref_(it);                  // extra ref so no delete on remove
  El_SetOwner_(it);             // change owner to us so it doesn't call CutLinks with Remove..
  if (old_own)
    old_own->RemoveEl_(it);
  Add_(it);
  El_unRef_(it);
  return true;
}


/////////////////////////
//        Link         //
/////////////////////////

void taPtrList_impl::Link_(void* it) {
  AddOnly_(it);
  if(it != NULL) {
    El_Ref_(it);
    if(hash_table)
      hash_table->AddHash(El_GetHashVal_(it), size-1, El_GetHashString_(it));
  }
  void* op2 = NULL;
  if (size > 1) op2 = FastEl_(size - 2); //for SigEmit
  SigEmit(SLS_LIST_ITEM_INSERT, it, op2);
}

bool taPtrList_impl::LinkUnique_(void* it) {
  if(FindEl_(it) >= 0)
    return false;
  Link_(it);
  return true;
}

bool taPtrList_impl::LinkUniqNameNew_(void* it) {
  int i = FindNameIdx(El_GetName_(it));
  if(i >= 0) {
    ReplaceLinkIdx_(i,it);      // semantics of LinkUniqName is to update..
    return false;
  }
  Link_(it);
  return true;
}

void* taPtrList_impl::LinkUniqNameOld_(void* it) {
  int i = FindNameIdx(El_GetName_(it));
  if(i >= 0) {
    return el[i];
  }
  Link_(it);
  return it;
}

bool taPtrList_impl::InsertLink_(void* it, int where) {
  if((where >= size) || (where < 0)) {
    Link_(it);
    return true;
  }
  if(size > 0)
    AddOnly_(NULL);
  int i;
  for(i=size-1; i>where; i--) {
    el[i] = el[i-1];
    UpdateIndex_(i);
  }
  el[where] = it;
  if(it != NULL) {
    El_Ref_(it);
    if(hash_table)
      hash_table->AddHash(El_GetHashVal_(it), where, El_GetHashString_(it));
    void* op2 = NULL;
    if (where > 0) op2 = FastEl_(where - 1);
    SigEmit(SLS_LIST_ITEM_INSERT, it, op2);
  }
  return true;
}

bool taPtrList_impl::ReplaceLinkEl_(void* ol, void* nw) {
  int i = FindEl_(ol);
  if (i < 0) return false;
  ReplaceLinkIdx_(i, nw);
  return true;
}

bool taPtrList_impl::ReplaceLinkName_(const String& ol, void* nw) {
  int i = FindNameIdx(ol);
  if(i < 0) return false;
  return ReplaceLinkIdx_(i, nw);
}

bool taPtrList_impl::ReplaceLinkIdx_(int ol, void* nw) {
  if((size == 0) || (ol >= size))
    return false;
  if(el[ol] != NULL) {
    if(hash_table)
      hash_table->RemoveHash(El_GetHashVal_(el[ol]), El_GetHashString_(el[ol]));
    SigEmit(SLS_LIST_ITEM_REMOVE, el[ol]);
    El_disOwn_(el[ol]);
  }
  el[ol] = nw;
  if(nw != NULL) {
    El_Ref_(nw);
    if(hash_table)
      hash_table->AddHash(El_GetHashVal_(nw), ol, El_GetHashString_(nw));
    void* op2 = NULL;
    if (ol > 0) op2 = FastEl_(ol - 1);
    SigEmit(SLS_LIST_ITEM_INSERT, nw, op2);
  }
  return true;
}


/////////////////////////
//        Push         //
/////////////////////////

void taPtrList_impl::Push_(void* it) {
  AddOnly_(it);
  if(it != NULL) {
    El_Ref_(it);
    if(hash_table)
      hash_table->AddHash(El_GetHashVal_(it), size-1, El_GetHashString_(it));
  }
  void* op2 = NULL;
  if (size > 1) op2 = FastEl_(size - 2);
  SigEmit(SLS_LIST_ITEM_INSERT, it, op2);
}

void* taPtrList_impl::Pop_() {
  if (size == 0) return NULL;
  void* rval = el[--size];
  if(rval != NULL) {
    if(hash_table)
      hash_table->RemoveHash(El_GetHashVal_(rval), El_GetHashString_(rval));
    SigEmit(SLS_LIST_ITEM_REMOVE, rval);
    El_unRef_(rval);
  }
  return rval;
}

void* taPtrList_impl::TakeItem_(int idx) {
  if ((idx < 0) || (idx >= size)) return NULL;
  void* rval = el[idx];
  RemoveIdx(idx);
  return rval;
}

/////////////////////////
//      List-Wise      //
/////////////////////////

void taPtrList_impl::PopAll() {
  int i;
  for(i=size-1; i>=0; i--)
    Pop_();
}

void taPtrList_impl::Permute() {
  int i, nv;
  for(i=0; i<size; i++) {
    nv = (int) ((MTRnd::genrand_int32() % (size - i)) + i); // get someone from the future
    SwapIdx(i, nv);
  }
}

void taPtrList_impl::Sort(bool descending) {
  Sort_(descending);
  UpdateAllIndicies();
  SigEmit(SLS_LIST_SORTED);
}

void taPtrList_impl::Sort_(bool descending) {
  if(size <= 1) return;
  int lt_compval = -1;          // comparison return value for less-than
  if(descending)
    lt_compval = 1;
  // lets do a heap sort since it requires no secondary storage
  int n = size;
  int l,j,ir,i;
  void* tmp;

  l = (n >> 1) + 1;
  ir = n;
  for(;;){
    if(l>1)
      tmp = el[--l -1]; // tmp = ra[--l]
    else {
      tmp = el[ir-1]; // tmp = ra[ir]
      el[ir-1] = el[0]; // ra[ir] = ra[1]
      if(--ir == 1) {
        el[0] = tmp; // ra[1]=tmp
        return;
      }
    }
    i=l;
    j=l << 1;
    while(j<= ir) {
      if(j<ir && (El_Compare_(el[j-1],el[j]) == lt_compval)) j++;
      if(El_Compare_(tmp,el[j-1]) == lt_compval) { // tmp < ra[j]
        el[i-1] = el[j-1]; // ra[i]=ra[j];
        j += (i=j);
      }
      else j = ir+1;
    }
    el[i-1] = tmp; // ra[i] = tmp;
  }
}

void taPtrList_impl::UpdateAllIndicies() {
  int i;
  for(i=0; i<size; i++)  {
    if(El_GetOwnerList_(el[i]) == this)
      El_SetIndex_(el[i], i);
  }
}


/////////////////////////
//      Duplication    //
/////////////////////////

void* taPtrList_impl::El_CopyN_(void* to, void* fm) {
  void* rval = El_Copy_(to, fm);
  El_SetName_(to, El_GetName_(fm));
  return rval;
}


void* taPtrList_impl::DuplicateEl_(void* it) {
  if(it == NULL)
    return NULL;
  int idx = FindEl_(it); // can't be not found!
  ++taMisc::is_duplicating;
  void* nw = El_MakeToken_(it);
  Insert_(nw, idx + 1, true); //defer notify until after copy
  El_Copy_(nw, it); // note: DONT set name, leave as default
  --taMisc::is_duplicating;
  SigEmit(SLS_LIST_ITEM_INSERT, nw, SafeEl_(idx));
  return nw;
}

void taPtrList_impl::Stealth_Borrow(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  int i;
  for(i=0; i < cp.size; i++) {
    AddOnly_(cp.el[i]);
  }
}

// this uses the local FindCheck insteaad of scratch_lists
int taPtrList_impl::Scratch_Find_(const String& it) const {
  int i;
  for(i=0; i < scratch_list.size; i++) {
    if(El_FindCheck_(scratch_list.el[i], it))
      return i;
  }
  return -1;
}

void taPtrList_impl::Trim(int n) {
  for (int i= size - 1; i >= n; --i)
      RemoveIdx(i);
}

void taPtrList_impl::Duplicate(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  int i;
  for(i=0; i < cp.size; i++) {
    if(cp.el[i] == NULL)
      Add_(NULL);
    else {
      ++taMisc::is_duplicating;
      void* it = El_MakeToken_(cp.el[i]);
      Add_(it, true);
      El_Copy_(it, cp.el[i]);
      SigEmit(SLS_LIST_ITEM_INSERT, it, SafeEl_(size - 2));
      --taMisc::is_duplicating;
    }
  }
}

// the following are optimized to only find duplicates between the two lists
// and not within a given one.  this makes them considerably more efficient!
// they use the scratch_list for keeping a copy of the old list

void taPtrList_impl::DupeUniqNameNew(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Borrow(*this);   // get this into scratch for find (since replacing
                                // we need to refer to these items (no stealth)
  int i;
  for(i=0; i < cp.size; i++) {
    if(cp.el[i] == NULL)  continue;
    ++taMisc::is_duplicating;
    void* it = El_MakeToken_(cp.el[i]);
    int idx = Scratch_Find_(El_GetName_(cp.el[i]));
    if (idx >= 0) {
      ReplaceIdx_(idx, it, true); //note: only insert notify is suppressed
      El_Copy_(it, cp.el[i]);
      SigEmit(SLS_LIST_ITEM_INSERT, it, PosSafeEl_(idx - 1));
    }
    else {
      Add_(it, true);
      El_Copy_(it, cp.el[i]);
      SigEmit(SLS_LIST_ITEM_INSERT, it, PosSafeEl_(size - 2));
    }
    --taMisc::is_duplicating;
  }
  scratch_list.Reset();
}
void taPtrList_impl::DupeUniqNameOld(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Stealth_Borrow(*this);   // get this into scratch for find
  int i;
  for(i=0; i < cp.size; i++) {
    if(cp.el[i] == NULL)  continue;
    if(Scratch_Find_(El_GetName_(cp.el[i])) < 0) {
      ++taMisc::is_duplicating;
      void* it = El_MakeToken_(cp.el[i]);
      Add_(it, true);
      El_Copy_(it, cp.el[i]);
      SigEmit(SLS_LIST_ITEM_INSERT, it, PosSafeEl_(size - 2));
      --taMisc::is_duplicating;
    }
  }
  scratch_list.size = 0;
}

void taPtrList_impl::Borrow(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  for(int i=0; i < cp.size; i++)
    Link_(cp.el[i]);
}
void taPtrList_impl::BorrowUnique(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Stealth_Borrow(*this);   // get this into scratch for find
  for(int i=0; i < cp.size; i++) {
    void* it = cp.el[i];
    if(scratch_list.FindEl_(it) < 0)
      Link_(it);
  }
  scratch_list.size = 0;
}
void taPtrList_impl::BorrowUniqNameNew(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Borrow(*this);   // get this into scratch for find (using replace..)
  for(int i=0; i < cp.size; i++) {
    void* it = cp.el[i];
    int idx = Scratch_Find_(El_GetName_(it));
    if (idx >= 0)
      ReplaceLinkIdx_(idx, it);
    else
      Link_(it);
  }
  scratch_list.Reset();
}
void taPtrList_impl::BorrowUniqNameOld(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Stealth_Borrow(*this);   // get this into scratch for find
  for(int i=0; i < cp.size; i++) {
    void* it = cp.el[i];
    if(Scratch_Find_(El_GetName_(it)) < 0)
      Link_(it);
  }
  scratch_list.size = 0;
}

void taPtrList_impl::BorrowUniqNameOldFirst(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Stealth_Borrow(*this);   // get this into scratch for find, and to add back
  size = 0;                             // effectively reset us to 0, so all new links go at start
  for(int i=0; i < cp.size; i++) {
    void* it = cp.el[i];
    if(Scratch_Find_(El_GetName_(it)) < 0)
      Link_(it);
  }
  Stealth_Borrow(scratch_list); // now get everyone back from scratch
  scratch_list.size = 0;
  ReBuildHashTable();           // needed if we have one
}

void taPtrList_impl::Copy_Common(const taPtrList_impl& cp) {
  int mx_idx = MIN(size,cp.size);
  for(int i=0; i < mx_idx; i++) {
    if(cp.el[i] == NULL) continue;
    void* it = el[i];
    ++taMisc::is_duplicating;
    El_CopyN_(it, cp.el[i]); //+name
    --taMisc::is_duplicating;
    SigEmit(SLS_LIST_ITEM_UPDATE, it);
  }
}

void taPtrList_impl::Copy_Duplicate(const taPtrList_impl& cp) {
  int mx_sz = MAX(size, cp.size);
  if(!Alloc(mx_sz)) return;
  Copy_Common(cp);
  Copy_Duplicate_impl(cp);
}

void taPtrList_impl::Copy_Duplicate_impl(const taPtrList_impl& cp) {
  for (int i=size; i < cp.size; i++) {
    void* cp_it = cp.el[i];
    switch (cp.El_Kind_(cp_it)) {
    case EK_NULL:
      Add_(NULL);
      break;
    case EK_OWN: {
      ++taMisc::is_duplicating;
      void* it = El_MakeToken_(cp_it);
      Add_(it, true);
      El_CopyN_(it, cp_it);
      SigEmit(SLS_LIST_ITEM_INSERT, it, PosSafeEl_(size - 2));
      --taMisc::is_duplicating;
    } break;
    case EK_LINK:
      Link_(cp_it);
      break;
    }
  }
}

void taPtrList_impl::Copy_Borrow(const taPtrList_impl& cp) {
  Copy_Common(cp);
  for(int i=size; i < cp.size; i++)
    Link_(cp.el[i]);
}

void taPtrList_impl::Copy_Exact(const taPtrList_impl& cp) {
  // if we have more items than cp, then trim ourself, otherwise make room
  if (size > cp.size)
    Trim(cp.size);
  else {
    if(!Alloc(cp.size)) return;
  }
  // do a pairwise check for items we can keep; remove others
  // we can keep owned items of same type, or same links, or NULL
  // copy commensurable owned items as we encounter them
  int i = 0;
  while ((i < size) && (i < cp.size)) {
    void* it = el[i];
    void* cp_it = cp.el[i];
    ElKind ek = El_Kind_(it);
    ElKind cp_ek = cp.El_Kind_(cp_it);
    // we check cases in order of likelihood
    // if both owned, must be same type
    if ((ek == EK_OWN) && (cp_ek == EK_OWN)) {
      if (El_GetType_(it) == cp.El_GetType_(cp_it)) {
        ++taMisc::is_duplicating;
        El_CopyN_(it, cp_it);
        --taMisc::is_duplicating;
        SigEmit(SLS_LIST_ITEM_UPDATE, it);
        goto cont;
      } // otherwise fall through
    }
    // if both are links, must be same item
    else if ((ek == EK_LINK) && (cp_ek == EK_LINK)) {
      if (it == cp_it) goto cont;
    }
    // if either null, then both must be null
    else if ((it == NULL) || (cp_it == NULL)) {
      if (it == cp_it) goto cont;
    }
    // not commensurable,
    RemoveIdx(i);
    continue; // don't advance index
cont:
    ++i;
  }
  // ok, now copy or link in any ones not in us
  Copy_Duplicate_impl(cp);
}

taPtrList_impl::ElKind taPtrList_impl::El_Kind_(void* it) const {
  if (!it) return EK_NULL;
  // if object is owned by us, or has no owner it is an instance
  else if ((El_GetOwnerList_(it) == this) ||
    (El_GetOwnerObj_(it) == NULL))
    return EK_OWN;
  // otherwise a link
  return EK_LINK;
}

String& taPtrList_impl::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << "Elements of List: " << GetListName_() << " [" << size << "] {\n";
  String_PArray nms;
  nms.Alloc(size);
  for(int i=0; i<size; i++) {
    if(el[i] == NULL)
      nms.Add("NULL");
    else
      nms.Add(El_GetName_(el[i]));
  }
  taMisc::FancyPrintList(strm, nms, indent+1);
  taMisc::IndentString(strm, indent);
  strm << "}";
  return strm;
}
