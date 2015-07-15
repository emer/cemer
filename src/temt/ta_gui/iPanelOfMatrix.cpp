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

#include "iPanelOfMatrix.h"
#include <iMatrixEditor>
#include <iMatrixTableView>
#include <iMainWindowViewer>



iPanelOfMatrix::iPanelOfMatrix(taiSigLink* dl_)
:inherited(dl_)
{
  me = NULL;
}

iPanelOfMatrix::~iPanelOfMatrix() {
}

QWidget* iPanelOfMatrix::firstTabFocusWidget() {
  if(!me) return NULL;
  return me->tv;
}

void iPanelOfMatrix::SigEmit_impl(int sls, void* op1_, void* op2_) {
  inherited::SigEmit_impl(sls, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
//TODO: maybe we should do something less crude???
//  idt->updateConfig();
}

void iPanelOfMatrix::tv_hasFocus(iTableView* sender) {
  iMainWindowViewer* vw = viewerWindow();
  if(vw) {
    vw->SetClipboardHandler(sender,
			    SLOT(GetEditActionsEnabled(int&)),
			    SLOT(EditAction(int)),
			    NULL,
			    SIGNAL(UpdateUi()) );
  }
}

String iPanelOfMatrix::panel_type() const {
  static String str("Edit Matrix");
  return str;
}

void iPanelOfMatrix::UpdatePanel_impl() {
  if (me) me->Refresh();
  inherited::UpdatePanel_impl();
}

void iPanelOfMatrix::Render_impl() {
  if (me) return; // shouldn't happen
  me = new iMatrixEditor();
  me->setObjectName("MatrixEditor"); // diagnostic
  setCentralWidget(me); //sets parent
  taMatrix* mat_ = mat();
  me->setMatrix(mat_);
  connect(me->tv, SIGNAL(hasFocus(iTableView*)), this, SLOT(tv_hasFocus(iTableView*)) );
  iMainWindowViewer* vw = viewerWindow();
  if (vw) {
    me->installEventFilter(vw);
    me->tv->m_window = vw;
  }
}
