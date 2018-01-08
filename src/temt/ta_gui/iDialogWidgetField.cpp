// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "iDialogWidgetField.h"
#include <iLabel>
#include <iTextEdit>
#include <taiWidgetField>
#include <iLineEdit>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QPushButton>
#include <QTextCursor>


iDialogWidgetField::iDialogWidgetField(bool modal_, bool read_only_,
  const String& desc, taiWidgetField* parent)
:inherited()
{
  field = parent;
  // TODO: Why set modality here, vs. letting the caller decide?
  //  * modal:    dialog->exec()
  //  * modeless: dialog->show()/raise()/activateWindow()
  setModal(modal_);
  init(read_only_, desc);
}

void iDialogWidgetField::init(bool read_only_, const String& desc) {
  m_read_only = read_only_;
  this->resize( taiM->dialogSize(taiMisc::dlgSmall | taiMisc::dlgHor) );

  this->setFont(taiM->dialogFont(taiM->ctrl_size));
  QVBoxLayout* layOuter = new QVBoxLayout(this);
  if (desc.nonempty()) {
    iLabel* prompt = new iLabel(desc, this);
    prompt->setWordWrap(true);
    QFont f = taiM->nameFont(taiM->ctrl_size);
    f.setBold(true);
    prompt->setFont(f);
    prompt->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    layOuter->addWidget(prompt);
  }
  txtText = new iTextEdit(this, true);  // true - add a completer to this text field
  if (txtText->GetCompleter()) {
    txtText->GetCompleter()->SetHostType(iCodeCompleter::TEXT_EDIT_HOST);
  }
  layOuter->addWidget(txtText);
  QHBoxLayout* layButtons = new QHBoxLayout();
  layButtons->setMargin(2);
  layButtons->setSpacing(4);
  layOuter->addLayout(layButtons);
  layButtons->addStretch();
  if (m_read_only) {
    txtText->setReadOnly(true);
    btnOk = NULL;
    btnCancel = new QPushButton("&Close", this);
    layButtons->addWidget(btnCancel);
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  }
  else {
    btnOk = new QPushButton("&Ok", this);
    layButtons->addWidget(btnOk);
    btnCancel = new QPushButton("&Cancel", this);
    layButtons->addWidget(btnCancel);
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
    QObject::connect(txtText, SIGNAL(textChanged() ), this, SLOT(repChanged() ) );
  }
}

iDialogWidgetField::~iDialogWidgetField() {
  if (field) {
    field->edit_dialog = NULL;
    field = NULL;
  }
}

void iDialogWidgetField::accept() {
  if (!m_read_only) {
    btnApply_clicked();  // need to apply even without the apply button
  }
  if (field)
    field->edit_dialog = NULL;
//   if (!isModal()) {
//   deleteLater();
//   }
  inherited::accept();
}

void iDialogWidgetField::reject() {
  if (!isModal()) {
    if (field)
      field->edit_dialog = NULL;
    deleteLater();
  }
  inherited::reject();
}

void iDialogWidgetField::setText(const QString& value, const int cursor_position) {
  txtText->setPlainText(value);
  QTextCursor cursor = txtText->textCursor();
  cursor.setPosition(cursor_position);
  txtText->setTextCursor(cursor);
}

void iDialogWidgetField::btnApply_clicked() {  // clicked virtually now that the button was removed
  field->rep()->setText(txtText->toPlainText().trimmed());
  // unless explicitly overridden, we always do an autoapply
  if (!(field->flags() & taiWidget::flgNoEditDialogAutoApply)) {
    field->applyNow();
  }
}

void iDialogWidgetField::repChanged() {
}

