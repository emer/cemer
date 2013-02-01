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

#ifndef iT3DataViewer_h
#define iT3DataViewer_h 1

// parent includes:
#include <iFrameViewer>

// member includes:
#include <iTabWidget>

// declare all other types mentioned but not required to include:
class T3DataViewer;
class iT3DataViewFrame;
class T3DataViewFrame;
class iTabBarBase;
class taiWidgetMenu;

class TA_API iT3DataViewer : public iFrameViewer {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS panel widget that contains 3D data views
  Q_OBJECT
INHERITED(iFrameViewer)
friend class T3DataViewer;
public:
  iTabWidget*           tw;

  inline T3DataViewer*  viewer() {return (T3DataViewer*)m_viewer;}
  iT3DataViewFrame*     iViewFrame(int idx) const; // the frame in indicated tab
  T3DataViewFrame*      viewFrame(int idx) const; // view of frame in indicate tab -- NOT necessarily same as view idx if all frames not viewed in frames

  iTabBarBase*          tabBar() { if(!tw) return NULL; return tw->GetTabBar(); }
  int                   TabIndexByName(const String& nm) const;
  bool                  SetCurrentTab(int tab_idx);
  // focus indicated tab -- returns success
  bool                  SetCurrentTabName(const String& tab_nm);
  // focus indicated tab -- returns success

  void                  AddT3DataViewFrame(iT3DataViewFrame* dvf, int idx = -1);
  void                  UpdateTabNames();

  iT3DataViewer(T3DataViewer* viewer_, QWidget* parent = NULL);
  ~iT3DataViewer(); //

public slots:
  void                  AddFrame();
  void                  DeleteFrame(int tab_idx);
  void                  FrameProperties(int tab_idx);
  void                  FocusFirstTab(); // hack to get first tab focussed on load

protected: // IDataViewWidget i/f
  int                   last_idx;
  override void         Refresh_impl();

  virtual void          FillContextMenu_impl(taiWidgetMenu* menu, int tab_idx);

protected slots:
  void                  tw_customContextMenuRequested2(const QPoint& pos, int tab_idx);
  void                  tw_currentChanged(int tab_idx);
  void                  tw_tabMoved(int fm, int to);

protected:
  override void         focusInEvent(QFocusEvent* ev);

private:
  void                  Init();
};

#endif // iT3DataViewer_h
