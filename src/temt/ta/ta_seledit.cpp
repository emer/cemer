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


#include "ta_seledit.h"

#include "ta_type.h"
#include "ta_project.h"
#include "ta_qt.h"


//////////////////////////////////
//  taBase			//
//////////////////////////////////

void taBase::GetSelectText(MemberDef* mbr, String xtra_lbl,
    String& full_lbl, String& desc) const
{
  if (xtra_lbl.empty())
    xtra_lbl = GetName().elidedTo(16);
  full_lbl = xtra_lbl;
  if (full_lbl.nonempty()) full_lbl += " ";
  full_lbl += mbr->GetLabel();
  // desc is the member description
  if (desc.empty()) 
    MemberDef::GetMembDesc(mbr, desc, "");
}


//////////////////////////////////
//  SelectEditItem		//
//////////////////////////////////

SelectEditItem* SelectEditItem::StatFindItemBase(taGroup_impl* grp, taBase* base,
    TypeItem* ti, int& idx)
{
  SelectEditItem* rval = NULL;
  taLeafItr itr;
  idx = 0;
  FOR_ITR_EL(SelectEditItem, rval, grp->, itr) {
    if ((rval->base == base) && (rval->typeItem() == ti))
      return rval;
    ++idx;
  }
  idx = -1;
  return NULL;
}

bool SelectEditItem::StatHasBase(taGroup_impl* grp, taBase* base) {
  SelectEditItem* ei;
  taLeafItr itr;
  FOR_ITR_EL(SelectEditItem, ei, grp->, itr) {
    if (ei->base == base) return true;
  }
  return false;
}

bool SelectEditItem::StatRemoveItemBase(taGroup_impl* grp, taBase* base) {
  bool rval = false;
  SelectEditItem* ei;
  taLeafItr itr;
  FOR_ITR_EL_REV(SelectEditItem, ei, grp->, itr) {
    if (ei->base == base) {
      rval = true;
      ei->Close();
    }
  }
  return rval;
}


void SelectEditItem::Initialize() {
  base = NULL;
}

void SelectEditItem::Destroy() {
}

void SelectEditItem::Copy_(const SelectEditItem& cp) {
  label = cp.label;
  desc = cp.desc;
  base = cp.base;
  item_nm = cp.item_nm;
}

String SelectEditItem::caption() const {
  return label;
}

String SelectEditItem::GetDesc() const {
  if (desc.nonempty()) return desc;
  return (typeItem()) ? typeItem()->desc : _nilString;
}

String SelectEditItem::GetName() const {
  return label;
}

String SelectEditItem::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "base_name") return (base) ? base->GetName() : String("NULL");
  else if (key == "base_type") return (base) ? base->GetTypeDef()->name : String("NULL");
  else if (key == "item_name") 
    return (typeItem()) ? typeItem()->name : String("NULL");
  else if (key == "label") return label;
  else return inherited::GetColText(key, itm_idx);
}


//////////////////////////////////
//  EditMbrItem		//
//////////////////////////////////

void EditMbrItem::Initialize() {
  mbr = NULL;
}

void EditMbrItem::Destroy() {
}

void EditMbrItem::Copy_(const EditMbrItem& cp) {
  mbr = cp.mbr;
}

void EditMbrItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (desc.empty() && mbr)
    MemberDef::GetMembDesc(mbr, desc, "");
    
}

String EditMbrItem::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "mbr_type") 
    return (mbr) ? mbr->type->name : String("NULL");
  else return inherited::GetColText(key, itm_idx);
}

//////////////////////////////////
//  EditMthItem_Group		//
//////////////////////////////////

void EditMbrItem_Group::DataChanged(int dcr, void* op1, void* op2) 
{
  inherited::DataChanged(dcr, op1, op2);
  SelectEdit::StatDataChanged_Group(this, dcr, op1, op2);
}

String EditMbrItem_Group::GetColHeading(const KeyString& key) const {
  if (key == "base_name") return "Base Name";
  else if (key == "base_type") return "Base Type";
  else if (key == "item_name") return "Member Name";
  else if (key == "mbr_type") return "Member Type";
  else if (key == "label") return "Label";
  else return inherited::GetColHeading(key);
}

const KeyString EditMbrItem_Group::GetListColKey(int col) const {
  switch (col) {
  case 0: return "base_name";
  case 1: return "base_type";
  case 2: return "item_name"; // mbr or mth
  case 3: return "mbr_type";
  case 4: return "label";
  default: break;
  }
  return inherited::GetListColKey(col);
}

