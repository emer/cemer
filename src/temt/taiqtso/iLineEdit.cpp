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

#include "iLineEdit.h"

#include <iTextEditDialog>

#include <taiMisc>
#include <QDesktopWidget>
#include <QPalette>


iLineEdit::iLineEdit(QWidget* parent)
: QLineEdit(parent)
{
  init();
}

iLineEdit::iLineEdit(const char* text, QWidget* parent)
: QLineEdit(QString(text), parent)
{
  init();
}

void iLineEdit::init() {
  mmin_char_width = 0;
  mchar_width = 0;
  ext_select_on = false;
  // this seems unnecessary, and conflicts with ctrl-U select-all!
//   QShortcut* sc = new QShortcut(QKeySequence(/*Qt::ALT +*/ Qt::CTRL + Qt::Key_U), this);
//   sc->setContext(Qt::WidgetShortcut);
//   connect(sc, SIGNAL(activated()), this, SLOT(editInEditor()));
}

void iLineEdit::editInEditor() {
  iTextEditDialog* dlg = new iTextEditDialog(); // no parent needed for modals
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

void iLineEdit::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  // activateWindow();          // make sure we're active when we click in a box!
  // std::cerr << "focus in" << std::endl;
  // emit focusChanged(true);
}

void iLineEdit::focusOutEvent(QFocusEvent* ev) {
  inherited::focusOutEvent(ev);
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
      s.append(QChar('0' + (i % 10)));
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
    pal.setColor(backgroundRole(), COLOR_RO_BACKGROUND);
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

void iLineEdit::keyPressEvent(QKeyEvent* e)
{
  // std::cerr << "keypress" << std::endl;

  // emacs keys!!
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if (ctrl_pressed) {
    switch (e->key()) {
    case Qt::Key_Space:
      e->accept();
      deselect();
      ext_select_on = true;
      return;
    case Qt::Key_G:
      e->accept();
      clearExtSelection();
      return;
    case Qt::Key_A:
      e->accept();
      home(ext_select_on);
      return;
    case Qt::Key_E:
      e->accept();
      end(ext_select_on);
      return;
    case Qt::Key_F:
      e->accept();
      cursorForward(ext_select_on, 1);
      return;
    case Qt::Key_B:
      e->accept();
      cursorBackward(ext_select_on, 1);
      return;
    case Qt::Key_D:
      e->accept();
      del();
      clearExtSelection();
      return;
    case Qt::Key_K:
      e->accept();
      end(true);                // mark
      cut();
      clearExtSelection();
      return;
    case Qt::Key_U:
      e->accept();
      selectAll();
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
    }
  }
  else if (e->modifiers() & Qt::AltModifier) {
    if (e->key() == Qt::Key_W) { // copy
      e->accept();
      copy();
      clearExtSelection();
      return;
    }
  }

  // Esc interferes with dialog cancel and other such things
  // if(e->key() == Qt::Key_Escape) {
  //   e->ignore();             // fake ignore so that any dialog ops will happen as expected
  //   clearExtSelection();
  //   return;
  // }

  QLineEdit::keyPressEvent( e );
}

void iLineEdit::wheelEvent(QWheelEvent * event)
{
  // actually, this is ok..
  inherited::wheelEvent(event);
  return;
}

void iLineEdit::emitReturnPressed()
{
  emit returnPressed();
}
