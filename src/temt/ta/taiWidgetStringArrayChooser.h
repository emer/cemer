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

#ifndef taiWidgetStringArrayChooser_h
#define taiWidgetStringArrayChooser_h 1

// parent includes:
#include <taiWidgetItemChooser>

// member includes:
#include <String_Array>

// declare all other types mentioned but not required to include:

class TA_API taiWidgetStringArrayChooser : public taiWidgetItemChooser {
// for strings in an array
INHERITED(taiWidgetItemChooser)
public:
  String_Array*         str_ary;

  inline String*        item() const {return (String*)m_sel;}
  int                   columnCount(int view) const override;
  const String          headerText(int index, int view) const override;
  int                   viewCount() const override {return 1;}
  const String          viewText(int index) const override;
  const String          titleText() override;

  String*               GetValue() {return item();}
  using inherited::GetImage;
  void                  GetImage(String_Array* ary, String* it);

  void                  BuildChooser(iDialogItemChooser* ic, int view = 0) override;

  taiWidgetStringArrayChooser(TypeDef* typ_, IWidgetHost* host,
                       taiWidget* par, QWidget* gui_parent_, int flags_ = 0,
                       const String& flt_start_txt = "");
protected:
  const String          labelNameNonNull() const override;

  virtual int           BuildChooser_0(iDialogItemChooser* ic);
  // we use this recursively, and also in gpi guy
};

#endif // taiWidgetStringArrayChooser_h
