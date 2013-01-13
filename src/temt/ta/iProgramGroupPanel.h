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

#ifndef iProgramGroupPanel_h
#define iProgramGroupPanel_h 1

// parent includes:
#include <iProgramPanelBase>

// member includes:

// declare all other types mentioned but not required to include:
class Program_Group; //


class TA_API iProgramGroupPanel: public iProgramPanelBase {
INHERITED(iProgramPanelBase)
  Q_OBJECT
public:
  Program_Group*        progGroup() {return (m_link) ?
    (Program_Group*)(link()->data()) : NULL;}
  override String       panel_type() const {return "Program Params";}

  void                  FillList();

  iProgramGroupPanel(taiDataLink* dl_);

public: // IDataLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iProgramGroupPanel;}

protected slots:
  void                  items_CustomExpandFilter(iTreeViewItem* item,
    int level, bool& expand);
};

#endif // iProgramGroupPanel_h
