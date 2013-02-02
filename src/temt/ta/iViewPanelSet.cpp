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

#include "iViewPanelSet.h"
#include <iViewPanel>


#include <QVBoxLayout>
#include <QStackedWidget>


iViewPanelSet::iViewPanelSet(taiSigLink* link_)
:inherited(link_)
{
  layDetail->addWidget(wsSubPanels, 1);
  tbSubPanels = new iTabBarBase(widg);
  tbSubPanels->setShape(QTabBar::TriangularSouth);
  layDetail->addWidget(tbSubPanels);

  connect(tbSubPanels, SIGNAL(currentChanged(int)), this, SLOT(setCurrentPanelId(int)));
}

iViewPanelSet::~iViewPanelSet() {
//  panels.Reset(); // don't need/want to find any when child panels deleting
}

void iViewPanelSet::AddSubPanel(iViewPanel* pn) {
  pn->m_dps = this;
  panels.Add(pn);
  wsSubPanels->addWidget(pn);
  tbSubPanels->addTab(pn->TabText());

  pn->AddedToPanelSet();
  iPanelViewer* itv = tabViewerWin();
  if (itv) pn->OnWindowBind(itv);
}

void iViewPanelSet::PanelDestroying(iViewPanel* pn) {
  int id = panels.FindEl(pn);
  if (id < 0) return;
  panels.RemoveIdx(id); // do 1st in case the gui gets triggered by:
  tbSubPanels->removeTab(id);
  pn->m_dps = NULL; // cut the link
}

void iViewPanelSet::setCurrentPanelId_impl(int id) {
  iPanelBase* pn = panels.PosSafeEl(id);
  if (!pn) return; //shouldn't happen
  wsSubPanels->setCurrentWidget(pn);
  tbSubPanels->setCurrentIndex(id);
}

void iViewPanelSet::UpdatePanel() {
  inherited::UpdatePanel();
  for (int i = 0; i < panels.size; ++i) {
    iPanelBase* pn = panels.FastEl(i);
    tbSubPanels->setTabText(i, pn->TabText());
    tbSubPanels->setTabToolTip(i, pn->TabText());
  }
}


