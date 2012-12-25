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
#include "ta_datatable.h"
#include "ClusterManager.h"
#include "Subversion.h"
#include "ta_datatable_qtso.h"

//////////////////////////////////
//  taBase                      //
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
//   if (desc.empty())
//     MemberDef::GetMembDesc(mbr, desc, "");
}


//////////////////////////////////
//  SelectEditItem              //
//////////////////////////////////

SelectEditItem* SelectEditItem::StatFindItemBase(const taGroup_impl* grp,
   taBase* base, TypeItem* ti, int& idx)
{
  idx = 0;
  FOREACH_ELEM_IN_GROUP(SelectEditItem, rval, *grp) {
    if ((rval->base == base) && (rval->typeItem() == ti))
      return rval;
    ++idx;
  }
  idx = -1;
  return NULL;
}

bool SelectEditItem::StatGetBase_Flat(const taGroup_impl* grp, int idx,
  taBase*& base)
{
  SelectEditItem* sei = dynamic_cast<SelectEditItem*>(grp->Leaf_(idx));
  if (sei) {
    base = sei->base;
    return true;
  }
  return false;
}

bool SelectEditItem::StatHasBase(taGroup_impl* grp, taBase* base) {
  FOREACH_ELEM_IN_GROUP(SelectEditItem, ei, *grp) {
    if (ei->base == base) return true;
  }
  return false;
}

bool SelectEditItem::StatRemoveItemBase(taGroup_impl* grp, taBase* base) {
  bool rval = false;
  FOREACH_ELEM_IN_GROUP_REV(SelectEditItem, ei, *grp) {
    if (ei->base == base) {
      rval = true;
      ei->Close();
    }
  }
  return rval;
}


void SelectEditItem::Initialize() {
  base = NULL;
  cust_desc = false;
}

void SelectEditItem::Destroy() {
}

void SelectEditItem::Copy_(const SelectEditItem& cp) {
  label = cp.label;
  desc = cp.desc;
  cust_desc = cp.cust_desc;
  prv_desc = desc;             // no change here
  base = cp.base;
  item_nm = cp.item_nm;
}

void SelectEditItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!cust_desc && !taMisc::is_loading && desc != prv_desc) {
    cust_desc = true;
  }
  prv_desc = desc;
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
//  EditMbrItem         //
//////////////////////////////////

void EditParamSearch::Initialize() {
  search = false;
  min_val = 0.0f;
  max_val = 1.0f;
  next_val = 0.0f;
  incr = 0.1f;
}

void EditParamSearch::Destroy() {
}

void EditMbrItem::Initialize() {
  mbr = NULL;
  is_numeric = false;
}

void EditMbrItem::Destroy() {
}

void EditMbrItem::InitLinks() {
  inherited::InitLinks();
  taBase::Own(param_search, this);
}

void EditMbrItem::Copy_(const EditMbrItem& cp) {
  mbr = cp.mbr;
}

void EditMbrItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (!cust_desc && mbr) {
    desc = _nilString;
    MemberDef::GetMembDesc(mbr, desc, "");
    prv_desc = desc;
  }
  is_numeric = false;
  if(mbr && !mbr->type->InheritsNonAtomicClass()) {
    if(mbr->type->InheritsFrom(&TA_float) || mbr->type->InheritsFrom(&TA_double)
       || mbr->type->InheritsFrom(&TA_int) || mbr->type->InheritsFrom(&TA_int64_t)) {
      if(!mbr->HasOption("READ_ONLY") && !mbr->HasOption("GUI_READ_ONLY"))
        is_numeric = true;
    }
  }
}

String EditMbrItem::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "mbr_type")
    return (mbr) ? mbr->type->name : String("NULL");
  else return inherited::GetColText(key, itm_idx);
}

