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

#include "iDataPanelSet.h"
#include <iDataPanelFrame>
#include <iDataPanelSetButton>
#include <taiViewType>
#include <Program>
#include <taSigLinkItr>
#include <iProgramViewScriptPanel>
#include <NumberedTextView>

#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QStackedLayout>

iDataPanelSet::iDataPanelSet(taiSigLink* link_)
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

iDataPanelSet::~iDataPanelSet() {
}

void iDataPanelSet::AddSubPanel(iDataPanelFrame* pn) {
  pn->m_dps = this;
  panels.Add(pn);
  int id = panels.size - 1;
  wsSubPanels->addWidget(pn);
  iDataPanelSetButton* but = new iDataPanelSetButton(frmButtons);
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
  iTabViewer* itv = tabViewerWin();
  if (itv) pn->OnWindowBind(itv);
}

void iDataPanelSet::AddSubPanelDynamic(iDataPanelFrame* pn) {
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

void iDataPanelSet::AddMinibar() {
  QFrame* fr = new QFrame(frmButtons);
  fr->setFrameStyle(QFrame::VLine | QFrame::Sunken);
  int idx = layButtons->count() - 2; // insert before stretch
  layButtons->insertWidget(idx, fr);
  layMinibar = new QStackedLayout;
  layButtons->addLayout(layMinibar);
}

void iDataPanelSet::AddMinibarCtrl(iDataPanelFrame* pn) {
  QWidget* ctrl = (pn) ? pn->minibarCtrls() : NULL;
  // note: easiest is just to create a null widg for missing guys
  if (ctrl) {
    ctrl->setParent(frmButtons);
  } else {
    ctrl = new QWidget(frmButtons);
  }
  layMinibar->addWidget(ctrl); // no final spacing etc. so same whether static or dyn
}

void iDataPanelSet::AddMinibarCtrls() {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanelFrame* pn = qobject_cast<iDataPanelFrame*>(panels.FastEl(i));
    AddMinibarCtrl(pn);
  }
}

void iDataPanelSet::AllSubPanelsAdded() {
  // note: called after all static panels added, and if
  // we add a dynamic that needs minibar and not made yet
  // check if any need the minibar, if yes, initialize it and break
  for (int i = 0; i < panels.size; ++i) {
    iDataPanelFrame* pn = qobject_cast<iDataPanelFrame*>(panels.FastEl(i));
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

void iDataPanelSet::SigEmit_impl(int sls, void* op1, void* op2) {
  inherited::SigEmit_impl(sls, op1, op2);
  // if UDC then we need to invoke the dyn panel update procedure on the tai guy
  if (sls == SLS_USER_DATA_UPDATED) {
    TypeDef* typ = link()->GetDataTypeDef();
    if (typ && typ->iv) {
      typ->iv->CheckUpdateDataPanelSet(this);
    }
  }
}

void iDataPanelSet::SetMenu(QWidget* menu) {
  menu->setParent(centralWidget());
  layDetail->insertWidget(0, menu);
}

void iDataPanelSet::SetMethodBox(QWidget* meths, taiEditorWidgetsOfClass* mgr) {
  meths->setParent(centralWidget());
  layDetail->addWidget(meths);
  method_box_mgr = mgr;
}

void iDataPanelSet::UpdateMethodButtons() {
  if(method_box_mgr) {
    method_box_mgr->GetButtonImage();
  }
}

void iDataPanelSet::setCurrentPanelId_impl(int id) {
  iDataPanel* pn = panels.PosSafeEl(id);
  if (!pn) return; //shouldn't happen
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

void iDataPanelSet::setPanelAvailable(iDataPanel* pn) {
}

QWidget* iDataPanelSet::firstTabFocusWidget() {
  if(panels.size == 0) return NULL;
  if(panels.size == 1) return panels[0]->firstTabFocusWidget(); // skip buttons

  // focus on the current button
  QAbstractButton* but = buttons->button(cur_panel_id);
  if(but) {
    return but;
  }
  return NULL;
}


///////////////////////////////////////////////////////////////////////
//      Program specific browser guys!


iDataPanelSet* Program::FindMyDataPanelSet() {
  if(!taMisc::gui_active) return NULL;
  taSigLink* link = sig_link();
  if(!link) return NULL;
  taSigLinkItr itr;
  iDataPanelSet* el;
  FOR_DLC_EL_OF_TYPE(iDataPanelSet, el, link, itr) {
//     if (el->data() == this) {
      return el;
//     }
  }
  return NULL;
}

bool Program::ViewCtrlPanel() {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(0);
  return true;
}

bool Program::ViewProgEditor() {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(1);
  return true;
}

bool Program::ViewCssScript() {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(2);
  return true;
}

bool Program::ViewProperties() {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(3);
  return true;
}

bool Program::ViewScriptEl(taBase* pel) {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(2);
  iProgramViewScriptPanel* pnl = dynamic_cast<iProgramViewScriptPanel*>(dps->panels.SafeEl(2));
  if(!pnl || !pnl->vs) return false;
  int start_ln, end_ln;
  if(!ScriptLinesEl(pel, start_ln, end_ln))
    return false;

  pnl->vs->setHighlightLines(start_ln, (end_ln - start_ln)+1);
  return true;
}

