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

#include "iDataTableSearch.h"
#include <iLineEdit>
#include <taString>
#include <taProject>
#include <taMatrix>
#include <DataTable>
#include <iDataTableView>
#include <iDataTableModel>
#include <iDataTableEditor>
#include <iMatrixEditor>
#include <iMatrixTableView>
#include <iMenuButton>
#include <iActionMenuButton>

#include <taMisc>
#include <taiMisc>

#include <QHBoxLayout>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QToolButton>


iDataTableSearch::iDataTableSearch(iDataTableEditor* parent) : QWidget(parent) {
  editor = NULL;
  table_view = NULL;
  table_model = NULL;
//  table_view_matrix = NULL;
//  table_model_matrix = NULL;
  cur_row_col_pair = NULL;
  Constr();
}

iDataTableSearch::iDataTableSearch(iDataTableView* table_view_, iDataTableEditor* parent)  : QWidget(parent) {
  editor = parent;
  table_view = table_view_;
  table_model = NULL;
  cur_row_col_pair = NULL;
  Constr();
}

iDataTableSearch::~iDataTableSearch() {
}

void iDataTableSearch::Constr() {
  search_mode = CONTAINS;  //  default
  taiMisc::SizeSpec currentSizeSpec = taiM->GetCurrentSizeSpec();

  QHBoxLayout* lay = new QHBoxLayout(this);
  srch_bar = new QToolBar(this);
  lay->addWidget(srch_bar);
  lay->setMargin(0);
  lay->setSpacing(0);
  
  srch_mode_button = new iActionMenuButton();
  srch_bar->addWidget(srch_mode_button);
  
  contains_action = new QAction("Find/Replace", this);
  matches_action = new QAction("Find Matching", this);

  srch_mode_menu = new QMenu(this);
  srch_mode_button->setMenu(srch_mode_menu);
  srch_mode_button->setDefaultAction(contains_action);
  srch_mode_button->setFont(taiM->nameFont(currentSizeSpec));
  srch_mode_button->setToolTip(taiMisc::ToolTipPreProcess("Find cells within the above data table: always case insensitive"));

  srch_text = new iLineEdit();
  srch_bar->addWidget(srch_text);
  repl_text = new iLineEdit();
  srch_bar->addWidget(repl_text);

  srch_nfound = new QLabel(" 0");
  srch_nfound->setFont(taiM->nameFont(currentSizeSpec));
  srch_nfound->setToolTip(taiMisc::ToolTipPreProcess("Number of items found"));
  srch_bar->addWidget(srch_nfound);
  
  srch_clear = srch_bar->addAction("x");
  srch_clear->setToolTip(taiMisc::ToolTipPreProcess("Clear search text and cell highlighting"));
  srch_prev = srch_bar->addAction("<");
  srch_prev->setToolTip(taiMisc::ToolTipPreProcess("Find previous occurrence of find text within table"));
  srch_next = srch_bar->addAction(">");
  srch_next->setToolTip(taiMisc::ToolTipPreProcess("Find next occurrence of find text within table"));
  repl_next = srch_bar->addAction("R>");
  repl_next->setToolTip(taiMisc::ToolTipPreProcess("Replace current selection, then find next occurrence"));

  srch_mode_menu->addAction(contains_action);
  srch_mode_menu->addAction(matches_action);

  connect(srch_clear, SIGNAL(triggered()), this, SLOT(SearchClear()));
  connect(srch_next, SIGNAL(triggered()), this, SLOT(SelectNext()));
  connect(srch_prev, SIGNAL(triggered()), this, SLOT(SelectPrevious()));
  connect(srch_text, SIGNAL(returnPressed()), this, SLOT(TextEntered()));
  connect(contains_action, SIGNAL(triggered()), this, SLOT(ContainsSelected()));
  connect(matches_action, SIGNAL(triggered()), this, SLOT(MatchesSelected()));
  connect(repl_text, SIGNAL(returnPressed()), this, SLOT(TextEntered()));
  connect(repl_next, SIGNAL(triggered()), this, SLOT(ReplaceNext()));
}

void iDataTableSearch::TextEntered() {
  Search(search_mode);
}

void iDataTableSearch::Search(iDataTableSearch::SearchMode mode) {
  if (!table_view) {
    return;
  }
  if (!table_model) {
      table_model = table_view->dataTable()->GetTableModel();
  }
  if (!table_model) {
    return;
  }
  
  // clear
  table_view->dataTable()->DataUpdate(true);
  table_model->ClearFoundList();
  table_view->dataTable()->DataUpdate(false);

  // start next search
  found_list = new taVector2i_List();
  bool contains = (search_mode == CONTAINS);
//  table_view->dataTable()->FindAllScalar(found_list, srch_text->text(), contains);
  table_view->dataTable()->FindAll(found_list, srch_text->text(), contains);

  table_view->dataTable()->DataUpdate(true);
  for (int i=0; i<found_list->size; i++) {
    table_model->AddToFoundList(found_list->FastEl(i)->x, found_list->FastEl(i)->y);
  }
  table_view->dataTable()->DataUpdate(false);
  srch_nfound->setText(String((int)found_list->size));
  SelectNext();
  
  // cleanup
  found_list = NULL;
  delete found_list;
}

