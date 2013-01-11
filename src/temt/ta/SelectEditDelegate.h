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

#ifndef SelectEditDelegate_h
#define SelectEditDelegate_h 1

// parent includes:
#include <taiDataDelegate>

// member includes:

// declare all other types mentioned but not required to include:
class iSelectEDitDataHost2;

class TA_API SelectEditDelegate: public taiDataDelegate {
INHERITED(taiDataDelegate)
Q_OBJECT
public:
  SelectEdit*           sele;
  iSelectEditDataHost2* sedh;

  override void         GetImage() const; // callable from edh any time
  override void         GetValue() const; // callable from edh

  override bool         IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const;

  override QWidget* createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const;

  SelectEditDelegate(SelectEdit* sele_, iSelectEditDataHost2* sedh_);

 protected:
  mutable EditMbrItem*   emi;
  mutable QPointer<taiData> ps_dat; // most recently created
  mutable QPointer<QWidget> ps_rep; // most recently created
};

#endif // SelectEditDelegate_h
