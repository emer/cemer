// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


#ifndef CSS_QTCONSOLE_H
#define CSS_QTCONSOLE_H

// css_qtconsole.h -- abstraction of qt-based console (unix only)

#include "css_console.h"

#include "qconsole.h"

class CSS_API QcssConsole : public QConsole {
  INHERITED(QConsole)
  Q_OBJECT
 public:

  static QcssConsole* getInstance(QObject *parent = NULL, cssCmdShell* cs = NULL);

  ~QcssConsole();
  QcssConsole(QObject* parent = NULL, cssCmdShell* cs = NULL);

 protected:
  static int autocompletePath(String cmd_b4, String cmd, QStringList& lst);
  static int autocompleteScoped(String cmd_b4, String cmd, QStringList& lst);
  static int autocompleteKeyword(String cmd_b4, String cmd, QStringList& lst);

  override QString interpretCommand(QString command, int* res);
  override QStringList autocompleteCommand(QString cmd);
  override void ctrlCPressed();

  override void keyPressEvent(QKeyEvent * e);
  override void resizeEvent(QResizeEvent* e);

  cssCmdShell* 	cmd_shell; // the command shell
  static QcssConsole *theInstance;
};

#endif

