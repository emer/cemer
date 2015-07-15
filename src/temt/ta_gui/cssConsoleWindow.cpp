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

#include "cssConsoleWindow.h"
#include <css_qtconsole.h>
#include <QApplication>
#include <tabMisc>
#include <taRootBase>
#include <taProject>
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <iAction>
#include <iRect>
#include <QIcon>
#include <iClipData>
#include <taiMisc>
#include <taiWidgetMenu>
#include <taiWidgetMenuBar>
#include <taMisc>
#include <taiMisc>

#include <QDateTime>
#include <QMenuBar>
#include <QMenu>
#include <QTextCursor>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QToolBar>
#include <QClipboard>

cssConsoleWindow::cssConsoleWindow(QWidget* parent) : inherited(parent) {
  lock_to_proj = true;
  self_resize_timestamp = 1;

  css_con = QcssConsole::getInstance(NULL, cssMisc::TopShell);

  QWidget* bod = new QWidget(this);
  setCentralWidget(bod);
  QHBoxLayout* hb = new QHBoxLayout(bod);
  hb->setMargin(0); hb->setSpacing(0);
  QToolBar* tb = new QToolBar(bod);
  tb->setOrientation(Qt::Vertical);
  tb->setMovable(false);
  tb->setFloatable(false);
  tb->setIconSize(QSize(16,16));

  hb->addWidget(tb);
  hb->addWidget(css_con);

  menu_bar = new taiWidgetMenuBar(bod, taiMisc::fonBig, menuBar());
  edit_menu = menu_bar->AddSubMenu("&Edit");
  window_menu = menu_bar->AddSubMenu("&Window");

  unpinned = new QIcon(":/images/tab_unpushed.png");
  pinned = new QIcon(":/images/tab_locked.png");
  clear_icon = new QIcon(":/images/clear.png");
  select_all_icon = new QIcon(":/images/select_all.png");

  pin_act = AddAction(new iAction("&pin", QKeySequence(), "pin"));
  pin_act->setIcon(*pinned);
  pin_act->setToolTip(taiMisc::ToolTipPreProcess("Toggle between being locked to bottom of project window, or not -- lock = locked (click to unlock), pin = unlocked (click to lock)"));
  tb->addAction(pin_act);
    
  tb->addSeparator();

  editCutAction = new iAction(iClipData::EA_CUT, "Cu&t",
                                       QKeySequence("Ctrl+X"), "editCutAction");
  editCutAction->setIcon(QIcon(":/images/editcut.png"));
  tb->addAction(editCutAction);
  edit_menu->AddAction(editCutAction);
  connect(editCutAction, SIGNAL(Action()), css_con, SLOT(cut()));

  editCopyAction = new iAction(iClipData::EA_COPY, "&Copy",
                                        QKeySequence("Ctrl+C"), "editCopyAction");
  editCopyAction->setIcon(QIcon(":/images/editcopy.png"));
  tb->addAction(editCopyAction);
  edit_menu->AddAction(editCopyAction);
  connect(editCopyAction, SIGNAL(Action()), css_con, SLOT(copy()));

  editPasteAction = new iAction(iClipData::EA_PASTE, "&Paste",
                                         QKeySequence("Ctrl+V"), "editPasteAction");
  editPasteAction->setIcon(QIcon(":/images/editpaste.png"));
  tb->addAction(editPasteAction);
  edit_menu->AddAction(editPasteAction);
  connect(editPasteAction, SIGNAL(Action()), css_con, SLOT(paste()));
    
  tb->addSeparator();

  clear_act = new iAction("&Clear", QKeySequence("Ctrl+."), "clear_act");
  clear_act->setIcon(*clear_icon);
  clear_act->setToolTip(taiMisc::ToolTipPreProcess("Clear the console window"));
  tb->addAction(clear_act);
  edit_menu->AddAction(clear_act);
  connect(clear_act, SIGNAL(Action()), css_con, SLOT(clear()));
    
  select_all_act = new iAction("&Select All", QKeySequence(), "select_all_act");
  select_all_act->setIcon(*select_all_icon);
  select_all_act->setToolTip(taiMisc::ToolTipPreProcess("Select all contents of console window"));
  tb->addAction(select_all_act);
  edit_menu->AddAction(select_all_act);
  connect(select_all_act, SIGNAL(Action()), css_con, SLOT(selectAll()));
    
  window_menu->AddAction(pin_act);
  window_menu->AddSep();

  // actions for dynamically built window menu need to be added to our own action list
  // so the code for these is a bit different
  window_min_action = AddAction(new iAction("&Minimize", QKeySequence(), "window_min_action"));
  window_zoom_action = AddAction(new iAction("&Zoom", QKeySequence(), "window_zoom_action"));
  window_menu->AddAction(window_min_action);
  window_menu->AddAction(window_zoom_action);
    
  connect(window_menu->menu(), SIGNAL(aboutToShow()), this, SLOT(windowMenu_aboutToShow()));
  connect(pin_act, SIGNAL(Action()), this, SLOT(PinAction()));
  connect(window_min_action, SIGNAL(Action()), this, SLOT(showMinimized()));
  connect(window_zoom_action, SIGNAL(Action()), this, SLOT(showMaximized()));

  setWindowTitle("css Console");
}

