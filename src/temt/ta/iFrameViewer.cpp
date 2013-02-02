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

#include "iFrameViewer.h"
#include <iAction>
#include <taiWidgetMenu>
#include <iMainWindowViewer>
#include <ISelectableHost>


iFrameViewer::iFrameViewer(FrameViewer* viewer_, QWidget* parent)
  : inherited(parent), IViewerWidget(viewer_)
{
  Init();
  // note: caller will still do a virtual Constr() on us after new
}

iFrameViewer::~iFrameViewer()
{
}

void iFrameViewer::Init() {
  shn_changing = 0;
}

void iFrameViewer::hideEvent(QHideEvent* e) {
  inherited::hideEvent(e);
  Showing(false);
}

void iFrameViewer::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  Showing(true);
}

void iFrameViewer::Showing(bool showing) {
  iMainWindowViewer* dv = viewerWindow();
  if (!dv) return;
  iAction* me = dv->frameMenu->FindActionByData((void*)this);
  if (!me) return;
  if (showing == me->isChecked()) return;
  me->setChecked(showing); //note: triggers event
}

void iFrameViewer::SelectableHostNotifySlot_Internal(ISelectableHost* src, int op) {
  ++shn_changing;
    emit SelectableHostNotifySignal(src, op);
  --shn_changing;
}

void iFrameViewer::SelectableHostNotifySlot_External(ISelectableHost* src, int op) {
  if (shn_changing > 0) return; // reflection back down, ignore it
  switch (op) {
  case ISelectableHost::OP_GOT_FOCUS:
  case ISelectableHost::OP_SELECTION_CHANGED:
    SelectionChanged_impl(src);
    break;
  case ISelectableHost::OP_DESTROYING: break;
  default: break; // shouldn't happen
  }
}

MainWindowViewer* iFrameViewer::mainWindowViewer() {
  return (m_viewer) ? ((FrameViewer*)m_viewer)->mainWindowViewer() : NULL;
}

