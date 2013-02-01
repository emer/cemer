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

#ifndef taiWidgetPlusToggle_h
#define taiWidgetPlusToggle_h 1

// parent includes:
#include <taiWidgetComposite>

// member includes:

// declare all other types mentioned but not required to include:
class iCheckBox;


class TA_API taiWidgetPlusToggle : public taiWidgetComposite {
  // a regular field plus a toggle..
  Q_OBJECT
  INHERITED(taiWidgetComposite)
public:
  taiWidget*              data; // set by caller after creating contained class
  iCheckBox*            but_rep;
  inline QWidget*       rep() {return (QWidget*)m_rep;} //parent of contained controls

  taiWidgetPlusToggle(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags_ = 0);
  ~taiWidgetPlusToggle();

  override void InitLayout();

  virtual int   GetValue();
  virtual void  GetImage(bool chk);
public slots:
  virtual void  Toggle_Callback();

protected:
  override void SigEmit_impl(taiWidget* chld);
  override void applyNow();
};

#endif // taiWidgetPlusToggle_h