bool EditMbrItem::PSearchValidTest() {
  if(TestError(!mbr, "PSearchValidTest", "item does not have member def set -- not valide parameter search item"))
    return false;
  if(TestError(!is_numeric, "PSearchValidTest", "item is not numeric and thus not a valid parameter search item.  member name:", mbr->name, "label:", label))
    return false;
  return true;
}

Variant EditMbrItem::PSearchCurVal() {
  if(!PSearchValidTest()) return 0.0;
  return mbr->type->GetValVar(mbr->GetOff(base), mbr);
}

bool EditMbrItem::PSearchCurVal_Set(const Variant& cur_val) {
  if(!PSearchValidTest()) return false;
  mbr->type->SetValVar(cur_val, mbr->GetOff(base), NULL, mbr);
  base->UpdateAfterEdit();
  return true;
}

bool EditMbrItem::PSearchMinToCur() {
  if(!PSearchValidTest()) return false;
  mbr->type->SetValVar(param_search.min_val, mbr->GetOff(base), NULL, mbr);
  base->UpdateAfterEdit();
  return true;
}

bool EditMbrItem::PSearchNextIncr() {
  if(!PSearchValidTest()) return false;
  double cur_val = PSearchCurVal().toDouble();
  param_search.next_val = cur_val + param_search.incr;
  if(param_search.next_val > param_search.max_val) {
    param_search.next_val = param_search.min_val;
    return false;
  }
  return true;
}

bool EditMbrItem::PSearchNextToCur() {
  if(!PSearchValidTest()) return false;
  mbr->type->SetValVar(param_search.next_val, mbr->GetOff(base), NULL, mbr);
  base->UpdateAfterEdit();
  return true;
}

//////////////////////////////////
//  EditMthItem_Group           //
//////////////////////////////////

void EditMbrItem_Group::DataChanged(int dcr, void* op1, void* op2)
{
  inherited::DataChanged(dcr, op1, op2);
  SelectEdit::StatDataChanged_Group(this, dcr, op1, op2);
}

taBase* EditMbrItem_Group::GetBase_Flat(int idx) const {
  taBase* rval = NULL;
  SelectEditItem::StatGetBase_Flat(this, idx, rval);
  return rval;
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

//////////////////////////////////////////////////
//      Parameter Searching Interface

EditMbrItem* EditMbrItem_Group::FindMbrName(const String& mbr_nm, const String& label) {
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr) continue;
    if(sei->mbr->name == mbr_nm) {
      if(label.nonempty()) {
        if(sei->label.contains(label)) {
          return sei;
        }
      }
    }
  }
  return NULL;
}

EditMbrItem* EditMbrItem_Group::PSearchFind(const String& mbr_nm, const String& label) {
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric) continue;
    if(sei->mbr->name == mbr_nm) {
      if(label.nonempty()) {
        if(sei->label.contains(label)) {
          return sei;
        }
      }
    }
  }
  TestError(true, "PSearchFind", "could not find member named:", mbr_nm, "label:", label);
  return NULL;
}

EditMbrItem* EditMbrItem_Group::PSearchNext(int& st_idx) {
  while(st_idx < leaves) {
    EditMbrItem* sei = Leaf(st_idx);
    if(!sei->mbr || !sei->is_numeric || !sei->param_search.search) {
      st_idx++;
      continue;
    }
    return sei;
  }
  return NULL;
}


bool& EditMbrItem_Group::PSearchOn(const String& mbr_nm, const String& label) {
  static bool no_val = false;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return sei->param_search.search;
}

bool EditMbrItem_Group::PSearchOn_Set(bool psearch, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  sei->param_search.search = psearch;
  return true;
}

double& EditMbrItem_Group::PSearchMinVal(const String& mbr_nm, const String& label) {
  static double no_val = 0;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return sei->param_search.min_val;
}

bool EditMbrItem_Group::PSearchMinVal_Set(double min_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  sei->param_search.min_val = min_val;
  return true;
}

double& EditMbrItem_Group::PSearchMaxVal(const String& mbr_nm, const String& label) {
  static double no_val = 0;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return sei->param_search.max_val;
}

