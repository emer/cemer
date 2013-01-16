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

#include "iProgramPanelBase.h"
#include <iProgramEditor>
#include <iTreeView>
#include <iMainWindowViewer>
#include <iDataPanelSet>

#include <DataChangedReason>
#include <taMisc>
#include <taiMisc>

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>


iProgramPanelBase::iProgramPanelBase(taiDataLink* dl_)
:inherited(dl_)
{
  pe = new iProgramEditor();
  setCentralWidget(pe); //sets parent

  // add view button(s)
  QLabel* lab = new QLabel;
  lab->setMaximumHeight(taiM->label_height(taiMisc::sizSmall));
  lab->setFont(taiM->nameFont(taiMisc::sizSmall));
  lab->setText("lines");
  lab->setToolTip("how many lines to show in the item editor");
  AddMinibarWidget(lab);
  QSpinBox* sp = new QSpinBox;
  sp->setFont(taiM->buttonFont(taiMisc::sizSmall));
  sp->setValue(pe->editLines()); // assume this is min as well
  sp->setMinimum(pe->editLines()); // assume this is min as well
  sp->setMaximum(20);// arbitrary
  sp->setToolTip(lab->toolTip());
  AddMinibarWidget(sp);
  connect(sp, SIGNAL(valueChanged(int)), this, SLOT(mb_Lines(int)) );

  // add view button(s)
  QCheckBox* but = new QCheckBox;
  but->setMaximumHeight(taiM->button_height(taiMisc::sizSmall));
  but->setFont(taiM->buttonFont(taiMisc::sizSmall));
  but->setText("expert");
  but->setToolTip("whether to show items marked 'expert' in the program editor");
  but->setChecked(!(taMisc::show_gui & TypeItem::NO_EXPERT));
  AddMinibarWidget(but);
  connect(but, SIGNAL(clicked(bool)), this, SLOT(mb_Expert(bool)) );
}

void iProgramPanelBase::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  if (dcr == DCR_RESOLVE_NOW)
  {
    if (pe->HasChanged()) {
      pe->GetValue();
      return;
    }
  }
  else
    inherited::DataChanged_impl(dcr, op1_, op2_);
    //NOTE: don't need to do anything because DataModel will handle it
}

bool iProgramPanelBase::HasChanged_impl() {
  return pe->HasChanged();
}

void iProgramPanelBase::mb_Expert(bool checked) {
  int show = pe->show();
  if (checked) {
    show = show & ~TypeItem::NO_EXPERT;
  } else {
    show = show | TypeItem::NO_EXPERT;
  }
  pe->setShow(show);
}

void iProgramPanelBase::mb_Lines(int val) {
  pe->setEditLines(val);
}

void iProgramPanelBase::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // connect the tree up to the panel
  pe->items->Connect_SelectableHostNotifySignal(itv,
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
  pe->m_window = itv->viewerWindow();
  pe->items->main_window = pe->m_window;

  pe->items->installEventFilter(pe->m_window);

  // make sure the Program toolbar is created
  MainWindowViewer* mvw = itv->viewerWindow()->viewer();
  mvw->FindToolBarByType(&TA_ProgramToolBar,"Program");
}

void iProgramPanelBase::UpdatePanel_impl() {
  pe->Refresh();
}

void iProgramPanelBase::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (pe->HasChanged()) {
    pe->Apply();
  }
}

QWidget* iProgramPanelBase::firstTabFocusWidget() {
  return pe->firstTabFocusWidget();
}

void iProgramPanelBase::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  m_dps->UpdateMethodButtons();
}
