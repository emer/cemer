// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "iConsole.h"

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

#include <taMisc>
#include <taiMisc> // for taiMisc::KeyEventCtrlPressed(e)

#include <qfile.h>

#include <QMouseEvent>
#include <QTextStream>
#include <QKeyEvent>
#include <QTextCursor>
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QMenu>
#include <QScrollBar>
#include <QMimeData>
#include <QTextDocumentFragment>
#include <QClipboard>
#include <QDesktopServices>

//#include <QDebug>

#include <iostream>


// buried in qtextcontrol_p.h
class MyQTextEditMimeData : public QMimeData
{
public:
    inline MyQTextEditMimeData(const QTextDocumentFragment &aFragment) : fragment(aFragment) {}

    virtual QStringList formats() const;
protected:
    virtual QVariant retrieveData(const QString &mimeType, QVariant::Type type) const;
private:
    void setup() const;

    mutable QTextDocumentFragment fragment;
};

QStringList MyQTextEditMimeData::formats() const
{
    if (!fragment.isEmpty())
        return QStringList() << QString::fromLatin1("text/plain") << QString::fromLatin1("text/html")
        ;
    else
        return QMimeData::formats();
}

QVariant MyQTextEditMimeData::retrieveData(const QString &mimeType, QVariant::Type type) const
{
    if (!fragment.isEmpty())
        setup();
    return QMimeData::retrieveData(mimeType, type);
}

void MyQTextEditMimeData::setup() const
{
    MyQTextEditMimeData *that = const_cast<MyQTextEditMimeData *>(this);
    that->setData(QLatin1String("text/html"), fragment.toHtml("utf-8").toUtf8());
    that->setText(fragment.toPlainText());
    fragment = QTextDocumentFragment();
}


using namespace std;


void iConsole::setFontNameSize(QString fnm, int sz) {
  setFontFamily(fnm);
  setFontPointSize(sz);
  QFont font(fnm, sz);
  setFont(font);
}

void iConsole::setFont(const QFont& font) {
  setCurrentFont(font);
  document()->setDefaultFont(font);
  getDisplayGeom();
}

void iConsole::setPager(bool pager) {
  noPager = !pager;
}

