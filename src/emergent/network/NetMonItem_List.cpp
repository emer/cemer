// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "NetMonItem_List.h"

TA_BASEFUNS_CTORS_DEFN(NetMonItem_List);

String NetMonItem_List::GetColHeading(const KeyString& key) const {
  static String col_obj("Object Name");
  static String col_typ("Object Type");
  static String col_var("Variable");

  if (key == NetMonItem::key_obj_name) return col_obj;
  else if (key == NetMonItem::key_obj_type) return col_typ;
  else if (key == NetMonItem::key_obj_var) return col_var;
  else return inherited::GetColHeading(key);
}


const KeyString NetMonItem_List::GetListColKey(int col) const {
  switch (col) {
  case 0: return NetMonItem::key_obj_name;
  case 1: return NetMonItem::key_obj_type;
  case 2: return NetMonItem::key_obj_var;
  default: return _nilKeyString;
  }
}

