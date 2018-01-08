// Co2018ght 2013-2017, Regents of the University of Colorado,
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
#include <DataTable>

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
  cp_state = REGULAR;
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
  cp_state = cp.cp_state;
  updt_while_running = cp.updt_while_running;
  desc = cp.desc;
  base_refs.Reset(); // should get added by copies below
  mbrs = cp.mbrs;
  mths = cp.mths;
}

void ControlPanel::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  RemoveNullItems();

  if (taMisc::is_loading) {
    if (auto_edit) { // obsolete - convert
      SetUserData("user_pinned", true);
      auto_edit = false;
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

void ControlPanel::UpdateAfterMove_impl(taBase* old_owner) {
  inherited::UpdateAfterMove_impl(old_owner);
  RemoveNullItems();
}

void ControlPanel::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ControlPanel);
  if(!taMisc::is_loading) {
    ControlPanel_Group* gp = GET_MY_OWNER(ControlPanel_Group);
    if(!gp) return;
    gp->MasterClonesUpdate();
  }
}

void ControlPanel::RemoveNullItems() {
  for (int i=mbrs.leaves-1; i>=0; i--) {
    ControlPanelMember* item = mbrs.Leaf(i);
    if (item == NULL || item->base == NULL || item->mbr == NULL) {
      taMisc::Info("ControlPanel::RemoveNullItems: removing member with NULL info:",
                   item->label, "in control panel:", name,
                   "(if loading, likely gone in new version, if copying, not found in new project)");
      mbrs.RemoveLeafIdx(i);
    }
  }
  for (int i=mths.leaves-1; i>=0; i--) {
    ControlPanelMethod* item = mths.Leaf(i);
    if (item == NULL || item->base == NULL || item->mth == NULL) {
      taMisc::Info("ControlPanel::RemoveNullItems: removing method with NULL info:",
                   item->label, "in control panel:", name,
                   "(if loading, likely gone in new version, if copying, not found in new project)");
      mths.RemoveLeafIdx(i);
    }
  }
}

int ControlPanel::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int rval = base_refs.UpdatePointers_NewPar(old_par, new_par);
  rval += inherited::UpdatePointers_NewPar(old_par, new_par);
  return rval;
}

void ControlPanel::MasterTriggerUpdate() {
  if(isDestroying() || !owner) return;
  if(!IsMaster()) return;
  ControlPanel_Group* gp = GET_MY_OWNER(ControlPanel_Group);
  if(!gp) return;
  if(gp->isDestroying()) return;
  gp->MasterClonesUpdate();
}


bool ControlPanel::UpdateCloneFromMaster(ControlPanel* master) {
  bool itm_chgs = UpdateCloneFromMaster_mbrlist(&(this->mbrs), &(master->mbrs), this);
  bool gp_chgs = UpdateCloneFromMaster_mbrgps(&(this->mbrs), &(master->mbrs), this);
  return itm_chgs || gp_chgs;
}

bool ControlPanel::UpdateCloneFromMaster_mbrlist
(ControlPanelMember_Group* clone, ControlPanelMember_Group* master, ControlPanel* clone_panel)
{
  bool any_changes = false;
  int i;  int ti;
  ControlPanelMember* cln;
  ControlPanelMember* mst;

  bool explore_new = false;
  if(clone_panel->InheritsFrom(&TA_ParamSet)) {
    explore_new = ((ParamSet*)clone_panel)->last_activated; // set new options to explore only for last activated
  }
  
  // delete not in master; freshen those that are
  for (i = clone->size - 1; i >= 0; --i) {
    cln = clone->FastEl(i);
    mst = (ControlPanelMember*)ControlPanelItem::StatFindItemBase_List(master, cln->base, cln->typeItem(), ti);
    if(mst) {
      cln->cust_label = mst->cust_label;
      cln->short_label = mst->short_label;
      cln->cust_desc = mst->cust_desc;
      cln->label = mst->label;
      cln->desc = mst->desc;
      cln->UpdateAfterEdit_NoGui();
    }
    else {
      clone->RemoveIdx(i);
      any_changes = true;
    }
  }
  // add in master not in us, and put in the right order
  for (ti = 0; ti < master->size; ++ti) {
    mst = master->FastEl(ti);
    cln = (ControlPanelMember*)ControlPanelItem::StatFindItemBase_List(clone, mst->base, mst->typeItem(), i);
    if (i < 0) {
      cln = (ControlPanelMember*)mst->Clone();
      clone->Insert(cln, ti);
      if(explore_new) {
        cln->SetToExplore();
      }
      cln->UpdateAfterEdit_NoGui();
      any_changes = true;
    }
    else if (i != ti) {
      clone->MoveIdx(i, ti);
      any_changes = true;
    }
  }
  return any_changes;
}

