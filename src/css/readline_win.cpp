/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// readline_win.cc -- stub replacement for readline, in windows

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

// basic readline interface -- from css_machine.cc

extern "C" {
  extern int rl_done;		// readline done reading
  extern int rl_pending_input;
  extern int (*rl_event_hook)(void);	// this points to the Qt event loop pump if running TA_GUI

  extern char* rl_readline(char*);
  extern void add_history(char*);
  extern int rl_stuff_char(int);
}

// readline waitproc and events -- from css_qt.cc (only included for TA_GUI)
extern "C" {
//  extern int readline_waitproc(void); // this is supplied by ta/css
  extern int (*rl_event_hook)(void);	// this points to the waitproc if running Qt
  extern int rl_done;			// set this to kill readline..
}


// readline completion -- from css_builtin.cc

extern "C" {
  typedef int rl_function(void);
  typedef char* rl_generator_fun(char*, int);

  extern char *rl_line_buffer; // only used in css_attempted_completion
  extern int (*rl_attempted_completion_function)(void);

  extern char** completion_matches (char* text, rl_generator_fun* gen);

  // following defined in css_builtin.cc
  extern char** css_attempted_completion(char* text, int start, int end);
  extern char* css_path_generator(char* text, int state); // rl_generator_fun
  extern char* css_scoped_generator(char* text, int state); // rl_generator_fun
  extern char* css_keyword_generator(char* text, int state); // rl_generator_fun
}

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
class WinCon {
public:
  static bool con_created;
  static bool con_inited;

  static void createConsole() {
    int rval = 0;
    if (AllocConsole() == 0) {
      int lasterr = GetLastError();
      // TODO: raise exception
      
    } 
    con_created = true;
  }

  static void getConHandles() {
    //TODO: error checks
    hstdin = GetStdHandle(STD_INPUT_HANDLE);
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hstderr = GetStdHandle(STD_ERROR_HANDLE);
  }

  static void connectStreams() {
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

  WinCon() {
    if (con_inited) return;
    if (!con_created) createConsole();
    //TODO: maybe not needed in true console process -- if so, modalize
    // initialize the handles
    getConHandles();
    connectStreams();

    con_inited = true;
  }
};

#ifdef TA_GUI
bool WinCon::con_created = false;
#else
bool WinCon::con_created = true;
#endif
bool WinCon::con_inited = false;

WinCon winConInstance; // should get created very early in system startup, so cerr available

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
char* rl_line_buffer = rl_line_buffer_;
int (*rl_event_hook)(void) = NULL;	// this points to the Qt event loop pump if running TA_GUI
int (*rl_attempted_completion_function)(void) = NULL;


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
    Sleep(20); // note: readline says it calls back 10x/s, but this is sluggish
  }
  if (read_error == ERROR_HANDLE_EOF) {
    return NULL; // our way of signalling NULL
  }
  rl_line_buffer_[num_read] = '\0'; // not sure if this is done, so do it
  return rl_line_buffer_;
}

void add_history(char*) {
}

int rl_stuff_char(int) {
  //TEMP
  return 0;
}


}
