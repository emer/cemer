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

#ifndef iDataTableView_Panel_h
#define iDataTableView_Panel_h 1

// parent includes:
#include <iViewPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:
class iMethodButtonMgr; //
class DataTableView; //


TypeDef_Of(iDataTableView_Panel);

class TA_API iDataTableView_Panel: public iViewPanelFrame {
  // abstract base for logview panels -- just has the viewspace widget; everything else is up to the subclass
  INHERITED(iViewPanelFrame)
  Q_OBJECT
public:
  QWidget*              widg;
  QVBoxLayout*          layWidg;
  QWidget*              widCmdButtons;
  iMethodButtonMgr*     meth_but_mgr;
  DataTableView*        lv() {return (DataTableView*)m_dv;}

  iDataTableView_Panel(DataTableView* lv);
  ~iDataTableView_Panel();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iDataTableView_Panel;}

protected:
};

#endif // iDataTableView_Panel_h
