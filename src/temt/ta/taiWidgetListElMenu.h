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

#ifndef taiWidgetListElMenu_h
#define taiWidgetListElMenu_h 1

// parent includes:
#include <taiWidgetTaBaseMenu>

// member includes:
#include <taiWidgetActions>

// declare all other types mentioned but not required to include:
class taList_impl; //


class TA_API taiWidgetListElMenu : public taiWidgetTaBaseMenu {
  // #OBSOLETE menu of elements in the list
  Q_OBJECT
public:
  bool          over_max;       // if over max_menu
  taList_impl*       ths;

  void          GetMenu(iMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}
  void  GetMenu(taiWidgetActions* menu, iMenuAction* actn = NULL) CPP11_OVERRIDE; // variant provided for MenuGroup_impl in winbase
  virtual void  UpdateMenu(iMenuAction* actn = NULL);

  QWidget*      GetRep();
  void          GetImage(taList_impl* base_lst, taBase* it);
  taBase*       GetValue();

  taiWidgetListElMenu(taiWidgetActions::RepType rt, int ft, taList_impl* lst, TypeDef* tp, IWidgetHost* host_, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0); // flags include: flgNullOk, flgNoList, flgEditOk
  taiWidgetListElMenu(taiWidgetMenu* existing_menu, taList_impl* gp, TypeDef* tp, IWidgetHost* host_, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0); // flags include: flgNullOk, flgNoList, flgEditOk

public slots:
  virtual void  Edit();         // edit callback
  virtual void  Choose();       // chooser callback

protected:
  virtual void  GetMenu_impl(taList_impl* lst, taiWidgetActions* menu, iMenuAction* actn = NULL);
};

#endif // taiWidgetListElMenu_h
