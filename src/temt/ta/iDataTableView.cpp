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

#include "iDataTableView.h"
#include <DataTable>
#include <iDataTableColHeaderView>
#include <iDataTableRowHeaderView>
#include <iDataTableModel>
#include <taiTabularDataMimeFactory>
#include <iClipData>
#include <CellRange>
#include <taiMimeSource>
#include <taProject>
#include <taiSigLink>
#include <taiWidgetMenu>
#include <taMisc>

#include <QHeaderView>

iDataTableView::iDataTableView(QWidget* parent)
:inherited(parent)
{
  setSelectionMode(QAbstractItemView::ContiguousSelection);   // mode for table items (not headers)
  gui_edit_op = false;

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this,
      SLOT(this_customContextMenuRequested(const QPoint&)) );

  col_header = new iDataTableColHeaderView(this); // subclass header
  this->setHorizontalHeader(col_header);

  row_header = new iDataTableRowHeaderView(this); // subclass header
  this->setVerticalHeader(row_header);
}

void iDataTableView::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
  inherited::currentChanged(current, previous);
  emit sig_currentChanged(current);
}

void iDataTableView::dataChanged(const QModelIndex& topLeft,
    const QModelIndex & bottomRight
#if (QT_VERSION >= 0x050000)
    , const QVector<int> &roles
#endif
)
{
#if (QT_VERSION >= 0x050000)
  inherited::dataChanged(topLeft, bottomRight, roles);
#else
  inherited::dataChanged(topLeft, bottomRight);
#endif
  emit sig_dataChanged(topLeft, bottomRight);
}

DataTable* iDataTableView::dataTable() const {
  iDataTableModel* mod = qobject_cast<iDataTableModel*>(model());
  if (mod) return mod->dataTable();
  else return NULL;
}

void iDataTableView::EditAction(int ea) {
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab || !selectionModel()) return;
  gui_edit_op = true;
  taiTabularDataMimeFactory* fact = taiTabularDataMimeFactory::instance();
  CellRange sel(selectionModel()->selectedIndexes());
  if (ea & iClipData::EA_SRC_OPS) {
    fact->Table_EditActionS(tab, sel, ea);
  } else {// dest op
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Table_EditActionD(tab, sel, ms, ea);

    QModelIndex newIndex;
    newIndex  = this->model()->index(sel.row_fr, sel.col_fr);
    QModelIndex topLeft = model()->index(sel.row_fr, sel.col_fr);
    QModelIndex bottomRight = model()->index(sel.row_to, sel.col_to);
    QItemSelection selection(topLeft, bottomRight);
    this->selectionModel()->select(selection, QItemSelectionModel::Select);
    this->setCurrentIndex(newIndex);
    this->setFocus();

    delete ms;
  }
  gui_edit_op = false;
}

void iDataTableView::ViewAction(int ea) {
  // only one action -- ignore arg
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab || !selectionModel()) return;
  CellRange sel(selectionModel()->selectedIndexes());
  String str;
  for (int col = sel.col_fr; col <= sel.col_to; ++col) {
    DataCol* da = tab->GetColData(col, true); // quiet
    if (!da) continue;
    int cell_size = da->cell_size();
    for (int row = sel.row_fr; row <= sel.row_to; ++row) {
      for (int cell = 0; cell < cell_size; ++cell) {
        str += tab->GetValAsStringM(col, row, cell);
      }
    }
  }
  // todo: could do more
  taMisc::Confirm("contents of cell(s):\n", str);
}

void iDataTableView::GetEditActionsEnabled(int& ea) {
  int allowed = 0;
  int forbidden = 0;
  DataTable* tab = this->dataTable(); // may not exist
  if (tab && selectionModel()) {
    taiTabularDataMimeFactory* fact = taiTabularDataMimeFactory::instance();
    CellRange sel(selectionModel()->selectedIndexes());
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Table_QueryEditActions(tab, sel, ms, allowed, forbidden);
    delete ms;
  }
  ea = allowed & ~forbidden;
}

