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

#ifndef ConsoleDockViewer_h
#define ConsoleDockViewer_h 1

// parent includes:
#include <DockViewer>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(ConsoleDockViewer);

class TA_API ConsoleDockViewer: public DockViewer {
  // #NO_TOKENS ##DEF_NAME_ROOT_Console encapsulates the console into a dock viewer window
INHERITED(DockViewer)
public:

  TA_DATAVIEWFUNS(ConsoleDockViewer, DockViewer)
protected:
  override IViewerWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE note: we just use base window, and put the console into it
  override void         MakeWinName_impl(); // set win_name, impl in subs
private:
  NOCOPY(ConsoleDockViewer)
  void                  Initialize();
  void                  Destroy() {}
};

#endif // ConsoleDockViewer_h
