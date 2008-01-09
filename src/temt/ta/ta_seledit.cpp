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
  item_nm = cp.item_nm;
  label = cp.label;
  base = cp.base;
}

String SelectEditItem::caption() const {
  String nm = label;
  if (nm.nonempty()) nm += " ";
  nm += typeItem()->GetLabel();
  return nm;
}

String SelectEditItem::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "base_name") return (base) ? base->GetName() : String("NULL");
  else if (key == "base_type") return (base) ? base->GetTypeDef()->name : String("NULL");
  else if (key == "item_name") 
    return (typeItem()) ? typeItem()->name : String("NULL");
  else if (key == "label") return label;
  else return inherited::GetColText(key, itm_idx);
}

String SelectEditItem::GetDisplayName() const {
  String rval;
  if (base) rval = base->GetName();
  TypeItem* ti = typeItem();
  if (rval.nonempty() && ti) rval += ":";
  if (ti) rval += ti->name;
  return rval;
  
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

void EditMbrItem_Group::GetMembsFmStrs() {
  EditMbrItem* item;
  taLeafItr itr;
  FOR_ITR_EL_REV(EditMbrItem, item, this->, itr) {
    taBase* bs = item->base;
    if (bs == NULL) { // didn't get loaded, bail..
      taMisc::Warning("*** SelectEdit: couldn't find object:", item->label, item->item_nm, "in object to edit");
      item->Close();
      continue;
    }
    String nm = item->item_nm;
    MemberDef* md = bs->GetTypeDef()->members.FindName((const char*)nm);
    if(md == NULL) {
      taMisc::Warning("*** SelectEdit: couldn't find member:", nm, "in object to edit:",bs->GetPath());
      item->Close();
      continue;
    }
    item->mbr = md;
  }
}

String EditMbrItem_Group::GetColHeading(const KeyString& key) const {
  if (key == "base_name") return "Base Name";
  else if (key == "base_type") return "Base Type";
  else if (key == "item_name") return "Member Name";
  else if (key == "mbr_type") return "Member Type";
  else if (key == "label") return "Base Type";
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


//////////////////////////////////
//  EditMthItem_Group		//
//////////////////////////////////

void EditMthItem_Group::DataChanged(int dcr, void* op1, void* op2) 
{
  inherited::DataChanged(dcr, op1, op2);
  SelectEdit::StatDataChanged_Group(this, dcr, op1, op2);
}

void EditMthItem_Group::GetMethsFmStrs() {
  EditMthItem* item;
  taLeafItr itr;
  FOR_ITR_EL_REV(EditMthItem, item, this->, itr) {
    taBase* bs = item->base;
    if (bs == NULL) { // didn't get loaded, bail..
      taMisc::Warning("*** SelectEdit: couldn't find object:", item->label, item->item_nm, "in object to edit");
      item->Close();
      continue;
    }
    String nm = item->item_nm;
    MethodDef* md = bs->GetTypeDef()->methods.FindName((const char*)nm);
    if(md == NULL) {
      taMisc::Warning("*** SelectEdit: couldn't find method:", nm, "in object to edit:",bs->GetPath());
      item->Close();
      continue;
    }
    item->mth = md;
  }
}

String EditMthItem_Group::GetColHeading(const KeyString& key) const {
  if (key == "base_name") return "Base Name";
  else if (key == "base_type") return "Base Type";
  else if (key == "item_name") return "Method Name";
  else if (key == "label") return "label";
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
    mbrs.GetMembsFmStrs();
    mths.GetMethsFmStrs();
    //TODO: must ref pointers????
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
//TODO
}

void SelectEdit::DataChanged_Group(taGroup_impl* grp,
    int dcr, void* op1, void* op2)
{
  if (m_changing) return;
  if (dcr == DCR_GROUP_ITEM_REMOVE) {
    SelectEditItem* ei = (SelectEditItem*)op1;
    BaseRemoved(ei->base);
  }
  else if (dcr == DCR_GROUP_ITEM_UPDATE) {
    SelectEditItem* ei = (SelectEditItem*)op1;
    // load is special case -- will probably be the UAE, register the base
    if (taMisc::is_loading) {
      if (ei->base) 
        BaseAdded(ei->base);
      return; // no gui stuff
    }
  }
  //pretty much everything else as well, need to reshow
//TODO: watch out of this one being too much remaking!!!
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
  obj_a->CompareSameTypeR(mds, trg_bases, src_bases, obj_b, no_ptrs);
  taMisc::Info("SelectEdit::CompareObjs generated", (String)mds.size, "differences");
  for(int i=0;i<mds.size;i++) {
    taBase* itma = (taBase*)trg_bases[i];
    taBase* itmb = (taBase*)src_bases[i];
    String nma = "A: " + itma->GetDisplayName().elidedTo(20);
    String nmb = "B: " + itmb->GetDisplayName().elidedTo(20);
    SelectMember_impl(itma, mds[i], nma);
    SelectMember_impl(itmb, mds[i], nmb);
  }
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

bool SelectEdit::SelectMember(taBase* base, MemberDef* md, const char* lbl) {
  bool rval = SelectMember_impl(base, md, lbl);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMemberNm(taBase* base, const char* md, const char* lbl) {
  if(base == NULL) return false;
  MemberDef* mda = (MemberDef*)base->FindMember(md);
  if(mda == NULL) return false;
  return SelectMember(base, mda, lbl);
}

bool SelectEdit::SelectMember_impl(taBase* base, MemberDef* md, const char* lbl) {
  int bidx = -1;
  EditMbrItem* item = (EditMbrItem*)SelectEditItem::StatFindItemBase(&mbrs, base, md, bidx);
  bool rval = false;
  if (bidx >= 0) {
    item->label = lbl;
  } else {
    item = (EditMbrItem*)mbrs.New(1);
    item->base = base;
    item->mbr = md;
    item->item_nm = md->name;
    item->label = lbl;
    BaseAdded(base);
    rval = true;
  }
  return rval;
}

bool SelectEdit::SelectMethod(taBase* base, MethodDef* md, const char* lbl) {
  bool rval = SelectMethod_impl(base, md, lbl);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMethodNm(taBase* base, const char* md, const char* lbl) {
  if(base == NULL) return false;
  MethodDef* mda = (MethodDef*)base->GetTypeDef()->methods.FindName(md);
  if(mda == NULL) return false;
  return SelectMethod(base, mda, lbl);
}

bool SelectEdit::SelectMethod_impl(taBase* base, MethodDef* mth, const char* lbl) {
  int bidx = -1;
  EditMthItem* item = (EditMthItem*)SelectEditItem::StatFindItemBase(&mths, base, mth, bidx);
  bool rval = false;
  if (bidx >= 0) {
    item->label = lbl;
  } else {
    item = (EditMthItem*)mths.New(1);
    item->base = base;
    item->mth = mth;
    item->item_nm = mth->name;
    item->label = lbl;
    BaseAdded(base);
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
  if (!(config.auto_edit || (mbr_bases.size > 0) || (meth_bases.size > 0)))
    return;
  auto_edit = config.auto_edit;
  for (int i = 0; i < mbr_bases.size; ++i) {
    EditMbrItem* item = (EditMbrItem*)mbrs.New(1);
    item->base = mbr_bases.FastEl(i);
    item->label = config.mbr_labels.SafeEl(i);
    item->item_nm = mbr_strs.SafeEl(i);
  }
  for (int i = 0; i < meth_bases.size; ++i) {
    EditMthItem* item = (EditMthItem*)mths.New(1);
    item->base = meth_bases.FastEl(i);
    item->label = config.meth_labels.SafeEl(i);
    item->item_nm = meth_strs.SafeEl(i);
  }
  mbr_bases.Reset();
  config.mbr_labels.Reset();
  mbr_strs.Reset();
  meth_bases.Reset();
  config.meth_labels.Reset();
  meth_strs.Reset();
}

/*void SelectEdit::UpdateAfterEdit() {
//  taNBase::UpdateAfterEdit(); // prob shouldn't do this, because it makes the dialog go Apply/Revert
  if((mbr_base_paths.size > 0) || (meth_base_paths.size > 0)) {
    BaseChangeReShow();		// must have been saved, so reopen it!
  }
  config.mbr_labels.SetSize(mbr_bases.size);
  config.meth_labels.SetSize(meth_bases.size);
  mbr_strs.SetSize(mbr_bases.size);
  meth_strs.SetSize(meth_bases.size);
  if(taMisc::is_loading) {
    GetMembsFmStrs();
    GetMethsFmStrs();
  }
  ReShowEdit(true); //forced
}*/

/*bool SelectEdit::BaseClosing(taBase* base) {
  bool gotone = false;
  int i;
  for(i=mbr_bases.size-1;i>=0;i--) {
    taBase* bs = mbr_bases.FastEl(i);
    if(!bs) continue;
    char* staddr = (char*)bs;
    char* endaddr=staddr+bs->GetSize();
    char* vbase = (char*)base;
    if((vbase >= staddr) && (vbase <= endaddr)) {
      RemoveField_impl(i);
      gotone = true;
    }
  }

  for(i=meth_bases.size-1;i>=0;i--) {
    taBase* bs = meth_bases.FastEl(i);
    if(bs == base) {
      RemoveFun_impl(i);
      gotone = true;
    }
  }
  if (gotone) ReShowEdit(true);
  return gotone;
}

bool SelectEdit::BaseDataChanged(taBase* base,
    int dcr, void* op1_, void* op2_) 
{
  bool rval = false;
  int i;
  for(i=mbr_bases.size-1;i>=0;i--) {
    taBase* bs = mbr_bases.FastEl(i);
    if(!bs) continue;
    char* staddr = (char*)bs;
    char* endaddr=staddr+bs->GetSize();
    char* vbase = (char*)base;
    if((vbase >= staddr) && (vbase <= endaddr)) {
      rval = true;
      break;
    }
  }

  if (!rval) for(i=meth_bases.size-1;i>=0;i--) {
    taBase* bs = meth_bases.FastEl(i);
    if(bs == base) {
      rval = true;
      break;
    }
  }
  if (rval) DataChanged(DCR_ITEM_UPDATED, NULL, NULL);

  return rval;
}*/


/*void SelectEdit::BaseChangeReShow() {
  if((mbr_base_paths.size == 0) && (meth_base_paths.size == 0)) return;

  if(mbr_base_paths.size > 0) {
    mbr_bases.Reset();		// get rid of the mbr_bases!
    int i;
    for(i=0;i<mbr_base_paths.size;i++) {
      String path = mbr_base_paths.FastEl(i);
      taBase* bs = tabMisc::root->FindFromPath(path);
      if(!bs) {
	taMisc::Error("SelectEdit::BaseChangeReOpen: could not find object from path:",path);
	members.RemoveIdx(i);  mbr_strs.RemoveIdx(i);  config.mbr_labels.RemoveIdx(i);  mbr_base_paths.RemoveIdx(i);
	i--;
	continue;
      }
      mbr_bases.Link(bs);
    }
    mbr_base_paths.Reset();
  }

  if(meth_base_paths.size > 0) {
    meth_bases.Reset();		// get rid of the meth_bases!
    int i;
    for(i=0;i<meth_base_paths.size;i++) {
      String path = meth_base_paths.FastEl(i);
      taBase* bs = tabMisc::root->FindFromPath(path);
      if(!bs) {
	taMisc::Error("SelectEdit::BaseChangeReOpen: could not find object from path:",path);
	methods.RemoveIdx(i);  meth_strs.RemoveIdx(i);  config.meth_labels.RemoveIdx(i);  meth_base_paths.RemoveIdx(i);
	i--;
	continue;
      }
      meth_bases.Link(bs);
    }
    meth_base_paths.Reset();
  }

  ReShowEdit(true); //forced
}*/

/*int SelectEdit::Dump_Load_Value(istream& strm, taBase* par) {
  members.Reset();
  mbr_bases.Reset();
  mbr_strs.Reset();
  config.mbr_labels.Reset();

  methods.Reset();
  meth_bases.Reset();
  meth_strs.Reset();
  config.meth_labels.Reset();

  return inherited::Dump_Load_Value(strm, par);
  ReShowEdit(true); //forced
} */

/*void SelectEdit::GetAllPaths() {
  if(mbr_bases.size > 0) {
    mbr_base_paths.Reset();
    for(int i=0;i<mbr_bases.size;i++) {
      mbr_base_paths.Add(mbr_bases.FastEl(i)->GetPath());
    }
  }
  if(meth_bases.size > 0) {
    meth_base_paths.Reset();
    int i;
    for(i=0;i<meth_bases.size;i++) {
      meth_base_paths.Add(meth_bases.FastEl(i)->GetPath());
    }
  }
}

void SelectEdit::UpdateAllBases() {
  int i;
  for(i=0;i<mbr_bases.size;i++) {
    taBase* bs = mbr_bases.FastEl(i);
    if(!bs) continue;
    bs->UpdateAfterEdit();
    taiMisc::Update(bs);
  }
  for(i=0;i<meth_bases.size;i++) {
    taBase* bs = meth_bases.FastEl(i);
    if(bs == NULL) continue;
    bs->UpdateAfterEdit();
  }
} */


/*void SelectEdit::NewEdit() {
  DataChanged(DCR_STRUCT_UPDATE_BEGIN);
  DataChanged(DCR_STRUCT_UPDATE_END);
}*/

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
