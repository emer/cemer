// Co2018ght 2012-2017, Regents of the University of Colorado,
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
#include <iTextEdit>

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

iDialogPublishDocs::iDialogPublishDocs(const QString& repo_name, const QString& proj_name, bool new_pub, const QString& pub_type)
: inherited()
, publish_type(pub_type)
{
  // Size the dialog.
  resize(taiM->dialogSize(taiMisc::dlgSmall | taiMisc::dlgHor));
  setFont(taiM->dialogFont(taiM->ctrl_size));

  // Dialog title
  QString title;
  if (new_pub) {
    title = "Publish ";
    title.append(publish_type);
    title.append("  to: ");
    title.append(repo_name);
  }
  else {
    String tmp(proj_name);
    title = "Update published ";
    title.append(publish_type);
    title.append(" '");
    title.append(proj_name);
    title.append("'");
  }
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
  QString tmp = QString(publish_type);
  tmp.append(" &name:");
  addLabeledWidget(project_box, tmp, nameEdit);
  
  versionEdit = new QLineEdit;
  versionEdit->setEnabled(true);
  tmp = QString("Current");
  tmp.append(publish_type);
  tmp.append(" version number e.g. 2.0.1");
  versionEdit->setStatusTip(tmp);
  addLabeledWidget(project_box, "Version:", versionEdit);

  if (new_pub) {
    // page name
    pgnameEdit = new QLineEdit;
    pgnameEdit->setStatusTip("Instructions: Enter the full name of wiki page to use for documenting this object -- for Programs this should always just be the program name)");
    pgnameEdit->installEventFilter(this);
    addLabeledWidget(vbox, "&Page Name:", pgnameEdit);

    authorEdit = new QLineEdit;
    authorEdit->setStatusTip("Enter name of primary author (set default in preferences)");
    authorEdit->installEventFilter(this);
    QHBoxLayout* author_box = newHBox(vbox);
    tmp = QString(publish_type);
    tmp.append(" &author:");
    addLabeledWidget(author_box, tmp, authorEdit);
    
    emailEdit = new QLineEdit;
    emailEdit->setStatusTip("Enter an email address for correspondence (set default in preferences) - typically author's email");
    emailEdit->installEventFilter(this);
    addLabeledWidget(author_box, "&Email:", emailEdit);

    if(publish_type == "Project") {
      pubCiteEdit = new QLineEdit;
      pubCiteEdit->setStatusTip("for a project that has been described in a publication, this is the citation key for that publication, in the form of Author1[Author2][Author3][EtAl]YY where Author is last name of given author -- e.g., OReillyMunakataFrankEtAl12 is the citation key for the online wiki textbook: http://ccnbook.colorado.edu");
      pubCiteEdit->installEventFilter(this);
      addLabeledWidget(author_box, "&PubCite:", pubCiteEdit);
    }
    
    // Tags
    tagsEdit = new QLineEdit;
    tagsEdit->setStatusTip("Instructions: Enter tags (comma spearated) to help users find your project when searching or browsing");
    tagsEdit->installEventFilter(this);
    addLabeledWidget(newHBox(vbox), "&Tags:", tagsEdit);
    
    // Description
    descEdit = new iTextEdit;
    descEdit->setTabChangesFocus(true);
    descEdit->setStatusTip("Instructions: Enter a brief description of the project (more detail can be added later on the wiki)");
    descEdit->installEventFilter(this);
    addLabeledWidget(vbox, "&Description:", descEdit);

    // create doc
    if(publish_type == "Project") {
      create_doc = new QCheckBox;
      create_doc->setChecked(true); // start off true by default -- no harm done if not needed
      create_doc->setToolTip(taiMisc::ToolTipPreProcess("Create a new Doc in docs called ProjectDocs that points to the new wiki page for this project?"));
      addLabeledWidget(vbox, "&Create ProjectDocs:", create_doc);
    }
  }
  
  // OK, Cancel buttons
  QDialogButtonBox *buttonBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  vbox->addWidget(buttonBox);
  if (!new_pub) {
    this->adjustSize();
  }
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

QString iDialogPublishDocs::GetPageName() const
{
  return pgnameEdit->text();
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

QString iDialogPublishDocs::GetPubCite() const
{
  return pubCiteEdit->text();
}

bool iDialogPublishDocs::GetCreateDoc() const
{
  return create_doc->isChecked();
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

void iDialogPublishDocs::SetPageName(const QString& page_name) {
  pgnameEdit->setText(page_name);
}

void iDialogPublishDocs::SetTags(const QString& tags) {
  tagsEdit->setText(tags);
}

void iDialogPublishDocs::SetVersion(const QString& version) {
  versionEdit->setText(version);
}

void iDialogPublishDocs::SetPubCite(const QString& pub_cite) {
  pubCiteEdit->setText(pub_cite);
}

