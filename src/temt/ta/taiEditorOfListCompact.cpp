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

#include "taiEditorOfListCompact.h"
#include <taList_impl>
#include <taBase_List>
#include <taiType>
#include <iEditGrid>
#include <MemberDef>

#include <taMisc>
#include <taiMisc>



taiEditorOfListCompact::taiEditorOfListCompact(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: taiEditorWidgetsMulti(base, typ_, read_only_, modal_, parent)
{
  header_row = false; 
  cur_lst = (taList_impl*)root;
}

taiEditorOfListCompact::~taiEditorOfListCompact() {
  lst_data_el.Reset();
}

void taiEditorOfListCompact::ClearMultiBody_impl() {
  lst_data_el.Reset();
  inherited::ClearMultiBody_impl();
}


void taiEditorOfListCompact::Constr_Strings() {
  prompt_str = cur_lst->GetTypeDef()->name + ": ";
  if (cur_lst->GetTypeDef()->desc == taBase_List::StatTypeDef(0)->desc) {
    prompt_str += cur_lst->el_typ->name + "s: " + cur_lst->el_typ->desc;
  }
  else {
    prompt_str += cur_lst->GetTypeDef()->desc;
  }
  win_str = String(def_title())
     + " " + cur_lst->GetPathNames();
}

// don't check for null im ptr here
bool taiEditorOfListCompact::ShowMember(MemberDef* md) const {
  return md->ShowMember(show());
}

void taiEditorOfListCompact::Constr_ElData() {
  for (int lf = 0; lf < cur_lst->size; ++lf) {
    taBase* tmp_lf = (taBase*)cur_lst->FastEl_(lf);
    if (tmp_lf == NULL)	continue; // note: not supposed to have NULL values in lists
    TypeDef* tmp_td = tmp_lf->GetTypeDef();
    lst_data_el.Add(new taiListTypeWidgets(tmp_td, tmp_lf));
  }
} 

void taiEditorOfListCompact::Constr_Final() {
  taiEditorWidgetsMulti::Constr_Final();
  multi_body->resizeNames(); //temp: idatatable should do this automatically
}

void taiEditorOfListCompact::Constr_MultiBody() {
  inherited::Constr_MultiBody(); 
  Constr_ElData();
  Constr_ListData();
}


void taiEditorOfListCompact::Constr_ListData() {
  for (int i = 0; i < lst_data_el.size; ++i) {
    taiListTypeWidgets* lf_el = lst_data_el.FastEl(i);
    String nm = String("[") + String(i) + "]: (" + lf_el->typ->name + ")";
    AddMultiRowName(i, nm, String(""));
    // note: the type better grok INLINE!!!!
    taiWidget* mb_dat = lf_el->typ->it->GetWidgetRep(this, NULL, multi_body->dataGridWidget(), NULL, taiWidget::flgInline);
    lf_el->data_el = mb_dat;
    AddMultiData(i, 1, mb_dat->GetRep());
  }
}

void taiEditorOfListCompact::GetValue_Membs() {
  bool rebuild = false;
  if (lst_data_el.size != cur_lst->size) rebuild = true;
  if (!rebuild) {		// check that same elements are present!
    for (int lf = 0;  lf < lst_data_el.size;  ++lf) {
      if (lst_data_el.FastEl(lf)->cur_base != (taBase*)cur_lst->FastEl_(lf)) {
	rebuild = true;
	break;
      }
    }
  }
 // NOTE: we should always be able to do a GetValue, because we always rebuild
 // when data changes (ie, in program, or from another gui panel)
  if (rebuild) {
    taMisc::Error("Cannot apply changes: List size or elements have changed");
    return;
  }

  // first for the List-structure members
  GetValue_Membs_def();
  // then the List elements
  for (int lf=0;  lf < lst_data_el.size;  ++lf) {
    taiListTypeWidgets* lf_el = lst_data_el.FastEl(lf);
    lf_el->typ->it->GetValue(lf_el->data_el, lf_el->cur_base);
    ((taBase*)lf_el->cur_base)->UpdateAfterEdit();
  }
  cur_lst->UpdateAfterEdit();	// call here too!
  taiMisc::Update((taBase*)cur_lst);
}

void taiEditorOfListCompact::GetImage_Membs() {
  bool rebuild = false;
  if (lst_data_el.size != cur_lst->size) rebuild = true;
  if (!rebuild) {		// check that same elements are present!
    for (int lf = 0;  lf < lst_data_el.size;  ++lf) {
      if (lst_data_el.FastEl(lf)->cur_base != (taBase*)cur_lst->FastEl_(lf)) {
	rebuild = true;
	break;
      }
    }
  }

  if (rebuild) {
    RebuildMultiBody(); 
  } 

  // first for the List-structure members
  GetImage_Membs_def();

  // then the elements
  for (int lf = 0;  lf < lst_data_el.size;  ++lf) {
    taiListTypeWidgets* lf_el = lst_data_el.FastEl(lf);
    lf_el->typ->it->GetImage(lf_el->data_el, lf_el->cur_base);
  }
}
/* TODO
int taiEditorOfListCompact::Edit() {
  if ((cur_lst != NULL) && (cur_lst->size > 100)) {
    int rval = taMisc::Choice("List contains more than 100 items (size = " +
			      String(cur_lst->size) + "), continue with Edit?",
			      "Ok", "Cancel");
    if (rval == 1) return 0;
  }
  return taiEditorWidgetsMulti::Edit();
} */


