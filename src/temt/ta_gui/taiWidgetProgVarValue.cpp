// Copyright 2017-2017, Regents of the University of Colorado,
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

#include "taiWidgetProgVarValue.h"
#include <ProgVar>
#include <taiWidgetField>
#include <taiWidgetComboBox>
#include <taiWidgetFieldIncr>
#include <taiWidgetBitBox>
#include <taiMemberOfDynEnum>

#include <QHBoxLayout>

TypeInst_Of(DynEnum);           // TAI_DynEnum


taiWidgetProgVarValue::taiWidgetProgVarValue(TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_)
: taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  sub_widget = NULL;
  SetRep( MakeLayoutWidget(gui_parent_));
  if(!typ)
    typ = &TA_ProgVar;

}

void taiWidgetProgVarValue::GetSubWidget(ProgVar* pv) {
  typ = &TA_ProgVar;
  MemberDef* md = pv->GetValMemberDef();
  if(!md) return; // undefined!
  int flags_ = mflags;          // inherit get the flags we got
  if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY))
    flags_ |= taiWidget::flgReadOnly;
  if((pv->var_type == ProgVar::T_HardEnum) || (pv->var_type == ProgVar::T_DynEnum)) {
    if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) {
      sub_widget = new taiWidgetField(NULL, host, NULL, m_rep, flags_);
    }
    else if((pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type &&
             pv->hard_enum_type->HasOption("BITS")) ||
            (pv->dyn_enum_val.enum_type && pv->dyn_enum_val.enum_type->bits)) {
      sub_widget = new taiWidgetBitBox(NULL, host, NULL, m_rep, flags_ | taiWidget::flgAutoApply);
    }
    else {
      sub_widget = new taiWidgetComboBox(true, NULL, host, NULL, m_rep, flags_ | taiWidget::flgAutoApply);
    }
  }
  else if (pv->var_type == ProgVar::T_Int) {
    taiWidgetFieldIncr* int_rep = new taiWidgetFieldIncr(NULL, host, NULL, m_rep, flags_);
    int_rep->setMinimum(INT_MIN);
    sub_widget = int_rep;
  }
  else {
    sub_widget = md->im->GetWidgetRep(host, NULL, m_rep, NULL, flags_);
  }
  // we need to manually set the md into the dat...
  // need to check for enums, because md is the type, not the val
  if (pv->var_type == ProgVar::T_HardEnum)
    md = pv->FindMemberName("int_val");
  else if (pv->var_type == ProgVar::T_DynEnum) {
    // special case -- we will be setting the base to the DynEnum, not pv
    // and herein need to set the md for the nested dyn_val, which
    // conceivably may not even exist, so we do this via the instance
    md = TAI_DynEnum->FindMemberName("value");
  }
  sub_widget->SetMemberDef(md); // usually done by im, but we are manual here...
  SetMemberDef(md);

  QObject::connect(sub_widget, SIGNAL(settingHighlight(bool)),
                   this, SIGNAL(settingHighlight(bool)) );
  QObject::connect(sub_widget, SIGNAL(settingLighten(bool)),
                   this, SIGNAL(settingLighten(bool)) );

  QHBoxLayout* lay = new QHBoxLayout(m_rep);
  lay->setMargin(0); lay->setSpacing(0);
  lay->addWidget(sub_widget->GetRep());
}


void taiWidgetProgVarValue::GetImageProgVar(ProgVar* pv) {
  if(!sub_widget) {
    GetSubWidget(pv);
  }
  MemberDef* md = pv->GetValMemberDef();
  if(!md || !sub_widget) return;
  sub_widget->SetBase(pv); // for all, except HardEnum which is nested again
  SetBase(pv);
  if(pv->var_type == ProgVar::T_HardEnum) {
    if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) {
      taiWidgetField* tmp_widget = dynamic_cast<taiWidgetField*>(sub_widget);
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetField, not: ",
                             sub_widget->metaObject()->className())) return;
      taiMember::SetHighlights(md, typ, tmp_widget, pv);
      tmp_widget->GetImage(pv->GenCssInitVal());
    }
    else if(pv->hard_enum_type && pv->hard_enum_type->HasOption("BITS")) {
      taiWidgetBitBox* tmp_widget = dynamic_cast<taiWidgetBitBox*>(sub_widget);
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetBitBox, not: ",
                             sub_widget->metaObject()->className())) return;
      taiMember::SetHighlights(md, typ, tmp_widget, pv);
      tmp_widget->SetEnumType(pv->hard_enum_type);
      tmp_widget->GetImage(pv->int_val);
    }
    else {
      taiWidgetComboBox* tmp_widget = dynamic_cast<taiWidgetComboBox*>(sub_widget);
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetComboBox, not: ",
                             sub_widget->metaObject()->className())) return;
      taiMember::SetHighlights(md, typ, tmp_widget, pv);
      tmp_widget->SetEnumType(pv->hard_enum_type);
      tmp_widget->GetEnumImage(pv->int_val);
    }
  }
  else if(pv->var_type == ProgVar::T_DynEnum) {
    sub_widget->SetBase(&pv->dyn_enum_val);
    SetBase(&pv->dyn_enum_val);
    if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) {
      taiWidgetField* tmp_widget = dynamic_cast<taiWidgetField*>(sub_widget);
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetField, not: ",
                             sub_widget->metaObject()->className())) return;
      taiMember::SetHighlights(md, typ, tmp_widget, pv);
      tmp_widget->GetImage(pv->GenCssInitVal());
    }
    else if(pv->dyn_enum_val.enum_type && pv->dyn_enum_val.enum_type->bits) {
      taiWidgetBitBox* tmp_widget = dynamic_cast<taiWidgetBitBox*>(sub_widget);
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetBitBox, not: ",
                             sub_widget->metaObject()->className())) return;
      taiMember::SetHighlights(md, typ, tmp_widget, pv);
      taiMemberOfDynEnum::UpdateDynEnumBits(tmp_widget, pv->dyn_enum_val);
    }
    else {
      taiWidgetComboBox* tmp_widget = dynamic_cast<taiWidgetComboBox*>(sub_widget);
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetComboBox, not: ",
                             sub_widget->metaObject()->className())) return;
      taiMember::SetHighlights(md, typ, tmp_widget, pv);
      taiMemberOfDynEnum::UpdateDynEnumCombo(tmp_widget, pv->dyn_enum_val);
    }
  }
  else if(pv->var_type == ProgVar::T_Int) { // todo: not supporting first_diff
    taiWidgetFieldIncr* tmp_widget = dynamic_cast<taiWidgetFieldIncr*>(sub_widget);
    if (pv->TestError_impl(!tmp_widget, "expected taiWidgetFieldIncr, not: ",
                           sub_widget->metaObject()->className())) return;
      taiMember::SetHighlights(md, typ, tmp_widget, pv);
    tmp_widget->GetImage(pv->int_val);
  }
  else {
    md->im->GetImage(sub_widget, (void*)pv);
  }
}

