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

#include <ControlPanel>
#include "ControlPanel_Group.h"
#include <DataTable>
#include <taProject>
#include <ParamSet_Group>
#include <SigLinkSignal>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ControlPanel_Group);

void ControlPanel_Group::Initialize() {
  master_and_clones = false;
}

void ControlPanel_Group::InitLinks() {
  inherited::InitLinks();
  if(!taMisc::is_loading) {
    if(!owner) return;
    taProject* proj = GET_MY_OWNER(taProject);
    if(!proj) return;
    if(InheritsFrom(&TA_ParamSet_Group) && leaves == 0 && owner != &proj->archived_params) {
      // default for new groups outside of main project ones is master and clones -- make a new master by default
      master_and_clones = true;
      ControlPanel* pan = (ControlPanel*)NewEl(1);                 // our new master
      pan->name = "NewMaster";
      MasterClonesUpdate();
    }
  }
}

void ControlPanel_Group::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!taMisc::is_loading) {
    MasterClonesUpdate();
  }
}

void ControlPanel_Group::SigEmit(int sls, void* op1, void* op2) {
  inherited::SigEmit(sls, op1, op2);
  if(sls >= SLS_LIST_ORDER_MIN && sls <= SLS_LIST_ORDER_MAX) { // order was changed
    MasterClonesUpdate();
  }
}

void ControlPanel_Group::MasterClonesUpdate() {
  if(isDestroying()) return;
  ControlPanel* master = NULL;
  if(master_and_clones) {
    if(TestWarning(size == 0, "MasterClonesUpdate",
                   "no items in top-level of the group -- master must be first top-level item -- turning off master_and_clones")) {
      master_and_clones = false;
    }
    else {
      master = FastEl(0);
      master->cp_state = ControlPanel::MASTER;
    }
  }
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    if(cp == master) continue;
    if(master_and_clones && master) {
      cp->cp_state = ControlPanel::CLONE;
      cp->UpdateCloneFromMaster(master);
    }
    else {
      cp->cp_state = ControlPanel::REGULAR;
    }
  }
}

void ControlPanel_Group::SetMasterAndClones(bool use_master_and_clones) {
  master_and_clones = use_master_and_clones;
  MasterClonesUpdate();
}

void ControlPanel_Group::AllStable() {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    cp->AllStable();
  }
}

void ControlPanel_Group::AllLocked() {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    cp->AllLocked();
  }
}

ControlPanel* ControlPanel_Group::GetMaster() {
  if(leaves == 0) return NULL;
  return Leaf(0);
}

void ControlPanel_Group::AddMember
(taBase* base, MemberDef* mbr, const String& xtra_lbl, const String& dscr, const String& sub_gp_nm, bool short_label) {
  if(master_and_clones) {
    ControlPanel* master = GetMaster();
    if(master) {
      master->AddMember(base, mbr, xtra_lbl, dscr, sub_gp_nm, short_label);
    }
  }
  else {
    FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
      cp->AddMember(base, mbr, xtra_lbl, dscr, sub_gp_nm, short_label);
    }
  }
}

void ControlPanel_Group::RemoveMember(taBase* base, MemberDef* mbr) {
  if(master_and_clones) {
    ControlPanel* master = GetMaster();
    if(master) {
      master->RemoveMember(base, mbr);
    }
  }
  else {
    FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
      cp->RemoveMember(base, mbr);
    }
  }
}

void ControlPanel_Group::RestorePanels() {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    if(cp->GetUserDataAsBool("user_pinned")) {
      cp->EditPanel(true, true); // true,true = new tab, pinned in place
    }
  }
}

void ControlPanel_Group::CopyFromDataTable(DataTable* table) {
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
  if(TestError(leaves == 0, "CopyFromDataTable",
               "group has no existing control panels -- must have at least one -- first one is used as a template for any new ones that are needed")) {
    return;
  }
  ControlPanel* templ = Leaf(0);
  for(int i=0; i<table->rows; i++) {
    String nm = table->GetValAsString(0, i);
    ControlPanel* cp = FindLeafName(nm);
    if(cp) {
      cp->CopyFromDataTable(table, i);
    }
    else {
      cp = (ControlPanel*)templ->Clone();
      cp->name = nm;
      Add(cp);
      cp->CopyFromDataTable(table, i);
    }
  }
}

void ControlPanel_Group::CopyToDataTable(DataTable* table) {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    cp->CopyToDataTable(table);
  }
}
