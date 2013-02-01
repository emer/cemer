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

#ifndef taiPolyData_h
#define taiPolyData_h 1

// parent includes:
#include <taiCompData>

// member includes:

// declare all other types mentioned but not required to include:

// this class supports the use of hierarchical sub-data within a data item
// its default behavior is to put everything in an hbox with labels

class TA_API taiPolyData : public taiCompData {
  Q_OBJECT
  INHERITED(taiCompData)
public:
  static taiPolyData*   New(bool add_members, TypeDef* typ_, IWidgetHost* host, taiData* par,
    QWidget* gui_parent_, int flags = 0); // set add_members false to manually add members, otherwise all eligible typ members added
  static bool           ShowMemberStat(MemberDef* md, int show); // primarily for ProgEdit so we can use the exact same algorithm

  Member_List           memb_el;        // member elements (1:1 with data_el)
  int                   show;

  inline QWidget*       rep() const { return (QWidget*)m_rep; }
  bool                  fillHor() {return true;} // override

  override void         AddChildMember(MemberDef* md);

  ~taiPolyData();

protected:
  void                  Constr(QWidget* gui_parent_);
  void                  AddTypeMembers(); // called to add all typ members
  override void         ChildRemove(taiData* child); // remove from memb_el too
  USING(inherited::GetImage_impl)
  override void         GetImage_impl(const void* base);
  override void         GetValue_impl(void* base) const;
  virtual bool          ShowMember(MemberDef* md) const;
  taiPolyData(TypeDef* typ_, IWidgetHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
};


#endif // taiPolyData_h
