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

#include "DataColView.h"
#include <DataCol>
#include <DataTableView>


/*
  The main DataColView operations are:
  * initialize ("bind") a new guy from a DataCol
  * update a bound guy (ex things change in the col)
  * unlink a guy (ex. table unlinking)
  * delete a guy (ex. col deletes in table (but table not deleting)
  * bind a guy to a new col (ex., binding to a new table)
  * calculate view-specific parameters based on current state
    -- this is done in Render_impl
*/

void DataColView::Initialize(){
  visible = true;
//   sticky = false;
  data_base = &TA_DataCol;
}

void DataColView::Copy_(const DataColView& cp) {
  visible = cp.visible;
//   sticky = cp.sticky;
}

void DataColView::Destroy() {
  CutLinks();
}

void DataColView::Unbind_impl() {
  if (m_data) setDataCol(NULL);
  inherited::Unbind_impl();
}

void DataColView::DataDestroying() {
  DataColUnlinked();
  inherited::DataDestroying();
}

void DataColView::setDataCol(DataCol* value, bool first_time) {
  if (dataCol() == value) return;
  SetData(value);
  if (value) {
    UpdateFromDataCol(first_time);
  } else {
    DataColUnlinked();
  }
}

void DataColView::UpdateFromDataCol(bool first) {
  UpdateFromDataCol_impl(first);
  DataChanged(DCR_ITEM_UPDATED);
}

void DataColView::UpdateFromDataCol_impl(bool first) {
  DataCol* col = dataCol();
  if (!name.contains(col->name)) {
    SetName(col->name);
  }
  // only copy display options first time, since user may override in view
  if (first) {
    if (col->GetUserData(DataCol::udkey_hidden).toBool())
      visible = false;
  }
}

bool DataColView::isVisible() const {
  return (visible && (bool)m_data);
}

void DataColView::Hide() {
  visible = false;
  DataTableView* par = parent();
  //  par->InitDisplay();
  par->UpdateDisplay();
}
