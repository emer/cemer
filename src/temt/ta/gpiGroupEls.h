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

#ifndef gpiGroupEls_h
#define gpiGroupEls_h 1

// parent includes:
#include <gpiListEls>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API gpiGroupEls : public gpiListEls {
  // menu of elements in the group
  Q_OBJECT
public:
  gpiGroupEls(taiWidgetActions::RepType rt, int ft, taList_impl* lst, TypeDef* tp, IWidgetHost* host_, taiWidget* par,
      QWidget* gui_parent_, int flags = 0); // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk
  gpiGroupEls(taiMenu* existing_menu, taList_impl* gp, TypeDef* tp = NULL, IWidgetHost* host_ = NULL, taiWidget* par = NULL,
      QWidget* gui_parent_ = NULL, int flags = 0); // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk

//  void                GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}

public slots:
  virtual void  ChooseGp();     // chooser callback

protected:
  override void         GetMenu_impl(taList_impl* cur_lst, taiWidgetActions* menu, taiMenuAction* actn = NULL);
};

#endif // gpiGroupEls_h
