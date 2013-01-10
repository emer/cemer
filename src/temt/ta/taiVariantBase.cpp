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

#include "taiVariantBase.h"

taiVariantBase::taiVariantBase(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
  : taiCompData(typ_, host_, par, gui_parent_, flags)
{
  //note: call Constr in your own class' constructor
}

taiVariantBase::~taiVariantBase() {
  data_el.Reset();
}

void taiVariantBase::Constr(QWidget* gui_parent_) {
  cmbVarType = NULL;
  fldVal = NULL;
  togVal = NULL;
  m_updating = 0;

  QWidget* rep_ = MakeLayoutWidget(gui_parent_);
  SetRep(rep_);
  if (host != NULL) {
    SET_PALETTE_BACKGROUND_COLOR(rep_, host->colorOfCurRow());
  }
  InitLayout();
  Constr_impl(gui_parent_, (mflags & flgReadOnly));
  EndLayout();
}

void taiVariantBase::Constr_impl(QWidget* gui_parent_, bool read_only_) {
  // type stuff
  QWidget* rep_ =  GetRep();
  QLabel* lbl = NULL;
  cmbVarType = NULL;
  if(!(mflags & flgFixedType)) {
    lbl = MakeLabel("var type", rep_);
    AddChildWidget(lbl, taiM->hsep_c);

    TypeDef* typ_var_enum = TA_Variant.sub_types.FindName("VarType");
    cmbVarType = new taiComboBox(true, typ_var_enum, host, this, rep_);
    //remove unused variant enum types according to flags
    if (mflags & (flgNoInvalid | flgIntOnly)) {
      cmbVarType->RemoveItemByData(QVariant(Variant::T_Invalid));
    }
    if (mflags & (flgNoAtomics | flgIntOnly)) {
      for (int vt = Variant::T_Atomic_Min; vt <= Variant::T_Atomic_Max; ++vt) {
        if (!((vt == Variant::T_Int) && (mflags & flgIntOnly)))
          cmbVarType->RemoveItemByData(QVariant(vt));
      }
    }
    if (mflags & (flgNoPtr | flgIntOnly)) {
      cmbVarType->RemoveItemByData(QVariant(Variant::T_Ptr));
    }
    if (mflags & (flgNoBase | flgIntOnly)) {
      cmbVarType->RemoveItemByData(QVariant(Variant::T_Matrix));
      cmbVarType->RemoveItemByData(QVariant(Variant::T_Base));
    }
    if (mflags & (flgNoTypeItem | flgIntOnly)) {
      cmbVarType->RemoveItemByData(QVariant(Variant::T_TypeItem));
    }
    AddChildWidget(cmbVarType->rep(), taiM->hsep_c);
    lbl->setBuddy(cmbVarType->rep());
    if (read_only_) {
      cmbVarType->rep()->setEnabled(false);
    } else {
      connect(cmbVarType, SIGNAL(itemChanged(int)), this, SLOT(cmbVarType_itemChanged(int)));
    }

    lbl = MakeLabel("var value", rep_);
    AddChildWidget(lbl, taiM->hsep_c);
  }

  stack = new QStackedWidget(rep_);
  AddChildWidget(stack); // fill rest of space
  if(lbl)
    lbl->setBuddy(stack);

  // created in order of StackControls
  lbl = MakeLabel("(no value for type Invalid)");
  stack->addWidget(lbl);
  togVal = new taiToggle(typ, host, this, NULL);
  stack->addWidget(togVal->rep());
  incVal = new taiIncrField(typ, host, this, NULL, mflags);
  incVal->setMinimum(INT_MIN); //note: must be int
  incVal->setMaximum(INT_MAX); //note: must be int
  stack->addWidget(incVal->rep());
  fldVal = new taiField(typ, host, this, NULL, mflags & ~flgEditDialog);
  stack->addWidget(fldVal->rep());
  lbl = MakeLabel("(Ptr cannot be set)");
  stack->addWidget(lbl);

  tabVal = new taiTokenPtrButton(&TA_taBase, host, this, NULL, flgEditDialog);
  stack->addWidget(tabVal->GetRep());

  tiVal = MakeLabel("(TypeItem cannot be set)");
  stack->addWidget(tiVal);
}

/*
bool taiVariantBase::ShowMember(MemberDef* md) {
  if (md->HasOption("HIDDEN_INLINE"))
    return false;
  else
    return md->ShowMember((taMisc::ShowMembs)show);
} */

void taiVariantBase::cmbVarType_itemChanged(int itm) {
  if (m_updating != 0) return;
  ++m_updating;
  int vt; //Variant::VarType
  // set combo box to right type
  cmbVarType->GetEnumValue(vt);
  switch (vt) {
  case Variant::T_Invalid:
    stack->setCurrentIndex(scInvalid);
    break;

  case Variant::T_Bool:
    stack->setCurrentIndex(scBool);
    break;

  case Variant::T_Int:
    stack->setCurrentIndex(scInt);
    break;

  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
  case Variant::T_Double:
  case Variant::T_Char:
  case Variant::T_String:
    stack->setCurrentIndex(scField);
    break;

  case Variant::T_Ptr:
    stack->setCurrentIndex(scPtr);
    break;
  case Variant::T_Base:
    stack->setCurrentIndex(scBase);
    tabVal->GetImage(NULL, &TA_taBase); // obj, no scope
    break;
  case Variant::T_Matrix:
    stack->setCurrentIndex(scBase);
    tabVal->GetImage(NULL, &TA_taMatrix); // obj, no scope
    break;
  case Variant::T_TypeItem:
    stack->setCurrentIndex(scTypeItem);
    break;
  default: return ;
  }
  --m_updating;
}

void taiVariantBase::GetImage_Variant(const Variant& var) {
  ++m_updating;
  // set combo box to right type
  if(cmbVarType)
    cmbVarType->GetEnumImage(var.type());

  switch (var.type()) {
  case Variant::T_Invalid:
    stack->setCurrentIndex(scInvalid);
    break;

  case Variant::T_Bool:
    stack->setCurrentIndex(scBool);
    togVal->GetImage(var.toBool());
    break;

  case Variant::T_Int:
    stack->setCurrentIndex(scInt);
    incVal->GetImage(var.toInt());
    break;

  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
  case Variant::T_Double:
  case Variant::T_Char:
  case Variant::T_String:
    stack->setCurrentIndex(scField);
    fldVal->GetImage(var.toString());
    break;

  case Variant::T_Ptr:
    stack->setCurrentIndex(scPtr);
    //TODO: need to set something!
    break;
  case Variant::T_Base:
    stack->setCurrentIndex(scBase);
    tabVal->GetImage(var.toBase(), &TA_taBase); // obj, no scope
    break;
  case Variant::T_Matrix:
    stack->setCurrentIndex(scBase);
    tabVal->GetImage(var.toMatrix(), &TA_taMatrix); // obj, no scope
    break;
  case Variant::T_TypeItem:
    stack->setCurrentIndex(scTypeItem);
    tiVal->setText(var.toString());
    break;
  default: return ;
  }
  --m_updating;
}

void taiVariantBase::GetValue_Variant(Variant& var) const {
  ++m_updating;
  // set combo box to right type
  if(cmbVarType) {
    int vt; //Variant::VarType
    cmbVarType->GetEnumValue(vt);
    var.setType((Variant::VarType)vt);
  }

  //note: the correct widget should be visible...

  switch (var.type()) {
  case Variant::T_Invalid:
    var = _nilVariant;
    break;

  case Variant::T_Bool:
    var.setBool(togVal->GetValue());
    break;

  case Variant::T_Int:
    var = incVal->GetValue();
    break;

  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
  case Variant::T_Double:
  case Variant::T_Char:
  case Variant::T_String:
    var.updateFromString(fldVal->GetValue());
    break;

  case Variant::T_Ptr:
    //nothing for now (will get set to NULL);
    break;
  case Variant::T_Base:
    var.setBase(tabVal->GetValue());
    break;
  case Variant::T_Matrix:
    var.setBase(tabVal->GetValue());
    break;
  case Variant::T_TypeItem:
    //nothing for now (will get set to NULL);
    break;
  default: break;
  }
  --m_updating;
  return;
}
