// Copyright 2017, Regents of the University of Colorado,
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
#include <QShowEvent>
#include <QTimer>
#include <QKeyEvent>

cssConsoleWindow::cssConsoleWindow(QWidget* parent) : inherited(parent) {
  lock_to_proj = true;

  css_con = QcssConsole::getInstance(NULL, cssMisc::TopShell);
  
  setWindowTitle("css Console");

  QWidget* central_widget = new QWidget(this);
  setCentralWidget(central_widget);
  QVBoxLayout* outer_layout = new QVBoxLayout(central_widget); // overall layout is vertical
  outer_layout->setMargin(0);
  outer_layout->setSpacing(0);

  // add a toolbar widget to the central widget
  outer_layout->addWidget(&toolbar_widget);
  
  toolbar_layout.setMargin(0);
  toolbar_layout.setSpacing(0);
  toolbar_widget.setLayout(&toolbar_layout);
  
  // add a toolbar to the toolbar widget
  toolbar.setOrientation(Qt::Vertical);
  toolbar.setMovable(false);
  toolbar.setFloatable(false);
  toolbar.setIconSize(QSize(16,16));
  toolbar_layout.addWidget(&toolbar);
  
  // add the console editor
  toolbar_layout.addWidget(css_con);
  
  menu_bar = new taiWidgetMenuBar(central_widget, taiMisc::fonBig, menuBar());
  edit_menu = menu_bar->AddSubMenu("&Edit");
  window_menu = menu_bar->AddSubMenu("&Window");

  unpinned = new QIcon(":/images/tab_unpushed.png");
  pinned = new QIcon(":/images/tab_locked.png");
  clear_icon = new QIcon(":/images/clear.png");
  select_all_icon = new QIcon(":/images/select_all.png");
  search_icon = new QIcon(":/images/find_icon.png");

  pin_act = AddAction(new iAction("&pin", QKeySequence(), "pin"));
  pin_act->setIcon(*pinned);
  pin_act->setToolTip(taiMisc::ToolTipPreProcess("Toggle between being locked to bottom of project window, or not -- lock = locked (click to unlock), pin = unlocked (click to lock)"));
  toolbar.addAction(pin_act);
  
  toolbar.addSeparator();

  editCutAction = new iAction(iClipData::EA_CUT, "Cu&t",
                                       QKeySequence("Ctrl+X"), "editCutAction");
  editCutAction->setIcon(QIcon(":/images/editcut.png"));
  toolbar.addAction(editCutAction);
  edit_menu->AddAction(editCutAction);
  connect(editCutAction, SIGNAL(Action()), css_con, SLOT(cut()));

  editCopyAction = new iAction(iClipData::EA_COPY, "&Copy",
                                        QKeySequence("Ctrl+C"), "editCopyAction");
  editCopyAction->setIcon(QIcon(":/images/editcopy.png"));
  toolbar.addAction(editCopyAction);
  edit_menu->AddAction(editCopyAction);
  connect(editCopyAction, SIGNAL(Action()), css_con, SLOT(copy()));

  editPasteAction = new iAction(iClipData::EA_PASTE, "&Paste",
                                         QKeySequence("Ctrl+V"), "editPasteAction");
  editPasteAction->setIcon(QIcon(":/images/editpaste.png"));
  toolbar.addAction(editPasteAction);
  edit_menu->AddAction(editPasteAction);
  connect(editPasteAction, SIGNAL(Action()), css_con, SLOT(paste()));
    
  toolbar.addSeparator();

  clear_act = new iAction("&Clear", QKeySequence("Ctrl+."), "clear_act");
  clear_act->setIcon(*clear_icon);
  clear_act->setToolTip(taiMisc::ToolTipPreProcess("Clear the console window"));
  toolbar.addAction(clear_act);
  edit_menu->AddAction(clear_act);
  connect(clear_act, SIGNAL(Action()), css_con, SLOT(clear()));
    
  select_all_act = new iAction("&Select All", QKeySequence(), "select_all_act");
  select_all_act->setIcon(*select_all_icon);
  select_all_act->setToolTip(taiMisc::ToolTipPreProcess("Select all contents of console window"));
  toolbar.addAction(select_all_act);
  edit_menu->AddAction(select_all_act);
  connect(select_all_act, SIGNAL(Action()), css_con, SLOT(selectAll()));
    
  show_search_act = new iAction("&Find", QKeySequence("Ctrl+F"), "show_search_act");
  show_search_act->setIcon(*search_icon);
  show_search_act->setToolTip(taiMisc::ToolTipPreProcess("Find text in this window"));
  toolbar.addAction(show_search_act);
  edit_menu->AddAction(show_search_act);
  connect(show_search_act, SIGNAL(Action()), this, SLOT(ShowSearchBar()));

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

  
  // add a search widget to the central widget
  outer_layout->addWidget(&search_widget);
  search_layout.setContentsMargins(28, 0, 0, 0);
  search_layout.setSpacing(0);
  search_widget.setLayout(&search_layout);
  
  search_layout.addWidget(&search_text_field);
  
  QPushButton* find_next_button = new QPushButton("Find Next", this);
  find_next_button->setToolTip(taiMisc::ToolTipPreProcess("Search all console text"));
  search_layout.addWidget(find_next_button);
  connect(find_next_button, SIGNAL(clicked()), this, SLOT(FindNext()));

  QPushButton* find_previous_button = new QPushButton("Find Previous", this);
  find_previous_button->setToolTip(taiMisc::ToolTipPreProcess("Search all console text"));
  search_layout.addWidget(find_previous_button);
  connect(find_previous_button, SIGNAL(clicked()), this, SLOT(FindPrevious()));

  QPushButton* clear_search_button = new QPushButton("x", this);
  clear_search_button->setToolTip(taiMisc::ToolTipPreProcess("Clear search text and highlighting"));
  search_layout.addWidget(clear_search_button);
  connect(clear_search_button, SIGNAL(clicked()), this, SLOT(ClearSearch()));

  QPushButton* hide_search_button = new QPushButton("Done", this);
  hide_search_button->setToolTip(taiMisc::ToolTipPreProcess("Hide the search bar"));
  search_layout.addWidget(hide_search_button);
  connect(hide_search_button, SIGNAL(clicked()), this, SLOT(HideSearchBar()));
  
  search_widget.hide();
  
}

