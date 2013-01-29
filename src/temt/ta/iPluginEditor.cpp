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

#include "iPluginEditor.h"
#include <taiMisc>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <iSplitter>
#include <iMainWindowViewer>
#include <NumberedTextView>
#include <iDataPanel>
#include <iTextEdit>
#include <taPlugins>
#include <QKeyEvent>

#include <taMisc>

using namespace std;

iPluginEditor* iPluginEditor::New(const String& dir, const String& file_bse) {
  iPluginEditor* pe = new iPluginEditor();
  pe->dir_path = dir;
  pe->file_base = file_bse;

  pe->LoadFiles();

  iSize sz = taiM->dialogSize(taiMisc::dlgBig);
  pe->resize(sz.width(), (int)(1.2f * (float)sz.height())); // a bit bigger than .6h
  pe->show();

  return pe;
}

// note: we parent to main_win so something will delete it
iPluginEditor::iPluginEditor()
:inherited(taiMisc::main_window)
{
  init();
}

iPluginEditor::~iPluginEditor() {
}

void iPluginEditor::init() {
  this->setAttribute(Qt::WA_DeleteOnClose, true); // kill on close
  this->setWindowTitle("Plugin Editor");

  main_widg = new QWidget();
  main_vbox = new QVBoxLayout(main_widg);
  main_vbox->setMargin(0);

  tool_box = new QHBoxLayout();
  main_vbox->addLayout(tool_box);

  tool_bar = new QToolBar(main_widg);
  tool_box->addWidget(tool_bar);

  actSave = tool_bar->addAction("Save");
  actSave->setToolTip("Save source code to files");

  actCompile = tool_bar->addAction("Compile");
  actCompile->setShortcut(QKeySequence("Ctrl+M"));
  actCompile->setToolTip("Compile (Make) plugin from current source code -- assumes the build directory exists and has been made previously -- if unsure, use ReBuild first -- does a Save first before compiling");

  actReBuild = tool_bar->addAction("ReBuild");
  actReBuild->setToolTip("ReBuild (Make) plugin from current source code, starting everything from scratch at the beginning, including a make clean -- avoids any out-of-date build stuff messing up the compile -- does a Save first before compiling");

  split = new iSplitter(main_widg);
  main_vbox->addWidget(split);

  hfile_view = new NumberedTextView;
  cfile_view = new NumberedTextView;

  hfile_view->installEventFilter(this);
  cfile_view->installEventFilter(this);

  hfile_view->textEdit()->setLineWrapMode(QTextEdit::WidgetWidth);
  cfile_view->textEdit()->setLineWrapMode(QTextEdit::WidgetWidth);
  hfile_view->textEdit()->setTabStopWidth(40);
  cfile_view->textEdit()->setTabStopWidth(40);

  split->addWidget(hfile_view);
  split->addWidget(cfile_view);

  setCentralWidget(main_widg);

  connect(actSave, SIGNAL(triggered()), this, SLOT(save_clicked()) );
  connect(actCompile, SIGNAL(triggered()), this, SLOT(compile_clicked()) );
  connect(actReBuild, SIGNAL(triggered()), this, SLOT(rebuild_clicked()) );
}

void iPluginEditor::save_clicked() {
  SaveFiles();
}

void iPluginEditor::compile_clicked() {
  SaveFiles();
  Compile();
}

void iPluginEditor::rebuild_clicked() {
  SaveFiles();
  ReBuild();
}

void iPluginEditor::LoadFiles() {
  fstream fsrch;
  String hfnm = dir_path + "/" + file_base + ".h";
  fsrch.open(hfnm, ios::in);
  if(!fsrch.good()) {
    taMisc::Error("Could not open file name for saving", hfnm);
    return;
  }
  String hstr;
  hstr.Load_str(fsrch);
  fsrch.close();
  hfile_view->textEdit()->setPlainText(hstr);

  fstream fsrcc;
  String cfnm = dir_path + "/" + file_base + ".cpp";
  fsrcc.open(cfnm, ios::in);
  if(!fsrcc.good()) {
    taMisc::Error("Could not open file name for saving", cfnm);
    return;
  }
  String cstr;
  cstr.Load_str(fsrcc);
  fsrcc.close();
  cfile_view->textEdit()->setPlainText(cstr);
}

void iPluginEditor::SaveFiles() {
  fstream fsrch;
  String hfnm = dir_path + "/" + file_base + ".h";
  fsrch.open(hfnm, ios::out);
  if(!fsrch.good()) {
    taMisc::Error("Could not open file name for saving", hfnm);
    return;
  }
  String hstr = hfile_view->textEdit()->toPlainText();
  hstr.Save_str(fsrch);
  fsrch.close();

  fstream fsrcc;
  String cfnm = dir_path + "/" + file_base + ".cpp";
  fsrcc.open(cfnm, ios::out);
  if(!fsrcc.good()) {
    taMisc::Error("Could not open file name for saving", cfnm);
    return;
  }
  String cstr = cfile_view->textEdit()->toPlainText();
  cstr.Save_str(fsrcc);
  fsrcc.close();
}

void iPluginEditor::Compile() {
  taPlugins::MakePlugin(dir_path, file_base, false, false); // false = assume user, false = no full rebuild
}

void iPluginEditor::ReBuild() {
  taPlugins::MakePlugin(dir_path, file_base, false, true); // false = assume user, true = full rebuild
}

void iPluginEditor::closeEvent(QCloseEvent* ev) {
  int chs = taMisc::Choice("Closing editor -- Save Files first?", "Save Files", "Discard Changes");
  if(chs == 0)
    SaveFiles();
  inherited::closeEvent(ev);
}

bool iPluginEditor::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }
  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed) {
    switch(e->key()) {
    case Qt::Key_O:
      if(hfile_view->hasFocus() || hfile_view->textEdit()->hasFocus())
        cfile_view->textEdit()->setFocus();
      else
        hfile_view->textEdit()->setFocus();
      return true;              // we absorb this event
    case Qt::Key_M:
      Compile();
      return true;
    }
  }
  return inherited::eventFilter(obj, event);
}

