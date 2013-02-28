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

#include "iPanelOfDataTable.h"
#include <iDataTableEditor>
#include <iMatrixEditor>
#include <iClipData>
#include <iDataTableView>
#include <iMainWindowViewer>
#include <taSigLinkItr>
#include <iMatrixTableView>

#include <taMisc>


iPanelOfDataTable::iPanelOfDataTable(taiSigLink* dl_)
:inherited(dl_)
{
  dte = NULL;
  /* TODO: add view button(s) when we add Flat mode, and/or image or block editing
  QToolButton* but = new QToolButton;
  but->setMaximumHeight(taiM->button_height(taiMisc::sizSmall));
  but->setFont(taiM->buttonFont(taiMisc::sizSmall));
  but->setText("View...");
  AddMinibarWidget(but);
  connect(but, SIGNAL(clicked()), this, SLOT(mb_View()) );*/
}

iPanelOfDataTable::~iPanelOfDataTable() {
}

QWidget* iPanelOfDataTable::firstTabFocusWidget() {
  if(!dte) return NULL;
  return dte->tvTable;
}

void iPanelOfDataTable::SigEmit_impl(int sls, void* op1_, void* op2_) {
  if(!isVisible() || !dte || !dte->isVisible()) return; // no update when hidden!
  inherited::SigEmit_impl(sls, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
}

int iPanelOfDataTable::EditAction(int ea) {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->EditAction_(sel_list, ea);
  }
  return rval;
}

int iPanelOfDataTable::GetEditActions() {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->QueryEditActions_(sel_list);
    // certain things disallowed if more than one item selected
    if (sel_list.size > 1) {
      rval &= ~(iClipData::EA_FORB_ON_MUL_SEL);
    }
  }
  return rval;
}

void iPanelOfDataTable::GetSelectedItems(ISelectable_PtrList& lst) {
//note: not applicable
}

void iPanelOfDataTable::GetWinState_impl() {
  inherited::GetWinState_impl();
  DataTable* dt = this->dt(); // cache
  if (!dt || !dte) return;
  QTableView* tv = dte->tvTable; // cache -- note: row# header size is separate
  // we store col widths as fraction of ctrl width
  float fwd = (float)tv->width();
  if (fwd <= 0.0f) return; // huh???
  for (int i = 0; i < dt->data.size; ++i) {
    DataCol* dc = dt->data.FastEl(i);
    int iwd = tv->columnWidth(i);
    if (iwd > 0) {
      float fcolwd = iwd / fwd;
      dc->SetUserData("view_panel_wd", fcolwd);
    }
  }
}

void iPanelOfDataTable::SetWinState_impl() {
  inherited::SetWinState_impl();
  DataTable* dt = this->dt(); // cache
  if (!dt || !dte) return;
  QTableView* tv = dte->tvTable; // cache -- note: row# header size is separate
  // we store col widths as fraction of ctrl width
  float fwd = (float)tv->width();
  if (fwd <= 0.0f) return; // huh???
  for (int i = 0; i < dt->data.size; ++i) {
    DataCol* dc = dt->data.FastEl(i);
    float fcolwd = dc->GetUserDataAsFloat("view_panel_wd");
    int iwd = (int)(fwd * fcolwd);
    if (iwd > 0) { // ==0 typically if not set in UD
      tv->setColumnWidth(i, iwd);
    }
  }
}

String iPanelOfDataTable::panel_type() const {
  static String str("Data Table");
  return str;
}

void iPanelOfDataTable::UpdatePanel_impl() {
  if(dte) {
    // bracket with gui_edit_op to prevent scroll to bottom on show events
    if(isShowUpdating())   dte->tvTable->gui_edit_op = true;
    dte->Refresh();
    if(isShowUpdating())   dte->tvTable->gui_edit_op = false;
  }
  inherited::UpdatePanel_impl();
}

void iPanelOfDataTable::Render_impl() {
  dte = new iDataTableEditor();
  setCentralWidget(dte);

  dte->setDataTable(dt());
  connect(dte->tvTable, SIGNAL(hasFocus(iTableView*)),
          this, SLOT(tv_hasFocus(iTableView*)) );
  connect(dte->tvCell->tv, SIGNAL(hasFocus(iTableView*)),
          this, SLOT(tv_hasFocus(iTableView*)) );

  iMainWindowViewer* vw = viewerWindow();
  if (vw) {
    dte->installEventFilter(vw);
    dte->tvTable->m_window = vw;
    dte->tvCell->tv->m_window = vw;
  }
}

void iPanelOfDataTable::tv_hasFocus(iTableView* sender) {
  iMainWindowViewer* vw = viewerWindow();
  if (vw) {
    vw->SetClipboardHandler(sender,
                            SLOT(GetEditActionsEnabled(int&)),
                            SLOT(EditAction(int)),
                            NULL,
                            SIGNAL(UpdateUi()) );
  }
}


void DataTable::ScrollEditorsToBottom() {
  if(!taMisc::gui_active) return;
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
    iPanelOfDataTable* el;
    FOR_DLC_EL_OF_TYPE(iPanelOfDataTable, el, dl, itr) {
      if(el->dte)
        el->dte->ScrollToBottom();
    }
  }
}
