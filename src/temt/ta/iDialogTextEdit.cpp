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

#include "iDialogTextEdit.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLayout>
#include <QPushButton>
#include <QPrintDialog>
#include <QPrinter>
#include <QShortcut>
#include <QSizePolicy>
#include <QTextEdit>
#include <QPalette>
#include <QClipboard>

iDialogTextEdit::iDialogTextEdit(bool readOnly_, QWidget* parent)
:inherited(parent)
{
  init(readOnly_);
}

void iDialogTextEdit::init(bool readOnly_) {
  m_readOnly = readOnly_;
  QRect rect(QApplication::desktop()->screenGeometry(0));
  int wd = (rect.width() * 3) / 4;
  int ht = (rect.height() * 3) / 4;
  if(wd > 640) wd = 640;        // don't make it too wide..
  this->resize(wd, ht);
  QVBoxLayout* layOuter = new QVBoxLayout(this);
  txtText = new QTextEdit(this);
  layOuter->addWidget(txtText);
  QHBoxLayout* layButtons = new QHBoxLayout();
  layButtons->setMargin(2);
  layButtons->setSpacing(4);
  layOuter->addLayout(layButtons);
  layButtons->addStretch();
  btnPrint = NULL;            // really is silly..
//   btnPrint = new QPushButton("&Print", this);
//   layButtons->addWidget(btnPrint);
//   layButtons->addSpacing(16);
//   connect(btnPrint, SIGNAL(clicked()), this, SLOT(btnPrint_clicked()) );
  btnCopy = new QPushButton("Copy to c&lipboard", this);
  layButtons->addWidget(btnCopy);
  layButtons->addSpacing(16);
  connect(btnCopy, SIGNAL(clicked()), this, SLOT(copyToClipboard()) );
  if (m_readOnly) {
    txtText->setReadOnly(true);
    btnOk = NULL;
    btnCancel = new QPushButton("&Close", this);
    layButtons->addWidget(btnCancel);
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  } else {
    btnOk = new QPushButton("&Ok", this);
    layButtons->addWidget(btnOk);
    btnCancel = new QPushButton("&Cancel", this);
    layButtons->addWidget(btnCancel);
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  }
}

iDialogTextEdit::~iDialogTextEdit() {
}

void iDialogTextEdit::btnPrint_clicked() {
  QPrinter pr;
  QPrintDialog pd(&pr, this);
  if (pd.exec() != QDialog::Accepted) return;
  // print ...
  txtText->document()->print(&pr);
}

void iDialogTextEdit::copyToClipboard() {
  QApplication::clipboard()->setText(txtText->toPlainText());
}

void iDialogTextEdit::setText(const QString& value) {
  txtText->setPlainText(value);
}
