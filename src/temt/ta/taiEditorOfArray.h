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

#ifndef taiEditorOfArray_h
#define taiEditorOfArray_h 1

// parent includes:
#include <taiEditorOfClass>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiEditorOfArray : public taiEditorOfClass {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiEditorOfClass)
public:
  bool          ShowMember(MemberDef* md) const;

  taiEditorOfArray(void* base, TypeDef* typ_, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0);
  taiEditorOfArray()                { };
  ~taiEditorOfArray();
protected:
  int                   array_set; // index of array_set
  void         GetValue_Membs() CPP11_OVERRIDE;
  void         GetImage_Membs() CPP11_OVERRIDE;
  void         ClearBody_impl() CPP11_OVERRIDE;
  void         Constr_Widget_Labels() CPP11_OVERRIDE;
  virtual void          Constr_AryWidget_Labels();
};

#endif // taiEditorOfArray_h
