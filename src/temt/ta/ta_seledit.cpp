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

#include <QFileInfo>
#include <QTextStream>
#include "ta_type.h"
#include "ta_platform.h"
#include "ta_project.h"
#include "ta_qt.h"
#include "ta_datatable.h"
#include "Subversion.h"

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

void SelectEdit::RunOnCluster(
  const String &repo,
  const String &prefix,
  const String &filename,
  const String &desc,
  int num_of_procs)
{
  // Run this model on a cluster using the parameters of this SelectEdit.
  taMisc::Info("RunOnCluster() test");

  // TODO: need to get username from somewhere else.
  String username = "houmanwc";
  String wc_path = taMisc::user_app_dir + PATH_SEP + "repos" + PATH_SEP + username;

  // Create Subversion client for this working copy directory.
  SubversionClient svnClient(wc_path.chars());

  // TODO: for now I set all the required parameters here
  String repo_path = repo;
  String user_app_dir = "/home/houman/Desktop/";  // path to the emergent directory
  String repo_user_path = repo_path + PATH_SEP + "repos" + PATH_SEP + username;  // path to the user's dir in the repo
  String proj_name = "myproj";
  String wc_proj_path = wc_path + PATH_SEP + proj_name;
  String submit_path = wc_proj_path + PATH_SEP + "submit";  // a subdir of the project
  String models_path = wc_proj_path + PATH_SEP + "models";  // a subdir of the project to contain model files
  String results_path = wc_proj_path + PATH_SEP + "results";  // a subdir of the project to contain results
  String repo_proj_path = repo_path + PATH_SEP + proj_name;

  // check if the user has a wc. checkout a wc if needed
  QFileInfo fi_wc(wc_path.chars());
  if (!fi_wc.exists()) {  // user never used c2c or at least never used it on this emergent instance

    // checkout the user's dir
    int co_rev = 0;
    co_rev = svnClient.Checkout(repo_user_path);
    //PrintCheckoutMessage(co_rev, repo_user_path, wc_path);

    // check if the user has a dir in the repo. create it for her if needed
    if (!fi_wc.exists()) {  // user doesn't have a dir in the repo
      // TODO: create a dir for the user in the repo directly
      co_rev = svnClient.Checkout(repo_user_path);
      //PrintCheckoutMessage(co_rev, repo_user_path, wc_path);
    }
  }
  else {  // update the existing wc
    bool update_success = svnClient.Update();
    //PrintUpdateMessage(update_success, wc_proj_path);
  }

  // check if the the project's dir already exists. create the project's dir and subdirs if needed
  QFileInfo fi_proj(wc_proj_path);
  if (!fi_proj.exists()) {// the project already exists (possibilities: user is running the same project, running the same project with different parameters, duplicate submission)
    // TODO warn user. what should be done here? (options: create a new dir for the project with a version number like "/project_2", use the existing project dir and attach a version number to the model file names, replace the old project with the new one)
  }
  else {
    bool mkdir_success = false;
    mkdir_success = svnClient.MakeDir(submit_path);
    //PrintMkdirMessage(mkdir_success, submit_path);
    mkdir_success = svnClient.MakeDir(models_path);
    //PrintMkdirMessage(mkdir_success, models_path);
    mkdir_success = svnClient.MakeDir(results_path);
    //PrintMkdirMessage(mkdir_success, results_path);
    // TODO set mkdir to commit immediately after adding dirs to avoid an explicit commit
  }

  // generate a txt file containing the model parameters
  String model_filename = "model.txt";  // TODO might need a version number
  String model_path = models_path + PATH_SEP + model_filename;
  QFile file(model_path);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  QTextStream out(&file);
  out << "<CONTENT OF THE MODEL>";  // TODO Where can I get the model parameters from?
  // optional, as QFile destructor will already do it
  file.close();

  // add the generated model file to the wc and commit it
  bool mkdir_success = false;
  mkdir_success = svnClient.MakeDir(model_path);
  // TODO set mkdir to commit immediately after adding dirs to avoid an explicit commit
  //PrintMkdirMessage(mkdir_success, model_path);
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
