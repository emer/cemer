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
public:
  //constructor
  QConsole(QWidget *parent = NULL, const char *name = NULL, bool initInterceptor = true);
  // set the prompt of the console
  virtual void setPrompt(QString prompt, bool display = true);
  // executes the command and displays back its result
  virtual void execCommand(QString command, bool writeCommand = true, bool showPrompt = true);
  // saves a file script
  virtual int saveScript(QString fileName);
  // loads a file script
  virtual int loadScript(QString fileName);
  // clear & reset the console (useful sometimes)
  virtual void clear();
  virtual void reset();
  virtual void flushOutput();
  // cosmetic methods !
  virtual void setCmdColor(QColor c) {cmdColor = c;};
  virtual void setErrColor(QColor c) {errColor = c;};
  virtual void setOutColor(QColor c) {outColor = c;};
  virtual void setCompletionColor(QColor c) {completionColor = c;};
  virtual void setFont(QFont f) {setCurrentFont(f);};

protected:
  // code that new specific implementation should override:

  // execute a validated command (should be reimplemented and called at the end)
  // the return value of the function is the string result
  // res must hold back the return value of the command (0: passed; else: error)
  virtual QString interpretCommand(QString command, int *res);
  // give suggestions to autocomplete a command (should be reimplemented)
  // the return value of the function is the string list of all suggestions
  virtual QStringList autocompleteCommand(QString cmd);
  // check if current command is complete (no hanging brackets, etc)
  virtual bool isCommandComplete(QString command);

protected:
  // Redefined virtual methods
  void keyPressEvent(QKeyEvent* e);
  void resizeEvent(QResizeEvent* e);
  void paste();

  virtual void	gotoPrompt(QTextCursor& cursor);		// set position to just after prompt (moves anchor)
  virtual void	gotoEnd(QTextCursor& cursor, bool select=true); // set position to end (and select text or not)
  virtual QString getCurrentCommand();			     // get text after prompt
  virtual void replaceCurrentCommand(QString newCommand);    // Replace current command with a new one
  virtual bool cursorInCurrentCommand();	// cursor is in the current command editing zone    
  virtual void stdReceived(QTextStream *s);	// displays redirected stdout/stderr

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
  uint historyIndex; // Current history index (needed because afaik QStringList does not have such an index)
  Interceptor *stdoutInterceptor; // Stdout interceptor
  Interceptor *stderrInterceptor; // Stderr interceptor

  // Redefined virtual slots
private slots:
  virtual void displayPrompt(); // displays the prompt

signals:
  // Signal emitted after that a command is executed
  void commandExecuted(QString command);
};

#endif
