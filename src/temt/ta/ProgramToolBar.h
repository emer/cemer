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

#ifndef ProgramToolBar_h
#define ProgramToolBar_h 1

// parent includes:
#include <ToolBar>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ProgramToolBar);

class TA_API ProgramToolBar: public ToolBar {
// thin subclass to define custom tb for Programs
INHERITED(ToolBar)
public:
  TA_DATAVIEWFUNS(ProgramToolBar, ToolBar) //
protected:
#ifdef TA_GUI
  IViewerWidget* ConstrWidget_impl(QWidget* gui_parent) CPP11_OVERRIDE; // in _qt file
#endif
private:
  NOCOPY(ProgramToolBar)
  void Initialize() {}
  void Destroy() {}
};

#endif // ProgramToolBar_h
