// Co2018ght 2013-2017, Regents of the University of Colorado,
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
#include <taiEditorOfString>
#include <ControlPanel>

#include <taMisc>
#include <taiMisc>

#include <QHeaderView>
#include <QInputDialog>
#include <QKeyEvent>
#include <QCheckBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
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

  col_header->setDefaultSectionSize(ConvertCharsToPixels(default_chars_per_line));
#if (QT_VERSION >= 0x050200)
  col_header->setMaximumSectionSize(ConvertCharsToPixels(max_chars_per_line));
  col_header->setResizeContentsPrecision(resize_precision_rows);
#endif

  row_header = new iDataTableRowHeaderView(this); // subclass header
  this->setVerticalHeader(row_header);
#if (QT_VERSION >= 0x050200)
  row_header->setResizeContentsPrecision(resize_precision_rows);
#endif
  
  connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(doubleClicked(const QModelIndex&)) );
  installEventFilter(this);
}

bool iDataTableView::eventFilter(QObject* obj, QEvent* event) {
  return inherited::eventFilter(obj, event);
}

void iDataTableView::SetItemDelegates() {
  inherited::SetItemDelegates();
  
//  iTableViewDefaultDelegate* default_delegate = new iTableViewDefaultDelegate(this);
//  iTableViewCheckboxDelegate* checkbox_delegate = new iTableViewCheckboxDelegate(this);
//
//  if (dataTable()) {
//    for (int col_idx=0; col_idx<dataTable()->data.size; col_idx++) {
//      DataCol* data_col = dataTable()->GetColData(col_idx);
//      if (data_col->valType() == taBase::VT_BOOL) {
//        setItemDelegateForColumn(col_idx, checkbox_delegate);
//        for(int i=0; i<dataTable()->rows; i++) {
//          openPersistentEditor(dataTable()->GetTableModel()->index(i, col_idx) );
//        }
//      }
//      else {
//        setItemDelegateForColumn(col_idx, default_delegate);
//      }
//    }
//  }
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
  (void)ea;
  
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab || !selectionModel())
    return;
  CellRange sel(selectionModel()->selectedIndexes());
  String str;
  String col_name;
  for (int col = sel.col_fr; col <= sel.col_to; ++col) {
    DataCol* da = tab->GetColData(col, true); // quiet
    if (!da) continue;
    int cell_size = da->cell_size();
    for (int row = sel.row_fr; row <= sel.row_to; ++row) {
      for (int cell = 0; cell < cell_size; ++cell) {
        if (sel.col_fr != sel.col_to) {
          col_name = da->name;
          str += col_name;
          if (col_name.length() <= 8) {
            str += "\t\t\t";
          }
          else {
            str += "\t\t";
          }
        }
        str += tab->GetValAsStringM(col, row, cell);
        str += "\n";
      }
    }
  }
  tab->cell_view = str;
  TypeDef* td = &TA_DataTable;
  MemberDef* md = td->members.FindName("cell_view");
  taiEditorOfString* host_ = new taiEditorOfString(md, tab, td, true, false, NULL, false, false);
  // args are: read_only, modal, parent, line_nos, rich_text
  host_->Constr("Contents of selected cell(s) in DataTable: " + tab->name);
  host_->Edit(false);
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

