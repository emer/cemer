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

#include "taiEditorWidgetsDoc.h"
#include <taDoc>
#include <iTextEdit>
#include <iMainWindowViewer>

#include <taiMisc>

#include <QVBoxLayout>
#include <QKeyEvent>


taiEditorWidgetsDoc::taiEditorWidgetsDoc(void* base, TypeDef* typ_, bool read_only_,
      bool modal_, QObject* parent)
: inherited(base, typ_, read_only_, modal_, parent)
{
  init();
}

void taiEditorWidgetsDoc::init() {
  tedHtml = NULL;
}

taDoc* taiEditorWidgetsDoc::doc() const {
  return static_cast<taDoc*>(root);
}

void taiEditorWidgetsDoc::Constr_Body() {
  //  taiEditorWidgetsMain::Constr_Body();
}

void taiEditorWidgetsDoc::Constr_Box() {
  // Html tab
  taDoc* doc_ = doc();
  tedHtml = new iTextEdit;
  tedHtml->setAcceptRichText(false); // is the raw html as text
  connect(tedHtml, SIGNAL(textChanged()), this, SLOT(Changed()) );
  if (read_only || (doc_ && doc_->web_doc)) {
    tedHtml->setReadOnly(true);
  }
  vblDialog->addWidget(tedHtml, 1); // gets all the space
  tedHtml->installEventFilter(this); // this does the Ctrl+Enter = Apply stuff..
  //  inherited::Constr_Box();
}

void taiEditorWidgetsDoc::ClearBody_impl() {

}

void taiEditorWidgetsDoc::GetImage_Membs() {
  //  inherited::GetImage_Membs();
  taDoc* doc = this->doc();
  if (!doc) return; // ex. for zombies
  
  if(read_only || doc->web_doc) {
    tedHtml->setReadOnly(true);
  }
  else {
    tedHtml->setReadOnly(false);
  }

  QString text = doc->text; 
  // to avoid the guy always jumping to the top after edit
  // we compare, and don't update if the same
  if (text == tedHtml->toPlainText()) return; 
  tedHtml->clear();
  tedHtml->insertPlainText(text); // we set the html as text
}

void taiEditorWidgetsDoc::GetValue_Membs() {
  //  inherited::GetValue_Membs();
  taDoc* doc = this->doc();
  if (!doc) return; // ex. for zombies
  
  doc->text = tedHtml->toPlainText();
  doc->UpdateAfterEdit();
  taiMisc::Update(doc);
}

QWidget* taiEditorWidgetsDoc::firstTabFocusWidget() {
  return tedHtml;
}

bool taiEditorWidgetsDoc::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }
  QKeyEvent* e = static_cast<QKeyEvent *>(event);

  iMainWindowViewer* mvw = viewerWindow();
  if(mvw) {
    mvw->FocusIsMiddlePanel();
    if(mvw->KeyEventFilterWindowNav(obj, e))
      return true;
  }

  // already has all the emacs edits!
//   if(taiMisc::KeyEventFilterEmacs_Edit(obj, e)) // full emacs edits
//     return true;

  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);

  // below are regular dialog guys
  if(ctrl_pressed && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))) {
    if(modal)
      Ok();
    else
      Apply();
    if(mvw)
      mvw->FocusCurTreeView(); // return focus back to current browser
    return true;
  }
  if(e->key() == Qt::Key_Escape) {
    if(modal)
      Cancel();
    else
      Revert();			// do it!
    if(mvw)
      mvw->FocusCurTreeView(); // return focus back to current browser
    return true;
  }
  return QObject::eventFilter(obj, event);
  // don't go back to regular host b/c it remaps arrows to tabs
}
