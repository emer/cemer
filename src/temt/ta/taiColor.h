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

#ifndef taiColor_h
#define taiColor_h 1

// parent includes:
#include <taiData>

// member includes:

// declare all other types mentioned but not required to include:
class iColorButton; //



class TA_API taiColor : public taiData {
  INHERITED(taiData)
public:
  iColorButton*         rep() const {return (iColorButton*)(QWidget*)m_rep;}

  void                  GetImage(const iColor& val);
  iColor                GetValue() const;

  taiColor(TypeDef* typ_, IWidgetHost* host, taiData* par, QWidget* gui_parent_,
    int flags = 0);

};


#endif // taiColor_h
