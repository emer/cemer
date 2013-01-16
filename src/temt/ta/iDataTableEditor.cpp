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

#include "iDataTableEditor.h"
#include <iMatrixTableView>
#include <iDataTableView>
#include <iSplitter>
#include <iMatrixEditor>
#include <MatrixTableModel>
#include <DataTableModel>
#include <taDataLInk>
#include <iMainWindowViewer>

#include <DataChangedReason>
#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QSplitter>
#include <QCoreApplication>

iDataTableEditor::iDataTableEditor(QWidget* parent)
:inherited(parent)
{
  m_cell_par = NULL;
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(0); layOuter->setSpacing(0);
  splMain = new iSplitter(this);
  splMain->setOrientation(Qt::Vertical);
  layOuter->addWidget(splMain);
  tvTable = new iDataTableView();
  tvCell = new iMatrixEditor();
  splMain->addWidget(tvTable);
  splMain->addWidget(tvCell);

  tvTable->installEventFilter(this);
  tvCell->tv->installEventFilter(this);

  connect(tvTable, SIGNAL(sig_currentChanged(const QModelIndex&)),
    this, SLOT(tvTable_currentChanged(const QModelIndex&)));
  connect(tvTable, SIGNAL(sig_dataChanged(const QModelIndex&, const QModelIndex&)),
    this, SLOT(tvTable_dataChanged(const QModelIndex&, const QModelIndex&)) );

}

iDataTableEditor::~iDataTableEditor() {
  if (m_cell_par) {
    setCellMat(NULL, QModelIndex());
  }
}

void iDataTableEditor::ConfigView() {
  DataTable* dt = this->dt(); // cache
  if (!dt) return;
  bool show_cell = (bool)m_cell; // if we have a cell, show it..
  // false if not in mode, or no mat cols
//   if (true) { // in cell mode
//     for (int i = 0; i < dt->data.size; ++i) {
//       DataCol* dc = dt->data.FastEl(i);
//       if (dc->is_matrix) {
//         show_cell = true;
//         break;
//       }
//     }
//   }
  tvCell->setVisible(show_cell);
  // make sure orphan cell viewer goes away...
  if (dt->rows == 0) {
    setCellMat(NULL, QModelIndex());
  }
  tvTable->setCurrentIndex(QModelIndex());
}


void iDataTableEditor::DataLinkDestroying(taDataLink* dl) {
  // note: following should always be true
  if (m_cell_par && (m_cell_par == dl->taData())) {
    // ok, probably the col is deleting, so unlink ourself now!
    setCellMat(NULL, QModelIndex());
    // WARNING: m_cell_par will now be NULL
  }
}

void iDataTableEditor::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
//   taMisc::Info("idte:ddc", String(dcr));
}

DataTableModel* iDataTableEditor::dtm() const {
  if (m_dt)
    return m_dt->GetTableModel();
  return NULL;
}

void iDataTableEditor::Refresh() {
  DataTableModel* dtm = this->dtm();
  if (dtm)
    dtm->refreshViews();
  if (m_cell)
    tvCell->Refresh();
}

void iDataTableEditor::setDataTable(DataTable* dt_) {
  if (dt_ == m_dt.ptr()) return;

  m_dt = dt_;

  // apparently not needed
//   dt_->AddDataClient(this);

  DataTableModel* mod = dtm();

  if (mod) {
//nn    tv->setItemDelegate(new DataTableDelegate(dt_));
    tvTable->setModel(mod);
    connect(mod, SIGNAL(layoutChanged()),
            this, SLOT(tvTable_layoutChanged()) );
  }

  ConfigView();
}

void iDataTableEditor::setCellMat(taMatrix* mat, const QModelIndex& index,
    bool pat_4d)
{
  // unlink old parent
  if (m_cell_par) {
    m_cell_par->RemoveDataClient(this);
    m_cell_par = NULL;
  }
  // link new parent, if any
  if (mat) {
    m_cell_par = mat->slicePar();
    if (m_cell_par) { // should exist!!!
      m_cell_par->AddDataClient(this);
    }
  }

  m_cell = mat;

  m_cell_index = index;
  // actually set mat last, because gui immediately calls back
  tvCell->setMatrix(mat, pat_4d);
  if (mat) {
    MatrixTableModel* mat_model = mat->GetTableModel();
    if(m_cell_par)
      mat_model->setDataCol((DataCol*)m_cell_par->owner);
    mat_model->col_idx = index.column(); // ok if done repeatedly, is always the same
    // connect the magic signal that updates the table -- note that there is just
    // one of these, and it hangs around even when the cell isn't viewed
    connect(mat_model, SIGNAL(matDataChanged(int)),
            dtm(), SLOT(matDataChanged(int)) );
  }
}

void iDataTableEditor::tvTable_layoutChanged() {
  if(!isVisible()) return;
  ConfigView();
//no-causes recursive invocation!  Refresh();
  if ((bool)m_cell) {
    MatrixTableModel* mat_model = m_cell->GetTableModel();
    mat_model->emit_layoutChanged(); // hacky but works
  }
}

void iDataTableEditor::tvTable_currentChanged(const QModelIndex& index) {
  DataTable* dt_ = dt(); // cache
  int colidx = index.column();
  if(colidx < 0) return;			// invalid
  DataCol* col = dt_->GetColData(colidx, true); // quiet
  if (col && col->is_matrix) {
    taMatrix* tcell = dt_->GetValAsMatrix(index.column(), index.row());
    if (tcell) {
      bool pat_4d = (col->HasColFlag(DataCol::PAT_4D) && tcell->dims() >= 4);
      setCellMat(tcell, index, pat_4d);

      MatrixTableModel* mat_model = tcell->GetTableModel();
      mat_model->setDimNames(&col->dim_names);
      tvCell->setVisible(true);
      return;
    }
  }
  tvCell->setVisible(false);
  setCellMat(NULL, QModelIndex());
}

void iDataTableEditor::tvTable_dataChanged(const QModelIndex& topLeft,
    const QModelIndex& bottomRight)
{
  if ((bool)m_cell && (m_cell_index.column() >= topLeft.column()) &&
    (m_cell_index.column() <= bottomRight.column()) &&
    (m_cell_index.row() >= topLeft.row()) &&
     (m_cell_index.row() <= bottomRight.row()) )
  {
    m_cell->DataChanged(DCR_ITEM_UPDATED, NULL, NULL); // easiest way
  }
}

void iDataTableEditor::UpdateSelectedItems_impl() {
  taMisc::Info("updt sel itms");
}


bool iDataTableEditor::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }

  if((bool)tvTable->m_window) {
    tvTable->m_window->FocusIsMiddlePanel();
  }

  QCoreApplication* app = QCoreApplication::instance();
  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed) {
    if(e->key() == Qt::Key_T) {
      if(obj->inherits("iDataTableView")) {
        tvTable->clearExtSelection();
        if(m_cell) {
          tvCell->tv->setFocus();
          tvCell->tv->selectCurCell();
        }
      }
      else {
        tvCell->tv->clearExtSelection();
        tvTable->setFocus();
        tvTable->selectCurCell();
      }
      return true;
    }
  }
  return tvTable->eventFilter(obj, event); // this has all the other good emacs xlations
}

void iDataTableEditor::ScrollToBottom() {
  if(tvTable)
    tvTable->scrollToBottom();
}

