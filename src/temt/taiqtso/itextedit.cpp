// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "itextedit.h"

#include <QKeyEvent>
#include <QTextCursor>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>

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

void iTextEdit::keyPressEvent(QKeyEvent* e) {
  QTextCursor cursor(textCursor());

  bool ctrl_pressed = false;
  if(e->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef TA_OS_MAC
  // ctrl = meta on apple
  if(e->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif

  QTextCursor::MoveMode mv_md = QTextCursor::MoveAnchor;
  if(ext_select_on)
    mv_md = QTextCursor::KeepAnchor;

  // emacs keys!!
  if(ctrl_pressed) {
    switch(e->key()) {
    case Qt::Key_Space:
      e->accept();
      cursor.clearSelection();
      setTextCursor(cursor);
      ext_select_on = true;
      return;
    case Qt::Key_G:
      e->accept();
      clearExtSelection();
      return;
    case Qt::Key_P:
    case Qt::Key_Up:
      e->accept();
      cursor.movePosition(QTextCursor::Up, mv_md);
      setTextCursor(cursor);
      return;
    case Qt::Key_N:
    case Qt::Key_Down:
      e->accept();
      cursor.movePosition(QTextCursor::Down, mv_md);
      setTextCursor(cursor);
      return;
    case Qt::Key_A:
      e->accept();
      cursor.movePosition(QTextCursor::StartOfLine, mv_md);
      setTextCursor(cursor);
      return;
    case Qt::Key_E:
      e->accept();
      cursor.movePosition(QTextCursor::EndOfLine, mv_md);
      setTextCursor(cursor);
      return;
    case Qt::Key_F:
    case Qt::Key_Right:
      e->accept();
      cursor.movePosition(QTextCursor::NextCharacter, mv_md);
      setTextCursor(cursor);
      return;
    case Qt::Key_B:
    case Qt::Key_Left:
      e->accept();
      cursor.movePosition(QTextCursor::PreviousCharacter, mv_md);
      setTextCursor(cursor);
      return;
    case Qt::Key_D:
      e->accept();
      cursor.deleteChar();
      setTextCursor(cursor);
      return;
    case Qt::Key_K:
      e->accept();
      cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
      cursor.removeSelectedText();
      clearExtSelection();
      return;
    case Qt::Key_Y:
      e->accept();
      paste();
      clearExtSelection();
      return;
    case Qt::Key_W:
      e->accept();
      cut();
      clearExtSelection();
      return;
    case Qt::Key_C: // global sc can grab these so do it here:
      e->accept();
      copy();
      clearExtSelection();
      return;
    case Qt::Key_X:
      e->accept();
      cut();
      clearExtSelection();
      return;
    case Qt::Key_V:
      e->accept();
      paste();
      clearExtSelection();
      return;
    case Qt::Key_S:
      findPrompt();
      return;
    case Qt::Key_Slash:
      e->accept();
      undo();
      return;
    case Qt::Key_Minus:
      e->accept();
      undo();
      return;
    case Qt::Key_L:
      e->accept();
      emit lookupKeyPressed();
      return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
      e->ignore();		// allow this to go up to higher guy
      return;
    }
  }
  else if(e->modifiers() & Qt::AltModifier) {
    if(e->key() == Qt::Key_W) { // copy
      e->accept();
      copy();
      clearExtSelection();
      return;
    }
  }
  QTextEdit::keyPressEvent( e );
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
