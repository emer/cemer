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

#include "taiDimEdit.h"
#include <iDimEdit>
#include <MatrixGeom>


taiDimEdit::taiDimEdit(TypeDef* typ_, IWidgetHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
}

void taiDimEdit::Initialize(QWidget* gui_parent_) {
  iDimEdit* de = new iDimEdit(gui_parent_); // cache
  // note: max user-dims is 1 less, since we reserve one for table cols
  de->setMaxDims(TA_MATRIX_DIMS_MAX - 1);
  SetRep(de);
  if (readOnly()) {
    rep()->setReadOnly(true);
  } else {
    connect(m_rep, SIGNAL(changed(iDimEdit*)), this, SLOT(repChanged()) );
  }
}

void taiDimEdit::GetImage(const MatrixGeom* arr) {
  iDimEdit* de = rep(); // cache
  de->setDims(arr->dims());
  for (int i = 0; i < arr->dims(); ++i) {
    de->setDim(i, arr->FastEl(i));
  }
}

void taiDimEdit::GetValue(MatrixGeom* arr) const {
  iDimEdit* de = rep(); // cache
  arr->SetDims(de->dims());
  for (int i = 0; i < de->dims(); ++i) {
    arr->Set(i, de->dim(i));
  }
}

