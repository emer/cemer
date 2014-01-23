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

#ifndef taiWidgetField_h
#define taiWidgetField_h 1

// parent includes:
#include <taiWidgetText>

// member includes:

// declare all other types mentioned but not required to include:
class iDialogWidgetField;

class TA_API taiWidgetField : public taiWidgetText {
  Q_OBJECT
  INHERITED(taiWidgetText)
  friend class iDialogWidgetField;
public:
  taiWidgetField(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags = 0);
  ~taiWidgetField();

protected slots:
  void         btnEdit_clicked(bool) CPP11_OVERRIDE;
  void         lookupKeyPressed() CPP11_OVERRIDE;
  void                  lookupKeyPressed_dialog();

public:
  MemberDef*            lookupfun_md;   // for lookup function, member def
  void*                 lookupfun_base; // for lookup function, base of owner

protected:
  iDialogWidgetField*     edit_dialog;    // an edit dialog, if created
};

#endif // taiWidgetField_h
