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


iTableView::iTableView(QWidget* parent)
:inherited(parent)
{
  ext_select_on = false;
  m_saved_scroll_pos = 0;

  setEditTriggers(DoubleClicked | SelectedClicked | EditKeyPressed | AnyKeyPressed);

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(this_customContextMenuRequested(const QPoint&)) );
  horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(horizontalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(hor_customContextMenuRequested(const QPoint&)) );
  verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  // note: auto resize a lot more important for vertical, for multi dims 
#if (QT_VERSION >= 0x050000)
  verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
  verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif
  connect(verticalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(ver_customContextMenuRequested(const QPoint&)) );
  // wire the selection signals to the UpdateUi, to update enabling
//  connect(me->tv, SIGNAL(activated(const QModelIndex&)), me, SIGNAL(UpdateUi()) );
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

  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed && e->key() == Qt::Key_Y) {
    // go into edit mode then press paste?
    QCoreApplication* app = QCoreApplication::instance();
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Space, Qt::MetaModifier));
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
    return true;
  }
  if(taiMisc::KeyEventFilterEmacs_Edit(obj, e))
    return true;
  if((bool)m_window) {
    if(m_window->KeyEventFilterWindowNav(obj, e))
      return true;
  }

  return inherited::eventFilter(obj, event);
}

void iTableView::keyPressEvent(QKeyEvent* e) {
  if(state() == EditingState) {
    e->ignore();
    return;
  }

  if(e->key() == Qt::Key_Delete) {
    RowColOp(OP_ROW | OP_DELETE);
    e->accept();
    return;
  }

  // note: emacs nav keys have all been converted into basic arrow keys by this point..

  QCoreApplication* app = QCoreApplication::instance();
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  QPersistentModelIndex newCurrent;
  if(ctrl_pressed) {
    switch (e->key()) {
      // note: cannot use Ctrl+Space as that is reserved for selecting item to start editing
    case Qt::Key_S:
      selectCurCell();
      ext_select_on = true;
      e->accept();
      break;
    case Qt::Key_G:
      clearExtSelection();
      e->accept();
      break;
    case Qt::Key_Up:
      newCurrent = moveCursor(MovePageUp, e->modifiers());
      e->accept();
      break;
    case Qt::Key_Down:
      newCurrent = moveCursor(MovePageDown, e->modifiers());
      e->accept();
      break;
    case Qt::Key_Left:
      newCurrent = moveCursor(MoveHome, e->modifiers());
      e->accept();
      break;
    case Qt::Key_Right:
      newCurrent = moveCursor(MoveEnd, e->modifiers());
      e->accept();
      break;
    case Qt::Key_I:
      RowColOp(OP_ROW | OP_INSERT);
      e->accept();
      break;
    case Qt::Key_M:
      RowColOp(OP_ROW | OP_DUPLICATE);
      e->accept();
      break;
    case Qt::Key_D:
      RowColOp(OP_ROW | OP_DELETE);
      e->accept();
      break;
    }
  }
  else {
    // deal with these here to manage the ext select
    switch (e->key()) {
    case Qt::Key_Down:
      newCurrent = moveCursor(MoveDown, e->modifiers());
      e->accept();
      break;
    case Qt::Key_Up:
      newCurrent = moveCursor(MoveUp, e->modifiers());
      e->accept();
      break;
    case Qt::Key_Right:
      newCurrent = moveCursor(MoveRight, e->modifiers());
      e->accept();
      break;
    case Qt::Key_Left:
      newCurrent = moveCursor(MoveLeft, e->modifiers());
      e->accept();
      break;
    }
  }
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
  inherited::keyPressEvent(e);
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

void iTableView::ContextMenuRequested(ContextArea ca, const QPoint& global_pos) {
  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  if(!selectionModel()) return;
  CellRange sel(selectionModel()->selectedIndexes());
  FillContextMenu_impl(ca, menu, sel);
  if (menu->count() > 0) { //only show if any items!
    menu->exec(global_pos);
  }
  delete menu;
}

void iTableView::FillContextMenu_impl(ContextArea ca,
  taiMenu* menu, const CellRange& sel)
{
  taiAction* act = NULL;
  // generic col guys
  if (ca == CA_COL_HDR) {
    if (!isFixedColCount()) {
//note: not include these yet, because fairly complicated semantics, ex
// popping up col editor etc. etc.
/*      act = menu->AddItem("Append Columns", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_COL | OP_APPEND) );
      act = menu->AddItem("Insert Columns", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_COL | OP_INSERT) );*/
      act = menu->AddItem("Delete Columns", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_COL | OP_DELETE) );
      menu->AddSep();
    }
  }
  
  // generic row guys
  if (ca == CA_ROW_HDR) {
    if (!isFixedRowCount()) {
      act = menu->AddItem("Append Rows", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_ROW | OP_APPEND) );
      act = menu->AddItem("Insert Rows (Ctrl+I)", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_ROW | OP_INSERT) );
      act = menu->AddItem("Duplicate Rows (Ctrl+M)", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_ROW | OP_DUPLICATE) );
      act = menu->AddItem("Delete Rows (Ctrl+D)", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_ROW | OP_DELETE) );
    }
    menu->AddSep();
  }
  
  // edit guys
  int ea = 0;
  GetEditActionsEnabled(ea);
    
  act = menu->AddItem("&Copy", taiMenu::normal, taiAction::int_act,
    this, SLOT(EditAction(int)), taiClipData::EA_COPY );
  act->setShortcut(QKeySequence("Ctrl+C"));
  if (!(ea & taiClipData::EA_COPY))
    act->setEnabled(false);
  act = menu->AddItem("&Paste", taiMenu::normal,
      taiAction::int_act, this, SLOT(EditAction(int)), taiClipData::EA_PASTE);
  act->setShortcut(QKeySequence("Ctrl+V"));
  if (!(ea & taiClipData::EA_PASTE)) 
    act->setEnabled(false);
  act = menu->AddItem("Clear", taiMenu::normal,
      taiAction::int_act, this, SLOT(EditAction(int)), taiClipData::EA_CLEAR);
  if (!(ea & taiClipData::EA_CLEAR)) 
    act->setEnabled(false);
    
  menu->AddSep();
  act = menu->AddItem("Select &All", taiAction::action,
    this, SLOT(selectAll()),_nilVariant,
      QKeySequence("Ctrl+A"));
}

void iTableView::RowColOp(int op_code) {
  if(!selectionModel()) return;
  CellRange sel(selectionModel()->selectedIndexes());
  RowColOp_impl(op_code, sel);
}

void iTableView::this_customContextMenuRequested(const QPoint& pos) {
  ContextArea ca = CA_GRID; // TODO: determine if in blank
  ContextMenuRequested(ca, mapToGlobal(pos));
}

void iTableView::hor_customContextMenuRequested(const QPoint& pos) {
  ContextArea ca = CA_COL_HDR; // TODO: determine if in blank
  ContextMenuRequested(ca, horizontalHeader()->mapToGlobal(pos));
}

void iTableView::ver_customContextMenuRequested(const QPoint& pos) {
  ContextArea ca = CA_ROW_HDR; // TODO: determine if in blank
  ContextMenuRequested(ca, verticalHeader()->mapToGlobal(pos));
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

