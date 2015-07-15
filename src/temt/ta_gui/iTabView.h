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

#ifndef iTabView_h
#define iTabView_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif

// member includes:
#include <taiSigLink>
#include <iPanelBase_PtrList>
#include <iPanelViewer>
#include <MemberDef>
#include <iTabBar>


// declare all other types mentioned but not required to include:
class iTabBar; //
class QVBoxLayout; // 
class QStackedWidget; //
class QMenu; //


// NOTE: pan_idx is of panelCount and panel()
//       tab_idx is of tabCount and tabPanel()

class TA_API iTabView: public QWidget {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS  control for managing tabbed data pages
  Q_OBJECT
friend class iTabBar;
friend class iTabView_PtrList;
friend class iPanelBase;
public:
  QVBoxLayout*          layDetail;
    iTabBar*            tbPanels; //note: we always maintain at least one tab
    QStackedWidget*     wsPanels; //

  bool                  autoCommit() const {return true;} // if we should autocommit dirty panels when browsing (NOTE: currently always true, could be made a user option)
  iPanelBase*           curPanel() const; // currently active panel
  int                   panelCount() const; // total number of panels (not just in tabs)
  iPanelBase*           panel(int pan_idx = 0); // implementation-independent way to access panels
  int                   tabCount() const; // number of tabs
  iPanelBase*           tabPanel(int tab_idx); // panel from indicated tab (can be NULL)
  iTabBarBase*          tabBar() { return tbPanels; }
  taiSigLink*          par_link() const {return (m_viewer_win) ? m_viewer_win->sel_link() : NULL;}
  MemberDef*            par_md() const {return (m_viewer_win) ? m_viewer_win->sel_md() : NULL;}
  iPanelViewer*           tabViewerWin() {return m_viewer_win;}
  iMainWindowViewer*    viewerWindow() {return (m_viewer_win) ? m_viewer_win->viewerWindow() : NULL;}

  void                  Activated(bool val); // called by parent to indicate if we are active tabview or not
  bool                  ActivatePanel(taiSigLink* dl); // if a panel exists for the link, make it active and return true
  bool                  AddPanel(iPanelBase* panel); // adds a panel if not already, true if newly added
  void                  AddPanelNewTab(iPanelBase* panel, bool lock = false); // adds a panel in a new tab
  void                  Closing(CancelOp& cancel_op);
  void                  DataPanelDestroying(iPanelBase* panel);
  void                  FillTabBarContextMenu(QMenu* contextMenu, int tab_idx = -1);
  iPanelBase*           GetDataPanel(taiSigLink* link); // get panel for indicated link, or make new one; par_link is not necessarily data item owner (ex. link lists, references, etc.)
  void                  RemoveDataPanel(iPanelBase* panel);
  void                  Refresh(); // manually refresh; just delegates to all
  void                  ResolveChanges(CancelOp& cancel_op);
  void                  OnWindowBind(iPanelViewer* itv); // called at constr_post time
  void                  ShowLink(taiSigLink* link, bool not_in_cur = false);
  void                  ShowPanel(iPanelBase* panel, bool not_in_cur = false); // top level guy, checks if exists, adds or sets current; if not_in_cur then won't replace current tab
  bool                  SetCurrentTab(int tab_idx);
  // focus indicated tab, but usually not if current is lockInPlace -- returns success
  bool                  SetCurrentTabName(const String& tab_nm);
  // focus indicated tab, but usually not if current is lockInPlace -- returns success
  void                  ShowTab(iPanelBase* panel, bool show, bool focus = false); // show/hide tab, esp for ctrl panel in visible frame
  int                   TabIndexOfPanel(iPanelBase* panel) const; // or -1 if not showing in a tab
  int                   TabIndexByName(const String& nm) const;
  void                  UpdateTabName(iPanelBase* pan); // called only by individual panel when its name may have changed
  void                  GetWinState();
  void                  SetWinState();

  iTabView(QWidget* parent = NULL);
  iTabView(iPanelViewer* data_viewer_, QWidget* parent = NULL);
  ~iTabView();

public slots:
  void                  AddTab(int tab = -1);
  void                  CloseTab(int tab = -1);
  virtual void          panelSelected(int idx);
  void                  UpdateTabNames(); // called by a siglink when a tab name might have changed; panels also hook to this

protected:
  iPanelViewer*   m_viewer_win;

  void keyPressEvent(QKeyEvent* e) override;

private:
  iPanelBase_PtrList    panels; // no external hanky-panky with this puppie
  void                  Init();
};

#endif // iTabView_h