bool EditMbrItem_Group::PSearchMaxVal_Set(double max_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  sei->param_search.max_val = max_val;
  return true;
}

double& EditMbrItem_Group::PSearchNextVal(const String& mbr_nm, const String& label) {
  static double no_val = 0;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return sei->param_search.next_val;
}

bool EditMbrItem_Group::PSearchNextVal_Set(double next_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  sei->param_search.next_val = next_val;
  return true;
}

double& EditMbrItem_Group::PSearchIncrVal(const String& mbr_nm, const String& label) {
  static double no_val = 0;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return sei->param_search.incr;
}
bool EditMbrItem_Group::PSearchIncrVal_Set(double incr_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  sei->param_search.incr = incr_val;
  return true;
}

Variant EditMbrItem_Group::PSearchCurVal(const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return _nilVariant;
  return sei->PSearchCurVal();
}

bool EditMbrItem_Group::PSearchCurVal_Set(const Variant& cur_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  return sei->PSearchCurVal_Set(cur_val);
}

bool EditMbrItem_Group::PSearchNextToCur(const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  return sei->PSearchNextToCur();
}

bool EditMbrItem_Group::PSearchMinToCur_All() {
  bool rval = false;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric || !sei->param_search.search) continue;
    bool psr = sei->PSearchMinToCur();
    rval |= psr;
  }
  return rval;
}

bool EditMbrItem_Group::PSearchNextIncr_Grid() {
  int cur_idx = 0;
  while(true) {
    EditMbrItem* sei = PSearchNext(cur_idx);
    if(!sei) return false;
    if(sei->PSearchNextIncr())
      return true;
    cur_idx++;
  }
  return false;                 // never gets here..
}

bool EditMbrItem_Group::PSearchNextToCur_All() {
  bool rval = false;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric || !sei->param_search.search) continue;
    bool psr = sei->PSearchNextToCur();
    rval |= psr;
  }
  return rval;
}

void EditMbrItem_Group::PSearchConfigTable(DataTable* dat, bool all_nums, bool add_eval) {
  if(TestError(!dat, "PSearchConfigTable", "data table is NULL"))
    return;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric) continue;
    if(!all_nums && !sei->param_search.search) continue;
    String nm = taMisc::StringCVar(sei->label);
    dat->FindMakeCol(nm, VT_DOUBLE);
  }
  if(add_eval)
    dat->FindMakeCol("eval", VT_DOUBLE);
}

void EditMbrItem_Group::PSearchRecordData(DataTable* dat, bool all_nums, bool add_eval,
                                          double eval_val) {
  if(TestError(!dat, "PSearchRecord", "data table is NULL"))
    return;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric) continue;
    if(!all_nums && !sei->param_search.search) continue;
    String nm = taMisc::StringCVar(sei->label);
    dat->SetValColName(sei->PSearchCurVal(), nm, -1);
  }
  if(add_eval)
    dat->SetValColName(eval_val, "eval", -1);
  dat->WriteClose();
}

//////////////////////////////////
//  EditMthItem                 //
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
  // version 5.0.2 -- update to new Gui buttons for Run and Step
  if(mth) {
    if(((mth->name == "Run") || (mth->name == "Step")) && !mth->im) {
      MethodDef* nwmth = mth->owner->FindName(mth->name + "_Gui");
      if(nwmth) {
        mth = nwmth;
      }
    }
  }
  if(!cust_desc && mth) {
    desc = mth->desc;
    prv_desc = desc;
  }
  inherited::UpdateAfterEdit_impl();
}


//////////////////////////////////
//  EditMthItem_Group           //
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


