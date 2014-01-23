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

#ifndef taiEditorWidgetsMulti_h
#define taiEditorWidgetsMulti_h 1

// parent includes:
#include <taiEditorOfClass>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiEditorWidgetsMulti: public taiEditorOfClass {
INHERITED(taiEditorOfClass)
public:
  QWidget*              multi; // outer container
    QScrollArea*        scrMulti;               // scrollbars for the multi items
    QHBoxLayout*        lay_multi; // used by gpiGroupDialog to add its group buttons
    iEditGrid*          multi_body;

  taiEditorWidgetsMulti(void* base, TypeDef* typ_, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0);
  taiEditorWidgetsMulti()                                { };

  void ClearBody_impl() CPP11_OVERRIDE;       // clear body data for reshowing
  void          AddMultiRowName(int row, const String& name, const String& desc)
    {taiEditorOfClass::AddMultiRowName(multi_body, row, name, desc);} // adds a label item in first column of multi data area
  void          AddMultiColName(int col, const String& name, const String& desc)
    {taiEditorOfClass::AddMultiColName(multi_body, col, name, desc);} // adds descriptive column text to top of a multi data item
  void          AddMultiWidget(int row, int col, QWidget* data)
    {taiEditorOfClass::AddMultiWidget(multi_body, row, col, data);}  // add a data item in the multi-data area -- expands if necessary
  void         SetMultiSize(int rows, int cols) CPP11_OVERRIDE; //note: can never shrink
protected:
  int           multi_rows;
  int           multi_cols;
  bool          header_row; // set false if you don't want the header row
  void Constr_Box() CPP11_OVERRIDE; // add the multi box
  void         Constr_Body() CPP11_OVERRIDE;
  void                  RebuildMultiBody(); // calls clear/constr, disabling updates
  virtual void          Constr_MultiBody(); // added in after Constr_Body -- also used for reshowing multi-body
  virtual void          ClearMultiBody_impl(); // clears multi-body for reshowing
};

#endif // taiEditorWidgetsMulti_h
