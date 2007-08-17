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


#ifndef CSS_CONSOLE_H
#define CSS_CONSOLE_H

// css_console.h -- abstraction of console (and readline)

#include "css_machine.h"

#include "ta_string.h"

#include <QObject>

class ConThread; // #IGNORE in .cpp


class CSS_API cssConsole: public QObject {
  // generic class that represents the console, if used
INHERITED(QObject)
  Q_OBJECT
friend class ConThread;
public:
  static cssConsole*	Get_SysConsole(QObject* parent = NULL);
    // get the system console instance, instantiating if necessary

  virtual const QString	prompt(); // note: QStrings used for threadsafety
  virtual void		setPrompt(const QString& value);
  virtual void		setTitle(const QString& value) {} // ex. for windows console
  
  virtual void		Start(); // call when prompt set, and ready to receive input
  
  cssConsole(QObject* parent = NULL);
  ~cssConsole();
  
signals:
  void			NewLine(QString ln, bool eof); 
  // YOU MUST CONNECT EXPLICITLY VIA Qt::QueuedConnection 
  
protected:
  ConThread*	  thread;
  static cssConsole*	m_sys_instance;
  
  static cssConsole*	New_SysConsole(QObject* parent = NULL);

protected: // thread
  void				emit_NewLine(String ln, bool eof);

};

#endif

