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

#include "iClusterTableView.h"
#include <taiWidgetMenu>
#include <CellRange>
#include <DataTable>
#include <taiSigLink>
#include <taMisc>
#include <taRootBase>
#include <ClusterRun>

iClusterTableView::iClusterTableView(QWidget* parent)
:inherited(parent)
{
}

void iClusterTableView::FillContextMenu_impl(ContextArea ca,
                                             taiWidgetMenu* menu, const CellRange& sel)
{
  inherited::FillContextMenu_impl(ca, menu, sel);
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab) return;
  // only do col items if one selected only
  if ((ca == CA_COL_HDR) && (sel.width() == 1)) {
    DataCol* col = tab->GetColData(sel.col_fr, true);
    if (col) {
      taiSigLink* link = (taiSigLink*)col->GetSigLink();
      if (link) link->FillContextMenu(menu);
    }
  }
  if (ca == CA_ROW_HDR) {
    iAction* act = NULL;
    menu->AddSep();
    act = menu->AddItem("List Job Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "ListJobFiles");
    act->setEnabled((tab->name == "jobs_running" || tab->name == "jobs_done" || tab->name == "jobs_archive") && sel.height() > 0);

    act = menu->AddItem("List Local Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "ListLocalFiles");
    act->setEnabled((tab->name == "jobs_running" || tab->name == "jobs_done" || tab->name == "jobs_archive") && sel.height() >= 0);
    
    menu->AddSep();
    act = menu->AddItem("Get Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "GetFiles");
    act->setEnabled((tab->name == "file_list") && sel.height() > 0);
    
    menu->AddSep();
    act = menu->AddItem("Update Notes", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "UpdtNotes");
    act->setEnabled((tab->name == "jobs_done") && sel.height() > 0);

    act = menu->AddItem("Load Data", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "LoadData");
    act->setEnabled((tab->name == "jobs_running" || tab->name == "jobs_done" || tab->name == "file_list") && sel.height() > 0);

    act = menu->AddItem("Save Job Params", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "SaveJobParams");
    act->setEnabled((tab->name == "jobs_done" || tab->name == "jobs_archive") && sel.height() > 0);

    act = menu->AddItem("Archive Jobs", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "ArchiveJobs");
    act->setEnabled((tab->name == "jobs_done") && sel.height() > 0);

    act = menu->AddItem("Kill Job", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "Kill");
    act->setEnabled((tab->name == "jobs_running") && sel.height() > 0);  //

    act = menu->AddItem("Remove Jobs", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "RemoveJobs");
    act->setEnabled((tab->name == "jobs_done" || tab->name == "jobs_archive") && sel.height() > 0);
    
    act = menu->AddItem("Remove Killed Jobs", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "RemoveKilledJobs");
    act->setEnabled((tab->name == "jobs_done") && sel.height() >= 0);
    
    act = menu->AddItem("Clean Job Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "CleanJobFiles");
    act->setEnabled((tab->name == "jobs_done" || tab->name == "jobs_archive") && sel.height() > 0);
    
    act = menu->AddItem("Remove Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "RemoveFiles");
    act->setEnabled((tab->name == "file_list" || tab->name == "jobs_archive" || tab->name == "jobs_done" ) && sel.height() > 0);
    
    act = menu->AddItem("Remove Non Data Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "RemoveNonDataFiles");
    act->setEnabled((tab->name == "jobs_done" || tab->name == "jobs_archive") && sel.height() > 0);

    menu->AddSep();
    act = menu->AddItem("Get Project at Rev", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "GetProjectAtRev");
    act->setEnabled((tab->name == "jobs_running" || tab->name == "jobs_done" || tab->name == "jobs_archive") && sel.height() == 0);
  }
}

void iClusterTableView::DoClusterOp(const Variant& var) {
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab) return;
  taProject* proj = GET_OWNER(tab, taProject);
  if(!proj) return;
  ClusterRun* cr = (ClusterRun*)proj->FindMakeControlPanel("ClusterRun", &TA_ClusterRun);
  if (cr) {
    cr->DoClusterOp(var.toString());
  }
}
