// Copyright 2017, Regents of the University of Colorado,
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

#include "taiEditorOfArray.h"
#include <taArray_base>
#include <taiType>
#include <MemberDef>
#include <iLabel>



taiEditorOfArray::taiEditorOfArray(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: taiEditorOfClass(base, typ_, read_only_, modal_, parent)
{
  // array items get their own memb, but they are handled by us
  membs.SetMinSize(membs.size + 1);
  array_set = membs.size - 1;
}

taiEditorOfArray::~taiEditorOfArray() {
  //anything?
}

void taiEditorOfArray::ClearBody_impl() {
  taiEditorOfClass::ClearBody_impl();
}

bool taiEditorOfArray::ShowMember(MemberDef* md) const {
  if (md->name == "size")
    return true;
  else
    return inherited::ShowMember(md);
}

void taiEditorOfArray::Constr_Widget_Labels() {
  inherited::Constr_Widget_Labels();
  Constr_AryWidget_Labels();
}

void taiEditorOfArray::Constr_AryWidget_Labels() {
  iLabel* lbl = new iLabel("Array items", body);
  AddSectionLabel(-1, lbl,
    "the data items in the array, one per line");
  
  taArray_base* cur_ary = (taArray_base*)root;
  MemberDef* eldm = typ->members.FindName("el");
  taiType* tit = eldm->type->GetNonPtrType()->it;
  for (int i = 0; i < cur_ary->size; ++i) {
    taiWidget* mb_dat = tit->GetWidgetRep(this, NULL, body);
    widget_el(array_set).Add(mb_dat);
    QWidget* rep = mb_dat->GetRep();
    bool fill_hor = mb_dat->fillHor();
    String nm = String("[") + String(i) + "]";
    //int idx = AddWidget(-1, rep, fill_hor);
    AddNameWidget(-1, nm, String(""), rep, mb_dat, NULL, fill_hor);
  }
}

/* TODO
int taiEditorOfArray::Edit() {
  taArray_base* cur_ary = (taArray_base*)cur_base;
  if((cur_ary != NULL) && (cur_ary->size > 100)) {
    int rval = taMisc::Choice("Array contains more than 100 items (size = " +
			      String(cur_ary->size) + "), continue with Edit?",
			      "Ok", "Cancel");
    if(rval == 1) return 0;
  }
  return taiEditorOfClass::Edit();
} */

void taiEditorOfArray::GetImage_Membs() {
  inherited::GetImage_Membs();
  taArray_base* cur_ary = (taArray_base*)root;
  MemberDef* eldm = typ->members.FindName("el");
  taiType* tit = eldm->type->GetNonPtrType()->it;
  for (int i = 0; i < cur_ary->size; ++i) {
    taiWidget* mb_dat = widget_el(array_set).PosSafeEl(i);
    if (mb_dat == NULL) return; // unexpected end
    tit->GetImage(mb_dat, cur_ary->FastEl_(i));
  }
}

void taiEditorOfArray::GetValue_Membs() {
  inherited::GetValue_Membs();
  taArray_base* cur_ary = (taArray_base*)root;
  MemberDef* eldm = typ->members.FindName("el");
  taiType* tit = eldm->type->GetNonPtrType()->it;
  for (int i = 0; i < cur_ary->size; ++i){
    taiWidget* mb_dat = widget_el(array_set).PosSafeEl(i);
    if (mb_dat == NULL) return; // unexpected
    tit->GetValue(mb_dat, cur_ary->FastEl_(i));
  }
}

