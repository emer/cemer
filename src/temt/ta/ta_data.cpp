// Copyright, 1995-2007, Regents of the University of Colorado,
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



#include "ta_data.h"


//////////////////////////
//  ChannelSpec	//
//////////////////////////

void ChannelSpec::Initialize() {
  chan_num = -1; // for standalone, means "at end", otherwise, when in list, is set to index number
  val_type = VT_FLOAT; // most common type
}

void ChannelSpec::Copy_(const ChannelSpec& cp) {
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

/*obs??? String ChannelSpec::GetColText(int col, int) {
  switch (col) {
  case 0: return chan_num;
  case 1: return name;
  case 2: return ValTypeToStr(val_type);
  case 3: return isMatrix();
  case 4: return isMatrix() ? cellGeom().GeomToString() : _nilString;
  case 5: return isMatrix() ? String(usesCellNames()) : _nilString;
  default: return _nilString; // compiler food
  }
}*/

//////////////////////////
//  MatrixChannelSpec	//
//////////////////////////

void MatrixChannelSpec::Initialize() {
  cell_geom.SetSize(1);
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
  cell_geom.SetSize(dims);
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

//NOTE: following are hacks to keep code working

const KeyString ChannelSpec_List::GetListColKey(int col) const 
{
  return (KeyString)col;
}
String ChannelSpec_List::GetColHeading(const KeyString& key) const {
  return GetColHeadingIdx(key.toInt());
}

String ChannelSpec_List::GetColHeadingIdx(int col) const {
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

void ChannelSpec_List::UpdateDataBlockSchema(DataBlock* db) {
  if (!db) return;
  for (int i = 0; i < size; ++i) {
    ChannelSpec* cs = FastEl(i);
    db->AssertSinkChannel(cs);
  }
}

//////////////////////////
//  DataBlock		//
//////////////////////////

const Variant DataBlock::GetData(int chan) { 
  if (ReadAvailable() && SourceChannelInRange(chan)) 
    return GetData_impl(chan);
  else return _nilVariant;
}

bool DataBlock::SetMatrixCellData(const Variant& data, int chan, int cell) 
{ 
  taMatrix* mat = GetSinkMatrix(chan); // mat is ref'ed!
  if (mat) { 
    mat->SetFmVar_Flat(data, cell);
    taBase::unRefDone(mat);
    return true; 
  }
  return false; 
}

bool DataBlock::SetMatrixCellDataByName(const Variant& data, const String& ch_nm,
  int cell) 
{ 
  taMatrix* mat = GetSinkMatrixByName(ch_nm); // mat is ref'ed!
  if(mat) {
    mat->SetFmVar_Flat(data, cell);
    taBase::unRefDone(mat);
    return true; 
  }
  return false;
}