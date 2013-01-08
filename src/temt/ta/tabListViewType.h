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

#ifndef tabListViewType_h
#define tabListViewType_h 1

// parent includes:
#include <tabOViewType>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API tabListViewType: public tabOViewType { // for taList
  TAI_TYPEBASE_SUBCLASS(tabListViewType, tabOViewType)
public:
  override int          BidForView(TypeDef*);
  void                  Initialize() {}
  void                  Destroy() {}
protected:
  override taiDataLink* CreateDataLink_impl(taBase* data_);
  override void         CreateDataPanel_impl(taiDataLink* dl_);
};

#endif // tabListViewType_h
