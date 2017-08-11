// Copyright 2017, Regents of the University of Colorado,
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

#include "iDataTableColHeaderView.h"
#include <iDataTableView>
#include <DataTable>
#include <taProject>
#include <taMisc>

#include <Qt>
#include <QHeaderView>

iDataTableColHeaderView::iDataTableColHeaderView(QWidget* parent)
:inherited(Qt::Horizontal, parent) {

#if (QT_VERSION >= 0x050000)
  this->setSectionsClickable(true);
  this->setSectionsMovable(true);
#else
  this->setClickable(true);
  this->setMovable(true);
#endif
  this->setSelectionMode(QAbstractItemView::ContiguousSelection);
  this->setSelectionBehavior(QAbstractItemView::SelectColumns);
  this->setContextMenuPolicy(Qt::CustomContextMenu);

  m_section_move_complete = false;      // no section (column) currently being moved

  connect(this, SIGNAL(sectionMoved(int, int, int)), this, SLOT(movedSection(int, int, int)));
  connect(this, SIGNAL(sectionResized(int, int, int)), this, SLOT(resizedSection(int, int, int)));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), parent, SLOT(hor_customContextMenuRequested(const QPoint&)) );
}

iDataTableColHeaderView::~iDataTableColHeaderView() {
}

void iDataTableColHeaderView::movedSection(int logicalIdx, int oldVisualIdx, int newVisualIdx)
{
  if (m_section_move_complete == false) {
    m_section_move_complete = true;
    this->moveSection(newVisualIdx, oldVisualIdx);

    DataTable* dt = dynamic_cast<iDataTableView*>(parent())->dataTable();
    if (!dt)
      return;

    taProject* proj = (taProject*)dt->GetOwner(&TA_taProject);
    if(proj)
      proj->undo_mgr.SaveUndo(dt, "MoveCol", dt);
    dt->MoveCol(oldVisualIdx, newVisualIdx);
  }
  else {
    m_section_move_complete = false;  // ready for another move
  }
}

void iDataTableColHeaderView::resizedSection(int columnIdx, int oldWidth, int newWidth)
{
  DataTable* dt = dynamic_cast<iDataTableView*>(parent())->dataTable();
  if (!dt)
    return;

  // IMPORTANT: this is called from *programatic* resizes too!!  not just from gui
  // interaction.  so we can't do too much here..
  
  // dt->StructUpdate(true);
  DataCol* dc = dt->data.FastEl(columnIdx);
  if(dc) {
    iDataTableView* view = dynamic_cast<iDataTableView*>(parent());
    if (view) {
      if(!dc->HasColFlag(DataCol::AUTO_WIDTH)) {
        dc->width = view->ConvertPixelsToChars(newWidth);
        view->UpdateMaxColWidth(dc->width);
      }
    }
  }
  // dt->StructUpdate(false);
}

iMatrixTableColHeaderView::iMatrixTableColHeaderView(QWidget* parent)
:inherited(Qt::Horizontal, parent) {
  
#if (QT_VERSION >= 0x050000)
  this->setSectionsClickable(true);
  this->setSectionsMovable(false);
#else
  this->setClickable(true);
  this->setMovable(false);
#endif
  this->setSelectionMode(QAbstractItemView::ContiguousSelection);
  this->setSelectionBehavior(QAbstractItemView::SelectColumns);
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), parent, SLOT(hor_customContextMenuRequested(const QPoint&)) );
}

iMatrixTableColHeaderView::~iMatrixTableColHeaderView() {
}
