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

#ifndef cssConsoleWindow_h
#define cssConsoleWindow_h 1

// parent includes:
#include "ta_def.h"
#include <iAction_List>
#include <QMainWindow>

// member includes:
#include <iRect>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolBar>
#include <QPushButton>

// declare all other types mentioned but not required to include:
class QcssConsole; //
class QIcon; //
class iAction; //
class iAction_List; //
class QMenu; //
class QMenuBar; //
class Variant; //
class taiWidgetMenuBar; //
class taiWidgetMenu; //

class TA_API cssConsoleWindow : public QMainWindow {
  // window to hold the css console
  Q_OBJECT
INHERITED(QMainWindow)
public:
  bool          lock_to_proj;   // whether we are currently locked (pinned) to project or not

  QcssConsole*  css_con;
  QIcon*        pinned;
  QIcon*        unpinned;
  QIcon*        clear_icon;
  QIcon*        select_all_icon;
  QIcon*        search_icon;
  iAction_List  actions; // our own list of all created actions
  iAction*      pin_act;
  iAction*      clear_act;
  iAction*      show_search_act;
  iAction*      hide_search_act;
  iAction*      select_all_act;
  iAction*      window_min_action;
  iAction*      window_zoom_action;
  iAction*      editCutAction;
  iAction*      editCopyAction;
  iAction*      editPasteAction;

  taiWidgetMenuBar*     menu_bar;
  taiWidgetMenu*        edit_menu;
  taiWidgetMenu*        window_menu;
  
  QWidget           search_widget;
  QWidget           toolbar_widget;
  QToolBar          toolbar;
  QLineEdit         search_text_field;
  String            last_search_text;
  
  QHBoxLayout       toolbar_layout;
  QHBoxLayout       search_layout;

  virtual void      UpdateFmLock();
  // update based on current lock status
  virtual void      SaveGeom();
  // save our (unlocked) geometry to root for safe keeping..
  virtual void      LoadGeom();
  // load (resize, reposition) us based on our (unlocked) saved geometry
  virtual void      LockedNewGeom(int left, int top, int width, int height);
  // if the console is in locked state, get this new geometry
  virtual iAction*  AddAction(iAction* act); // add the action to the list, returning the instance (for convenience)

  cssConsoleWindow(QWidget* parent = NULL);
  ~cssConsoleWindow();

public slots:
  virtual void      PinAction();  // pin pressed
  virtual void      windowActivateByName(const Variant& title_);
  virtual void      windowMenu_aboutToShow();
  virtual void      UpdateUi();
  virtual void      ShowSearchBar();
  // display the search widget with text field and buttons
  virtual void      HideSearchBar();
  // hide the search widget
  virtual void      FindNext();
  // search the console text for next occurence of text in search_text field
  virtual void      FindPrevious();
  // search the console text for previous occurence of text in search_text field
  virtual void      ClearSearch();
  // unhighlight found items, clear search text
  
protected:
  iRect         prev_geom;      // previous geometry -- only update for new
  
  void          resizeEvent(QResizeEvent* ev) override;
  void          moveEvent(QMoveEvent* e) override;
  void          closeEvent(QCloseEvent* e) override;
  void          changeEvent(QEvent* ev) override;
  void          showEvent(QShowEvent* e) override;
  void          keyPressEvent(QKeyEvent* e) override;

};

#endif // cssConsoleWindow_h
