// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_group.cc

#include <sstream>

#include "ta_group.h"
#include "ta_TA_type.h"

#ifdef TA_GUI
# include "ta_qtgroup.h"
# include "ta_qtclipdata.h"
#endif



////////////////////////////
//      taSubGroup        //
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
//      taGroup_impl      //
////////////////////////////

bool taGroup_impl::def_nw_item;

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
  gp.SetBaseType(GetTypeDef()); // more of the same type of group
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

void taGroup_impl::CanCopy_impl(const taBase* cp_fm_, bool quiet,
  bool& ok, bool virt) const
{
  if (virt) {
    inherited::CanCopy_impl(cp_fm_, quiet, ok, virt);
    if (!ok) return; // no reason to continue, and could be bad to do so
  }
  const taGroup_impl* cp_fm = (const taGroup_impl*)cp_fm_; // is safe
  // we only allow group copies when the group types are identical
  // since otherwise, we must be dealing with groups for different
  // purposes, and they cannot be considered assignable
  if (CheckError((GetTypeDef() != cp_fm->GetTypeDef()), quiet, ok,
    "Copy: Groups must be of same type to be copyable")) return;
  // make sure that the gp elbase's are commensurable
  gp.CanCopy(&cp_fm->gp, quiet, ok);
}

void taGroup_impl::Copy_(const taGroup_impl& cp) {
  gp.Copy(cp.gp);
  leaves = cp.leaves; // prob not needed
}

bool taGroup_impl::ChildCanDuplicate(const taBase* chld,
    bool quiet) const
{
  if (gp.FindEl(chld) >= 0) {
    return true;
  }
  return inherited::ChildCanDuplicate(chld, quiet);
}

taBase* taGroup_impl::ChildDuplicate(const taBase* chld) {
  if (gp.FindEl(chld) >= 0) {
    return gp.DuplicateEl(chld);
  }
  return inherited::ChildDuplicate(chld);
}

void taGroup_impl::AddOnly_(void* it) {
  inherited::AddOnly_(it);
  UpdateLeafCount_(1);          // not the most efficient, but gets it at a low level
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

void taGroup_impl::Search_impl(const String& srch, taBase_PtrList& items,
                               taBase_PtrList* owners,
                               bool contains, bool case_sensitive,
                               bool obj_name, bool obj_type,
                               bool obj_desc, bool obj_val,
                               bool mbr_name, bool type_desc) {
  int st_sz = items.size;
  inherited::Search_impl(srch, items, owners, contains, case_sensitive, obj_name, obj_type,
                       obj_desc, obj_val, mbr_name, type_desc);
  bool already_added_me = false;
  if(items.size > st_sz)
    already_added_me = true;
  gp.Search_impl(srch, items, owners, contains, case_sensitive, obj_name, obj_type,
                 obj_desc, obj_val, mbr_name, type_desc);
  if(owners && (items.size > st_sz) && !already_added_me) { // we added somebody somewhere..
    owners->Link(this);
  }
}


void taGroup_impl::CompareSameTypeR(Member_List& mds, TypeSpace& base_types,
                                    voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                                    taBase* cp_base,
                                    int show_forbidden, int show_allowed, bool no_ptrs) {
  if(!cp_base) return;
  if(GetTypeDef() != cp_base->GetTypeDef()) return; // must be same type..
  inherited::CompareSameTypeR(mds, base_types, trg_bases, src_bases, cp_base, show_forbidden,
                              show_allowed, no_ptrs);
  taGroup_impl* cp_gp = (taGroup_impl*)cp_base;
  gp.CompareSameTypeR(mds, base_types, trg_bases, src_bases, &(cp_gp->gp),
                      show_forbidden, show_allowed, no_ptrs);
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

String taGroup_impl::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
                              bool force_inline) const {
  String nm = " Size: ";
  nm += String(size);
  if(gp.size > 0)
    nm += String(".") + String(gp.size);
  if(leaves != size)
    nm += String(".") + String((int) leaves);
  nm += String(" (") + el_typ->name + ")";
  return nm;
}

bool taGroup_impl::SetValStr(const String& val, void* par, MemberDef* memb_def,
                            TypeDef::StrContext sc, bool force_inline) {
  if(val != String::con_NULL) {
    String tmp = val;
    if(tmp.contains('(')) {
      tmp = tmp.after('(');
      tmp = tmp.before(')');
    }
    tmp.gsub(" ", "");
    TypeDef* td = taMisc::FindTypeName(tmp);
    if(td != NULL) {
      el_typ = td;
      return true;
    }
  }
  return false;
}

taObjDiffRec* taGroup_impl::GetObjDiffVal(taObjDiff_List& odl, int nest_lev,
  MemberDef* memb_def, const void* par, TypeDef* par_typ, taObjDiffRec* par_od) const {
  // always just add a record for this guy
  taObjDiffRec* odr = inherited::GetObjDiffVal(odl, nest_lev, memb_def, par, par_typ, par_od);
  MemberDef* gpmd = FindMember("gp");
  taObjDiffRec* gpodr = gp.GetObjDiffVal(odl, nest_lev+1, gpmd, this, GetTypeDef(), odr);
  gpodr->name = odr->name + "_gp";
  gpodr->value = odr->value + "_gp";
  gpodr->ComputeHashCode();
  return odr;
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
  int idx = FindEl(it);
  if(idx >= 0)
    return idx;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    idx = sbg->FindLeafEl(it);
    if(idx >= 0)
      return idx + new_idx;
    new_idx += (int)sbg->leaves;
  }
  return -1;
}

