// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


// ta_group.cc

#include <sstream>

#include "ta_group.h"
#include "ta_TA_type.h"

#ifdef TA_GUI
# include "ta_qtgroup.h"
# include "ta_qtclipdata.h"
#endif



////////////////////////////
//      taSubGroup  	  //
////////////////////////////

// not only sub-groups cause dirtiness, because if you add an el to a
// previously null group, it needs added, etc..

void taSubGroup::DataChanged(int dcr, void* op1, void* op2) {
  if (owner == NULL) return;
  // send LIST events to the owning group as a GROUP_ITEM event
  if ((dcr >= DCR_LIST_ITEM_TO_GROUP_MIN) && (dcr <= DCR_LIST_ITEM_TO_GROUP_MAX))
    ((TAGPtr)owner)->DataChanged(dcr + DCR_ListItem_Group_Offset, op1, op2);
}

bool taSubGroup::Transfer(taBase* it) {
  // need to leaf count on parent group
  TAGPtr myown = (TAGPtr)owner;
  taGroup_impl* git = (taGroup_impl*)it;
  if((git->super_gp == myown) || (git->super_gp == NULL))
    return false;
  taGroup_impl* old_own = git->super_gp;
  bool rval = TALOG::Transfer(git);
  //TODO: notification is not right, because counts are not rejigged yet!
  if (rval) {
    old_own->UpdateLeafCount_(-git->leaves);
    if(myown != NULL) {
      myown->UpdateLeafCount_(git->leaves);
    }
  }
  return rval;
}


////////////////////////////
//      taGroup_impl  	  //
////////////////////////////


void taGroup_impl::Initialize() {
  leaves = 0;
  super_gp = NULL;
  leaf_gp = NULL;
  root_gp = NULL;
}

void taGroup_impl::Destroy() {
  if (leaf_gp) {
    taBase::unRefDone(leaf_gp);
    leaf_gp = NULL;
  }
  RemoveAll();
  CutLinks();
}

void taGroup_impl::InitLinks() {
  inherited::InitLinks();
  gp.SetBaseType(GetTypeDef());	// more of the same type of group
  taBase::Own(gp, this);

  super_gp = GetSuperGp_();
  if (super_gp) {
    root_gp = super_gp->root_gp;
    SetBaseType(super_gp->el_base);
    el_typ = super_gp->el_typ;
  } else {
    root_gp = this;
  }
}

void taGroup_impl::CutLinks() {
  root_gp = this; //prob not needed, but maintains the strict contract of root_gp not null
  inherited::CutLinks();
}

void taGroup_impl::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  //note: we have to process everyone, because this is the routine
  // that asserts or clears the state, even if an invalid found early
  for (int i = 0; i < gp.size; ++i) {
    taGroup_impl* child_gp = gp.FastEl(i);
    child_gp->CheckConfig(quiet, rval);
  }
}

void taGroup_impl::DataChanged(int dcr, void* op1, void* op2) {
  // group staling
  if (useStale() && (!taMisc::is_loading) && 
    ((dcr >= DCR_GROUP_MIN) && (dcr <= DCR_GROUP_MAX)))
    setStale();
  taList_impl::DataChanged(dcr, op1, op2); // normal processing
  // send LIST events to the root group as a GROUP_ITEM event
  if (root_gp && (dcr >= DCR_LIST_ITEM_TO_GROUP_ITEM_MIN) &&
     (dcr <= DCR_LIST_ITEM_TO_GROUP_ITEM_MAX)) {
    root_gp->DataChanged(dcr + DCR_ListItem_GroupItem_Offset, op1, op2);
  } 
  // GROUP_ITEM +/- and GROUP +/- events cause invalidation of the group iteration cache
  // have to trigger on items too, because iteration cache does funky stuff for size==0
  if ( (dcr == DCR_LIST_ITEM_INSERT) || (dcr == DCR_LIST_ITEM_REMOVE)
    || (dcr == DCR_GROUP_ITEM_INSERT) || (dcr == DCR_GROUP_ITEM_REMOVE)
    || ((dcr >= DCR_GROUP_MIN) && (dcr <= DCR_GROUP_MAX)) ) 
  {
    if (leaf_gp) {
      taBase::unRefDone(leaf_gp);
      leaf_gp = NULL;
    }
  }
}

