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

#ifndef ToolBoxDockViewer_h
#define ToolBoxDockViewer_h 1

// parent includes:
#include <DockViewer>

// member includes:

// declare all other types mentioned but not required to include:
class iToolBoxDockViewer; // 


taTypeDef_Of(ToolBoxDockViewer);

class TA_API ToolBoxDockViewer : public DockViewer {
  // ##DEF_NAME_ROOT_ToolBox floatable dockable toolbox window
INHERITED(DockViewer)
public:
  static ToolBoxDockViewer*     New(); // create an initialized instance

  inline iToolBoxDockViewer*    widget() {return (iToolBoxDockViewer*)inherited::widget();}
  // #IGNORE

  TA_DATAVIEWFUNS(ToolBoxDockViewer, DockViewer) //

protected:
  override IViewerWidget* ConstrWidget_impl(QWidget* gui_parent); //note: in _qt.h file
  override void         MakeWinName_impl();

private:
  NOCOPY(ToolBoxDockViewer)
  void  Initialize();
  void  Destroy() {}
};

#endif // ToolBoxDockViewer_h