bool ControlPanel::UpdateCloneFromMaster_mbrgps
(ControlPanelMember_Group* clone, ControlPanelMember_Group* master, ControlPanel* clone_panel) {
  bool any_changes = false;
  int i;  int ti;
  ControlPanelMember_Group* cln;
  ControlPanelMember_Group* mst;
  // delete not in master; freshen those that are
  for (i = clone->gp.size - 1; i >= 0; --i) {
    cln = (ControlPanelMember_Group*)clone->gp.FastEl(i);
    mst = (ControlPanelMember_Group*)master->gp.FindName(cln->name);
    if(mst) {
      any_changes |= UpdateCloneFromMaster_mbrlist(cln, mst, clone_panel);
    }
    else {
      clone->gp.RemoveIdx(i);
      any_changes = true;
    }
  }
  // add in master not in us, and put in the right order
  for (ti = 0; ti < master->gp.size; ++ti) {
    mst = (ControlPanelMember_Group*)master->gp.FastEl(ti);
    i = clone->gp.FindNameIdx(mst->name);
    if (i < 0) {
      cln = (ControlPanelMember_Group*)mst->Clone();
      cln->name = mst->name;    // not otherwise copied!
      clone->gp.Insert(cln, ti);
      any_changes = true;
    }
    else if (i != ti) {
      clone->gp.MoveIdx(i, ti);
      UpdateCloneFromMaster_mbrlist((ControlPanelMember_Group*)clone->gp.FastEl(i), mst, clone_panel);
      any_changes = true;
    }
  }
  return any_changes;
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
  MasterTriggerUpdate();
}

void ControlPanel::SigEmit_Ref(taBase_RefList* src, taBase* ta,
    int sls, void* op1, void* op2)
{
  if(sls >= SLS_UPDATE_VIEWS) return;
  if(!ta) return;

  if(HasBaseFlag(BF_MISC3))
    return;
  SetBaseFlag(BF_MISC3);        // signal that is updating - our updates can cause updates..
  bool is_prog = ta->InheritsFrom(&TA_Program);
  
  if(!updt_while_running) {
    if((Program::global_run_state == Program::RUN ||
        Program::global_run_state == Program::INIT) && !is_prog) {
      ClearBaseFlag(BF_MISC3);
      return;
    }
  }

  bool on_meths = ControlPanelItem::StatHasBase(&mths, ta);
  bool on_membs = ControlPanelItem::StatHasBase(&mbrs, ta);

  if(on_meths && !on_membs && !is_prog) { // don't update for any non-program method only cases
    ClearBaseFlag(BF_MISC3);
    return;
  }

  bool updated_something = false;

  if(on_membs) {
    FOREACH_ELEM_IN_GROUP_REV(ControlPanelMember, item, mbrs) {
      if(ControlPanelItem::StatCheckBase(item, ta)) {
        bool updt = item->MbrUpdated();
        if(updt) updated_something = true;
      }
    }
  }

  if(updated_something) {
    MasterTriggerUpdate();
  }
  SigEmitUpdated();             // trigger an update of us -- this is expensive!
  ClearBaseFlag(BF_MISC3);
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
  MasterTriggerUpdate();        // should be one-stop update!?
  ReShowEdit(true);
}

void ControlPanel::RemoveMember(taBase* base, MemberDef* md) {
  int idx = FindMbrBase(base, md);
  if(idx >= 0) {
    RemoveMemberIdx(idx);
  }
}

