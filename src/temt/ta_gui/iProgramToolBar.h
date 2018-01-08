// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef iProgramToolBar_h
#define iProgramToolBar_h 1

// parent includes:
#include "ta_def.h"
#include <iToolBar>

// member includes:

// declare all other types mentioned but not required to include:
class iToolBoxDockViewer; //


TA_API void ProgramToolBoxProc(iToolBoxDockViewer* tb); // fills toolbox

class TA_API iProgramToolBar: public iToolBar {
INHERITED(iToolBar)
public:
  iProgramToolBar(ToolBar* viewer, QWidget* parent = NULL)
  :iToolBar(viewer, parent){}

  static void ptbp_add_widget(iToolBoxDockViewer* tb, int sec, TypeDef* td);

protected:
  void         Constr_post() override;
};

#endif // iProgramToolBar_h
