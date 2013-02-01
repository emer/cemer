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

#ifndef taiWidgetMemberMethodDefChooser_h
#define taiWidgetMemberMethodDefChooser_h 1

// parent includes:
#include <taiWidgetItemChooser>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiWidgetMemberMethodDefChooser : public taiWidgetItemChooser {
  // for MemberDefs AND MethodDefs -- useful for path completion lookup for example
  INHERITED(taiWidgetItemChooser)
public:
  inline TypeItem*      md() const {return (TypeItem*)m_sel;}

  int                   columnCount(int view) const; // override
  const String          headerText(int index, int view) const; // override
  int                   viewCount() const {return 4;} // override
  const String          viewText(int index) const; // override

  USING(inherited::GetImage)
  void                  GetImage(MemberDef* cur_sel, TypeDef* targ_typ)
    {taiWidgetItemChooser::GetImage((void*)cur_sel, targ_typ);}
  TypeItem*             GetValue() {return md();}

  override void         BuildChooser(iDialogItemChooser* ic, int view = 0); //

  virtual bool          ShowMember(MemberDef* mbr);
  virtual bool          ShowMethod(MethodDef* mth);
  override void         btnHelp_clicked();

  taiWidgetMemberMethodDefChooser(TypeDef* typ_, IWidgetHost* host,
                           taiWidget* par, QWidget* gui_parent_, int flags_ = 0,
                           const String& flt_start_txt = "");
protected:
  const String          itemTag() const {return "Member/Method: ";}
  const String          labelNameNonNull() const;

  override void         BuildCategories_impl();
  void                  BuildChooser_0(iDialogItemChooser* ic); // all
  void                  BuildChooser_1(iDialogItemChooser* ic); // just mbr
  void                  BuildChooser_2(iDialogItemChooser* ic); // just mth
  void                  BuildChooser_3(iDialogItemChooser* ic); // expert
};

#endif // taiWidgetMemberMethodDefChooser_h