void taGroup_impl::InitLeafGp() const {
  if (leaf_gp)
    return;
  taGroup_impl* ncths = (taGroup_impl*)this;
  ncths->leaf_gp = new TALOG;
  taBase::Own(leaf_gp, ncths);
  InitLeafGp_impl(ncths->leaf_gp);
}

void taGroup_impl::InitLeafGp_impl(TALOG* lg) const {
  if(size > 0)
    lg->Push((taGroup_impl*)this);
  int i;
  for(i=0; i<gp.size; i++)
    FastGp_(i)->InitLeafGp_impl(lg);
}

void taGroup_impl::AssignFrom_impl(const taBase* cpy_from_) {
  inherited::AssignFrom_impl(cpy_from_);
  const taGroup_impl* cpy_from = dynamic_cast<const taGroup_impl*>(cpy_from_);
  if (!cpy_from) return; // shouldn't happen
  // make gp sizes the same
  gp.SetSize(cpy_from->gp.size);
  // recursively make gp/item sizes same for the gps
  for (int i = 0; i < cpy_from->gp.size; ++i) {
    taGroup_impl* gp_cpy_to = gp.SafeEl(i);
    if (!gp_cpy_to) return; // shouldn't happen
    const taGroup_impl* gp_cpy_from = cpy_from->gp.FastEl(i); 
    gp_cpy_to->AssignFrom_impl(gp_cpy_from);
  }
  // UnSafeCopy should then take care of items and gps
}

void taGroup_impl::Copy_(const taGroup_impl& cp) {
  gp.Copy(cp.gp);
  leaves = cp.leaves; // prob not needed
}

void taGroup_impl::AddOnly_(void* it) {
  inherited::AddOnly_(it);
  UpdateLeafCount_(1);		// not the most efficient, but gets it at a low level
}

void taGroup_impl::Borrow(const taGroup_impl& cp) {
  inherited::Borrow(cp);
  gp.Borrow(cp.gp);
}

void taGroup_impl::BorrowUnique(const taGroup_impl& cp) {
  inherited::BorrowUnique(cp);
  gp.BorrowUnique(cp.gp);
}

void taGroup_impl::BorrowUniqNameOld(const taGroup_impl& cp) {
  inherited::BorrowUniqNameOld(cp);
  gp.BorrowUniqNameOld(cp.gp);
}

void taGroup_impl::BorrowUniqNameNew(const taGroup_impl& cp) {
  inherited::BorrowUniqNameNew(cp);
  gp.BorrowUniqNameNew(cp.gp);
}

void taGroup_impl::Copy_Common(const taGroup_impl& cp) {
  inherited::Copy_Common(cp);
  gp.Copy_Common(cp.gp);
}

void taGroup_impl::Copy_Duplicate(const taGroup_impl& cp) {
  inherited::Copy_Duplicate(cp);
  gp.Copy_Duplicate(cp.gp);
}

void taGroup_impl::Copy_Borrow(const taGroup_impl& cp) {
  inherited::Copy_Borrow(cp);
  gp.Copy_Borrow(cp.gp);
}

int taGroup_impl::SelectForEditSearch(const String& memb_contains, SelectEdit*& editor) {
  int nfound = inherited::SelectForEditSearch(memb_contains, editor);
  nfound += gp.SelectForEditSearch(memb_contains, editor);
  return nfound;
}

void taGroup_impl::SearchNameContains(const String& nm, taBase_PtrList& items,
				      taBase_PtrList* owners) {
  int st_sz = items.size;
  inherited::SearchNameContains(nm, items, owners);
  bool already_added_me = false;
  if(items.size > st_sz)
    already_added_me = true;
  gp.SearchNameContains(nm, items, owners);
  if(owners && (items.size > st_sz) && !already_added_me) { // we added somebody somewhere..
    owners->Link(this);
  }
}


void taGroup_impl::CompareSameTypeR(Member_List& mds, void_PArray& trg_bases,
				    void_PArray& src_bases, taBase* cp_base,
				    int show_forbidden, int show_allowed) {
  if(!cp_base) return;
  if(GetTypeDef() != cp_base->GetTypeDef()) return; // must be same type..
  inherited::CompareSameTypeR(mds, trg_bases, src_bases, cp_base, show_forbidden, show_allowed);
  taGroup_impl* cp_gp = (taGroup_impl*)cp_base;
  gp.CompareSameTypeR(mds, trg_bases, src_bases, &(cp_gp->gp), show_forbidden, show_allowed);
}

