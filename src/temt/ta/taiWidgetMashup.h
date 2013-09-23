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

#ifndef taiWidgetMashup_h
#define taiWidgetMashup_h 1

// parent includes:
#include <taiWidgetComposite>

// member includes:
#include <taBase_PtrList>

// declare all other types mentioned but not required to include:

class TA_API taiWidgetMashup : public taiWidgetComposite {
  // like taiWidgetPoly but each member has its own base
  Q_OBJECT
  INHERITED(taiWidgetComposite)
public:
  static taiWidgetMashup*   New(bool add_members, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
    QWidget* gui_parent_, int flags = 0); // set add_members false to manually add members, otherwise all eligible typ members added
  static bool           ShowMemberStat(MemberDef* md, int show); // primarily for ProgEdit so we can use the exact same algorithm

  Member_List           memb_el;        // member elements (1:1 with widget_el)
  taBase_PtrList        memb_bases;     // the bases in 1:1 with widget_el
  int                   show;

  inline QWidget*       rep() const { return (QWidget*)m_rep; }
  bool                  fillHor() {return true;} // override

  override void         AddChildMember(MemberDef* md);
  
  virtual void          AddBase(taBase* b);
  // call this if base is avail at time when adding a child member

  virtual void          SetBases(taBase* b1, taBase* b2=NULL, taBase* b3=NULL,
                                 taBase* b4=NULL, taBase* b5=NULL, taBase* b6=NULL);
  // convenience function for setting the memb_bases -- call this prior to GetImage and GetValue -- will reset bases first and then add these -- you can also do this manualy..

  ~taiWidgetMashup();

protected:
  void                  Constr(QWidget* gui_parent_);
  void                  AddTypeMembers(); // called to add all typ members
  override void         ChildRemove(taiWidget* child); // remove from memb_el too
  using inherited::GetImage_impl;
  override void         GetImage_impl(const void* base);
  override void         GetValue_impl(void* base) const;
  virtual bool          ShowMember(MemberDef* md) const;
  taiWidgetMashup(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags = 0);
};

#endif // taiWidgetMashup_h
