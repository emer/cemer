// Co2018ght 2016-2017, Regents of the University of Colorado,
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

#include "iDialogLineEdit.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QPalette>

#include <iLineEdit>
#include <iLabel>

iDialogLineEdit::iDialogLineEdit(QWidget* parent)
:inherited(parent)
{
  init();
}

void iDialogLineEdit::init() {
  QRect rect(QApplication::desktop()->screenGeometry(0));
  int wd = (rect.width() * 1) / 4;
  int ht = (rect.height() * 1) / 6;
  if(wd > 640) wd = 640;        // don't make it too wide..
  if(ht > 240) ht = 240;
  this->resize(wd, ht);
  QVBoxLayout* layOuter = new QVBoxLayout(this);
  prompt = new iLabel(this);
  layOuter->addWidget(prompt);
  layOuter->addSpacing(16);
  lnEdit = new iLineEdit(this);
  layOuter->addWidget(lnEdit);
  QHBoxLayout* layButtons = new QHBoxLayout();
  layButtons->setMargin(2);
  layButtons->setSpacing(4);
  layOuter->addLayout(layButtons);
  layButtons->addStretch();
  btnOk = new QPushButton("&Ok", this);
  layButtons->addWidget(btnOk);
  btnCancel = new QPushButton("&Cancel", this);
  layButtons->addWidget(btnCancel);
  connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect(lnEdit, SIGNAL(editingFinished()), this, SLOT(accept()) );
}

iDialogLineEdit::~iDialogLineEdit() {
}


bool iDialogLineEdit::LineEditDialog
(QString& str_val, const QString& prompt, const QString& ok_txt, const QString cancel_txt) {
  iDialogLineEdit* dlg = new iDialogLineEdit();
  
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  dlg->lnEdit->setText(str_val);
  dlg->prompt->setText(prompt);
  dlg->btnOk->setText(ok_txt);
  dlg->btnCancel->setText(cancel_txt);
  
  int rval = dlg->exec();

  str_val = dlg->lnEdit->text();
  
  delete dlg;
  QApplication::restoreOverrideCursor();

  return (bool)rval; // 0 = false
}
