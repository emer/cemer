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

#ifndef T3DataView_List_h
#define T3DataView_List_h 1

// parent includes:
#include <DataView_List>

// member includes:

// declare all other types mentioned but not required to include:
class T3DataView; 


TypeDef_Of(T3DataView_List);

class TA_API T3DataView_List: public DataView_List { // ##NO_TOKENS
INHERITED(DataView_List)
friend class T3DataView;
public:

 virtual T3DataView*  FindData(taBase* dat, int& idx);
 // find DataView guy with data() == dat (fills in its index in list), returns NULL & idx=-1 for not found

  TA_DATAVIEWLISTFUNS(T3DataView_List, DataView_List, T3DataView)
private:
  NOCOPY(T3DataView_List)
  void                  Initialize() {SetBaseType(&TA_T3DataView);}
  void                  Destroy() {}
};

#endif // T3DataView_List_h
