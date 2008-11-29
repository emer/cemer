/***************************************************************************
                          qconsole.h  -  description
                             -------------------
    begin                : mar mar 15 2005
    copyright            : (C) 2005 by Houssem BDIOUI
    email                : houssem.bdioui@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QCONSOLE_H
#define QCONSOLE_H

// note: need config.h for HAVE_QT_CONSOLE
#include "config.h"
#include "taiqtso_def.h"

#ifdef HAVE_QT_CONSOLE
#include "interceptor.h"
#include <qstringlist.h>
#include <qtextedit.h>
#include <QMouseEvent>
#include <QKeyEvent>

/**An abstract Qt console
 *@author Houssem BDIOUI, modified extensively by Randall O'Reilly
 */

class QConsole : protected  QTextEdit {
Q_OBJECT
INHERITED(QTextEdit)
public:
  //constructor
  QConsole(QWidget *parent = NULL, const char *name = NULL, bool initInterceptor = true);
  // set the prompt of the console
  virtual void setPrompt(QString prompt, bool display = true);
  // executes the command and displays back its result
  virtual void execCommand(QString command, bool writeCommand = true, bool showPrompt = true);
  virtual int saveContents(QString fileName); // save the entire contents of console window
  virtual int saveScript(QString fileName); // loads a file script
  virtual int loadScript(QString fileName);
  virtual void clear();	       // clear & reset the console (useful sometimes)
  virtual void reset();
  virtual void exit();		// exit shell
  virtual void flushOutput(bool wait_for_pager = false);
  // flush cout & cerr output.  if wait_for_pager, then wait until user has paged through
  // everything in the buffer

  // cosmetic methods
  virtual void setCmdColor(QColor c) {cmdColor = c;};
  virtual void setErrColor(QColor c) {errColor = c;};
  virtual void setOutColor(QColor c) {outColor = c;};
  virtual void setCompletionColor(QColor c) {completionColor = c;};
  virtual void setFont(const QFont& f);
  virtual void setFontNameSize(QString fnm, int sz);
  virtual void setPager(bool pager);
  // determines whether to use a pager mechanism to control the flow of text through the console (as in the unix more command)


using inherited::setMinimumSize;
using inherited::minimumSize;
protected:
  // code that new specific implementation should override:

  // execute a validated command (should be reimplemented and called at the end)
  // the return value of the function is the string result
  // res must hold back the return value of the command (0: passed; else: error)
  virtual QString interpretCommand(QString command, int *res);
  // give suggestions to autocomplete a command (should be reimplemented)
  // the return value of the function is the string list of all suggestions
  virtual QStringList autocompleteCommand(QString cmd);
  // do completion of string as a possible file name: must be called by a specific implementation!
  virtual QStringList autocompleteFilename(QString fnm, QString pre_fnm);
  // check if current command is complete (no hanging brackets, etc)
  virtual QString findIntersection(const QStringList& lst);
  // find substring that is is common to all strings in the list (i.e., the intersection)

  virtual bool isCommandComplete(QString command);
  virtual void ctrlCPressed();	// the user pressed ctrl-C -- do something (e.g., stop program)


protected:
  // Redefined virtual methods
  void keyPressEvent(QKeyEvent* e);
  void resizeEvent(QResizeEvent* e);
  void paste();
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  virtual void getDisplayGeom();

  virtual void displayPrompt(bool force = false); // displays the prompt, force = definitely do so
  virtual void	gotoPrompt(QTextCursor& cursor);		// set position to just after prompt (moves anchor)
  virtual void	gotoEnd(QTextCursor& cursor, bool select=true); // set position to end (and select text or not)
  virtual QString getCurrentCommand();			     // get text after prompt
  virtual void replaceCurrentCommand(QString newCommand);    // Replace current command with a new one
  virtual bool cursorInCurrentCommand();	// cursor is in the current command editing zone    
  virtual bool stdDisplay(QTextStream *s);
  // displays redirected stdout/stderr: return true if waiting for pager

  //protected attributes
protected:
  QColor cmdColor, errColor, outColor, completionColor;     // colors
  int curPromptPos;		// just after current prompt position
  int curOutputLn;		// last output line
  int maxLines;			// max number of lines to display at a time (pager)
  int maxCols;			// max display columns
  int fontHeight;		// height of current font, used for computing maxLines
  int fontWidth;		// width of current font, used for computing maxCols
  bool noPager;			// completely disable pager mechanism
  bool quitPager;		// quit pager until next time
  bool contPager;		// continue pager until next time
  bool promptDisp;		// just displayed the prompt -- no output in between
  int promptLength;		// cached prompt length
  QString prompt;		// The prompt string
  QStringList history;	// The commands history
  QStringList recordedScript; // commands that have succeeded
  int historyIndex; // Current history index (needed because afaik QStringList does not have such an index)
  Interceptor *stdoutInterceptor; // Stdout interceptor
  Interceptor *stderrInterceptor; // Stderr interceptor

  // Redefined virtual slots
private slots:
  virtual void stdReceived(); 	// standard output/err received: display and flush

signals:
  // Signal emitted after that a command is executed
  void commandExecuted(QString command);
};

#endif //HAVE_QT_CONSOLE
#endif
