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

#include "ControlPanel.h"
#include <voidptr_PArray>
#include <taGuiDialog>
#include <Program>

taTypeDef_Of(taProject);
SMARTREF_OF_CPP(ControlPanel); //


#include <SigLinkSignal>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ControlPanel);



void ControlPanel::StatSigEmit_Group(taGroup_impl* grp, int sls,
  void* op1, void* op2)
{
  if (!grp->owner || !grp->owner->InheritsFrom(&TA_ControlPanel)) return;
  ((ControlPanel*)(grp->owner))->SigEmit_Group(grp, sls, op1, op2);
}


void ControlPanel::Initialize() {
  auto_edit = true;
  running_updt = false;
  m_changing = 0;
  base_refs.setOwner(this);
}

void ControlPanel::Destroy() {
  ++m_changing;
  base_refs.Reset();
  mbrs.Reset();
  mths.Reset();
  --m_changing;
}

void ControlPanel::Copy_(const ControlPanel& cp) {
  auto_edit = cp.auto_edit;
  desc = cp.desc;
  base_refs.Reset(); // should get added by copies below
  mbrs = cp.mbrs;
  mths = cp.mths;

  UpdatePointers_NewPar_IfParNotCp((taBase*)&cp, &TA_taProject);
}

void ControlPanel::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (taMisc::is_loading) {
    // add all the bases, since they weren't available during load
    FOREACH_ELEM_IN_GROUP(ControlPanelItem, sei, mbrs) {
      BaseAdded(sei);
    }
    FOREACH_ELEM_IN_GROUP(ControlPanelItem, sei, mths) {
      BaseAdded(sei);
    }
  }
}

int ControlPanel::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int rval = base_refs.UpdatePointers_NewPar(old_par, new_par);
  rval += inherited::UpdatePointers_NewPar(old_par, new_par);
  return rval;
}

void ControlPanel::BaseAdded(ControlPanelItem* sei) {
  if(!sei->base) return;
  bool add_base = true;
  if(sei->InheritsFrom(&TA_EditMthItem)) {
    add_base = false;         // in general don't add for meths
    if(sei->base->InheritsFrom(&TA_Program))
      add_base = true;       // except for programs
  }
  if(add_base)
    base_refs.AddUnique(sei->base);
}

void ControlPanel::BaseRemoved(ControlPanelItem* sei) {
  taBase* base = sei->base;
  if (!base) return;
  bool has = ControlPanelItem::StatHasBase(&mbrs, base);
  if (has) return;
  has = ControlPanelItem::StatHasBase(&mths, base);
  if (has) return;
  // ok, no extant refs to the guy, so remove from list
  base_refs.RemoveEl(base);
}

void ControlPanel::SigDestroying_Ref(taBase_RefList* src, taBase* base) {
  // note: item will already have been removed from list
  if (m_changing) return;
  m_changing++;
  ControlPanelItem::StatRemoveItemBase(&mbrs, base);
  ControlPanelItem::StatRemoveItemBase(&mths, base);
  m_changing--;
}

void ControlPanel::SigEmit_Ref(taBase_RefList* src, taBase* ta,
    int sls, void* op1, void* op2)
{
  if(sls >= SLS_UPDATE_VIEWS) return;
  if(!running_updt) {
    if(Program::global_run_state == Program::RUN ||
       Program::global_run_state == Program::INIT) {
      if(ta && !ta->InheritsFrom(&TA_Program))
        return;                     // skip any non-program updates while running!
    }
  }
  SigEmitUpdated();
}

void ControlPanel::SigEmit_Group(taGroup_impl* grp,
    int sls, void* op1, void* op2)
{
  if (m_changing) return;
  if (taMisc::is_loading) return; // note: base's aren't set yet, so we can't add
  if (sls == SLS_GROUP_ITEM_REMOVE) {
    ControlPanelItem* ei = (ControlPanelItem*)op1;
    BaseRemoved(ei);
  }
  else if (sls == SLS_GROUP_ITEM_INSERT) {
    ControlPanelItem* ei = (ControlPanelItem*)op1;
    BaseAdded(ei); // ignored if null, but shouldn't happen anyway
  }
  //pretty much everything else as well, need to reshow
  //note: this is asynch, so multiple events (ex Compare, etc.) will only cause 1 reshow
  ReShowEdit(true);
}

int ControlPanel::CompareObjs(taBase* obj_a, taBase* obj_b, bool no_ptrs) {
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
  taMisc::Info("ControlPanel::CompareObjs generated", (String)mds.size, "differences");
  for(int i=0;i<mds.size;i++) {
    TypeDef* td = base_types[i];
    if(!td->IsActualTaBase()) continue;
    //    MemberDef* md = mds[i];
    taBase* itma = (taBase*)trg_bases[i];
    taBase* itmb = (taBase*)src_bases[i];
    String nma = "A: " + itma->GetDisplayName().elidedTo(20) + "." + mds[i]->name;
    String nmb = "B: " + itmb->GetDisplayName().elidedTo(20) + "." + mds[i]->name;
    SelectMember_impl(itma, mds[i], nma, _nilString);
    SelectMember_impl(itmb, mds[i], nmb, _nilString);
  }
  SigEmitUpdated(); // so name updates in tree
  ReShowEdit(true);
  return mds.size;
}

void ControlPanel::RemoveField(int idx) {
  RemoveField_impl(idx);
  ReShowEdit(true); //forced
}

void ControlPanel::RemoveField_impl(int idx) {
  EditMbrItem* item = mbrs.Leaf(idx);
  if (item)
    item->Close();
}

