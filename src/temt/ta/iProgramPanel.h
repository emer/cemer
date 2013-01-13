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

#ifndef iProgramPanel_h
#define iProgramPanel_h 1

// parent includes:
#include <iProgramPanelBase>

// member includes:

// declare all other types mentioned but not required to include:
class Program; //


class TA_API iProgramPanel: public iProgramPanelBase {
INHERITED(iProgramPanelBase)
  Q_OBJECT
public:
  Program*              prog() {return (m_link) ? (Program*)(link()->data()) : NULL;}
  override String       panel_type() const {return "Edit Program";}

  void                  FillList();

  iProgramPanel(taiDataLink* dl_);

public: // IDataLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iProgramPanel;}

protected:
  override void         OnWindowBind_impl(iTabViewer* itv);

protected slots:
  void                  items_CustomExpandFilter(iTreeViewItem* item,
    int level, bool& expand);
};

#endif // iProgramPanel_h
