/***************************************************************************
                          qconsole.cpp  -  description
                             -------------------
    begin                : mar mar 15 2005
    copyright            : (C) 2005 by Houssem BDIOUI and Randall C. O'Reilly
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
#include <QApplication>
#include <QCoreApplication>
#include <QDir>

//#include <QDebug>

using namespace std;
#include <iostream>

void QConsole::setFontNameSize(QString fnm, int sz) {
  QFont font(fnm, sz);
  setFont(font);
}

void QConsole::setFont(QFont font) {
  setCurrentFont(font);
  getDisplayGeom();
}

void QConsole::getDisplayGeom() {
  QFontMetrics fm(currentFont());
  fontHeight = fm.height();
  fontWidth = fm.charWidth("m",0);
  if(fontHeight < 5) fontHeight = 5;
  if(fontWidth < 5) fontWidth = 5;
  maxLines = (height() / fontHeight) - 4;
  maxCols = (width() / fontWidth) - 1;
  if(maxLines < 10) maxLines = 10;
  if(maxCols < 10) maxCols = 10;
}

//Clear the console
void QConsole::clear() {
  QTextEdit::clear();
  getDisplayGeom();
  curPromptPos = 0;
  setAcceptRichText(false);	// just plain
  noPager = false;
  quitPager = false;
  contPager = false;
  curOutputLn = 0;
  displayPrompt(true);		// force
}

//Reset the console
void QConsole::reset() {
  clear();
  //init attributes
  historyIndex = 0;
  history.clear();
  recordedScript.clear();
  promptDisp = false;
}
// todo: get height,width,font, etc from styles and actual screen size..

void QConsole::exit() {
  qApp->exit();
}

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
    connect(stdoutInterceptor, SIGNAL(received(QTextStream *)), SLOT(stdReceived()));

    stderrInterceptor = new Interceptor(this);
    stderrInterceptor->initialize(2);
    connect(stderrInterceptor, SIGNAL(received(QTextStream *)), SLOT(stdReceived()));
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

void QConsole::flushOutput(bool wait_for_pager) {
  //  noPager = true;		// debugging!!
  cout.flush();
  cerr.flush();
  bool waiting = false;
  do {
    if(stdoutInterceptor) {
      setTextColor(outColor);
      waiting = stdDisplay(stdoutInterceptor->textIStream());
    }
    if(stderrInterceptor) {
      setTextColor(errColor);
      waiting = (stdDisplay(stderrInterceptor->textIStream()) || waiting);
    }
    if(wait_for_pager && waiting) {
      QCoreApplication::processEvents();
      sleep(1); //note: 1ms is fine, shorter values result in cpu thrashing
    }
  } while(wait_for_pager && waiting);
}

void QConsole::stdReceived() {
  flushOutput();
}

// Displays the prompt and move the cursor to the end of the line.
void QConsole::displayPrompt(bool force) {
  flushOutput();
  if(!force && promptDisp) return;
  QTextCursor cursor(textCursor());
  // displays the prompt
  gotoEnd(cursor, false);
  setTextCursor(cursor);
  setTextColor(cmdColor);
  append(prompt);
  gotoEnd(cursor, false);
  setTextCursor(cursor);
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

// displays redirected stdout/stderr
bool QConsole::stdDisplay(QTextStream* s) {
  if((curOutputLn >= maxLines) && !contPager && !noPager)
    return true;
  while((curOutputLn < maxLines) || contPager || noPager || quitPager) {
    QString line = s->readLine(maxCols);
    if(line.isNull()) break;
    if(!quitPager) {
      promptDisp = false;
      append(line);
      if(!contPager && !noPager) {
	curOutputLn++;
	if(curOutputLn >= maxLines) {
	  append("---Press Return for More, q=quit displaying, c=continue without paging ---");
	  return true;
	}
      }
    }
  }
  return false;
}

void QConsole::resizeEvent(QResizeEvent* e) {
  maxLines = (height() / fontHeight) - 4;
  maxCols = (width() / fontWidth) - 1;
  if(maxLines < 10) maxLines = 10;
  if(maxCols < 10) maxCols = 10;
  QTextEdit::resizeEvent(e);
//   cerr << "console; font height: " << fontHeight << ", wd: " << fontWidth
//        << ", lines: " << maxLines << ", cols: " << maxCols << endl;
}

// Reimplemented key press event
void QConsole::keyPressEvent(QKeyEvent* e) {
  QTextCursor cursor(textCursor());

  bool ctrl_pressed = false;
  if(e->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef __APPLE__
  // ctrl = meta on apple; except it doesnt' work!
  if(e->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif

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
    flushOutput();
    promptDisp = false;
    e->accept();
    return;
  }

  // ctrl key on mac is screwed up: a p is not a p when the ctrl key is pressed
//   cerr << hex << e->key() << " p: " << Qt::Key_P << endl;

  //If Ctrl + C pressed, then undo the current command
  if((e->key() == Qt::Key_C) && ctrl_pressed) {
    ctrlCPressed();
    e->accept();
    displayPrompt();
  }
  else if(e->key() == Qt::Key_Tab) {   //Treat the tab key & autocomplete the current command
    e->accept();
    QString command = getCurrentCommand();
    QStringList sl = autocompleteCommand(command);
    QString intersect = findIntersection(sl);
    if(!intersect.isEmpty()) {
      command = intersect;
      replaceCurrentCommand(command);
    }
    QString str = sl.join(" ");
    if(sl.count() == 1)
      replaceCurrentCommand(sl[0]);
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
    else
      e->accept();
  }
  else if(e->key() == Qt::Key_Return) {
    e->accept();
    if(promptDisp) {
      QString command = getCurrentCommand();
      if(isCommandComplete(command))
	execCommand(command, false);
    }
    else {
      displayPrompt(true);
    }
  }
  else if((e->key() == Qt::Key_L) && ctrl_pressed) {
    e->accept();
    clear();
  }
  else if((e->key() == Qt::Key_Up) || ((e->key() == Qt::Key_P) && ctrl_pressed)) {
    e->accept();
    if(history.size() > 0) {
      historyIndex--; if(historyIndex < 0) historyIndex = 0;
      QString cmd = history[historyIndex];
      if(!cmd.isEmpty())
	replaceCurrentCommand(cmd);
    }
  }
  else if((e->key() == Qt::Key_Down) || ((e->key() == Qt::Key_N) && ctrl_pressed)) {
    e->accept();
    if(history.size() > 0) {
      historyIndex++; if(historyIndex >= history.size()) historyIndex = history.size() -1;
      QString cmd = history[historyIndex];
      if(!cmd.isEmpty())
	replaceCurrentCommand(cmd);
    }
  }
  else if((e->key() == Qt::Key_A) && ctrl_pressed) {
    e->accept();
    gotoPrompt(cursor);
    setTextCursor(cursor);
  }
  else if((e->key() == Qt::Key_E) && ctrl_pressed) {
    e->accept();
    gotoEnd(cursor, false);
    setTextCursor(cursor);
  }
  else if((e->key() == Qt::Key_F) && ctrl_pressed) {
    e->accept();
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
    setTextCursor(cursor);
  }
  else if((e->key() == Qt::Key_B) && ctrl_pressed) {
    e->accept();
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
    setTextCursor(cursor);
  }
  else if((e->key() == Qt::Key_D) && ctrl_pressed) {
    e->accept();
    cursor.deleteChar();
    setTextCursor(cursor);
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
  // Display the prompt again
  if(showPrompt)
    displayPrompt(true);
}

int QConsole::saveContents(QString fileName) {
  quitPager = true;
  flushOutput();		// get anything pending
  QCoreApplication::processEvents();
  quitPager = false;
  QFile f(fileName);
  if (!f.open(QIODevice::WriteOnly))
    return -1;
  QTextStream ts(&f);
  ts << document()->toPlainText();
  f.close();
  return 0;
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
  QTextCursor cursor(textCursor());
  gotoEnd(cursor, false);		// goto end but don't select
  setTextCursor(cursor);
  QTextEdit::paste();
}

///////////////////////////////////////////////////////
//  these need to be overridden by implementations,

//Basically, puts the command into the history list
//And emits a signal (should be called by reimplementations)
QString QConsole::interpretCommand(QString command, int *res) {
  //Add the command to the recordedScript list
  if(command.isEmpty() || (command == "\n")) return "";
  if (*res)
    recordedScript.append(command);
  //update the history and its index
  history.append(command);
  historyIndex = history.size();
  //emit the commandExecuted signal
//   emit commandExecuted(command);
  return "";
}

// give suggestions to autocomplete a command (should be reimplemented)
// the return value of the function is the string list of all suggestions
QStringList QConsole::autocompleteCommand(QString) {
  return QStringList();
}

// note: implementations need to explicitly call this   
QStringList QConsole::autocompleteFilename(QString cmd, QString pre_fnm) {
  QString path;
  QString fnm = cmd;
  if(cmd.contains('/')) {
    int pos = cmd.lastIndexOf('/');
    path = cmd.left(pos+1);
    fnm = cmd.right(cmd.size()-1 - pos);
  }
  else if(cmd.contains('\\')) {
    int pos = cmd.lastIndexOf('\\');
    path = cmd.left(pos+1);
    fnm = cmd.right(cmd.size()-1 - pos);
  }
  int fnmlen = fnm.length();
  QStringList lst;
  QDir dir(path);
  QStringList files = dir.entryList();
  for(int i=0;i<files.size();i++) {
    QString fl = files[i];
    if(fl.left(fnmlen) == fnm)
      lst.append(pre_fnm + path + fl);
  }
  return lst;
}

static QString StringIntersect(QString str1, QString str2) {
  int mxlen = qMin(str1.length(), str2.length());
  int i;
  for(i=0;i<mxlen;i++) {
    if(str1[i] != str2[i]) break;
  }
  if(i > 0)
    return str1.left(i);
  return "";
}

QString QConsole::findIntersection(const QStringList& lst) {
  if(lst.size() == 0) return "";
  if(lst.size() == 1) return lst[0];
  QString isect = lst[0];
  for(int i=1;i<lst.size();i++) {
    isect = StringIntersect(isect, lst[i]);
  }
  return isect;
}
  
//default implementation: command always complete
bool QConsole::isCommandComplete(QString cmd) {
//   if(cmd.isEmpty()) return false;
  return true;
}

// implementation should do something here..
void QConsole::ctrlCPressed() {
}
