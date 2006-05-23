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

#include <QThread>

cssConsole*	cssConsole::m_sys_instance = NULL;

cssConsole* cssConsole::Get_SysConsole(QObject* parent) {
 if (!m_sys_instance) {
   m_sys_instance = New_SysConsole(parent);
 }
 return m_sys_instance;
}

cssConsole::cssConsole(QObject* parent)
:inherited(parent)
{
}

cssConsole::~cssConsole() {
  if (this == m_sys_instance)
    m_sys_instance = NULL;
}

#ifdef TA_OS_UNIX

class cssUnixConsole;

class ConThread: public QThread {
INHERITED(QThread)
public:
  String		prompt;
  cssUnixConsole*	con;
  
  void			stop(); // (main) safe way to tell it to stop
  
  ConThread(cssUnixConsole* parent);

protected:
  override void 	run();

};

class cssUnixConsole: public cssConsole {
INHERITED(cssConsole)
friend class ConThread;
public:
  ConThread*		thread;
  
  override const String	prompt() 
    {if (thread) return thread->prompt; else return _nilString;}
  override void		setPrompt(const String& value)
    {if (thread) thread->prompt = value;}

  void			emit_NewLine(String ln, bool eof)
    {emit NewLine(ln, eof);}

  cssUnixConsole(QObject* parent = NULL);
  ~cssUnixConsole();
};

cssConsole* cssConsole::New_SysConsole(QObject* parent) {
 cssUnixConsole* rval = new cssUnixConsole(parent);
 rval->thread->start();
 return rval;
}

ConThread::ConThread(cssUnixConsole* parent)
:inherited(parent)
{
  con = parent;
}

void ConThread::run() {
  String lprompt; // local, so it is threadsafe
  String curln;
  bool eof;
  char* curln_;
  while (con) { // con null'ed can indicate stop needed
    lprompt = prompt;
    //NOTE: according to rl spec, we must call free() on the string returned
    curln_ = readline((char*)lprompt.chars());
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

cssUnixConsole::cssUnixConsole(QObject* parent)
:inherited(parent)
{
  thread = new ConThread(this);
}

cssUnixConsole::~cssUnixConsole() {
  if (thread) {
    thread->con = NULL;
    thread->stop();
    thread = NULL;
  }
}


#endif // TA_OS_UNIX
