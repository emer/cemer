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

#ifndef taiWidgetEnumTypeDefChooser_h
#define taiWidgetEnumTypeDefChooser_h 1

// parent includes:
#include <taiWidgetTypeDefChooser>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiWidgetEnumTypeDefChooser : public taiWidgetTypeDefChooser {
//
INHERITED(taiWidgetTypeDefChooser)
public:
  int                   columnCount(int view) const; // override
  const String          headerText(int index, int view) const; // override

  void                  BuildChooser(iDialogItemChooser* ic, int view = 0); // override
  void         btnHelp_clicked() CPP11_OVERRIDE;

  taiWidgetEnumTypeDefChooser(TypeDef* typ_, IWidgetHost* host,
                       taiWidget* par, QWidget* gui_parent_, int flags_ = 0,
                       const String& flt_start_txt = "");
protected:
  const String          itemTag() const {return "Enum Type: ";}

  bool                  AddType_Enum(TypeDef* typ_, TypeDef* par_typ); // true if should be shown to user
  int                   BuildChooser_0(iDialogItemChooser* ic, TypeDef* top_typ,
    QTreeWidgetItem* top_item); // we use this recursively
};

#endif // taiWidgetEnumTypeDefChooser_h
