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

#ifndef iTabBarEx_h
#define iTabBarEx_h 1

// parent includes:
#include <iTabBarBase>

// member includes:

// declare all other types mentioned but not required to include:
class iTabWidget;


class TA_API iTabBarEx : public iTabBarBase {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS decodes tab for context menu
  Q_OBJECT
INHERITED(iTabBarBase)
public:
  iTabBarEx(iTabWidget* parent = NULL);

protected:
  iTabWidget*           m_tab_widget;
  override void contextMenuEvent(QContextMenuEvent* ev);
};


#endif // iTabBarEx_h
