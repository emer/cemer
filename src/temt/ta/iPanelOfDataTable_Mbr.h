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

#ifndef iPanelOfDataTable_Mbr_h
#define iPanelOfDataTable_Mbr_h 1

// parent includes:
#include <iPanelOfDataTable>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API iPanelOfDataTable_Mbr : public iPanelOfDataTable {
  Q_OBJECT			// use this for member data tables that you want to have an edit panel for -- does a few things differently
INHERITED(iPanelOfDataTable)
public:
  override String       panel_type() const; // this string is on the subpanel button for this panel
 iPanelOfDataTable_Mbr(taiSigLink* dl_) : iPanelOfDataTable(dl_) { };
  ~iPanelOfDataTable_Mbr();
};

#endif // iPanelOfDataTable_Mbr_h