int taGroup_impl::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int nchg = inherited::UpdatePointers_NewPar(old_par, new_par);
  nchg += gp.UpdatePointers_NewPar(old_par, new_par);
  return nchg;
}

int taGroup_impl::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) {
  int nchg = inherited::UpdatePointers_NewParType(par_typ, new_par);
  nchg += gp.UpdatePointers_NewParType(par_typ, new_par);
  return nchg;
}

int taGroup_impl::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  int nchg = inherited::UpdatePointers_NewObj(old_ptr, new_ptr);
  nchg += gp.UpdatePointers_NewObj(old_ptr, new_ptr);
  return nchg;
}

int taGroup_impl::UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr) {
  int nchg = inherited::UpdatePointersToMyKids_impl(scope_obj, new_ptr);
  taGroup_impl* new_gp = (taGroup_impl*)new_ptr;
  for(int i=0; i<gp.size; i++) {
    taGroup_impl* osg = FastGp_(i);
    taGroup_impl* nsg = NULL;
    if(new_gp && (new_gp->gp.size > i))
      nsg= new_gp->FastGp_(i);
    nchg += osg->UpdatePointersToMe_impl(scope_obj, nsg);
  }
  return nchg;
}

int taGroup_impl::Dump_Save_PathR(ostream& strm, taBase* par, int indent) {
  return inherited::Dump_Save_PathR(strm, par, indent);
}

int taGroup_impl::Dump_Save_PathR_impl(ostream& strm, taBase* par, int indent) {
  int rval = inherited::Dump_Save_PathR_impl(strm, par, indent); // save first-level
  if(rval == false)
    rval = gp.Dump_Save_PathR_impl(strm, par, indent);
  // note that it must be relative to parent, not this
  else
    gp.Dump_Save_PathR_impl(strm, par, indent);
  return rval;
}

int taGroup_impl::Dump_SaveR(ostream& strm, taBase* par, int indent) {
  int rval = inherited::Dump_SaveR(strm, par, indent);
  gp.Dump_SaveR(strm, par, indent); // subgroups get saved -- relative to parent, not this
  return rval;
}

void taGroup_impl::DupeUniqNameOld(const taGroup_impl& cp) {
  taList_impl::DupeUniqNameOld(cp);
  gp.DupeUniqNameOld(cp.gp);
}

void taGroup_impl::DupeUniqNameNew(const taGroup_impl& cp) {
  taList_impl::DupeUniqNameNew(cp);
  gp.DupeUniqNameNew(cp.gp);
}

void taGroup_impl::Duplicate(const taGroup_impl& cp) {
  taList_impl::Duplicate(cp);
  gp.Duplicate(cp.gp);
}

void taGroup_impl::EnforceLeaves(int sz){
  if(sz > leaves)  New(sz - leaves,el_typ);
  while(leaves > sz) RemoveLeafIdx(leaves-1);
}

void taGroup_impl::EnforceSameStru(const taGroup_impl& cp) {
  taList_impl::EnforceSameStru(cp);
  gp.EnforceSameStru(cp.gp);
  int i;
  for(i=0; i<gp.size; i++) {
    FastGp_(i)->EnforceSameStru(*(cp.FastGp_(i)));
  }
}

int taGroup_impl::FindLeafEl(taBase* it) const {
  int idx;
  if((idx = FindEl(it)) >= 0)
    return idx;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if((idx = sbg->FindLeafEl(it)) >= 0)
      return idx + new_idx;
    new_idx += (int)sbg->leaves;
  }
  return -1;
}

taBase* taGroup_impl::FindLeafName_(const char* nm, int& idx) const {
  taBase* rval;
  if((rval = (taBase*)FindName_(nm,idx)))
    return rval;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if((rval =(taBase*) sbg->FindName_(nm, idx))) {
      idx += new_idx;
      return rval;
    }
    new_idx += (int)sbg->leaves;
  }
  idx = -1;
  return NULL;
}

taBase* taGroup_impl::FindLeafNameContains_(const String& nm, int& idx) const {
  taBase* rval;
  if((rval = (taBase*)FindNameContains_(nm,idx)))
    return rval;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if((rval =(taBase*) sbg->FindNameContains_(nm, idx))) {
      idx += new_idx;
      return rval;
    }
    new_idx += (int)sbg->leaves;
  }
  idx = -1;
  return NULL;
}