cssConsoleWindow::~cssConsoleWindow() {
  delete pinned;
  delete unpinned;
  delete pin_act;
  delete clear_act;
  delete select_all_act;
}

void cssConsoleWindow::UpdateFmLock() {
  if(lock_to_proj) 
    pin_act->setIcon(*pinned);
  else
    pin_act->setIcon(*unpinned);

  if(lock_to_proj) {
    taProject* proj = tabMisc::root->projects.DefaultEl();
    if(proj) {
      MainWindowViewer* mwv = proj->GetDefaultProjectBrowser();
      if(mwv && mwv->widget()) {
        mwv->widget()->AlignCssConsole();
      }
    }
  }
  else {
    LoadGeom();
  }
}

void cssConsoleWindow::SaveGeom() {
  iRect r = frameGeometry(); //note: same as size() for widgets
  // convert from screen coords to relative (note, allowed to be >1.0)
  // adjust for scrn geom, esp for evil mac
  float lft = (float)(r.left() - taiM->scrn_geom.left()) /
    (float)(taiM->scrn_s.w); // all of these convert from screen coords
  float top = (float)(r.top() - taiM->scrn_geom.top()) / (float)(taiM->scrn_s.h);
  float wd = (float)r.width() / (float)(taiM->scrn_s.w);
  float ht = (float)r.height() / (float)(taiM->scrn_s.h);
  
  tabMisc::root->console_size.SetXY(wd, ht);
  tabMisc::root->console_pos.SetXY(lft, top);
}

void cssConsoleWindow::StartSelfResize() {
  // note: resize/move combo recommended by Qt
  QDateTime tm = QDateTime::currentDateTime();
  self_resize_timestamp = tm.toTime_t();
}

bool cssConsoleWindow::CheckSelfResize() {
  QDateTime tm = QDateTime::currentDateTime();
  int64_t cur_ts = tm.toTime_t();
  // 9/15/14 - changed to 1 second
  if(cur_ts - self_resize_timestamp < 1) // 10 seconds to resize..
    return true;
  return false;
}

void cssConsoleWindow::LoadGeom() {
  if(tabMisc::root->console_size == 0.0f) return;

  float lft = tabMisc::root->console_pos.x;
  float top = tabMisc::root->console_pos.y;
  float wd = tabMisc::root->console_size.x;
  float ht = tabMisc::root->console_size.y;

  iSize s((int)(wd * taiM->scrn_s.w), (int)(ht * taiM->scrn_s.h));
  s.h -= taiM->frame_s.h;
  s.w -= taiM->frame_s.w;
  iRect tr = iRect(
    (int)((lft * taiM->scrn_s.w)) + taiM->scrn_geom.left(),
    (int)((top * taiM->scrn_s.h)) + taiM->scrn_geom.top(),
    s.w,
    s.h
  );

  StartSelfResize();
  resize(tr.w, tr.h);
  move(tr.x, tr.y);
}

