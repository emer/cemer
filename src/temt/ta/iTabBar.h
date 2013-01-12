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

#ifndef iTabBar_h
#define iTabBar_h 1

// parent includes:
#include <iTabBarBase>

// member includes:
#ifndef __MAKETA__
#include <QPalette>
#endif

// declare all other types mentioned but not required to include:
class iDataPanel; //
class iTabView; //


class TA_API iTabBar: public iTabBarBase { //  encapsulates the TabBar for iTabView
  Q_OBJECT
INHERITED(iTabBarBase)
public:
  enum TabIcon {
    TI_NONE             = -1,
    TI_UNPINNED,
    TI_PINNED,
    TI_LOCKED
  };

  static QIcon*         tab_icon[TI_LOCKED + 1]; // 0=unpinned, 1=pinned

  static void           InitClass(); // auto executed

  iDataPanel*           panel(int tab_idx); // #IGNORE gets the current panel, if any
  iTabView*             tabView() {return (iTabView*)parent();} // #IGNORE

#ifndef __MAKETA__
  using                 QTabBar::addTab; // bring also into scope
#endif
  int                   addTab(iDataPanel* panel); //#IGNORE puts at end if locked else inserts at end of unlocked
  void                  setTabIcon(int idx, TabIcon ti);
  void                  SetPanel(int idx, iDataPanel* value, bool force = false); //#IGNORE set or remove (NULL) a panel

  iTabBar(iTabView* parent_ = NULL);
  ~iTabBar();
protected:
  QPalette      defPalette;

  override bool focusNextPrevChild(bool next);
  override void contextMenuEvent(QContextMenuEvent * e);
  override void mousePressEvent(QMouseEvent* e);
};

#endif // iTabBar_h
