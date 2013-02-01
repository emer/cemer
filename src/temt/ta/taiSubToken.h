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

#ifndef taiSubToken_h
#define taiSubToken_h 1

// parent includes:
#include <taiElBase>

// member includes:
#include <taiWidgetActions>

// declare all other types mentioned but not required to include:

class TA_API taiSubToken : public taiElBase {
  // Menu for sub tokens of a giventype
  Q_OBJECT
  INHERITED(taiElBase)
public:
  void*         menubase;

  QWidget*      GetRep();

  USING(inherited::GetMenu)
  virtual void  GetMenu(taiMenuAction* actn = NULL);
  virtual void  UpdateMenu(taiMenuAction* actn = NULL);
  virtual void  GetMenuImpl(void* base, taiMenuAction* actn = NULL);

  virtual void  GetImage(const void* ths, void* sel=NULL);
  virtual void* GetValue();

  taiSubToken(taiWidgetActions::RepType rt, int ft, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
      QWidget* gui_parent_, int flags_ = flgEditOk); // uses flgNullOk, and flgEditOk
  taiSubToken(taiMenu* existing_menu, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
      QWidget* gui_parent_, int flags_ = flgEditOk); // uses flgNullOk, and flgEditOk

public slots:
  virtual void  Edit();         // for edit callback
};

#endif // taiSubToken_h