taBase* EditMthItem_Group::GetBase_Flat(int idx) const {
  taBase* rval = NULL;
  SelectEditItem::StatGetBase_Flat(this, idx, rval);
  return rval;
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
//  SelectEdit                  //
//////////////////////////////////

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

bool SelectEdit::ReShowEdit(bool force) {
  if(!taMisc::gui_active) return false;
#ifdef TA_GUI
  if (force) { // ugh
    DataChanged(DCR_STRUCT_UPDATE_BEGIN);
    DataChanged(DCR_STRUCT_UPDATE_END);
  }
  else {
    DataChanged(DCR_ITEM_UPDATED);
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

//////////////////////////////////
//      LEGACY  //
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


/////////////////////////////////////////////////////
//      ParamSearchAlgo, GridSearch, etc.

void
ParamSearchAlgo::Initialize()
{
  m_cluster_run = GET_MY_OWNER(ClusterRun);
}

void
ParamSearchAlgo::Reset()
{
}

bool
ParamSearchAlgo::CreateJobs()
{
  return false;
}

void
ParamSearchAlgo::ProcessResults()
{
}

String
ParamSearchAlgo::BuildCommand()
{
  // Start command with either "emergent" or "emergent_mpi".
  String cmd(taMisc::app_name);
  if (m_cluster_run->use_mpi) {
    cmd.cat("_mpi");
  }

  // The cluster script needs to substitute the correct relative
  // filename for the project file in its working copy.  It also
  // needs to substitute the tag, which is based on the revision
  // and row number.
  cmd.cat(" -nogui -ni -p <PROJ_FILENAME> tag=<TAG>");

  if (m_cluster_run->use_mpi) {
    // TODO: What's the right switch(es) for MPI settings?
    cmd.cat(" --mpi_nodes ").cat(String(m_cluster_run->mpi_nodes));
  }

  if (m_cluster_run->n_threads > 0) {
    cmd.cat(" --n_threads ").cat(String(m_cluster_run->n_threads));
  }

  // Add a name=val term for each parameter in the search.
  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, m_cluster_run->mbrs) {
    const EditParamSearch &ps = mbr->param_search;
    if (ps.search) {
      cmd.cat(" ").cat(mbr->GetName()).cat("=").cat(String(ps.next_val));
    }
  }

  return cmd;
}


void
GridSearch::Initialize()
{
  max_jobs = 20;
}

void
GridSearch::Reset()
{
  // TODO: shouldn't need to do this here, since it's done in
  // ParamSearchAlgo::Initialize(), but for some reason that
  // doesn't work.
  m_cluster_run = GET_MY_OWNER(ClusterRun);

  // Build the m_counts and m_names arrays.
  m_names.Reset();
  m_counts.Reset();
  int total_jobs = 1;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, m_cluster_run->mbrs) {
    EditParamSearch &ps = mbr->param_search;
    if (ps.search) {
      // Keep track of names to make sure they haven't changed by the time
      // ProcessResults is called.
      String name = mbr->GetName();
      m_names.Push(name);

      // Sanity check range.
      if (ps.max_val <= ps.min_val || ps.incr > (ps.max_val - ps.min_val)) {
        taMisc::Warning("ClusterRun search range invalid for parameter", name);
      }

      // Count how many values this parameter will take.  The +1 is to
      // include the beginning and end points; the +.01 is for rounding
      // error.  For example, (0.9 - 0.2) / 0.1 + 1.01 == 8.
      int count = (ps.max_val - ps.min_val) / ps.incr + 1.01;
      m_counts.Push(count);
      total_jobs *= count;

      // Reset the next_val to the minimum.
      ps.next_val = ps.min_val;
    }
  }

  // Initialize the iterator array to all 0s.
  m_iter.SetSize(m_counts.size);
  m_iter.InitVals(0);

  m_cmd_id = 0;
  m_all_jobs_created = false;
}

bool
GridSearch::CreateJobs()
{
  // If we've already created all jobs for this search, don't create any more.
  if (m_all_jobs_created) return false;

  // If max_jobs is 0, create them all.
  bool create_all = (max_jobs <= 0);

  // Check how many jobs are currently running so we know how many more
  // we can create.
  int num_jobs = max_jobs;
  if (!create_all) {
    int running_jobs = m_cluster_run->CountJobs(
      m_cluster_run->jobs_running,
      "SUBMITTED|QUEUED|RUNNING");
    num_jobs -= running_jobs;

    // Check if we can't create any more jobs because the maximum number
    // allowed are already running.
    if (num_jobs <= 0) {
      return false;
    }
  }

  // Create num_jobs jobs (or all jobs if create_all set).
  m_cluster_run->jobs_submit.ResetData();
  for (int idx = 0; idx < num_jobs || create_all; ++idx, ++m_cmd_id) {
    // Add the current job.
    String cmd = BuildCommand();
    m_cluster_run->AddJobRow(cmd, m_cmd_id);

    // Cycle parameters for the next job.
    if (!nextParamCombo()) {
      // No next combo means all jobs have been created.
      m_all_jobs_created = true;
      break;
    }
  }

  // Success: jobs created.
  return true;
}

bool
GridSearch::nextParamCombo()
{
  // Cycle parameters for the next job by adding 1 to the least-significant
  // element in the iterator array, and carrying if needed.
  int idx = 0;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, m_cluster_run->mbrs) {
    EditParamSearch &ps = mbr->param_search;
    if (ps.search) {
      // Sanity check that the parameters to search haven't changed.
      String name = mbr->GetName();
      if (name != m_names[idx]) {
        taMisc::Error("Search parameters have changed; expected parameter",
          m_names[idx], "at index", String(idx), "but got", name);
        return false;
      }

      // Increment the current iterator element.
      if (++m_iter[idx] < m_counts[idx]) {
        // No carry, so incrementing the iterator is complete.
        ps.next_val = ps.min_val + (m_iter[idx] * ps.incr);
        return true;
      }

      // Iterator elem rolled over; reset to 0/min and carry the 1 to
      // the next element (if there is one).
      m_iter[idx] = 0;
      ps.next_val = ps.min_val;
      if (++idx >= m_iter.size) {
        // The iterator overflowed (all elements rolled over to 0)
        // so we're done creating jobs.
        return false;
      }
    }
  }

  // Should never get here due to the idx>=m_iter.size check.
  return false;
}

