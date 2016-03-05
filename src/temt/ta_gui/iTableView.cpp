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

#include "iTableView.h"
#include <iMainWindowViewer>
#include <iClipData>
#include <CellRange>
#include <iVec2i>
#include <iLineEdit>

#include <taMisc>
#include <taiMisc>

#include <QHeaderView>
#include <QCoreApplication>
#include <QApplication>
#include <QScrollBar>
#include <QKeyEvent>
#include <QWheelEvent>


iTableView::iTableView(QWidget* parent)
:inherited(parent)
{
  edit_start_pos = 0;
  edit_start_kill = false;
  ext_select_on = false;
  m_saved_scroll_pos = 0;

  iTableViewDefaultDelegate* del = new iTableViewDefaultDelegate(this);
  setItemDelegate(del);

  setEditTriggers(DoubleClicked | SelectedClicked | EditKeyPressed | AnyKeyPressed);

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(clicked(const QModelIndex&)), this, SIGNAL(UpdateUi()) );

  installEventFilter(this);
}

void iTableView::clearExtSelection() {
  ext_select_on = false;
  inherited::clearSelection();
}

void iTableView::selectCurCell() {
  clearExtSelection();
  if(!selectionModel()) return;
  selectionModel()->setCurrentIndex(currentIndex(), QItemSelectionModel::ClearAndSelect);
}

bool iTableView::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }
  
  QKeyEvent* e = static_cast<QKeyEvent*>(event);
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed && e->key() == Qt::Key_Y) {
    // go into edit mode then press paste?
    QCoreApplication* app = QCoreApplication::instance();
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Space, Qt::MetaModifier));
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
    return true;
  }
  return inherited::eventFilter(obj, event);
}

void iTableView::keyPressEvent(QKeyEvent* key_event) {
  if(state() == EditingState) {
    key_event->ignore();
    return;
  }

  taiMisc::UpdateUiOnCtrlPressed(this, key_event);
  QPersistentModelIndex newCurrent;
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::DATATABLE_CONTEXT, key_event);
  
  switch (action) {
    case taiMisc::DATATABLE_DELETE:
    case taiMisc::DATATABLE_DELETE_II:
      RowColOp(OP_ROW | OP_DELETE);
      key_event->accept();
      return;
    case taiMisc::DATATABLE_SELECT:
    case taiMisc::DATATABLE_SELECT_II:
      selectCurCell();
      ext_select_on = true;
      key_event->accept();
      break;
    case taiMisc::DATATABLE_CLEAR_SELECTION:
    case taiMisc::DATATABLE_CLEAR_SELECTION_II:
      clearExtSelection();
      key_event->accept();
      break;
    case taiMisc::DATATABLE_MOVE_FOCUS_UP:
    case taiMisc::DATATABLE_MOVE_FOCUS_UP_II:
      newCurrent = moveCursor(MoveUp, QApplication::keyboardModifiers());
      key_event->accept();
      break;
    case taiMisc::DATATABLE_MOVE_FOCUS_DOWN:
    case taiMisc::DATATABLE_MOVE_FOCUS_DOWN_II:
      newCurrent = moveCursor(MoveDown, QApplication::keyboardModifiers());
      key_event->accept();
      break;
    case taiMisc::DATATABLE_MOVE_FOCUS_LEFT:
    case taiMisc::DATATABLE_MOVE_FOCUS_LEFT_II:
      newCurrent = moveCursor(MoveLeft, QApplication::keyboardModifiers());
      key_event->accept();
      break;
    case taiMisc::DATATABLE_MOVE_FOCUS_RIGHT:
    case taiMisc::DATATABLE_MOVE_FOCUS_RIGHT_II:
      newCurrent = moveCursor(MoveRight, QApplication::keyboardModifiers());
      key_event->accept();
      break;
    case taiMisc::DATATABLE_INSERT:
    case taiMisc::DATATABLE_INSERT_II:
      RowColOp(OP_ROW | OP_INSERT);
      key_event->accept();
      break;
    case taiMisc::DATATABLE_INSERT_AFTER:
    case taiMisc::DATATABLE_INSERT_AFTER_II:
      RowColOp(OP_ROW | OP_INSERT_AFTER);
      key_event->accept();
      break;
    case taiMisc::DATATABLE_DUPLICATE:
    case taiMisc::DATATABLE_DUPLICATE_II:
      RowColOp(OP_ROW | OP_DUPLICATE);
      key_event->accept();
      break;
    case taiMisc::DATATABLE_EDIT_HOME:
    case taiMisc::DATATABLE_EDIT_HOME_II:
      edit_start_pos = 0;
      edit_start_kill = false;
      edit(currentIndex());
      key_event->accept();
      break;
    case taiMisc::DATATABLE_EDIT_END:
    case taiMisc::DATATABLE_EDIT_END_II:
      edit_start_pos = -1;
      edit_start_kill = false;
      edit(currentIndex());
      key_event->accept();
      break;
    case taiMisc::DATATABLE_DELETE_TO_END:
    case taiMisc::DATATABLE_DELETE_TO_END_II:
      edit_start_pos = 0;
      edit_start_kill = true;
      edit(currentIndex());
      key_event->accept();
      break;
    case taiMisc::DATATABLE_PAGE_UP:
    case taiMisc::DATATABLE_PAGE_UP_II:
    {
      newCurrent = moveCursor(MovePageUp, QApplication::keyboardModifiers());
      key_event->accept();
      break;
    }
    case taiMisc::DATATABLE_PAGE_DOWN:
    case taiMisc::DATATABLE_PAGE_DOWN_II:
      newCurrent = moveCursor(MovePageDown, QApplication::keyboardModifiers());
      key_event->accept();
      break;
    default:
      ;  // no op - without default you get a warning about missing cases
  }
  
  //    case Qt::Key_Left:
  //      newCurrent = moveCursor(MoveHome, QApplication::keyboardModifiers());
  //      e->accept();
  //      break;
  //    case Qt::Key_Right:
  //      newCurrent = moveCursor(MoveEnd, QApplication::keyboardModifiers());
  //      e->accept();
