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

#ifndef taiEditorWidgetsOfSelectEditFull_h
#define taiEditorWidgetsOfSelectEditFull_h 1

// parent includes:
#include <taiEditorWidgetsOfSelectEditBase>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiEditorWidgetsOfSelectEditFull : public taiEditorWidgetsOfSelectEditBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditorWidgetsOfSelectEditBase)
  Q_OBJECT
public:

  taiEditorWidgetsOfSelectEditFull(void* base, TypeDef* td, bool read_only_ = false,
        QObject* parent = 0);
  taiEditorWidgetsOfSelectEditFull() { Initialize();};
  ~taiEditorWidgetsOfSelectEditFull();

protected slots:
  override void         DoRemoveSelEdit();

protected:
  override void         ClearBody_impl();       // we also clear all the methods, and then rebuild them

  override void         Constr_Data_Labels();
  override void         FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id);
  override void         GetImage_Membs_def();
  override void         GetValue_Membs_def();

private:
  void  Initialize();
};

#endif // taiEditorWidgetsOfSelectEditFull_h