void iConsole::getDisplayGeom() {
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
void iConsole::clear() {
  inherited::clear();
  ext_select_on = false;
  getDisplayGeom();
  curPromptPos = 0;
  curOutputLn = 0;
  quitPager = false;
  contPager = false;
  waiting_for_key = false;
  key_response = 0;
  setAcceptRichText(true);     // just plain
  setReadOnly(false);           // this determines if links are clickable
  setOpenExternalLinks(false);
  setOpenLinks(false);          // we do it ourselves b/c it doesn't seem to work otherwise
  displayPrompt(true);          // force
}

//Reset the console
void iConsole::reset() {
  clear();
  //init attributes
  historyIndex = 0;
  history.clear();
  recordedScript.clear();
  promptDisp = false;
}

void iConsole::exit() {
  qApp->exit();
}

void iConsole::onQuit() {
  applicationIsQuitting = true;
}

//iConsole constructor (init the QTextEdit & the attributes)
iConsole::iConsole(QWidget *parent, const char *name, bool initiInterceptor)
  : inherited(parent)
  , cmdColor(Qt::blue)
  , errColor(Qt::red)
  , outColor(Qt::black)
  , completionColor(Qt::darkGreen)
  , applicationIsQuitting(false)
#ifndef TA_OS_WIN
  , stdoutiInterceptor(NULL), stderriInterceptor(NULL)
#endif
{
  //resets the console
  reset();

  connect(this, SIGNAL(anchorClicked(const QUrl&)), SLOT(linkClicked(const QUrl&)));

#ifndef TA_OS_WIN
  if(initiInterceptor) {
    //Initialize the interceptors
    stdoutiInterceptor = new iInterceptor(this);
    stdoutiInterceptor->initialize(1);
    connect(stdoutiInterceptor, SIGNAL(received(QTextStream *)), SLOT(stdReceived()));

    stderriInterceptor = new iInterceptor(this);
    stderriInterceptor->initialize(2);
    connect(stderriInterceptor, SIGNAL(received(QTextStream *)), SLOT(stdReceived()));
  }
#endif
}

//Sets the prompt and cache the prompt length to optimize the processing speed
void iConsole::setPrompt(QString newPrompt, bool display) {
  prompt = newPrompt;
  promptLength = prompt.length();
  //display the new prompt
  if (display)
    displayPrompt();
}

void iConsole::flushOutput() {
  cout.flush();
  cerr.flush();
#ifndef TA_OS_WIN
  bool waiting = false;
  do {
    if(stdoutiInterceptor) {
      setTextColor(outColor);
      waiting = stdDisplay(stdoutiInterceptor->textIStream());
    }
    if(stderriInterceptor) {
      setTextColor(errColor);
      waiting = (stdDisplay(stderriInterceptor->textIStream()) || waiting);
    }
    if(waiting) {
      QCoreApplication::processEvents();
      taMisc::SleepMs(1); //note: 1ms is fine, shorter values result in cpu thrashing
    }
  } while(waiting);
#endif
}

int iConsole::queryForKeyResponse(QString query) {
  flushOutput();
  QTextCursor cursor(textCursor());
  gotoEnd(cursor, false);
  append(query);
  waiting_for_key = true;
  key_response = 0;
  while (waiting_for_key && !applicationIsQuitting) {
    QCoreApplication::processEvents();
    taMisc::SleepMs(10); //note: 1ms is fine, shorter values result in cpu thrashing
    // keypress event turns off waiting_for_key and sets key_response
  }
  // get rid of prompt
  cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
  cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
  cursor.removeSelectedText();
  cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
  cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
  cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  cursor.removeSelectedText();
  return key_response;
}

void iConsole::stdReceived() {
  flushOutput();
}

// Displays the prompt and move the cursor to the end of the line.
void iConsole::displayPrompt(bool force) {
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
  quitPager = false;            // reset this flag whenver prompt returns
  contPager = false;
  promptDisp = true;
}

void iConsole::gotoPrompt(QTextCursor& cursor, bool select) {
  if(select)
    cursor.setPosition(curPromptPos, QTextCursor::KeepAnchor);
  else 
    cursor.setPosition(curPromptPos, QTextCursor::MoveAnchor);
}

void iConsole::gotoEnd(QTextCursor& cursor, bool select) {
  if(select)
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor); // selects
  else
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor); // not selects
}

void iConsole::gotoEnd() {
  QTextCursor cursor(textCursor());
  gotoEnd(cursor, false);
  setTextCursor(cursor);
}

bool iConsole::scrolledToEnd() {
  // check if scrollbar is scrolled to end
  QScrollBar* vscr = verticalScrollBar();
  if(!vscr) return true;
  return (vscr->value() >= vscr->maximum() - 4); // give a few lines at the end leeway
}

void iConsole::outputLine(QString line, bool err) {
  bool scrolled_to_end = scrolledToEnd();

  // reset any residual formatting -- otherwise links carry over..
  QTextCharFormat nf;
  setCurrentCharFormat(nf);

  if(err) {
    setTextColor(errColor);
  }
  else {
    setTextColor(outColor);
  }
  promptDisp = false;
  append(line);
  setTextColor(outColor);       // reset to default
  if(scrolled_to_end) {         // keep on keeping on..
    gotoEnd();
  }
  emit receivedNewStdin(1);
  QCoreApplication::processEvents();
}

#ifndef TA_OS_WIN
// displays redirected stdout/stderr
bool iConsole::stdDisplay(QTextStream* s) {
  if((curOutputLn >= maxLines) && !contPager && !noPager)
    return true;
  bool scrolled_to_end = scrolledToEnd();
  int n_lines_recvd = 0;
  if(noPager) {
    // no pager mode just grabs everything and returns true if any lines were recv'd
    while(true) {
      QString line = s->readLine(maxCols);
      if(line.isNull()) break;
      n_lines_recvd++;
      promptDisp = false;
      append(line);
      if(logfile.isOpen()) {
        logfile.write(line.toLocal8Bit());
        logfile.write("\n", strlen("\n"));
        logfile.flush();
      }
    }
  }
  else {
    // pager mode has more complicated logic
    while((curOutputLn < maxLines) || contPager || quitPager) {
      QString line = s->readLine(maxCols);
      if(line.isNull()) break;
      n_lines_recvd++;
      if(!quitPager) {
        promptDisp = false;
        append(line);
        if(logfile.isOpen()) {
          logfile.write(line.toLocal8Bit());
          logfile.write("\n", strlen("\n"));
          logfile.flush();
        }
        if(!contPager) {
          curOutputLn++;
          if(curOutputLn >= maxLines) {
            append("---Press Return for More, q=quit displaying, c=continue without paging ---");
            viewport()->update();       // repaint the window, in case it is weird
            return true;
          }
        }
      }
    }
  }

  if(scrolled_to_end) {         // keep on keeping on..
    gotoEnd();
  }
  emit receivedNewStdin(n_lines_recvd);
  return (n_lines_recvd > 0);
}
#endif

