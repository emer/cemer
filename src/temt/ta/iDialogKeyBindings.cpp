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

#include "iDialogKeyBindings.h"

#include <taMisc>
#include <taiMisc>
#include <iMainWindowViewer>
#include <EnumSpace>
#include <TypeDef>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QKeySequenceEdit>
#include <QPushButton>

iDialogKeyBindings* iDialogKeyBindings::New(iMainWindowViewer* par_window_)
{
  iDialogKeyBindings* rval = new iDialogKeyBindings(par_window_);
//  wflg &= ~Qt::WindowStaysOnTopHint;
//  rval->setFont(taiM->dialogFont(ft));
  rval->Constr();
  return rval;
}

iDialogKeyBindings::iDialogKeyBindings(QWidget* par_window_)
:inherited(par_window_)
{
  setModal(true);
  setWindowTitle("Key Binding Preferences");
  resize(taiM->dialogSize(taiMisc::hdlg_s));
}

iDialogKeyBindings::iDialogKeyBindings() {
}

void iDialogKeyBindings::Constr() {  // TODO - use tabWidget for each context and all of the actual binding
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);
  
  String instruction_str = "Type the key sequence you want to use - simple as that";
  QLabel* instruction = new QLabel(instruction_str);
  layOuter->addWidget(instruction);
  
  // add all of the actions that can be bound to keys
  QFormLayout* bindings_layout = new QFormLayout;
  bindings_layout->setLabelAlignment(Qt::AlignLeft);
  
  String enum_label;
  QKeySequenceEdit* edit;
  for (int i=0; i<5; i++) {
    enum_label = TA_taiMisc.GetEnumString("BoundAction", i);
    QLabel* action = new QLabel(enum_label);
    edit = new QKeySequenceEdit(QKeySequence());
    bindings_layout->addRow(action, edit);
  }
  layOuter->addLayout(bindings_layout);
  
  QHBoxLayout* button_layout = new QHBoxLayout();
  button_layout->addStretch();
  button_ok = new QPushButton("&Ok", this);
  button_ok->setDefault(true);
  button_layout->addWidget(button_ok);
  button_layout->addSpacing(taiM->vsep_c);
  button_revert = new QPushButton("&Revert", this);
  button_layout->addWidget(button_revert);
  layOuter->addLayout(button_layout);

  connect(button_ok, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(button_revert, SIGNAL(clicked()), this, SLOT(reject()) );
}

void iDialogKeyBindings::accept() {
  inherited::accept();
  // here is where we execute actions!
}

void iDialogKeyBindings::reject() {
  inherited::reject();
}
