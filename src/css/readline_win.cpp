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


// readline_win.cc -- stub replacement for readline, in windows

#include "css_console.h"

#include <windows.h>

#include <iostream>
#include <fstream>

#include <io.h>
#include <fcntl.h>
using namespace std;

//TODO
/* it seems that rl_readline is defined in xmemory, but it itself is just
   a C wrapper for the readline routine, which is also presumably C
   so it doesn't seem to make sense to need that extra redundant wrapper

   but we just go with the flow here, and implement readline
*/

extern "C" { // readline completion -- either refers to readline, or we fake it
  typedef int rl_function(void);
  typedef char* rl_generator_fun(char*, int);

  extern char *rl_line_buffer; // only used in css_attempted_completion
  extern int (*rl_attempted_completion_function)(void);

  extern char** completion_matches (char* text, rl_generator_fun* gen);
}
/*nn
extern "C" { // following defined in css_builtin.cc
  extern char** css_attempted_completion(char* text, int start, int end);
  extern char* css_path_generator(char* text, int state); // rl_generator_fun
  extern char* css_scoped_generator(char* text, int state); // rl_generator_fun
  extern char* css_keyword_generator(char* text, int state); // rl_generator_fun
}*/


enum ReadStatus {
  Idle = 0,
  Waiting,
  SuccessResult,
  ErrorResult
};
#define BUF_SIZE 1024
char rl_line_buffer_[BUF_SIZE];
bool readline_init = false;
HANDLE hstdin = INVALID_HANDLE_VALUE; // handle to standard input, ie console
HANDLE hstdout = INVALID_HANDLE_VALUE;
HANDLE hstderr = INVALID_HANDLE_VALUE;
HANDLE hread_thread = INVALID_HANDLE_VALUE;
DWORD read_thread_id;
DWORD num_read = 0;
ReadStatus read_status = Idle;
DWORD read_error = 0;

// Windows console processes automatically have a console (duh!)
// Windows GUI processes don't have one, so we have to create it
class cssWinConsole: public cssConsole {
INHERITED(cssConsole)
public:

  static BOOL WINAPI Console_HandlerRoutine(
    DWORD dwCtrlType); // handler routines for console events

  override void		setTitle(const QString& value);

  cssWinConsole(QObject* parent);
  ~cssWinConsole();
  
protected:
  static bool con_created;
  static bool con_inited;
  static bool con_closing; // we set this when deleting, so we can detect user closing and recreate console

  static void initConsole(); // master routine, called to create, and if user closes
  static void createConsole();
  static void getConHandles();
  static void connectStreams();
};

#ifdef TA_GUI
bool cssWinConsole::con_created = false;
#else
bool cssWinConsole::con_created = true;
#endif
bool cssWinConsole::con_inited = false;
bool cssWinConsole::con_closing; 

cssConsole* cssConsole::New_SysConsole(QObject* parent) {
 cssWinConsole* rval = new cssWinConsole(parent);
 return rval;
}

void cssWinConsole::createConsole() {
  int rval = 0;
  if (AllocConsole() == 0) {
    int lasterr = GetLastError();
    // TODO: raise exception
    
  } 
  con_created = true;
}

void cssWinConsole::getConHandles() {
  //TODO: error checks
  hstdin = GetStdHandle(STD_INPUT_HANDLE);
  hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
  hstderr = GetStdHandle(STD_ERROR_HANDLE);
}