void iConsole::resizeEvent(QResizeEvent* e) {
  getDisplayGeom();
  inherited::resizeEvent(e);
}

// Reimplemented key press event
void iConsole::keyPressEvent(QKeyEvent* e)
{
  QTextCursor cursor(textCursor());

  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  bool is_enter = e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return;

  if (curOutputLn >= maxLines) {
    if (is_enter) {
      curOutputLn = 0;
    }
    else if (e->key() == Qt::Key_Q) {
      curOutputLn = 0;
      quitPager = true;
    }
    else if (e->key() == Qt::Key_C) {
      curOutputLn = 0;
      contPager = true;
    }
    flushOutput();
    promptDisp = false;
    e->accept();
    return;
  }

  QTextCursor::MoveMode mv_mode = ext_select_on ? QTextCursor::KeepAnchor :
    QTextCursor::MoveAnchor;

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
      displayPrompt(true);      // force!
      cursor.insertText(command);
    }
  }
  else if(e->key() == Qt::Key_Backspace) { // todo: trap other cases..
    if(cursorInCurrentCommand()) // don't backup into prompt
      inherited::keyPressEvent( e );
    else
      e->accept();
  }
  else if(e->key() == Qt::Key_Return) {
    e->accept();
    if(waiting_for_key) {
      key_response = e->key();
      waiting_for_key = false;
    }
    else {
      if(promptDisp) {
        QString command = getCurrentCommand();
        if(isCommandComplete(command))
          execCommand(command, false);
      }
      else {
        displayPrompt(true);
      }
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
  else if((e->key() == Qt::Key_Space) && ctrl_pressed) {
    e->accept();
    cursor.clearSelection();
    ext_select_on = true;
  }
  else if((e->key() == Qt::Key_G) && ctrl_pressed) {
    e->accept();
    cursor.clearSelection();
    ext_select_on = false;
  }
  else if((e->key() == Qt::Key_A) && ctrl_pressed) {
    e->accept();
    gotoPrompt(cursor, ext_select_on);
    setTextCursor(cursor);
  }
  else if((e->key() == Qt::Key_E) && ctrl_pressed) {
    e->accept();
    gotoEnd(cursor, ext_select_on);
    setTextCursor(cursor);
  }
  else if((e->key() == Qt::Key_F) && ctrl_pressed) {
    e->accept();
    cursor.movePosition(QTextCursor::NextCharacter, mv_mode);
    setTextCursor(cursor);
  }
  else if((e->key() == Qt::Key_B) && ctrl_pressed) {
    e->accept();
    cursor.movePosition(QTextCursor::PreviousCharacter, mv_mode);
    setTextCursor(cursor);
  }
  else if((e->key() == Qt::Key_D) && ctrl_pressed) {
    e->accept();
    cursor.deleteChar();
    setTextCursor(cursor);
  }
  else if((e->key() == Qt::Key_K) && ctrl_pressed) {
    e->accept();
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QTextDocumentFragment frag = cursor.selection();
    MyQTextEditMimeData* md = new MyQTextEditMimeData(frag);
    QApplication::clipboard()->setMimeData(md);
    cursor.removeSelectedText();
    ext_select_on = false;
  }
  else if((e->key() == Qt::Key_Y) && ctrl_pressed) {
    e->accept();
    inherited::paste();         // don't go to end first!
    ext_select_on = false;
  }
  else if((e->key() == Qt::Key_W) && ctrl_pressed) {
    e->accept();
    cut();
    ext_select_on = false;
  }
  else if(waiting_for_key) {
    key_response = e->key();
    waiting_for_key = false;
    e->accept();
  }
  else {
    inherited::keyPressEvent( e );
  }

  // make sure we never go past prompt..
  QTextCursor end_cursor(textCursor());
  if(end_cursor.position() < curPromptPos) {
    gotoPrompt(end_cursor);
    setTextCursor(end_cursor);
  }
}

void iConsole::mousePressEvent(QMouseEvent *e) {
  setReadOnly(true);            // this is key for allowing links to be clicked
  inherited::mousePressEvent(e);
}

void iConsole::mouseMoveEvent(QMouseEvent *e) {
  inherited::mouseMoveEvent(e);
}

void iConsole::mouseReleaseEvent(QMouseEvent *e) {
  inherited::mouseReleaseEvent(e);
  setReadOnly(false);           // undo the RO for link clicking
#if defined(TA_OS_MAC) || defined(TA_OS_WIN)
  if(e->button() & Qt::MidButton) {
    paste();
  }
  else
#endif
 if(e->button() & Qt::LeftButton) {
    copy();                     // always copy!
  }
  // this is actually confusing to people -- just let it be..
//   QTextCursor cursor(textCursor());
//   // displays the prompt
//   gotoEnd(cursor, false);
//   setTextCursor(cursor);
}

void iConsole::contextMenuEvent(QContextMenuEvent *event) {
  QMenu* menu = createStandardContextMenu();
  menu->addSeparator();
  menu->addAction("&Clear All", this, SLOT(clear()),  QKeySequence("Ctrl+L"));
  menu->exec(event->globalPos());
  delete menu;
}

//Get the current command
QString iConsole::getCurrentCommand() {
  QTextCursor cursor = textCursor();
  gotoPrompt(cursor);
  gotoEnd(cursor, true);
  QString command = cursor.selectedText();
  if(command.isNull() || command.isEmpty()) command = "\n";
  cursor.clearSelection();
  return command;
}

//Replace current command with a new one
void iConsole::replaceCurrentCommand(QString newCommand) {
  QTextCursor cursor = textCursor();
  gotoPrompt(cursor);
  gotoEnd(cursor, true); // select
  cursor.insertText(newCommand);                                   // replaces
  cursor.clearSelection();
}

bool iConsole::cursorInCurrentCommand() {
  QTextCursor cursor = textCursor();
  int pos = cursor.position();
  return (pos > curPromptPos);
}

//execCommand(QString) executes the command and displays back its result
void iConsole::execCommand(QString command, bool writeCommand, bool showPrompt) {
  //Display the prompt with the command first
  if (writeCommand) {
    if(getCurrentCommand() != "")
      displayPrompt(true);      // force
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

int iConsole::saveContents(QString fileName) {
  quitPager = true;
  flushOutput();                // get anything pending
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
int iConsole::saveScript(QString fileName) {
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
int iConsole::loadScript(QString fileName) {
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

int iConsole::setStdLogfile(QString fileName) {
  if(logfile.isOpen()) {
    if(fileName.isNull())
      logfile.close();
    return 1;
  }
  logfile.setFileName(fileName);
  if (!logfile.open(QIODevice::WriteOnly))
    return -1;
  return 0;
}

//Allows pasting with middle mouse button (x window)
//when clicking outside of the edition zone
void iConsole::paste() {
  gotoEnd();
  inherited::paste();
}

///////////////////////////////////////////////////////
//  these need to be overridden by implementations,

//Basically, puts the command into the history list
//And emits a signal (should be called by reimplementations)
QString iConsole::interpretCommand(QString command, int *res) {
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
QStringList iConsole::autocompleteCommand(QString) {
  return QStringList();
}

// note: implementations need to explicitly call this
QStringList iConsole::autocompleteFilename(QString cmd, QString pre_fnm) {
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

QString iConsole::findIntersection(const QStringList& lst) {
  if(lst.size() == 0) return "";
  if(lst.size() == 1) return lst[0];
  QString isect = lst[0];
  for(int i=1;i<lst.size();i++) {
    isect = StringIntersect(isect, lst[i]);
  }
  return isect;
}

void iConsole::linkClicked(const QUrl & link) {
  // QString path = link.path(); // will only be part before #, if any
  // outputLine(path, true);
  QDesktopServices::openUrl(link);
}

//default implementation: command always complete
bool iConsole::isCommandComplete(QString cmd) {
//   if(cmd.isEmpty()) return false;
  return true;
}

// implementation should do something here..
void iConsole::ctrlCPressed() {
}
