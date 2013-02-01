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

#include "taiWidgetComboBox.h"
#include <iComboBox>
#include <EnumDef>

#include <taMisc>
#include <taiMisc>



taiWidgetComboBox::taiWidgetComboBox(TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
:taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
}

taiWidgetComboBox::taiWidgetComboBox(bool is_enum, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_)
:taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_, is_enum);
  if (is_enum && typ) {
    SetEnumType(typ, true);
  }
}

void taiWidgetComboBox::Initialize(QWidget* gui_parent_, bool is_enum_) {
  m_is_enum = is_enum_;
  SetRep(new iComboBox(gui_parent_));
  rep()->setFixedHeight(taiM->combo_height(defSize()));

  //connect changed signal to our slot, or apply
  if (mflags & flgAutoApply)
    QObject::connect(m_rep, SIGNAL(activated(int) ),
          this, SLOT(applyNow() ) );
  else
    QObject::connect(m_rep, SIGNAL(activated(int) ),
          this, SLOT(repChanged() ) );
  // also to aux signal (used by non-IWidgetHost clients)
  QObject::connect(m_rep, SIGNAL(activated(int) ),
    this, SIGNAL(itemChanged(int)) );
}

void taiWidgetComboBox::AddItem(const String& val, const QVariant& usrData) {
  rep()->addItem(val, usrData);
}

void taiWidgetComboBox::Clear() {
  rep()->clear();
}

void taiWidgetComboBox::GetImage(int itm) {
    // set to this item number
  rep()->setCurrentIndex(itm);
}

void taiWidgetComboBox::GetValue(int& itm) const {
  itm = rep()->currentIndex();
}

void taiWidgetComboBox::GetEnumImage(int enum_val) {
  int i = rep()->findData(QVariant(enum_val));
  rep()->setCurrentIndex(i);
}

void taiWidgetComboBox::GetEnumValue(int& enum_val) const {
  int i = rep()->currentIndex();
  if (i >= 0) enum_val = rep()->itemData(i).toInt();
  else enum_val = 0; // perhaps the safest invalid choice...
}

void taiWidgetComboBox::RemoveItemByData(const QVariant& userData) {
  int i;
  while ((i = rep()->findData(userData)) != -1) {
    rep()->removeItem(i);
  }
}

void taiWidgetComboBox::RemoveItemByText(const String& val) {
  int i;
  while ((i = rep()->findText(val)) != -1) {
    rep()->removeItem(i);
  }
}

void taiWidgetComboBox::SetEnumType(TypeDef* enum_typ, bool force) {
  if ((typ != enum_typ) || force) {
    Clear();
    typ = enum_typ;
    for (int i = 0; i < typ->enum_vals.size; ++i) {
      EnumDef* ed = typ->enum_vals.FastEl(i);
      // sometimes we have aliases, or enums that are subbits in a proper enum
      // so we want to hide those
      if (ed->HasOption("NO_SHOW") || ed->HasOption("IGNORE"))
        continue;
      AddItem(ed->GetLabel(), QVariant(ed->enum_no));
    }
  }
}
