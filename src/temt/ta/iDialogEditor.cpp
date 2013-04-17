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

#include "iDialogEditor.h"
#include <iScrollArea>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QApplication>
#include <QCloseEvent>


iDialogEditor::iDialogEditor(taiEditor* owner_, QWidget* parent, int wflags)
:iDialog(parent, (Qt::WindowFlags)wflags)
{
  owner = owner_;
  mcentralWidget = NULL;
  scr = new iScrollArea(this);
  scr->setWidgetResizable(true);
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(0);
  layOuter->setSpacing(0); // none needed
  layOuter->addWidget(scr, 1);

  iSize ss = taiM->scrn_s;
  setMaximumSize(ss.width(), ss.height());
  setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
}

iDialogEditor::~iDialogEditor() {
  if (owner != NULL) {
    owner->WidgetDeleting(); // removes our ref
    owner = NULL;
  }
}

void iDialogEditor::closeEvent(QCloseEvent* ev) {
  //note: a higher level routine may already have resolved changes
  // if so, the ResolveChanges call is superfluous
  ev->accept(); // default, unless we override;
  if (owner == NULL) return;
  bool discarded = false;
  CancelOp cancel_op = CO_PROCEED;
  owner->ResolveChanges(cancel_op, &discarded);
  if (cancel_op == CO_CANCEL) {
    ev->ignore();
    return;
  } else if (!discarded) {
    owner->state = taiEditorWidgetsMain::ACCEPTED;
    return; // not rejected
  }
  // discarded, or didn't have any changes
  owner->state = taiEditorWidgetsMain::CANCELED;
  setResult(Rejected);
}

bool iDialogEditor::post(bool modal) {
  if (modal) {
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
    bool rval = (exec() == Accepted);
    QApplication::restoreOverrideCursor();
    return rval;
  } else {
    show();
    return true;
  }
}

void iDialogEditor::accept() {
  if(owner)
    owner->Ok_impl();
  inherited::accept();
}

void iDialogEditor::dismiss(bool accept_) {
  if (accept_)
    accept();
  else
    reject();
}

void iDialogEditor::iconify() {
  // Iv compatibility routine
  if (isModal() || isMinimized()) return;
  showMinimized();
}

void iDialogEditor::deiconify() {
   // Iv compatibility routine
  if (isModal() || !isMinimized()) return;
  showNormal();
}

void iDialogEditor::setButtonsWidget(QWidget* widg) {
  widg->setParent(this);
  layOuter->addWidget(widg);
}

void iDialogEditor::setCentralWidget(QWidget* widg) {
  mcentralWidget = widg;
  scr->setWidget(widg);
  widg->show();
}
