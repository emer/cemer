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

#include "gpiCompactListDataHost.h"

gpiCompactListDataHost::gpiCompactListDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: gpiMultiEditDataHost(base, typ_, read_only_, modal_, parent)
{
  header_row = false; 
  cur_lst = (taList_impl*)root;
}

gpiCompactListDataHost::~gpiCompactListDataHost() {
  lst_data_el.Reset();
}

void gpiCompactListDataHost::ClearMultiBody_impl() {
  lst_data_el.Reset();
  inherited::ClearMultiBody_impl();
}


void gpiCompactListDataHost::Constr_Strings() {
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
bool gpiCompactListDataHost::ShowMember(MemberDef* md) const {
  return md->ShowMember(show());
}

void gpiCompactListDataHost::Constr_ElData() {
  for (int lf = 0; lf < cur_lst->size; ++lf) {
    taBase* tmp_lf = (taBase*)cur_lst->FastEl_(lf);
    if (tmp_lf == NULL)	continue; // note: not supposed to have NULL values in lists
    TypeDef* tmp_td = tmp_lf->GetTypeDef();
    lst_data_el.Add(new gpiCompactList_ElData(tmp_td, tmp_lf));
  }
} 

void gpiCompactListDataHost::Constr_Final() {
  gpiMultiEditDataHost::Constr_Final();
  multi_body->resizeNames(); //temp: idatatable should do this automatically
}

void gpiCompactListDataHost::Constr_MultiBody() {
  inherited::Constr_MultiBody(); 
  Constr_ElData();
  Constr_ListData();
}


void gpiCompactListDataHost::Constr_ListData() {
  for (int i = 0; i < lst_data_el.size; ++i) {
    gpiCompactList_ElData* lf_el = lst_data_el.FastEl(i);
    String nm = String("[") + String(i) + "]: (" + lf_el->typ->name + ")";
    AddMultiRowName(i, nm, String(""));
    // note: the type better grok INLINE!!!!
    taiData* mb_dat = lf_el->typ->it->GetDataRep(this, NULL, multi_body->dataGridWidget(), NULL, taiData::flgInline);
    lf_el->data_el = mb_dat;
    AddMultiData(i, 1, mb_dat->GetRep());
  }
}

void gpiCompactListDataHost::GetValue_Membs() {
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
    gpiCompactList_ElData* lf_el = lst_data_el.FastEl(lf);
    lf_el->typ->it->GetValue(lf_el->data_el, lf_el->cur_base);
    ((taBase*)lf_el->cur_base)->UpdateAfterEdit();
  }
  cur_lst->UpdateAfterEdit();	// call here too!
  taiMisc::Update((taBase*)cur_lst);
}

void gpiCompactListDataHost::GetImage_Membs() {
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
    gpiCompactList_ElData* lf_el = lst_data_el.FastEl(lf);
    lf_el->typ->it->GetImage(lf_el->data_el, lf_el->cur_base);
  }
}
/* TODO
int gpiCompactListDataHost::Edit() {
  if ((cur_lst != NULL) && (cur_lst->size > 100)) {
    int rval = taMisc::Choice("List contains more than 100 items (size = " +
			      String(cur_lst->size) + "), continue with Edit?",
			      "Ok", "Cancel");
    if (rval == 1) return 0;
  }
  return gpiMultiEditDataHost::Edit();
} */


