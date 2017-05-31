// Copyright 2016, Regents of the University of Colorado,
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

#ifndef iT3PanelViewer_h
#define iT3PanelViewer_h 1

// parent includes:
#include <iFrameViewer>

// member includes:
#include <iTabWidget>

// declare all other types mentioned but not required to include:
class T3PanelViewer;
class iT3Panel;
class T3Panel;
class iTabBarBase;
class taiWidgetMenu;

class TA_API iT3PanelViewer : public iFrameViewer {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS panel widget that contains 3D data views
  Q_OBJECT
INHERITED(iFrameViewer)
friend class T3PanelViewer;
public:
  iTabWidget*           tw;

  inline T3PanelViewer*  viewer() {return (T3PanelViewer*)m_viewer;}
  iT3Panel*     iViewPanel(int idx) const; // the panel in indicated tab
  T3Panel*      viewPanel(int idx) const; // view of panel in indicate tab -- NOT necessarily same as view idx if all panels not viewed in panels

  iTabBarBase*          tabBar() { if(!tw) return NULL; return tw->GetTabBar(); }
  int                   TabIndexByName(const String& nm) const;
  bool                  SetCurrentTab(int tab_idx);
  // focus indicated tab -- returns success
  bool                  SetCurrentTabName(const String& tab_nm);
  // focus indicated tab -- returns success
  int                   CurrentTabNo();
  // returns current tab tab index number

  void                  AddT3Panel(iT3Panel* dvf, int idx = -1);
  void                  UpdateTabNames();

  iT3PanelViewer(T3PanelViewer* viewer_, QWidget* parent = NULL);
  ~iT3PanelViewer(); //

public slots:
  void                  AddPanel();
  void                  DeletePanel(int tab_idx);
  void                  PanelProperties(int tab_idx);
  void                  FirstViewProperties(int tab_idx);
  void                  SetColorScheme(int tab_idx);
  void                  SetAllColorScheme(int tab_idx);
  void                  FocusFirstTab(); // hack to get first tab focussed on load

protected: // IViewerWidget i/f
  int                   last_idx;
  void         Refresh_impl() override;

  virtual void          FillContextMenu_impl(taiWidgetMenu* menu, int tab_idx);

protected slots:
  void                  tw_customContextMenuRequested2(const QPoint& pos, int tab_idx);
  void                  tw_currentChanged(int tab_idx);
  void                  tw_tabMoved(int fm, int to);

protected:
  void         focusInEvent(QFocusEvent* ev) override;

private:
  void                  Init();
};

#endif // iT3PanelViewer_h
