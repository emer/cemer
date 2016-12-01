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

#define use_completer 0

#include "iLineEdit.h"

#include <iDialogTextEdit>

#include <taiMisc>
#include <QDesktopWidget>
#include <QPalette>
#include <QApplication>
#include <QTextEdit>
#include <QKeyEvent>
#include <QObject>
#include <QDebug>

#include <taMisc>
#include <KeyBindings>
#include <KeyBindings_List>

#include <iCodeCompleter>

iLineEdit::iLineEdit(QWidget* parent, bool add_completer)
: QLineEdit(parent)
{
  has_completer = add_completer;
  init();
}

iLineEdit::iLineEdit(const char* text, QWidget* parent, bool add_completer)
: QLineEdit(QString(text), parent)
{
  has_completer = add_completer;
  init();
}

void iLineEdit::init() {
  // none of these did anything for the mac trackpad hypersensitivity:
  // see iMainWindowViewer for soln involving turning off touch events
  // per bug ticket: https://bugreports.qt-project.org/browse/QTBUG-38815
#if defined(TA_OS_MAC) && (QT_VERSION >= 0x050200)
  setAttribute(Qt::WA_AcceptTouchEvents, false);
#endif
  init_start_pos = 0;
  init_start_kill = false;
  mmin_char_width = 0;
  mchar_width = 0;
  ext_select_on = false;
  // this seems unnecessary, and conflicts with ctrl-U select-all!
//   QShortcut* sc = new QShortcut(QKeySequence(/*Qt::ALT +*/ Qt::CTRL + Qt::Key_U), this);
//   sc->setContext(Qt::WidgetShortcut);
//   connect(sc, SIGNAL(activated()), this, SLOT(editInEditor()));
  
  completer = NULL;
#if use_completer
  if (has_completer) {
    completer = new iCodeCompleter(parent());
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    this->setCompleter(completer);
  }
#endif
}

void iLineEdit::editInEditor() {
  iDialogTextEdit* dlg = new iDialogTextEdit(); // no parent needed for modals
  // set to be ~3/4 of screen
  QDesktopWidget *d = QApplication::desktop();
  int primaryScreen = d->primaryScreen();
  QSize sz = d->availableGeometry(primaryScreen).size();
  int wd = (sz.width() * 3) / 4;
  int ht = (sz.height() * 3) / 4;
  if(wd > 640) wd = 640;        // don't make it too wide..
  dlg->resize(wd, ht);
  if (isReadOnly())
    dlg->txtText->setReadOnly(true);
  dlg->txtText->setPlainText(text());
  if (!isReadOnly() && (dlg->exec() == QDialog::Accepted)) {
    setText(dlg->txtText->toPlainText());
  }
  dlg->deleteLater();
}

void iLineEdit::focusInEvent(QFocusEvent* e) {
  inherited::focusInEvent(e);
  
  if (!(taMisc::edit_options & taMisc::TYPING_REPLACES) &&
      e->reason() != Qt::PopupFocusReason) {
    if(hasSelectedText()) {
      deselect();
      if(init_start_pos == -1) {
        end(false);
      }
      else {
        setCursorPosition(0);
      }
    }
    if(init_start_kill) {
      end(true);                // mark
      cut();
      clearExtSelection();
    }
  }
  // activateWindow();          // make sure we're active when we click in a box!
  // std::cerr << "focus in" << std::endl;
  // emit focusChanged(true);
}

void iLineEdit::focusOutEvent(QFocusEvent* e) {
  inherited::focusOutEvent(e);
  // std::cerr << "focus out" << std::endl;
  // emit focusChanged(false);
}

void iLineEdit::setCharWidth(int num) {
  if (num > 128) num = 128;
  else if (num <= 0) num = 0;
  if (mchar_width == num) return;
  mchar_width = num;
  if (num == 0) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  } else {
    QFontMetrics fm(font());
    QString s; s.reserve(num);
    // just use numbers, which are probably of about average width
    for (int i = 0; i < num; ++i) {
      s.append('0');
    }
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    int w = fm.size(Qt::TextSingleLine, s).width();
    setMinimumWidth(w);
    setMaximumWidth(w);
  }
}

void iLineEdit::setMinCharWidth(int num) {
  if (num > 128) num = 128;
  else if (num < 0) num = 0;
  if (mmin_char_width == num) return;
  mmin_char_width = num;
  if (num == 0) {
    setMinimumWidth(0);
  } else {
    QFontMetrics fm(font());
    QString s; s.reserve(num);
    // just use numbers, which are probably of about average width
    for (int i = 0; i < num; ++i) {
      s.append(QChar('0' + (i % 10)));
    }
    setMinimumWidth(fm.size(Qt::TextSingleLine, s).width());
  }
}

void iLineEdit::setReadOnly(bool value) {
  if (isReadOnly() == value) return;
  inherited::setReadOnly(value);
  QPalette pal(palette());
  if (value) {
    setFocusPolicy(Qt::ClickFocus); // take out of tab chain
    pal.setColor(backgroundRole(), Qt::lightGray);
  } else {
    setFocusPolicy(Qt::StrongFocus); // put back in tab chain
    pal.setColor(backgroundRole(),
      QApplication::palette(this).color(QPalette::Base));
  }
  setPalette(pal);
  update();
}

