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

#include "iPanelSetButton.h"
#include <iPanelSet>

#include <taMisc>
#include <taiMisc>

#include <QButtonGroup>
#include <QKeyEvent>


iPanelSetButton::iPanelSetButton(QWidget* parent) : QToolButton(parent) {
  m_datapanelset = NULL;
  m_idx = -1;
}

void iPanelSetButton::keyPressEvent(QKeyEvent* e) {
  if(!m_datapanelset || m_idx < 0) { inherited::keyPressEvent(e); return; }

  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  // arrow/ctrl-key nav to adjacent buttons
  if((e->key() == Qt::Key_Left) || (ctrl_pressed && e->key() == Qt::Key_B)) {
    int prv_idx = m_idx -1;
    if(prv_idx < 0) prv_idx = m_datapanelset->panels.size-1;
    if(prv_idx == m_idx) { e->accept(); return; }
    QAbstractButton* pbut = m_datapanelset->buttons->button(prv_idx);
    if(pbut) {
      pbut->click();
      pbut->setFocus();
    }
    e->accept();
    return;
  }
  else if((e->key() == Qt::Key_Right) || (ctrl_pressed && e->key() == Qt::Key_F)) {
    int nxt_idx = m_idx +1;
    if(nxt_idx >= m_datapanelset->panels.size) nxt_idx = 0;
    if(nxt_idx == m_idx) { e->accept(); return; }
    QAbstractButton* pbut = m_datapanelset->buttons->button(nxt_idx);
    if(pbut) {
      pbut->click();
      pbut->setFocus();
    }
    e->accept();
    return;
  }
  inherited::keyPressEvent(e);
}

bool iPanelSetButton::focusNextPrevChild(bool next) {
  if(!m_datapanelset || m_idx < 0) return inherited::focusNextPrevChild(next);
  if(next) {
    iPanelBase* tpan = m_datapanelset->panels.SafeEl(m_idx); // my panel
    if(!tpan) return inherited::focusNextPrevChild(next);
    QWidget* nxt = tpan->firstTabFocusWidget();
    if(!nxt) return inherited::focusNextPrevChild(next);
    nxt->setFocus();
  }
  else { // prev
    iTabView* itv = m_datapanelset->tabView(); // tab view
    if(!itv) return inherited::focusNextPrevChild(next);
    itv->tbPanels->setFocus();  // focus back on overall tab bar
  }
  return true;
}