taBase* taGroup_impl::FindLeafType_(TypeDef* it, int& idx) const {
  taBase* rval;
  if((rval = FindType_(it,idx)))
    return rval;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if((rval = sbg->FindType_(it, idx))) {
      idx += new_idx;
      return rval;
    }
    new_idx += (int)sbg->leaves;
  }
  idx = -1;
  return NULL;
}

TAGPtr taGroup_impl::FindMakeGpName(const String& nm, TypeDef* typ) {
  TAGPtr rval = gp.FindName(nm);
  if(rval) return rval;
  rval = NewGp_(1, typ);
  rval->SetName(nm);
  return rval;
}

MemberDef* taGroup_impl::FindMembeR(const String& nm, void*& ptr) const {
  String idx_str = nm;
  idx_str = idx_str.before(']');
  if(idx_str != "") {
    idx_str = idx_str.after('[');
    int idx = atoi(idx_str);
    if((size == 0) || (idx >= size)) {
      ptr = NULL;
      return NULL;
    }
    ptr = el[idx];
    return ReturnFindMd();
  }

  int i;
  if((ptr = FindLeafName_(nm, i))) {
    return ReturnFindMd();
  }

  MemberDef* rval;
  if((rval = GetTypeDef()->members.FindNameAddrR(nm, (void*)this, ptr)) != NULL)
    return rval;
  int max_srch = MIN(taMisc::search_depth, size);
  for(i=0; i<max_srch; i++) {
    taBase* first_el = (taBase*)FastEl_(i);
    if((first_el != NULL) && // only search owned objects
       ((first_el->GetOwner()==NULL) || (first_el->GetOwner() == (taBase *) this))) {
      return first_el->FindMembeR(nm, ptr);
    }
  }
  ptr = NULL;
  return NULL;
}

MemberDef* taGroup_impl::FindMembeR(TypeDef* it, void*& ptr) const {
  int i;
  if((ptr = FindLeafType_(it, i))) {
    return ReturnFindMd();
  }

  MemberDef* rval;
  if((rval = GetTypeDef()->members.FindTypeAddrR(it, (void*)this, ptr)) != NULL)
    return rval;
  int max_srch = MIN(taMisc::search_depth, size);
  for(i=0; i<max_srch; i++) {
    taBase* first_el = (taBase*)FastEl_(i);
    if((first_el != NULL) && // only search owned objects
       ((first_el->GetOwner()==NULL) || (first_el->GetOwner() == (taBase *) this))) {
      return first_el->FindMembeR(it, ptr);
    }
  }
  ptr = NULL;
  return NULL;
}

TAGPtr taGroup_impl::GetSuperGp_() {
  if(owner == NULL)
    return NULL;
  if(owner->InheritsFrom(TA_taList)) {
    taBase* ownr = owner->GetOwner();
    if((ownr != NULL) && (ownr->InheritsFrom(TA_taGroup_impl)))
      return (TAGPtr)ownr;
  }
  return NULL;
}

String taGroup_impl::GetValStr(const TypeDef* td, void* par,
	MemberDef* memb_def) const
{
  String nm = " Size: ";
  nm += String(size);
  if(gp.size > 0)
    nm += String(".") + String(gp.size);
  if(leaves != size)
    nm += String(".") + String((int) leaves);
  nm += String(" (") + el_typ->name + ")";
  return nm;
 }

taBase* taGroup_impl::Leaf_(int idx) const {
  if(idx >= leaves)
    return NULL;
  if(size && (idx < size))
    return (taBase*)el[idx];

  int nw_idx = (int)idx - size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if(sbg->leaves && (sbg->leaves > nw_idx))
      return sbg->Leaf_(nw_idx);
    nw_idx -= (int)sbg->leaves;
  }
  return NULL;
}

TAGPtr taGroup_impl::LeafGp_(int leaf_idx) const {
  if(leaf_idx >= leaves)
    return NULL;
  if(size && (leaf_idx < size))
    return (TAGPtr)this;

  int nw_idx = (int)leaf_idx - size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if(sbg->leaves && (sbg->leaves > nw_idx))
      return sbg->LeafGp_(nw_idx);
    nw_idx -= (int)sbg->leaves;
  }
  return NULL; 
}