void cssConsoleWindow::PinAction() {
  lock_to_proj = !lock_to_proj;
  UpdateFmLock();
}

void cssConsoleWindow::LockedNewGeom(int left, int top, int width, int height) {
  if(!lock_to_proj) return;
  StartSelfResize();
  resize(width, height);
  move(left, top);
  css_con->gotoEnd();
}

void cssConsoleWindow::resizeEvent(QResizeEvent* e) {
  inherited::resizeEvent(e);
  if(CheckSelfResize()) return;
  
  taProject* proj = tabMisc::root->projects.DefaultEl();
  if(!proj) return;             // only functions if there is a project
  
  if(!lock_to_proj) {
    SaveGeom();
  }
  else {
    lock_to_proj = false;
    UpdateFmLock();
  }
}

void cssConsoleWindow::moveEvent(QMoveEvent* e) {
  inherited::moveEvent(e);
  if(CheckSelfResize()) return;

  taProject* proj = tabMisc::root->projects.DefaultEl();
  if(!proj) return;             // only functions if there is a project

  if(!lock_to_proj) {
    SaveGeom();
  }
  else {
    lock_to_proj = false;
    UpdateFmLock();
  }
}

void cssConsoleWindow::closeEvent(QCloseEvent* e) {
  if(taMisc::quitting == taMisc::QF_RUNNING) {
    e->ignore();  // do NOT close the window!
    showMinimized();
  }
  else {
    inherited::closeEvent(e);
  }
}

iAction* cssConsoleWindow::AddAction(iAction* act) {
  actions.Add(act); // refs the act
  // note: don't parent Actions because we manage them manually in our lists
  // note: because Qt only activates acts with shortcuts if visible, we need
  // to add the shorcutted guys to something visible... how about... us!
  if (!act->shortcut().isEmpty())
    this->addAction(act);
  return act;
}

void cssConsoleWindow::windowMenu_aboutToShow() {
  // Clear and rebuild submenu.
  window_menu->Reset();
  if(lock_to_proj)
    pin_act->setText("Unlock");
  else
    pin_act->setText("Lock To Project");
  window_menu->AddAction(pin_act);
  window_menu->AddAction(window_min_action);
  window_menu->AddAction(window_zoom_action);
  window_menu->AddSep();
  // Populate with current windows.
  iWidget_List wl;
  taiMisc::GetWindowList(wl);
  for (int i = 0; i < wl.size; ++i) {
    QWidget* wid = wl.FastEl(i);
    if (wid->isWindow()) {
      String title = wid->windowTitle();
      String label = title;
      if (wid->isWindowModified()) {
        label = label + "*";
      }
      window_menu->AddItem(label, iAction::var_act,
                      this, SLOT(windowActivateByName(const Variant&)), title);
    }
  }
}

void cssConsoleWindow::windowActivateByName(const Variant& title_) {
  iWidget_List wl;
  taiMisc::GetWindowList(wl);
  String title = title_.toString();
  QWidget* wid;
  for (int i = 0; i < wl.size; ++i) {
    wid = wl.FastEl(i);
    if (wid->isWindow()) {
      String wid_title = wid->windowTitle();
      if (title == wid_title)
        break;
    }
  }
  if (!wid)
    return;
  if (wid->isMinimized()) {
    wid->showMaximized();
  }
  wid->activateWindow();
  wid->raise();
}

void cssConsoleWindow::changeEvent(QEvent* ev) {
  if(ev->type() == QEvent::ActivationChange) {
    if (isActiveWindow()) {
      UpdateUi();
    }
  }
  inherited::changeEvent(ev);
}

void cssConsoleWindow::UpdateUi() {
}