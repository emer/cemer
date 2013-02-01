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

#ifndef taiEditorWidgetsOfArray_h
#define taiEditorWidgetsOfArray_h 1

// parent includes:
#include <taiEditorOfWidgetsClass>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiEditorWidgetsOfArray : public taiEditorOfWidgetsClass {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiEditorOfWidgetsClass)
public:
  bool          ShowMember(MemberDef* md) const;

  taiEditorWidgetsOfArray(void* base, TypeDef* typ_, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0);
  taiEditorWidgetsOfArray()                { };
  ~taiEditorWidgetsOfArray();
protected:
  int                   array_set; // index of array_set
  override void         GetValue_Membs();
  override void         GetImage_Membs();
  override void         ClearBody_impl();
  override void         Constr_Data_Labels();
  virtual void          Constr_AryData_Labels();
};

#endif // taiEditorWidgetsOfArray_h
