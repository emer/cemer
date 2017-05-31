// Copyright 2017, Regents of the University of Colorado,
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

#ifndef ToolBar_List_h
#define ToolBar_List_h 1

// parent includes:
#include <taViewer_List>

// member includes:

// declare all other types mentioned but not required to include:
class ToolBar; // 
taTypeDef_Of(ToolBar);

taTypeDef_Of(ToolBar_List);

class TA_API ToolBar_List: public taViewer_List {
INHERITED(taViewer_List)
public:
  TA_DATAVIEWLISTFUNS(ToolBar_List, taViewer_List, ToolBar)
private:
  NOCOPY(ToolBar_List)
  void                  Initialize() {SetBaseType(&TA_ToolBar);}
  void                  Destroy() {}
};

#endif // ToolBar_List_h