void ControlPanel::RemoveMemberIdx(int idx) {
  if(IsClone() && owner) {
    ControlPanel* master = ((ControlPanel_Group*)owner)->GetMaster();
    if(master) {
      master->RemoveMemberIdx(idx);
      return;
    }
  }

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

void ControlPanel::MoveMemberToCtrlPanelIdx(int idx, ControlPanel* cp) {
  ControlPanelMember* item = mbrs.Leaf(idx);
  if(!item) return;
  cp->mbrs.Transfer(item);
  // ReShowEdit(true); //forced
}

void ControlPanel::MoveMemberToCtrlPanelGpIdx(int idx, ControlPanel_Group* cp) {
  ControlPanelMember* item = mbrs.Leaf(idx);
  if(!item) return;
  ControlPanel* mstr = cp->GetMaster();
  if(TestError(!mstr, "MoveMemberToCtrlPanelGpIdx",
               "selected group to move to does not have a master to move to")) {
    return;
  }
  mstr->mbrs.Transfer(item);
  // ReShowEdit(true); //forced
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
    bool use_default_label = !item->cust_label;
    dlg.AddHBoxLayout(curow, "mainv","","");
    dlg.AddLabel("use_default_label", "main", curow, "label=Use Default: ;");
    dlg.AddBoolCheckbox(&use_default_label, "use_default_label", "main", curow,
                        "tooltip=Use the default label generated from the member and owner name;");

    curow = "short";
    bool use_short_label = item->short_label;
    dlg.AddHBoxLayout(curow, "mainv","","");
    dlg.AddLabel("use_short_label", "main", curow, "label=Use Short: ;");
    dlg.AddBoolCheckbox(&use_short_label, "use_short_label", "main", curow,
                        "tooltip=Use the short label generated from the member and owner name;");

    int drval = dlg.PostDialog(true);
    if(drval != 0) {
      item->short_label = use_short_label;
      String default_label;
      item->base->GetControlPanelLabel(item->mbr, default_label, "", use_short_label);
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
      item->prv_label = item->label;
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
    param_set = (ParamSet*)proj->archived_params.NewArchive();
    param_set->SetName(name + "_" + taDateTime::CurrentDateStampString());
  }
  param_set->mbrs.Copy_Duplicate(mbrs);  // preserves subgroups
  if(!InheritsFrom(&TA_ParamSet)) {
    param_set->CopyActiveToSaved();
  }
  param_set->AllStable();
  return param_set;
}

ParamSet* ControlPanel::Archive(const String& descr) {
  if(TestError(IsArchived(), "Archive", "this is already archived!")) {
    return NULL;
  }
  taProject* proj = GetMyProj();
  if(!proj) return NULL;
  ParamSet* param_set = proj->archived_params.NewArchive();
  String nm = name;
  if(InheritsFrom(&TA_ParamSet) && owner->InheritsFrom(&TA_ParamSet_Group)) {
    if(owner->GetOwner() != proj) { // don't add name of base group
      nm = owner->GetName() + "_" + nm;
    }
  }
  param_set->SetName(nm + "_" + taDateTime::CurrentDateStampString());
  param_set->desc = descr;
  param_set->mbrs.Copy_Duplicate(mbrs);  // preserves subgroups
  if(!InheritsFrom(&TA_ParamSet)) {
    param_set->CopyActiveToSaved();
  }
  param_set->AllLocked();
  return param_set;
}

bool ControlPanel::IsArchived() {
  taBase* agp = GetOwner(&TA_ArchivedParams_Group);
  return (agp != NULL);
}

void ControlPanel::CopyFromDataTable(DataTable* table, int row_num) {
  if(TestError(!table, "CopyFromDataTable",
               "table is NULL")) {
    return;
  }
  if(TestError(table->rows == 0, "CopyFromDataTable",
               "table has no rows")) {
    return;
  }
  if(TestError(table->cols() == 0, "CopyFromDataTable",
               "table has no columns")) {
    return;
  }
  if(row_num < 0) {
    row_num = table->FindVal(name, 0, 0, true); // true = err msg
    if(row_num < 0) return;
  }
  if(table->cols() > 1 && table->data[1]->name.contains_ci("desc")) {
    desc = table->GetValAsString(1, row_num);
  }
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, item, mbrs) {
    Variant val = table->GetVal(item->label, row_num);
    if (!item->short_label && !val.isValid()) {
      String sh_label;
      item->base->GetControlPanelLabel(item->mbr, sh_label, "", true);
      val = table->GetVal(sh_label, row_num);
    }
    if(val.isValid()) {
      if(item->IsParamSet()) {
        item->data.saved_value = val.toString();
      }
      else {
        item->SetCurVal(val);
      }
    }
  }
  SigEmitUpdated();
}