void iDataTableView::FillContextMenu_impl(ContextArea ca, taiWidgetMenu* menu, const CellRange& sel)
{
  inherited::FillContextMenu_impl(ca, menu, sel);

  iAction* act = NULL;
  
  DataCol* dc = dataTable()->data.FastEl(sel.col_fr);
  int row = sel.row_fr;

  // generic col guys
  if (dc && ca == CA_COL_HDR) {
    act = menu->AddItem("Set Fixed Column Width...", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_COL | OP_SET_WIDTH) );
    act = menu->AddItem("Set Auto-Width Mode", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_COL | OP_RESIZE_TO_CONTENT) );
    if(dc->HasColFlag(DataCol::AUTO_WIDTH))
      act->setEnabled(false);
    act = menu->AddItem("Set Auto-Width Mode for All Cols", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_COL | OP_RESIZE_TO_CONTENT_ALL) );
    menu->AddSep();
    if (!isFixedColCount()) {
      act = menu->AddItem("Delete Columns", taiWidgetMenu::normal, iAction::int_act,
                          this, SLOT(RowColOp(int)), (OP_COL | OP_DELETE) );
      act = menu->AddItem("Duplicate Columns", taiWidgetMenu::normal, iAction::int_act,
                          this, SLOT(RowColOp(int)), (OP_COL | OP_DUPLICATE) );
      menu->AddSep();
    }
  }
  
  // generic row guys
  if (ca == CA_ROW_HDR) {
    act = menu->AddItem("Set Fixed Row Height...", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_SET_HEIGHT) );
    act = menu->AddItem("Set Row Auto-Height Mode", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_RESIZE_HEIGHT_TO_CONTENT) );
    if(dataTable()->HasDataFlag(DataTable::ROW_AUTO_HEIGHT))
      act->setEnabled(false);
    act = menu->AddItem("Restore Fixed Row Height", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_RESTORE_HEIGHT) );
    if(!dataTable()->HasDataFlag(DataTable::ROW_AUTO_HEIGHT))
      act->setEnabled(false);
    menu->AddSep();
    if (!isFixedRowCount()) {
      act = menu->AddItem("Append Rows", taiWidgetMenu::normal, iAction::int_act,
                          this, SLOT(RowColOp(int)), (OP_ROW | OP_APPEND) );
      act = menu->AddItem("Insert Rows (Ctrl+I)", taiWidgetMenu::normal, iAction::int_act,
                          this, SLOT(RowColOp(int)), (OP_ROW | OP_INSERT) );
      act = menu->AddItem("Insert Rows After (Ctrl+O)", taiWidgetMenu::normal,
                          iAction::int_act,
                          this, SLOT(RowColOp(int)), (OP_ROW | OP_INSERT_AFTER) );
      act = menu->AddItem("Duplicate Rows (Ctrl+M)", taiWidgetMenu::normal,
                          iAction::int_act,
                          this, SLOT(RowColOp(int)), (OP_ROW | OP_DUPLICATE) );
      act = menu->AddItem("Delete Rows (Ctrl+D)", taiWidgetMenu::normal, iAction::int_act,
                          this, SLOT(RowColOp(int)), (OP_ROW | OP_DELETE) );
      menu->AddSep();
    }
    act = menu->AddItem("Highlight Row", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_HILITE) );
    act = menu->AddItem("Clear Highlight", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_CLEAR_HILITE) );
    act = menu->AddItem("Show Only Selected Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_DELETE_UNSELECTED) );
    act = menu->AddItem("Show All Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_SHOW_ALL) );
    menu->AddSep();
  }
}

