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

#include "taiEditorOfTypeItem.h"
#include <MemberDefBase>
#include <EnumDef>
#include <MemberDef>
#include <MethodDef>
#include <iLineEdit>
#include <iCheckBox>
#include <iSpinBox>

taiEditorOfTypeItem::taiEditorOfTypeItem(TypeItem* ti_, TypeItem::TypeInfoKinds tik_, 
  bool read_only_, bool modal_, QObject* parent)
:inherited(NULL, NULL, read_only, modal_, parent)
{
  ti = ti_;
  tik = tik_;
}

void taiEditorOfTypeItem::Constr_Data_Labels() {
//  taiData* dat;
  int row = 0; // makes following more generic
  
  // name
//  mb_dat = md->im->GetDataRep(this, NULL, body);
//  data_el->Add(mb_dat);
//    rep = mb_dat->GetRep();
  iCheckBox* chk = NULL;
  iLineEdit* rep = new iLineEdit(ti->name, body);
  rep->setReadOnly(true);
  AddNameData(row++, "name", "name of the type item", rep, NULL, NULL, true);
  
  // description
  rep = new iLineEdit(ti->desc, body);
  rep->setReadOnly(true);
  AddNameData(row++, "description", "description of the type item", rep,
    NULL, NULL, true);
  
  // opts
//  String tmp = taMisc::StrArrayToChar(ti->opts);//ti->opts.AsString();
  String tmp = ti->opts.AsString();
  rep = new iLineEdit(tmp, body);
  rep->setReadOnly(true);
  AddNameData(row++, "options", "ta # options, including inherited",
    rep, NULL, NULL, true);
  
  // stuff shared for Members/Props
  MemberDefBase* md = dynamic_cast<MemberDefBase*>(ti);
  if (md) {
    // type
    rep = new iLineEdit(md->type->Get_C_Name(), body);
    rep->setReadOnly(true);
    AddNameData(row++, "type", "type of the member/property",
      rep, NULL, NULL, true);
    // static
    chk = new iCheckBox(md->is_static, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_static", "static (non-instance) member/property",
     chk, NULL, NULL, true);
    // read-only
    chk = new iCheckBox(md->isReadOnly(), body);
    chk->setReadOnly(true);
    AddNameData(row++, "readOnly", "member/property is read-only (including in CSS/programs)", chk, NULL, NULL, true);
    // gui read-only
    chk = new iCheckBox(md->isGuiReadOnly(), body);
    chk->setReadOnly(true);
    AddNameData(row++, "guiReadOnly", "member/property is read-only in the gui (but may be writable in CSS/programs)", chk, NULL, NULL, true);
  }
  
  switch (tik) {
  case TypeItem::TIK_ENUM: {
    EnumDef* ed = static_cast<EnumDef*>(ti);
    // value
    rep = new iLineEdit(String(ed->enum_no), body);
    rep->setReadOnly(true);
    AddNameData(row++, "value", "value of the enum", rep, NULL, NULL, true);
    break;
    }
  case TypeItem::TIK_MEMBER:  {
    MemberDef* md = static_cast<MemberDef*>(ti);
    //nothing specific
    // size
    iSpinBox* repi = new iSpinBox(body);
    repi->setValue((ta_intptr_t)md->GetOff(NULL));
    repi->setReadOnly(true);
    AddNameData(row++, "offset", "offset, in bytes, of the member", 
      repi, NULL, NULL, true);
    break;
    }
  case TypeItem::TIK_PROPERTY:  {
//     PropertyDef* pd = static_cast<PropertyDef*>(ti);
    // for properties only, indicate (so can distinguish from members in .properties)
    chk = new iCheckBox(true, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_property", "this is a property (not a member)",
      chk, NULL, NULL, true);
    break;
    }
  case TypeItem::TIK_METHOD:  {
    MethodDef* md = static_cast<MethodDef*>(ti);
    // return type
    rep = new iLineEdit(md->type->Get_C_Name(), body);
    rep->setReadOnly(true);
    AddNameData(row++, "return type", "return type of the method",
      rep, NULL, NULL, true);
    
    // params
    rep = new iLineEdit(md->ParamsAsString(), body);
    rep->setReadOnly(true);
    AddNameData(row++, "params", "params of the method", 
      rep, NULL, NULL, true);
    
    chk = new iCheckBox(md->is_static, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_static", "static (non-instance) method",
      chk, NULL, NULL, true);
    
    chk = new iCheckBox(md->is_virtual, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_virtual", "virtual (overridable) method", 
      chk, NULL, NULL, true);
    
    chk = new iCheckBox(md->is_override, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_override", "virtual override of a base method", 
      chk, NULL, NULL, true);
    break;
    }

  case TypeItem::TIK_TYPE:  {
    TypeDef* td = static_cast<TypeDef*>(ti);
    // size
    iSpinBox* repi = new iSpinBox(body);
    repi->setValue(td->size);
    repi->setReadOnly(true);
    AddNameData(row++, "size", "size, in bytes, of the type",
      repi, NULL, NULL, true);
    
    // plugin, if any
    String plg;
    if (td->plugin) {
      plg = td->plugin->name;
    }
    rep = new iLineEdit(plg, body);
    rep->setReadOnly(true);
    AddNameData(row++, "plugin class", "the classname of the IPlugin for the plugin in which this type was defined, if any",
      rep, NULL, NULL, true);
    
    // parents
    String pars;
    for (int i = 0; i < td->parents.size; ++i) {
      if (i > 0) pars.cat(", ");
      pars.cat(td->parents.FastEl(i)->name);
    }
    rep = new iLineEdit(pars, body);
    rep->setReadOnly(true);
    AddNameData(row++, "parents", "parent type(s)",
      rep, NULL, NULL, true);
    
    break;
    }
  default: break; // compiler food
  }
}

