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

  // emacs keys!!
  if(ctrl_pressed) {
    if(e->key() == Qt::Key_P) {
      e->accept();
      cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
      setTextCursor(cursor);
    }
    else if(e->key() == Qt::Key_N) {
      e->accept();
      cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
      setTextCursor(cursor);
    }
    else if(e->key() == Qt::Key_A) {
      e->accept();
      cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
      setTextCursor(cursor);
    }
    else if(e->key() == Qt::Key_E) {
      e->accept();
      cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
      setTextCursor(cursor);
    }
    else if(e->key() == Qt::Key_F) {
      e->accept();
      cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
      setTextCursor(cursor);
    }
    else if(e->key() == Qt::Key_B) {
      e->accept();
      cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
      setTextCursor(cursor);
    }
    else if(e->key() == Qt::Key_D) {
      e->accept();
      cursor.deleteChar();
      setTextCursor(cursor);
    }
    else if(e->key() == Qt::Key_K) {
      e->accept();
      cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
      cursor.removeSelectedText();
    }
    else if(e->key() == Qt::Key_Y) {
      e->accept();
      paste();
    }
    else if(e->key() == Qt::Key_W) {
      e->accept();
      cut();
    }
    else if(e->key() == Qt::Key_Slash) {
      e->accept();
      undo();
    }
    else if(e->key() == Qt::Key_Minus) {
      e->accept();
      undo();
    }
    else if(e->key() == Qt::Key_L) {
      e->accept();
      emit lookupKeyPressed();
    }
    else {
      QTextEdit::keyPressEvent( e );
    }
  }
  else {
    QTextEdit::keyPressEvent( e );
  }
}
