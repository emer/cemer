// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

// implimentation header for the ta list classes

#include "ta_list.h"
#include "ta_mtrnd.h"

// needed just for the taMisc::display_width variable..
#include "ta_type.h"

taPtrList_impl taPtrList_impl::scratch_list;

taHashVal taPtrList_impl::HashCode_String(const String& string_) {
  // now using the one that Qt uses, comment is:
  // These functions are based on Peter J. Weinberger's hash function (from the
  // Dragon Book). The constant 24 in the original function was replaced with 23
  // to produce fewer collisions on input such as "a", "aa", "aaa", "aaaa", ...

  taHashVal hash = 0;
  const char* string = string_.chars();
  taHashVal g;
  while (1) {
    unsigned int c = *string;
    string++;
    if (c == 0) {
      break;
    }
    hash += (hash<<4) + c;
    if ((g = (hash & 0xf0000000)) != 0)
      hash ^= g >> 23;
    hash &= ~g;
  }
  return hash;
}

taHashVal taPtrList_impl::HashCode_Ptr(const void* ptr) {
  return (taHashVal)ptr;
}

void taPtrList_impl::InitList_() {
  hash_table = NULL;
//   alloc_size = 2;
//   el = (void**)calloc(alloc_size, sizeof(void*));
  // start out un-allocated
  alloc_size = 0;
  el = NULL;
  size = 0;
  DataChanged(DCR_LIST_INIT);
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
  if(alloc_size >= sz)	return true;	// no need to increase..
  int old_alloc_sz = alloc_size;
  sz = MAX(16-TA_ALLOC_OVERHEAD-1,sz);		// once allocating, use a minimum of 16
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
  for(int i=0; i<size; i++)
    hash_table->AddHash(El_GetHashVal_(el[i]), i, El_GetHashString_(el[i]));
}

taHashVal taPtrList_impl::El_GetHashVal_(void* it) const {
  int kt = 0;
  if(hash_table) kt = hash_table->key_type;
  switch (kt) {
  case taHashTable::KT_PTR:
    return HashCode_Ptr(it);
  case taHashTable::KT_NAME:
  default:
    return HashCode_String(El_GetName_(it));
  }
}

String taPtrList_impl::El_GetHashString_(void* it) const {
  return (hash_table->key_type == taHashTable::KT_NAME) ? El_GetName_(it) : _nilString;
}

void* taPtrList_impl::GetTA_Element_(Variant i, TypeDef*& eltd) const {
  eltd = NULL;
  if(i.isStringType()) {	// lookup by name if string
    void* rval = FindName_(i.toString());
    if(rval) { eltd = El_GetType_(rval); return rval; }
  }
  int dx = i.toInt();	// string could be number in disguise -- try that next
  void* rval = SafeEl_(dx); if (rval) eltd = El_GetType_(rval); return rval;
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
    for (int j = fm; j < to; j++) {		// compact, if necc
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
  DataChanged(DCR_LIST_ITEM_MOVED, itm, op2);
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
    DataChanged(DCR_LIST_ITEM_MOVED, itm, op2); // itm, itm_after, NULL at beg
  } else { // fm > to
    for (int j = fm; j > to; j--) {
      el[j] = el[j-1];
      UpdateIndex_(j);
    }
    el[to] = itm;
    UpdateIndex_(to);
    if (to > 0) op2 = FastEl_(to - 1);
    DataChanged(DCR_LIST_ITEM_MOVED, itm, op2); // itm, itm_after, NULL at beg
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
  DataChanged(DCR_LIST_ITEMS_SWAP, el[pos2], el[pos1]);
  return true;
}


/////////////////////////
//        Add          //
/////////////////////////

void taPtrList_impl::AddOnly_(void* it) {
//TODO: note, the logistics of the DataChanged are wrong, since ex. Add_
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
  DataChanged(DCR_LIST_ITEM_INSERT, it, op2); 
}

bool taPtrList_impl::AddUnique_(void* it) {
  if(FindEl_(it) >= 0)
    return false;
  Add_(it);
  return true;
}
bool taPtrList_impl::AddUniqNameNew_(void* it) {
  int i = FindNameIdx(El_GetName_(it));
  if(i >= 0) {
    ReplaceIdx_(i,it);
    return false;
  }
  Add_(it);
  return true;
}
void* taPtrList_impl::AddUniqNameOld_(void* it) {
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
  for(j=i; j < size-1; j++) {		// compact, if necc
    el[j] = el[j+1];
    UpdateIndex_(j);
  }
  --size;
  ItemRemoved_(); //NOTE: for Groups, we update leaf counts (supercursively) at this point
  DataChanged(DCR_LIST_ITEM_REMOVE, tel);
  if (tel) El_disOwn_(tel);
  //WARNING: other modules depends on no more code after this point! (ex. using El_Done to self-delete)
  return true;
}

bool taPtrList_impl::RemoveEl_(void* it) {
  int i;
  if((i = FindEl_(it)) < 0)
    return false;
  return RemoveIdx(i);
}
bool taPtrList_impl::RemoveName(const String& it) {
  int i = FindNameIdx(it);
  if(i >= 0)
    return RemoveIdx(i);
  return false;
}
bool taPtrList_impl::RemoveLast() {
  if(size == 0) return false;
  return RemoveIdx(size-1);
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
    if (!no_notify) DataChanged(DCR_LIST_ITEM_INSERT, it, op2);
  }
  return true;
}
bool taPtrList_impl::ReplaceEl_(void* ol, void* nw) {
  int i;
  if((i = FindEl_(ol)) < 0)
    return false;
  ReplaceIdx_(i, nw);
  return true;
}
bool taPtrList_impl::ReplaceName_(const String& ol, void* nw) {
  int i = FindNameIdx(ol);
  if(i >= 0)
    return ReplaceIdx_(i, nw);
  return false;
}
bool taPtrList_impl::ReplaceIdx_(int ol, void* nw, bool no_notify_insert) {
  if((size == 0) || (ol >= size))
    return false;
  if(el[ol] != NULL) {
    if(hash_table)
      hash_table->RemoveHash(El_GetHashVal_(el[ol]), El_GetHashString_(el[ol]));
    DataChanged(DCR_LIST_ITEM_REMOVE, el[ol]);
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
      DataChanged(DCR_LIST_ITEM_INSERT, nw, op2);
    }
  }
  return true;
}

bool taPtrList_impl::Transfer_(void* it) {
  taPtrList_impl* old_own = El_GetOwnerList_(it);
  if (old_own == this)
    return false;
  El_Ref_(it);			// extra ref so no delete on remove
  El_SetOwner_(it);		// change owner to us so it doesn't call CutLinks with Remove..
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
  if (size > 1) op2 = FastEl_(size - 2); //for DataChanged
  DataChanged(DCR_LIST_ITEM_INSERT, it, op2); 
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
    ReplaceLinkIdx_(i,it);	// semantics of LinkUniqName is to update..
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
    DataChanged(DCR_LIST_ITEM_INSERT, it, op2);
  }
  return true;
}

