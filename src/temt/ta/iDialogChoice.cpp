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

#include "iDialogChoice.h"


#include <taMisc>
#include <taiMisc>

#include <QApplication>
#include <QButtonGroup>
#include <QPushButton>
#include <QClipboard>
#include <QKeyEvent>

#include <iDialog>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QSpacerItem>
#include <QStyle>
#include <QHBoxLayout>

const String iDialogChoice::delimiter("!|");

int iDialogChoice::ChoiceDialog(QWidget* parent_, const String& msg,
  const String& but_list, const char* title)
{
  iDialogChoice* dlg = new iDialogChoice(Question, QString(title),
    msg.toQString(), but_list,  parent_, false);
  // show the dialog
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  dlg->setIcon(QMessageBox::Question);
  dlg->setWindowModality(Qt::ApplicationModal);
  dlg->resize( taiM->dialogSize(taiMisc::dlgMedium | taiMisc::dlgVer) );
  int rval = dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
  return rval;
}

int iDialogChoice::ChoiceDialogSepBut(QWidget* parent_, const String& msg,
   const String& but_list_sep, const String& but_list, const char* title)
{
  iDialog* dlg = new iDialogChoiceSepBut(Question, QString(title),
                               msg.toQString(), but_list_sep, but_list, parent_);
  // show the dialog
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  // dlg->setIcon(QMessageBox::Question);
  dlg->setWindowModality(Qt::ApplicationModal);
  // dlg->resize( taiM->dialogSize(taiMisc::dlgMedium | taiMisc::dlgVer) );
  int rval = dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
  return rval;
}


bool iDialogChoice::ErrorDialog(QWidget* parent_, const char* msg,
  const char* title, bool copy_but_, bool cancel_errs_but_)
{
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  String buts;
  if(cancel_errs_but_)
    buts = "Cancel Remaining Error Dialogs" + delimiter + "OK";
  iDialogChoice* dlg = new iDialogChoice(Warning, QString(title),
    QString(msg), buts, parent_, copy_but_);
  dlg->setIcon(QMessageBox::Critical);
  dlg->setWindowModality(Qt::ApplicationModal);
  int chs = dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
  return chs == 0;              // true if cancel
}

void iDialogChoice::ConfirmDialog(QWidget* parent_, const char* msg,
  const char* title, bool copy_but_)
{
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  iDialogChoice* dlg = new iDialogChoice(Information, QString(title),
    QString(msg), "", parent_, copy_but_);
  dlg->setIcon(QMessageBox::Question);
  dlg->setWindowModality(Qt::ApplicationModal);
  dlg->resize( taiM->dialogSize(taiMisc::dlgMedium | taiMisc::dlgVer) );
  dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
}

iDialogChoice::iDialogChoice(Icon icon, const QString& title,
  const QString& text, String but_list, QWidget* parent,
  bool copy_but_)
:inherited(icon, title, text, NoButton, parent)
{
  if (title.isEmpty()) {
    setWindowTitle(QCoreApplication::instance()->applicationName());
  }
  bgChoiceButtons = new QButtonGroup(this); // note: not a widget, invisible
  bgChoiceButtons->setExclusive(false); // not really applicable

  // create buttons
  String_PArray sa;
  if (but_list.contains(delimiter)) { // has options encoded within prompt string
    // strip leading/trailing delims, and parse all at once, to get count now
    if (but_list.startsWith(delimiter))
      but_list = but_list.after(delimiter);
    if (but_list.endsWith(delimiter))
      but_list = but_list.left(but_list.length() - delimiter.length());
    sa.SetFromString(but_list, delimiter);
  }
  // we always have at least an Ok button
  if (sa.size == 0) {
    sa.Add("&Ok");
  }
  num_chs = sa.size;

  for (int i = 0; i < sa.size; ++i) {
    ButtonRole role = AcceptRole;
    if ((i > 0) && (i == (sa.size - 1)))
      role = RejectRole;
    Constr_OneBut(sa[i], i, role);
  }

  if (copy_but_) {
    // damn copy button
    QAbstractButton* but = Constr_OneBut("Copy to clipboard", -2, ActionRole);
    connect(but, SIGNAL(clicked()), this, SLOT(copyToClipboard()));
  }
}