int taGroup_impl::FindLeafNameIdx(const String& nm) const {
  int idx = FindNameIdx(nm);
  if(idx >= 0)
    return idx;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    idx = sbg->FindLeafNameIdx(nm);
    if(idx >= 0)
      return idx + new_idx;
    new_idx += (int)sbg->leaves;
  }
  return -1;
}

taBase* taGroup_impl::FindLeafName_(const String& nm) const {
  int idx = FindLeafNameIdx(nm);
  if(idx >= 0)
    return Leaf_(idx);
  return NULL;
}

int taGroup_impl::FindLeafNameContainsIdx(const String& nm) const {
  int idx = FindNameContainsIdx(nm);
  if(idx >= 0)
    return idx;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    idx = sbg->FindLeafNameContainsIdx(nm);
    if(idx >= 0)
      return idx + new_idx;
    new_idx += (int)sbg->leaves;
  }
  return -1;
}

taBase* taGroup_impl::FindLeafNameContains_(const String& nm) const {
  int idx = FindLeafNameContainsIdx(nm);
  if(idx >= 0)
    return Leaf_(idx);
  return NULL;
}

int taGroup_impl::FindLeafNameTypeIdx(const String& nm) const {
  int idx = FindNameTypeIdx(nm);
  if(idx >= 0)
    return idx;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    idx = sbg->FindLeafNameTypeIdx(nm);
    if(idx >= 0)
      return idx + new_idx;
    new_idx += (int)sbg->leaves;
  }
  return -1;
}

taBase* taGroup_impl::FindLeafNameType_(const String& nm) const {
  int idx = FindLeafNameTypeIdx(nm);
  if(idx >= 0)
    return Leaf_(idx);
  return NULL;
}

int taGroup_impl::FindLeafTypeIdx(TypeDef* it) const {
  int idx = FindTypeIdx(it);
  if(idx >= 0)
    return idx;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    idx = sbg->FindLeafTypeIdx(it);
    if(idx >= 0)
      return idx + new_idx;
    new_idx += (int)sbg->leaves;
  }
  return -1;
}

taBase* taGroup_impl::FindLeafType_(TypeDef* it) const {
  int idx = FindLeafTypeIdx(it);
  if(idx >= 0)
    return Leaf_(idx);
  return NULL;
}

TAGPtr taGroup_impl::FindMakeGpName(const String& nm, TypeDef* typ, bool& nw_item) {
  TAGPtr rval = gp.FindName(nm);
  if (rval) {
    nw_item = false;
    return rval;
  }
  rval = NewGp_(1, typ, nm);
  nw_item = true;
  return rval;
}