void iDataTableView::RowColOp_impl(int op_code, const CellRange& sel) {
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab) return;
  taProject* proj = (taProject*)tab->GetOwner(&TA_taProject);

  gui_edit_op = true;
  if (op_code & OP_ROW) {
    // must have >=1 row selected to make sense
    if ((op_code & (OP_APPEND | OP_INSERT | OP_DUPLICATE | OP_DELETE | OP_INSERT_AFTER | OP_DELETE_UNSELECTED | OP_COMPARE | OP_CLEAR_COMPARE))) {
      if (sel.height() < 1)
        goto bail;
      QModelIndex newIndex;
      bool rval = false;

      if (op_code & OP_APPEND) {
        if(proj)
          proj->undo_mgr.SaveUndo(tab, "AddRows", tab);
        rval = tab->AddRows(sel.height());
        newIndex  = this->model()->index(tab->rows-1, 0);
      }
      else if (op_code & OP_INSERT) {
        if(proj)
          proj->undo_mgr.SaveUndo(tab, "InsertRows", tab);
        rval = tab->InsertRows(sel.row_fr, sel.height());
        newIndex  = this->model()->index(sel.row_fr, 0);
      }
      else if (op_code & OP_INSERT_AFTER) {
        if(proj)
          proj->undo_mgr.SaveUndo(tab, "InsertRowsAfter", tab);
        rval = tab->InsertRowsAfter(sel.row_fr, sel.height());
        newIndex  = this->model()->index(sel.row_fr + sel.height(), 0);
      }
      else if (op_code & OP_DUPLICATE) {
        if(proj)
          proj->undo_mgr.SaveUndo(tab, "DuplicateRows", tab);
        rval = tab->DuplicateRows(sel.row_fr, sel.height());
        newIndex  = this->model()->index(sel.row_fr + sel.height(), 0);
      }
      else if (op_code & OP_DELETE) {
        if(taMisc::delete_prompts) {
          if (taMisc::Choice("Are you sure you want to delete the selected rows?", "Yes", "Cancel") != 0)
            goto bail;
        }
        if(proj)
          proj->undo_mgr.SaveUndo(tab, "RemoveRows", tab);
        rval = tab->RemoveRows(sel.row_fr, sel.height());
        if (sel.row_fr != 0)
          newIndex = this->model()->index(sel.row_fr - 1, 0);
        else
          newIndex = this->model()->index(0, 0);
      }
      else if (op_code & OP_DELETE_UNSELECTED) {
        if(taMisc::delete_prompts) {
          if (taMisc::Choice("Are you sure you want to delete the selected rows?", "Yes", "Cancel") != 0)
            goto bail;
        }
        if(proj)
          proj->undo_mgr.SaveUndo(tab, "RemoveRowsUnselected", tab);
        // we are using contiguous selection so remove rows above and below selection
        if (sel.row_to != tab->rows-1) // i.e. last row not selected
          rval = tab->RemoveRows(sel.row_to + 1, tab->rows - sel.row_to -1);
        if (sel.row_fr != 0) // i.e. first row not selected
          rval = tab->RemoveRows(0, sel.row_fr);
        newIndex = this->model()->index(0, 0);
      }
      else if (op_code & OP_COMPARE) {
        tab->CompareRows(sel.row_fr, sel.height());
        this->selectionModel()->clearSelection();
      }
      else if (op_code & OP_CLEAR_COMPARE) {
        tab->ClearCompareRows();
      }
      if (rval) {
        this->selectionModel()->select(newIndex, QItemSelectionModel::Select);
        this->setCurrentIndex(newIndex);
        this->setFocus();
      }
    }
  }

  else if (op_code & OP_COL) {
    // must have >=1 col selected to make sense
    if ((op_code & (OP_APPEND | OP_INSERT | OP_DELETE | OP_DUPLICATE))) {
      if (sel.width() < 1) goto bail;
      /*note: not supporting col ops here
	      if (op_code & OP_APPEND) {
	      } else
	      if (op_code & OP_INSERT) {
	      } else */
      if (op_code & OP_DELETE) {
        if(taMisc::delete_prompts) {
          if (taMisc::Choice("Are you sure you want to delete the selected columns?", "Yes", "Cancel") != 0) goto bail;
        }
        tab->StructUpdate(true);
        if(proj) proj->undo_mgr.SaveUndo(tab, "RemoveCols", tab);
        for (int col = sel.col_to; col >= sel.col_fr; --col) {
          tab->RemoveCol(col);
        }
        tab->StructUpdate(false);
      }
      if (op_code & OP_DUPLICATE) {
        tab->StructUpdate(true);
        if(proj) proj->undo_mgr.SaveUndo(tab, "DuplicateCols", tab);
        for (int col = sel.col_to; col >= sel.col_fr; --col) {
          tab->DuplicateCol(col);
        }
        tab->StructUpdate(false);
      }
    }
    else if (op_code & OP_RESIZE_TO_CONTENT) {
      for (int col = sel.col_to; col >= sel.col_fr; --col) {
        this->resizeColumnToContents(col);
      }
    }
    else if (op_code & OP_RESIZE_TO_CONTENT_ALL) {
      int cols = tab->data.size;
      for (int col = cols-1; col >= 0; --col) {
        this->resizeColumnToContents(col);
      }
    }
  }
  bail:
  gui_edit_op = false;
}

void iDataTableView::FillContextMenu_impl(ContextArea ca,
    taiWidgetMenu* menu, const CellRange& sel)
{
  inherited::FillContextMenu_impl(ca, menu, sel);
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab) return;
  // only do col items if one selected only
  if ((ca == CA_COL_HDR) && (sel.width() == 1)) {
    DataCol* col = tab->GetColData(sel.col_fr, true);
    if (col) {
      taiSigLink* link = (taiSigLink*)col->GetSigLink();
      if (link) link->FillContextMenu(menu);
    }
  }
  if (ca == CA_ROW_HDR) {
    iAction* act = NULL;
    menu->AddSep();
    act = menu->AddItem("Compare Selected Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_COMPARE));
    act->setEnabled(sel.height() > 1);  // enable if compare state is on

    act = menu->AddItem("Clear Compare Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_CLEAR_COMPARE));
    act->setEnabled(tab->CompareRowsState());  // enable if compare state is on

  }
}
