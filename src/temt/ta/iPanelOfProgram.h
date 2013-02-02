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

#ifndef iPanelOfProgram_h
#define iPanelOfProgram_h 1

// parent includes:
#include <iPanelOfProgramBase>

// member includes:

// declare all other types mentioned but not required to include:
class Program; //
class iTreeViewItem; //


TypeDef_Of(iPanelOfProgram);

class TA_API iPanelOfProgram: public iPanelOfProgramBase {
INHERITED(iPanelOfProgramBase)
  Q_OBJECT
public:
  Program*              prog() {return (m_link) ? (Program*)(link()->data()) : NULL;}
  override String       panel_type() const {return "Edit Program";}

  void                  FillList();

  iPanelOfProgram(taiSigLink* dl_);

public: // ISigLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iPanelOfProgram;}

protected:
  override void         OnWindowBind_impl(iPanelViewer* itv);

protected slots:
  void                  items_CustomExpandFilter(iTreeViewItem* item,
    int level, bool& expand);
};

#endif // iPanelOfProgram_h
