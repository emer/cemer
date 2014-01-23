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

#ifndef taiWidgetFieldRegexp_h
#define taiWidgetFieldRegexp_h 1

// parent includes:
#include <taiWidgetText>

// member includes:

// declare all other types mentioned but not required to include:
class iDialogRegexpPopulator;

class TA_API taiWidgetFieldRegexp : public taiWidgetText {
  // A text field with a "..." button to bring up a context-specific regular expression editor.
  Q_OBJECT
  INHERITED(taiWidgetText)
public:
  taiWidgetFieldRegexp(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags, iDialogRegexpPopulator *re_populator);

  void                  SetFieldOwner(const void *fieldOwner);

protected slots:
  void         btnEdit_clicked(bool) CPP11_OVERRIDE;  // "..." button
  void         lookupKeyPressed() CPP11_OVERRIDE;     // Same as clicking the "..." button.

private:
  iDialogRegexpPopulator* m_populator;
  const void *          m_fieldOwner;
};

#endif // taiWidgetFieldRegexp_h
