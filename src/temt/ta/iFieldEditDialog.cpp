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

#include "iFieldEditDialog.h"

iFieldEditDialog::iFieldEditDialog(bool modal_, bool read_only_,
  const String& desc, taiField* parent)
:inherited()
{
  field = parent;
  // TODO: Why set modality here, vs. letting the caller decide?
  //  * modal:    dialog->exec()
  //  * modeless: dialog->show()/raise()/activateWindow()
  setModal(modal_);
  init(read_only_, desc);
}

void iFieldEditDialog::init(bool read_only_, const String& desc) {
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
  txtText = new iTextEdit(this);
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
  } else {
    btnOk = new QPushButton("&Ok", this);
    layButtons->addWidget(btnOk);
    btnCancel = new QPushButton("&Cancel", this);
    layButtons->addWidget(btnCancel);
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
    QObject::connect(txtText, SIGNAL(textChanged() ),
          this, SLOT(repChanged() ) );
  }
  btnApply = NULL;
  btnRevert = NULL;
  if (!isModal() && !m_read_only) {
    layButtons->addSpacing(8);
    btnApply = new QPushButton("&Apply", this);
    layButtons->addWidget(btnApply);
    btnRevert = new QPushButton("&Revert", this);
    layButtons->addWidget(btnRevert);
    setApplyEnabled(false);
    connect(btnApply, SIGNAL(clicked()), this, SLOT(btnApply_clicked()) );
    connect(btnRevert, SIGNAL(clicked()), this, SLOT(btnRevert_clicked()) );
  }
}

iFieldEditDialog::~iFieldEditDialog() {
  if (field) {
    field->edit_dialog = NULL;
    field = NULL;
  }
}

void iFieldEditDialog::accept() {
  if (!m_read_only)
    btnApply_clicked();
  if (field)
    field->edit_dialog = NULL;
//   if (!isModal()) {
//   deleteLater();
//   }
  inherited::accept();
}

void iFieldEditDialog::reject() {
  if (!isModal()) {
    if (field)
      field->edit_dialog = NULL;
    deleteLater();
  }
  inherited::reject();
}

void iFieldEditDialog::setApplyEnabled(bool enabled) {
  if (btnApply) btnApply->setEnabled(enabled);
  if (btnRevert) btnRevert->setEnabled(enabled);
}

void iFieldEditDialog::setText(const QString& value) {
  txtText->setPlainText(value);
  setApplyEnabled(false);
}

void iFieldEditDialog::btnApply_clicked() {
  field->rep()->setText(txtText->toPlainText());
  // unless explicitly overridden, we always do an autoapply
  if (!(field->flags() & taiData::flgNoEditDialogAutoApply)) {
    field->applyNow();
  }
  setApplyEnabled(false);
}

void iFieldEditDialog::btnRevert_clicked() {
  setText(field->rep()->text());
}

void iFieldEditDialog::repChanged() {
  setApplyEnabled(true);
}