void ControlPanel::CopyToDataTable(DataTable* table, int row_num) {
  if(TestError(!table, "CopyFromDataTable",
               "table is NULL")) {
    return;
  }
  table->StructUpdate(true);
  bool blank_table = false;
  if(table->cols() == 0) {      // new table: initialize name column
    table->NewCol(VT_STRING, "name");
    table->NewCol(VT_STRING, "desc");
    blank_table = true;
  }
  if(row_num < 0) {
    row_num = table->FindVal(name, 0, 0, false); // false = no err msg
    if(row_num < 0) {
      table->AddBlankRow();
      row_num = table->rows - 1;
      table->SetVal(name, 0, row_num); // save name in first col
      if(table->cols() > 1 && table->data[1]->name.contains_ci("desc")) {
        table->SetVal(desc, 1, row_num);
      }
    }
  }
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, item, mbrs) {
    String val;
    if(item->IsParamSet()) {
      val = item->data.saved_value;
    }
    else {
      val = item->CurValAsString();
    }
    DataCol* dc = table->FindColName(item->label, false); // no err
    if(!dc) {
      ValType vt = taBase::ValTypeForType(item->mbr->type);
      if(vt == VT_VARIANT) vt = VT_STRING;
      dc = table->NewCol(vt, item->label);
    }
    dc->SetValAsString(val, row_num);
  }
  table->StructUpdate(false);
}


void ControlPanel::AllStable() {
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, item, mbrs) {
    item->data.SetToStable();   // use data form to prevent updating
  }
  ReShowEdit(true); //forced
}

void ControlPanel::AllLocked() {
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, item, mbrs) {
    item->data.SetToLocked();   // use data form to prevent updating
  }
  ReShowEdit(true); //forced
}