void taGroup_impl::List(ostream& strm) const {
  taList_impl::List(strm);
  int i;
  for(i=0; i<gp.size; i++)
    FastGp_(i)->List(strm);
}

taBase* taGroup_impl::New(int no, TypeDef* typ) {
  if (typ == NULL)
    typ = el_typ;

  // if requested typ inherits from the list el type, then 
  // we assume it is for a list el, and create the instances
  if (typ->InheritsFrom(el_base)) {
    taBase* rval = taList_impl::New(no, typ);
    return rval;
  }
  
  // otherwise, if it is for a group type, we check to make sure
  // it either inherits from the current group type, or the current
  // groups inherits from it -- in the latter case, we create the derived type
  // (there is no officially supported member for specifying group type,
  // so we have to be conservative and assume group must contain subgroups of 
  // at least its own type)
  if (typ->InheritsFrom(&TA_taGroup_impl)) {
    if (GetTypeDef()->InheritsFrom(typ)) {
      typ = GetTypeDef(); 
    } else if (!typ->InheritsFrom(GetTypeDef()))
      goto err;
    taBase* rval = gp.New(no, typ);
//    UpdateAfterEdit();
    return rval;
  }
err: 
  taMisc::Warning("*** Attempt to create type:", typ->name,
		   "in group of type:", GetTypeDef()->name,
		   "with base element type:", el_base->name);
  return NULL;
}

taBase* taGroup_impl::NewEl_(int no, TypeDef* typ) {
  if(typ == NULL)
    typ = el_typ;
  taBase* rval = taList_impl::New(no, typ);
  return rval;
}

TAGPtr taGroup_impl::NewGp_(int no, TypeDef* typ) {
  if(typ == NULL)
    typ = GetTypeDef();		// always create one of yourself..
  TAGPtr rval = (TAGPtr)gp.New(no, typ);
//  UpdateAfterEdit();
  return rval;
}

ostream& taGroup_impl::OutputR(ostream& strm, int indent) const {
  taMisc::indent(strm, indent) << name << "[" << size << "] = {\n";
  TypeDef* td = GetTypeDef();
  int i;
  for(i=0; i < td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if(md->HasOption("EDIT_IN_GROUP"))
      md->Output(strm, (void*)this, indent+1);
  }

  for(i=0; i<size; i++) {
    if(el[i] == NULL)	continue;
    ((taBase*)el[i])->OutputR(strm, indent+1);
  }

  gp.OutputR(strm, indent+1);

  taMisc::indent(strm, indent) << "}\n";
  return strm;
}

void taGroup_impl::ItemRemoved_() {
  inherited::ItemRemoved_();
  UpdateLeafCount_(-1);
}

void taGroup_impl::RemoveAll() {
  gp.RemoveAll();
  taList_impl::RemoveAll();
  leaves = 0;
}

bool taGroup_impl::RemoveLeafIdx(int idx) {
  if(idx >= leaves)
    return false;
  if(size && (idx < size))
    return RemoveIdx(idx);

  int nw_idx = (int)idx - size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if(sbg->leaves && (sbg->leaves > nw_idx))
      return sbg->RemoveLeafIdx(nw_idx);
    nw_idx -= (int)sbg->leaves;
  }
  return false;
}

bool taGroup_impl::RemoveLeafName(const char* it) {
  int i;
  if((FindLeafName_(it, i)))
    return RemoveLeafIdx(i);
  return false;
}

bool taGroup_impl::RemoveLeafEl(taBase* it) {
  int i;
  if((i = FindLeafEl(it)) < 0)
    return false;
  return RemoveLeafIdx(i);
}

int taGroup_impl::ReplaceType(TypeDef* old_type, TypeDef* new_type) {
  int nchanged = taList_impl::ReplaceType(old_type, new_type);
  nchanged += gp.ReplaceType(old_type, new_type);
  int i;
  for(i=0; i<gp.size; i++) {
    nchanged += FastGp_(i)->ReplaceType(old_type, new_type);
  }
  return nchanged;
}

TAGPtr taGroup_impl::SafeLeafGp_(int gp_idx) const {
  if (gp_idx == 0) return const_cast<TAGPtr>(this);
  if (!leaf_gp) InitLeafGp();
  return leaf_gp->SafeEl(gp_idx);
}

void taGroup_impl::UpdateLeafCount_(int no) {
  leaves += no;
  if(super_gp != NULL)
    super_gp->UpdateLeafCount_(no);
}