void
GridSearch::ProcessResults()
{
  // TODO: load result files??
}


/////////////////////////////////////////////////////
//      Cluster Run

void ClusterRun::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ClusterRun);
  FormatTables();
}

void ClusterRun::Initialize() {
  ram_gb = -1;
  n_threads = taMisc::thread_defaults.n_threads;
  use_mpi = false;
  mpi_nodes = 10;
  m_cm = 0;
}

void ClusterRun::Destroy() {
  delete m_cm; m_cm = 0;
}

void ClusterRun::initClusterManager() {
  m_cm = new ClusterManager(*this);
}

void ClusterRun::NewSearchAlgo(TypeDef *type) {
  search_algos.New(1, type);
}

void ClusterRun::Run() {
  initClusterManager();         // ensure it has been created.
  FormatTables();               // ensure tables are formatted properly
  jobs_submit.ResetData();      // clear the submission table
  bool prompt_user = true;      // always prompt the user on a new run.
  if (m_cm->BeginSearch(prompt_user)) {
    // Get revisions of the committed project and jobs_submit.dat files.
    String wc_proj = m_cm->GetWcProjFilename();
    String wc_submit = m_cm->GetWcSubmitFilename();
    if (!wc_proj.empty() && !wc_submit.empty()) {
      int model_rev = m_cm->GetLastChangedRevision(wc_proj);
      int submit_rev = m_cm->GetLastChangedRevision(wc_submit);

      // Put those revisions into the datatable just committed.
      // (There's no way to put them in *before* committing.)
      for (int row = 0; row < jobs_submit.rows; ++row) {
        jobs_submit.SetValColName(model_rev, "model_svn", row);
        jobs_submit.SetValColName(submit_rev, "submit_svn", row);
      }
    }
  }
}

