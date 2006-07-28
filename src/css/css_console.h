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


#ifndef CSS_CONSOLE_H
#define CSS_CONSOLE_H

// css_console.h -- abstraction of console (and readline)

#include "ta_string.h"
#include "css_machine.h"

#include <QObject>

class CSS_API cssQandDConsole: public QObject {
  // quick-and-dirty console
INHERITED(QObject)
  Q_OBJECT
public:
  static cssQandDConsole*	Get_SysConsole(QObject* parent = NULL);
    // get the system console instance, instantiating if necessary

  virtual const String	prompt() = 0;
  virtual void		setPrompt(const String& value) = 0;
  
  virtual void		Start() {} // call when prompt set, and ready to receive input
  
  cssQandDConsole(QObject* parent = NULL);
  ~cssQandDConsole();
  
signals:
  void			NewLine(QString ln, bool eof); 
  // YOU MUST CONNECT EXPLICITLY VIA Qt::QueuedConnection 
  
protected:
  static cssQandDConsole*	m_sys_instance;
  
  static cssQandDConsole*	New_SysConsole(QObject* parent = NULL);
};

#include "qconsole.h"

class CSS_API QcssConsole : public QConsole {
  INHERITED(QConsole);
  Q_OBJECT;
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

  cssCmdShell* 	cmd_shell; // the command shell
  static QcssConsole *theInstance;
};



#endif

