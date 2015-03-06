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

#ifndef iDialogPublishDocs_h
#define iDialogPublishDocs_h

// Note: maketa does not run on this file.

#include <iDialog>
#include "ta_def.h" // TA_API

class QEvent;
class QLineEdit;
class QObject;
class QStatusBar;
class QString;
class QStringList;
class QTextEdit;
class QCheckBox;

class TA_API iDialogPublishDocs : public iDialog {
// This dialog is invoked when user selects File -> PublishProjectOnWeb
  Q_OBJECT
  INHERITED(iDialog)
public:
  iDialogPublishDocs(const char *repo_name, const char *proj_name);
  QString     GetName() const;
  QString     GetAuthor() const;
  QString     GetEmail() const;
  QString     GetDesc() const;
  QString     GetTags() const;
  bool        GetUploadChoice() const;
  
  void        SetName(const QString& name);
  void        SetAuthor(const QString& author);
  void        SetEmail(const QString& email);
  void        SetDesc(const QString& desc);
  void        SetTags(const QString& tags);

private:
  bool        event(QEvent *event) override;
  bool        eventFilter(QObject *obj, QEvent *event) override;

  QLineEdit*  nameEdit;
  QLineEdit*  authorEdit;
  QLineEdit*  emailEdit;
  QTextEdit*  descEdit;
  QLineEdit*  tagsEdit;
  QStatusBar* statusBar;
  QCheckBox*  upload_project;
};

#endif // iDialogPublishDocs_h
