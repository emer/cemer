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

#ifndef taiWidgetDelegateControlPanel_h
#define taiWidgetDelegateControlPanel_h 1

// parent includes:
#include <taiWidgetDelegate>

// member includes:

// declare all other types mentioned but not required to include:
class taiEditorOfControlPanelFast;
class ControlPanel;
class EditMbrItem;


class TA_API taiWidgetDelegateControlPanel: public taiWidgetDelegate {
INHERITED(taiWidgetDelegate)
Q_OBJECT
public:
  ControlPanel*           sele;
  taiEditorOfControlPanelFast* sedh;

  void         GetImage() const override; // callable from edh any time
  void         GetValue() const override; // callable from edh

  bool         IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const override;

  QWidget* createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const override;

  taiWidgetDelegateControlPanel(ControlPanel* sele_, taiEditorOfControlPanelFast* sedh_);

 protected:
  mutable EditMbrItem*   emi;
  mutable QPointer<taiWidget> ps_dat; // most recently created
  mutable QPointer<QWidget> ps_rep; // most recently created
};

#endif // taiWidgetDelegateControlPanel_h
