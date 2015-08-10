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

#ifndef iTabBarBase_h
#define iTabBarBase_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QTabBar>
#endif

// member includes:

// declare all other types mentioned but not required to include:


class TA_API iTabBarBase : public QTabBar {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base tab bar with ctrl+f/b navigation
  Q_OBJECT
INHERITED(QTabBar)
public:
  iTabBarBase(QWidget* parent_ = NULL);

public slots:
  virtual void  selectNextTab();
  // select the next tab in the list
  virtual void  selectPrevTab();
  // select the previous tab in the list
  
  virtual void  ClearTabWasSelected() { tab_was_selected = false; }
  virtual bool  TabWasSelected() { return tab_was_selected; }

protected:
  void          keyPressEvent(QKeyEvent* e) override;
  void          mouseReleaseEvent(QMouseEvent * event) override;
  
  bool          tab_was_selected;   // need to distinguish tab selection done by user and tab selection that is a by-product
                                    // of some other action such as deletion of the corresponding tree item
};

#endif // iTabBarBase_h
