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

#ifndef PUBLISHDOCSDIALOG_H
#define PUBLISHDOCSDIALOG_H

// Note: maketa does not run on this file.

#include "idialog.h"
#include "ta_def.h" // TA_API

class QEvent;
class QLineEdit;
class QObject;
class QStatusBar;
class QString;
class QStringList;
class QTextEdit;

// This dialog is invoked when user clicks
// "File -> Publish Project Docs on Web".
class TA_API PublishDocsDialog : public iDialog
{
  Q_OBJECT
  INHERITED(iDialog)

public:
  PublishDocsDialog(const char *repositoryName);
  QString getName() const;
  QString getDesc() const;
  QStringList getTags() const;

private:
  override bool event(QEvent *event);
  override bool eventFilter(QObject *obj, QEvent *event);

  QLineEdit *m_nameEdit;
  QTextEdit *m_descEdit;
  QLineEdit *m_tagsEdit;
  QStatusBar *m_statusBar;
};

#endif // PUBLISHDOCSDIALOG_H
