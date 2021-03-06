// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef taiWidgetColor_h
#define taiWidgetColor_h 1

// parent includes:
#include <taiWidget>

// member includes:

// declare all other types mentioned but not required to include:
class iColorButton; //



class TA_API taiWidgetColor : public taiWidget {
  INHERITED(taiWidget)
public:
  iColorButton*         rep() const {return (iColorButton*)(QWidget*)m_rep;}

  void                  GetImage(const iColor& val);
  iColor                GetValue() const;

  taiWidgetColor(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_,
    int flags = 0);

};


#endif // taiWidgetColor_h
