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
#include "ta_css.h"

#include <QThread>

extern "C" {
  extern char* rl_readline(char*);
  extern void add_history(char*);
  extern int rl_done;		// readline done reading
}

cssQandDConsole*	cssQandDConsole::m_sys_instance = NULL;

cssQandDConsole* cssQandDConsole::Get_SysConsole(QObject* parent) {
 if (!m_sys_instance) {
   m_sys_instance = New_SysConsole(parent);
 }
 return m_sys_instance;
}

cssQandDConsole::cssQandDConsole(QObject* parent)
:inherited(parent)
{
}

cssQandDConsole::~cssQandDConsole() {
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

class cssUnixConsole: public cssQandDConsole {
INHERITED(cssQandDConsole)
friend class ConThread;
public:
  ConThread*		thread;
  
  override const String	prompt() {
    if (thread) return thread->prompt; else return _nilString;
  }
  override void		setPrompt(const String& value) {
    if (thread) {
      if (thread->prompt == value) return;
      //TODO: try tot erase last prompt if any
      thread->prompt = value;
      cout << value;
    }
  }
  
  override void		Start() {
     if (!thread) return;
     //TODO: maybe need to check not already started???
     thread->start();
  } 


  void			emit_NewLine(String ln, bool eof)
    {emit NewLine(ln, eof);}

  cssUnixConsole(QObject* parent = NULL);
  ~cssUnixConsole();
};

cssQandDConsole* cssQandDConsole::New_SysConsole(QObject* parent) {
 cssUnixConsole* rval = new cssUnixConsole(parent);
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
    curln_ = rl_readline((char*)lprompt.chars());
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

//////////////////////////////////////////////////////////////////////////
//

QcssConsole* QcssConsole::theInstance = NULL;

QcssConsole::~QcssConsole() {
}

QcssConsole::QcssConsole(QObject* parent, cssCmdShell* cs) :
  inherited((QWidget*)parent, "css> ", true)
{
  cmd_shell = cs;
}

QcssConsole* QcssConsole::getInstance(QObject* parent, cssCmdShell* cs) {
  if(theInstance != NULL) return theInstance;
  theInstance = new QcssConsole(parent, cs);
  return theInstance;
}

QString QcssConsole::interpretCommand(QString command, int* res) {
  *res = 0;
  cmd_shell->AcceptNewLine(command, false);
  inherited::interpretCommand(command, res);
  return "";
}

void QcssConsole::keyPressEvent( QKeyEvent *e ) {
  if((e->key() == Qt::Key_C) && (e->modifiers() == Qt::ControlModifier)) {
    cmd_shell->src_prog->Stop();
  }
  else {
    // todo: also implement basic emacs keys!  may need to redef qtext edit for this
    inherited::keyPressEvent(e);
  }
}



/////////////////////////////////////////////
// autocompletion

int QcssConsole::autocompletePath(String cmd_b4, String cmd, QStringList& lst) {
  int st_len = lst.size();
  if(cssBI::root == NULL) return 0;

  String par_path = cmd;
  par_path = par_path.before('.',-1);
  String mb_name = cmd;
  mb_name = mb_name.after('.', -1);
  int len = mb_name.length();
  TAPtr parent = NULL;
  TAPtr root = (TAPtr)cssBI::root->ptr;
  if(par_path == "") {
    parent = root;
  }
  else {
    MemberDef* md = NULL;
    parent = root->FindFromPath(par_path, md);
  }

  if(parent == NULL) return 0;
  TypeDef* par_td = parent->GetTypeDef();

  par_path = cmd_b4 + par_path + ".";		// add back for returning

  for(int i=0;i<par_td->members.size;i++) {
    MemberDef* md = par_td->members.FastEl(i);
    if(md->name(0,len) == mb_name)
      lst.append(par_path + md->name);
  }
  for(int i=0;i<par_td->methods.size;i++) {
    MethodDef* md = par_td->methods.FastEl(i);
    if(md->name(0,len) == mb_name)
      lst.append(par_path + md->name);
  }
  return lst.size() - st_len;
}

int QcssConsole::autocompleteScoped(String cmd_b4, String cmd, QStringList& lst) {
  int st_len = lst.size();
  String par_path = cmd;
  par_path = par_path.before("::",-1);
  String mb_name = cmd;
  mb_name = mb_name.after("::", -1);
  int len = mb_name.length();
  TAPtr parent = NULL;
  TypeDef* par_td = taMisc::types.FindName(par_path);
  if(par_td == NULL) return 0;

  par_path = cmd_b4 + par_path + "::";		// add back for returning

  for(int i=0;i<par_td->members.size;i++) {
    MemberDef* md = par_td->members.FastEl(i);
    if(md->name(0,len) == mb_name)
      lst.append(par_path + md->name);
  }
  for(int i=0;i<par_td->methods.size;i++) {
    MethodDef* md = par_td->methods.FastEl(i);
    if(md->name(0,len) == mb_name)
      lst.append(par_path + md->name);
  }
  for(int i=0;i<par_td->sub_types.size;i++) {
    TypeDef* st = par_td->sub_types.FastEl(i);
    if(st->name(0,len) == mb_name)
      lst.append(par_path + st->name);
    if(st->InheritsFormal(TA_enum)) {
      for(int j=0;j<st->enum_vals.size; j++) {
	EnumDef* en = st->enum_vals.FastEl(j);
	if(en->name(0,len) == mb_name)
	  lst.append(par_path + en->name);
      }
    }
  }
  return lst.size() - st_len;
}

int QcssConsole::autocompleteKeyword(String cmd_b4, String cmd, QStringList& lst) {
  int st_len = lst.size();
  int len = cmd.length();
  int spc_idx = 0;
  int extra_spcs = 2;
  if(cssMisc::cur_class != NULL)
    extra_spcs++;
  cssSpace* spc = NULL;
  do {
    if(spc_idx == 0)
      spc = &(cssMisc::cur_top->types);
    else if(spc_idx == 1)
      spc = &cssMisc::TypesSpace;
    else if((cssMisc::cur_class != NULL) && (spc_idx == 2))
      spc = cssMisc::cur_class->types;
    else
      spc = cssMisc::cur_top->GetParseSpace(spc_idx-extra_spcs);

    if(spc == NULL) break;

    for(int i=0;i<spc->size;i++) {
      cssEl* el = spc->FastEl(i);
      if(el->name(0,len) == cmd)
	lst.append(cmd_b4 + el->name);
    }
    spc_idx++;
  } while(spc != NULL);
  return lst.size() - st_len;
}

QStringList QcssConsole::autocompleteCommand(QString q_cmd) {
  String cmd = q_cmd;
  String fcmd = q_cmd;
  String cmd_b4;
  if(cmd.contains(' ')) {
    cmd = fcmd.after(' ',-1);
    cmd_b4 = fcmd.through(' ',-1);
  }

  QStringList lst;

  if(cmd[0] == '.') {		// path
    autocompletePath(cmd_b4, cmd, lst);
  }
  else if(cmd[0] == '"') {		// filename
    lst = autocompleteFilename(cmd.after(0));
    for(int i=0;i<lst.size();i++) {
      String tmp = lst[i];
      lst[i] = (const char*)(cmd_b4 + '"' + tmp);
    }
  }
  else if(cmd.contains("::")) { // scoped type
    autocompleteScoped(cmd_b4, cmd, lst);
  }
  else {			// keyword
    autocompleteKeyword(cmd_b4, cmd, lst);
  }
  return lst;
}

void QcssConsole::ctrlCPressed() {
  cssMisc::cur_top->Stop();
}


#endif // TA_OS_UNIX
