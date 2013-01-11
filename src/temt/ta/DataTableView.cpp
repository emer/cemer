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

#include "DataTableView.h"


void DataTableView::Initialize() {
  data_base = &TA_DataTable;
  view_rows = 3; //note: set by actual class based on screen
  view_range.min = 0;
  view_range.max = -1;
  display_on = true;
  manip_ctrl_on = true;

  main_xform.translate.x = 1.0f;

  updating = 0;
  m_lvp = NULL;
  m_rows = 0;

  children.SetBaseType(&TA_DataColView); // subclasses need to set this to their type!
}

void DataTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(view_range, this);
}

void DataTableView::CutLinks() {
  view_range.CutLinks();
  inherited::CutLinks();
}

void DataTableView::Copy_(const DataTableView& cp) {
  view_rows = cp.view_rows;
  view_range = cp.view_range;
  display_on = cp.display_on;
  manip_ctrl_on = cp.manip_ctrl_on;
}

void DataTableView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //note: UAE calls setDirty, which is where we do most of the rejigging
}

void DataTableView::IgnoredDataChanged(taDataLink* dl, int dcr,
    void* op1, void* op2)
{
  // if it is a struct begin or end, we better not ignore it, because
  // it could require us to reset, so we free locked matrix col slices
  // this could reduce our invisible efficiency BUT is necessary!!!
  if ((dcr == DCR_STRUCT_UPDATE_BEGIN) ||
      (dcr == DCR_STRUCT_UPDATE_END))
  {
    DataDataChanged(dl, dcr, op1, op2);
  }
}

const String DataTableView::caption() const {
  DataTable* dt = dataTable();
  String rval;
  if (dt) {
    rval = dt->GetDisplayName();
  } else rval = "(no table)";
  return rval;
}

void DataTableView::ClearData() {
  m_rows = 0;
  if (dataTable()) {
    dataTable()->ResetData();
  }
  ClearViewRange();
  InitDisplay();
  UpdateDisplay();
}

void DataTableView::ClearViewRange() {
  view_range.min = 0;
  view_range.max = -1; // gets adjusted later
}

void DataTableView::Unbind_impl() {
  if (m_data) {
    SetData(NULL);
    DataTableUnlinked();
  }
  inherited::Unbind_impl(); // will unlink kids
}

void DataTableView::setDataTable(DataTable* dt) {
  if (dataTable() == dt) return;
  if (dt) {
    SetData(dt);
    bool first = (colViewCount() == 0);
    int u_view_rows = dt->GetUserDataAsInt("VIEW_ROWS");
    if (u_view_rows > 0) view_rows = u_view_rows;
    UpdateFromDataTable(first);
    T3DataViewFrame* frame = GetFrame();
    if(frame) {
      main_xform.translate.y = 1.3f * (frame->root_view.children.size - 1); // move to unique position (up)
    }
    //TEST
    UpdatePanel(); // to update name
  } else {
    Unbind(); // also does kids
  }
}

void DataTableView::InitPanel() {
  if (m_lvp)
    m_lvp->InitPanel();
}

void DataTableView::UpdatePanel() {
  if (m_lvp)
    m_lvp->UpdatePanel();
}

bool DataTableView::isVisible() const {
  return (taMisc::gui_active && isMapped());
}

void DataTableView::MakeViewRangeValid() {
  int rows = this->rows();
  if (view_range.min >= rows) {
    view_range.min = MAX(0, (rows - view_rows - 1));
  }
  if(view_rows >= rows) // start at start if we can view whole thing
    view_range.min = 0;
  view_range.max = view_range.min + view_rows - 1; // always keep min and max valid
  view_range.MaxLT(rows - 1); // keep it less than max
}

int DataTableView::CheckRowsChanged(int& orig_rows) {
  DataTable* data_table = dataTable();
  int rval = 0;
  if(!data_table) {
    rval = -m_rows;
    m_rows = 0;
    return rval;
  }
  orig_rows = m_rows;
  rval = data_table->rows - orig_rows;
  m_rows = data_table->rows;
  return rval;
}

void DataTableView::DataUpdateView_impl() {
  if(!display_on) return;
  UpdateDisplay(true);
}

void DataTableView::DoActionChildren_impl(DataViewAction acts) {
  if(acts & RESET_IMPL) return;
  // don't do the reset action on children -- updatefromdatatable does that
  inherited::DoActionChildren_impl(acts);
}


void DataTableView::DataDestroying() {
  Unbind(); //unlinks everyone
  inherited::DataDestroying();
}

void DataTableView::DataTableUnlinked() {
}

void DataTableView::UpdateFromDataTable(bool first) {
  UpdateFromDataTable_child(first);
  UpdateFromDataTable_this(first);
}

void DataTableView::UpdateName() {
  DataTable* dt = dataTable();
  if (dt) {
    if (!name.contains(dt->name))
      SetName(dt->name);
  }
  else {
    if (name.empty())
      SetName("no_table");
  }
}

