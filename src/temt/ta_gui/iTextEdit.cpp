// Copyright 2017, Regents of the University of Colorado,
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
#include <iCodeCompleter>

#include <QKeyEvent>
#include <QTextCursor>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QCoreApplication>
#include <QApplication>
#include <QScrollBar>

iTextEdit::iTextEdit(QWidget* parent, bool add_completer)
:inherited(parent)
{
  ext_select_on = false;
  init(add_completer);
}

iTextEdit::iTextEdit(const QString& text, QWidget* parent, bool add_completer)
 :inherited(text, parent)
{
  ext_select_on = false;
  init(add_completer);
}

iTextEdit::~iTextEdit()
{
}

void iTextEdit::init(bool add_completer) {
  completer = NULL;
  completion_enabled = false;
  cursor_position_from_end = 0;
  if (add_completer) {
    completer = new iCodeCompleter(parent());
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completion_enabled = taMisc::code_completion.auto_complete;
    QObject::connect(completer, SIGNAL(activated(QString)), this, SLOT(InsertCompletion(QString)));
  }
  installEventFilter(this);
}

void iTextEdit::clearExtSelection() {
  ext_select_on = false;
  QTextCursor cursor(textCursor());
  cursor.clearSelection();
  setTextCursor(cursor);
}

void iTextEdit::keyPressEvent(QKeyEvent* key_event) {
  taiMisc::UpdateUiOnCtrlPressed(this, key_event);
  
  if (key_event->key() == Qt::Key_Tab  && GetCompleter() && GetCompleter()->popup()->isVisible()) {
    key_event->ignore();
    return;
  }

  QCoreApplication* app = QCoreApplication::instance();
  QTextCursor cursor(textCursor());
  QTextCursor::MoveMode mv_md = QTextCursor::MoveAnchor;
  if(ext_select_on)
    mv_md = QTextCursor::KeepAnchor;
  
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::TEXTEDIT_CONTEXT, key_event);

  switch(action) {
    case taiMisc::TEXTEDIT_START_SELECT:
    case taiMisc::TEXTEDIT_START_SELECT_II:
      key_event->accept();
      cursor.clearSelection();
      setTextCursor(cursor);
      ext_select_on = true;
      return;
    case taiMisc::TEXTEDIT_CLEAR_SELECTION:
    case taiMisc::TEXTEDIT_CLEAR_SELECTION_II:
      key_event->accept();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_CURSOR_UP:
    case taiMisc::TEXTEDIT_CURSOR_UP_II:
      key_event->accept();
      cursor.movePosition(QTextCursor::Up, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::TEXTEDIT_CURSOR_DOWN:
    case taiMisc::TEXTEDIT_CURSOR_DOWN_II:
      key_event->accept();
      cursor.movePosition(QTextCursor::Down, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::TEXTEDIT_HOME:
    case taiMisc::TEXTEDIT_HOME_II:
      key_event->accept();
      cursor.movePosition(QTextCursor::StartOfLine, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::TEXTEDIT_END:
    case taiMisc::TEXTEDIT_END_II:
      key_event->accept();
      cursor.movePosition(QTextCursor::EndOfLine, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::TEXTEDIT_CURSOR_FORWARD:
    case taiMisc::TEXTEDIT_CURSOR_FORWARD_II:
      key_event->accept();
      cursor.movePosition(QTextCursor::NextCharacter, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::TEXTEDIT_CURSOR_BACKWARD:
    case taiMisc::TEXTEDIT_CURSOR_BACKWARD_II:
      key_event->accept();
      cursor.movePosition(QTextCursor::PreviousCharacter, mv_md);
      setTextCursor(cursor);
      return;
    case taiMisc::TEXTEDIT_PAGE_UP:
    case taiMisc::TEXTEDIT_PAGE_UP_II:
      app->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier));
      key_event->accept();
      return;
    case taiMisc::TEXTEDIT_PAGE_DOWN:
    case taiMisc::TEXTEDIT_PAGE_DOWN_II:
      key_event->accept();
        app->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
      return;
    case taiMisc::TEXTEDIT_DELETE:
    case taiMisc::TEXTEDIT_DELETE_II:
      key_event->accept();
      cursor.deleteChar();
      setTextCursor(cursor);
      return;
    case taiMisc::TEXTEDIT_DELETE_TO_END:
    case taiMisc::TEXTEDIT_DELETE_TO_END_II:
      key_event->accept();
      cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
      cursor.removeSelectedText();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_PASTE:
    case taiMisc::TEXTEDIT_PASTE_II:
      key_event->accept();
      paste();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_CUT:
    case taiMisc::TEXTEDIT_CUT_II:
      key_event->accept();
      cut();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_COPY_CLEAR:
    case taiMisc::TEXTEDIT_COPY_CLEAR_II:
      key_event->accept();
      copy();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_UNDO:
    case taiMisc::TEXTEDIT_UNDO_II:
      key_event->accept();
      undo();
      return;
    case taiMisc::TEXTEDIT_LOOKUP:
    case taiMisc::TEXTEDIT_LOOKUP_II:
      key_event->accept();
      emit lookupKeyPressed();
      return;
    case taiMisc::TEXTEDIT_FIND_IN_TEXT:
    case taiMisc::TEXTEDIT_FIND_IN_TEXT_II:
      findPrompt();
      return;
    default:
      ;
  }
  {
    if(!taiMisc::KeyEventCtrlPressed(key_event) && ((key_event->key() == Qt::Key_Enter || key_event->key() == Qt::Key_Return))) {
      if (key_event->key() == Qt::Key_Return && GetCompleter()->currentRow() > 0) {
        inherited::keyPressEvent(key_event);
        CompletionDone();
      }
      else if (key_event->key() == Qt::Key_Enter) {
        DoCompletion(true);  // try to extend
      }
      else {
        inherited::keyPressEvent(key_event);
      }
    }
    else if(!taiMisc::KeyEventCtrlPressed(key_event)
            && key_event->key() != Qt::Key_Escape
            && key_event->key() != Qt::Key_Right
            && key_event->key() != Qt::Key_Left)
    {
      inherited::keyPressEvent(key_event);
#ifdef TA_OS_MAC
      if (!(key_event->modifiers() & Qt::ControlModifier)) { // don't complete if mac command key
        DoCompletion(false);
        return;
      }
#else
      if (!(key_event->modifiers() & Qt::MetaModifier)) { // don't complete if other platform control key
        DoCompletion(false);
        return;
      }
#endif
    }
    else {
      inherited::keyPressEvent(key_event);
    }
  }

  //  inherited::keyPressEvent(key_event);
}

void iTextEdit::contextMenuEvent(QContextMenuEvent *event) {
  QMenu* menu = createStandardContextMenu();
  menu->addSeparator();
  menu->addAction("&Find..", this, SLOT(findPrompt()), QKeySequence("Ctrl+S"));
  taMisc::in_eventproc++;       // this is an event proc!
  menu->exec(event->globalPos());
  taMisc::in_eventproc--;
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

void iTextEdit::DoCompletion(bool extend) {
  if (!GetCompleter()) return;
  
  String prefix = toPlainText();
  prefix = prefix.through(textCursor().position() - 1);
  if (prefix.length() < toPlainText().length()) {
    cursor_position_from_end = toPlainText().length() - textCursor().position();
  }
  emit characterEntered();
  GetCompleter()->setCompletionPrefix(prefix);
  GetCompleter()->setCompletionMode(QCompleter::PopupCompletion);
  
  // if (IsDelimter(prefix.lastchar())) {
  //    GetCompleter()->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
  //  }
  //  else {
  //    GetCompleter()->setCompletionMode(QCompleter::PopupCompletion);
  //  }
  
  GetCompleter()->FilterList(prefix);
  
  if (extend) {
    String extended_prefix = prefix;
    GetCompleter()->ExtendSeed(extended_prefix);
    if (extended_prefix.length() > prefix.length()) {
      String extension = extended_prefix.after(prefix.length()-1);
      toPlainText().insert(0, QString(extension.chars_ptr()));
    }
  }
  
  QRect cursor_rect = cursorRect();
  cursor_rect.setWidth(GetCompleter()->popup()->sizeHintForColumn(0)
              + GetCompleter()->popup()->verticalScrollBar()->sizeHint().width());
  GetCompleter()->complete(cursor_rect);
  return;
}

void iTextEdit::CompletionDone() {
  QModelIndex index = GetCompleter()->currentIndex();
  emit completed(GetCompleter()->currentIndex());
}

void iTextEdit::focusInEvent(QFocusEvent *e) // without this the completer can't find the widget! - see http://doc.qt.io/qt-5/qtwidgets-tools-customcompleter-example.html
{
  if (GetCompleter())
    GetCompleter()->setWidget(this);
  QTextEdit::focusInEvent(e);
}

void iTextEdit::InsertCompletion(const QString& completion)
{
  if (GetCompleter()->widget() != this)
    return;
  QTextCursor tc = textCursor();
  int extra = completion.length() - GetCompleter()->completionPrefix().length();
  tc.movePosition(QTextCursor::Left);
  tc.movePosition(QTextCursor::EndOfWord);
  tc.insertText(completion.right(extra));
  setTextCursor(tc);
}


