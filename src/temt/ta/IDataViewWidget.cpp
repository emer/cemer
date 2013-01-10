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

#include "IDataViewWidget.h"

IDataViewWidget::IDataViewWidget(DataViewer* viewer_)
{
  m_viewer = viewer_;
  m_window = NULL;
  // note: caller will still do a virtual Constr() on us after new
}

IDataViewWidget::~IDataViewWidget() {
  //note: the viewer may already have deleted, so it will have nulled its ref here
  if (m_viewer) {
    m_viewer->WidgetDeleting();
    m_viewer = NULL;
  }
}

void IDataViewWidget::Close() {
  if (m_viewer) {
    CancelOp cancel_op = CO_NOT_CANCELLABLE; // the Close call is unconditional
    OnClosing_impl(cancel_op);
  }
  if (m_viewer) {
    m_viewer->WidgetDeleting();
    m_viewer = NULL;
  }
//TEMP  delete this;
widget()->deleteLater();
  //NO CODE AFTER THIS POINT -- WE ARE DELETED
}

void IDataViewWidget::closeEvent_Handler(QCloseEvent* e,
    CancelOp def_cancel_op)
{
  OnClosing_impl(def_cancel_op); // param is op&
  if (def_cancel_op == CO_CANCEL)
    e->ignore();
  else // proceed or not cancellable
    e->accept();
}

void IDataViewWidget::OnClosing_impl(CancelOp& cancel_op) {
  if (m_viewer) {
    m_viewer->WindowClosing(cancel_op);
  }
}

void IDataViewWidget::Refresh() {
  taMisc::Busy(true);
  Refresh_impl();
  taMisc::Busy(false);
}

iMainWindowViewer* IDataViewWidget::viewerWindow() const {
  //note: an owner might have simply set this on creation
  if (!m_window) {
    //note: ok to cast away constness
    QWidget* par = const_cast<IDataViewWidget*>(this)->widget(); // we start here, so MainViewer itself returns itself
    do {
      m_window = qobject_cast<iMainWindowViewer*>(par);
      if (m_window) break;
    } while ((par = par->parentWidget()));
  }
  return m_window; // could still be null if not found
}