cssConsoleWindow::~cssConsoleWindow() {
  delete pinned;
  delete unpinned;
  delete pin_act;
  delete clear_act;
  delete show_search_act;
  delete select_all_act;
}

void cssConsoleWindow::UpdateFmLock() {
  tabMisc::root->console_locked = lock_to_proj;

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
  if(lock_to_proj)
    return;                     // don't save when locked
  iRect r = frameGeometry(); //note: same as size() for widgets
  // if(r == prev_geom) return;    // don't double-save -- on mac when desktop widget
  // // is resizing due to sleep etc, a spurious 2nd resize can come through and
  // // resize the console relative to the wrong screen..
  // prev_geom = r;
  
  // convert from screen coords to relative (note, allowed to be >1.0)
  // adjust for scrn geom, esp for evil mac
  float lft = (float)(r.left() - taiM->scrn_geom.left()) /
    (float)(taiM->scrn_s.w); // all of these convert from screen coords
  float top = (float)(r.top() - taiM->scrn_geom.top()) / (float)(taiM->scrn_s.h);
  float wd = (float)r.width() / (float)(taiM->scrn_s.w);
  float ht = (float)r.height() / (float)(taiM->scrn_s.h);

  // taMisc::Info("save console geom: wd:", String(wd), "ht:", String(ht),
  //              "raw w:", String(r.width()), "h:", String(r.height()));

  if(wd > 0.8f && ht > 0.8f) {
    taMisc::Info
      ("NOT saving css console size because wd > 0.8 && ht > 0.8, wd: " + String(wd),
       "ht: " + String(ht), "raw w: " + String(r.width()),
       "h: " + String(r.height()),
       "srcn w:" + String(taiM->scrn_s.w), "srcn h: " + String(taiM->scrn_s.h)
       );
  }
  else {
    tabMisc::root->console_size.SetXY(wd, ht);
    tabMisc::root->console_pos.SetXY(lft, top);
  }
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

  // taMisc::Info("load console geom: wd:", String(s.w), "ht:", String(s.h),
  //              "raw w:", String(wd), "h:", String(ht));
  
  resize(tr.w, tr.h);
  move(tr.x, tr.y);
}

void cssConsoleWindow::PinAction() {
  if(!lock_to_proj) {
    SaveGeom();                 // save last geom before locking to restore for later
  }
  lock_to_proj = !lock_to_proj;
  UpdateFmLock();
}

void cssConsoleWindow::LockedNewGeom(int left, int top, int width, int height) {
  if(!lock_to_proj) return;
  resize(width, height);
  move(left, top);
  css_con->gotoEnd();
}

void cssConsoleWindow::resizeEvent(QResizeEvent* e) {
  inherited::resizeEvent(e);
  if(lock_to_proj) return;
  // only save on project saves or root saves
  // SaveGeom();
}

void cssConsoleWindow::moveEvent(QMoveEvent* e) {
  inherited::moveEvent(e);
  if(lock_to_proj) return;
  // only save on project saves or root saves
  // SaveGeom();
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
  QWidget* wid = NULL;
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
  // this is bad: keeps resetting prompt all the time
  // if(ev->type() == QEvent::ActivationChange) {
  //   if (isActiveWindow()) {
  //     UpdateUi();
  //   }
  // }
  inherited::changeEvent(ev);
}

void cssConsoleWindow::UpdateUi() {
  css_con->displayPrompt(true);
}

void cssConsoleWindow::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  QTimer::singleShot(150, css_con, SLOT(clear()));
}

void cssConsoleWindow::ShowSearchBar() {
  search_widget.show();
}

void cssConsoleWindow::HideSearchBar() {
  search_widget.hide();
}

void cssConsoleWindow::ClearSearch() {
  search_text_field.clear();
}

void cssConsoleWindow::FindNext() {
  if (search_text_field.text() != last_search_text) { // new search - move cursor to start
    css_con->moveCursor(QTextCursor::Start);
    last_search_text = search_text_field.text();
  }
  css_con->find(search_text_field.text());

}

void cssConsoleWindow::FindPrevious() {
  css_con->find(search_text_field.text(), QTextDocument::FindBackward);
}

void cssConsoleWindow::keyPressEvent(QKeyEvent* key_event)
{
  if (key_event->key() == Qt::Key_Return) {
    if (key_event->modifiers() & Qt::ShiftModifier) {
      FindPrevious();
    }
    else {
      FindNext();
    }
  }
}

