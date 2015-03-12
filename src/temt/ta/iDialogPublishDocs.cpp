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

#include "iDialogPublishDocs.h"

#include <taiMisc>

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
#include <QStatusTipEvent>
#include <QCheckBox>

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

iDialogPublishDocs::iDialogPublishDocs(const char *repo_name, const char *proj_name)
//  : inherited(), nameEdit(0), descEdit(0), tagsEdit(0), statusBar(0), upload_project(0)
: inherited()
{
  // Size the dialog.
  resize(taiM->dialogSize(taiMisc::dlgSmall | taiMisc::dlgHor));
  setFont(taiM->dialogFont(taiM->ctrl_size));

  // Dialog title.
  QString title("Publish project to: ");
  title.append(repo_name);
  setWindowTitle(title);

  // Create a marginless layout to hold the status bar,
  // plus an inner layout (with margins) to hold everything else.
  QVBoxLayout *topVbox = new QVBoxLayout(this);
  QVBoxLayout *vbox = new QVBoxLayout; // inner
#if (QT_VERSION >= 0x040600) // QMargins didn't exist until Qt 4.6
  vbox->setContentsMargins(topVbox->contentsMargins());
  topVbox->setContentsMargins(0, 0, 0, 0);
#endif
  topVbox->addLayout(vbox);

  // Add a status bar for instructions.
  statusBar = new QStatusBar;
  topVbox->addWidget(statusBar);

  // All other widgets get added to the inner vbox.
  // Project name
  nameEdit = new QLineEdit;
  nameEdit->setEnabled(false);
  nameEdit->setStatusTip("");
  nameEdit->installEventFilter(this);
  
  QHBoxLayout* project_box = newHBox(vbox);
  addLabeledWidget(project_box, "Project &name:", nameEdit);
  
  versionEdit = new QLineEdit;
  versionEdit->setEnabled(true);
  versionEdit->setStatusTip("Current project version number e.g. 2.0.1");
  addLabeledWidget(project_box, "Version:", versionEdit);

  // upload project - do it now - default is true
  // rohrlich - 3/11/2015 - require project file when publishing
//  upload_project = new QCheckBox;
//  upload_project->setChecked(true);
//  upload_project->setStatusTip("You can upload the project when you publish or just create the wiki page and later upload the project. You can always upload a new version of the project");
//  addLabeledWidget(project_box, "Upload Project File", upload_project);

  // author
  authorEdit = new QLineEdit;
  authorEdit->setStatusTip("Enter name of primary author (set default in preferences)");
  authorEdit->installEventFilter(this);
  QHBoxLayout* author_box = newHBox(vbox);
  addLabeledWidget(author_box, "Project &author:", authorEdit);

  emailEdit = new QLineEdit;
  emailEdit->setStatusTip("Enter an email address for correspondence (set default in preferences) - typically author's email");
  emailEdit->installEventFilter(this);
  addLabeledWidget(author_box, "&Email:", emailEdit);

  // Tags
  tagsEdit = new QLineEdit;
  tagsEdit->setStatusTip("Instructions: Enter keywords (comma spearated) to help users find your project when searching or browsing");
  tagsEdit->installEventFilter(this);
  addLabeledWidget(newHBox(vbox), "&Keywords:", tagsEdit);
  
  // Description
  descEdit = new QTextEdit;
  descEdit->setTabChangesFocus(true);
  descEdit->setStatusTip("Instructions: Enter a brief description of the project (more detail can be added later on the wiki)");
  descEdit->installEventFilter(this);
  addLabeledWidget(vbox, "&Description:", descEdit);
  
  // OK, Cancel buttons
  QDialogButtonBox *buttonBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  vbox->addWidget(buttonBox);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString iDialogPublishDocs::GetName() const
{
  return nameEdit->text();
}

QString iDialogPublishDocs::GetAuthor() const
{
  return authorEdit->text();
}

QString iDialogPublishDocs::GetEmail() const
{
  return emailEdit->text();
}

QString iDialogPublishDocs::GetDesc() const
{
  return descEdit->toPlainText();
}

QString iDialogPublishDocs::GetTags() const
{
  QString tags = tagsEdit->text();
//  return tags.split(QRegExp("[,\\s]+"), QString::SkipEmptyParts);
  return tags;
}

QString iDialogPublishDocs::GetVersion() const
{
  return versionEdit->text();
}

bool iDialogPublishDocs::GetUploadChoice() const
{
  return upload_project->isChecked();
}

bool iDialogPublishDocs::event(QEvent *event)
{
  // When a status tip event occurs (e.g., from mouseover), display the tip.
  if (event->type() == QEvent::StatusTip) {
    if (QStatusTipEvent *stev = dynamic_cast<QStatusTipEvent *>(event)) {
      statusBar->showMessage(stev->tip());
      return true;
    }
  }

  // Other events, let the parent handle them.
  return inherited::event(event);
}

bool iDialogPublishDocs::eventFilter(QObject *obj, QEvent *event)
{
  // When a widget (obj) comes into focus, display its status tip.
  if (event->type() == QEvent::FocusIn) {
    if (QWidget *widget = dynamic_cast<QWidget *>(obj)) {
      statusBar->showMessage(widget->statusTip());
    }
  }
  else if (event->type() == QEvent::FocusOut) {
    statusBar->clearMessage();
  }

  // Always return false, so the event will get processed further.
  return false;
}

void iDialogPublishDocs::SetName(const QString& name) {
  nameEdit->setText(name);
}

void iDialogPublishDocs::SetAuthor(const QString& author) {
  authorEdit->setText(author);
}

void iDialogPublishDocs::SetEmail(const QString& email) {
  emailEdit->setText(email);
}

void iDialogPublishDocs::SetDesc(const QString& desc) {
  descEdit->setText(desc);
}

void iDialogPublishDocs::SetTags(const QString& tags) {
  tagsEdit->setText(tags);
}

void iDialogPublishDocs::SetVersion(const QString& version) {
  versionEdit->setText(version);
}

