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

#include "iMatrixEditor.h"
#include <taMatrix>
#include <MatrixTableModel>
#include <iMatrixTableView>

#include <QVBoxLayout>



iMatrixEditor::iMatrixEditor(QWidget* parent)
:inherited(parent)
{
  init();
}

iMatrixEditor::~iMatrixEditor() {
  m_mat = NULL;
}

void iMatrixEditor::init() {
  m_mat = NULL;
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(2); layOuter->setSpacing(2);
//  layDims = new QHBoxLayout(layOuter);
  layDims = new QHBoxLayout; layOuter->addLayout(layDims);
  tv = new iMatrixTableView(this);
  layOuter->addWidget(tv);
}

/*taMatrix* iMatrixEditor::mat() const {
  return m_mat;
}

MatrixTableModel* iMatrixEditor::model() const {
  if (m_mat) 
    return m_mat->GetTableModel();
  return NULL;
}*/

void iMatrixEditor::Refresh() {
 // MatrixTableModel* mod = q_objectcast<>(model();
  MatrixTableModel* mod = qobject_cast<MatrixTableModel*>(tv->model());
  if (mod)
    mod->emit_layoutChanged(); // default values mean entire table
}

void iMatrixEditor::setMatrix(taMatrix* mat_, bool pat_4d) {
  tv->clearExtSelection();	// nuke any existing selection
  QAbstractItemModel* mod = (mat_) ? mat_->GetTableModel() : NULL;
  if(mod)
    tv->setModel(mod);
//   QItemSelectionModel* m = tv->selectionModel();
  // why are we deleting the selection model!?  this is bad and likely bug-inducing
//   if(m)
//     delete m;
}

