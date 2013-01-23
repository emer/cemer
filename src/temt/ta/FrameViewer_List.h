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

#ifndef FrameViewer_List_h
#define FrameViewer_List_h 1

// parent includes:
#include <DataViewer_List>

// member includes:

// declare all other types mentioned but not required to include:
class FrameViewer; // 
TypeDef_Of(FrameViewer);

TypeDef_Of(FrameViewer_List);

class TA_API FrameViewer_List: public DataViewer_List { // #NO_TOKENS
INHERITED(DataViewer_List)
public:
  TA_DATAVIEWLISTFUNS(FrameViewer_List, DataViewer_List, FrameViewer)
private:
  NOCOPY(FrameViewer_List)
  void  Initialize() { SetBaseType(&TA_FrameViewer);}
  void  Destroy() {}
};

#endif // FrameViewer_List_h