//      break;

  // from qabstractitemview.cpp
  QPersistentModelIndex oldCurrent = currentIndex();
  if(newCurrent != oldCurrent && newCurrent.isValid()) {
    QItemSelectionModel::SelectionFlags command;
    if(ext_select_on) {
      command = QItemSelectionModel::Select;
    }
    else {
      command = QItemSelectionModel::ClearAndSelect;
    }
    selectionModel()->setCurrentIndex(newCurrent, command);
    return;
  }
  inherited::keyPressEvent(key_event);
}

bool iTableView::event(QEvent* ev) {
  //NOTE: this probably doesn't even get triggered
  bool rval = inherited::event(ev);
  // look for anything that indicates we are focused
  QEvent::Type t = ev->type();
  if ((t == QEvent::FocusIn) ||
      (t == QEvent::KeyPress) ||
      (t == QEvent::MouseButtonPress) //||
      ) {
    emit hasFocus(this);
    if(t == QEvent::KeyPress)
      ev->accept();		// this prevents beep on mac due to unaccepted event!
  }
  return rval;
}

void iTableView::FillContextMenu_impl(ContextArea ca,
  taiWidgetMenu* menu, const CellRange& sel)
{
  iAction* act = NULL;
  // generic col guys
  if (ca == CA_COL_HDR) {
    act = menu->AddItem("Set Column Width...", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_COL | OP_SET_WIDTH) );
    act = menu->AddItem("Resize Width to Content", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_COL | OP_RESIZE_TO_CONTENT) );
    act = menu->AddItem("Resize All Widths to Content", taiWidgetMenu::normal,
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
    act = menu->AddItem("Show Only Selected Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_DELETE_UNSELECTED) );
    act = menu->AddItem("Show All Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_SHOW_ALL) );
    menu->AddSep();
  }
  
  // edit guys
  int ea = 0;
  GetEditActionsEnabled(ea);
    
  act = menu->AddItem("&Copy", taiWidgetMenu::normal, iAction::int_act,
                      this, SLOT(EditAction(int)), iClipData::EA_COPY );
  act->setShortcut(QKeySequence("Ctrl+C"));
  if (!(ea & iClipData::EA_COPY)) {
    act->setEnabled(false);
  }
  act = menu->AddItem("&Paste", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(EditAction(int)), iClipData::EA_PASTE);
  act->setShortcut(QKeySequence("Ctrl+V"));
  if (!(ea & iClipData::EA_PASTE)) {
    act->setEnabled(false);
  }
  act = menu->AddItem("Clear", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(EditAction(int)), iClipData::EA_CLEAR);
  if (!(ea & iClipData::EA_CLEAR)) 
    act->setEnabled(false);
    
  menu->AddSep();
  menu->AddItem("Select &All", iAction::action,
                this, SLOT(selectAll()),_nilVariant,
                QKeySequence("Ctrl+A"));

  act = menu->AddItem("&View", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(ViewAction(int)), 1);
  if (ca == CA_COL_HDR) {
    if (sel.col_fr != sel.col_to) {
      act->setEnabled(false);
    }
  }
  else if (ca == CA_ROW_HDR) {
    if (sel.row_fr != sel.row_to) {
      act->setEnabled(false);
    }
  }
  act = menu->AddItem("&Reset Colors", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(ResetColorsAction(int)), 1);

}

void iTableView::RowColOp(int op_code) {
  if(!selectionModel()) return;
  CellRange sel(selectionModel()->selectedIndexes());
  RowColOp_impl(op_code, sel);
}

void iTableView::this_customContextMenuRequested(const QPoint& pos) {
  taiWidgetMenu* menu = new taiWidgetMenu(this, taiWidgetMenu::normal, taiMisc::fonSmall);
  if(!selectionModel())
    return;
  CellRange sel(selectionModel()->selectedIndexes());
  FillContextMenu_impl(CA_GRID, menu, sel);
  if (menu->count() > 0) {
    menu->exec(mapToGlobal(pos));
  }
  delete menu;
}

void iTableView::hor_customContextMenuRequested(const QPoint& pos) {
  int columnIndex = horizontalHeader()->logicalIndexAt(pos);
  // ** don't clear current selection if this column (individually or as one of a group of columns) is already selected
  QModelIndex mdlIndex = model()->index(0, columnIndex);
  if (!selectionModel()->isColumnSelected(columnIndex, mdlIndex.parent())) {
    this->clearSelection();
  }
  QItemSelection columnSelection;
  // we select the entire column so we don't need to specify the rows
  QModelIndex topLeft = model()->index(0, columnIndex);
  QModelIndex bottomRight = model()->index(0, columnIndex);
  columnSelection.select(topLeft, bottomRight);
  this->selectionModel()->select(columnSelection, QItemSelectionModel::Select | QItemSelectionModel::Columns);

  // build and display menu
  taiWidgetMenu* menu = new taiWidgetMenu(this, taiWidgetMenu::normal, taiMisc::fonSmall);
  CellRange sel(selectionModel()->selectedIndexes());
  FillContextMenu_impl(CA_COL_HDR, menu, sel);
  if (menu->count() > 0) {
    menu->exec(horizontalHeader()->mapToGlobal(pos));
  }
  delete menu;
}

void iTableView::ver_customContextMenuRequested(const QPoint& pos) {
  int rowIndex = verticalHeader()->logicalIndexAt(pos);
  // ** don't clear current selection if this row (individually or as one of a group of rows) is already selected
  QModelIndex mdlIndex = model()->index(rowIndex, 0);
  if (!selectionModel()->isRowSelected(rowIndex, mdlIndex.parent())) {
    this->clearSelection();
  }
  QItemSelection rowSelection;
  // we select the entire row so we don't need to specify the rows
  QModelIndex topLeft = model()->index(rowIndex, 0);
  QModelIndex bottomRight = model()->index(rowIndex, 0);
  rowSelection.select(topLeft, bottomRight);
  this->selectionModel()->select(rowSelection, QItemSelectionModel::Select | QItemSelectionModel::Rows);

  // build and display menu
  taiWidgetMenu* menu = new taiWidgetMenu(this, taiWidgetMenu::normal, taiMisc::fonSmall);
  CellRange sel(selectionModel()->selectedIndexes());
  FillContextMenu_impl(CA_ROW_HDR, menu, sel);
  if (menu->count() > 0) {
    menu->exec(verticalHeader()->mapToGlobal(pos));
  }
  delete menu;
}

void iTableView::SaveScrollPos() {
  m_saved_scroll_pos = verticalScrollBar()->value();
}

void iTableView::RestoreScrollPos() {
  ScrollTo(m_saved_scroll_pos);
}

void iTableView::ScrollTo(int scr_pos) {
  taiMisc::ScrollTo_SA(this, scr_pos);
}

void iTableView::CenterOn(QWidget* widg) {
  taiMisc::CenterOn_SA(this, this, widg);
}

void iTableView::KeepInView(QWidget* widg) {
  taiMisc::KeepInView_SA(this, this, widg);
}  

bool iTableView::PosInView(int scr_pos) {
  return taiMisc::PosInView_SA(this, scr_pos);
}

QPoint iTableView::MapToTree(QWidget* widg, const QPoint& pt) {
  return taiMisc::MapToArea_SA(this, this, widg, pt);
}

int iTableView::MapToTreeV(QWidget* widg, int pt_y) {
  return taiMisc::MapToAreaV_SA(this, this, widg, pt_y);
}

bool iTableView::SelectedRows(int& st_row, int& end_row) {
  st_row = -1;
  end_row = -1;
  QModelIndexList sels = selectedIndexes();
  if(sels.count() == 0) {
    return false;
  }
  st_row = sels.at(0).row();
  end_row = st_row;
  for(int i=1; i<sels.count(); i++) {
    QModelIndex idx = sels.at(i);
    st_row = MIN(st_row, idx.row());
    end_row = MAX(end_row, idx.row());
  }
  return true;
}

bool iTableView::SelectRows(int st_row, int end_row) {
  int maxcol = model()->columnCount(rootIndex())-1;
  int maxrow = model()->rowCount(rootIndex())-1;
  QModelIndex tl = model()->index(st_row, 0, rootIndex()); // start
  if(!tl.isValid())
    return false;
  
  end_row = MIN(maxrow, end_row);
  QModelIndex br = model()->index(end_row, maxcol, rootIndex()); // end
  if(!br.isValid())
    return false;
  
  selectionModel()->select(QItemSelection(tl,br), QItemSelectionModel::ClearAndSelect);
  return true;
}

bool iTableView::SelectColumns(int st_col, int end_col) {
  int maxcol = model()->columnCount(rootIndex())-1;
  int maxrow = model()->rowCount(rootIndex())-1;
  QModelIndex tl = model()->index(0, st_col, rootIndex());  // start
  if(!tl.isValid())
    return false;
  
  end_col = MIN(maxcol, end_col);
  QModelIndex br = model()->index(maxrow, end_col, rootIndex()); // end
  if(!br.isValid())
    return false;
  
  selectionModel()->select(QItemSelection(tl,br), QItemSelectionModel::ClearAndSelect);
  return true;
}

void iTableView::wheelEvent(QWheelEvent *e)
{
  // attempt to get reduce scrolling in orthogonal direction when only wanting to scroll horizontal or vertical
  if(QApplication::keyboardModifiers() & Qt::ShiftModifier) {
    int h_scroll_pos = horizontalScrollBar()->value();
    int v_scroll_pos = verticalScrollBar()->value();
    
    int delta_x = e->pos().x() - last_x;
    int delta_y = e->pos().y() - last_y;

    last_x = e->pos().x();
    last_y = e->pos().y();
    
    if (delta_y > 2*delta_x) {
      horizontalScrollBar()->setValue(h_scroll_pos);
    }
    if (delta_x > 2*delta_y) {
      verticalScrollBar()->setValue(v_scroll_pos);
    }
  }
  inherited::wheelEvent(e);
}

void iTableView::SetCurrentAndSelect(int row, int col) {
  this->selectionModel()->clearSelection();
  // for single cell only
  QModelIndex topLeft = model()->index(row, col);
  QModelIndex bottomRight = model()->index(row, col);
  QItemSelection selection(topLeft, bottomRight);
  this->selectionModel()->select(selection, QItemSelectionModel::Select);
  this->setCurrentIndex(topLeft);
  this->setFocus();
}


////////////////////////////////////////////////
//      iTableViewDefaultDelegate


iTableViewDefaultDelegate::iTableViewDefaultDelegate(iTableView* own_tw) :
  inherited(own_tw)
{
  own_table_widg = own_tw;
}

QWidget* iTableViewDefaultDelegate::createEditor(QWidget *parent,
                                                  const QStyleOptionViewItem &option,
                                                  const QModelIndex &index) const {
  QWidget* widg = inherited::createEditor(parent, option, index);
  QLineEdit* le = dynamic_cast<QLineEdit*>(widg);
  if(le) {
    iLineEdit* il = new iLineEdit(le->text().toLatin1(), parent);
    il->init_start_pos = own_table_widg->edit_start_pos;
    il->init_start_kill = own_table_widg->edit_start_kill;
    // if(own_table_widg) {
    //   QObject::connect(il, SIGNAL(lookupKeyPressed(iLineEdit*)),
    //                    own_table_widg, SLOT(lookupKeyPressed(iLineEdit*)) );
    // }
    return il;
  }
  return widg;
}

void iTableViewDefaultDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                              const QModelIndex& index) const {
  inherited::setModelData(editor, model, index);
  own_table_widg->edit_start_pos = 0;
  own_table_widg->edit_start_kill = false;
  // if(own_table_widg)
  //   own_table_widg->itemWasEdited(index);
}


