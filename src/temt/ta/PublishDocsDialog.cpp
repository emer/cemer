// Copyright, 1995-2012, Regents of the University of Colorado,
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

#include "PublishDocsDialog.h"

#include <QDialogButtonBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QStringList>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "ta_qt.h" // taiM, taiMisc

namespace // anon
{
  // Add a label and a widget to a layout as buddies.
  void addLabeledWidget(QLayout *layout, const QString &labelText, QWidget *widget)
  {
    QLabel *label = new QLabel(labelText);
    label->setBuddy(widget);
    layout->addWidget(label);
    layout->addWidget(widget);
  }

  // Create and return a new hbox inside the given vbox.
  QHBoxLayout * newHBox(QVBoxLayout *vbox)
  {
    QHBoxLayout *hbox = new QHBoxLayout;
    vbox->addLayout(hbox);
    return hbox;
  }
}

PublishDocsDialog::PublishDocsDialog(const char *repositoryName)
  : inherited()
  , m_nameEdit(0)
  , m_descEdit(0)
  , m_tagsEdit(0)
  , m_statusBar(0)
{
  // Size the dialog.
  resize(taiM->dialogSize(taiMisc::dlgSmall | taiMisc::dlgHor));
  setFont(taiM->dialogFont(taiM->ctrl_size));

  // Dialog title.
  QString title("Publish project documentation to: ");
  title.append(repositoryName);
  setWindowTitle(title);

  // Create a marginless layout to hold the status bar,
  // plus an inner layout (with margins) to hold everything else.
  QVBoxLayout *topVbox = new QVBoxLayout(this);
  QVBoxLayout *vbox = new QVBoxLayout; // inner
  vbox->setContentsMargins(topVbox->contentsMargins());
  topVbox->setContentsMargins(0, 0, 0, 0);
  topVbox->addLayout(vbox);

  // Add a status bar for instructions.
  m_statusBar = new QStatusBar;
  topVbox->addWidget(m_statusBar);

  // All other widgets get added to the inner vbox.
  // Project name
  m_nameEdit = new QLineEdit;
  m_nameEdit->setStatusTip("Instructions: Enter a human-readable name for the project (used for wiki page name)");
  m_nameEdit->installEventFilter(this);
  addLabeledWidget(newHBox(vbox), "Project &name:", m_nameEdit);

  // Description
  m_descEdit = new QTextEdit;
  m_descEdit->setTabChangesFocus(true);
  m_descEdit->setStatusTip("Instructions: Enter a brief description of the project (more detail can be added later on the wiki)");
  m_descEdit->installEventFilter(this);
  addLabeledWidget(vbox, "&Description:", m_descEdit);

  // Tags
  m_tagsEdit = new QLineEdit;
  m_tagsEdit->setStatusTip("Instructions: Enter tags relevant to this project (comma or space spearated)");
  m_tagsEdit->installEventFilter(this);
  addLabeledWidget(newHBox(vbox), "&Tags:", m_tagsEdit);

  // OK, Cancel buttons
  QDialogButtonBox *buttonBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  vbox->addWidget(buttonBox);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString PublishDocsDialog::getName() const
{
  return m_nameEdit->text();
}

QString PublishDocsDialog::getDesc() const
{
  return m_descEdit->toPlainText();
}

QStringList PublishDocsDialog::getTags() const
{
  QString tags = m_tagsEdit->text();
  return tags.split(QRegExp("[,\\s]+"), QString::SkipEmptyParts);
}

bool PublishDocsDialog::event(QEvent *event)
{
  // When a status tip event occurs (e.g., from mouseover), display the tip.
  if (event->type() == QEvent::StatusTip) {
    if (QStatusTipEvent *stev = dynamic_cast<QStatusTipEvent *>(event)) {
      m_statusBar->showMessage(stev->tip());
      return true;
    }
  }

  // Other events, let the parent handle them.
  return inherited::event(event);
}

bool PublishDocsDialog::eventFilter(QObject *obj, QEvent *event)
{
  // When a widget (obj) comes into focus, display its status tip.
  if (event->type() == QEvent::FocusIn) {
    if (QWidget *widget = dynamic_cast<QWidget *>(obj)) {
      m_statusBar->showMessage(widget->statusTip());
    }
  }
  else if (event->type() == QEvent::FocusOut) {
    m_statusBar->clearMessage();
  }

  // Always return false, so the event will get processed further.
  return false;
}
