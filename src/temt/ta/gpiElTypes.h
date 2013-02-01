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

#ifndef gpiElTypes_h
#define gpiElTypes_h 1

// parent includes:
#include <taiTypeHier>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API gpiElTypes : public taiTypeHier {
// TypeHier provides the guts, we just replace the NULL default with "Group"
INHERITED(taiTypeHier)
public:
  TypeDef*      lst_typd;       // typedef of the list

  gpiElTypes(taiWidgetActions::RepType rt, int ft, TypeDef* lstd, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0); // no flags
  gpiElTypes(taiWidgetMenu* existing_menu, TypeDef* lstd, TypeDef* typ_, IWidgetHost* host_,
      taiWidget* par, QWidget* gui_parent_, int flags_ = 0); // no flags

  USING(inherited::GetMenu)
  void          GetMenu(iMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}
  override void         GetMenu(taiWidgetActions* menu, iMenuAction* nact = NULL);
};

#endif // gpiElTypes_h
