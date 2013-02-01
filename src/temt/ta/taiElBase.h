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

#ifndef taiElBase_h
#define taiElBase_h 1

// parent includes:
#include <taiData>

// member includes:

// declare all other types mentioned but not required to include:
class taiActions; //
class taiMenuAction; // #IGNORE

// base class for sundry taiData items that use a menu, and have a taBase-derived current item
class TA_API taiElBase: public taiData {
  INHERITED(taiData)
public:
  taBase*               cur_obj;
  override void         SigEmit(taiData* chld = NULL); // do autoapply
//  void                GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_menu, actn);}
  virtual void  GetMenu(taiActions* actions, taiMenuAction* actn = NULL) {} // variant provided for MenuGroup_impl in winbase
  void          setCur_obj(taBase* value, bool do_chng = true); // set cur_obj and notifies change if different
  taiElBase(taiActions* actions_, TypeDef* tp, IWidgetHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  ~taiElBase();
protected:
  taiActions*   ta_actions;
  bool          ownflag;
};

#endif // taiElBase_h