void iDataTableView::RowColOp_impl(int op_code, const CellRange& sel) {
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab) return;
  taProject* proj = (taProject*)tab->GetOwner(&TA_taProject);

  gui_edit_op = true;
  if (op_code & OP_ROW) {
    // must have >=1 row selected to make sense
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
    else if (op_code & OP_SHOW_ALL) {
      proj->undo_mgr.SaveUndo(tab, "ShowAllRows", tab);
      tab->ShowAllRows();
    }
    else if (op_code & OP_COMPARE) {
      tab->CompareRows(sel.row_fr, sel.height());
      this->selectionModel()->clearSelection();
    }
    else if (op_code & OP_CLEAR_COMPARE) {
      tab->ClearCompareRows();
    }
    else if (op_code & OP_SET_HEIGHT) {
      int rows = QInputDialog::getInt(0, "Set Row Height - ", "Height in lines of text:",
                                      tab->row_height, 1, iTableView::max_lines_per_row);
      SetRowHeight(rows);
      tab->row_height = rows;
      tab->ClearDataFlag(DataTable::ROW_AUTO_HEIGHT);
    }
    else if (op_code & OP_RESIZE_HEIGHT_TO_CONTENT) {
      SetRowHeightToContents();
      tab->SetDataFlag(DataTable::ROW_AUTO_HEIGHT);
    }
    else if (op_code & OP_RESTORE_HEIGHT) {
      SetRowHeight(tab->row_height);
      tab->ClearDataFlag(DataTable::ROW_AUTO_HEIGHT);
    }
    else if (op_code & OP_HILITE) {
      SetRowHighlight(sel.row_fr);
    }
    else if (op_code & OP_CLEAR_HILITE) {
      SetRowHighlight(-1);
    }
    if (rval) {
      this->selectionModel()->select(newIndex, QItemSelectionModel::Select);
      this->setCurrentIndex(newIndex);
      this->setFocus();
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
    else if (op_code & OP_SET_WIDTH) {
      int width = ConvertPixelsToChars(columnWidth(sel.col_fr));
      width = QInputDialog::getInt(0, "Set Column Width", "Width in Characters:", width, 1);
      if (sel.col_to == sel.col_fr) {
        int col_idx = sel.col_fr;
        this->SetColumnWidth(col_idx, width);  // call our version -- not qtableview::setColumnWidth
      }
      else {
        for (int col = sel.col_to; col >= sel.col_fr; --col) {
          this->SetColumnWidth(col, width);
        }
      }
    }
    else if (op_code & OP_RESIZE_TO_CONTENT) {
      for (int col = sel.col_to; col >= sel.col_fr; --col) {
        this->ResizeColumnToContents(col);
      }
    }
    else if (op_code & OP_RESIZE_TO_CONTENT_ALL) {
      int cols = tab->data.size;
      for (int col = cols-1; col >= 0; --col) {
        this->ResizeColumnToContents(col);
      }
    }
  }
  bail:
  gui_edit_op = false;
}

void iDataTableView::doubleClicked(const QModelIndex& index) {
  iDataTableModel* model = dataTable()->GetTableModel();
  Qt::ItemFlags flags = model->flags(index);
  if(!flags.testFlag(Qt::ItemIsEditable)) {
    ViewAction(0);
  }
}

void iDataTableView::keyPressEvent(QKeyEvent* key_event) {
  inherited::keyPressEvent(key_event);
}

void iDataTableView::UpdateRowHeightColWidth() {
  DataTable* dt = dataTable();
  if(!dt) return;

  if(col_header->count() != dt->data.size) {
    // out of date!
    return;
  }
  
  int row_height = 1;
#if (QT_VERSION >= 0x050200)
  col_header->setMaximumSectionSize(ConvertCharsToPixels(dt->max_col_width));
  // set precision based on number of columns!
  // could also use 0 to select visible area...
  int prec_rows = 1000;
  if(dt->cols() > 0) {
    prec_rows = 1000 / dt->cols();
    if(prec_rows < 10)
      prec_rows = 10;
  }
  col_header->setResizeContentsPrecision(prec_rows);
  row_header->setResizeContentsPrecision(prec_rows);
#endif

  if (!dt->HasDataFlag(DataTable::ROW_AUTO_HEIGHT)) {
    if(dt->row_height < 1)
      dt->row_height = 1;
    row_height = dt->row_height;
    SetRowHeight(row_height);
  }
  else {
    SetRowHeightToContents();
  }

  for (int col_idx=0; col_idx<dt->data.size; col_idx++) {
    DataCol* data_col = dt->GetColData(col_idx);
    if (data_col) {
      if (data_col->HasColFlag(DataCol::AUTO_WIDTH)) {
#if (QT_VERSION >= 0x050200)
        if(col_header->sectionResizeMode(col_idx) != QHeaderView::ResizeToContents) {
          col_header->setSectionResizeMode(col_idx, QHeaderView::ResizeToContents);
        }
#else
        if(col_header->resizeMode(col_idx) != QHeaderView::ResizeToContents) {
          col_header->setResizeMode(col_idx, QHeaderView::ResizeToContents);
        }
#endif
      }
      else {
        int pix_wd = ConvertCharsToPixels(data_col->width);
#if (QT_VERSION >= 0x050200)
        if(col_header->sectionResizeMode(col_idx) != QHeaderView::Interactive) {
          col_header->setSectionResizeMode(col_idx, QHeaderView::Interactive);
          setColumnWidth(col_idx, pix_wd); // qt version
        }
#else
        if(col_header->resizeMode(col_idx) != QHeaderView::Interactive) {
          col_header->setResizeMode(col_idx, QHeaderView::Interactive);
          setColumnWidth(col_idx, pix_wd); // qt version
        }
#endif        
        else {
          if(columnWidth(col_idx) != pix_wd) {
            setColumnWidth(col_idx, pix_wd); // qt version
          }
        }
      }
    }
  }
}

