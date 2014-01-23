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

#ifndef iTabWidget_h
#define iTabWidget_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QTabWidget>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class iTabBarBase;


class TA_API iTabWidget : public QTabWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS decodes tab for context menu
  Q_OBJECT
INHERITED(QTabWidget)
friend class iTabBarEx;
public:
  iTabWidget(QWidget* parent = NULL);

  iTabBarBase*  GetTabBar();

#ifndef __MAKETA__
signals:
  void          customContextMenuRequested2(const QPoint& pos, int tab_idx);
#endif

protected:
  void          emit_customContextMenuRequested2(const QPoint& pos, int tab_idx);
  void contextMenuEvent(QContextMenuEvent* ev) CPP11_OVERRIDE;
};

#endif // iTabWidget_h
