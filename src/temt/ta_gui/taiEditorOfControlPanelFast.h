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

#ifndef taiEditorOfControlPanelFast_h
#define taiEditorOfControlPanelFast_h 1

// parent includes:
#include <taiEditorOfControlPanelBase>

// member includes:

// declare all other types mentioned but not required to include:
class iTableWidget;
class taiWidgetDelegateControlPanel;


class TA_API taiEditorOfControlPanelFast : public taiEditorOfControlPanelBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS alternative, using viewer
INHERITED(taiEditorOfControlPanelBase)
  Q_OBJECT
public:
  iTableWidget*   tw;

  void         Constr_Box() override;
  void         GetImage_Item(int row) override; // called from GetImage and ed->GetValue

  taiEditorOfControlPanelFast(void* base, TypeDef* td, bool read_only_ = false,
        QObject* parent = 0);
  taiEditorOfControlPanelFast()        { Initialize();};
  ~taiEditorOfControlPanelFast();

protected slots:
  void         DoRemoveSelEdit() override;
  void                  tw_currentCellChanged( int currentRow,
    int currentColumn, int previousRow, int previousColumn);
  void                  tw_customContextMenuRequested(const QPoint& pos);

protected:
  taiWidgetDelegateControlPanel*   sed;

  void         Constr_Body_impl() override;
  void         ClearBody_impl() override;       // we also clear all the methods, and then rebuild them

  void         Constr_Widget_Labels() override;
  void         FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id) override;
  void         GetImage_Membs_def() override;
  void         GetValue_Membs_def() override;

private:
  void  Initialize();
};

#endif // taiEditorOfControlPanelFast_h
