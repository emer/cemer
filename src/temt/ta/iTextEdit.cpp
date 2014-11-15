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

#include "iTextEdit.h"

#include <taMisc>
#include <taiMisc>
// ^^ note: this creates dependence on ta stuff, but needed for keyboard prefs

#include <QKeyEvent>
#include <QTextCursor>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QCoreApplication>
#include <QApplication>

iTextEdit::iTextEdit(QWidget* parent)
:inherited(parent)
{
}

iTextEdit::iTextEdit(const QString& text, QWidget* parent)
 :inherited(text, parent)
{
}

iTextEdit::~iTextEdit()
{
}

void iTextEdit::clearExtSelection() {
  ext_select_on = false;
  QTextCursor cursor(textCursor());
  cursor.clearSelection();
  setTextCursor(cursor);
}

void iTextEdit::keyPressEvent(QKeyEvent* key_event) {
  taiMisc::UpdateUiOnCtrlPressed(this, key_event);

  QCoreApplication* app = QCoreApplication::instance();
  QTextCursor cursor(textCursor());
  QTextCursor::MoveMode mv_md = QTextCursor::MoveAnchor;
  if(ext_select_on)
    mv_md = QTextCursor::KeepAnchor;
  
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::LINE_EDIT_CONTEXT, key_event);

  switch(action) {
    case taiMisc::EMACS_DESELECT:
      key_event->accept();
      cursor.clearSelection();
      setTextCursor(cursor);
      ext_select_on = true;
      return;
    case taiMisc::EMACS_CLEAR_EXTENDED_SELECTION:
      key_event->accept();
      clearExtSelection();
      return;
    case taiMisc::CURSOR_UP:
      key_event->accept();
      cursor.movePosition(QTextCursor::Up, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::CURSOR_DOWN:
      key_event->accept();
      cursor.movePosition(QTextCursor::Down, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::EMACS_HOME:
      key_event->accept();
      cursor.movePosition(QTextCursor::StartOfLine, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::EMACS_END:
      key_event->accept();
      cursor.movePosition(QTextCursor::EndOfLine, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::EMACS_CURSOR_FORWARD:
      key_event->accept();
      cursor.movePosition(QTextCursor::NextCharacter, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::EMACS_CURSOR_BACKWARD:
      key_event->accept();
      cursor.movePosition(QTextCursor::PreviousCharacter, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::EMACS_PAGE_UP:
      app->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier));
      key_event->accept();
      return;
    case taiMisc::EMACS_PAGE_DOWN:
      key_event->accept();
      if (taMisc::emacs_mode) {
        app->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
      }
      else
      {
        paste();
        clearExtSelection();
      }
      return;
    case taiMisc::EMACS_DELETE:
      key_event->accept();
      cursor.deleteChar();
      setTextCursor(cursor);
      return;
    case taiMisc::EMACS_KILL:
      key_event->accept();
      cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
      cursor.removeSelectedText();
      clearExtSelection();
      return;
    case taiMisc::EMACS_PASTE:
      key_event->accept();
      paste();
      clearExtSelection();
      return;
    case taiMisc::EMACS_CUT:
      key_event->accept();
      cut();
      clearExtSelection();
      return;
    case taiMisc::EMACS_COPY_CLEAR:
      key_event->accept();
      copy();
      clearExtSelection();
      return;
    case taiMisc::EMACS_UNDO:
      key_event->accept();
      undo();
      return;
    case taiMisc::LOOKUP:
      key_event->accept();
      emit lookupKeyPressed();
      return;
    case taiMisc::TEXT_EDIT_IGNORE:
      key_event->ignore();		// allow this to go up to higher guy
      return;
    case taiMisc::EMACS_FIND_IN_TEXT:
      findPrompt();
      return;
      
//    case Qt::Key_C: // global sc can grab these so do it here:
//      key_event->accept();
//      copy();
//      clearExtSelection();
//      return;
//    case Qt::Key_X:
//      key_event->accept();
//      cut();
//      clearExtSelection();
//      return;

    default:
      QTextEdit::keyPressEvent(key_event);
  }
}

void iTextEdit::contextMenuEvent(QContextMenuEvent *event) {
  QMenu* menu = createStandardContextMenu();
  menu->addSeparator();
  menu->addAction("&Find..", this, SLOT(findPrompt()), QKeySequence("Ctrl+S"));
  menu->exec(event->globalPos());
  delete menu;
}

bool iTextEdit::findPrompt() {
  bool ok;
  QString srch = QInputDialog::getText
    (this, "Find...", "Enter text to find", QLineEdit::Normal, prev_find_str, &ok);

  if(ok && !srch.isEmpty()) {
    prev_find_str = srch;

    bool match = find(srch);

    if(!match) {
      QMessageBox::information(this, "Text Editor", QString("Find text not found -- trying from start of document: ") + srch);
		
      QTextCursor cursor(textCursor());
      cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
      setTextCursor(cursor);

      match = find(srch);
      if(!match) {
	QMessageBox::information(this, "Text Editor", QString("Find text still not found -- giving up: ") + srch);
	return false;
      }
    }
  }
  return true;
}
