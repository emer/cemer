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

#ifndef DockViewer_h
#define DockViewer_h 1

// parent includes:
#include <TopLevelViewer>

// member includes:

// declare all other types mentioned but not required to include:
class iDockViewer; // 


class TA_API DockViewer : public TopLevelViewer {
  // #VIRT_BASE the controller for dock windows, which can float, or be in a MainWindow
INHERITED(TopLevelViewer)
public:
  enum DockViewerFlags { // #BITS controls behavior
    DV_NONE             = 0, // #NO_BIT
    DV_CLOSABLE         = 0x01, // #BIT true if we are allowed to close it
    DV_MOVABLE          = 0x02, // #BIT true if we are allowed to move it around
    DV_FLOATABLE        = 0x04  // #BIT true if we are allowed to undock it
#ifndef __MAKETA__
    ,DV_ALL             = DV_CLOSABLE | DV_MOVABLE | DV_FLOATABLE
#endif
  };

  DockViewerFlags       dock_flags; // #READ_ONLY #SHOW how this dock window is allowed to behave
  int                   dock_area; // one of the Qt::DockWidgetArea flags, def is bottom
  inline iDockViewer*   widget() {return (iDockViewer*)inherited::widget();}

  TA_DATAVIEWFUNS(DockViewer, TopLevelViewer) //

protected:
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent);
  //override void               MakeWinName_impl(); each subguy will need this
  override void         GetWinState_impl();
  override void         SetWinState_impl();

private:
  void  Copy_(const DockViewer& cp)
    {dock_flags = cp.dock_flags; dock_area = cp.dock_area;}
  void  Initialize();
  void  Destroy() {}
};

#endif // DockViewer_h
