// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// virtual table instantiation file
 
#include "rbp.h"
#include <ta_misc/datagraph.h>
#include <ta_misc/datatable_iv.h>
#include <ta/ta_dump.h>
#include <pdp/pdpshell.h>
#include <pdp/pdp_iv.h>
#include <pdp/pdplog.h>
#include <pdp/net_iv.h>
#include <pdp/netstru_extra.h>
#include <pdp/enviro_extra.h>
#include <pdp/procs_extra.h>
#include <css/css_iv.h>
#include <css/c_ptr_types.h>
#include <css/ta_css.h>

#include <ta/enter_iv.h>
#include <InterViews/patch.h>
#include <InterViews/window.h>
#include <ta/leave_iv.h>

class GetWinPosPatch : public ivPatch {
public:
  WinBase*	obj;

  GetWinPosPatch(WinBase* wb, ivGlyph* g) : ivPatch(g) { obj = wb; }

  void 	allocate(ivCanvas* c, const ivAllocation& a, ivExtension& e) {
    ivPatch::allocate(c, a, e);
     if(obj->IsMapped()) {
      obj->win_pos.wd = obj->window->width();
#ifndef CYGWIN
      obj->win_pos.ht = obj->window->height();
#else
      obj->win_pos.ht = obj->window->height() - 20.0f; // subtract out window bar
#endif
    }
  }
};
