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

#ifndef taiWidgetListElChooser_h
#define taiWidgetListElChooser_h 1

// parent includes:
#include <taiWidgetListElChooser_base>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiWidgetListElChooser : public taiWidgetListElChooser_base {
  // for items in a list
  INHERITED(taiWidgetListElChooser_base)
public:
  taList_impl*               list;

  const String titleText() override;

  using inherited::GetImage;
  void                  GetImage(taList_impl* base_lst, taBase* it);

  void                  BuildChooser(iDialogItemChooser* ic, int view = 0) override;

  taiWidgetListElChooser(TypeDef* typ, IWidgetHost* host, taiWidget* par,
                   QWidget* gui_parent_, int flags_ = 0,
                   const String& flt_start_txt = ""); //note: typ is type of list
};

#endif // taiWidgetListElChooser_h