//////////////////////////////////
//  EditMthItem			//
//////////////////////////////////

void EditMthItem::Initialize() {
  mth = NULL;
}

void EditMthItem::Destroy() {
}

void EditMthItem::Copy_(const EditMthItem& cp) {
  mth = cp.mth;
}

void EditMthItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (desc.empty() && mth)
    desc = mth->desc;
    
}


//////////////////////////////////
//  EditMthItem_Group		//
//////////////////////////////////

void EditMthItem_Group::Initialize() {
  SetBaseType(&TA_EditMthItem);
  is_root = true;
  group_type = GT_BUTTONS; // for root only
}

void EditMthItem_Group::Copy_(const EditMthItem_Group& cp) {
  group_type = cp.group_type; 
}

void EditMthItem_Group::InitLinks() {
  inherited::InitLinks();
  is_root = IsRoot();
  if (!is_root)
    group_type = GT_MENU_BUTTON;
}

void EditMthItem_Group::DataChanged(int dcr, void* op1, void* op2) 
{
  inherited::DataChanged(dcr, op1, op2);
  SelectEdit::StatDataChanged_Group(this, dcr, op1, op2);
}


String EditMthItem_Group::GetColHeading(const KeyString& key) const {
  if (key == "base_name") return "Base Name";
  else if (key == "base_type") return "Base Type";
  else if (key == "item_name") return "Method Name";
  else if (key == "label") return "Label";
  else return inherited::GetColHeading(key);
}

const KeyString EditMthItem_Group::GetListColKey(int col) const {
  switch (col) {
  case 0: return "base_name";
  case 1: return "base_type";
  case 2: return "item_name"; // mbr or mth
  case 4: return "label";
  default: break;
  }
  return inherited::GetListColKey(col);
}

void EditMthItem_Group::SetGroupType(MthGroupType group_type_) {
  if (is_root && (group_type_ != GT_BUTTONS))
    taMisc::Error("The root group may only show buttons -- create a subgroup for menus or button groups");
  group_type = group_type_;
}


//////////////////////////////////
//  SelectEdit			//
//////////////////////////////////

void SelectEdit::StatDataChanged_Group(taGroup_impl* grp, int dcr,
  void* op1, void* op2) 
{
  if (!grp->owner || !grp->owner->InheritsFrom(&TA_SelectEdit)) return;
  ((SelectEdit*)(grp->owner))->DataChanged_Group(grp, dcr, op1, op2);
}


void SelectEdit::Initialize() {
  auto_edit = false;
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
    taLeafItr itr;
    SelectEditItem* sei = NULL;
    FOR_ITR_EL(SelectEditItem, sei, mbrs., itr) {
      BaseAdded(sei->base);
    }
    FOR_ITR_EL(SelectEditItem, sei, mths., itr) {
      BaseAdded(sei->base);
    }
  }
}

int SelectEdit::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
return 0;
//TODO:
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
  DataChanged(DCR_ITEM_UPDATED);
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
  void_PArray trg_bases;
  void_PArray src_bases;
  TypeSpace base_types;
  obj_a->CompareSameTypeR(mds, base_types, trg_bases, src_bases, obj_b, no_ptrs);
  taMisc::Info("SelectEdit::CompareObjs generated", (String)mds.size, "differences");
  for(int i=0;i<mds.size;i++) {
    TypeDef* td = base_types[i];
    if(!td->InheritsFrom(&TA_taBase)) continue;
    //    MemberDef* md = mds[i];
    taBase* itma = (taBase*)trg_bases[i];
    taBase* itmb = (taBase*)src_bases[i];
    String nma = "A: " + itma->GetDisplayName().elidedTo(20);
    String nmb = "B: " + itmb->GetDisplayName().elidedTo(20);
    SelectMember_impl(itma, mds[i], nma, _nilString);
    SelectMember_impl(itmb, mds[i], nmb, _nilString);
  }
  DataChanged(DCR_ITEM_UPDATED); // so name updates in tree
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