void iLineEdit::clearExtSelection()
{
  ext_select_on = false;
  inherited::deselect();
}

bool iLineEdit::event(QEvent* e)
{
  return inherited::event(e);
  // std::cerr << "event" << e->type() << std::endl;
}

void iLineEdit::keyPressEvent(QKeyEvent* key_event)
{
  taiMisc::UpdateUiOnCtrlPressed(this, key_event);
  
#if use_completer
  if(!taiMisc::KeyEventCtrlPressed(key_event) &&
     (key_event->key() == Qt::Key_Return || key_event->key() == Qt::Key_Enter)) {
    inherited::keyPressEvent(key_event);
    QModelIndex index = GetCompleter()->currentIndex();
    CompletionDone();
    return;
  }
  else if(!taiMisc::KeyEventCtrlPressed(key_event) && key_event->key() != Qt::Key_Escape &&
     key_event->key() != Qt::Key_Minus &&
     key_event->key() != Qt::Key_Greater &&
     key_event->key() != Qt::Key_Right &&
     key_event->key() != Qt::Key_Left &&
     key_event->key() != Qt::Key_Backspace) {  // knows about mac vs other OS
    QString cur_text = text();
    completer->setCompletionPrefix(text() + QString(key_event->key()).toLower());
    CharEntered();
    inherited::keyPressEvent(key_event);
    return;
  }
#endif

  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::TEXTEDIT_CONTEXT, key_event);
  
  switch (action) {
    case taiMisc::TEXTEDIT_START_SELECT:
    case taiMisc::TEXTEDIT_START_SELECT_II:
      key_event->accept();
      deselect();
      ext_select_on = true;
      return;
    case taiMisc::TEXTEDIT_CLEAR_SELECTION:
    case taiMisc::TEXTEDIT_CLEAR_SELECTION_II:
      key_event->accept();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_HOME:
    case taiMisc::TEXTEDIT_HOME_II:
      key_event->accept();
      home(ext_select_on);
      return;
    case taiMisc::TEXTEDIT_END:
    case taiMisc::TEXTEDIT_END_II:
      key_event->accept();
      end(ext_select_on);
      return;
    case taiMisc::TEXTEDIT_CURSOR_FORWARD:
    case taiMisc::TEXTEDIT_CURSOR_FORWARD_II:
      key_event->accept();
      cursorForward(ext_select_on, 1);
      return;
    case taiMisc::TEXTEDIT_CURSOR_BACKWARD:
    case taiMisc::TEXTEDIT_CURSOR_BACKWARD_II:
      key_event->accept();
      cursorBackward(ext_select_on, 1);
      return;
    case taiMisc::TEXTEDIT_DELETE:
    case taiMisc::TEXTEDIT_DELETE_II:
      key_event->accept();
      del();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_BACKSPACE:
    case taiMisc::TEXTEDIT_BACKSPACE_II:
      key_event->accept();
      backspace();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_DELETE_TO_END:
    case taiMisc::TEXTEDIT_DELETE_TO_END_II:
      key_event->accept();
      end(true);                // mark
      cut();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_SELECT_ALL:
    case taiMisc::TEXTEDIT_SELECT_ALL_II:
      key_event->accept();
      selectAll();
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
    case taiMisc::TEXTEDIT_UNDO:
    case taiMisc::TEXTEDIT_UNDO_II:
      key_event->accept();
      undo();
      return;
    case taiMisc::TEXTEDIT_COPY_CLEAR:
    case taiMisc::TEXTEDIT_COPY_CLEAR_II:
      key_event->accept();
      copy();
      clearExtSelection();
      return;
    case taiMisc::TEXTEDIT_WORD_FORWARD:
    case taiMisc::TEXTEDIT_WORD_FORWARD_II:
      key_event->accept();
      cursorWordForward(ext_select_on);
      return;
    case taiMisc::TEXTEDIT_WORD_BACKWARD:
    case taiMisc::TEXTEDIT_WORD_BACKWARD_II:
      key_event->accept();
      cursorWordBackward(ext_select_on);
      return;
    case taiMisc::TEXTEDIT_LOOKUP:
    case taiMisc::TEXTEDIT_LOOKUP_II:
      key_event->accept();
      doLookup();
      return;
    default:
      inherited::keyPressEvent(key_event);
  }
}

void iLineEdit::doLookup() {
  emit lookupKeyPressed(this);
}

void iLineEdit::CharEntered() {
  emit characterEntered(this);
}

void iLineEdit::CompletionDone() {
  emit completed(GetCompleter()->currentIndex());
}

void iLineEdit::wheelEvent(QWheelEvent * e)
{
  // actually, this is ok..
  inherited::wheelEvent(e);
  return;
}

void iLineEdit::emitReturnPressed()
{
  emit returnPressed();
}
