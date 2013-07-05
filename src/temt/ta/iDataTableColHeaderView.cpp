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

#include "iDataTableColHeaderView.h"
#include <iDataTableView>
#include <DataTable>
#include <taProject>

#include <Qt>
#include <QHeaderView>

iDataTableColHeaderView::iDataTableColHeaderView(QWidget* parent)
:inherited(Qt::Horizontal, parent) {

  this->setClickable(true);
  this->setMovable(true);
  this->setSelectionMode(QAbstractItemView::ContiguousSelection);
  this->setSelectionBehavior(QAbstractItemView::SelectColumns);
  this->setContextMenuPolicy(Qt::CustomContextMenu);

  m_section_move_complete = false;      // no section (column) currently being moved

  connect(this, SIGNAL(sectionMoved(int, int, int)), this, SLOT(movedSection(int, int, int)));
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

