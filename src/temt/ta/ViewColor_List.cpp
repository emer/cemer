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

#include "ViewColor_List.h"

TA_BASEFUNS_CTORS_DEFN(ViewColor_List);

bool ViewColor_List::FindMakeViewColor(const String& nm, const String& dsc,
				       bool fg, const String& fg_name,
				       bool bg, const String& bg_name) {
  ViewColor* vc = FindName(nm);
  bool rval = true;
  if(!vc) {
    vc = new ViewColor;
    vc->name = nm;		// note: this list has a hash table typicaly, so make sure name is set first before adding!
    Add(vc);
    rval = false;
  }
  vc->desc = dsc;
  vc->use_fg = fg;
  vc->use_bg = bg;
  if(fg) {
    vc->fg_color.name = fg_name;
    vc->fg_color.UpdateAfterEdit();
  }
  if(bg) {
    vc->bg_color.name = bg_name;
    vc->bg_color.UpdateAfterEdit();
  }
  return rval;
}
