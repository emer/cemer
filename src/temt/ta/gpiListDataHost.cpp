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

#include "gpiListDataHost.h"

gpiListDataHost::gpiListDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: gpiMultiEditDataHost(base, typ_, read_only_, modal_, parent)
{
  cur_lst = (taList_impl*)root;
  num_lst_fields = 0;
}

gpiListDataHost::~gpiListDataHost() {
  lst_data_el.Reset();
  lst_membs.Reset();
}

void gpiListDataHost::ClearMultiBody_impl() {
  lst_data_el.Reset();
  lst_membs.Reset();
  num_lst_fields = 0;
  inherited::ClearMultiBody_impl();
}


void gpiListDataHost::Constr_Strings() {
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

void gpiListDataHost::Constr_Final() {
  gpiMultiEditDataHost::Constr_Final();
  multi_body->resizeNames(); //temp: idatatable should do this automatically
}

void gpiListDataHost::Constr_MultiBody() {
  inherited::Constr_MultiBody(); 
  Constr_ElData();
  Constr_ListLabels();
  Constr_ListData();
}

void gpiListDataHost::Constr_ElData() {
  for (int lf = 0; lf < cur_lst->size; ++lf) {
    taBase* tmp_lf = (taBase*)cur_lst->FastEl_(lf);
    if (tmp_lf == NULL)	continue; // note: not supposed to have NULL values in lists
    TypeDef* tmp_td = tmp_lf->GetTypeDef();
    lst_data_el.Add(new gpiList_ElData(tmp_td, tmp_lf));
    // add to the unique list of all showable members
    for (int i = 0; i < tmp_td->members.size; ++i) {
      MemberDef* md = tmp_td->members.FastEl(i);
      if (ShowMember(md)) {
        lst_membs.AddUnique(md->name);
      }
    }
  }
} 

void gpiListDataHost::Constr_ListData() {
  for (int lf = 0; lf < lst_data_el.size; ++lf) {
    gpiList_ElData* lf_el = lst_data_el.FastEl(lf);
    String nm = String("[") + String(lf) + "]: (" + lf_el->typ->name + ")";
    AddMultiColName(lf, nm, String(""));

    for (int i = 0; i < lf_el->typ->members.size; ++i) {
      MemberDef* md = lf_el->typ->members.FastEl(i);
      if (!ShowMember(md)) continue;
      int lst_idx = lst_membs.FindEl(md->name);
      if (lst_idx < 0) continue; //note: shouldn't happen!!!
      cur_row = lst_idx; 
      taiData* mb_dat = md->im->GetDataRep(this, NULL, multi_body->dataGridWidget());
      lf_el->memb_el.Add(md);
      lf_el->data_el.Add(mb_dat);
      //TODO: should get desc for this member, to add to tooltip for rep
      AddMultiData(cur_row, lf, mb_dat->GetRep());
    }
  }
}

void gpiListDataHost::Constr_ListLabels() {
  for (int lf = 0; lf < lst_membs.size; ++lf) {
    //NOTE: no desc's because same name'd member could conflict
    AddMultiRowName(lf, lst_membs.FastEl(lf), "");
  }
}

void gpiListDataHost::GetValue_Membs() {
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
    gpiList_ElData* lf_el = lst_data_el.FastEl(lf);
    GetValue_impl(&lf_el->memb_el, lf_el->data_el, lf_el->cur_base);
    ((taBase*)lf_el->cur_base)->UpdateAfterEdit();
  }
  cur_lst->UpdateAfterEdit();	// call here too!
  taiMisc::Update((taBase*)cur_lst);
}

void gpiListDataHost::GetImage_Membs() {
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
  /*obs lst_data_el.Reset();
    ivGlyphIndex i;
    for(i=lst_data_g->count()-1; i >= 0; i--)
      lst_data_g->remove(i);
    for(i=labels->count()-1; i >= 1; i--)
      labels->remove(i);
    lst_membs.Reset();
    Constr_ListMembs();
    Constr_Labels_impl(lst_membs);
    Constr_ElData(); */
  } 

  // first for the List-structure members
  GetImage_Membs_def();

  // then the elements
  for (int lf = 0;  lf < lst_data_el.size;  ++lf) {
    gpiList_ElData* lf_el = lst_data_el.FastEl(lf);
    GetImage_impl(&lf_el->memb_el, lf_el->data_el, lf_el->cur_base);
  }
}
/* TODO
int gpiListDataHost::Edit() {
  if ((cur_lst != NULL) && (cur_lst->size > 100)) {
    int rval = taMisc::Choice("List contains more than 100 items (size = " +
			      String(cur_lst->size) + "), continue with Edit?",
			      "Ok", "Cancel");
    if (rval == 1) return 0;
  }
  return gpiMultiEditDataHost::Edit();
} */
