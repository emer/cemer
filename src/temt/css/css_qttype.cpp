// Copyright, 1995-2007, Regents of the University of Colorado,
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



#include "css_qttype.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "css_qtdata.h"
#include "css_qtdialog.h"

#include <taiTypeOfTokenPtr>
#include <taiWidgetField>
#include <taiWidgetComboBox>
#include <taiWidgetMenuButton>
#include <BuiltinTypeDefs>

#include <taiMisc>

//#include <stdlib.h>

//////////////////////////////////////////////////
//              cssiType                        //
//////////////////////////////////////////////////

cssiType_QObj::cssiType_QObj(cssiType* owner_)
:QObject()
{
  owner = owner_;
}

cssiType_QObj::~cssiType_QObj()
{
  if (owner) owner->qobj = NULL;
}

void cssiType_QObj::CallEdit() {
  if (owner) owner->CallEdit();
}



cssiType::cssiType(cssEl* orgo, TypeDef* tp, void* bs, bool use_ptr_type) : taiType(tp) {
  orig_obj = orgo;
  cur_base = bs;
  use_it = NULL;
  if (use_ptr_type) {
    use_it = new taiTypeOfTokenPtr(tp);
    use_it->no_setpointer = true; // don't set pointers for css pointers!
  }
  qobj = NULL;
}
cssiType::~cssiType() {
  if (use_it)
    delete use_it;
  if (qobj) {
    delete qobj;
    qobj = NULL;
  }
}

void  cssiType::Assert_QObj() {
  if (!qobj) {
    qobj = new cssiType_QObj(this);
  }
}

taiWidget* cssiType::GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                              taiType*, int, MemberDef*)
{
  if (use_it)
    return use_it->GetWidgetRep(host_, par, gui_parent_);
  else
    return typ->it->GetWidgetRep(host_, par, gui_parent_);
}

void cssiType::GetImage(taiWidget* dat, const void* base) {
  if (use_it)
    use_it->GetImage(dat, base);
  else
    typ->it->GetImage(dat, base);
}

void cssiType::GetValue(taiWidget* dat, void* base) {
  if (use_it)
    use_it->GetValue(dat, base);
  else
    typ->it->GetValue(dat, base);
}

//TODO: update to use the ReadOnly approach used by ta_qtdata (using handlesReadOnly aware types)

//////////////////////////////////////////////////
//              cssiROType                      //
//////////////////////////////////////////////////

cssiROType::cssiROType(cssEl* orgo, TypeDef* tp, void* bs, bool use_ptr_type)
  : cssiType(orgo, tp, bs, use_ptr_type)
{
}

taiWidget* cssiROType::GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                taiType*, int, MemberDef*)
{
  taiWidgetField* rval = new taiWidgetField(typ, host_, par, gui_parent_, true);
  return rval;
}

void cssiROType::GetImage(taiWidget* dat, const void* base) {
  taiWidgetField* rval = (taiWidgetField*)dat;
  String strval = typ->GetValStr(base);
  rval->GetImage(strval);
}

//////////////////////////////////////////////////
//              cssiEnumType                    //
//////////////////////////////////////////////////

cssiEnumType::cssiEnumType(cssEl* orgo, cssEnumType* enum_typ, void* bs)
  : cssiType(orgo, &TA_int, bs, false) // note: was TA_enum
{
  enum_type = enum_typ;
}

taiWidget* cssiEnumType::GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                  taiType*, int, MemberDef*)
{
  taiWidgetComboBox* rval = new taiWidgetComboBox(typ, host_, par, gui_parent_);
  for (int i = 0; i < enum_type->enums->size; ++i) {
    rval->AddItem(enum_type->enums->FastEl(i)->name);
  }
  return rval;
}

void cssiEnumType::GetImage(taiWidget* dat, const void* base) {
  taiWidgetComboBox* rval = (taiWidgetComboBox*)dat;
  int enm_val = *((int*)base);
  for (int i = 0; i < enum_type->enums->size; ++i) {
    if (((cssInt*)enum_type->enums->FastEl(i))->val == enm_val) {
      rval->GetImage(i);
      break;
    }
  }
}

