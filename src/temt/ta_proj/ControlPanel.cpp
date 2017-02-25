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
#include <ParamSet>
#include <taProject>

#include <QDateTime>

#include <SigLinkSignal>
#include <taMisc>
#include <tabMisc>

taTypeDef_Of(taProject);
SMARTREF_OF_CPP(ControlPanel); //

TA_BASEFUNS_CTORS_DEFN(ControlPanel);

void ControlPanel::StatSigEmit_Group(taGroup_impl* grp, int sls,
  void* op1, void* op2)
{
  if (!grp->owner || !grp->owner->InheritsFrom(&TA_ControlPanel)) return;
  ((ControlPanel*)(grp->owner))->SigEmit_Group(grp, sls, op1, op2);
}


void ControlPanel::Initialize() {
  updt_while_running = false;
  m_changing = 0;
  base_refs.setOwner(this);
  auto_edit = false;
}

void ControlPanel::Destroy() {
  ++m_changing;
  base_refs.Reset();
  mbrs.Reset();
  mths.Reset();
  --m_changing;
}

void ControlPanel::Copy_(const ControlPanel& cp) {
  desc = cp.desc;
  base_refs.Reset(); // should get added by copies below
  mbrs = cp.mbrs;
  mths = cp.mths;
}

void ControlPanel::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (taMisc::is_loading) {
    if (auto_edit) { // obsolete - convert
      SetUserData("user_pinned", true);
      auto_edit = false;
    }
    
    // check that this item is still supported - types are occassionally removed
    for (int i=mbrs.leaves-1; i>=0; i--) {
      ControlPanelMember* item = mbrs.Leaf(i);
      if (item == NULL || item->base == NULL || item->mbr == NULL) {
        taMisc::DebugInfo("ControlPanel::UpdateAfterEdit_impl: could not find item: ", item->label, "in control panel:", name);
        mbrs.RemoveLeafIdx(i);
      }
    }
    
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
  // note: item will already have been removed from ref base list
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
  if(!ta) return;

  bool is_prog = ta->InheritsFrom(&TA_Program);
  
  if(!updt_while_running) {
    if((Program::global_run_state == Program::RUN ||
        Program::global_run_state == Program::INIT) && !is_prog) {
      return;                     // skip any non-program updates while running!
    }
  }

  bool on_meths = ControlPanelItem::StatHasBase(&mths, ta);
  bool on_membs = ControlPanelItem::StatHasBase(&mbrs, ta);

  if(on_meths && !on_membs && !is_prog) { // don't update for any non-program method only cases
    return;
  }

  if(on_membs) {
    FOREACH_ELEM_IN_GROUP_REV(ControlPanelMember, item, mbrs) {
      if (!item->cust_label && ControlPanelItem::StatCheckBase(item, ta)) {
        // regenerate label as spec name or program name etc might have changed
        String new_label;
        ta->GetControlPanelLabel(item->mbr, new_label);
        item->label = new_label;
      }
    }
  }
  
  SigEmitUpdated();             // trigger an update of us -- this is expensive!
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

void ControlPanel::RemoveMember(taBase* base, MemberDef* md) {
  int idx = FindMbrBase(base, md);
  if(idx >= 0) {
    RemoveMemberIdx(idx);
  }
}

void ControlPanel::RemoveMemberIdx(int idx) {
  RemoveMember_impl(idx);
  ReShowEdit(true); //forced
}

void ControlPanel::RemoveMember_impl(int idx) {
  taBase* base_owner = NULL;
  ControlPanelMember* item = mbrs.Leaf(idx);
  
  taBase* mbrown = NULL;
  if(item && item->base) {
    mbrown = item->base->GetMemberOwner(true);
    if(!mbrown)
      mbrown = item->base;       // must be object itself?
  }
  
  if (mbrown) {
    base_owner = mbrown->GetOwner();
  }
  if (item)
    item->Close();
  
  if(base_owner) {
    base_owner->SigEmit(SLS_ITEM_DELETING, mbrown);
  }
}

void ControlPanel::GoToObject(int idx) {
  ControlPanelMember* item = mbrs.Leaf(idx);
  if(item && item->base) {
    taBase* mbrown = item->base->GetMemberOwner(true);
    if(!mbrown) 
      mbrown = item->base;       // must be object itself
    taMisc::Info("Going to:", mbrown->GetPathNames());
    tabMisc::DelayedFunCall_gui(mbrown, "BrowserSelectMe");
  }
}

void ControlPanel::EditLabel(int idx) {
  ControlPanelMember* item = mbrs.Leaf(idx);
  if(item && item->base) {
    String new_label = item->label;
    taGuiDialog dlg;
    dlg.Reset();
    dlg.prompt = "Edit label or use default";
    dlg.win_title = "Edit ControlPanelItem label";
    dlg.AddWidget("main", "", "");
    dlg.AddVBoxLayout("mainv","","main","");
    String curow = "lbl";
    dlg.AddHBoxLayout(curow, "mainv","","");
    dlg.AddLabel("full_lbl_lbl", "main", curow, "label=Label: ;");
    dlg.AddStringField(&new_label, "full_lbl", "main", curow, "tooltip=enter label to use;");

    curow = "default";
    bool use_default_label = false;
    dlg.AddHBoxLayout(curow, "mainv","","");
    dlg.AddLabel("use_default_label", "main", curow, "label=Use Default: ;");
    dlg.AddBoolCheckbox(&use_default_label, "use_default_label", "main", curow,
                        "tooltip=Use the default label generated from the member and owner name;");

    int drval = dlg.PostDialog(true);
    if(drval != 0) {
      String default_label;
      item->base->GetControlPanelLabel(item->mbr, default_label);
      if (use_default_label) {
        item->label = default_label;
        item->cust_label = false;
      }
      else {
        if (new_label != default_label) {
          item->cust_label = true;
        }
        item->label = new_label;
      }
    }
    ReShowEdit(true); //forced
  }
}

void ControlPanel::RemoveMethod(int idx) {
  RemoveMethod_impl(idx);
  ReShowEdit(true); //forced
}

void ControlPanel::RemoveMethod_impl(int idx) {
  ControlPanelMethod* item = mths.Leaf(idx);
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


ParamSet* ControlPanel::CopyToParamSet(ParamSet* param_set) {
  if(!param_set) {
    taProject* proj = GetMyProj();
    if(!proj) return NULL;
    param_set = (ParamSet*)proj->param_sets.NewEl(1);
    param_set->SetName(name + "_" +
                       (String)QDateTime::currentDateTime().toString("MM_dd_yy"));
  }
  param_set->mbrs.Copy_Duplicate(mbrs);  // preserves subgroups
  param_set->CopyActiveToSaved();
  return param_set;
}

bool ControlPanel::AddMember(taBase* base, MemberDef* mbr, const String& xtra_lbl,
                                const String& dscr, const String& sub_gp_nm)
{
  if (!base) return false;
  String eff_desc = dscr; // non-const
  String full_lbl;
  base->GetControlPanelText(mbr, xtra_lbl, full_lbl, eff_desc);
  bool rval = AddMember_impl(base, mbr, full_lbl, eff_desc, sub_gp_nm);
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::AddMemberPrompt(taBase* base, MemberDef* mbr, const String& desc) {
  if (!base) return false;
  
  // for "inline" objects with multiple parameters offer option to add as individual control panel items
  bool show_individual_option = mbr->type->members.size > 0; // if no members we don't show "add members individually"
  bool add_individually = false;  // default is to add inline

  String full_lbl;
  base->GetControlPanelLabel(mbr, full_lbl);
  String full_lbl_copy = full_lbl;
  String sub_grp_name;
  
  taGuiDialog dlg;
  dlg.Reset();
  dlg.prompt = "Enter label for control panel item -- will be converted to a valid C name automatically.\n Leave Subgroup empty to create item in main group";
  dlg.win_title = "Enter ControlPanelItem label & group";
  dlg.AddWidget("main", "", "");
  dlg.AddVBoxLayout("mainv","","main","");
  
  String curow = "lbl";
  dlg.AddHBoxLayout(curow, "mainv","","");
  dlg.AddLabel("full_lbl_lbl", "main", curow, "label=Label: ;");
  dlg.AddStringField(&full_lbl, "full_lbl", "main", curow, "tooltip=enter label to use;");
  
  curow = "grp";
  dlg.AddHBoxLayout(curow, "mainv","","");
  dlg.AddLabel("sub_grp_name_lbl", "main", curow, "label=Subgroup: ;");
  dlg.AddStringField(&sub_grp_name, "sub_grp_name", "main", curow, "tooltip=enter subgroup name or leave blank if item not in a subgroup;");
  
  if (show_individual_option) {
    curow = "add_individual";
    dlg.AddHBoxLayout(curow, "mainv","","");
    dlg.AddLabel("full_lbl_lbl", "main", curow, "label=Add Individually: ;");
    dlg.AddBoolCheckbox(&add_individually, "add_individually", "main", curow, "tooltip=checking will cause each submember to be added as a separate control panel item that will appear on its own line. Doing this allows the item to be used in parameter searches. If checked the label will be used as a prefix for all items;");
  }
  int drval = dlg.PostDialog(true);
  if(drval == 0) {
    return false;
  }
  bool custom_label = false;
  if (full_lbl != full_lbl_copy) {
    custom_label = true;
  }
  
  bool rval = false;
  full_lbl = taMisc::StringCVar(full_lbl);
  if (add_individually) {
    MemberDef* mbr_md = NULL;
    TypeDef* mbr_td = mbr->type;
    taBase* mbr_base = (taBase*)base->FindMembeR(mbr->name, mbr_md);
    TypeDef* mbr_base_td = mbr_base->GetTypeDef();
    for (int i=0; i<mbr_td->members.size; i++) {
      mbr_base->FindMembeR(mbr_base_td->members.SafeEl(i)->name, mbr_md);
      if (mbr_md->isReadOnly() || mbr_md->HasOption("HIDDEN") || mbr_md->HasOption("HIDDEN_INLINE")) {
        continue;
      }
      if (mbr_md->name == "user_data_") {
        continue;
      }
      if (!mbr_md->GetCondOptTest("CONDSHOW", mbr_td, mbr_base)) {
        continue;
      }
      String complete_lbl;
      if(full_lbl.nonempty()) {
        complete_lbl = full_lbl + "_" + mbr_td->members.SafeEl(i)->name;
      }
      else {                    // sometimes you just want the members, if they have distinctive names..
        complete_lbl = mbr_td->members.SafeEl(i)->name;
      }
      rval = AddMember_impl(mbr_base, mbr_md, complete_lbl, desc, sub_grp_name, custom_label);
    }
  }
  else {
    rval = AddMember_impl(base, mbr, full_lbl, desc, sub_grp_name, custom_label);
  }
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::AddMemberNm(taBase* base, const String& md_nm,
  const String& xtra_lbl, const String& dscr, const String& sub_gp_nm)
{
  if(base == NULL) return false;
  MemberDef* md = (MemberDef*)base->FindMemberName(md_nm);
  if (md == NULL) return false;
  return AddMember(base, md, xtra_lbl, dscr, sub_gp_nm);
}

bool ControlPanel::AddMember_impl(taBase* base, MemberDef* md,
            const String& full_lbl, const String& dscr, const String& sub_gp_nm, bool custom_label)
{
  int bidx = -1;
  // this looks at the leaves:
  ControlPanelMember* item = (ControlPanelMember*)ControlPanelItem::StatFindItemBase(&mbrs, base, md, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new ControlPanelMember;
    item->base = base;
    item->mbr = md;
    item->item_nm = md->name;
    item->label = full_lbl;
    item->cust_label = custom_label;
    item->desc = dscr; // even if empty
    if(dscr.nonempty())
      item->cust_desc = true;
    else
      item->cust_desc = false;
    if(sub_gp_nm.nonempty()) {
      ControlPanelMember_Group* egp = (ControlPanelMember_Group*)mbrs.FindMakeGpName(sub_gp_nm);
      egp->Add(item);
    }
    else {
      mbrs.Add(item); // will trigger BaseAdded
    }
    rval = true;
    item->CopyActiveToSaved();
  }
  else if(sub_gp_nm.nonempty()) {
    ControlPanelMember_Group* egp = (ControlPanelMember_Group*)item->owner;
    if(egp == &mbrs || egp->name != sub_gp_nm) {
      ControlPanelMember_Group* negp = (ControlPanelMember_Group*)mbrs.FindMakeGpName(sub_gp_nm);
      negp->Transfer(item);     // grab it
    }
  }
  item->UpdateAfterEdit();
  return rval;
}

bool ControlPanel::AddMethod(taBase* base, MethodDef* md, const String& dscr)
{
  bool rval = AddMethod_impl(base, md, dscr);
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::AddMethodNm(taBase* base, const String& md_nm, const String& dscr)
{
  if(base == NULL) return false;
  MethodDef* md = (MethodDef*)base->GetTypeDef()->methods.FindName(md_nm);
  if (md == NULL) return false;
  return AddMethod(base, md, dscr);
}

bool ControlPanel::AddMethod_impl(taBase* base, MethodDef* mth, const String& dscr)
{
  int bidx = -1;
  // this looks at the leaves:
  ControlPanelMethod* item = (ControlPanelMethod*)ControlPanelItem::StatFindItemBase(&mths, base, mth, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new ControlPanelMethod;
    item->base = base;
    item->mth = mth;
    item->item_nm = mth->name;
    item->desc = dscr;
    if(dscr.nonempty())
      item->cust_desc = true;
    else
      item->cust_desc = false;
    item->label = mth->GetLabel();
    mths.Add(item); // will call BaseAdded
    rval = true;
  }
  item->UpdateAfterEdit();
  return rval;
}


ControlPanelMember* ControlPanel::mbr(int i) const {
  return mbrs.Leaf(i);
}

ControlPanelMethod* ControlPanel::mth(int i) const {
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

String ControlPanel::ToWikiTable() {
  String rval = "{| class=\"wikitable\"\n\
|-\n\
! Parameter !! Value  !! Notes \n";
  
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, sei, mbrs) {
    rval << "|-\n";
    rval << "| " << sei->label << " || " << sei->CurValAsString()
    << " || " << sei->data.notes << "\n";
  }
  
  rval << "|}\n";
  return rval;
}

void  ControlPanel::MbrUpdated(taBase* base, MemberDef* mbr) {
  if (!base || !mbr) return;
  int idx = FindMbrBase(base, mbr);
  if (idx < 0) return;
  
  ControlPanelMember* item = (ControlPanelMember*)mbrs.FastEl(idx);
  if (!item->cust_label) {
    // regenerate label as spec name or program name etc might have changed
    String new_label;
    base->GetControlPanelLabel(item->mbr, new_label);
    item->label = new_label;
  }
}
