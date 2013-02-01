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

#include "taiEditorWidgetsOfSArg.h"
#include <SArg_Array>
#include <MemberDef>
#include <taiType>


taiEditorWidgetsOfSArg::taiEditorWidgetsOfSArg(void* base, TypeDef* tp,  bool read_only_,
  bool modal_, QObject* parent)
:inherited(base, tp, read_only_, modal_, parent) 
{
}

bool taiEditorWidgetsOfSArg::ShowMember(MemberDef* md) const {
  if (md->name == "size")
    return true;
  else
    return inherited::ShowMember(md);
}

void taiEditorWidgetsOfSArg::Constr_AryData() {
  SArg_Array* cur_ary = (SArg_Array*)root;
  cur_ary->UpdateAfterEdit();
  MemberDef* eldm = typ->members.FindName("el");
  taiType* it = eldm->type->GetNonPtrType()->it;
  QWidget* rep;
  for (int i=0; i < cur_ary->size; ++i) {
    taiData* mb_dat = it->GetDataRep(this, NULL, body);
    data_el(array_set).Add(mb_dat);
    rep = mb_dat->GetRep();
    bool fill_hor = mb_dat->fillHor();
    String nm = String("[") + String(i) + "]";
    String lbl = cur_ary->labels[i];
    if (!lbl.empty())
      nm = lbl + nm;
    //int idx = AddData(-1, rep, fill_hor);
    AddNameData(-1, nm, String(""), rep, mb_dat, NULL, fill_hor);
  }
}
