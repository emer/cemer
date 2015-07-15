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

#ifndef iApplicationToolBar_h
#define iApplicationToolBar_h 1

// parent includes:
#include <iToolBar>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API iApplicationToolBar: public iToolBar {
  // widget for the Application toolbar -- created by ToolBar if name=Application
INHERITED(iToolBar)
public:
  iApplicationToolBar(ToolBar* viewer, QWidget* parent = NULL)
  :iToolBar(viewer, parent){}
 protected:
  void         Constr_post() override;
};

#endif // iApplicationToolBar_h
