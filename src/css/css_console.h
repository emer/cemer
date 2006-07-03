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

#include "css_def.h"

#include <QObject>

extern "C" { // basic readline interface -- either refers to readline, or we fake it
  extern int rl_done;		// readline done reading
  extern int rl_pending_input;
  extern int (*rl_event_hook)(void); // note: we don't hook anymore...
  extern char* readline(char*);
  extern void add_history(char*);
  extern int rl_stuff_char(int);
}



class CSS_API cssConsole: public QObject {
  // abstract base definition of a console
INHERITED(QObject)
  Q_OBJECT
public:
  static cssConsole*	Get_SysConsole(QObject* parent = NULL);
    // get the system console instance, instantiating if necessary

  virtual const String	prompt() = 0;
  virtual void		setPrompt(const String& value) = 0;
  
  virtual void		Start() {} // call when prompt set, and ready to receive input
  
  cssConsole(QObject* parent = NULL);
  ~cssConsole();
  
signals:
  void			NewLine(QString ln, bool eof); 
    // YOU MUST CONNECT EXPLICITLY VIA Qt::QueuedConnection 
  
protected:
  static cssConsole*	m_sys_instance;
  
  static cssConsole*	New_SysConsole(QObject* parent = NULL);
};


#endif

