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

#ifndef iToolBar_h
#define iToolBar_h 1

// parent includes:
#ifndef __MAKETA__
#include <QToolBar>
#endif

#include <IViewerWidget>

// member includes:

// declare all other types mentioned but not required to include:
class ToolBar;


class TA_API iToolBar: public QToolBar, public IViewerWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(QToolBar)
friend class ToolBar;
friend class iMainWindowViewer;
  Q_OBJECT
public:
  ToolBar*              viewer() {return (ToolBar*)m_viewer;}

  iToolBar(ToolBar* viewer, QWidget* parent = NULL);
  ~iToolBar();

public: // ItaViewerWidget i/f
  override QWidget*     widget() {return this;}
protected:
//  override void               Constr_impl();

protected:
  void                  hideEvent(QHideEvent* e); // override
  void                  showEvent(QShowEvent* e); // override
  virtual void          Showing(bool showing); // #IGNORE called by the show/hide handlers
private:
  void                  Init();
};

#endif // iToolBar_h
