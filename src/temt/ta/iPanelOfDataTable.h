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

#ifndef iPanelOfDataTable_h
#define iPanelOfDataTable_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
class iTableView; //
class iDataTableEditor; //
class DataTable; //
class ISelectable_PtrList; //


taTypeDef_Of(iPanelOfDataTable);

class TA_API iPanelOfDataTable: public iPanel {
  Q_OBJECT
INHERITED(iPanel)
public:
  iDataTableEditor*     dte;

  DataTable*            dt() const {return (m_link) ? (DataTable*)(link()->data()) : NULL;}
  virtual String        panel_type() const; // this string is on the subpanel button for this panel

  virtual int           EditAction(int ea);
  virtual int           GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  void                  GetSelectedItems(ISelectable_PtrList& lst); // list of the selected cells

  QWidget*     firstTabFocusWidget() override;

  iPanelOfDataTable(taiSigLink* dl_);
  ~iPanelOfDataTable();

protected:
  void         GetWinState_impl() override; // when saving view state
  void         SetWinState_impl() override; // when showing, from view state

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfDataTable;}
protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) override; //
//  int                EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL) override;

protected:
  void         Render_impl() override;
  void         UpdatePanel_impl() override;

protected slots:
  void                  tv_hasFocus(iTableView* sender); // for both tableviews
  void                  ViewChange();

};

#endif // iPanelOfDataTable_h
