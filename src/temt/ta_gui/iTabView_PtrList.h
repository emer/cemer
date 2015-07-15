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

#ifndef iTabView_PtrList_h
#define iTabView_PtrList_h 1

// parent includes:
#include <taPtrList>
#include <iTabView>

// member includes:

// declare all other types mentioned but not required to include:
class iPanelBase; // 


class TA_API iTabView_PtrList: public taPtrList<iTabView> {
public:
  void                  DataPanelDestroying(iPanelBase* panel); // dispatch to all
  iTabView_PtrList() {}
  ~iTabView_PtrList();
};

#endif // iTabView_PtrList_h
