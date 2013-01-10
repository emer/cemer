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

#ifndef taiRegexpField_h
#define taiRegexpField_h 1

// parent includes:
#include <taiText>

// member includes:

// declare all other types mentioned but not required to include:
class iRegexpDialogPopulator;

class TA_API taiRegexpField : public taiText {
  // A text field with a "..." button to bring up a context-specific regular expression editor.
  Q_OBJECT
  INHERITED(taiText)
public:
  taiRegexpField(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags, iRegexpDialogPopulator *re_populator);

  void                  SetFieldOwner(const void *fieldOwner);

protected slots:
  override void         btnEdit_clicked(bool);  // "..." button
  override void         lookupKeyPressed();     // Same as clicking the "..." button.

private:
  iRegexpDialogPopulator* m_populator;
  const void *          m_fieldOwner;
};

#endif // taiRegexpField_h
