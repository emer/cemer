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

#ifndef iDockViewer_h
#define iDockViewer_h 1

// parent includes:
#include <IViewerWidget>
#ifndef __MAKETA__
#include <QDockWidget>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class DockViewer; //


class TA_API iDockViewer: public QDockWidget, public IViewerWidget {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS gui portion of the DockViewer
  Q_OBJECT
INHERITED(QDockWidget)
public:
  inline DockViewer*    viewer() {return (DockViewer*)m_viewer;}

  iDockViewer(DockViewer* viewer_, QWidget* parent = NULL);
  ~iDockViewer();

public: // ItaViewerWidget i/f
  QWidget*     widget() CPP11_OVERRIDE {return this;}
protected:
//  void               Constr_impl() CPP11_OVERRIDE;

protected:
  void         closeEvent(QCloseEvent* ev) CPP11_OVERRIDE;
  bool                  event(QEvent* ev);
  void                  hideEvent(QHideEvent* e); // override
  void                  showEvent(QShowEvent* e); // override
  virtual void          Showing(bool showing); // #IGNORE called by the show/hide handlers

private:
  void                  Init();
};

#endif // iDockViewer_h
