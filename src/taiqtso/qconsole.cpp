/***************************************************************************
                          qconsole.cpp  -  description
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

#include "qconsole.h"
#include <qfile.h>

#include <QMouseEvent>
#include <QTextStream>
#include <QKeyEvent>
#include <QTextCursor>

using namespace std;

#include <iostream>
#include <fstream>

//Clear the console
void QConsole::clear() {
  QTextEdit::clear();
  displayPrompt();
}

//Reset the console
void QConsole::reset() {
  clear();
  QFont font("LucidaTypewriter", 8);
  setCurrentFont(font);
  QFontMetrics fm(font);
  fontHeight = fm.height();
  fontWidth = fm.charWidth("m",0);
  if(fontHeight < 5) fontHeight = 5;
  if(fontWidth < 5) fontWidth = 5;
  maxLines = 40;
  maxCols = 80;
  //init attributes
  historyIndex = 0;
  history.clear();
  recordedScript.clear();
  noPager = false;
  quitPager = false;
  contPager = false;
  curOutputLn = 0;
  promptDisp = false;
}
// todo: get height,width,font, etc from styles and actual screen size..


//QConsole constructor (init the QTextEdit & the attributes)
QConsole::QConsole(QWidget *parent, const char *name, bool initInterceptor) 
  : QTextEdit(parent), cmdColor(Qt::black), errColor(Qt::red), outColor(Qt::blue),
    completionColor(Qt::green), stdoutInterceptor(NULL), stderrInterceptor(NULL)
{
  //resets the console
  reset();

  if(initInterceptor) {
    //Initialize the interceptors
    stdoutInterceptor = new Interceptor(this);
    stdoutInterceptor->initialize(1);
    connect(stdoutInterceptor, SIGNAL(received(QTextStream *)), SLOT(displayPrompt()));

    stderrInterceptor = new Interceptor(this);
    stderrInterceptor->initialize(2);
    connect(stderrInterceptor, SIGNAL(received(QTextStream *)), SLOT(displayPrompt()));
  }
}

//Sets the prompt and cache the prompt length to optimize the processing speed
void QConsole::setPrompt(QString newPrompt, bool display) {
  prompt = newPrompt;
  promptLength = prompt.length();
  //display the new prompt
  if (display)
    displayPrompt();
}

void QConsole::flushOutput() {
  if(stdoutInterceptor) {
    setTextColor(outColor);
    stdReceived(stdoutInterceptor->textIStream());
  }
  if(stderrInterceptor) {
    setTextColor(errColor);
    stdReceived(stderrInterceptor->textIStream());
  }
}

// Displays the prompt and move the cursor to the end of the line.
void QConsole::displayPrompt(bool force) {
  // flush the stdout/stderr before displaying the prompt
  flushOutput();
  if(!force && promptDisp) return;
  QTextCursor cursor = textCursor();
  // displays the prompt
  setTextColor(cmdColor);
  gotoEnd(cursor, false);		// todo: maybe a newline?
  append(prompt);
  curPromptPos = cursor.position(); // save this position for future reference
  quitPager = false;		// reset this flag whenver prompt returns
  contPager = false;
  promptDisp = true;
}

void QConsole::gotoPrompt(QTextCursor& cursor) {
  cursor.setPosition(curPromptPos, QTextCursor::MoveAnchor);
}

void QConsole::gotoEnd(QTextCursor& cursor, bool select) {
  if(select)
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor); // selects
  else
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor); // not selects
}

//displays redirected stdout/stderr
void QConsole::stdReceived(QTextStream *s) {
  //  while(!s->atEnd() && (curOutputLn < maxLines)) {
  while(curOutputLn < maxLines) {
    QString line = s->readLine(maxCols);
    if(line.isNull()) break;
    if(!quitPager) {
      promptDisp = false;
      append(line);
      if(!contPager && !noPager) {
	curOutputLn++;
	if(curOutputLn >= maxLines) {
	  append("---Press Return for More, q=quit, c=continue without paging ---\n");
	  break;
	}
      }
    }
  }
}

void QConsole::resizeEvent(QResizeEvent* e) {
  maxLines = (width() / fontHeight) - 1;
  maxCols = (height() / fontWidth) - 1;
  if(maxLines < 10) maxLines = 10;
  if(maxCols < 10) maxCols = 10;
  QTextEdit::resizeEvent(e);
}

// Reimplemented key press event
void QConsole::keyPressEvent(QKeyEvent* e) {
  QTextCursor cursor = textCursor();

  if(curOutputLn >= maxLines) {
    if(e->key() == Qt::Key_Return) {
      curOutputLn = 0;
    }
    else if(e->key() == Qt::Key_Q) {
      curOutputLn = 0;
      quitPager = true;
    }
    else if(e->key() == Qt::Key_C) {
      curOutputLn = 0;
      contPager = true;
    }
    return;
  }

  //If Ctrl + C pressed, then undo the current command
  if((e->key() == Qt::Key_C) && (e->modifiers() == Qt::ControlModifier)) {
    displayPrompt();
  }
  else if(e->key() == Qt::Key_Tab) {   //Treat the tab key & autocomplete the current command
    QString command = getCurrentCommand();
    QStringList sl = autocompleteCommand(command);
    QString str = sl.join(" ");
    if(sl.count() == 1)
      replaceCurrentCommand(sl[0] + " ");
    else if(sl.count() > 1) {
      setTextColor(completionColor);
      append(sl.join(" "));
      displayPrompt(true);	// force!
      cursor.insertText(command);
    }
  }
  else if(e->key() == Qt::Key_Backspace) { // todo: trap other cases..
    if(cursorInCurrentCommand()) // don't backup into prompt
      QTextEdit::keyPressEvent( e );
  }
  else if(e->key() == Qt::Key_Return) {
    QString command = getCurrentCommand();
    if(isCommandComplete(command))
      execCommand(command, false);
  }
  else {
    QTextEdit::keyPressEvent( e );
  }
}

//Get the current command
QString QConsole::getCurrentCommand() {
  QTextCursor cursor = textCursor();
  gotoPrompt(cursor);
  gotoEnd(cursor, true);
  QString command = cursor.selectedText();
  if(command.isNull() || command.isEmpty()) command = "\n";
  cursor.clearSelection();
  return command;
}

//Replace current command with a new one
void QConsole::replaceCurrentCommand(QString newCommand) {
  QTextCursor cursor = textCursor();
  gotoPrompt(cursor);
  gotoEnd(cursor, true); // select
  cursor.insertText(newCommand);				   // replaces
  cursor.clearSelection();
}

bool QConsole::cursorInCurrentCommand() {
  QTextCursor cursor = textCursor();
  int pos = cursor.position();
  return (pos > curPromptPos);
}

//execCommand(QString) executes the command and displays back its result
void QConsole::execCommand(QString command, bool writeCommand, bool showPrompt) {
  //Display the prompt with the command first
  if (writeCommand) {
    if(getCurrentCommand() != "")
      displayPrompt(true);	// force
    append(command);
  }
  //execute the command and get back its text result and its return value
  int res = 0;
  QString strRes = interpretCommand(command, &res);
  //According to the return value, display the result either in red or in blue
  if (res == 0)
    setTextColor(outColor);
  else
    setTextColor(errColor);
  if(!strRes.isEmpty())
    append(strRes);
  if(command.isEmpty()) {
    append("\n");
    displayPrompt(true);
  }
  //Display the prompt again
  if(showPrompt)
    displayPrompt(true);
}

//saves a file script
int QConsole::saveScript(QString fileName) {
  QFile f(fileName);
  if (!f.open(QIODevice::WriteOnly))
    return -1;
  QTextStream ts(&f);
  for ( QStringList::Iterator it = recordedScript.begin(); it != recordedScript.end(); ++it)
    ts << *it << "\n";

  f.close();
  return 0;
}

//loads a file script
int QConsole::loadScript(QString fileName) {
  QFile f(fileName);
  if (!f.open(QIODevice::ReadOnly))
    return -1;
  QTextStream ts(&f);
  QString command;
  while(true) {
    command=ts.readLine();
    if (command.isNull())
      break; //done
    execCommand(command, true, false);
  }
  f.close();
  return 0;
}

//Allows pasting with middle mouse button (x window)
//when clicking outside of the edition zone
void QConsole::paste() {
  QTextCursor cursor = textCursor();
  gotoEnd(cursor, false);		// goto end but don't select
  QTextEdit::paste();
}

///////////////////////////////////////////////////////
//  these need to be overridden by implementations,

//Basically, puts the command into the history list
//And emits a signal (should be called by reimplementations)
QString QConsole::interpretCommand(QString command, int *res) {
  //Add the command to the recordedScript list
  if (*res)
    recordedScript.append(command);
  //update the history and its index
  history.append(command.replace("\n", "\\n"));
  historyIndex = history.size();
  //emit the commandExecuted signal
  emit commandExecuted(command);
  return "";
}

// give suggestions to autocomplete a command (should be reimplemented)
// the return value of the function is the string list of all suggestions
QStringList QConsole::autocompleteCommand(QString) {
  return QStringList();
}
  
//default implementation: command always complete
bool QConsole::isCommandComplete(QString cmd) {
//   if(cmd.isEmpty()) return false;
  return true;
}

