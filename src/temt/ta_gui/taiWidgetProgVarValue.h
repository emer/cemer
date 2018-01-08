// Copyright 2017-2018, Regents of the University of Colorado,
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

#ifndef taiWidgetProgVarValue_h
#define taiWidgetProgVarValue_h 1

// parent includes:
#include <taiWidget>

// member includes:

// declare all other types mentioned but not required to include:
class ProgVar; //

taTypeDef_Of(taiWidgetProgVarValue);

class TA_API taiWidgetProgVarValue : public taiWidget {
  Q_OBJECT
public:
  taiWidgetProgVarValue(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags_ = 0);
  taiWidgetProgVarValue(bool is_enum, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
    QWidget* gui_parent_, int flags_ = 0); // treats typ as enum, and fills values

  taiWidget*    sub_widget;     // specific widget for given subtype
  
  void          GetSubWidget(ProgVar* val);
  void          GetImageProgVar(ProgVar* val);
  void          GetValueProgVar(ProgVar* val) const;
};

#endif // taiWidgetProgVarValue_h
