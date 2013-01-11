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

#include "ChannelSpec_List.h"


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