void cssiEnumType::GetValue(taiWidget* dat, void* base) {
  taiWidgetComboBox* rval = (taiWidgetComboBox*)dat;
  int itm_no = -1;
  rval->GetValue(itm_no);

  if (itm_no >= 0)  {
    cssEl* itm = enum_type->enums->El(itm_no);
    if (itm != &cssMisc::Void)
      *((int*)base) = (int)*itm;
  }
}

//////////////////////////////////////////////////
//              cssiClassType                  //
//////////////////////////////////////////////////

cssiClassType::cssiClassType(cssEl* orgo, void* bs)
  : cssiType(orgo, &TA_void, bs, false)
{
}

taiWidget* cssiClassType::GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType*, int, MemberDef*)
{
  cssClassInst* obj = (cssClassInst*) cur_base;
  if ((obj->type_def != NULL) && (obj->type_def->HasOption("INLINE")
        || obj->type_def->HasOption("EDIT_INLINE")))
  {
    cssiPolyData* rval = new cssiPolyData(obj, typ, host_, par, gui_parent_);
    return rval;
  }
  else {
    taiWidgetMenuButton* rval = new taiWidgetMenuButton
      (taiWidgetMenu::normal_update, taiMisc::fonSmall, typ, host_, par, gui_parent_);
    Assert_QObj();
    rval->AddItem("Edit", taiWidgetMenu::use_default, iAction::action,
        qobj, SLOT(CallEdit()), (void*)NULL);
    String lbl = String(obj->GetTypeName()) + ": Actions";
    rval->setLabel(lbl);
    return rval;
  }
}

void cssiClassType::GetImage(taiWidget* dat, const void* base) {
  cssClassInst* obj = (cssClassInst*) cur_base;
  if((obj->type_def != NULL) && (obj->type_def->HasOption("INLINE")
                                 || obj->type_def->HasOption("EDIT_INLINE"))) {
    cssiPolyData* rval = (cssiPolyData*)dat;
    rval->GetImage_(base);
  }
}

void cssiClassType::GetValue(taiWidget* dat, void* base) {
  cssClassInst* obj = (cssClassInst*) cur_base;
  if ((obj->type_def != NULL) && (obj->type_def->HasOption("INLINE")
         || obj->type_def->HasOption("EDIT_INLINE")))
  {
    cssiPolyData* rval = (cssiPolyData*)dat;
    rval->GetValue_(base);
  }
}

void cssiClassType::CallEdit() {
  cssClassInst* obj = (cssClassInst*) cur_base;
  obj->Edit();
}


//////////////////////////////////////////////////
//              cssiArrayType                  //
//////////////////////////////////////////////////

cssiArrayType::cssiArrayType(cssEl* orgo, void* bs)
  : cssiType(orgo, &TA_void, bs, false)
{
}

taiWidget* cssiArrayType::GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType*, int, MemberDef*)
{
  taiWidgetMenuButton* rval = new taiWidgetMenuButton
    (taiWidgetMenu::normal_update, taiMisc::fonSmall, typ, host_, par, gui_parent_);
  Assert_QObj();
  rval->AddItem("Edit", taiWidgetMenu::use_default, iAction::action,
        qobj, SLOT(CallEdit), (void*)NULL);
  cssArray* obj = (cssArray*) cur_base;
  String lbl = String(obj->GetTypeName()) + ": Actions";
  rval->setLabel(lbl);
  return rval;
}

void cssiArrayType::CallEdit() {
  cssArray* obj = (cssArray*) cur_base;

  cssClassInst* arg_obj = new cssClassInst("Select element of array to edit");
  arg_obj->name = "Select element: (0-" + String((int)obj->items->size-1) + ")";
  cssInt* eldx = new cssInt(0, "index");
  arg_obj->members->Push(eldx);
  cssiEditDialog* carg_dlg = new cssiEditDialog(arg_obj, NULL, true); //modal
  carg_dlg->Constr();
  int ok_can = carg_dlg->Edit();        // true = wait for a response
  if(ok_can) {
    cssEl* el = obj->items->El(eldx->val);
    el->Edit();
  }
  delete carg_dlg;
  delete arg_obj;
}


