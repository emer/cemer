// Copyright 2016-2017, Regents of the University of Colorado,
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

#ifndef taiWidgetNameVarArrayChooser_h
#define taiWidgetNameVarArrayChooser_h 1

// parent includes:
#include <taiWidgetItemChooser>

// member includes:

// declare all other types mentioned but not required to include:
class NameVar_PArray; //
class NameVar; //

class TA_API taiWidgetNameVarArrayChooser : public taiWidgetItemChooser {
// for strings in an array
INHERITED(taiWidgetItemChooser)
public:
  NameVar_PArray*         str_ary;

  inline NameVar*        item() const {return (NameVar*)m_sel;}
  int                   columnCount(int view) const override;
  const String          headerText(int index, int view) const override;
  int                   viewCount() const override {return 1;}
  const String          viewText(int index) const override;
  const String          titleText() override;

  NameVar*               GetValue() {return item();}
  using inherited::GetImage;
  void                  GetImage(NameVar_PArray* ary, NameVar* it);

  void                  BuildChooser(iDialogItemChooser* ic, int view = 0) override;

  taiWidgetNameVarArrayChooser(TypeDef* typ_, IWidgetHost* host,
                       taiWidget* par, QWidget* gui_parent_, int flags_ = 0,
                       const String& flt_start_txt = "");
protected:
  const String          labelNameNonNull() const override;

  virtual int           BuildChooser_0(iDialogItemChooser* ic);
  // we use this recursively, and also in gpi guy
};

#endif // taiWidgetNameVarArrayChooser_h
