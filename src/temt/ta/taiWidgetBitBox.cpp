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

#include "taiWidgetBitBox.h"
#include <EnumDef>

#include <taMisc>
#include <taiMisc>

#include <QHBoxLayout>


/* IMPORTANT NOTE:
  taiWidgetBitBox is designed so that it preserves the values of NO_SHOW or
  otherwise hidden bits -- the m_val value is copied directly in GetImage,
  without any masking, and only bits that are exposed can be altered; the
  GetValue then returns the modified original
*/

iBitCheckBox::iBitCheckBox(bool auto_apply_, int val_, String label,
QWidget * parent)
:inherited(label.chars(), parent)
{
  auto_apply = auto_apply_;
  val = val_;
  connect(this, SIGNAL(clicked(bool)), this, SLOT(this_clicked(bool)) );
}

void iBitCheckBox::this_clicked(bool on)
{
  emit clickedEx(this, on);
}

taiWidgetBitBox::taiWidgetBitBox(TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
:taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
}

taiWidgetBitBox::taiWidgetBitBox(bool is_enum, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_)
:taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
  if (is_enum && typ) {
    SetEnumType(typ, true);
  }
}

void taiWidgetBitBox::SetEnumType(TypeDef* enum_typ, bool force) {
  if ((typ != enum_typ) || force) {
    Clear();
    typ = enum_typ;
    for (int i = 0; i < typ->enum_vals.size; ++i) {
      EnumDef* ed = typ->enum_vals.FastEl(i);
      if (ed->HasOption("NO_BIT") || ed->HasOption("IGNORE") ||
          ed->HasOption("NO_SHOW"))
        continue;
      // auto apply if entire guy marked, or if item is marked
      bool auto_apply = ((flags() & taiWidget::flgAutoApply)
                         || (ed->HasOption(TypeItem::opt_APPLY_IMMED)));
      AddBoolItem(auto_apply, ed->GetLabel(), ed->enum_no, ed->desc,
                  ed->HasOption("READ_ONLY"));
    }
    lay->addStretch();
  }
}

void taiWidgetBitBox::Initialize(QWidget* gui_parent_) {
  no_show = 0;
  no_edit = 0;
  m_par_obj_base = NULL;
  SetRep(MakeLayoutWidget(gui_parent_));
  lay = new QHBoxLayout(m_rep);
  lay->setMargin(0); // in Qt4 it adds style-dependent defaults
}

void taiWidgetBitBox::bitCheck_clicked(iBitCheckBox* sender, bool on) {
  if (on) m_val |= sender->val;
  else    m_val &= ~(sender->val);
  if (sender->auto_apply)
    applyNow();
  else
    SigEmit();
}

void taiWidgetBitBox::Clear() {
//   QLayoutItem *child;
//   while ((child = lay->takeAt(0)) != 0) {
//     delete child;
//   }
  taiMisc::DeleteChildrenNow(m_rep);
  lay = new QHBoxLayout(m_rep);
  lay->setMargin(0); // in Qt4 it adds style-dependent defaults
}

void taiWidgetBitBox::AddBoolItem(bool auto_apply, String name, int val,
  const String& desc, bool bit_ro)
{
  iBitCheckBox* bcb = new iBitCheckBox(auto_apply, val, name, m_rep);
  if (desc.nonempty()) {
    bcb->setToolTip(desc);
  }
  if(lay->count() > 0)
    lay->addSpacing(taiM->hsep_c);
  lay->addWidget(bcb);
  if (readOnly() || bit_ro) {
    bcb->setReadOnly(true);
  } else {
    QObject::connect(bcb, SIGNAL(clickedEx(iBitCheckBox*, bool)),
      this, SLOT(bitCheck_clicked(iBitCheckBox*, bool) ) );
  }
}

void taiWidgetBitBox::GetImage(int val) {
  QObject *obj;
  foreach (obj, m_rep->children() ) {
    iBitCheckBox* bcb = dynamic_cast<iBitCheckBox*>(obj);
    if (bcb) {
      // CONDSHOW
      bcb->setVisible(!(no_show & bcb->val));
      // CONDEDIT
      if (!readOnly())
        bcb->setEnabled(!(no_edit & bcb->val));
      // value
      bcb->setChecked((val & bcb->val)); //note: prob raises signal -- ok
    }
  }
  m_val = val;
}

void taiWidgetBitBox::GetValue(int& val) const {
  val = m_val;
}
