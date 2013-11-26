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

#include "iPanel.h"
#include <iPanelSet>

#include <taiMisc>

#include <QHBoxLayout>
#include <QLabel>


iPanel::iPanel(taiSigLink* dl_)
:inherited(dl_)
{
  m_dps = NULL;
  m_minibar_ctrls = NULL;
  lay_minibar_ctrls = NULL;
  
  // AddMiniBarMembers();
}

iPanel::~iPanel() {
}

// void iPanel::AddMiniBarMembers() {
//   if(!link() || !link()->isBase()) 
//     return;

//   taBase* obj = link()->taData();
//   if(!obj) return;

//   TypeDef* td = obj->GetTypeDef();
//   for(int i=0;i<td->members.size; i++) {
//     MemberDef* md = td->members[i];
//     if(!md->HasOption("MINIBAR")) continue;
//     QLabel* lab = new QLabel;
//     lab->setMaximumHeight(taiM->label_height(taiMisc::sizSmall));
//     lab->setFont(taiM->nameFont(taiMisc::sizSmall));
//     lab->setText(md->name + ": " + md->type->GetValStr(md->GetOff(obj), obj, md));
//     lab->setToolTip(md->desc);
//     AddMinibarWidget(lab);
//   }
// }

void iPanel::AddMinibarWidget(QWidget* ctrl) {
  if (!m_minibar_ctrls) {
    m_minibar_ctrls = new QWidget; // not owned, because panel set inserts
    lay_minibar_ctrls = new QHBoxLayout(m_minibar_ctrls);
    lay_minibar_ctrls->setMargin(0);
    lay_minibar_ctrls->setSpacing(2);
  }
  ctrl->setParent(m_minibar_ctrls);
  lay_minibar_ctrls->addWidget(ctrl, 0, (Qt::AlignRight | Qt::AlignVCenter));
}

void iPanel::ClosePanel() {
  CancelOp cancel_op = CO_NOT_CANCELLABLE;
  Closing(cancel_op); // forced, ignore cancel
//TENT -- this could have bad side effects, but receiver should treat us as deleted...
  emit destroyed(this); // signals tab view we are gone
/*  if (m_tabView) // effectively unlink from system
    m_tabView->DataPanelDestroying(this); */
  if (!m_dps) // if in a panelset, we let panelset destroy us
    deleteLater(); // per Qt specs, defer deletions to avoid issues
}

void iPanel::SigLinkDestroying(taSigLink*) {
  if (!m_dps) // if in a panelset, we let panelset destroy us
    ClosePanel();
}

void iPanel::UpdatePanel() {
  if (!HasChanged())
    inherited::UpdatePanel();
}

taiSigLink* iPanel::par_link() const {
  if (m_dps) return m_dps->par_link();
  else       return (tabView()) ? tabView()->par_link() : NULL;
}

MemberDef* iPanel::par_md() const {
  if (m_dps) return m_dps->par_md();
  else       return (tabView()) ? tabView()->par_md() : NULL;
}

String iPanel::TabText() const {
  if (m_dps) return m_dps->TabText();
  else       return inherited::TabText();
}

iPanelViewer* iPanel::tabViewerWin() const {
  if (m_dps) return m_dps->tabViewerWin();
  else       return (tabView()) ? tabView()->tabViewerWin() : NULL;
}

