// Copyright 2017, Regents of the University of Colorado,
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

#include "iPanelSet.h"
#include <iPanel>
#include <iPanelSetButton>
#include <taiViewType>
#include <taSigLinkItr>

#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QStackedLayout>

iPanelSet::iPanelSet(taiSigLink* link_)
:inherited(link_)
{
  layMinibar = NULL;
  frmButtons = new QFrame(widg);
  frmButtons->setFrameShape(QFrame::Box);
  frmButtons->setFrameShadow(QFrame::Sunken);
  layButtons = new QHBoxLayout(frmButtons);
  layButtons->setMargin(2);
  layButtons->setSpacing(taiM->hspc_c);
  // note: because we can add btns dynamically, we insert them, so put final
  // spacing here
  layButtons->addSpacing(3); // in case of minibar
  layButtons->addStretch(); // before mb
  layDetail->addWidget(frmButtons);
  buttons = new QButtonGroup(frmButtons); // note: not a widget, not visible
  buttons->setExclusive(true); // this is the default anyway

  layDetail->addWidget(wsSubPanels, 1);

  method_box_mgr = NULL;

  connect(buttons, SIGNAL(buttonClicked(int)), this, SLOT(setCurrentPanelId(int)));
}

iPanelSet::~iPanelSet() {
}

void iPanelSet::AddSubPanel(iPanel* pn) {
  pn->m_dps = this;
  panels.Add(pn);
  int id = panels.size - 1;
  wsSubPanels->addWidget(pn);
  iPanelSetButton* but = new iPanelSetButton(frmButtons);
  but->m_datapanelset = this;
  but->m_idx = id;
  but->setMaximumHeight(taiM->button_height(taiMisc::sizSmall));
  but->setFont(taiM->buttonFont(taiMisc::sizSmall));
  // first visible button should be down
  if (id == 0) but->setDown(true); // first button should be down
  but->setCheckable(true);
  but->setText(pn->panel_type());
  buttons->addButton(but, id);
  // layout position is same as button (no preceding layouts/wids)
  layButtons->insertWidget(id, but);
  but->show();
  pn->AddedToPanelSet();
  iPanelViewer* itv = tabViewerWin();
  if (itv) pn->OnWindowBind(itv);
}

void iPanelSet::AddSubPanelDynamic(iPanel* pn) {
  AddSubPanel(pn);
  // ok, now the tricky part -- esp if we have a minibar but others didn't
  if (pn->minibarCtrls()) {
    if (layMinibar) {
      // already created, so just add in our ctrl or dummy
      AddMinibarCtrl(pn);
    } else {
      // none, so need to make the whole bar and add all dummies
      AllSubPanelsAdded();
    }
  } else { // no mb for us, but add dummy if we had one
    if (layMinibar) {
      AddMinibarCtrl(pn); // we know we don't have one, but routine just does dummy
    }
  }
}

void iPanelSet::AddMinibar() {
  QFrame* fr = new QFrame(frmButtons);
  fr->setFrameStyle(QFrame::VLine | QFrame::Sunken);
  int idx = layButtons->count() - 2; // insert before stretch
  layButtons->insertWidget(idx, fr);
  layMinibar = new QStackedLayout;
  layButtons->addLayout(layMinibar);
}

void iPanelSet::AddMinibarCtrl(iPanel* pn) {
  QWidget* ctrl = (pn) ? pn->minibarCtrls() : NULL;
  // note: easiest is just to create a null widg for missing guys
  if (ctrl) {
    ctrl->setParent(frmButtons);
  } else {
    ctrl = new QWidget(frmButtons);
  }
  layMinibar->addWidget(ctrl); // no final spacing etc. so same whether static or dyn
}

void iPanelSet::AddMinibarCtrls() {
  for (int i = 0; i < panels.size; ++i) {
    iPanel* pn = qobject_cast<iPanel*>(panels.FastEl(i));
    AddMinibarCtrl(pn);
  }
}

void iPanelSet::AllSubPanelsAdded() {
  // note: called after all static panels added, and if
  // we add a dynamic that needs minibar and not made yet
  // check if any need the minibar, if yes, initialize it and break
  for (int i = 0; i < panels.size; ++i) {
    iPanel* pn = qobject_cast<iPanel*>(panels.FastEl(i));
    if (!pn) continue; // should always be
    if (pn->minibarCtrls()) {
      AddMinibar();
      break;
    };
  }
  if (layMinibar) {
    AddMinibarCtrls();
  }
}

void iPanelSet::SigEmit_impl(int sls, void* op1, void* op2) {
  inherited::SigEmit_impl(sls, op1, op2);
  // if UDC then we need to invoke the dyn panel update procedure on the tai guy
  if (sls == SLS_USER_DATA_UPDATED) {
    TypeDef* typ = link()->GetDataTypeDef();
    if (typ && typ->iv) {
      typ->iv->CheckUpdateDataPanelSet(this);
    }
  }
}

void iPanelSet::SetMenu(QWidget* menu) {
  menu->setParent(centralWidget());
  layDetail->insertWidget(0, menu);
}

void iPanelSet::SetMethodBox(QWidget* meths, taiEditorOfClass* mgr) {
  meths->setParent(centralWidget());
  layDetail->addWidget(meths);
  method_box_mgr = mgr;
}

void iPanelSet::UpdateMethodButtons() {
  if(method_box_mgr) {
    method_box_mgr->GetButtonImage();
  }
}

void iPanelSet::setCurrentPanelId_impl(int id) {
  iPanelBase* pn = panels.PosSafeEl(id);
  if (!pn)
    return; //shouldn't happen
  emit qt_sig_PanelChanged(id);
  wsSubPanels->setCurrentWidget(pn);
  QAbstractButton* but = buttons->button(id);
  // for when called programmatically:
  if (but) // should always have a value
    but->setDown(true);
  //note: very annoying, but autoexclusive was not working, so we had to do it manually
  QAbstractButton* but2;
  foreach(but2, buttons->buttons()) {
    if (but2 != but) but2->setDown(false);
  }
  if (layMinibar) {
    layMinibar->setCurrentIndex(id);
  }
  //TODO: maybe something to change tab color
}

void iPanelSet::setPanelAvailable(iPanelBase* pn) {
}

QWidget* iPanelSet::firstTabFocusWidget() {
  if(panels.size == 0) return NULL;
  if(panels.size == 1) return panels[0]->firstTabFocusWidget(); // skip buttons

  // focus on the current button
  QAbstractButton* but = buttons->button(cur_panel_id);
  if(but) {
    return but;
  }
  return NULL;
}


