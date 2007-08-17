// Copyright, 1995-2005, Regents of the University of Colorado,
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

#include "ilineedit.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLayout>
#include <QPushButton>
#include <QPrintDialog>
#include <QPrinter>
#include <QShortcut>
#include <QSizePolicy>
#include <QTextEdit>
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
  QShortcut* sc = new QShortcut(QKeySequence(/*Qt::ALT +*/ Qt::CTRL + Qt::Key_U), this);
  sc->setContext(Qt::WidgetShortcut);
  connect(sc, SIGNAL(activated()), this, SLOT(editInEditor()));
}

void iLineEdit::editInEditor() {
  iTextEditDialog* dlg = new iTextEditDialog(); // no parent needed for modals
  // set to be ~3/4 of screen
  QDesktopWidget *d = QApplication::desktop();
  int primaryScreen = d->primaryScreen();
  QSize sz = d->availableGeometry(primaryScreen).size();
  int wd = (sz.width() * 3) / 4;
  int ht = (sz.height() * 3) / 4;
  if(wd > 640) wd = 640; 	// don't make it too wide..
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
  emit focusChanged(true);
}

void iLineEdit::focusOutEvent(QFocusEvent* ev) {
  inherited::focusOutEvent(ev);
  emit focusChanged(false);
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
//    setFocusPolicy(ClickFocus);
    pal.setColor(backgroundRole(), COLOR_RO_BACKGROUND);
  } else {
//    setFocusPolicy(StrongFocus);
    pal.setColor(backgroundRole(), 
      QApplication::palette(this).color(QPalette::Base));
  }
  setPalette(pal);
  update();
}


//////////////////////////////////
//  iTextEditDialog		//
//////////////////////////////////


iTextEditDialog::iTextEditDialog(bool readOnly_, QWidget* parent)
:inherited(parent)
{
  init(readOnly_);
}

void iTextEditDialog::init(bool readOnly_) {
  m_readOnly = readOnly_;
  QRect rect(QApplication::desktop()->screenGeometry(0));
  int wd = (rect.width() * 3) / 4;
  int ht = (rect.height() * 3) / 4;
  if(wd > 640) wd = 640; 	// don't make it too wide..
  this->resize(wd, ht);
  QVBoxLayout* layOuter = new QVBoxLayout(this);
  txtText = new QTextEdit(this);
  layOuter->addWidget(txtText);
  QHBoxLayout* layButtons = new QHBoxLayout();
  layButtons->setMargin(2);
  layButtons->setSpacing(4);
  layOuter->addLayout(layButtons);
  layButtons->addStretch();
  btnPrint = new QPushButton("&Print", this);
  layButtons->addWidget(btnPrint);
  layButtons->addSpacing(16);
  connect(btnPrint, SIGNAL(clicked()), this, SLOT(btnPrint_clicked()) );
  if (m_readOnly) {
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
  }
}
 
iTextEditDialog::~iTextEditDialog() {
}

void iTextEditDialog::btnPrint_clicked() {
  QPrinter pr;
  QPrintDialog pd(&pr, this);
  if (pd.exec() != QDialog::Accepted) return;
  // print ...
  txtText->document()->print(&pr);
}

void iTextEditDialog::setText(const QString& value) {
  txtText->setPlainText(value);
}