void taiWidgetProgVarValue::GetValueProgVar(ProgVar* pv) const {
  MemberDef* md = pv->GetValMemberDef();
  if(!md || !sub_widget) return;
  if(pv->var_type == ProgVar::T_HardEnum) {
    if(pv->hard_enum_type && pv->hard_enum_type->HasOption("BITS")) {
      taiWidgetBitBox* tmp_widget = dynamic_cast<taiWidgetBitBox*>(sub_widget);
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetBitBox, not: ",
                             sub_widget->metaObject()->className())) return;
      tmp_widget->GetValue(pv->int_val);
    }
    else {
      taiWidgetComboBox* tmp_widget = dynamic_cast<taiWidgetComboBox*>(sub_widget);
      //note: use of pv for tests is just a hook, pv not really germane
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetComboBox, not: ",
                             sub_widget->metaObject()->className())) return;
      tmp_widget->GetEnumValue(pv->int_val); // todo: not supporting first_diff
    }
  }
  else if(pv->var_type == ProgVar::T_DynEnum) { // todo: not supporting first_diff
    if(pv->dyn_enum_val.enum_type && pv->dyn_enum_val.enum_type->bits) {
      taiWidgetBitBox* tmp_widget = dynamic_cast<taiWidgetBitBox*>(sub_widget);
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetBitBox, not: ",
                             sub_widget->metaObject()->className())) return;
      tmp_widget->GetValue(pv->dyn_enum_val.value);
    }
    else {
      taiWidgetComboBox* tmp_widget = dynamic_cast<taiWidgetComboBox*>(sub_widget);
      if (pv->TestError_impl(!tmp_widget, "expected taiWidgetComboBox, not: ",
                             sub_widget->metaObject()->className())) return;
      tmp_widget->GetValue(pv->dyn_enum_val.value);
    }
  }
  else if(pv->var_type == ProgVar::T_Int) { // todo: not supporting first_diff
    taiWidgetFieldIncr* tmp_widget = dynamic_cast<taiWidgetFieldIncr*>(sub_widget);
    if (pv->TestError_impl(!tmp_widget, "expected taiWidgetFieldIncr, not: ",
                           sub_widget->metaObject()->className())) return;
    pv->int_val = tmp_widget->GetValue();
  }
  else {
    bool first_diff = true;
    md->im->GetMbrValue(sub_widget, (void*)pv, first_diff);
    // if (!first_diff)
    //   taiMember::EndScript(base);
  }
}


// void taiWidgetProgVarValue::this_GetEditActionsEnabled(int& ea) {
//   if(!rep()) return;
//   if (!readOnly())
//     ea |= iClipData::EA_PASTE;
//   if (rep()->hasSelectedText()) {
//     ea |= (iClipData::EA_COPY);
//     if (!readOnly())
//       ea |= (iClipData::EA_CUT |  iClipData::EA_DELETE);
//   }
// }

// void taiWidgetProgVarValue::this_EditAction(int ea) {
//   if(!rep()) return;
//   if (ea & iClipData::EA_CUT) {
//     rep()->cut();
//   } else if (ea & iClipData::EA_COPY) {
//     rep()->copy();
//   } else if (ea & iClipData::EA_PASTE) {
//     rep()->paste();
//   } else if (ea & iClipData::EA_DELETE) {
//     rep()->del(); //note: assumes we already qualified with hasSelectedText, otherwise it is a BS
//   }
// }

// void taiWidgetProgVarValue::this_SetActionsEnabled() {
//   //TODO: UNDO/REDO
// }

