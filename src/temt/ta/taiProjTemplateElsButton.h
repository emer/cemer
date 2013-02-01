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

#ifndef taiProjTemplateElsButton_h
#define taiProjTemplateElsButton_h 1

// parent includes:
#include <taiListElsButton>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiProjTemplateElsButton : public taiListElsButton {
// for prog lib items
INHERITED(taiListElsButton)
public:
  override int          columnCount(int view) const;
  override const String headerText(int index, int view) const;
  override const String titleText();

  taiProjTemplateElsButton(TypeDef* typ, IWidgetHost* host, taiData* par,
                      QWidget* gui_parent_, int flags_ = 0); //note: typ is type of list
protected:
  override void         BuildCategories_impl();
  override int          BuildChooser_0(taiItemChooser* ic, taList_impl* top_lst,
                                       QTreeWidgetItem* top_item);
};

#endif // taiProjTemplateElsButton_h