void* taGroup_impl::FindMembeR(const String& nm, MemberDef*& ret_md) const {
  ret_md = NULL;

  // first look for special list index syntax
  String idx_str = nm;
  idx_str = idx_str.before(']');
  if(idx_str.nonempty()) {
    idx_str = idx_str.after('[');
    if(idx_str.contains('\"')) {
      String elnm = idx_str.between('\"','\"');
      elnm = taMisc::StringCVar(elnm);
      if(TestWarning(elnm.empty(), "FindMembeR","empty string index name:", idx_str))
        return NULL;
      return FindName_(elnm);
    }
    else {
      int idx = atoi(idx_str);
      if((idx >= size) || (idx < 0)) {
        return NULL;
      }
      return el[idx];
    }
  }

  // then look for items in the list itself, by name or type
  taBase* fnd = FindLeafNameType_(nm);
  if(fnd)
    return fnd;

  // then look on members of list obj itself, recursively
  void* rval = taBase::FindMembeR(nm, ret_md); // don't call taList guy!
  if(rval)
    return rval;

  // finally, look recursively on owned objs on list
  //  int max_srch = MIN(taMisc::search_depth, size);
  // these days, it just doesn't make sense to restrict!
  for(int i=0; i<size; i++) {
    taBase* itm = (taBase*)FastEl_(i);
    if(itm && itm->GetOwner() == this) {
      rval = itm->FindMembeR(nm, ret_md);
      if(rval)
        return rval;
    }
  }

  // for groups, then just try the subgroups -- this will be a tiny bit redundant, but ok..
  TAGPtr sbg;
  for(int i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    rval = sbg->FindMembeR(nm, ret_md);
    if(rval)
      return rval;
  }

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

taBase* taGroup_impl::Leaf_(int idx) const {
  if ((idx < 0) || (idx >= leaves))
    return NULL;
  if(size && (idx < size))
    return (taBase*)el[idx];

  int nw_idx = (int)idx - size;
  TAGPtr sbg;
  for(int i=0; i<gp.size; i++) {
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

taBase* taGroup_impl::New_impl(int no, TypeDef* typ, const String& name_)
{
  taBase* rval = NULL;
  if (typ == NULL)
    typ = el_typ;
  // if requested typ inherits from the list el_base, then
  // just create list el
  else if (!typ->InheritsFrom(el_base))
    goto cont;

  rval = taList_impl::New_impl(no, typ, name_);
  return rval;

cont:
  // otherwise, we assume it will be a group, but do some additional
  // checks here that we wouldn't do for the Gp guy...
  // we check to make sure it either inherits from the current group type,
  // or the current groups inherits from it -- in the latter case, we
  // create the derived type (there is no officially supported member for
  // specifying group type, so we have to be conservative and assume
  // group must contain subgroups of at least its own type)
  if (typ->InheritsFrom(&TA_taGroup_impl)) {
    if (GetTypeDef()->InheritsFrom(typ)) {
      typ = GetTypeDef(); // make our own type, in case we assume that
    } else if (!typ->InheritsFrom(GetTypeDef()))
      goto err;
  } else goto err; // not a group type, so can't be right

  rval = (TAGPtr)gp.New(no, typ, name_);
//  UpdateAfterEdit();
  return rval;

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

TAGPtr taGroup_impl::NewGp_(int no, TypeDef* typ, const String& name_) {
  if (typ == NULL)
    typ = GetTypeDef(); // always create one of yourself..

  // note: following will spit it out if it isn't a taGroup_impl of right type
  TAGPtr rval = (TAGPtr)gp.New(no, typ, name_);
//  UpdateAfterEdit();
  return rval;
}

TAGPtr taGroup_impl::NewGp_gui(int no, TypeDef* typ, const String& name_) {
  TAGPtr rval = NewGp_(no, typ, name_);
  if (rval) {
    if (taMisc::gui_active && !taMisc::no_auto_expand) {
      if(!HasOption("NO_EXPAND_ALL") && !rval->HasOption("NO_EXPAND_ALL"))
      {
        tabMisc::DelayedFunCall_gui(rval, "BrowserExpandAll");
        tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
      }
    }
  }
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
    if(el[i] == NULL)   continue;
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
  int i = FindLeafNameIdx(it);
  if(i >= 0)
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


////////////////////////////
//  UserDataItem_List     //
////////////////////////////

bool UserDataItem_List::hasVisibleItems() const {
  // iterate all items and return true on first isVisible found
  FOREACH_ELEM_IN_GROUP(UserDataItemBase, udi, *this) {
    if (udi->isVisible()) return true;
  }
  return false;
}

UserDataItem* UserDataItem_List::NewItem(const String& name, const Variant& value,
    const String& desc)
{
  if (TestError((name.empty() || (FindName(name) != NULL)),
    "UserDataItem_List::NewItem",
    "name must be a valid name, not already in the list")) {
    return NULL;
  }
  // note: make the item w/o owner, then rename and insert, to avoid unnecessary updates
  UserDataItem* udi = new UserDataItem;
  udi->name = name;
  udi->value = value;
  udi->desc = desc;
  Add(udi);
  return udi;
}

//////////////////////////
//      CircBuffer      //
//////////////////////////

void float_CircBuffer::Initialize() {
  st_idx = 0;
  length = 0;
}

void float_CircBuffer::Copy_(const float_CircBuffer& cp) {
  st_idx = cp.st_idx;
  length = cp.length;
}

void float_CircBuffer::Reset() {
  float_Array::Reset();
  st_idx = 0;
  length = 0;
}
