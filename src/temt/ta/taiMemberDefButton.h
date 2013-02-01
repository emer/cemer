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

#ifndef taiMemberDefButton_h
#define taiMemberDefButton_h 1

// parent includes:
#include <taiItemPtrBase>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiMemberDefButton : public taiItemPtrBase {
  // for MemberDefs
  INHERITED(taiItemPtrBase)
public:
  inline MemberDef*     md() const {return (MemberDef*)m_sel;}
  int                   columnCount(int view) const; // override
  const String          headerText(int index, int view) const; // override
  int                   viewCount() const {return 2;} // override
  const String          viewText(int index) const; // override

  USING(inherited::GetImage)
  void                  GetImage(MemberDef* cur_sel, TypeDef* targ_typ)
    {taiItemPtrBase::GetImage((void*)cur_sel, targ_typ);}
  MemberDef*            GetValue() {return md();}

  override void         BuildChooser(taiItemChooser* ic, int view = 0); //

  virtual bool          ShowMember(MemberDef* mbr);
  override void         btnHelp_clicked();

  taiMemberDefButton(TypeDef* typ_, IWidgetHost* host,
                     taiData* par, QWidget* gui_parent_, int flags_ = 0,
                     const String& flt_start_txt = "");
protected:
  const String          itemTag() const {return "Member: ";}
  const String          labelNameNonNull() const;

  override void         BuildCategories_impl();
  void                  BuildChooser_0(taiItemChooser* ic);
  void                  BuildChooser_1(taiItemChooser* ic);
};



#endif // taiMemberDefButton_h
