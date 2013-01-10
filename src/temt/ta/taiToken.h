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

#ifndef taiToken_h
#define taiToken_h 1

// parent includes:
#include <taiElBase>

// member includes:
#include <taiActions>

// declare all other types mentioned but not required to include:

class TA_API taiToken : public taiElBase {
  // for making menus of tokens
  Q_OBJECT
  INHERITED(taiElBase)
public:
//  bool                over_max;       // over max_menu

  override QWidget*     GetRep() { return (ta_actions) ? ta_actions->GetRep() : NULL; }

  virtual void          GetImage(taBase* ths);
    // get image, using the current type and scope
  virtual void          GetImage(taBase* ths, TypeDef* new_typ, taBase* new_scope);
    // get image, using the new type and scope supplied
  virtual taBase*               GetValue(); //
//nn  virtual void              SetTypeScope(TypeDef* new_typ, taBase* new_scope);
    // set a new base type and scope; doesn't update menu

  virtual void          GetUpdateMenu(const taiMenuAction* actn = NULL);
    // gets or updates the menu

private: //test for usage
  taiToken(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = (flgNullOk | flgEditOk)); // uses flags flgNullOk, flgEditOk,
//            bool nul_not=false, bool edt_not=false);
protected:
  taBase*               scope_ref;      // reference object for scoping, default is none

  virtual void  GetMenu_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* actn = NULL);

protected slots:
  virtual void  Edit();         // for edit callback
  virtual void  Chooser();      // for chooser callback
  void          ItemChosen(taiAction* menu_el); // when user chooses from menu
};

#endif // taiToken_h
