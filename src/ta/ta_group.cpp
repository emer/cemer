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
  if ((dcr >= DCR_LIST_ITEM_MIN) && (dcr <= DCR_LIST_ITEM_MAX))
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
  if (leaf_gp != NULL) {
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
  taList_impl::DataChanged(dcr, op1, op2); // normal processing
  // send LIST events to the root group as a GROUP_ITEM event
  if (root_gp && (dcr >= DCR_LIST_ITEM_MIN) && (dcr <= DCR_LIST_ITEM_MAX)) {
    root_gp->DataChanged(dcr + (DCR_GROUP_ITEM_MIN - DCR_LIST_ITEM_MIN) , op1, op2);
  } 
  // GROUP_ITEM +/- and GROUP +/- events cause invalidation of the group iteration cache
  // have to trigger on items too, because iteration cache does funky stuff for size==0
  if ( (dcr == DCR_LIST_ITEM_INSERT) || (dcr == DCR_LIST_ITEM_REMOVE)
    || (dcr == DCR_GROUP_ITEM_INSERT) || (dcr == DCR_GROUP_ITEM_REMOVE)
    || ((dcr >= DCR_GROUP_MIN) && (dcr <= DCR_GROUP_MAX)) ) 
  {
    if (leaf_gp != NULL) {
      taBase::unRefDone(leaf_gp);
      leaf_gp = NULL;
    }
  }
}