void DataTableView::DataUpdateAfterEdit_impl() {
  UpdateName();
  inherited::DataUpdateAfterEdit_impl();
}


void DataTableView::UpdateFromDataTable_this(bool first) {
  UpdateName();
}

void DataTableView::UpdateFromDataTable_child(bool first) {
  DataTableCols* cols = &(dataTable()->data);
  DataColView* dcs = NULL;
  DataCol* dc = NULL;
  int i;
/*TODO: revise algorithm as follows:
(may require intermediate link list, to track guys)
1. identify potential orphans, put them aside
2. update existing guys
3. attempt to rebind missing guys to potential orphans
  ex. if cols are in same position, then probably just name difference
4. delete true orphans
5. add missing guys
ALSO: need to probably revise the scheme for reordering -- maybe user
  wants the data in the new order? (OR: maybe should provide separate
  "view_index" in the spec, so user can reorder)
*/
//  delete orphans (except sticky guys)
  for (i = colViewCount() - 1; i >= 0; --i) {
    dcs = colView(i);
    // first, see if it is bound to our table, if so, just update it (name may have changed, etc.
    int old_colno = cols->FindEl(dcs->dataCol());
    if (old_colno >= 0) {
      if(old_colno != i)
        dcs->UpdateFromDataCol(true); // first = get options
      else
        dcs->UpdateFromDataCol(false); // no options
      continue;
    }
    // if not bound, try to find it by name
    dc = cols->FindName(dcs->GetName());
    if (dc) {
      // make sure it is this col bound to the guy!
      dcs->setDataCol(dc);
    } else { // no target found in table
//       if (dcs->sticky) {
//         dcs->setDataCol(NULL); //keep him, but unbind from any col
//       } else {
        children.RemoveIdx(i);
//       }
    }
  }
// items: add missing, order correctly, and update existing (will be only action 1st time)
  for (i = 0; i < cols->size; ++i) {
    dc = cols->FastEl(i);
    bool first = false;
    int fm = children.FindNameIdx(dc->GetName());
    if (fm >= 0) {
      dcs = (DataColView*)children.FastEl(fm);
      if (fm != i) children.MoveIdx(fm, i);
    } else {
      first = true;
      dcs = (DataColView*)taBase::MakeToken(children.el_typ); // of correct type for this
      children.Insert(dcs, i);
    }
    dcs->setDataCol(dc, first);
  }
}

void DataTableView::BuildAll() {
  UpdateFromDataTable();
}

void DataTableView::Render_pre() {
  if (!node_so()) return; // shouldn't happen
  InitPanel();
  inherited::Render_pre();
}

void DataTableView::Render_impl() {
  // set origin: in allocated area
  taTransform* ft = transform(true);
  *ft = main_xform;

  inherited::Render_impl();

  T3Node* node_so = this->node_so(); // cache
  if(!node_so) return;
  SoFont* font = node_so->captionFont(true);
  float font_size = 0.3f;
  font->size.setValue(font_size); // is in same units as geometry units of network
  node_so->setCaption(caption().chars());
  //NOTE: will need to move caption appropriately in subclass
}

void DataTableView::Render_post() {
  inherited::Render_post();
}

void DataTableView::Reset_impl() {
//   ClearViewRange();
  inherited::Reset_impl();
}

void DataTableView::setDirty(bool value) {
  inherited::setDirty(value);
  // this is a bad idea: if the view specs themselves are being updated,
  // it can be for many reasons including rebuilding from original data
  // so we should not do anything here..
  //  if (value) UpdateDisplay();
}

void DataTableView::setDisplay(bool value) {
  if (display_on == value) return;
  display_on = value;
  //  UpdateAfterEdit(); // does the whole kahuna
  UpdateDisplay(false);         //
}

void DataTableView::ViewRow_At(int start) {
  if (!taMisc::gui_active) return;
  int rows = this->rows();
  if (start >= rows)
    start = rows - 1;
  if (start < 0)
    start = 0;
  view_range.min = start;
  UpdateDisplay();
}

void DataTableView::RowBackAll() {
  ViewRow_At(0);
}
void DataTableView::RowBackPg() {
  int cur_row = view_range.min;
  int goto_row = cur_row - view_rows;
  goto_row = MAX(0, goto_row);
  ViewRow_At(goto_row);
}
void DataTableView::RowBack1() {
  int cur_row = view_range.min;
  int goto_row = cur_row - 1;
  goto_row = MAX(0, goto_row);
  ViewRow_At(goto_row);
}
void DataTableView::RowFwd1() {
  int cur_row = view_range.min;
  int goto_row = cur_row + 1;
  goto_row = MIN(rows()-view_rows, goto_row);

  ViewRow_At(goto_row);
}
void DataTableView::RowFwdPg() {
  int cur_row = view_range.min;
  int goto_row = cur_row + view_rows;
  goto_row = MIN(rows()-view_rows, goto_row);

  ViewRow_At(goto_row);
}
void DataTableView::RowFwdAll() {
  ViewRow_At(rows() - view_rows);
}
