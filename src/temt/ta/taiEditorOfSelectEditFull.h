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

#ifndef taiEditorOfSelectEditFull_h
#define taiEditorOfSelectEditFull_h 1

// parent includes:
#include <taiEditorOfSelectEditBase>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiEditorOfSelectEditFull : public taiEditorOfSelectEditBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditorOfSelectEditBase)
  Q_OBJECT
public:
  taiEditorOfSelectEditFull(void* base, TypeDef* td, bool read_only_ = false,
        QObject* parent = 0);
  taiEditorOfSelectEditFull() { Initialize();};
  ~taiEditorOfSelectEditFull();

protected slots:
  void         DoRemoveSelEdit() CPP11_OVERRIDE;

protected:
  void         ClearBody_impl() CPP11_OVERRIDE;       // we also clear all the methods, and then rebuild them

  void         Constr_Widget_Labels() CPP11_OVERRIDE;
  void         FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id) CPP11_OVERRIDE;
  void         GetImage_Membs_def() CPP11_OVERRIDE;
  void         GetValue_Membs_def() CPP11_OVERRIDE;

private:
  void  Initialize();
};

#endif // taiEditorOfSelectEditFull_h
