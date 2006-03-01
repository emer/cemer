// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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
//#include "ta_css.h"
//#include "ta_type.h"
#include "css_qtdata.h"
#include "css_qtdialog.h"
#include "ta_qt.h"
#include "ta_TA_type.h"

//#include <stdlib.h>

/* OBS:
#ifndef CYGWIN
#include <IV-X11/xevent.h>
#endif
*/

//////////////////////////////////////////////////
// 		cssiType			//
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
    use_it = new taiTokenPtrType(tp);
    use_it->no_setpointer = true; // don't set pointers for css pointers!
  }
  qobj = NULL;
}
cssiType::~cssiType() {
  if (use_it != NULL)
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

taiData* cssiType::GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent) {
  if (use_it != NULL)
    return use_it->GetDataRep(host_, par, gui_parent);
  else
    return typ->it->GetDataRep(host_, par, gui_parent);
}

void cssiType::GetImage(taiData* dat, void* base) {
  if (use_it != NULL)
    use_it->GetImage(dat, base);
  else
    typ->it->GetImage(dat, base);
}

void cssiType::GetValue(taiData* dat, void* base) {
  if (use_it != NULL)
    use_it->GetValue(dat, base);
  else
    typ->it->GetValue(dat, base);
}

//TODO: update to use the ReadOnly approach used by ta_qtdata (using handlesReadOnly aware types)

//////////////////////////////////////////////////
// 		cssiROType			//
//////////////////////////////////////////////////

cssiROType::cssiROType(cssEl* orgo, TypeDef* tp, void* bs, bool use_ptr_type)
  : cssiType(orgo, tp, bs, use_ptr_type)
{
}

taiData* cssiROType::GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent) {
  taiField* rval = new taiField(typ, host_, par, gui_parent, true);
  return rval;
}

void cssiROType::GetImage(taiData* dat, void* base) {
  taiField* rval = (taiField*)dat;
  String strval = typ->GetValStr(base);
  rval->GetImage(strval);
}

//////////////////////////////////////////////////
// 		cssiEnumType			//
//////////////////////////////////////////////////

cssiEnumType::cssiEnumType(cssEl* orgo, cssEnumType* enum_typ, void* bs)
: cssiType(orgo, &TA_enum, bs, false) {
  enum_type = enum_typ;
}

taiData* cssiEnumType::GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent) {
  taiComboBox* rval = new taiComboBox(typ, host_, par, gui_parent);
  for (int i = 0; i < enum_type->enums->size; ++i) {
    rval->AddItem(enum_type->enums->FastEl(i)->name);
  }
  return rval;
}

void cssiEnumType::GetImage(taiData* dat, void* base) {
  taiComboBox* rval = (taiComboBox*)dat;
  int enm_val = *((int*)base);
  for (int i = 0; i < enum_type->enums->size; ++i) {
    if (((cssInt*)enum_type->enums->FastEl(i))->val == enm_val) {
      rval->GetImage(i);
      break;
    }
  }
}

void cssiEnumType::GetValue(taiData* dat, void* base) {
  taiComboBox* rval = (taiComboBox*)dat;
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
: cssiType(orgo, &TA_void, bs, false) {
}

taiData* cssiClassType::GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent) {
  cssClassInst* obj = (cssClassInst*) cur_base;
  if ((obj->type_def != NULL) && (obj->type_def->HasOption("INLINE")
	|| obj->type_def->HasOption("EDIT_INLINE")))
  {
    cssiPolyData* rval = new cssiPolyData(obj, typ, host_, par, gui_parent);
    return rval;
  }
  else {
    taiButtonMenu* rval = new taiButtonMenu
      (taiMenu::normal_update, taiMisc::fonSmall, typ, host_, par, gui_parent);
    Assert_QObj();
    rval->AddItem("Edit", taiMenu::use_default, taiAction::action,
	qobj, SLOT(CallEdit()), (void*)NULL);
    String lbl = String(obj->GetTypeName()) + ": Actions";
    rval->setLabel(lbl);
    return rval;
  }
}

void cssiClassType::GetImage(taiData* dat, void* base) {
  cssClassInst* obj = (cssClassInst*) cur_base;
  if((obj->type_def != NULL) && (obj->type_def->HasOption("INLINE")
				 || obj->type_def->HasOption("EDIT_INLINE"))) {
    cssiPolyData* rval = (cssiPolyData*)dat;
    rval->GetImage(base);
  }
}

void cssiClassType::GetValue(taiData* dat, void* base) {
  cssClassInst* obj = (cssClassInst*) cur_base;
  if ((obj->type_def != NULL) && (obj->type_def->HasOption("INLINE")
	 || obj->type_def->HasOption("EDIT_INLINE")))
  {
    cssiPolyData* rval = (cssiPolyData*)dat;
    rval->GetValue(base);
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

taiData* cssiArrayType::GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent) {
  taiButtonMenu* rval = new taiButtonMenu
    (taiMenu::normal_update, taiMisc::fonSmall, typ, host_, par, gui_parent);
  Assert_QObj();
  rval->AddItem("Edit", taiMenu::use_default, taiAction::action,
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
  int ok_can = carg_dlg->Edit();	// true = wait for a response
  if(ok_can) {
    cssEl* el = obj->items->El(eldx->val);
    el->Edit();
  }
  delete carg_dlg;
  delete arg_obj;
}