void ControlPanel::RemoveFun(int idx) {
  RemoveFun_impl(idx);
  ReShowEdit(true); //forced
}

void ControlPanel::RemoveFun_impl(int idx) {
  EditMthItem* item = mths.Leaf(idx);
  if (item)
    item->Close();
}

void ControlPanel::Reset() {
  mbrs.Reset();
  mths.Reset();
  ReShowEdit(true); //forced
}

bool ControlPanel::ReShowEdit(bool force) {
  if(!taMisc::gui_active) return false;
#ifdef TA_GUI
  if (force) { // ugh
    SigEmit(SLS_STRUCT_UPDATE_BEGIN);
    SigEmit(SLS_STRUCT_UPDATE_END);
  }
  else {
    SigEmitUpdated();
  }
//  return taiMisc::ReShowEdits((void*)this, GetTypeDef(), force);
#endif
  return false;
}


int ControlPanel::SearchMembers(taNBase* obj, const String& memb_contains) {
  if(TestError(!obj || memb_contains.empty(), "SearchMembers",
               "null object or empty search")) return -1;
  ControlPanel* se = this;
  int rval = obj->AddToControlPanelSearch(memb_contains, se);
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::SelectMember(taBase* base, MemberDef* mbr,
  const String& xtra_lbl, const String& dscr, const String& sub_gp_nm)
{
  if (!base) return false;
  String eff_desc = dscr; // non-const
  String full_lbl;
  base->GetSelectText(mbr, xtra_lbl, full_lbl, eff_desc);
  bool rval = SelectMember_impl(base, mbr, full_lbl, eff_desc, sub_gp_nm);
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::SelectMemberPrompt(taBase* base, MemberDef* mbr) {
  if (!base) return false;
  String eff_desc; // = desc -- this is our desc -- not relevant
  String full_lbl;
  base->GetSelectText(mbr, _nilString, full_lbl, eff_desc);

  taGuiDialog dlg;
  dlg.Reset();
  dlg.prompt = "Enter label for select edit item -- will be converted to a valid C name automatically";
  dlg.win_title = "Enter ControlPanel label";
  dlg.AddWidget("main", "", "");
  dlg.AddVBoxLayout("mainv","","main","");
  String curow = "lbl";
  dlg.AddHBoxLayout(curow, "mainv","","");
  dlg.AddLabel("full_lbl_lbl", "main", curow, "label=Label: ;");
  dlg.AddStringField(&full_lbl, "full_lbl", "main", curow, "tooltip=enter label to use;");
  int drval = dlg.PostDialog(true);
  if(drval == 0) {
    return false;
  }
  full_lbl = taMisc::StringCVar(full_lbl);
  bool rval = SelectMember_impl(base, mbr, full_lbl, eff_desc, _nilString);
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::SelectMemberNm(taBase* base, const String& md_nm,
  const String& xtra_lbl, const String& dscr, const String& sub_gp_nm)
{
  if(base == NULL) return false;
  MemberDef* md = (MemberDef*)base->FindMember(md_nm);
  if (md == NULL) return false;
  return SelectMember(base, md, xtra_lbl, dscr, sub_gp_nm);
}

bool ControlPanel::SelectMember_impl(taBase* base, MemberDef* md,
            const String& full_lbl, const String& dscr, const String& sub_gp_nm)
{
  int bidx = -1;
  // this looks at the leaves:
  EditMbrItem* item = (EditMbrItem*)ControlPanelItem::StatFindItemBase(&mbrs, base, md, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new EditMbrItem;
    item->base = base;
    item->mbr = md;
    item->item_nm = md->name;
    item->label = full_lbl;
    item->desc = dscr; // even if empty
    if(dscr.nonempty())
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

bool ControlPanel::SelectMethod(taBase* base, MethodDef* md,
  const String& xtra_lbl, const String& dscr, const String& sub_gp_nm)
{
  bool rval = SelectMethod_impl(base, md, xtra_lbl, dscr, sub_gp_nm);
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::SelectMethodNm(taBase* base, const String& md_nm,
  const String& xtra_lbl, const String& dscr, const String& sub_gp_nm)
{
  if(base == NULL) return false;
  MethodDef* md = (MethodDef*)base->GetTypeDef()->methods.FindName(md_nm);
  if (md == NULL) return false;
  return SelectMethod(base, md, xtra_lbl, dscr, sub_gp_nm);
}

bool ControlPanel::SelectMethod_impl(taBase* base, MethodDef* mth,
  const String& xtra_lbl, const String& dscr, const String& sub_gp_nm)
{
  int bidx = -1;
  // this looks at the leaves:
  EditMthItem* item = (EditMthItem*)ControlPanelItem::StatFindItemBase(&mths, base, mth, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new EditMthItem;
    item->base = base;
    item->mth = mth;
    item->item_nm = mth->name;
    item->desc = dscr;
    if(dscr.nonempty())
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


EditMbrItem* ControlPanel::mbr(int i) const {
  return mbrs.Leaf(i);
}

EditMthItem* ControlPanel::mth(int i) const {
  return mths.Leaf(i);
}

int ControlPanel::FindMbrBase(taBase* base, MemberDef* md) {
  int rval = -1;
  ControlPanelItem::StatFindItemBase(&mbrs, base, md, rval);
  return rval;
}

int ControlPanel::FindMethBase(taBase* base, MethodDef* md) {
  int rval = -1;
  ControlPanelItem::StatFindItemBase(&mths, base, md, rval);
  return rval;
}

