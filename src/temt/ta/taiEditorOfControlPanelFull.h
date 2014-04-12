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

#ifndef taiEditorOfControlPanelFull_h
#define taiEditorOfControlPanelFull_h 1

// parent includes:
#include <taiEditorOfControlPanelBase>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiEditorOfControlPanelFull : public taiEditorOfControlPanelBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditorOfControlPanelBase)
  Q_OBJECT
public:
  taiEditorOfControlPanelFull(void* base, TypeDef* td, bool read_only_ = false,
        QObject* parent = 0);
  taiEditorOfControlPanelFull() { Initialize();};
  ~taiEditorOfControlPanelFull();

protected slots:
  void         DoRemoveSelEdit() override;

protected:
  void         ClearBody_impl() override;       // we also clear all the methods, and then rebuild them

  void         Constr_Widget_Labels() override;
  void         FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id) override;
  void         GetImage_Membs_def() override;
  void         GetValue_Membs_def() override;

private:
  void  Initialize();
};

#endif // taiEditorOfControlPanelFull_h
