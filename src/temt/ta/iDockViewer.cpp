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

#include "iDockViewer.h"
#include <DockViewer>
#include <IViewerWidget>
#include <iAction>
#include <MainWindowViewer>
#include <iMainWindowViewer>

taTypeDef_Of(ToolBoxDockViewer);

#include <taMisc>
#include <taiMisc>

iDockViewer::iDockViewer(DockViewer* viewer_, QWidget* parent)
  : inherited(parent), IViewerWidget(viewer_)
{
  setAttribute(Qt::WA_DeleteOnClose, true);
  Init();
}

iDockViewer::~iDockViewer()
{
}

void iDockViewer::Init() {
  // set the features
  DockViewer::DockViewerFlags dock_flags = viewer()->dock_flags; // cache
  DockWidgetFeatures dwf = 0;
  if (dock_flags & DockViewer::DV_CLOSABLE)
    dwf |= QDockWidget::DockWidgetClosable;
  if (dock_flags & DockViewer::DV_MOVABLE)
    dwf |= QDockWidget::DockWidgetMovable;
  if (dock_flags & DockViewer::DV_FLOATABLE)
    dwf |= QDockWidget::DockWidgetFloatable;
  setFeatures(dwf);
}

void iDockViewer::closeEvent(QCloseEvent* e) {
   // always closing if force-quitting, docked or we no longer have our mummy
  CancelOp cancel_op = ((taMisc::quitting == taMisc::QF_FORCE_QUIT) ||
    !isFloating() || (!m_viewer)) ?
    CO_NOT_CANCELLABLE : CO_PROCEED;
  closeEvent_Handler(e, cancel_op);
}

bool iDockViewer::event(QEvent* ev) {
  bool rval = inherited::event(ev);
  if (ev->type() == QEvent::WindowActivate)
    taiMisc::active_wins.GotFocus_DockWindow(this);
  return rval;
}

void iDockViewer::hideEvent(QHideEvent* e) {
  inherited::hideEvent(e);
  Showing(false);
}

void iDockViewer::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  Showing(true);
}

void iDockViewer::Showing(bool showing) {
  DockViewer* vw = viewer();
  if (!vw) return; // shouldn't happen
  MainWindowViewer* wvw = (MainWindowViewer*)vw->GetOwner(&TA_MainWindowViewer);
  if (!wvw) return; // normally shouldn't happen for owned docks
  iMainWindowViewer* dv = wvw->viewerWindow();
  if (!dv) return;
  iAction* me = dv->toolBarMenu->FindActionByData((void*)this);
  if (!me) return;
  if(showing && vw->InheritsFrom(&TA_ToolBoxDockViewer)) {
    if(taMisc::viewer_options & taMisc::VO_NO_TOOLBOX) {
      me->setChecked(false);
      hide();
      return;
    }
  }
  if (showing == me->isChecked()) return;
  me->setChecked(showing); //note: triggers event
}

