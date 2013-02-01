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

#include "iDataPanelFrame.h"
#include <iDataPanelSet>

#include <QHBoxLayout>


iDataPanelFrame::iDataPanelFrame(taiSigLink* dl_)
:inherited(dl_)
{
  m_dps = NULL;
  m_minibar_ctrls = NULL;
  lay_minibar_ctrls = NULL;
}

iDataPanelFrame::~iDataPanelFrame() {
}

void iDataPanelFrame::AddMinibarWidget(QWidget* ctrl) {
  if (!m_minibar_ctrls) {
    m_minibar_ctrls = new QWidget; // not owned, because panel set inserts
    lay_minibar_ctrls = new QHBoxLayout(m_minibar_ctrls);
    lay_minibar_ctrls->setMargin(0);
    lay_minibar_ctrls->setSpacing(2);
  }
  ctrl->setParent(m_minibar_ctrls);
  lay_minibar_ctrls->addWidget(ctrl, 0, (Qt::AlignRight | Qt::AlignVCenter));
}

void iDataPanelFrame::ClosePanel() {
  CancelOp cancel_op = CO_NOT_CANCELLABLE;
  Closing(cancel_op); // forced, ignore cancel
//TENT -- this could have bad side effects, but receiver should treat us as deleted...
  emit destroyed(this); // signals tab view we are gone
/*  if (m_tabView) // effectively unlink from system
    m_tabView->DataPanelDestroying(this); */
  if (!m_dps) // if in a panelset, we let panelset destroy us
    deleteLater(); // per Qt specs, defer deletions to avoid issues
}

void iDataPanelFrame::SigLinkDestroying(taSigLink*) {
  if (!m_dps) // if in a panelset, we let panelset destroy us
    ClosePanel();
}

void iDataPanelFrame::UpdatePanel() {
  if (!HasChanged())
    inherited::UpdatePanel();
}

taiSigLink* iDataPanelFrame::par_link() const {
  if (m_dps) return m_dps->par_link();
  else       return (tabView()) ? tabView()->par_link() : NULL;
}

MemberDef* iDataPanelFrame::par_md() const {
  if (m_dps) return m_dps->par_md();
  else       return (tabView()) ? tabView()->par_md() : NULL;
}

String iDataPanelFrame::TabText() const {
  if (m_dps) return m_dps->TabText();
  else       return inherited::TabText();
}

iTabViewer* iDataPanelFrame::tabViewerWin() const {
  if (m_dps) return m_dps->tabViewerWin();
  else       return (tabView()) ? tabView()->tabViewerWin() : NULL;
}

