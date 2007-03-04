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


#include "css_console.h"
#include "css_basic_types.h"
#include "css_builtin.h"
#include "css_ta.h"

#include <QThread>

extern "C" {
  extern char* rl_readline(char*);
  extern void add_history(char*);
  extern int rl_done;		// readline done reading
}

//////////////////////////
//   ConThread		//
//////////////////////////

class ConThread: public QThread { // #IGNORE used to get input so input doesn't block events
INHERITED(QThread)
public:
  QString		prompt; // we use a QString because it is threadsafe
  cssConsole*	con;
  
  void			stop(); // (main) safe way to tell it to stop
  
  ConThread(cssConsole* parent);

protected:
  override void 	run(); // OS-dependent
};


//////////////////////////
//   cssConsole	//
//////////////////////////

cssConsole* cssConsole::m_sys_instance = NULL;

cssConsole* cssConsole::Get_SysConsole(QObject* parent) {
 if (!m_sys_instance) {
   m_sys_instance = New_SysConsole(parent);
 }
 return m_sys_instance;
}

cssConsole::cssConsole(QObject* parent)
:inherited(parent)
{
  thread = new ConThread(this);
}

cssConsole::~cssConsole() {
  if (thread) {
    thread->con = NULL;
    thread->stop();
    thread = NULL;
  }
  if (this == m_sys_instance)
    m_sys_instance = NULL;
}

void cssConsole::emit_NewLine(String ln, bool eof) {
//**called from thread
    emit NewLine(ln, eof);
}

const QString cssConsole::prompt() {
  if (thread) 
    return thread->prompt;
  else 
    return QString();
}

void cssConsole::setPrompt(const QString& value) {
  if (thread) {
    if (thread->prompt == value) return; // test is threadsafe/threadcorrect
    //TODO: try to erase last prompt if any
    thread->prompt = value; // threadsafe
    cout << value.latin1();
  }
}

void cssConsole::Start() {
    if (!thread) return;
    //TODO: maybe need to check not already started???
    thread->start();
} 


//////////////////////////
//   ConThread		//
//////////////////////////

ConThread::ConThread(cssConsole* parent)
:inherited(parent)
{
  con = parent;
}

void ConThread::run() {
  QString lprompt; // local, so latin1() is threadsafe
  String curln;
  bool eof;
  char* curln_;
  while (con) { // con null'ed can indicate stop needed
    lprompt = prompt;
    //NOTE: according to rl spec, we must call free() on the string returned
    curln_ = rl_readline(const_cast<char*>(lprompt.latin1())); // rl doesn't change it, so safe
    eof = (!curln_);
    if (eof) {
      curln = _nilString;
    } else {
      curln = curln_;
      free(curln_); 
      curln_ = NULL;
    }
    if (con)
      con->emit_NewLine(curln, eof);
  }
}

void ConThread::stop() { // (main)
//TODO: maybe this is too brutal??? 
  terminate(); 
}



#ifdef TA_OS_UNIX


cssConsole* cssConsole::New_SysConsole(QObject* parent) {
 cssConsole* rval = new cssConsole(parent);
 return rval;
}


#endif  TA_OS_UNIX


