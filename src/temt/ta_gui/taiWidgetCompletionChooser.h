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

#ifndef taiWidgetCompletionChooser_h
#define taiWidgetCompletionChooser_h 1

// parent includes:
#include <taiWidgetItemChooser>

// member includes:

// declare all other types mentioned but not required to include:
class completions;

class TA_API taiWidgetCompletionChooser : public taiWidgetItemChooser {
  // for items appropriate for selecting an expression completion
  
  INHERITED(taiWidgetItemChooser)
public:
  taList_impl*          list;
  iDialogItemChooser*   dialog_item_chooser;
  Completions*          completions; // the items to display

  inline TypeItem*      GetItemType() const {return (TypeItem*)m_sel;}
  inline taBase*        item() const {return (taBase*)m_sel;}
  int                   columnCount(int view) const override;
  const String          headerText(int index, int view) const override;
  int                   viewCount() const override {return 1;}
  const String          viewText(int index) const override;
  String                GetSelectionText(); // return the selected string
  void                  BuildChooser(iDialogItemChooser* item_chooser, int view = 0) override;
  void                  SetCompletions(Completions* completions_info);
  taBase*               GetValue() {return item();}

  using inherited::GetImage;
  void                  GetImage(taList_impl* base_lst, taBase* it);

  taiWidgetCompletionChooser(TypeDef* typ_, IWidgetHost* host,
                              taiWidget* par, QWidget* gui_parent_, int flags_ = 0,
                              const String& flt_start_txt = "", int button_width = -1);
protected:
  const String          labelNameNonNull() const override;
  virtual int           Populate(iDialogItemChooser* item_chooser, Completions* the_completions, QTreeWidgetItem* top_item);

};

#endif // taiWidgetCompletionChooser_h