bool ControlPanel::AddMember(taBase* base, MemberDef* mbr, const String& xtra_lbl,
                             const String& dscr, const String& sub_gp_nm, bool short_label)
{
  if (!base) return false;
  bool cust_desc = false;
  String eff_desc;
  if(dscr.nonempty()) {
    eff_desc = dscr;
    cust_desc = true;
  }
  else {
    base->GetControlPanelDesc(mbr, eff_desc);
  }
  bool cust_lbl = false;
  String eff_lbl;
  if(xtra_lbl.nonempty())
    cust_lbl = true;
  base->GetControlPanelLabel(mbr, eff_lbl, xtra_lbl, short_label);
  bool rval = AddMember_impl(base, mbr, eff_lbl, eff_desc, sub_gp_nm, cust_lbl, cust_desc,
                             short_label);
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::AddMemberNm
(taBase* base, const String& md_nm, const String& xtra_lbl, const String& dscr, const String& sub_gp_nm, bool short_name)
{
  if(base == NULL) return false;
  MemberDef* md = (MemberDef*)base->FindMemberName(md_nm);
  if (md == NULL) return false;
  return AddMember(base, md, xtra_lbl, dscr, sub_gp_nm, short_name);
}

bool ControlPanel::AddMemberPrompt(taBase* base, MemberDef* mbr, bool short_label) {
  if (!base) return false;
  
  // for "inline" objects with multiple parameters offer option to add as individual control panel items
  bool show_individual_option = mbr->type->members.size > 0; // if no members we don't show "add members individually"
  bool add_individually = false;  // default is to add inline

  String full_lbl;
  base->GetControlPanelLabel(mbr, full_lbl, "", short_label);
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
    dlg.AddLabel("add_indiv_lbl", "main", curow, "label=Add Individually: ;");
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
  
  String eff_desc;
  base->GetControlPanelDesc(mbr, eff_desc);

  bool rval = false;
  full_lbl = taMisc::StringCVar(full_lbl);
  if (add_individually) {
    MemberDef* mbr_md = NULL;
    TypeDef* mbr_td = mbr->type;
    taBase* mbr_base = (taBase*)base->FindMembeR(mbr->name, mbr_md);
    TypeDef* mbr_base_td = mbr_base->GetTypeDef();
    for (int i=0; i<mbr_td->members.size; i++) {
      mbr_base->FindMembeR(mbr_base_td->members.SafeEl(i)->name, mbr_md);
      if (mbr_md->IsGuiReadOnly() || mbr_md->HasHidden() ||
          mbr_md->HasHiddenInline()) {
        continue;
      }
      if (mbr_md->name == "user_data_") {
        continue;
      }
      if (mbr_md->HasCondShow() && !mbr_md->GetCondOptTest("CONDSHOW", mbr_td, mbr_base)) {
        continue;
      }
      String complete_lbl;
      if(custom_label) {
        if(full_lbl.nonempty()) {
          complete_lbl = full_lbl + "_" + mbr_td->members.SafeEl(i)->name;
        }
        else {                    // sometimes you just want the members, if they have distinctive names..
          complete_lbl = mbr_td->members.SafeEl(i)->name;
        }
      }
      else {
        base->GetControlPanelLabel(mbr_md, complete_lbl, "", short_label);
      }
      String sub_desc;
      mbr_base->GetControlPanelDesc(mbr_md, sub_desc);

      rval = AddMember_impl(mbr_base, mbr_md, complete_lbl, sub_desc, sub_grp_name,
                            custom_label, false, short_label);
    }
  }
  else {
    rval = AddMember_impl(base, mbr, full_lbl, eff_desc, sub_grp_name, custom_label, false,
                          short_label);
  }
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::AddMember_impl
(taBase* base, MemberDef* md, const String& full_lbl, const String& dscr, const String& sub_gp_nm, bool custom_label, bool custom_desc, bool short_label)
{
  int bidx = -1;
  // this looks at the leaves:
  ControlPanelMember* item = (ControlPanelMember*)ControlPanelItem::StatFindItemBase(&mbrs, base, md, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new ControlPanelMember;
    item->base = base;
    item->mbr = md;
    item->SetLabel(full_lbl, custom_label, short_label);
    item->SetDesc(desc, custom_desc);
    item->CopyActiveToSaved();  // do this BEFORE so clones get it!
    if(sub_gp_nm.nonempty()) {
      ControlPanelMember_Group* egp = (ControlPanelMember_Group*)mbrs.FindMakeGpName(sub_gp_nm);
      egp->Add(item);
    }
    else {
      mbrs.Add(item); // will trigger BaseAdded
    }
    rval = true;
  }
  else if(sub_gp_nm.nonempty()) {
    ControlPanelMember_Group* egp = (ControlPanelMember_Group*)item->owner;
    if(egp == &mbrs || egp->name != sub_gp_nm) {
      ControlPanelMember_Group* negp = (ControlPanelMember_Group*)mbrs.FindMakeGpName(sub_gp_nm);
      negp->Transfer(item);     // grab it
    }
  }
  item->data.SetCtrlType();
  if(InheritsFromName("ClusterRun")) {
    item->SetToExplore();
  }
  item->UpdateAfterEdit();
  return rval;
}

bool ControlPanel::AddMethod(taBase* base, MethodDef* md, const String& xtra_lbl, const String& sub_gp_nm)
{
  String lbl;
  bool cust_lbl = false;
  if(xtra_lbl.nonempty()) {
    lbl = xtra_lbl;
    if(!lbl.endsWith("_"))
      lbl += "_";
    lbl += md->GetLabel();
    cust_lbl = true;
  }
  else {
    lbl = md->GetLabel();
  }
  bool rval = AddMethod_impl(base, md, lbl, md->desc, sub_gp_nm, cust_lbl, false);
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::AddMethodNm(taBase* base, const String& md_nm, const String& xtra_lbl, const String& dsc, const String& sub_gp_nm)
{
  if(base == NULL) return false;
  MethodDef* md = (MethodDef*)base->GetTypeDef()->methods.FindName(md_nm);
  if (md == NULL) return false;
  String lbl;
  bool cust_lbl = false;
  if(xtra_lbl.nonempty()) {
    lbl = xtra_lbl;
    if(!lbl.endsWith("_"))
      lbl += "_";
    lbl += md->GetLabel();
    cust_lbl = true;
  }
  else {
    lbl = md->GetLabel();
  }
  bool cust_desc = false;
  String desc_eff;
  if(dsc.nonempty()) {
    cust_desc = true;
    desc_eff = dsc;
  }
  else {
    desc_eff = md->desc;
  }
  bool rval = AddMethod_impl(base, md, lbl, desc_eff, sub_gp_nm, cust_lbl, cust_desc);
  ReShowEdit(true); //forced
  return rval;
}

bool ControlPanel::AddMethod_impl(taBase* base, MethodDef* mth, const String& lbl, const String& dsc, const String& sub_gp_nm, bool custom_label, bool custom_desc)
{
  int bidx = -1;
  // this looks at the leaves:
  ControlPanelMethod* item = (ControlPanelMethod*)ControlPanelItem::StatFindItemBase(&mths, base, mth, bidx);
  bool rval = false;
  if (bidx < 0) {
    item = new ControlPanelMethod;
    item->base = base;
    item->mth = mth;
    item->SetLabel(lbl, custom_label);
    item->SetDesc(dsc, custom_desc);
    if(sub_gp_nm.nonempty()) {
      ControlPanelMethod_Group* egp = (ControlPanelMethod_Group*)mths.FindMakeGpName(sub_gp_nm);
      egp->Add(item);
    }
    else {
      mths.Add(item); // will trigger BaseAdded
    }
    rval = true;
  }
  else if(sub_gp_nm.nonempty()) {
    ControlPanelMethod_Group* egp = (ControlPanelMethod_Group*)item->owner;
    if(egp == &mths || egp->name != sub_gp_nm) {
      ControlPanelMethod_Group* negp = (ControlPanelMethod_Group*)mths.FindMakeGpName(sub_gp_nm);
      negp->Transfer(item);     // grab it
    }
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

int ControlPanel::FindMbrBase(const taBase* base, MemberDef* md) {
  int rval = -1;
  ControlPanelItem::StatFindItemBase(&mbrs, base, md, rval);
  return rval;
}

int ControlPanel::FindMethBase(const taBase* base, MethodDef* md) {
  int rval = -1;
  ControlPanelItem::StatFindItemBase(&mths, base, md, rval);
  return rval;
}

String ControlPanel::ToTable(taMarkUp::Format fmt) {
  String rval = "\n";
  int n_cols = 0;
  rval << taMarkUp::TableStart(fmt) << taMarkUp::TableHeaderStart(fmt, n_cols);
  rval << taMarkUp::TableHeader(fmt, "Parameter", n_cols)
       << taMarkUp::TableHeader(fmt, "Value", n_cols)
       << taMarkUp::TableHeader(fmt, "Notes", n_cols)
       << taMarkUp::TableHeaderEnd(fmt, n_cols);
  
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, sei, mbrs) {
    int col_no = 0;
    rval << taMarkUp::TableRowStart(fmt, col_no)
         << taMarkUp::TableCell(fmt, taMarkUp::Code(fmt, sei->label), col_no);
    if(sei->IsParamSet())
      rval << taMarkUp::TableCell(fmt, sei->data.saved_value, col_no);
    else
      rval << taMarkUp::TableCell(fmt, sei->CurValAsString(), col_no);
    rval << taMarkUp::TableCell(fmt, sei->data.notes, col_no)
         << taMarkUp::TableRowEnd(fmt, col_no);
  }
  rval << taMarkUp::TableEnd(fmt);
  return rval;
}

DataTable* ControlPanel::ToDataTable(DataTable* dt) {
  bool new_table = false;
  if(!dt) {
    taProject* proj = GetMyProj();
    if(!proj) return NULL;
    dt = proj->GetNewAnalysisDataTable(name, true);
    new_table = true;
  }
  dt->StructUpdate(true);
  int idx;
  dt->RemoveAllRows();
  DataCol* col;
  col = dt->FindMakeColName("parameter", idx, VT_STRING);
  col->desc = "name of parameter";
  col = dt->FindMakeColName("value", idx, VT_STRING);
  col->desc = "text representation of value";
  col = dt->FindMakeColName("notes", idx, VT_STRING);
  col->desc = "notes";

  FOREACH_ELEM_IN_GROUP(ControlPanelMember, sei, mbrs) {
    dt->AddBlankRow();
    dt->SetVal(sei->label, "parameter", -1);
    if(sei->IsParamSet())
      dt->SetVal(sei->data.saved_value, "value", -1);
    else
      dt->SetVal(sei->CurValAsString(), "value", -1);
    dt->SetVal(sei->data.notes, "notes", -1);
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

String ControlPanel::GetRecursiveVariableValue(String var_name, bool use_search_vals) {
  String value = "";
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
    if(!mbr->base) continue;
    ControlPanel* sub_panel= mbr->GetControlPanelPointer();
    if(sub_panel) {
      String ovalue = sub_panel->GetRecursiveVariableValue(var_name, use_search_vals);
      if (!value.empty() && !ovalue.empty()) {
        taMisc::Warning("Variable ", var_name, " doesn't seem to be unique when expanding paramsets for ", this->name);
      }
      if (!ovalue.empty()) value = ovalue;
    }
    else {
      if (var_name == mbr->GetName()) {
        String variable_value;
        const ControlPanelMemberData &ps = mbr->data;
        
        //If we are in a search algorithm, then we need to use the value
        //set in the search parameters
        if (!use_search_vals || !mbr->data.is_numeric || !ps.IsSearch()) {
          variable_value = mbr->CurValAsString();
        }
        else {
          variable_value = String(ps.next_val);
        }
        if (!value.empty()) {
          taMisc::Warning("Variable ", var_name, " doesn't seem to be unique when expanding paramsets for ", this->name);
        }
        value = variable_value;
      }
    }
  }
  return value;
}
String ControlPanel::ExploreMembersToString(bool use_search_vals) {
  String params;
  bool first = true;
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
    if(!mbr->base) continue;
    ControlPanel* sub_panel= mbr->GetControlPanelPointer();
    if(sub_panel) {
      if(!first)
        params.cat(" "); // sep
      else
        first = false;
      String oparams = sub_panel->ExploreMembersToString(use_search_vals);
      params.cat(oparams);
    }
    else {
      if(!mbr->RecordValue()) continue; // only active values!
      if(!first)
        params.cat(" "); // sep
      else
        first = false;
      params.cat(mbr->label).cat("=");
      params.cat(mbr->RecordValueString(use_search_vals));
    }
  }
  return params;
}

bool ControlPanel::SearchMembersToList(ControlPanelMember_Group * m_params, ControlPanelMember_Group * m_yoked, int_PArray * m_counts) {
  
  bool has_yoked = false;
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
    ControlPanel* sub_panel= mbr->GetControlPanelPointer();
    if (sub_panel) {
      if (!sub_panel->SearchMembersToList(m_params, m_yoked, m_counts)) {
        return false;
      }
    } else {
      ControlPanelMemberData &ps = mbr->data;
      if (ps.IsSearch()) {
        bool ok = ps.ParseRange();          // just to be sure
        if(ok) {
          if(ps.range.startsWith('%')) {
            has_yoked = true;
            continue; // skip for now
          }
          m_params->Link(mbr);     // link does not transfer ownership to this tmp list!
          m_counts->Add(ps.srch_vals.size);
        }
      }
    }
  }
  if(has_yoked) {
    FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
      ControlPanel* sub_panel= mbr->GetControlPanelPointer();
      if (sub_panel) {
        continue; //Already handled in the above loop
      } else {
        ControlPanelMemberData &ps = mbr->data;
        if (ps.IsSearch()) {
          if(!ps.range.startsWith('%')) continue; // only yoked
          String prnm = ps.range.after('%');
          // ControlPanelMember* src = m_params.FindName(prnm); // FindName only works for owned
          int idx = -1;
          for(int i=0;i<m_params->size; i++) {
            ControlPanelMember* it = (*m_params)[i];
            if(it->label == prnm) {
              idx = i;
              break;
            }
          }
          if(TestError(idx < 0, "StartSearch",
                       "search parameter:", mbr->label,
                       "copying from other parameter:", prnm,
                       "which was not found -- aborting search!")) {
            return false;
          }
          m_yoked->Link(mbr);
        }
      }
    }
  }
  return true;
}

void ControlPanel::SaveNameValueMembers_impl(ParamSet* param_set, String_Array& name_vals) {
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
    if(!mbr->base) continue;
    ControlPanel* sub_panel= mbr->GetControlPanelPointer();
    if(sub_panel) {
      sub_panel->SaveNameValueMembers_impl(param_set, name_vals);
    }
    else {
      int idx = name_vals.FindStartsWith(mbr->label + "=");
      if(idx >= 0) {
        String str = name_vals[idx];
        String val = str.after("=");
        ControlPanelMember* itm = (ControlPanelMember*)mbr->Clone();
        param_set->mbrs.Add(itm);
        itm->data.saved_value = val;
        itm->SetToLocked();
        itm->SetCtrlType();
        name_vals.RemoveIdx(idx);
      }        
    }
  }
}

void ControlPanel::SaveNameValueMembers(ParamSet* param_set, const String& name_vals_str) {
  String_Array name_vals;
  name_vals.Split(name_vals_str, " ");    // space sep
  SaveNameValueMembers_impl(param_set, name_vals);
  for(int i=0; i<name_vals.size; i++) { // deal with remainders
    String str = name_vals[i];
    if(str.contains("=")) {
      String nm = str.before("=");
      String val = str.after("=");
      if(nm.nonempty() && val.nonempty()) {
        param_set->NewDummyMember(nm, val);
      }
    }
  }
}

void ControlPanel::ActivateAll(bool info_msg) {
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
    if(!mbr->base) continue;
    ControlPanel* sub_panel= mbr->GetControlPanelPointer();
    if(sub_panel) {
      sub_panel->ActivateAll(info_msg);
    }
  }
  if(this->InheritsFrom(&TA_ParamSet)) {
    ((ParamSet*)this)->Activate(info_msg);
  }
}