void taGroup_impl::InitLeafGp() const {
  if (leaf_gp != NULL)
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

void taGroup_impl::Copy(const taGroup_impl& cp) {
  taList_impl::Copy(cp);
  gp.Copy(cp.gp);
}

void taGroup_impl::AddOnly_(void* it) {
  taList_impl::AddOnly_(it);
  UpdateLeafCount_(1);		// not the most efficient, but gets it at a low level
}

void taGroup_impl::Borrow(const taGroup_impl& cp) {
  taList_impl::Borrow(cp);
  gp.Borrow(cp.gp);
}

void taGroup_impl::BorrowUnique(const taGroup_impl& cp) {
  taList_impl::BorrowUnique(cp);
  gp.BorrowUnique(cp.gp);
}

void taGroup_impl::BorrowUniqNameOld(const taGroup_impl& cp) {
  taList_impl::BorrowUniqNameOld(cp);
  gp.BorrowUniqNameOld(cp.gp);
}

void taGroup_impl::BorrowUniqNameNew(const taGroup_impl& cp) {
  taList_impl::BorrowUniqNameNew(cp);
  gp.BorrowUniqNameNew(cp.gp);
}

void taGroup_impl::Copy_Common(const taGroup_impl& cp) {
  taList_impl::Copy_Common(cp);
  gp.Copy_Common(cp.gp);
}

void taGroup_impl::Copy_Duplicate(const taGroup_impl& cp) {
  taList_impl::Copy_Duplicate(cp);
  gp.Copy_Duplicate(cp.gp);
}

void taGroup_impl::Copy_Borrow(const taGroup_impl& cp) {
  taList_impl::Copy_Borrow(cp);
  gp.Copy_Borrow(cp.gp);
}

int taGroup_impl::SelectForEditSearch(const String& memb_contains, SelectEdit*& editor) {
  int nfound = inherited::SelectForEditSearch(memb_contains, editor);
  nfound += gp.SelectForEditSearch(memb_contains, editor);
  return nfound;
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

int taGroup_impl::Dump_Save_PathR(ostream& strm, TAPtr par, int indent) {
  return inherited::Dump_Save_PathR(strm, par, indent);
}

int taGroup_impl::Dump_Save_PathR_impl(ostream& strm, TAPtr par, int indent) {
  int rval = inherited::Dump_Save_PathR_impl(strm, par, indent); // save first-level
  if(rval == false)
    rval = gp.Dump_Save_PathR_impl(strm, par, indent);
  // note that it must be relative to parent, not this
  else
    gp.Dump_Save_PathR_impl(strm, par, indent);
  return rval;
}

int taGroup_impl::Dump_SaveR(ostream& strm, TAPtr par, int indent) {
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

int taGroup_impl::FindLeafEl(TAPtr it) const {
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

TAPtr taGroup_impl::FindLeafName_(const char* nm, int& idx) const {
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

TAPtr taGroup_impl::FindLeafType_(TypeDef* it, int& idx) const {
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
    TAPtr first_el = (TAPtr)FastEl_(i);
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
    TAPtr first_el = (TAPtr)FastEl_(i);
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
    TAPtr ownr = owner->GetOwner();
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

TAPtr taGroup_impl::Leaf_(int idx) const {
  if(idx >= leaves)
    return NULL;
  if(size && (idx < size))
    return (TAPtr)el[idx];

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
  // obsolete
//   if(no == 0) {
// #ifdef TA_GUI
//   if(taMisc::gui_active)
//     return gpiGroupNew::New(this, NULL, no, typ);
// #endif
//     return NULL;
//   }

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

TAPtr taGroup_impl::NewEl_(int no, TypeDef* typ) {
  // obsolete
//   if(no == 0) {
// #ifdef TA_GUI
//   if(taMisc::gui_active)
//     return gpiGroupNew::New(this, NULL, no, typ);
// #endif
//     return NULL;
//   }
  if(typ == NULL)
    typ = el_typ;
  TAPtr rval = taList_impl::New(no, typ);
  return rval;
}

TAGPtr taGroup_impl::NewGp_(int no, TypeDef* typ) {
  // obsolete
//   if(no == 0) {
// #ifdef TA_GUI
//   if(taMisc::gui_active) {
//     gpiGroupNew::New(this, NULL, no, typ);
//     return NULL;		// not sure if rval is a group or not
//   }
// #endif
//     return NULL;
//   }
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
    ((TAPtr)el[i])->OutputR(strm, indent+1);
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

bool taGroup_impl::RemoveLeafEl(TAPtr it) {
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


#ifdef TA_GUI

/*
  CLipboard Operations

  Src: Group
  Dst: Group

*/
void taGroup_impl::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child, const taiMimeSource* ms,
  int& allowed, int& forbidden)
{
  int subgrp_idx = -1;
  if (child) subgrp_idx = gp.FindEl(child);

  // if it is a group item, or is null, then we can do group operations, so we call our G version
  if ((child == NULL) || (subgrp_idx >= 0))
    ChildQueryEditActionsG_impl(md, subgrp_idx, (taGroup_impl*)child, ms, allowed, forbidden);

  // if child was a group item, then we don't pass the child to the base (since it doesn't boggle group items)
  if (subgrp_idx >=0)
    taList_impl::ChildQueryEditActions_impl(md, NULL, ms, allowed, forbidden);
  else
    taList_impl::ChildQueryEditActions_impl(md, child, ms, allowed, forbidden);
}

void taGroup_impl::ChildQueryEditActionsG_impl(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, const taiMimeSource* ms, int& allowed, int& forbidden)
{
  // SRC ops
  if (subgrp) {
    // CUT generally always allowed (will either DELETE or UNLINK src item, depending on context)
    allowed |= taiClipData::EA_CUT;
    // Delete only allowed if we are the owner
    if (subgrp->GetOwner() == &gp)
      allowed |= taiClipData::EA_DELETE;
    else // otherwise, it is unlinking, not deleting
      allowed |= taiClipData::EA_UNLINK;
  }

  if (ms == NULL) return; // src op query
  // DST ops
  // if not a taBase type of object, no more applicable
  if (!ms->isBase()) return;
  if (!ms->IsThisProcess())
    forbidden &= taiClipData::EA_IN_PROC_OPS; // note: redundant during queries, but needed for G action calls

  // generic group paste only allows exact type, so we check for each inheriting from the other, which means same
  bool right_gptype = (ms->td() && (ms->td()->InheritsFrom(GetTypeDef()) && GetTypeDef()->InheritsFrom(ms->td())));

  //TODO: should we allow copy?
  if (right_gptype)
    allowed |= (taiClipData::EA_PASTE | taiClipData::EA_DROP_MOVE);
}

// called by a child -- lists etc. can then allow drops on the child, to indicate inserting into the list, etc.
int taGroup_impl::ChildEditAction_impl(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea) {
  // if child exists, but is not a group item, then just delegate down to base
  int subgrp_idx = -1;
  if (child) {
    subgrp_idx = gp.FindEl(child);
    if (subgrp_idx < 0)
      return taList_impl::ChildEditAction_impl(md, child, ms, ea);
  }

  // we will be calling our own G routines...
  // however, if child is NULL, and our ops don't do anything, then we must call base ops
  // determine the list-only operations allowed/forbidden, and apply to ea
  int rval = taiClipData::ER_IGNORED;
  int allowed = 0;
  int forbidden = 0;
  ChildQueryEditActionsG_impl(md, subgrp_idx, (taGroup_impl*)child, ms, allowed, forbidden);
  if (ea & forbidden) return taiClipData::ER_FORBIDDEN; // requested op was forbidden
  int eax = ea & (allowed & (~forbidden));

  if (eax & taiClipData::EA_SRC_OPS) {
    rval = ChildEditActionGS_impl(md, subgrp_idx, (taGroup_impl*)child, eax);
  } else  if (eax & taiClipData::EA_DST_OPS) {
    if (ms == NULL) return taiClipData::ER_IGNORED;

    // decode src location
    if (ms->IsThisProcess())
      rval = ChildEditActionGD_impl_inproc(md, subgrp_idx, (taGroup_impl*)child, ms, eax);
    else
      // DST OP, SRC OUT OF PROCESS
      rval = ChildEditActionGD_impl_ext(md, subgrp_idx, (taGroup_impl*)child, ms, eax);
  }

  if ((rval == 0) && (child == NULL))
      rval = taList_impl::ChildEditAction_impl(md, NULL, ms, ea);
  return rval;
}

int taGroup_impl::ChildEditActionGS_impl(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, int ea)
{
  // if the child is a group, we handle it, otherwise we let base class handle it
  switch (ea & taiClipData::EA_OP_MASK) {
  //note: COPY is handled by the child object itself
  case taiClipData::EA_CUT: return 1; //nothing to do, just acknowledge -- deletion triggered by the dst, whether local or remote
  case taiClipData::EA_DELETE: {
    if (subgrp) {
      RemoveGpIdx(subgrp_idx);
      return taiClipData::ER_OK;
    } else return taiClipData::ER_ERROR; // error TODO: error message
  }
  case taiClipData::EA_DRAG: return taiClipData::ER_OK; // nothing for us to do on the drag
  default: break; // compiler food
  }
  return taiClipData::ER_IGNORED; // this function never calls down to List
}

int taGroup_impl::ChildEditActionGD_impl_inproc(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp,
  taiMimeSource* ms, int ea)
{
  // if src is not even a taBase, we just stop
  if (!ms->isBase()) return taiClipData::ER_IGNORED;
  int srcgrp_idx = -1; // -1 means not in this group
  taBase* srcobj = NULL;

  // only fetch obj for ops that require it
  if (ea & (taiClipData::EA_PASTE | taiClipData::EA_LINK  | taiClipData::EA_DROP_COPY |
    taiClipData::EA_DROP_LINK | taiClipData::EA_DROP_MOVE))
  {
    srcobj = (taBase*)ms->object();
    if (srcobj == NULL) {
      taMisc::Error("Could not retrieve object for operation.");
      return taiClipData::ER_ERROR;
    }
    // already in this list? (affects how we do drops/copies, etc.)
    srcgrp_idx = gp.FindEl(srcobj);
  }
/*TODO: work out logistics for this... maybe this should only be for when the src is a group
  // All non-move paste ops (i.e., copy an object)
  if (
    (ea & (taiClipData::EA_DROP_COPY)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE) && (ms->srcAction() & taiClipData::EA_SRC_COPY))
  ) {
    // TODO: instead of cloning, we might be better off just streaming a new copy
    // since this will better guarantee that in-proc and outof-proc behavior is same
    taBase* new_obj = obj->Clone();
    //TODO: maybe the renaming should be delayed until put in list, or maybe better, done by list???
    new_obj->SetDefaultName(); // should give it a new name, so not confused with existing obj
    int new_idx;
    if (itm_idx <= 0) 
      new_idx = 0; // if dest is list, then insert at beginning
    else if (itm_idx == (size - 1)) 
      new_idx = -1; // if clicked on last, then insert at end
    else new_idx = itm_idx + 1;
    subgrp->Insert(new_obj, new_idx);
    return taiClipData::ER_OK;
  } */
  
  // All Move-like ops
  if (
    (ea & (taiClipData::EA_DROP_MOVE)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE) && (ms->srcAction() & taiClipData::EA_SRC_CUT))
  ) {
    if (srcobj == subgrp) return taiClipData::ER_OK; // nop
    if (srcgrp_idx >= 0) { // in this group: just do a group move
      // to_idx will differ depending on whether dst is before or after the src object
      if (subgrp_idx < srcgrp_idx) { // for before, to will be dst + 1
        gp.MoveIdx(srcgrp_idx, subgrp_idx + 1);
      } else if (subgrp_idx > srcgrp_idx) { // for after, to will just be the dst
        gp.MoveIdx(srcgrp_idx, subgrp_idx);
      } else return taiClipData::ER_OK; // do nothing case of drop on self
    } else { // not directly in this group, need to do a transfer
      if (gp.Transfer(srcobj)) { // should always succeed -- only fails if we already own item
      // was added at end, fix up location, if necessary
        gp.MoveIdx(gp.size - 1, subgrp_idx + 1);
      } else return taiClipData::ER_ERROR; //TODO: error message
    }
    // NOTE: we don't acknowledge action to source because we moved the item ourself
    return taiClipData::ER_OK;
  }

  // Link ops
  if (ea &
    (taiClipData::EA_LINK | taiClipData::EA_DROP_LINK))
  {
    if (srcgrp_idx >= 0) return taiClipData::ER_FORBIDDEN; // in this list: link forbidden
    gp.InsertLink(srcobj, srcgrp_idx + 1);
    return taiClipData::ER_OK;
  }
  return taiClipData::ER_IGNORED;
}
int taGroup_impl::ChildEditActionGD_impl_ext(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, taiMimeSource* ms, int ea)
{
  // if src is not even a taBase, we just stop
  if (!ms->isBase()) return taiClipData::ER_IGNORED;

  // DST OPS WHEN SRC OBJECT IS OUT OF PROCESS
  switch (ea & taiClipData::EA_OP_MASK) {
  case taiClipData::EA_DROP_COPY:
  case taiClipData::EA_DROP_MOVE:
  case taiClipData::EA_PASTE:
  {
    istringstream istr;
    if (ms->objectData(istr) > 0) {
      TypeDef* td = GetTypeDef();
      int dump_val = td->Dump_Load(istr, this, this);
      if (dump_val == 0) {
        //TODO: error output
        return taiClipData::ER_ERROR; // load failed
      }
      return taiClipData::ER_OK;
    } else { // no data
      return taiClipData::ER_ERROR; //TODO: error message
    }
  }
  }
  return taiClipData::ER_IGNORED;
}


#endif