void cssWinConsole::connectStreams() {
  // reconnect the cxx streams
  //TODO: probably not needed for true console processes (NO GUI)
  int hConHandle = _open_osfhandle((intptr_t)hstderr, _O_TEXT);
  FILE *fp = _fdopen(hConHandle, "w");
  filebuf *fb = new filebuf(fp);
  cerr.rdbuf(fb);

  hConHandle = _open_osfhandle((intptr_t)hstdout, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  fb = new filebuf(fp);
  cout.rdbuf(fb);

  hConHandle = _open_osfhandle((intptr_t)hstdin, _O_TEXT);
  fp = _fdopen(hConHandle, "r");
  fb = new filebuf(fp);
  cin.rdbuf(fb);
}

BOOL cssWinConsole::Console_HandlerRoutine(
    DWORD dwCtrlType)
{
  // note: return TRUE if you handle the event
  switch (dwCtrlType) {
  //note: treat Ctrl-C and Ctrl-Break the same
  case CTRL_C_EVENT: 
  case CTRL_BREAK_EVENT: 
    cssMisc::TopShell->Exit();
    return true;
  case CTRL_CLOSE_EVENT: 
    con_inited = false; con_created = false;
    initConsole(); // maybe we have to do this in a timer call
    return TRUE;
  case CTRL_LOGOFF_EVENT: break;
  case CTRL_SHUTDOWN_EVENT: break;
  default: break;
  }
  return FALSE;
}

void cssWinConsole::initConsole() {
  if (con_inited) return;
  if (!con_created) createConsole();
  //TODO: maybe not needed in true console process -- if so, modalize
  // initialize the handles
  getConHandles();
  connectStreams();
  BOOL ok = SetConsoleCtrlHandler(
    Console_HandlerRoutine, TRUE); // add handler

  con_inited = true;
}

cssWinConsole::cssWinConsole(QObject* parent) 
:inherited(parent)
{
  initConsole();
}

cssWinConsole::~cssWinConsole() {
  SetConsoleCtrlHandler(
    Console_HandlerRoutine, FALSE); // remove handler
  con_closing = true;
}

void cssWinConsole::setTitle(const QString& value) {
  SetConsoleTitle(value);
}


extern "C" {
int rl_done = 0;		// readline done reading
int rl_pending_input = 0;
int rl_keyboard_input_timeout_ms = 100; // default of .1s
char* rl_line_buffer = rl_line_buffer_;
int (*rl_event_hook)(void) = NULL;	// this points to the Qt event loop pump if running TA_GUI
int (*rl_attempted_completion_function)(void) = NULL;

int rl_set_keyboard_input_timeout (int u) { // in microseconds
  if (u < 2000) u = 2000; // sanity
  rl_keyboard_input_timeout_ms = u / 1000;
  return u;
}

char** completion_matches (char* text, rl_generator_fun* gen) {
  return NULL;
}

int init_readline() {
  int rval = 0;
  // get hstdin for case where not using the console
  if (hstdin == INVALID_HANDLE_VALUE)
    hstdin = GetStdHandle(STD_INPUT_HANDLE);

  // does nothing at this point
  readline_init = true;
  return rval;
}


VOID CALLBACK readline_completion(
  DWORD dwErrorCode,
  DWORD dwNumberOfBytesTransfered,
  LPOVERLAPPED lpOverlapped
) {
  if (dwErrorCode == 0) {
    read_status = SuccessResult;
    num_read = dwNumberOfBytesTransfered;
  } else {
    read_error = GetLastError(); //note: could be eof
    read_status = ErrorResult;
  }
}

OVERLAPPED overlapped; // be global so can't go out of scope

char* readline(char* prompt) {
  DWORD hresult; // for return vals from api funcs
  DWORD last_error = 0;


  if (!readline_init) {
    init_readline();
  }

  // prompt
  int lprompt = (prompt) ? strlen(prompt) : 0;
  hresult = WriteFile(hstdout, prompt, lprompt, NULL, NULL);

  read_error = 0;
  read_status = Waiting;
  num_read = 0;
  
  // we execute one of two versions, depending on whether blocking or nonblocking
  if (rl_event_hook) { // non blocking
    overlapped.Offset = 0;
    overlapped.OffsetHigh = 0;
    overlapped.hEvent = 0; // don't use an event, we use the completion routine
    rl_event_hook();
    while ((read_error == 0) && (rl_done == 0) && (read_status == Waiting)) {
      // asynchronous read -- calls the callback when done or error
      read_status = Waiting;
      // note: the handle is usually invalid, unless the parent process attached something to them
      BOOL syn_hresult = ReadFileEx(hstdin, rl_line_buffer_, BUF_SIZE - 1, &overlapped, readline_completion);
      // an error here means the async read could not be established -- the actual read result comes later
      if (syn_hresult == 0) {
        read_error = GetLastError(); //note: could be eof
        read_status = ErrorResult; // NOTE: this means the 
      } else while ((read_error == 0) && (rl_done == 0) && (read_status == Waiting) &&
        !cssMisc::TopShell->external_exit)
      { // just loop waiting for result
        rl_event_hook(); // has a function, since that is what caused us to execute
	Sleep(rl_keyboard_input_timeout_ms); 
      }
    }
  } else { // blocking
    // note: blocks until eol or eof
    hresult = ReadFile(hstdin, rl_line_buffer_, BUF_SIZE - 1, &num_read, NULL); 
    if (hresult != 0) {
      read_status = SuccessResult;
    } else {
      read_error = GetLastError(); //note: could be eof
      read_status = ErrorResult;
    }
  }

  if (read_error == ERROR_HANDLE_EOF) {
    return NULL; // our way of signalling EOF
  }
  rl_line_buffer_[num_read] = '\0'; // not sure if this is done, so do it
  // gnu readline actually returns a new string
  char* rval = (char*)malloc(num_read + 1);
  strcpy(rval, rl_line_buffer_);
  return rval;
}

void add_history(char*) {
}

int rl_stuff_char(int) {
  //TEMP
  return 0;
}
} // C

/*obs
// thread procedure that actually does the console reading
// it is created suspended, and only activated to read a line
DWORD WINAPI readFile_ThreadProc(LPVOID lpParameter) {
  while (true) {
    //NOTE: always change read_status last, after setting all other values
    DWORD hresult;
    hresult = ReadFile(hstdin, rl_line_buffer_, BUF_SIZE - 1, &num_read, NULL); 
    if (hresult == 0) {
      read_error = GetLastError(); //note: could be eof
      read_status = ErrorResult;
    } else {
      read_status = SuccessResult;
    }
    //TODO: we could signal the main thread here, for faster response
    SuspendThread(hread_thread);
  }
}


int init_readline() {
  int rval = 0;

  // create the worker thread, create it suspended
  hread_thread = CreateThread(NULL, 0, readFile_ThreadProc, NULL,
    (CREATE_SUSPENDED), &read_thread_id);
  readline_init = true;
  return rval;
}

extern "C" {
int rl_done = 0;		// readline done reading
int rl_pending_input = 0;
int rl_keyboard_input_timeout_ms = 100; // default of .1s
char* rl_line_buffer = rl_line_buffer_;
int (*rl_event_hook)(void) = NULL;	// this points to the Qt event loop pump if running TA_GUI
int (*rl_attempted_completion_function)(void) = NULL;

int rl_set_keyboard_input_timeout (int u) { // in microseconds
  if (u < 2000) u = 2000; // sanity
  rl_keyboard_input_timeout_ms = u / 1000;
  return u;
}

char** completion_matches (char* text, rl_generator_fun* gen) {
  return NULL;
}

char* readline(char* prompt) {
  DWORD hresult; // for return vals from api funcs
  DWORD last_error = 0;
#ifdef TA_GUI
#endif

  if (!readline_init) {
    init_readline();
  }
  // prompt
  int lprompt = (prompt) ? strlen(prompt) : 0;
  hresult = WriteFile(hstdout, prompt, lprompt, NULL, NULL);

  read_error = 0;
  read_status = Waiting;
  num_read = 0;
  ResumeThread(hread_thread); // let'r rip
  if (rl_event_hook)
    rl_event_hook();
  while ((read_error == 0) && (rl_done == 0) && (read_status == Waiting)) {
    // see if anything available, else wait
    if (rl_event_hook)
      rl_event_hook();
    Sleep(rl_keyboard_input_timeout_ms); 
  }
  if (read_error == ERROR_HANDLE_EOF) {
    return NULL; // our way of signalling EOF
  }
  rl_line_buffer_[num_read] = '\0'; // not sure if this is done, so do it
  // gnu readline actually returns a new string
  char* rval = (char*)malloc(num_read + 1);
  strcpy(rval, rl_line_buffer_);
  return rval;
}

void add_history(char*) {
}

int rl_stuff_char(int) {
  //TEMP
  return 0;
}
} // C
*/