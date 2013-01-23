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

#include "SelectEdit.h"
#include <voidptr_PArray>

TypeDef_Of(taProject);

#include <DataChangedReason>
#include <taMisc>



void SelectEdit::StatDataChanged_Group(taGroup_impl* grp, int dcr,
  void* op1, void* op2)
{
  if (!grp->owner || !grp->owner->InheritsFrom(&TA_SelectEdit)) return;
  ((SelectEdit*)(grp->owner))->DataChanged_Group(grp, dcr, op1, op2);
}


void SelectEdit::Initialize() {
  auto_edit = true;
  m_changing = 0;
  base_refs.setOwner(this);
}

void SelectEdit::Destroy() {
  ++m_changing;
  base_refs.Reset();
  mbrs.Reset();
  mths.Reset();
  --m_changing;
}

void SelectEdit::Copy_(const SelectEdit& cp) {
  auto_edit = cp.auto_edit;
  desc = cp.desc;
  base_refs.Reset(); // should get added by copies below
  mbrs = cp.mbrs;
  mths = cp.mths;

  UpdatePointers_NewPar_IfParNotCp((taBase*)&cp, &TA_taProject);
}

void SelectEdit::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (taMisc::is_loading) {
    ConvertLegacy(); // LEGACY
    // add all the bases, since they weren't available during load
    // this is harmless if Convert actually did this instead
    FOREACH_ELEM_IN_GROUP(SelectEditItem, sei, mbrs) {
      BaseAdded(sei->base);
    }
    FOREACH_ELEM_IN_GROUP(SelectEditItem, sei, mths) {
      BaseAdded(sei->base);
    }
  }
}

int SelectEdit::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int rval = base_refs.UpdatePointers_NewPar(old_par, new_par);
  rval += inherited::UpdatePointers_NewPar(old_par, new_par);
  return rval;
}

void SelectEdit::BaseAdded(taBase* base) {
  if (!base) return;
  base_refs.AddUnique(base);
}

void SelectEdit::BaseRemoved(taBase* base) {
  if (!base) return;
  bool has = SelectEditItem::StatHasBase(&mbrs, base);
  if (has) return;
  has = SelectEditItem::StatHasBase(&mths, base);
  if (has) return;
  // ok, no extant refs to the guy, so remove from list
  base_refs.RemoveEl(base);
}

void SelectEdit::DataDestroying_Ref(taBase_RefList* src, taBase* base) {
  // note: item will already have been removed from list
  if (m_changing) return;
  m_changing++;
  SelectEditItem::StatRemoveItemBase(&mbrs, base);
  SelectEditItem::StatRemoveItemBase(&mths, base);
  m_changing--;
}

void SelectEdit::DataChanged_Ref(taBase_RefList* src, taBase* ta,
    int dcr, void* op1, void* op2)
{
  // simplest, is to just issue our own DataChanged
  if(dcr < DCR_UPDATE_VIEWS)
    DataItemUpdated();
}

void SelectEdit::DataChanged_Group(taGroup_impl* grp,
    int dcr, void* op1, void* op2)
{
  if (m_changing) return;
  if (taMisc::is_loading) return; // note: base's aren't set yet, so we can't add
  if (dcr == DCR_GROUP_ITEM_REMOVE) {
    SelectEditItem* ei = (SelectEditItem*)op1;
    BaseRemoved(ei->base);
  }
  else if (dcr == DCR_GROUP_ITEM_INSERT) {
    SelectEditItem* ei = (SelectEditItem*)op1;
    BaseAdded(ei->base); // ignored if null, but shouldn't happen anyway
  }
  //pretty much everything else as well, need to reshow
  //note: this is asynch, so multiple events (ex Compare, etc.) will only cause 1 reshow
  ReShowEdit(true);
}

int SelectEdit::CompareObjs(taBase* obj_a, taBase* obj_b, bool no_ptrs) {
  if(TestError(!obj_a || !obj_b, "CompareObjs", "null object(s)")) return -1;
  if(TestError(obj_a->GetTypeDef() != obj_b->GetTypeDef(), "CompareObjs",
               "objects must have the exact same type to be able to be compared")) return -1;
  name = "Cmp_" + obj_a->GetName() + "_" + obj_b->GetName();
  desc = "Differences between: A: " + obj_a->GetDisplayName() + " and B: " +
    obj_b->GetDisplayName();
  Member_List mds;
  voidptr_PArray trg_bases;
  voidptr_PArray src_bases;
  TypeSpace base_types;
  obj_a->CompareSameTypeR(mds, base_types, trg_bases, src_bases, obj_b, no_ptrs);
  taMisc::Info("SelectEdit::CompareObjs generated", (String)mds.size, "differences");
  for(int i=0;i<mds.size;i++) {
    TypeDef* td = base_types[i];
    if(!td->InheritsFrom(&TA_taBase)) continue;
    //    MemberDef* md = mds[i];
    taBase* itma = (taBase*)trg_bases[i];
    taBase* itmb = (taBase*)src_bases[i];
    String nma = "A: " + itma->GetDisplayName().elidedTo(20) + "." + mds[i]->name;
    String nmb = "B: " + itmb->GetDisplayName().elidedTo(20) + "." + mds[i]->name;
    SelectMember_impl(itma, mds[i], nma, _nilString);
    SelectMember_impl(itmb, mds[i], nmb, _nilString);
  }
  DataItemUpdated(); // so name updates in tree
  ReShowEdit(true);
  return mds.size;
}

