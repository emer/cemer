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

#include "iTextDataPanel.h"
#include <iTextEdit>
#include <iClipData>
#include <iMainWindowViewer>

#include <SigLinkSignal>


iTextDataPanel::iTextDataPanel(taiSigLink* dl_)
:inherited(dl_)
{
  txtText = new iTextEdit(this);
  setCentralWidget(txtText);
  // default is ro
  setReadOnly(true);
  connect(txtText, SIGNAL(copyAvailable(bool)),
      this, SLOT(textText_copyAvailable(bool)) );
}

iTextDataPanel::~iTextDataPanel() {
}

QWidget* iTextDataPanel::firstTabFocusWidget() {
  return txtText;
}

void iTextDataPanel::SigEmit_impl(int sls, void* op1_, void* op2_) {
  inherited::SigEmit_impl(sls, op1_, op2_);
//TODO:  if (sls <= SLS_ITEM_UPDATED_ND) ;
  //get updated text
}

int iTextDataPanel::EditAction(int ea) {
  int rval = 0;
//todo
  return rval;
}


int iTextDataPanel::GetEditActions() {
  int rval = 0;
  QTextCursor tc(txtText->textCursor());
  if (tc.hasSelection())
    rval |= iClipData::EA_COPY;
//TODO: more, if not readonly
  return rval;
}

bool iTextDataPanel::readOnly() {
  return txtText->isReadOnly();
}

void iTextDataPanel::setReadOnly(bool value) {
  txtText->setReadOnly(value);
}

void iTextDataPanel::setText(const String& value) {
  txtText->setPlainText(value);
}


void iTextDataPanel::textText_copyAvailable (bool) {
  viewerWindow()->UpdateUi();
}

String iTextDataPanel::panel_type() const {
  static String str("Text View");
  return str;
}


