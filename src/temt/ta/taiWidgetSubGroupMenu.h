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

#ifndef taiWidgetSubGroupMenu_h
#define taiWidgetSubGroupMenu_h 1

// parent includes:
#include <taiWidgetTaBaseMenu>

// member includes:
#include <taiWidgetActions>

// declare all other types mentioned but not required to include:
class taGroup_impl; //


class TA_API taiWidgetSubGroupMenu : public taiWidgetTaBaseMenu {
  // menu of sub-groups within a group
  Q_OBJECT
public:
  taGroup_impl*        ths;
  bool          over_max;       // if over max_menu
//  taGroup_impl*      chs_obj;        // object chosen by the chooser

  void          GetMenu(iMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}
  override void GetMenu(taiWidgetActions* menu, iMenuAction* actn = NULL);
  virtual void  UpdateMenu(iMenuAction* actn = NULL);

  QWidget*      GetRep();
  void          GetImage(taGroup_impl* base_gp, taGroup_impl* gp);
  taGroup_impl*        GetValue();

  taiWidgetSubGroupMenu(taiWidgetActions::RepType rt, int ft, taGroup_impl* gp, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0); // uses flags: flgNullOk, flgEditOk
  taiWidgetSubGroupMenu(taiWidgetMenu* existing_menu, taGroup_impl* gp, TypeDef* typ_ = NULL, IWidgetHost* host_ = NULL, taiWidget* par = NULL,
      QWidget* gui_parent_ = NULL, int flags_ = 0); // uses flags: flgNullOk, flgEditOk
public slots:
  virtual void  Edit();         // edit callback
  virtual void  Choose();       // chooser callback

protected:
  virtual void  GetMenu_impl(taGroup_impl* gp, taiWidgetActions* menu, iMenuAction* actn = NULL);
};

#endif // taiWidgetSubGroupMenu_h