bool ClusterRun::Update() {
  initClusterManager(); // ensure it has been created.

  // Update the working copy and load the running/done tables.
  bool has_updates = m_cm->UpdateTables();
  if (has_updates && cur_search_algo) {
    cur_search_algo->ProcessResults();
  }
  return has_updates;
}

void ClusterRun::Cont() {
  initClusterManager(); // ensure it has been created.

  // Create the next batch of jobs.
  if (cur_search_algo && cur_search_algo->CreateJobs()) {
    // Commit the table to submit the jobs.
    m_cm->CommitJobSubmissionTable();
  }
}

void ClusterRun::Kill() {
  initClusterManager(); // ensure it has been created.

  // Get the (inclusive) range of rows to kill.
  int st_row, end_row;
  if (SelectedRows(jobs_running, st_row, end_row)) {
    // Populate the jobs_submit table with CANCEL requests for the selected jobs.
    jobs_submit.ResetData();
    for (int row = st_row; row <= end_row; ++row) {
      CancelJob(row);
    }

    // Commit the table.
    m_cm->CommitJobSubmissionTable();
  }
}

void ClusterRun::ImportData() {
}

void ClusterRun::FormatTables() {
  jobs_submit.name = "jobs_submit";
  jobs_running.name = "jobs_running";
  jobs_done.name = "jobs_done";

  FormatTables_impl(jobs_submit);
  FormatTables_impl(jobs_running);
  FormatTables_impl(jobs_done);
}

void ClusterRun::FormatTables_impl(DataTable& dt) {
  DataCol* dc;

  // The cluster script populates these fields in the running/done tables.
  // The client (this code) can set them in the submit table, but there's not
  // much point since they can't be set until *after* the table is committed.
  dc = dt.FindMakeCol("model_svn", VT_STRING);
  dc->desc = "svn revision for the model";
  dc = dt.FindMakeCol("submit_svn", VT_STRING);
  dc->desc = "svn revision for the job submission commands";

  // Cluster script populates.
  dc = dt.FindMakeCol("submit_job", VT_STRING);
  dc->desc = "index of job number within a given submission -- equal to the row number of the original set of jobs submitted in submit_svn jobs";
  dc = dt.FindMakeCol("job_no", VT_STRING);
  dc->desc = "job number on cluster -- assigned once the job is submitted to the cluster";
  dc = dt.FindMakeCol("tag", VT_STRING);
  dc->desc = "unique tag id for this job -- all files etc are named according to this tag";

  // The client sets this field in the jobs_submit table to:
  //   REQUESTED to request the job be submitted.
  //   CANCELLED to request the job indicated by job_no or tag be cancelled.
  // The cluster script sets this field in the running/done tables to:
  //   SUBMITTED after job successfully submitted to a queue.
  //   QUEUED    when the job is known to be in the cluster queue.
  //             At this point, we have a job number (job_no).
  //   RUNNING   when the job has begun.
  //   DONE      if the job completed successfully.
  //   KILLED    if the job was cancelled.
  dc = dt.FindMakeCol("status", VT_STRING);
  dc->desc = "status of job: REQUESTED, CANCELLED, SUBMITTED, QUEUED, RUNNING, DONE, KILLED";

  // Cluster script populates.
  dc = dt.FindMakeCol("job_out", VT_STRING);
  dc->desc = "job output information -- contains (top of) the job standard output and standard error output as the job is running (truncated to top 2048 characters if longer than that) -- full information available in job_out_file";
  dc = dt.FindMakeCol("job_out_file", VT_STRING);
  dc->desc = "job output file -- file name containing full job output information -- file name should be tag.out";
  dc = dt.FindMakeCol("dat_files", VT_STRING);
  dc->desc = "list of data table output files generated by model -- files are named as  tag_<extra>.out -- this list automatically generated by parsing the result files checked into svn";

  // Search algo populates these fields.
  dc = dt.FindMakeCol("command_id", VT_INT);
  dc->desc = "id for this command, assigned by the search algorithm in an algorithm-specific manner (optional)";
  dc = dt.FindMakeCol("command", VT_STRING);
  dc->desc = "emergent command line";

  // Populated from values the user enters/chooses.
  dc = dt.FindMakeCol("notes", VT_STRING);
  dc->desc = "notes for the job -- describe any specific information about the model configuration etc -- can use this for searching and sorting results";
  dc = dt.FindMakeCol("repo_url", VT_STRING);
  dc->desc = "name of repository to run job on";
  dc = dt.FindMakeCol("cluster", VT_STRING);
  dc->desc = "name of cluster to run job on";
  dc = dt.FindMakeCol("queue", VT_STRING);
  dc->desc = "if specified, indicate a particular queue on the computing resource";
  dc = dt.FindMakeCol("run_time", VT_STRING);
  dc->desc = "how long will the jobs take to run -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate";
  dc = dt.FindMakeCol("ram_gb", VT_INT);
  dc->desc = "how many gigabytes of ram is required?  -1 means do not specify this parameter for the job submission -- for large memory jobs, it can be important to specify this to ensure proper allocation of resources";
  dc = dt.FindMakeCol("n_threads", VT_INT);
  dc->desc = "number of parallel threads to use for running";
  dc = dt.FindMakeCol("mpi_nodes", VT_INT);
  dc->desc = "number of nodes to use for mpi run -- 0 or -1 means not to use mpi";
}