void iDataTableView::Refresh() {
  // this is called at initial display -- from there updates work as expected
  UpdateRowHeightColWidth(); 
  update();
}

void iDataTableView::ResizeColumnToContents(int column) {
  DataCol* col = dataTable()->GetColData(column);
  col->SetColFlag(DataCol::AUTO_WIDTH);
#if (QT_VERSION >= 0x050200)
  horizontalHeader()->setSectionResizeMode(column, QHeaderView::ResizeToContents);
#else
  horizontalHeader()->setResizeMode(column, QHeaderView::ResizeToContents);
#endif
}

void iDataTableView::SetColumnWidth(int column, int n_chars) {
  inherited::SetColumnWidth(column, n_chars);
  DataCol* col = dataTable()->GetColData(column);
  col->ClearColFlag(DataCol::AUTO_WIDTH);
  col->width = n_chars;
  UpdateMaxColWidth(n_chars);
}

void iDataTableView::UpdateMaxColWidth(int width) {
  DataTable* dt = dataTable();
  if(!dt) return;
  if(width > dt->max_col_width) {
    dt->max_col_width = width;
#if (QT_VERSION >= 0x050200)
    col_header->setMaximumSectionSize(ConvertCharsToPixels(dt->max_col_width));
#endif
  }
}

void iDataTableView::SetRowHighlight(int row) {
  if(row < 0) {
    dataTable()->row_with_hilite = -1; // toggles off highlight
  }
  else if (dataTable()->row_with_hilite == row) {
    dataTable()->row_with_hilite = -1; // toggles off highlight
  }
  else {
    dataTable()->row_with_hilite = dataTable()->GetIndexRow(row);
  }
  iDataTableModel* mod = qobject_cast<iDataTableModel*>(model());
  if (mod) {
    mod->refreshViews();
  }
}


////////////////////////////////////////////////
//      iTableViewCheckboxDelegate

// Not being used - rohrlich 4/11/16

iTableViewCheckboxDelegate::iTableViewCheckboxDelegate(iTableView* own_tw) :
inherited(own_tw)
{
  own_table_widg = own_tw;
}

QWidget* iTableViewCheckboxDelegate::createEditor(QWidget *parent,
                                                  const QStyleOptionViewItem &option,
                                                  const QModelIndex &index) const {
  QCheckBox* editor = new QCheckBox(parent);
  connect(editor, SIGNAL(stateChanged(int)), this, SLOT(CheckBoxStateChanged(int)));
  return editor;
}

void iTableViewCheckboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QCheckBox *check_box = static_cast<QCheckBox*>(editor);
  bool value = index.model()->data(index, Qt::CheckStateRole).toBool();
  check_box->setChecked(value);
}

void iTableViewCheckboxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                              const QModelIndex& index) const {
  QCheckBox *check_box = static_cast<QCheckBox*>(editor);
  model->setData(index, check_box->isChecked(), Qt::CheckStateRole);
}

void iTableViewCheckboxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionButton button_style;
  button_style.state = QStyle::State_Enabled;
  if(index.model()->data(index, Qt::DisplayRole).toBool() == true)
    button_style.state |= QStyle::State_On;
  else
    button_style.state |= QStyle::State_Off;
  button_style.direction = QApplication::layoutDirection();
  button_style.rect = option.rect;
  QApplication::style()->drawControl(QStyle::CE_CheckBox, &button_style, painter);
}

void iTableViewCheckboxDelegate::CheckBoxStateChanged(int value) {
  QModelIndex current = own_table_widg->currentIndex();
}