void iDialogChoice::done(int r) {
  QAbstractButton* but = clickedButton();
  int id = -1;
  if (but)
    id = bgChoiceButtons->id(but);
  if (id < -1) return; // ignore
  // if user goes Esc, it returns -1 -- we want our last button instead
  if ((id == -1) || (r < 0))
    r = num_chs - 1;
  inherited::done(r);
}

int iDialogChoice::exec() {
  // we're losing focus here so need to restore it!!
// #ifdef TA_OS_MAC
  QPointer<QWidget> m_prev_active = QApplication::activeWindow();
// #endif
  int rval = inherited::exec();
  //#ifdef TA_OS_MAC
  if((bool)m_prev_active) {
//     QApplication::setActiveWindow(m_prev_active);
    // note: above does NOT work! -- likely source of bug in cocoa 4.6.0
    m_prev_active->activateWindow();
  }
// #endif
  return rval;
}

void iDialogChoice::copyToClipboard() {
  QApplication::clipboard()->setText(text());
}

QAbstractButton* iDialogChoice::Constr_OneBut(String lbl, int id, ButtonRole role) {
  if (lbl.startsWith(" ")) // allow for one space..
    lbl = lbl.after(' ');
  if (lbl.empty())
    return NULL; // not really supposed to happen...

  QPushButton* but = new QPushButton(lbl, this);
  addButton(but, role);
  bgChoiceButtons->addButton(but, id);
  return but;
}

void iDialogChoice::keyPressEvent(QKeyEvent* ev) {
  // we allow 0-n numeric keys to be accelerators for the buttons
  int key_code = ev->key();
  if ((key_code >= Qt::Key_0) && (key_code <= Qt::Key_9)) {
    int but_index = key_code - Qt::Key_0;
    QPushButton* but = (QPushButton*)bgChoiceButtons->button(but_index);
    if (but != NULL) {
      // simulate effect of pressing the button
      but->click();
      ev->accept(); // swallow key
    }
  } else {
    QDialog::keyPressEvent(ev);
  }
}

/////////////////////////////////

const String iDialogChoiceSepBut::delimiter("!|");

iDialogChoiceSepBut::iDialogChoiceSepBut(QMessageBox::Icon icon, const QString& title,
  const QString& text, String but_list_sep, String but_list, QWidget* parent)
:inherited(parent)
{
  // from qmessagebox.cpp:
  label = new QLabel(this);
  label->setObjectName(QLatin1String("qt_msgbox_label"));
  label->setTextInteractionFlags(Qt::TextInteractionFlags(style()->styleHint(QStyle::SH_MessageBox_TextInteractionFlags, 0, this)));
  label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  label->setOpenExternalLinks(true);

  QIcon tmpIcon = style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, this);
  int iconSize = style()->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, this);

  iconLabel = new QLabel(this);
  iconLabel->setObjectName(QLatin1String("qt_msgboxex_icon_label"));
  iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  iconLabel->setPixmap(tmpIcon.pixmap(iconSize, iconSize));

  buttonBox = new QDialogButtonBox(this);
  buttonBox->setObjectName(QLatin1String("qt_msgbox_buttonbox"));
  buttonBox->setCenterButtons(style()->styleHint(QStyle::SH_MessageBox_CenterButtons, 0, this));
  sep_buts = new QWidget(this);
  sep_but_lay = new QHBoxLayout(sep_buts);

  QGridLayout *grid = new QGridLayout;
  bool hasIcon = iconLabel->pixmap() && !iconLabel->pixmap()->isNull();

  if (hasIcon)
    grid->addWidget(iconLabel, 0, 0, 2, 1, Qt::AlignTop);
  iconLabel->setVisible(hasIcon);
