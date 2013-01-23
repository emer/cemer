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

#ifndef DataViewer_List_h
#define DataViewer_List_h 1

// parent includes:
#include <DataView_List>

// member includes:

// declare all other types mentioned but not required to include:
class DataViewer; // 


TypeDef_Of(DataViewer_List);

class TA_API DataViewer_List: public DataView_List {
  // #NO_TOKENS ##EXPAND_DEF_0 list of data viewers
INHERITED(DataView_List)
public:

  void                  GetWinState();
  void                  SetWinState();
  TA_DATAVIEWLISTFUNS(DataViewer_List, DataView_List, DataViewer)

private:
  NOCOPY(DataViewer_List)
  void  Initialize() { SetBaseType(&TA_DataViewer);}
  void  Destroy() {}
};

#endif // DataViewer_List_h