void SelectEdit::RemoveField(int idx) {
  RemoveField_impl(idx);
  ReShowEdit(true); //forced
}

void SelectEdit::RemoveField_impl(int idx) {
  EditMbrItem* item = mbrs.Leaf(idx);
  if (item)
    item->Close();
}

void SelectEdit::RemoveFun(int idx) {
  RemoveFun_impl(idx);
  ReShowEdit(true); //forced
}

void SelectEdit::RemoveFun_impl(int idx) {
  EditMthItem* item = mths.Leaf(idx);
  if (item)
    item->Close();
}

void SelectEdit::Reset() {
  mbrs.Reset();
  mths.Reset();
  ReShowEdit(true); //forced
}

bool SelectEdit::ReShowEdit(bool force) {
  if(!taMisc::gui_active) return false;
#ifdef TA_GUI
  if (force) { // ugh
    DataChanged(DCR_STRUCT_UPDATE_BEGIN);
    DataChanged(DCR_STRUCT_UPDATE_END);
  }
  else {
    DataItemUpdated();
  }
//  return taiMisc::ReShowEdits((void*)this, GetTypeDef(), force);
#endif
  return false;
}


int SelectEdit::SearchMembers(taNBase* obj, const String& memb_contains) {
  if(TestError(!obj || memb_contains.empty(), "SearchMembers",
               "null object or empty search")) return -1;
  SelectEdit* se = this;
  int rval = obj->SelectForEditSearch(memb_contains, se);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMember(taBase* base, MemberDef* mbr,
  const String& xtra_lbl, const String& desc, const String& sub_gp_nm)
{
  if (!base) return false;
  String eff_desc = desc; // non-const
  String full_lbl;
  base->GetSelectText(mbr, xtra_lbl, full_lbl, eff_desc);
  bool rval = SelectMember_impl(base, mbr, full_lbl, eff_desc, sub_gp_nm);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMemberNm(taBase* base, const String& md_nm,
  const String& xtra_lbl, const String& desc, const String& sub_gp_nm)
{
  if(base == NULL) return false;
  MemberDef* md = (MemberDef*)base->FindMember(md_nm);
  if (md == NULL) return false;
  return SelectMember(base, md, xtra_lbl, desc, sub_gp_nm);
}

bool SelectEdit::SelectMember_impl(taBase* base, MemberDef* md,
            const String& full_lbl, const String& desc, const String& sub_gp_nm)
{
  int bidx = -1;
  // this looks at the leaves:
  EditMbrItem* item = (EditMbrItem*)SelectEditItem::StatFindItemBase(&mbrs, base, md, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new EditMbrItem;
    item->base = base;
    item->mbr = md;
    item->item_nm = md->name;
    item->label = full_lbl;
    item->desc = desc; // even if empty
    if(desc.nonempty())
      item->cust_desc = true;
    else
      item->cust_desc = false;
    if(sub_gp_nm.nonempty()) {
      EditMbrItem_Group* egp = (EditMbrItem_Group*)mbrs.FindMakeGpName(sub_gp_nm);
      egp->Add(item);
    }
    else {
      mbrs.Add(item); // will trigger BaseAdded
    }
    rval = true;
  }
  else if(sub_gp_nm.nonempty()) {
    EditMbrItem_Group* egp = (EditMbrItem_Group*)item->owner;
    if(egp == &mbrs || egp->name != sub_gp_nm) {
      EditMbrItem_Group* negp = (EditMbrItem_Group*)mbrs.FindMakeGpName(sub_gp_nm);
      negp->Transfer(item);     // grab it
    }
  }
  item->UpdateAfterEdit();
  return rval;
}

bool SelectEdit::SelectMethod(taBase* base, MethodDef* md,
  const String& xtra_lbl, const String& desc, const String& sub_gp_nm)
{
  bool rval = SelectMethod_impl(base, md, xtra_lbl, desc, sub_gp_nm);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMethodNm(taBase* base, const String& md_nm,
  const String& xtra_lbl, const String& desc, const String& sub_gp_nm)
{
  if(base == NULL) return false;
  MethodDef* md = (MethodDef*)base->GetTypeDef()->methods.FindName(md_nm);
  if (md == NULL) return false;
  return SelectMethod(base, md, xtra_lbl, desc, sub_gp_nm);
}

bool SelectEdit::SelectMethod_impl(taBase* base, MethodDef* mth,
  const String& xtra_lbl, const String& desc, const String& sub_gp_nm)
{
  int bidx = -1;
  // this looks at the leaves:
  EditMthItem* item = (EditMthItem*)SelectEditItem::StatFindItemBase(&mths, base, mth, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new EditMthItem;
    item->base = base;
    item->mth = mth;
    item->item_nm = mth->name;
    item->desc = desc;
    if(desc.nonempty())
      item->cust_desc = true;
    else
      item->cust_desc = false;
    item->label = xtra_lbl;
    if (item->label.nonempty()) item->label += " ";
    item->label += mth->GetLabel();
    if(sub_gp_nm.nonempty()) {
      EditMthItem_Group* egp = (EditMthItem_Group*)mths.FindMakeGpName(sub_gp_nm);
      egp->Add(item);
    }
    else {
      mths.Add(item); // will call BaseAdded
    }
    rval = true;
  }
  else if(sub_gp_nm.nonempty()) {
    EditMthItem_Group* egp = (EditMthItem_Group*)item->owner;
    if(egp == &mths || egp->name != sub_gp_nm) {
      EditMthItem_Group* negp = (EditMthItem_Group*)mths.FindMakeGpName(sub_gp_nm);
      negp->Transfer(item);     // grab it
    }
  }
  item->UpdateAfterEdit();
  return rval;
}


EditMbrItem* SelectEdit::mbr(int i) const {
  return mbrs.Leaf(i);
}

EditMthItem* SelectEdit::mth(int i) const {
  return mths.Leaf(i);
}

int SelectEdit::FindMbrBase(taBase* base, MemberDef* md) {
  int rval = -1;
  SelectEditItem::StatFindItemBase(&mbrs, base, md, rval);
  return rval;
}

int SelectEdit::FindMethBase(taBase* base, MethodDef* md) {
  int rval = -1;
  SelectEditItem::StatFindItemBase(&mths, base, md, rval);
  return rval;
}

void SelectEdit::ConvertLegacy() {
  if (config.auto_edit || (mbr_bases.size > 0) || (meth_bases.size > 0)) {
    auto_edit = config.auto_edit;
    for (int i = 0; i < mbr_bases.size; ++i) {
      EditMbrItem* item = (EditMbrItem*)mbrs.New(1);
      item->base = mbr_bases.FastEl(i);
      item->label = config.mbr_labels.SafeEl(i);
      item->item_nm = mbr_strs.SafeEl(i);
      if(item->label.nonempty()) item->label += " ";
      item->label += item->item_nm;//md->GetLabel();
    }
    for (int i = 0; i < meth_bases.size; ++i) {
      EditMthItem* item = (EditMthItem*)mths.New(1);
      item->base = meth_bases.FastEl(i);
      item->label = config.meth_labels.SafeEl(i);
      item->item_nm = meth_strs.SafeEl(i);
      if(item->label.nonempty()) item->label += " ";
      item->label += item->item_nm;//md->GetLabel();
    }
    mbr_bases.Reset();
    config.mbr_labels.Reset();
    mbr_strs.Reset();
    meth_bases.Reset();
    config.meth_labels.Reset();
    meth_strs.Reset();
  }
  { // load memberdefs -- this only needed for legacy, and for brief crossover period projs
  FOREACH_ELEM_IN_GROUP_REV(EditMbrItem, item, this->mbrs) {
    taBase* bs = item->base;
    if (bs == NULL) { // didn't get loaded, bail..
      taMisc::Warning("*** SelectEdit: couldn't find object:", item->label, "in object to edit");
      item->Close();
      continue;
    }
    if (!item->mbr && item->item_nm.nonempty()) {
      String nm = item->item_nm;
      item->mbr = bs->GetTypeDef()->members.FindName((const char*)nm);
    }
    if (item->mbr == NULL) {
      taMisc::Warning("*** SelectEdit: couldn't find member:", item->item_nm, "in object to edit:",bs->GetPathNames());
      item->Close();
      continue;
    }
  }}
  // GetMethsFmStrs()
  {
  FOREACH_ELEM_IN_GROUP_REV(EditMthItem, item, this->mths) {
    taBase* bs = item->base;
    if (bs == NULL) { // didn't get loaded, bail..
      taMisc::Warning("*** SelectEdit: couldn't find object:", item->label, "in object to edit");
      item->Close();
      continue;
    }
    if (!item->mth && item->item_nm.nonempty()) {
      String nm = item->item_nm;
      item->mth = bs->GetTypeDef()->methods.FindName((const char*)nm);
    }
    if (item->mth == NULL) {
      taMisc::Warning("*** SelectEdit: couldn't find method:", item->item_nm, "in object to edit:",bs->GetPathNames());
      item->Close();
      continue;
    }
  }}
}

