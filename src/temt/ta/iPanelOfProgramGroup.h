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

#ifndef iPanelOfProgramGroup_h
#define iPanelOfProgramGroup_h 1

// parent includes:
#include <iPanelOfProgramBase>

// member includes:

// declare all other types mentioned but not required to include:
class Program_Group; //
class iTreeViewItem; //


taTypeDef_Of(iPanelOfProgramGroup);

class TA_API iPanelOfProgramGroup: public iPanelOfProgramBase {
INHERITED(iPanelOfProgramBase)
  Q_OBJECT
public:
  Program_Group*        progGroup() {return (m_link) ?
    (Program_Group*)(link()->data()) : NULL;}
  String       panel_type() const override {return "Program Params";}

  void                  FillList();

  iPanelOfProgramGroup(taiSigLink* dl_);

public: // ISigLinkClient interface
//  void*      This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfProgramGroup;}

protected slots:
  void                  items_CustomExpandFilter(iTreeViewItem* item,
    int level, bool& expand);
};

#endif // iPanelOfProgramGroup_h
