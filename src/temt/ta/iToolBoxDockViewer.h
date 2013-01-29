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

#ifndef iToolBoxDockViewer_h
#define iToolBoxDockViewer_h 1

// parent includes:
#include <iDockViewer>

// member includes:

// declare all other types mentioned but not required to include:
class ToolBoxDockViewer; //
class iToolBoxDockViewer; //
class QToolBox; //
class iClipWidgetAction; //
class QToolBar; //


typedef void (*ToolBoxProc)(iToolBoxDockViewer* tb);

class TA_API iToolBoxDockViewer: public iDockViewer {
  Q_OBJECT
INHERITED(iDockViewer)
public:
  QToolBox*             tbx;

  inline ToolBoxDockViewer*     viewer() {return (ToolBoxDockViewer*)m_viewer;}

  int                   AssertSection(const String& sec_name); // insures the tab exists; returns idx
  QWidget*              AddClipToolWidget(int sec, iClipWidgetAction* cwa);
  // returns widget for that action
  void                  AddSeparator(int sec); // adds a separator (if one is not at the end already)

  iToolBoxDockViewer(ToolBoxDockViewer* viewer_, QWidget* parent = NULL);
  ~iToolBoxDockViewer(); //

public: // IDataViewerWidget i/f
//  override QWidget*   widget() {return this;}
protected:
//  override void               Constr_impl();
  override void         Constr_post();

protected:
  QToolBar*             sectionWidget(int sec);

private:
  void                  Init();
};

#endif // iToolBoxDockViewer_h
