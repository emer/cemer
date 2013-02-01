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

#ifndef iPluginEditor_h
#define iPluginEditor_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QMainWindow>
#endif

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class iSplitter; //
class iNumberedTextView; //
class QVBoxLayout; // 
class QHBoxLayout; // 


class TA_API iPluginEditor: public QMainWindow {
  // editor window for plugins
INHERITED(QMainWindow)
  Q_OBJECT
public:
#ifndef __MAKETA__
  enum Roles { // extra roles, for additional data, etc.
    ObjUrlRole = Qt::UserRole + 1, // Url stored in this
//    ObjCatRole  // for object category string, whether shown or not
  };
#endif

  String                dir_path;       // path to directory with files
  String                file_base;      // base name of files to edit

  static iPluginEditor* New(const String& dir, const String& file_bse);
  // main interface -- make a new editor

  virtual void          LoadFiles(); // load files into editors
  virtual void          SaveFiles(); // save files from editors
  virtual void          Compile(); // compile
  virtual void          ReBuild(); // full rebuild

#ifndef __MAKETA__
public:

  QWidget*              main_widg;
  QVBoxLayout*          main_vbox;
  iSplitter*            split;
  iNumberedTextView*     hfile_view; // header file
  iNumberedTextView*     cfile_view; // cpp file
  QHBoxLayout*          tool_box;
  QToolBar*             tool_bar;
  QAction*                actSave;
  QAction*                actCompile;
  QAction*                actReBuild;

protected:

  iPluginEditor();
  ~iPluginEditor();

  override void closeEvent(QCloseEvent* ev);
  override bool eventFilter(QObject *obj, QEvent *event);

protected slots:
  void                  save_clicked();
  void                  compile_clicked();
  void                  rebuild_clicked();

private:
  void          init(); // called by constructors
#endif // !__MAKETA__
};

#endif // iPluginEditor_h
