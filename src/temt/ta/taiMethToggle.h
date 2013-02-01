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

#ifndef taiMethToggle_h
#define taiMethToggle_h 1

// parent includes:
#include <taiMethodData>

// member includes:

// declare all other types mentioned but not required to include:
class QCheckBox; //


class TA_API taiMethToggle : public taiMethodData {
  // toggle representation of a method (does not call directly, but checks flag)
  Q_OBJECT
public:
  inline QCheckBox*     rep() {return (QCheckBox*)(QWidget*)m_rep;}

  taiMethToggle(void* bs, MethodDef* md, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0);

public slots:
  void                  CallFun(); // replaces
};

#endif // taiMethToggle_h