bool taPtrList_impl::ReplaceLinkEl_(void* ol, void* nw) {
  int i;
  if((i = FindEl_(ol)) < 0)
    return false;
  ReplaceLinkIdx_(i, nw);
  return true;
}

bool taPtrList_impl::ReplaceLinkName_(const String& ol, void* nw) {
  int i = FindNameIdx(ol);
  if(i >= 0)
    return ReplaceLinkIdx_(i, nw);
  return false;
}

bool taPtrList_impl::ReplaceLinkIdx_(int ol, void* nw) {
  if((size == 0) || (ol >= size))
    return false;
  if(el[ol] != NULL) {
    if(hash_table)
      hash_table->RemoveHash(El_GetHashVal_(el[ol]), El_GetHashString_(el[ol]));
    DataChanged(DCR_LIST_ITEM_REMOVE, el[ol]);
    El_disOwn_(el[ol]);
  }
  el[ol] = nw;
  if(nw != NULL) {
    El_Ref_(nw);
    if(hash_table)
      hash_table->AddHash(El_GetHashVal_(nw), ol, El_GetHashString_(nw));
    void* op2 = NULL;
    if (ol > 0) op2 = FastEl_(ol - 1);
    DataChanged(DCR_LIST_ITEM_INSERT, nw, op2);
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
  DataChanged(DCR_LIST_ITEM_INSERT, it, op2);
}

void* taPtrList_impl::Pop_() {
  if (size == 0) return NULL;
  void* rval = el[--size];
  if(rval != NULL) {
    if(hash_table)
      hash_table->RemoveHash(El_GetHashVal_(rval), El_GetHashString_(rval));
    DataChanged(DCR_LIST_ITEM_REMOVE, rval);
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
  DataChanged(DCR_LIST_SORTED);
}

void taPtrList_impl::Sort_(bool descending) {
  if(size <= 1) return;
  int lt_compval = -1;		// comparison return value for less-than
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
  DataChanged(DCR_LIST_ITEM_INSERT, nw, SafeEl_(idx)); 
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
      DataChanged(DCR_LIST_ITEM_INSERT, it, SafeEl_(size - 2)); 
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
  scratch_list.Borrow(*this);	// get this into scratch for find (since replacing
				// we need to refer to these items (no stealth)
  int i;
  for(i=0; i < cp.size; i++) {
    if(cp.el[i] == NULL)  continue;
    ++taMisc::is_duplicating;
    void* it = El_MakeToken_(cp.el[i]);
    int idx;
    if((idx=Scratch_Find_(El_GetName_(cp.el[i]))) >= 0) {
      ReplaceIdx_(idx,it, true); //note: only insert notify is suppressed
      El_Copy_(it, cp.el[i]);
      DataChanged(DCR_LIST_ITEM_INSERT, it, PosSafeEl_(idx - 1)); 
    }  else {
      Add_(it, true);
      El_Copy_(it, cp.el[i]);
      DataChanged(DCR_LIST_ITEM_INSERT, it, PosSafeEl_(size - 2)); 
    }
    --taMisc::is_duplicating;
  }
  scratch_list.Reset();
}
void taPtrList_impl::DupeUniqNameOld(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Stealth_Borrow(*this);	// get this into scratch for find
  int i;
  for(i=0; i < cp.size; i++) {
    if(cp.el[i] == NULL)  continue;
    if(Scratch_Find_(El_GetName_(cp.el[i])) < 0) {
      ++taMisc::is_duplicating;
      void* it = El_MakeToken_(cp.el[i]);
      Add_(it, true);
      El_Copy_(it, cp.el[i]);
      DataChanged(DCR_LIST_ITEM_INSERT, it, PosSafeEl_(size - 2)); 
      --taMisc::is_duplicating;
    }
  }
  scratch_list.size = 0;
}

void taPtrList_impl::Borrow(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  int i;
  for(i=0; i < cp.size; i++)
    Link_(cp.el[i]);
}
void taPtrList_impl::BorrowUnique(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Stealth_Borrow(*this);	// get this into scratch for find
  int i;
  for(i=0; i < cp.size; i++) {
    void* it = cp.el[i];
    if(scratch_list.FindEl_(it) < 0)
      Link_(it);
  }
  scratch_list.size = 0;
}
void taPtrList_impl::BorrowUniqNameNew(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Borrow(*this);	// get this into scratch for find (using replace..)
  int i;
  for(i=0; i < cp.size; i++) {
    void* it = cp.el[i];
    int idx;
    if((idx=Scratch_Find_(El_GetName_(it))) >= 0)
      ReplaceLinkIdx_(idx, it);
    else
      Link_(it);
  }
  scratch_list.Reset();
}
void taPtrList_impl::BorrowUniqNameOld(const taPtrList_impl& cp) {
  if(!Alloc(size + cp.size)) return;
  scratch_list.size = 0;
  scratch_list.Stealth_Borrow(*this);	// get this into scratch for find
  int i;
  for(i=0; i < cp.size; i++) {
    void* it = cp.el[i];
    if(Scratch_Find_(El_GetName_(it)) < 0)
      Link_(it);
  }
  scratch_list.size = 0;
}

void taPtrList_impl::Copy_Common(const taPtrList_impl& cp) {
  int mx_idx = MIN(size,cp.size);
  int i;
  void* it;
  for(i=0; i < mx_idx; i++) {
    if(cp.el[i] == NULL) continue;
    it = el[i];
    ++taMisc::is_duplicating;
    El_CopyN_(it, cp.el[i]); //+name
    --taMisc::is_duplicating;
    DataChanged(DCR_LIST_ITEM_UPDATE, it); 
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
      DataChanged(DCR_LIST_ITEM_INSERT, it, PosSafeEl_(size - 2)); 
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
  int i;
  for(i=size; i < cp.size; i++)
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
        DataChanged(DCR_LIST_ITEM_UPDATE, it); 
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

ostream& taPtrList_impl::Indenter(ostream& strm, const String& itm, int no, int prln, int tabs)
{
  strm << itm << " ";
  if((no+1) % prln == 0) {
    strm << "\n";
    taMisc::FlushConsole();
    return strm;
  }
  int len = (int)strlen(itm) + 1;
  int i;
  for(i=tabs; i>=0; i--) {
    if(len < (i * 8))
      strm << "\t";
  }
  return strm;
}

void taPtrList_impl::List(ostream& strm) const {
  int i;
  strm << "\nElements of List: " << GetListName_() << " (" << size << ")\n";
  int names_width = 0;
  for(i=0; i<size; i++) {
    if(el[i] == NULL)
      names_width = MAX(names_width, 4);
    else
      names_width = MAX(names_width, (int)El_GetName_(el[i]).length());
  }
  int tabs = (names_width / 8) + 1;
  int prln = taMisc::display_width / (tabs * 8);  if(prln <= 0) prln = 1;
  for(i=0; i<size; i++) {
    if(el[i] == NULL)
      Indenter(strm, "NULL", i, prln, tabs);
    else
      Indenter(strm, El_GetName_(el[i]), i, prln, tabs);
  }
  strm << "\n";
  strm.flush();
}

///////////////////////////
//        Hash Table     //
///////////////////////////

// some of the following code (the hashing function and the prime number list)
// were taken from the COOL object library package, which has the following
// copyright notice:
//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.

int taHashTable::n_bucket_primes[] =
                     {3, 7, 13, 19, 29, 41, 53, 67, 83, 97, 113, 137,
                      163, 191, 223, 263, 307, 349, 401, 461, 521,
		      653, 719, 773, 839, 911, 983, 1049, 1123, 1201,
		      1279, 1367, 1459, 1549, 1657, 1759, 1861, 1973,
		      2081, 2179, 2281, 2383, 2503, 2617, 2729, 2843,
		      2963, 3089, 3203, 3323, 3449, 3571, 3697, 3833,
		      3967, 4099, 4241, 4391, 4549, 4703, 4861, 5011,
		      5171, 5333, 5483, 5669, 5839, 6029, 6197, 6361,
		      6547, 6761, 6961, 7177, 7393, 7517, 7727, 7951,
		      8101, 8209, 16411, 32771, 65537, 131301, 262147,
		      524287};

int taHashTable::n_primes = 86;

void taHashTable::InitList_() {
  Alloc(3);
}

void taHashTable::AddHash(taHashVal hash, int val, const String& str) {
  if (size == 0) return;		// this shouldn't happen, but justin case..
  int buck_no = (int)(hash % size);
  taHashBucket* bucket = FastEl(buck_no);
  if(!bucket) {
    bucket = new taHashBucket();
    ReplaceIdx(buck_no, bucket);
  }
  bucket->Add(new taHashEl(hash, val, str));
  bucket_max = MAX(bucket_max, bucket->size);
}

bool taHashTable::Alloc(int sz) {
  Reset();			// get rid of any existing ones
  bucket_max = 0;
  int act_sz = 0;
  int cnt = 0;
  while((cnt < n_primes) && (act_sz < sz))	act_sz = n_bucket_primes[cnt++];
  if(!taPtrList<taHashBucket>::Alloc(act_sz)) return false;
  int i;
  for(i=0; i<act_sz; i++)	// initialize with nulls
    AddOnly_(NULL);
  return true;
}

int taHashBucket::FindBucketIndex(taHashVal hash, const String& str) const {
  for(int i=0; i < size; i++) {
    taHashEl* hel = FastEl(i);
    if(hel->hash_code == hash && hel->hashed_str == str) { // nil strs always match..
      return i;
    }
  }
  return -1;
}

int taHashBucket::FindHashVal(taHashVal hash, const String& str) const {
  for(int i=0; i < size; i++) {
    taHashEl* hel = FastEl(i);
    if(hel->hash_code == hash && hel->hashed_str == str) { // nil strs always match..
      return FastEl(i)->value;
    }
  }
  return -1;
}

int taHashTable::FindHashVal(taHashVal hash, const String& str) const {
  if(size == 0)	return -1;
  int buck_no = (int)(hash % size);
  taHashBucket* bucket = FastEl(buck_no);
  if(bucket == NULL) return -1;
  return bucket->FindHashVal(hash, str);
}

bool taHashTable::UpdateHashVal(taHashVal hash, int val, const String& str) {
  if(size == 0)	return false;
  int buck_no = (int)(hash % size);
  taHashBucket* bucket = FastEl(buck_no);
  if(bucket == NULL) return false;
  int idx = bucket->FindBucketIndex(hash, str);
  if(idx < 0)    return false;
  bucket->FastEl(idx)->value = val;
  return true;
}

bool taHashTable::RemoveHash(taHashVal hash, const String& str) {
  if(size == 0)	return false;
  int buck_no = (int)(hash % size);
  taHashBucket* bucket = FastEl(buck_no);
  if(bucket == NULL) return false;
  int idx = bucket->FindBucketIndex(hash, str);
  if(idx == -1)
    return false;
  return bucket->RemoveIdx(idx);
}

void taHashTable::RemoveAll() {
  taPtrList<taHashBucket>::RemoveAll();
  bucket_max = 0;
}

/////////////////////////
//  taFixedArray_impl  //
/////////////////////////

void taFixedArray_impl::Add_(const void* it) {
  if(!Alloc_(size + 1)) return;
  El_Copy_(FastEl_(size++), it);
}

bool taFixedArray_impl::AddUnique_(const void* it) {
  if (Find_(it) >= 0)
    return false;
  Add_(it);
  return true;
}

bool taFixedArray_impl::Alloc_(uint alloc) {
  char* nw = (char*)MakeArray_(alloc);
  if(!nw) {
    taMisc::Error("taFixedArray_impl::Alloc_ -- malloc error -- array is too big -- could be fatal!");
    return false;
  }
  for (int i = 0; i < size; ++i) {
    El_Copy_(nw + (El_SizeOf_() * i), FastEl_(i));
  }
  SetArray_(nw);
  return true;
}

void taFixedArray_impl::Copy_(const taFixedArray_impl& cp) {
  if (cp.size < size) ReclaimOrphans_(cp.size, size - 1);
  else if(!Alloc_(cp.size)) return;
  
  for (int i=0; i < cp.size; ++i) {
    El_Copy_(FastEl_(i), cp.FastEl_(i));
  }
  size = cp.size;
}

void taFixedArray_impl::SetSize(int new_size) {
  if (new_size < 0) new_size = 0;
  if (new_size > size) {
    if(!Alloc_(new_size)) return;
    const void* blank = El_GetBlank_();
    for (int i = size; i < new_size; ++i) {
      El_Copy_(FastEl_(i), blank);
    }
  } else if (new_size < size) {
    ReclaimOrphans_(new_size, size - 1);
  }
  size = new_size;	
}

bool taFixedArray_impl::Equal_(const taFixedArray_impl& src) const {
  //NOTE: this should only be called if src is same type as us (use dynamic_cast)
  if (size != src.size) return false;
  for (int i = 0; i < size; ++i) {
    if (!El_Equal_(FastEl_(i), src.FastEl_(i))) return false;
  }
  return true;
}

int taFixedArray_impl::Find_(const void* it, int where) const {
  int i;
  for (i=where; i<size; i++) {
    if (El_Equal_(it, FastEl_(i)))
      return i;
  }
  return -1;
}

void taFixedArray_impl::InitVals_(const void* it, int start, int end) {
  if (end == -1) end = size;  
  else end = MIN(size, end);
  for (int i = start; i < end; ++i) {
    El_Copy_(FastEl_(i), it);
  }
}

void taFixedArray_impl::Insert_(const void* it, int where, int n) {
  if ((where > size) || (n <= 0)) return; // errors
  if (where < 0) where = size; // -1 means at end
  if(!Alloc_(size + n)) return;	// pre-add stuff

  int i;
  // if not appending, move the items
  if ((where < size)) { 
    int n_mv = size - where;	// number that must be moved
    for (i = size - 1; i >= where; --i)	{	// shift everyone over
      El_Copy_(FastEl_(i + n_mv), FastEl_(i));
    }
  }
  // write the new items
  for (i=where; i < where + n; ++i)
    El_Copy_(FastEl_(i), it);
  size += n;
}

const void* taFixedArray_impl::SafeEl_(int i) const {
  i=Index(i);
  if (InRange(i)) return FastEl_(i); 
  else            return El_GetErr_();
}


//////////////////////////
//  taArray_impl	//
//////////////////////////


void taArray_impl::Clear_Tmp_() {
  String val;
  El_SetFmStr_(El_GetTmp_(), val);
}

void taArray_impl::AddOnly_(const void* it) {
  if (size >= alloc_size) {
    if(!Alloc(size+1))
      return;
  }
  El_Copy_(FastEl_(size++), it);
}

void taArray_impl::Add_(const void* it) {
  AddOnly_(it);
  DataChanged(DCR_ARY_SIZE_CHANGED);
}

bool taArray_impl::AddUniqueOnly_(const void* it) {
  if(FindEl_(it) >= 0)
    return false;
  AddOnly_(it);
  return true;
}

bool taArray_impl::AddUnique_(const void* it) {
  if(FindEl_(it) >= 0)
    return false;
  Add_(it);
  return true;
}

bool taArray_impl::Alloc(int sz) {
  if (alloc_size < sz)	{
    // start w/ 4, double up to 64, then 1.5x thereafter
    if (alloc_size == 0) alloc_size = MAX(4, sz);
    else if (alloc_size < 64) alloc_size = MAX((alloc_size * 2), sz);
    else alloc_size =  MAX(((alloc_size * 3) / 2) , sz);
    char* nw = (char*)MakeArray_(alloc_size);
    if(!nw) {
      taMisc::Error("taArray_impl::Alloc -- malloc error -- array too big! this may be fatal");
      return false;
    }
    for (int i = 0; i < size; ++i) {
      El_Copy_(nw + (El_SizeOf_() * i), FastEl_(i));
    }
    SetArray_(nw);
  }
  return true;
}

void taArray_impl::AddBlank(int n_els) {
  if (n_els < 0) n_els = 0;
  SetSize(size + n_els);
}

void taArray_impl::Copy_(const taArray_impl& cp) {
  if (cp.size < size) ReclaimOrphans_(cp.size, size - 1);
  else if (cp.size > alloc_size) {
    if(!Alloc(cp.size)) return;
  }
  
  for (int i=0; i < cp.size; ++i) {
    El_Copy_(FastEl_(i), cp.FastEl_(i));
  }
  size = cp.size;
}

void taArray_impl::SetSize(int new_size) {
  if (new_size < 0) new_size = 0;
  if (new_size == size) return; 
  else if (new_size > size) {
    if(!Alloc(new_size)) return;
    Clear_Tmp_();
    Insert_(El_GetTmp_(), size, new_size - size);
  } else if (new_size < size)  {
    ReclaimOrphans_(new_size, size - 1);
  }
  size = new_size;
  DataChanged(DCR_ARY_SIZE_CHANGED);
}

bool taArray_impl::Equal_(const taArray_impl& ar) const {
  if (size != ar.size) return false;
  for (int i = 0; i < size; ++i)
    if (!El_Equal_(FastEl_(i), ar.FastEl_(i))) return false;
  return true;
}

int taArray_impl::FindEl_(const void* it, int where) const {
  int i;
  for(i=where; i<size; i++) {
    if(El_Compare_(it, FastEl_(i)) == 0)
      return i;
  }
  return -1;
}

void taArray_impl::InitVals_(const void* it, int start, int end) {
  if(end == -1)	end = size;  else end = MIN(size, end);
  int i;
  for(i=start;i<end;i++) {
    El_Copy_(FastEl_(i), it);
  }
}

void taArray_impl::Insert_(const void* it, int where, int n) {
  if((where > size) || (n <= 0)) return;
  if ((size + n) > alloc_size) {
    if(!Alloc(size + n)) return;	// pre-add stuff
  }
  if((where==size) || (where < 0)) {
    int i;
    for (i=0; i<n; i++) AddOnly_(it);
    DataChanged(DCR_ARY_SIZE_CHANGED);
    return;
  }
  int i;
  int n_mv = size - where;	// number that must be moved
  size += n;
  int trg_o = size-1;
  int src_o = size-1-n;
  for(i=0; i<n_mv; i++)		// shift everyone over
    El_Copy_(FastEl_(trg_o - i), FastEl_(src_o - i));
  for(i=where; i<where+n; i++)
    El_Copy_(FastEl_(i), it);
  DataChanged(DCR_ARY_SIZE_CHANGED);
}

bool taArray_impl::MoveIdx(int fm, int to) {
  if((size == 0) || (fm >= size) || (to >= size)) return false;

  void* tmp = El_GetTmp_();
  El_Copy_(tmp, FastEl_(fm));
  int j;
  for(j=fm; j < size-1; j++) {		// compact, if necc
    El_Copy_(FastEl_(j), FastEl_(j+1));
  }
  for(j=size-1; j>to; j--) {
    El_Copy_(FastEl_(j), FastEl_(j-1));
  }
  El_Copy_(FastEl_(to), tmp);
  return true;
}

bool taArray_impl::SwapIdx(int pos1, int pos2) {
  if ((pos1 < 0) || (pos2 < 0) || (pos1 >= size) || (pos2 >= size)) return false;
  if (pos1 == pos2) return true;
  void* tmp = El_GetTmp_();
  El_Copy_(tmp, FastEl_(pos1));
  El_Copy_(FastEl_(pos1), FastEl_(pos2));
  El_Copy_(FastEl_(pos2), tmp);
  return true;
}

void taArray_impl::Permute() {
  int i, nv;
  void* tmp = El_GetTmp_();
  for(i=0; i<size; i++) {
    nv = (int) ((MTRnd::genrand_int32() % (size - i)) + i); // get someone from the future
    El_Copy_(tmp, FastEl_(i));
    El_Copy_(FastEl_(i), FastEl_(nv));	// swap with yourself
    El_Copy_(FastEl_(nv), tmp);
  }
}

bool taArray_impl::RemoveEl_(const void* it) {
  int i;
  if((i = FindEl_(it)) < 0)
    return false;
  return RemoveIdx(i);
}

bool taArray_impl::RemoveIdx(uint i, int n) {
  if((int)i >= size) return false;
  n = MIN(n, size-(int)i);
  int j;
  for(j=i; j < size-n; j++)
    El_Copy_(FastEl_(j), FastEl_(j+n));
  size -= n;
  DataChanged(DCR_ARY_SIZE_CHANGED);
  return true;
}

void taArray_impl::RemoveIdxOnly(int i) {
  for(int j=i; j < size-1; j++)
    El_Copy_(FastEl_(j), FastEl_(j+1));
  size -= 1;
}

const void* taArray_impl::SafeEl_(int i) const {
  i = Index(i);
  if (InRange(i)) return ((taArray_impl*)this)->FastEl_(i); //safe const cast
  else            return El_GetErr_();
}

void taArray_impl::Sort(bool descending) {
  if(size <= 1) return;
  int lt_compval = -1;		// comparison return value for less-than
  if(descending)
    lt_compval = 1;
  // lets do a heap sort since it requires no secondary storage
  int n = size;
  int l,j,ir,i;
  void* tmp = El_GetTmp_();

  l = (n >> 1)+1;
  ir = n;
  for(;;){
    if(l>1)
      El_Copy_(tmp,FastEl_(--l -1)); // tmp = ra[--l]
    else {
      El_Copy_(tmp,FastEl_(ir-1)); // tmp = ra[ir]
      El_Copy_(FastEl_(ir-1),FastEl_(0)); // ra[ir] = ra[1]
      if(--ir == 1) {
	El_Copy_(FastEl_(0),tmp); // ra[1]=tmp
	return;
      }
    }
    i=l;
    j=l << 1;
    while(j<= ir) {
      if(j<ir && (El_Compare_(FastEl_(j-1),FastEl_(j)) == lt_compval)) j++;
      if(El_Compare_(tmp,FastEl_(j-1)) == lt_compval) { // tmp < ra[j]
	El_Copy_(FastEl_(i-1),FastEl_(j-1)); // ra[i]=ra[j];
	j += (i=j);
      }
      else j = ir+1;
    }
    El_Copy_(FastEl_(i-1),tmp); // ra[i] = tmp;
  }
}

void taArray_impl::ShiftLeft(int nshift) {
  if (nshift <= 0) return;
  if (nshift >= size) {
    Reset();
    return;
  }

  int i;
  for(i=0; i < size - nshift; i++) {
    El_Copy_(FastEl_(i), FastEl_(i+nshift)); // move left..
  }
  size = size - nshift;		// update the size now..
  DataChanged(DCR_ARY_SIZE_CHANGED);
}

void taArray_impl::ShiftLeftPct(float pct) {
  int nshift = (int)(pct * (float)size);
  if(nshift == 0)    nshift = 1;
  ShiftLeft(nshift);
}

int taArray_impl::V_Flip(int width){
  if (size % width) return false;
  int size_orig = size;
  int from_end,from_start;
  for(from_end = size-width,from_start = 0;
      (from_start <from_end);
      from_end -= width, from_start += width){
    int j;
    for(j=0;j<width;j++){
      AddOnly_(FastEl_(from_start+j));
      El_Copy_(FastEl_(from_start+j),FastEl_(from_end+j));
      El_Copy_(FastEl_(from_end+j),FastEl_(size-1));
      RemoveIdxOnly(size-1);
    }
  }
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
  return true;
}

void taArray_impl::Duplicate(const taArray_impl& cp) {
  int i;
  int size_orig = size;
  for(i=0; i<cp.size; i++)
    AddOnly_(cp.FastEl_(i));
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
}

void taArray_impl::DupeUnique(const taArray_impl& cp) {
  int i;
  int size_orig = size;
  for(i=0; i<cp.size; i++)
    AddUniqueOnly_(cp.FastEl_(i));
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
}

void taArray_impl::Copy_Common(const taArray_impl& cp) {
  int i;
  int mx_idx = MIN(size, cp.size);
  for(i=0; i<mx_idx; i++)
    El_Copy_(FastEl_(i), cp.FastEl_(i));
}

void taArray_impl::Copy_Duplicate(const taArray_impl& cp) {
  Copy_Common(cp);
  int size_orig = size;
  for(int i=size; i<cp.size; i++)
    AddOnly_(cp.FastEl_(i));
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
}

void taArray_impl::CopyVals(const taArray_impl& from, int start, int end, int at) {
  if(end == -1)	end = from.size;  else end = MIN(from.size, end);
  int len = end - start;
  if(size < at + len)
    SetSize(at + len);
  int i, trg;
  for(i=start, trg=at;i<end;i++, trg++)
    El_Copy_(FastEl_(trg), from.FastEl_(i));
}

void taArray_impl::List(ostream& strm) const {
  strm << "[" << size << "] {";
  for(int i=0;i<size;i++) {
    strm << " " << El_GetStr_(FastEl_(i)) << ",";
    if(i+1 % 8 == 0) {
      strm << endl;
      taMisc::FlushConsole();
    }
  }
  strm << "}";
}

String taArray_impl::GetArrayStr() const {
  String rval = "[" + String(size) + "] {";
  for(int i=0;i<size;i++) {
    rval += " " + El_GetStr_(FastEl_(i)) + ",";
  }
  rval += "}";
  return rval;
}

void taArray_impl::InitFromString(const String& val) {
  int size_orig = size;
  String tmp = val;
  Reset_impl();
  tmp = tmp.after('{');		// find starting point
  while(tmp.length() > 0) {
    String el_val = tmp.before(',');
    if(el_val.empty()) {
      el_val = tmp.before('}');
      if (el_val.empty())
	break;
    }
    tmp = tmp.after(',');
    if (el_val.contains(' '))
      el_val = el_val.after(' ');
    AddOnly_(El_GetTmp_());		// add a blank
    El_SetFmStr_(FastEl_(size-1), String(el_val));
  }
  if (size_orig != size)
    DataChanged(DCR_ARY_SIZE_CHANGED);
}


/////////////////////////////////////////////// 
// String-Based Text Diff Algorithm

// C++ version of C# code by Matthias Hertel: 
// http://www.mathertel.de/Diff

// This Class implements the Difference Algorithm published in
// "An O(ND) Difference Algorithm and its Variations" by Eugene Myers
// Algorithmica Vol. 1 No. 2, 1986, p 251.  
// 
// There are many C, Java, Lisp implementations public available but they all
// seem to come from the same source (diffutils) that is under the (unfree)
// GNU public License and cannot be reused as a sourcecode for a commercial
// application.  There are very old C implementations that use other (worse)
// algorithms.  Microsoft also published sourcecode of a diff-tool (windiff)
// that uses some tree data.  Also, a direct transfer from a C source to C# is
// not easy because there is a lot of pointer arithmetic in the typical C
// solutions and i need a managed solution.  These are the reasons why I
// implemented the original published algorithm from the scratch and make it
// avaliable without the GNU license limitations.  I do not need a high
// performance diff tool because it is used only sometimes.  I will do some
// performace tweaking when needed.
// 
// The algorithm itself is comparing 2 arrays of numbers so when comparing 2
// text documents each line is converted into a (hash) number. See DiffText().
// 
// Some chages to the original algorithm: The original algorithm was described
// using a recursive approach and comparing zero indexed arrays.  Extracting
// sub-arrays and rejoining them is very performance and memory intensive so
// the same (readonly) data arrays are passed arround together with their
// lower and upper bounds.  This circumstance makes the LCS and SMS functions
// more complicate.  I added some code to the LCS function to get a fast
// response on sub-arrays that are identical, completely deleted or inserted.
// 
// The result from a comparisation is stored in 2 arrays that flag for
// modified (deleted or inserted) lines in the 2 data arrays. These bits are
// then analysed to produce a array of Item objects.
// 
// Further possible optimizations: (first rule: don't do it; second: don't do
// it yet) The arrays DataA and DataB are passed as parameters, but are never
// changed after the creation so they can be members of the class to avoid the
// paramter overhead.  In SMS is a lot of boundary arithmetic in the for-D and
// for-k loops that can be done by increment and decrement of local variables.
// The DownVector and UpVector arrays are alywas created and destroyed each
// time the SMS gets called.  It is possible to reuse tehm when transfering
// them to members of the class.  See TODO: hints.
// 
// diff.cs: A port of the algorythm to C# Copyright (c) by Matthias Hertel,
// http://www.mathertel.de This work is licensed under a BSD style
// license. See http://www.mathertel.de/License.aspx

// Copyright (c) 2005-2007 by Matthias Hertel, http://www.mathertel.de/

// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.  Redistributions in binary
// form must reproduce the above copyright notice, this list of conditions and
// the following disclaimer in the documentation and/or other materials
// provided with the distribution.  Neither the name of the copyright owners
// nor the names of its contributors may be used to endorse or promote
// products derived from this software without specific prior written
// permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// 
// Changes: 2002.09.20 There was a "hang" in some situations.  Now I undestand
// a little bit more of the SMS algorithm.  There have been overlapping boxes;
// that where analyzed partial differently.  One return-point is enough.  A
// assertion was added in CreateDiffs when in debug-mode, that counts the
// number of equal (no modified) lines in both arrays.  They must be
// identical.
// 
// 2003.02.07 Out of bounds error in the Up/Down vector arrays in some
// situations.  The two vetors are now accessed using different offsets that
// are adjusted using the start k-Line.  A test case is added.
// 
// 2006.03.05 Some documentation and a direct Diff entry point.
// 
// 2006.03.08 Refactored the API to static methods on the Diff class to make
// usage simpler.  2006.03.10 using the standard Debug class for self-test
// now.  compile with: csc /target:exe /out:diffTest.exe /d:DEBUG /d:TRACE
// /d:SELFTEST Diff.cs 2007.01.06 license agreement changed to a BSD style
// license.  2007.06.03 added the Optimize method.  2007.09.23 UpVector and
// DownVector optimization by Jan Stoklasa ().  2008.05.31 Adjusted the
// testing code that failed because of the Optimize method (not a bug in the
// diff algorithm).  2008.10.08 Fixing a test case and adding a new test case.

// Find the difference in 2 text documents, comparing by textlines.  The
// algorithm itself is comparing 2 arrays of numbers so when comparing 2 text
// documents each line is converted into a (hash) number. This hash-value is
// computed by storing all textlines into a common hashtable so i can find
// dublicates in there, and generating a new number each time a new textline
// is inserted.

void taStringDiff::DiffStrings(const String& str_a, const String& str_b,
			       bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  GetLines(data_a, str_a); // get starting line positions within strings
  GetLines(data_b, str_b); // get starting line positions within strings

  hash_codes.Alloc(MAX(data_a.lines, data_b.lines)); // max should be more than enough
  cur_hash_idx = 0;

  // The A-Version of the data (original data) to be compared.
  DiffCodes(data_a, str_a, trimSpace, ignoreSpace, ignoreCase);
  data_a.InitFmData();

  // The B-Version of the data (modified data) to be compared.
  DiffCodes(data_b, str_b,trimSpace, ignoreSpace, ignoreCase);
  data_b.InitFmData();

  int max_len = data_a.lines + data_b.lines + 1;
  down_vector.SetSize(2 * max_len + 2);
  up_vector.SetSize(2 * max_len + 2);

  LCS(0, data_a.lines, 0, data_b.lines);

  Optimize(data_a);
  Optimize(data_b);
  CreateDiffs(str_a, str_b);
}

void taStringDiff::ReDiffB(const String& str_a, const String& str_b,
		       bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  GetLines(data_b, str_b); // get starting line positions within strings

  data_a.modified.InitVals(0);

  // The B-Version of the data (modified data) to be compared.
  DiffCodes(data_b, str_b, trimSpace, ignoreSpace, ignoreCase);
  data_b.InitFmData();

  int max_len = data_a.lines + data_b.lines + 1;
  down_vector.SetSize(2 * max_len + 2);
  up_vector.SetSize(2 * max_len + 2);

  LCS(0, data_a.lines, 0, data_b.lines);

  Optimize(data_a);
  Optimize(data_b);
  CreateDiffs(str_a, str_b);
}

bool taStringDiff::DiffFiles(const String& fname_a, const String& fname_b,
			     String& str_a, String& str_b,
			     bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  bool rval = false;
  fstream istrm;
  int err;
  istrm.open(fname_a.chars(), ios::in);
  if(!istrm.is_open()) goto exit;
  err = str_a.Load_str(istrm);
  if(err) goto exit;

  istrm.close();
  istrm.open(fname_b.chars(), ios::in);
  if(!istrm.is_open()) goto exit;
  err = str_b.Load_str(istrm);
  if(err) goto exit;
  istrm.close();

  DiffStrings(str_a, str_b, trimSpace, ignoreSpace, ignoreCase);

exit:
  istrm.close();
  return rval;
}

void taStringDiff::GetLines(taStringDiffData& ddata, const String& str) {
  ddata.line_st.Reset();
  int st_ln = 0;
  int i;
  for(i=0; i< str.length(); i++) {
    if(str[i] != '\n') continue;
    ddata.line_st.Add(st_ln);
    st_ln = i+1;
  }
  if(st_ln != i) {
    ddata.line_st.Add(st_ln);
  }
  ddata.lines = ddata.line_st.size;
}

// This function converts all textlines of the text into unique numbers for
// every unique textline so further work can work only with simple numbers.
void taStringDiff::DiffCodes(taStringDiffData& ddata, const String& str, 
			     bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  ddata.data.Reset();
  for(int i=0; i< ddata.lines; i++) {
    String cur_ln = ddata.GetLine(str, i);
    if(trimSpace)
      cur_ln = trim(cur_ln);
    if(ignoreSpace) {
      cur_ln.gsub(" ", "");
      cur_ln.gsub("\t", "");
    }
    if(ignoreCase)
      cur_ln.downcase();
    taHashVal hash = taPtrList_impl::HashCode_String(cur_ln);
    int val = hash_codes.FindHashVal(hash);
    if(val < 0) {
      val = cur_hash_idx++;
      hash_codes.AddHash(hash, val);
    }
    ddata.data.Add(val);
  }
}

// If a sequence of modified lines starts with a line that contains the same
// content as the line that appends the changes, the difference sequence is
// modified so that the appended line and not the starting line is marked as
// modified.  This leads to more readable diff sequences when comparing text
// files.

void taStringDiff::Optimize(taStringDiffData& ddata) {
  int StartPos, EndPos;

  StartPos = 0;
  while (StartPos < ddata.lines) {
    while ((StartPos < ddata.lines) && !ddata.GetModified(StartPos))
      StartPos++;
    EndPos = StartPos;
    while ((EndPos < ddata.lines) && ddata.GetModified(EndPos))
      EndPos++;

    if ((EndPos < ddata.lines) && (ddata.data[StartPos] == ddata.data[EndPos])) {
      ddata.SetModified(StartPos, false);
      ddata.SetModified(EndPos, true);
    } else {
      StartPos = EndPos;
    } // if
  } // while
}


// Find the difference in 2 arrays of integers.
void taStringDiff::DiffInts(const int_PArray& array_a, const int_PArray& array_b) {
  // The A-Version of the data (original data) to be compared.
  data_a.data = array_a;
  data_a.InitFmData();

  // The B-Version of the data (modified data) to be compared.
  data_b.data = array_b;
  data_b.InitFmData();

  int max_len = data_a.lines + data_b.lines + 1;
  down_vector.SetSize(2 * max_len + 2);
  up_vector.SetSize(2 * max_len + 2);

  LCS(0, data_a.lines, 0, data_b.lines);

  CreateDiffs("", "");		// null strings
} 


// This is the algorithm to find the Shortest Middle Snake (SMS).

void taStringDiff::SMS(int& sms_x, int& sms_y, int lower_a, int upper_a, int lower_b, int upper_b) {
  int max_len = data_a.lines + data_b.lines + 1;

  int down_k = lower_a - lower_b; // the k-line to start the forward search
  int up_k = upper_a - upper_b; // the k-line to start the reverse search

  int delta = (upper_a - lower_a) - (upper_b - lower_b);
  bool odddelta = (delta & 1) != 0;

  // The vectors in the publication accepts negative indexes. the vectors
  // implemented here are 0-based and are access using a specific offset:
  // up_off up_vector and down_off for DownVektor
  int down_off = max_len - down_k;
  int up_off = max_len - up_k;

  int max_d = ((upper_a - lower_a + upper_b - lower_b) / 2) + 1;

  // init vectors
  down_vector[down_off + down_k + 1] = lower_a;
  up_vector[up_off + up_k - 1] = upper_a;

  for (int d = 0; d <= max_d; d++) {

    // Extend the forward path.
    for (int k = down_k - d; k <= down_k + d; k += 2) {
      // find the only or better starting point
      int x, y;
      if (k == down_k - d) {
	x = down_vector[down_off + k + 1]; // down
      } else {
	x = down_vector[down_off + k - 1] + 1; // a step to the right
	if ((k < down_k + d) && (down_vector[down_off + k + 1] >= x))
	  x = down_vector[down_off + k + 1]; // down
      }
      y = x - k;

      // find the end of the furthest reaching forward d-path in diagonal k.
      while ((x < upper_a) && (y < upper_b) && (data_a.data[x] == data_b.data[y])) {
	x++; y++;
      }
      down_vector[down_off + k] = x;

      // overlap ?
      if (odddelta && (up_k - d < k) && (k < up_k + d)) {
	if (up_vector[up_off + k] <= down_vector[down_off + k]) {
	  sms_x = down_vector[down_off + k];
	  sms_y = down_vector[down_off + k] - k;
	  return;
	} // if
      } // if

    } // for k

    // Extend the reverse path.
    for (int k = up_k - d; k <= up_k + d; k += 2) {
      // find the only or better starting point
      int x, y;
      if (k == up_k + d) {
	x = up_vector[up_off + k - 1]; // up
      } else {
	x = up_vector[up_off + k + 1] - 1; // left
	if ((k > up_k - d) && (up_vector[up_off + k - 1] < x))
	  x = up_vector[up_off + k - 1]; // up
      } // if
      y = x - k;

      while ((x > lower_a) && (y > lower_b) && (data_a.data[x - 1] == data_b.data[y - 1])) {
	x--; y--; // diagonal
      }
      up_vector[up_off + k] = x;

      // overlap ?
      if (!odddelta && (down_k - d <= k) && (k <= down_k + d)) {
	if (up_vector[up_off + k] <= down_vector[down_off + k]) {
	  sms_x = down_vector[down_off + k];
	  sms_y = down_vector[down_off + k] - k;
	  return;
	} // if
      } // if

    } // for k

  } // for d

  taMisc::Error("taStringDiff: the algorithm should never come here!");
} 

// This is the divide-and-conquer implementation of the longes
// common-subsequence (LCS) algorithm.  The published algorithm passes
// recursively parts of the A and B sequences.  To avoid copying these
// arrays the lower and upper bounds are passed while the sequences stay
// constant.

void taStringDiff::LCS(int lower_a, int upper_a, int lower_b, int upper_b) {
  // Fast walkthrough equal lines at the start
  while (lower_a < upper_a && lower_b < upper_b && data_a.data[lower_a] == data_b.data[lower_b]) {
    lower_a++; lower_b++;
  }

  // Fast walkthrough equal lines at the end
  while (lower_a < upper_a && lower_b < upper_b && data_a.data[upper_a - 1] == data_b.data[upper_b - 1]) {
    --upper_a; --upper_b;
  }

  if (lower_a == upper_a) {
    // mark as inserted lines.
    while (lower_b < upper_b)
      data_b.SetModified(lower_b++, true);

  } else if (lower_b == upper_b) {
    // mark as deleted lines.
    while (lower_a < upper_a)
      data_a.SetModified(lower_a++, true);

  } else {
    // Find the middle snakea and length of an optimal path for A and B
    int sms_x, sms_y;
    SMS(sms_x, sms_y, lower_a, upper_a, lower_b, upper_b);

    // The path is from LowerX to (x,y) and (x,y) to UpperX
    LCS(lower_a, sms_x, lower_b, sms_y);
    LCS(sms_x, upper_a, sms_y, upper_b);  // 2002.09.20: no need for 2 points 
  }
} 


// Scan the tables of which lines are inserted and deleted,
// producing an edit script in forward order.  

void taStringDiff::CreateDiffs(const String& str_a, const String& str_b) {
  diffs.Reset();

  int start_a, start_b;
  int line_a = 0;
  int line_b = 0;

  while (line_a < data_a.lines || line_b < data_b.lines) {
    if ((line_a < data_a.lines) && (!data_a.GetModified(line_a))
	&& (line_b < data_b.lines) && (!data_b.GetModified(line_b))) {
      // equal lines
      line_a++;
      line_b++;

    } else {
      // maybe deleted and/or inserted lines
      start_a = line_a;
      start_b = line_b;

      while (line_a < data_a.lines && (line_b >= data_b.lines || data_a.GetModified(line_a)))
	// while (line_a < data_a.lines && data_a.GetModified(line_a])
	line_a++;

      while (line_b < data_b.lines && (line_a >= data_a.lines || data_b.GetModified(line_b)))
	// while (line_b < data_b.lines && data_b.GetModified(line_b])
	line_b++;

      if ((start_a < line_a) || (start_b < line_b)) {
	taStringDiffItem nw_itm;
	// store a new difference-item
	nw_itm.start_a = start_a;
	nw_itm.start_b = start_b;
	nw_itm.delete_a = line_a - start_a;
	nw_itm.insert_b = line_b - start_b;
	if(str_b.nonempty() && nw_itm.insert_b > 0) {
	  nw_itm.insert_b_str = data_b.GetLine(str_b, start_b, line_b-1);
	}
	diffs.Add(nw_itm);
      } // if
    } // if
  } // while
}

void taStringDiff::GetEdits(taStringDiffEdits& edits) {
  edits.diffs.Reset();
  edits.diffs = diffs;
  edits.line_st.Reset();
  edits.line_st = data_a.line_st;
}

String taStringDiff::GetDiffStr(const String& str_a, const String& str_b, OutputFmt fmt) {
  if(diffs.size == 0) return "No differences found!\n";
  if(fmt == NORMAL) {
    return GetDiffStr_normal(str_a, str_b);
  }
  else {			// context
    return GetDiffStr_context(str_a, str_b);
  }
}

static String string_diff_get_diff_range(int st, int rg) {
  if(rg > 1)
    return "," + String(st + rg);
  return _nilString;
}

String taStringDiff::GetDiffStr_normal(const String& str_a, const String& str_b) {
  String rval;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    bool chg = false;
    if(df.delete_a == df.insert_b) {
      rval += String(df.start_a+1) + "c" + String(df.start_b+1) +
	string_diff_get_diff_range(df.start_b, df.insert_b) + "\n";
      chg = true;
    }
    if(df.delete_a > 0) {
      if(!chg) {
	rval += String(df.start_a+1) +
	  string_diff_get_diff_range(df.start_a, df.delete_a)
	  + "d" + String(df.start_b+1) + "\n";
      }
      for(int l=df.start_a; l<df.start_a + df.delete_a; l++)
	rval += "< " + data_a.GetLine(str_a, l) + "\n";
      if(chg) {
	rval += "---\n";
      }
    }
    if(df.insert_b > 0) {
      if(!chg) {
	rval += String(df.start_a+1) + "a" + String(df.start_b+1) +
	  string_diff_get_diff_range(df.start_b, df.insert_b) + "\n";
      }
      for(int l=df.start_b; l<df.start_b + df.insert_b; l++)
	rval += "> " + data_b.GetLine(str_b, l) + "\n";
    }
  }
  return rval;
}

String taStringDiff::GetDiffStr_context(const String& str_a, const String& str_b) {
  return "Context format not currently supported -- please use NORMAL\n";
}

int taStringDiff::GetLinesChanged() {
  int tot = 0;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    tot += df.insert_b + df.delete_a;
  }
  return tot;
}

void taStringDiff::Reset() {
  diffs.Reset();
  hash_codes.Reset();
  cur_hash_idx = 0;
  data_a.Reset();
  data_b.Reset();
  down_vector.Reset();
  up_vector.Reset();
}


String taStringDiffEdits::GenerateB(const String& str_a) {
  String rval(0, str_a.length(), '\0'); // pre-alloc buffer
  int last_a = 0;
//  int last_b = 0;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    String alns = GetLine(str_a, last_a, df.start_a-1);
    rval.cat(alns).cat("\n");
    last_a = df.start_a + df.delete_a; // skip over deleted lines
    if(df.insert_b > 0)
      rval.cat(df.insert_b_str).cat("\n");
  }
  if(last_a < line_st.size-1) {
    String alns = GetLine(str_a, last_a, line_st.size-1);
    rval.cat(alns).cat("\n");
  }
  return rval;
}

String taStringDiffEdits::GetDiffStr(const String& str_a) {
  String rval;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    bool chg = false;
    if(df.delete_a == df.insert_b) {
      rval += String(df.start_a+1) + "c" + String(df.start_b+1) +
	string_diff_get_diff_range(df.start_b, df.insert_b) + "\n";
      chg = true;
    }
    if(df.delete_a > 0) {
      if(!chg) {
	rval += String(df.start_a+1) +
	  string_diff_get_diff_range(df.start_a, df.delete_a)
	  + "d" + String(df.start_b+1) + "\n";
      }
      if(str_a.nonempty()) {
	for(int l=df.start_a; l<df.start_a + df.delete_a; l++)
	  rval += "< " + GetLine(str_a, l) + "\n";
	if(chg) {
	  rval += "---\n";
	}
      }
    }
    if(df.insert_b > 0) {
      if(!chg) {
	rval += String(df.start_a+1) + "a" + String(df.start_b+1) +
	  string_diff_get_diff_range(df.start_b, df.insert_b) + "\n";
      }
      for(int l=df.start_b; l<df.start_b + df.insert_b; l++)
	rval += "> " + df.insert_b_str + "\n"; // todo: need to line-a-fy with > 
    }
  }
  return rval;
}

int taStringDiffEdits::GetLinesChanged() {
  int tot = 0;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    tot += df.insert_b + df.delete_a;
  }
  return tot;
}

