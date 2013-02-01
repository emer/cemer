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

#ifndef iDataTablePanel_h
#define iDataTablePanel_h 1

// parent includes:
#include <iDataPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:
class iTableView; //
class iDataTableEditor; //
class DataTable; //
class ISelectable_PtrList; //


TypeDef_Of(iDataTablePanel);

class TA_API iDataTablePanel: public iDataPanelFrame {
  Q_OBJECT
INHERITED(iDataPanelFrame)
public:
  iDataTableEditor*     dte;

  DataTable*            dt() const {return (m_link) ? (DataTable*)(link()->data()) : NULL;}
  override String       panel_type() const; // this string is on the subpanel button for this panel

  override int          EditAction(int ea);
  override int          GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  void                  GetSelectedItems(ISelectable_PtrList& lst); // list of the selected cells

  override QWidget*     firstTabFocusWidget();

  iDataTablePanel(taiSigLink* dl_);
  ~iDataTablePanel();

protected:
  override void         GetWinState_impl(); // when saving view state
  override void         SetWinState_impl(); // when showing, from view state

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iDataTablePanel;}
protected:
  override void         SigEmit_impl(int dcr, void* op1, void* op2); //
//  override int                EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

protected:
  override void         Render_impl();
  override void         UpdatePanel_impl();

protected slots:
  void                  tv_hasFocus(iTableView* sender); // for both tableviews
};

#endif // iDataTablePanel_h
