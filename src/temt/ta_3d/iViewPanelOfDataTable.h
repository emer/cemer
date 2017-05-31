// Copyright 2017, Regents of the University of Colorado,
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

#ifndef iViewPanelOfDataTable_h
#define iViewPanelOfDataTable_h 1

// parent includes:
#include <iViewPanel>

// member includes:

// declare all other types mentioned but not required to include:
class iMethodButtonMgr; //
class DataTableView; //


taTypeDef_Of(iViewPanelOfDataTable);

class TA_API iViewPanelOfDataTable: public iViewPanel {
  // abstract base for logview panels -- just has the viewspace widget; everything else is up to the subclass
  INHERITED(iViewPanel)
  Q_OBJECT
public:
  QWidget*              widg;
  QVBoxLayout*          layWidg;
  QWidget*              widCmdButtons;
  iMethodButtonMgr*     meth_but_mgr;
  int                   dt_cells_last_updt; // number of cells in data table at last panel update -- used for determining if data has been updated since then.. impl in each subtype
  
  DataTableView*        lv() {return (DataTableView*)m_dv;}

  String       TabText() const override;

  iViewPanelOfDataTable(DataTableView* lv);
  ~iViewPanelOfDataTable();

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iViewPanelOfDataTable;}

protected:

  void         UpdatePanel_impl() override;
  
};

#endif // iViewPanelOfDataTable_h