void
ClusterRun::AddJobRow(const String &cmd, int cmd_id)
{
  int row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("REQUESTED", "status",     row);
  jobs_submit.SetVal(cmd_id,      "command_id", row);
  jobs_submit.SetVal(cmd,         "command",    row);

  jobs_submit.SetVal(repo_url,    "repo_url",   row);
  jobs_submit.SetVal(cluster,     "cluster",    row);
  jobs_submit.SetVal(queue,       "queue",      row);
  jobs_submit.SetVal(run_time,    "run_time",   row);
  jobs_submit.SetVal(ram_gb,      "ram_gb",     row);
  jobs_submit.SetVal(n_threads,   "n_threads",  row);
  jobs_submit.SetVal(mpi_nodes,   "mpi_nodes",  row);
}

void
ClusterRun::CancelJob(int running_row)
{
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("CANCELLED", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, jobs_running, "job_no", running_row);
  jobs_submit.CopyCell("tag", dst_row, jobs_running, "tag", running_row);
}

int
ClusterRun::CountJobs(const DataTable &table, const String &status_regexp)
{
  int count = 0;
  QRegExp re(status_regexp.toQString());
  for (int row = 0; row < table.rows; ++row) {
    QString status = table.GetValAsString("status", row).toQString();
    if (status.contains(re)) ++count;
  }
  return count;
}

iDataTableEditor* ClusterRun::DataTableEditor(DataTable& dt) {
  // String strm;
  // dt.ListDataClients(strm);
  // taMisc::ConsoleOutput(strm);
  taDataLink* dl = dt.data_link();
  if(dl) {
    taDataLinkItr itr;
    iDataTablePanel* el;
    FOR_DLC_EL_OF_TYPE(iDataTablePanel, el, dl, itr) {
      return el->dte;
    }
  }
  return NULL;
}

bool ClusterRun::SelectedRows(DataTable& dt, int& st_row, int& end_row) {
  st_row = -1;
  end_row = -1;
  iDataTableEditor* ed = DataTableEditor(dt);
  if(!ed || !ed->tvTable) return false;
  bool rval = ed->tvTable->SelectedRows(st_row, end_row);
  if(!rval) {
    taMisc::Info("no items selected");
    return false;
  }
  taMisc::Info("start row:", String(st_row), "end row:", String(end_row));
  return true;
}
