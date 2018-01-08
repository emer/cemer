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

#ifndef taiWidgetLinkGroupEditButton_h
#define taiWidgetLinkGroupEditButton_h 1

// parent includes:
#include <taiWidgetGroupEditButton>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiWidgetLinkGroupEditButton : public taiWidgetGroupEditButton {
// link groups don't have the option to create or xfer
public:
  taiWidgetLinkGroupEditButton(void* base, TypeDef* tp, IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_ = 0);
  void         GetMethMenus() override; // exclude certain methods here..
};


#endif // taiWidgetLinkGroupEditButton_h