int SelectEdit::SearchMembers(taNBase* obj, const String& memb_contains) {
  if(TestError(!obj || memb_contains.empty(), "SearchMembers", 
	       "null object or empty search")) return -1;
  SelectEdit* se = this;
  int rval = obj->SelectForEditSearch(memb_contains, se);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMember(taBase* base, MemberDef* mbr,
  const String& xtra_lbl, const String& desc) 
{
  if (!base) return false;
  String eff_desc = desc; // non-const
  String full_lbl;
  base->GetSelectText(mbr, xtra_lbl, full_lbl, eff_desc);
  bool rval = SelectMember_impl(base, mbr, full_lbl, eff_desc);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMemberNm(taBase* base, const String& md_nm,
  const String& xtra_lbl, const String& desc) 
{
  if(base == NULL) return false;
  MemberDef* md = (MemberDef*)base->FindMember(md_nm);
  if (md == NULL) return false;
  return SelectMember(base, md, xtra_lbl, desc);
}

bool SelectEdit::SelectMember_impl(taBase* base, MemberDef* md,
  const String& full_lbl, const String& desc)
{
  int bidx = -1;
  EditMbrItem* item = (EditMbrItem*)SelectEditItem::StatFindItemBase(&mbrs, base, md, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new EditMbrItem;
    item->base = base;
    item->mbr = md;
    item->item_nm = md->name;
    item->label = full_lbl;
    item->desc = desc; // even if empty
    mbrs.Add(item); // will trigger BaseAdded
    rval = true;
  }
  return rval;
}

bool SelectEdit::SelectMethod(taBase* base, MethodDef* md,
  const String& xtra_lbl, const String& desc) 
{
  bool rval = SelectMethod_impl(base, md, xtra_lbl, desc);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMethodNm(taBase* base, const String& md_nm,
  const String& xtra_lbl, const String& desc) 
{
  if(base == NULL) return false;
  MethodDef* md = (MethodDef*)base->GetTypeDef()->methods.FindName(md_nm);
  if (md == NULL) return false;
  return SelectMethod(base, md, xtra_lbl, desc);
}

bool SelectEdit::SelectMethod_impl(taBase* base, MethodDef* mth,
  const String& xtra_lbl, const String& desc)
{
  int bidx = -1;
  EditMthItem* item = (EditMthItem*)SelectEditItem::StatFindItemBase(&mths, base, mth, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new EditMthItem;
    item->base = base;
    item->mth = mth;
    item->item_nm = mth->name;
    if (desc.empty())
      item->desc = desc;
    else item->desc = mth->desc;
    item->label = xtra_lbl;
    if (item->label.nonempty()) item->label += " ";
    item->label += mth->GetLabel();
    mths.Add(item); // will call BaseAdded
    rval = true;
  }
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

//////////////////////////////////
//	LEGACY	//
//////////////////////////////////

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
  EditMbrItem* item;
  taLeafItr itr;
  FOR_ITR_EL_REV(EditMbrItem, item, this->mbrs., itr) {
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
      taMisc::Warning("*** SelectEdit: couldn't find member:", item->item_nm, "in object to edit:",bs->GetPath());
      item->Close();
      continue;
    }
  }}
  // GetMethsFmStrs() 
  {
  EditMthItem* item;
  taLeafItr itr;
  FOR_ITR_EL_REV(EditMthItem, item, this->mths., itr) {
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
      taMisc::Warning("*** SelectEdit: couldn't find method:", item->item_nm, "in object to edit:",bs->GetPath());
      item->Close();
      continue;
    }
  }}
}


/*int SelectEdit::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int nchg = 0;
  // NOTE: we update notify list by just doing a series of Remove/Add
  for(int j=mbr_bases.size-1; j>=0; j--) {
    taBase* itm = mbr_bases[j];
    taBase* old_own = itm->GetOwner(old_par->GetTypeDef());
    if(old_own != old_par) continue;
    String old_path = itm->GetPath(NULL, old_par);
    taBase* nitm = new_par->FindFromPath(old_path);
    if(nitm != NULL) {
      mbr_bases.ReplaceLinkIdx(j, nitm);
      BaseRemoved(itm);
      BaseAdded(nitm);
      nchg++;
    }
    else {
      RemoveField_impl(j);	// this is why this is diff from std case!
    }
  }
  for(int j=meth_bases.size-1; j>=0; j--) {
    taBase* itm = meth_bases[j];
    String old_path = itm->GetPath(NULL, old_par);
    taBase* nitm = new_par->FindFromPath(old_path);
    if(nitm != NULL) {
      meth_bases.ReplaceLinkIdx(j, nitm);
      BaseRemoved(itm);
      BaseAdded(nitm);
      nchg++;
    }
    else {
      RemoveFun_impl(j);	// this is why this is diff from std case!
    }
  }
  return nchg;
}*/