void iDataTableSearch::ReplaceNext() {
  if (table_model == NULL) {
    return;
  }
  taProject* proj = table_model->dataTable()->GetMyProj();
  if(proj) {
    DataCol* col = table_model->dataTable()->GetColData(cur_row_col_pair->y, true); // quiet
    if (col) {
      proj->undo_mgr.SaveUndo(table_model->dataTable(), "Replace", col);
      int row = GetRowNumForDataTableView(col, cur_row_col_pair);  // handles scalar and matrix
      
      // get the current value
      String cur_text;
      if (col->isMatrix()) {
        if (editor) {
          int cell_number = cur_row_col_pair->x % col->cell_size();
          Variant cell_value = table_model->dataTable()->GetMatrixFlatVal(col->name, row, cell_number);
          cur_text = cell_value.toString();
        }
      }
      else {
        cur_text = table_model->dataTable()->GetValAsString(cur_row_col_pair->y, row);
      }
      // do the text substitution
      cur_text.repl(srch_text->text(), repl_text->text());
      
      // now do the actual replace
      if (col->isMatrix()) {
        int cell_number = cur_row_col_pair->x % col->cell_size();
        table_model->dataTable()->SetMatrixFlatVal(cur_text, col->name, row, cell_number);
      }
      else {
        table_model->dataTable()->SetVal(cur_text, cur_row_col_pair->y, cur_row_col_pair->x);
      }
    }
  }
  SelectNext();
}

void iDataTableSearch::SelectNext() {
  if (table_model == NULL) {
    return;
  }
  cur_row_col_pair = table_model->GetNextFound();
  if (cur_row_col_pair) {
    int row;
    DataCol* column = table_view->dataTable()->GetColData(cur_row_col_pair->y);
    if (column) {
      row = GetRowNumForDataTableView(column, cur_row_col_pair);   // handles scalar and matrix
      table_view->SetCurrentAndSelect(row, cur_row_col_pair->y);
      
      if (column->isMatrix()) {
        if (editor) {
          taMatrix* cell = column->GetValAsMatrix(row);  // row here is the tables row
          iMatrixTableView* mtv = editor->tvCell->tv;
          taVector2i cell_pair;
          GetCellForMatrixView(column, cur_row_col_pair->x, cell_pair);  // gets row and column of this matrix cell
          mtv->SetCurrentAndSelect(cell_pair.y, cell_pair.x);
        }
      }
    }
  }
}

void iDataTableSearch::SelectPrevious() {
  if (table_model == NULL) {
    return;
  }
  cur_row_col_pair = table_model->GetPreviousFound();
  if (cur_row_col_pair) {
    int row;
    DataCol* column = table_view->dataTable()->GetColData(cur_row_col_pair->y);
    if (column){
      row = GetRowNumForDataTableView(column, cur_row_col_pair);   // handles scalar and matrix
      table_view->SetCurrentAndSelect(row, cur_row_col_pair->y);
    }
    if (column->isMatrix()) {
      if (editor) {
        taMatrix* cell = column->GetValAsMatrix(row);
        iMatrixTableView* mtv = editor->tvCell->tv;
        taVector2i cell_pair;
        GetCellForMatrixView(column, cur_row_col_pair->x, cell_pair);
        mtv->SetCurrentAndSelect(cell_pair.y, cell_pair.x);
      }
    }
  }
}

void iDataTableSearch::SearchClear() {
  if (table_model == NULL) {
    return;
  }
  table_view->dataTable()->DataUpdate(true);
  table_model->ClearFoundList();
  srch_nfound->setText("0");
  srch_text->setText("");
  repl_text->setText("");
  table_view->dataTable()->DataUpdate(false);
}

void iDataTableSearch::ContainsSelected() {
  search_mode = CONTAINS;
}

void iDataTableSearch::MatchesSelected() {
  search_mode = MATCHES;
}

int iDataTableSearch::GetRowNumForDataTableView(DataCol *col, const taVector2i *row_col_pair) const {
  int row;
  if (col->isMatrix()) {
    row = cur_row_col_pair->x / col->cell_size();
  }
  else {
    row = cur_row_col_pair->x;
  }
  return row;
}

void iDataTableSearch::GetCellForMatrixView(DataCol *col, int column_cell_number, taVector2i& cell_pair) {
  int row_cell_number = column_cell_number % col->cell_size();
  cell_pair.y = row_cell_number / col->cell_geom.dim(0);
  cell_pair.x = row_cell_number % col->cell_geom.dim(1);
  
  // table view has 0,0 in upper left but our matrix view starts in lower left
  cell_pair.y = col->cell_geom.dim(1) - cell_pair.y - 1;
}



