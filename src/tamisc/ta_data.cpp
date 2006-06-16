// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#include "ta_data.h"


//////////////////////////
//  ChannelSpec	//
//////////////////////////

void ChannelSpec::Initialize() {
  chan_num = -1; // for standalone, means "at end", otherwise, when in list, is set to index number
  val_type = VT_FLOAT; // most common type
}

void ChannelSpec::Copy_(const ChannelSpec& cp) {
  new_group_name = cp.new_group_name;
  chan_num = cp.chan_num;
  val_type = cp.val_type; 
}

const MatrixGeom& ChannelSpec::cellGeom() const {
  static MatrixGeom no_geom;
  return no_geom;
}

const String_Matrix& ChannelSpec::cellNames() const {
  static String_Matrix no_names;
  return no_names;
}

String ChannelSpec::GetColText(int col, int) {
  switch (col) {
  case 0: return chan_num;
  case 1: return name;
  case 2: return ValTypeToStr(val_type);
  case 3: return isMatrix();
  case 4: return isMatrix() ? taMatrix::GeomToString(cellGeom()) : _nilString;
  case 5: return isMatrix() ? String(usesCellNames()) : _nilString;
  case 6: return new_group_name;
  default: return _nilString; // compiler food
  }
}

//////////////////////////
//  MatrixChannelSpec	//
//////////////////////////

void MatrixChannelSpec::Initialize() {
  cell_geom.EnforceSize(1);
  uses_cell_names = false;
}

void MatrixChannelSpec::InitLinks() {
  inherited::InitLinks();
  taBase::Own(cell_geom, this);
  taBase::Own(cell_names, this);
}
 
void MatrixChannelSpec::CutLinks() {
  cell_names.CutLinks();
  cell_geom.CutLinks();
  inherited::CutLinks();
}

void MatrixChannelSpec::Copy_(const MatrixChannelSpec& cp) {
  cell_geom = cp.cell_geom;
  uses_cell_names = cp.uses_cell_names;
  cell_names = cp.cell_names;
}

void MatrixChannelSpec::UpdateAfterEdit() {
  InitCellNames();
  inherited::UpdateAfterEdit();
}

void MatrixChannelSpec::InitCellNames() {
  if (uses_cell_names) {
    cell_names.SetGeomN(cell_geom);
  } else { // no cell names
    cell_names.Reset();
  }
}

void MatrixChannelSpec::SetCellGeom(bool uses_names, int dims,
    int d0, int d1, int d2, int d3, int d4)
{
  uses_cell_names = uses_names;
  cell_geom.EnforceSize(dims);
  cell_geom.Set(0, d0);
  cell_geom.Set(1, d1);
  cell_geom.Set(2, d2);
  cell_geom.Set(3, d3);
  cell_geom.Set(4, d4);
  UpdateAfterEdit();
}

void MatrixChannelSpec::SetCellGeomN(bool uses_names, const MatrixGeom& geom) {
  uses_cell_names = uses_names;
  cell_geom = geom;
  UpdateAfterEdit();
}

//////////////////////////
//  ChannelSpec_List	//
//////////////////////////

/*void ChannelSpec_List::El_SetIndex_(void* it, int idx) {
  ((ChannelSpec*)it)->chan_num = idx;
}*/

String ChannelSpec_List::GetColHeading(int col) {
  static String hd_num("Chan #");
  static String hd_nm("Chan Name");
  static String hd_vt("Val Type");
  static String hd_mat("Is Mat");
  static String hd_gm("Geom");
  static String hd_cl_gm("Use Cell Nms");
  static String hd_gp_nm("New Gp Name");
  
  switch (col) {
  case 0: return hd_num;
  case 1: return hd_nm;
  case 2: return hd_vt;
  case 3: return hd_mat;
  case 4: return hd_gm;
  case 5: return hd_cl_gm;
  case 6: return hd_gp_nm;
  default: return _nilString; // compiler food
  }
}


/* prob nn
/////////////////////////
//  SequenceMaster     //
/////////////////////////

// Note: this singleton uses the safe singleton pattern

SequenceMaster& SequenceMaster::in
/* prob nn
/////////////////////////
//  SequenceMaster     //
/////////////////////////

// Note: this singleton uses the safe singleton pattern

SequenceMaster& SequenceMaster::instance() {
  static SequenceMaster	s_instance; // actually initialized first time through here
  return s_instance;
}
  
SequenceMaster::SequenceMaster()
:m_cycle(1)
{
}stance() {
  static SequenceMaster	s_instance; // actually initialized first time through here
  return s_instance;
}
  
SequenceMaster::SequenceMaster()
:m_cycle(1)
{
}

SequenceMaster::~SequenceMaster()
{
}

void SequenceMaster::Next() {
  ++m_cycle;
}

bool SequenceMaster::UpdateClient(int64_t& client_cycle, int64_t* diff) {
  // updates a client's own cycle count, returning 'true' if it was out of date, and optionally the (+ve) number of cycles by which it was stale
  if (m_cycle <= client_cycle) return false;
  if (diff != NULL) *diff = m_cycle - client_cycle;
  client_cycle = m_cycle;
  return true;
}
*/


