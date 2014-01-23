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

#ifndef taiWidgetGroupElChooser_h
#define taiWidgetGroupElChooser_h 1

// parent includes:
#include <taiWidgetListElChooser_base>

// member includes:

// declare all other types mentioned but not required to include:
class taGroup_impl; //


class TA_API taiWidgetGroupElChooser : public taiWidgetListElChooser_base {
// for items in a list
INHERITED(taiWidgetListElChooser_base)
public:
  taGroup_impl*         grp;

  const String titleText() CPP11_OVERRIDE;
  using inherited::GetImage;
  void                  GetImage(taGroup_impl* base_grp, taBase* it);

  void                  BuildChooser(iDialogItemChooser* ic, int view = 0); // override

  taiWidgetGroupElChooser(TypeDef* typ, IWidgetHost* host, taiWidget* par,
                    QWidget* gui_parent_, int flags_ = 0,
                    const String& flt_start_txt = ""); //note: typ is type of grp
protected:
  int                   BuildChooser_1(iDialogItemChooser* ic, taGroup_impl* top_grp,
    QTreeWidgetItem* top_item); // we use this recursively, and also in gpi guy
};

#endif // taiWidgetGroupElChooser_h