#ifdef TA_OS_MAC
  QSpacerItem *indentSpacer = new QSpacerItem(14, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
#else
  QSpacerItem *indentSpacer = new QSpacerItem(hasIcon ? 7 : 15, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
#endif
  grid->addItem(indentSpacer, 0, hasIcon ? 1 : 0, 2, 1);
  grid->addWidget(label, 0, hasIcon ? 2 : 1, 1, 1);
#ifdef TA_OS_MAC
  grid->addWidget(sep_buts, grid->rowCount(), hasIcon ? 2 : 1, 1, 1);
  grid->addWidget(buttonBox, grid->rowCount(), hasIcon ? 2 : 1, 1, 1);
  grid->setMargin(0);
  grid->setVerticalSpacing(8);
  grid->setHorizontalSpacing(0);
  setContentsMargins(24, 15, 24, 20);
  grid->setRowStretch(1, 100);
  grid->setRowMinimumHeight(2, 6);
#else
  grid->addWidget(sep_buts, grid->rowCount(), 0, 1, grid->columnCount());
  grid->addWidget(buttonBox, grid->rowCount(), 0, 1, grid->columnCount());
#endif
  grid->setSizeConstraint(QLayout::SetNoConstraint);
  setLayout(grid);

  if (!title.isEmpty() || !text.isEmpty()) {
    setWindowTitle(title);
    label->setText(text);
    // label->setWordWrap(label->textFormat() == Qt::RichText
    //     || (label->textFormat() == Qt::AutoText && Qt::mightBeRichText(text)));
  }
  setModal(true);
#ifdef TA_OS_MAC
  QFont f = font();
  f.setBold(true);
  label->setFont(f);
#endif

  if (title.isEmpty()) {
    setWindowTitle(QCoreApplication::instance()->applicationName());
  }
  bgChoiceButtons = new QButtonGroup(this); // note: not a widget, invisible
  bgChoiceButtons->setExclusive(false); // not really applicable

  QObject::connect(bgChoiceButtons, SIGNAL(buttonClicked(int)),
                   this, SLOT(dialogButtonClicked(int)));
  // direct it to return value!

  // create buttons
  String_PArray sas;
  if (but_list_sep.contains(delimiter)) { // has options encoded within prompt string
    // strip leading/trailing delims, and parse all at once, to get count now
    if (but_list_sep.startsWith(delimiter))
      but_list_sep = but_list_sep.after(delimiter);
    if (but_list_sep.endsWith(delimiter))
      but_list_sep = but_list_sep.left(but_list_sep.length() - delimiter.length());
    sas.SetFromString(but_list_sep, delimiter);
  }
  num_chs = sas.size;

  // create buttons
  String_PArray sa;
  if (but_list.contains(delimiter)) { // has options encoded within prompt string
    // strip leading/trailing delims, and parse all at once, to get count now
    if (but_list.startsWith(delimiter))
      but_list = but_list.after(delimiter);
    if (but_list.endsWith(delimiter))
      but_list = but_list.left(but_list.length() - delimiter.length());
    sa.SetFromString(but_list, delimiter);
  }
  // we always have at least an Ok button
  if (sa.size == 0) {
    sa.Add("&Ok");
  }
  num_chs += sa.size;

  for (int i = 0; i < sa.size; ++i) {
    QMessageBox::ButtonRole role = QMessageBox::AcceptRole;
    if ((i > 0) && (i == (sa.size - 1)))
      role = QMessageBox::RejectRole;
    Constr_DlgBut(sa[i], i, role);
  }

  for (int i = 0; i < sas.size; ++i) {
    QAbstractButton* but = Constr_SepBut(sas[i], sa.size+i);
    if(i==0)
      but->setFocus();
  }
}

iDialogChoiceSepBut::~iDialogChoiceSepBut() {
}

QAbstractButton* iDialogChoiceSepBut::Constr_DlgBut(String lbl, int id, QMessageBox::ButtonRole role) {
  if (lbl.startsWith(" ")) // allow for one space..
    lbl = lbl.after(' ');
  if (lbl.empty())
    return NULL; // not really supposed to happen...

  QPushButton* but = new QPushButton(lbl, this);
  buttonBox->addButton(but, (QDialogButtonBox::ButtonRole)role);
  customButtonList.append(but);
  bgChoiceButtons->addButton(but, id);
  return but;
}

QAbstractButton* iDialogChoiceSepBut::Constr_SepBut(String lbl, int id) {
  if (lbl.startsWith(" ")) // allow for one space..
    lbl = lbl.after(' ');
  if (lbl.empty())
    return NULL; // not really supposed to happen...

  QPushButton* but = new QPushButton(lbl, this);
  sep_but_lay->addWidget(but);
  customButtonList.append(but);
  bgChoiceButtons->addButton(but, id);
  return but;
}

void iDialogChoiceSepBut::dialogButtonClicked(int id) {
  done(id);
}