String ControlPanel::ParamSetNames(bool recursive, bool explore_only) {
  String rval;
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
    if(!mbr->base) continue;
    if(explore_only && !mbr->IsControl() && !mbr->IsExplore())
      continue;
    ControlPanel* sub_panel= mbr->GetControlPanelPointer();
    if(!sub_panel) continue;
    if(rval.empty()) {
      rval = sub_panel->name;
    }
    else {
      rval += String("_") + sub_panel->name;
    }
  }
  if(recursive) {
    FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
      if(!mbr->base) continue;
      if(explore_only && !mbr->IsControl() && !mbr->IsExplore())
        continue;
      ControlPanel* sub_panel= mbr->GetControlPanelPointer();
      if(sub_panel) {
        rval += String("_") + sub_panel->ParamSetNames(recursive);
      }
    }
  }
  return rval;
}

void ControlPanel::AddMembersAsArgs(bool active_only, bool follow_control_panel_links) {
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
    if(!mbr->base) continue;
    ControlPanel* sub_panel= mbr->GetControlPanelPointer();
    if(sub_panel) {
      if(follow_control_panel_links) {
        sub_panel->AddMembersAsArgs(active_only, follow_control_panel_links);
      }
    }
    else {
      if(!mbr->data.is_single) continue;
      if(active_only && !mbr->IsExplore()) continue;
      taMisc::AddEqualsArgName(mbr->label);
      taMisc::AddArgNameDesc(mbr->label, "Control Panel Member: " + name);
    }
  }
}

void ControlPanel::SetMembersFromArgs(bool active_only, bool follow_control_panel_links) {
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
    if(!mbr->base) continue;
    ControlPanel* sub_panel= mbr->GetControlPanelPointer();
    if(sub_panel) {
      if(follow_control_panel_links) {
        sub_panel->SetMembersFromArgs(active_only, follow_control_panel_links);
      }
    }
    else {
      if(!mbr->data.is_single) continue;
      if(active_only && !mbr->IsExplore()) continue;
      String argval = taMisc::FindArgByName(mbr->label);
      if(argval.empty()) continue;
      mbr->SetCurValFmString(argval, true, true); // warn no match, info msg
    }
  }
}

void ControlPanel::MbrUpdated(taBase* base, MemberDef* mbr) {
  if (!base || !mbr) return;
  int idx = FindMbrBase(base, mbr);
  if (idx < 0) return;
  
  ControlPanelMember* item = (ControlPanelMember*)mbrs.FastEl(idx);
  bool updt = item->MbrUpdated();
  if(updt) {
    item->SigEmitUpdated();
  }
}

int ControlPanel::GetEnabled() const {
  return 1;
}

int ControlPanel::GetSpecialState() const {
  if(IsClone()) return 2;
  return inherited::GetSpecialState();
}
