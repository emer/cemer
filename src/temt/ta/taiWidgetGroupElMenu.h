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

#ifndef taiWidgetGroupElMenu_h
#define taiWidgetGroupElMenu_h 1

// parent includes:
#include <taiWidgetListElMenu>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiWidgetGroupElMenu : public taiWidgetListElMenu {
  // menu of elements in the group
  Q_OBJECT
public:
  taiWidgetGroupElMenu(taiWidgetActions::RepType rt, int ft, taList_impl* lst, TypeDef* tp, IWidgetHost* host_, taiWidget* par,
      QWidget* gui_parent_, int flags = 0); // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk
  taiWidgetGroupElMenu(taiWidgetMenu* existing_menu, taList_impl* gp, TypeDef* tp = NULL, IWidgetHost* host_ = NULL, taiWidget* par = NULL,
      QWidget* gui_parent_ = NULL, int flags = 0); // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk

//  void                GetMenu(iMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}

public slots:
  virtual void  ChooseGp();     // chooser callback

protected:
  void         GetMenu_impl(taList_impl* cur_lst, taiWidgetActions* menu, iMenuAction* actn = NULL) CPP11_OVERRIDE;
};

#endif // taiWidgetGroupElMenu_h
