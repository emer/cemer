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

#ifndef taiWidgetMethodButton_h
#define taiWidgetMethodButton_h 1

// parent includes:
#include <taiWidgetMethod>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiWidgetMethodButton : public taiWidgetMethod {
  // button representation of a method -- uses the QAbstractButton, and sets it as the Rep as well
public:
  QWidget* GetButtonRep() CPP11_OVERRIDE;

  taiWidgetMethodButton(void* bs, MethodDef* md, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0);
};

#endif // taiWidgetMethodButton_h
