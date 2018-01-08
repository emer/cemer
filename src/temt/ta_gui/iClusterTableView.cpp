// Co2018ght 2015-2017, Regents of the University of Colorado,
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

    act = menu->AddItem("Kill Job", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "Kill");
    act->setEnabled((tab->name == "running") && sel.height() > 0);  //

    act = menu->AddItem("Load Data", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "LoadData");
    act->setEnabled((tab->name == "running" || tab->name == "done" || tab->name == "archive" || tab->name == "files") && sel.height() > 0);

    menu->AddSep();
    
    act = menu->AddItem("Delete Jobs", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "DeleteJobs");
    act->setEnabled((tab->name == "done" || tab->name == "archive" || tab->name == "deleted") && sel.height() > 0);
    
    act = menu->AddItem("Nuke Jobs", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "NukeJobs");
    act->setEnabled((tab->name == "done" || tab->name == "archive" || tab->name == "deleted") && sel.height() > 0);
    
    act = menu->AddItem("Delete Killed Jobs", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "DeleteKilledJobs");
    act->setEnabled((tab->name == "done") && sel.height() >= 0);
    
    act = menu->AddItem("UnDelete Jobs", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "UnDeleteJobs");
    act->setEnabled((tab->name == "deleted") && sel.height() > 0);

    act = menu->AddItem("Archive Jobs", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "ArchiveJobs");
    act->setEnabled((tab->name == "done") && sel.height() > 0);

    menu->AddSep();
    
    act = menu->AddItem("Save Job Params", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "SaveJobParams");
    act->setEnabled((tab->name == "done" || tab->name == "archive") && sel.height() > 0);

    act = menu->AddItem("Update Notes", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "UpdtNotes");
    act->setEnabled((tab->name == "running" || tab->name == "done" || tab->name == "archive") && sel.height() >= 0);

    act = menu->AddItem("Save State", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "SaveState");
    act->setEnabled((tab->name == "running") && sel.height() > 0);

    menu->AddSep();

    act = menu->AddItem("List Job Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "ListJobFiles");
    act->setEnabled((tab->name == "running" || tab->name == "done" || tab->name == "archive") && sel.height() > 0);

    act = menu->AddItem("List Local Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "ListLocalFiles");
    act->setEnabled((tab->name == "running" || tab->name == "done" || tab->name == "archive") && sel.height() >= 0);
    
    act = menu->AddItem("Get Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "GetFiles");
    act->setEnabled((tab->name == "files") && sel.height() > 0);
    
    menu->AddSep();
    
    act = menu->AddItem("Clean Job Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "CleanJobFiles");
    act->setEnabled((tab->name == "done" || tab->name == "archive") && sel.height() > 0);
    
    act = menu->AddItem("Delete Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "DeleteFiles");
    act->setEnabled((tab->name == "files" || tab->name == "archive" || tab->name == "done" ) && sel.height() > 0);
    
    act = menu->AddItem("Delete Non Data Files", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "DeleteNonDataFiles");
    act->setEnabled((tab->name == "done" || tab->name == "archive") && sel.height() > 0);

    menu->AddSep();
    
    act = menu->AddItem("Get Proj at Rev", taiWidgetMenu::normal, iAction::var_act, this, SLOT(DoClusterOp(const Variant&)), "GetProjAtRev");
    act->setEnabled((tab->name == "running" || tab->name == "done" || tab->name == "archive") && sel.height() > 0);

    menu->AddSep();
    
    act = menu->AddItem("Compare Selected Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_COMPARE));
    act->setEnabled(sel.height() > 1);  // enable if compare state is on
    
    act = menu->AddItem("Clear Compare Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_CLEAR_COMPARE));
    act->setEnabled(tab->CompareRowsState());  // enable if compare state is on
}
}

void iClusterTableView::DoClusterOp(const Variant& var) {
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab) return;
  taProject* proj = tab->GetMyProj();
  if(!proj) return;
  ClusterRun* cr = (ClusterRun*)proj->FindMakeControlPanel("ClusterRun", &TA_ClusterRun);
  if (cr) {
    cr->DoClusterOp(var.toString());
  }
}
