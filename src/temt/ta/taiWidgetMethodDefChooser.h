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

#ifndef taiWidgetMethodDefChooser_h
#define taiWidgetMethodDefChooser_h 1

// parent includes:
#include <taiWidgetItemChooser>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiWidgetMethodDefChooser : public taiWidgetItemChooser {
  // for MethodDefs
  INHERITED(taiWidgetItemChooser)
public:
  inline MethodDef*     md() const {return (MethodDef*)m_sel;}
  int                   columnCount(int view) const; // override
  const String          headerText(int index, int view) const; // override
  int                   viewCount() const {return 3;} // override
  const String          viewText(int index) const; // override

  using inherited::GetImage;
  void                  GetImage(MethodDef* cur_sel, TypeDef* targ_typ)
    {taiWidgetItemChooser::GetImage((void*)cur_sel, targ_typ);}
  MethodDef*            GetValue() {return md();}

  void                  BuildChooser(iDialogItemChooser* ic, int view = 0); // override
  override void         btnHelp_clicked();

  taiWidgetMethodDefChooser(TypeDef* typ_, IWidgetHost* host,
                     taiWidget* par, QWidget* gui_parent_, int flags_ = 0,
                     const String& flt_start_txt = "");
protected:
  static const int      tool_tip_wrap_length = 70;

  const String          itemTag() const {return "Method: ";}
  const String          labelNameNonNull() const;

  override void         BuildCategories_impl();
  void                  BuildChooser_0(iDialogItemChooser* ic);
  int                   BuildChooser_1(iDialogItemChooser* ic, TypeDef* top_typ,
    QTreeWidgetItem* top_item); // we use this recursively
  void                  BuildChooser_2(iDialogItemChooser* ic);

  virtual bool          ShowMethod(MethodDef* mth);
};

#endif // taiWidgetMethodDefChooser_h
