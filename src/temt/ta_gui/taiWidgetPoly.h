// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef taiWidgetPoly_h
#define taiWidgetPoly_h 1

// parent includes:
#include <taiWidgetComposite>

// member includes:

// declare all other types mentioned but not required to include:

// this class supports the use of hierarchical sub-data within a data item
// its default behavior is to put everything in an hbox with labels

class TA_API taiWidgetPoly : public taiWidgetComposite {
  Q_OBJECT
  INHERITED(taiWidgetComposite)
public:
  static taiWidgetPoly*   New(bool add_members, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
    QWidget* gui_parent_, int flags = 0); // set add_members false to manually add members, otherwise all eligible typ members added
  Member_List           memb_el;        // member elements (1:1 with widget_el)

  inline QWidget*       rep() const { return (QWidget*)m_rep; }
  bool                  fillHor() override {return true;}

  void         AddChildMember(MemberDef* md, int column = 0, bool mbr_type_only = false) override;

  ~taiWidgetPoly();

protected:
  void         Constr(QWidget* gui_parent_);
  void         AddTypeMembers(); // called to add all typ members
  void         ChildRemove(taiWidget* child) override; // remove from memb_el too
  using inherited::GetImage_impl;
  void         GetImage_impl(const void* base) override;
  void         GetValue_impl(void* base) const override;
  taiWidgetPoly(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags = 0);
};


#endif // taiWidgetPoly_h
