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

#ifndef taiDimEdit_h
#define taiDimEdit_h 1

// parent includes:
#include <taiData>

// member includes:
class MatrixGeom;

// declare all other types mentioned but not required to include:

class TA_API taiDimEdit : public taiData { // specify number of dims and each dim value, ex. for Matrix dims
  Q_OBJECT
  INHERITED(taiData)
public:
  inline iDimEdit*              rep() const {return (iDimEdit*)(QWidget*)m_rep;}
  bool                  fillHor() {return true;} // override
  taiDimEdit(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
    // uses flags:

  void                  GetImage(const MatrixGeom* arr);
  void                  GetValue(MatrixGeom* arr) const;

protected:
  USING(inherited::GetImage_impl)
  override void         GetImage_impl(const void* base) {GetImage((const MatrixGeom*)base);}
  override void         GetValue_impl(void* base) const {GetValue((MatrixGeom*)base);}
private:
  void                  Initialize(QWidget* gui_parent_);

};

#endif // taiDimEdit_h
