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

#include "ObjDiff.h"

TA_BASEFUNS_CTORS_DEFN(ObjDiff);

#include <ObjDiffRec>
#include <int_PArray>
#include <int_Array>
#include <taStringDiff>
#include <taGroup_impl>
#include <MemberDef>
#include <taMatrix>
#include <taBaseItr>
#include <DataCol>
#include <DataTable>
#include <taArray_base>
#include <iDialogObjDiffBrowser>

#include <taMisc>
#include <taiMisc>

void ObjDiff::Initialize() {
}


bool ObjDiff::DisplayDialog(bool modal_dlg) {
  iDialogObjDiffBrowser* odb = iDialogObjDiffBrowser::New(this, taiMisc::defFontSize);
  bool rval = odb->Browse(modal_dlg);
  // if !modal, browser now owns the diffs list and will delete it when it dies -- and it will
  // be responsible for performing any actions that get done..
  return rval;
}

int ObjDiff::Diff(taBase* obj_a, taBase* obj_b) {
  diffs.Reset();                      // clear out my list of diffs

  a_top = obj_a;
  b_top = obj_b;

  DiffObjs(NULL, obj_a, obj_b);
  
  if(diffs.size == 0)
    return 0;
  return diffs[0]->n_diffs;
}

int ObjDiff::DiffObjs(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj) {
  if(!a_obj || !b_obj) return 0; // shouldn't happen
  if(a_obj->GetTypeDef() != b_obj->GetTypeDef()) {
    // different types must be dealt with at top-level by choosing one or the other change
    return 0;                   // nothing
  }

  int rollback_idx = diffs.size; // rollback to here if no further diffs
  ObjDiffRec* new_par = NewParRec(par_rec, a_obj, b_obj);
  if(!new_par) {
    return 0;
  }
  int n_diffs = 0;
  if(a_obj->InheritsFrom(&TA_taList_impl)) {
    n_diffs = DiffLists(new_par, (taList_impl*)a_obj, (taList_impl*)b_obj);
  }
  else {
    int n_mbr_diffs = DiffMembers(new_par, a_obj, b_obj);
    int n_list_diffs = DiffMemberLists(new_par, a_obj, b_obj);
    n_diffs = n_mbr_diffs + n_list_diffs;
  }
  if(n_diffs == 0) {
    RollBack(rollback_idx);
  }
  else {
    new_par->n_diffs = n_diffs;
  }
  return n_diffs;
}

int ObjDiff::DiffLists(ObjDiffRec* par_rec, taList_impl* list_a, taList_impl* list_b) {
  taStringDiff str_diff;

  HashList(list_a, str_diff.data_a.data);
  HashList(list_b, str_diff.data_b.data);

  str_diff.Diff_impl("", "");       // null strings, use int hash's

  int n_diffs = str_diff.diffs.size;

  // these must be on stack because recursive..
  int_Array a_ok;
  int_Array b_ok;
  a_ok.SetSize(list_a->size);  a_ok.FillSeq();
  b_ok.SetSize(list_b->size);  b_ok.FillSeq();
  
  for(int i=0;i<n_diffs;i++) {
    taStringDiffItem& df = str_diff.diffs[i];
    
    if(df.delete_a == df.insert_b) {
      for(int l=0; l<df.delete_a; l++) {
        ObjDiffRec* rec = NewListDiff(par_rec, ObjDiffRec::A_B_DIFF, list_a, df.start_a+l,
                                      list_b, df.start_b+l);
      }
    }
    else {
      if(df.delete_a > 0) {     // a records exist, b do not..
        for(int l=0; l<df.delete_a; l++) {
          ObjDiffRec* rec = NewListDiff(par_rec, ObjDiffRec::A_NOT_B, list_a, df.start_a + l,
                                        list_b, df.start_b);
          FastIdxRemove(a_ok, df.start_a + l); // don't sub-process A
        }
      }
      if(df.insert_b > 0) {     // b records exist, a do not..
        for(int l=0; l<df.insert_b; l++) {
          ObjDiffRec* rec = NewListDiff(par_rec, ObjDiffRec::B_NOT_A, list_a, df.start_a,
                                        list_b, df.start_b + l);
          FastIdxRemove(b_ok, df.start_b + l); // don't sub-process B
        }
      }
    }
  }

  // a_ok and b_ok MUST have same size at this point!
  if(a_ok.size != b_ok.size) {
    taMisc::Error("DiffList error -- remaining list items not same size!");
    return n_diffs;
  }

  int n_subs = a_ok.size;
  for(int i=0; i<n_subs; i++) {
    taBase* a_obj = (taBase*)list_a->SafeEl_(a_ok[i]);
    taBase* b_obj = (taBase*)list_b->SafeEl_(b_ok[i]);
    int sub_diffs = DiffObjs(par_rec, a_obj, b_obj); // will automatically add parent if needed
    n_diffs += sub_diffs;
  }

  if(list_a->InheritsFrom(&TA_taGroup_impl)) {
    int rollback_idx = diffs.size; // rollback to here if no further diffs
    ObjDiffRec* new_par = NewParRec(par_rec, list_a, list_b);
    int n_gp_diffs = DiffLists(new_par, &(((taGroup_impl*)list_a)->gp),
                               &(((taGroup_impl*)list_b)->gp));
    if(n_gp_diffs == 0) {
      RollBack(rollback_idx);
    }
    else {
      new_par->n_diffs = n_gp_diffs;
    }
    n_diffs += n_gp_diffs;
  }
  
  return n_diffs;
}

void ObjDiff::HashList(taList_impl* list, int_PArray& array) {
  array.Reset();
  array.Alloc(list->size);
  for(int i=0; i<list->size; i++) {
    taBase* obj = (taBase*)list->FastEl_(i);
    if(!obj || obj->GetOwner() != list) {
      array.Add(0);
      continue;
    }
    String key = obj->GetTypeDef()->name + "&" + obj->GetName();
    taHashVal hash = taHashEl::HashCode_String(key);
    array.Add(hash);
  }
}

String ObjDiff::PtrPath(TypeDef* td, void* addr, taBase* top) {
  taBase* rbase = NULL;
  if((td->IsPointer()) && td->IsTaBase()) {
    rbase = *((taBase**)addr);
  }
  else if(InheritsFrom(TA_taSmartRef)) {
    rbase = ((taSmartRef*)addr)->ptr();
  }
  else if(InheritsFrom(TA_taSmartPtr)) {
    rbase = ((taSmartPtr*)addr)->ptr();
  }
  if(rbase) {
    if(rbase->IsChildOf(top)) {
      return rbase->GetPathNames(top);
    }
    return rbase->DisplayPath();
  }
  return "NULL";
}

int ObjDiff::DiffMember(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj, MemberDef* md) {
  TypeDef* td = md->type;
  void* a_addr = md->GetOff(a_obj);
  void* b_addr = md->GetOff(b_obj);
  if(td->IsActualTaBase()) {
    int rollback_idx = diffs.size; // rollback to here if no further diffs
    taBase* a_sub = (taBase*)a_addr;
    taBase* b_sub = (taBase*)b_addr;
    ObjDiffRec* new_par = NewParRec(par_rec, a_sub, b_sub, md);
    int n_diffs = DiffMembers(new_par, a_sub, b_sub);
    if(n_diffs == 0) {
      RollBack(rollback_idx);
    }
    else {
      new_par->n_diffs = n_diffs;
    }
    return n_diffs;
  }
  if(td->IsBasePointerType()) {
    String a_val = PtrPath(td, a_addr, a_top);
    String b_val = PtrPath(td, b_addr, b_top);
    return DiffMemberStrings(par_rec, a_obj, b_obj, md, a_val, b_val);
  }
  String a_val = md->GetValStr(a_obj, TypeDef::SC_VALUE, false);
  String b_val = md->GetValStr(b_obj, TypeDef::SC_VALUE, false);
  return DiffMemberStrings(par_rec, a_obj, b_obj, md, a_val, b_val);
}

int ObjDiff::DiffMembers(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj) {
  // guaranteed to have same types -- just compare members and generate member diffs
  // do NOT go into lists..
  TypeDef* td = a_obj->GetTypeDef();

  if(td->HasOption("DIFF_STRING")) {
    String a_val = a_obj->BrowserEditString();
    String b_val = b_obj->BrowserEditString();
    if(a_val == b_val) return 0;
    // co-opt the parent record now!
    par_rec->a_val = a_val;
    par_rec->b_val = b_val;
    par_rec->n_diffs = 1;
    par_rec->ClearDiffFlag(ObjDiffRec::PARENTS);
    par_rec->SetDiffFlag(ObjDiffRec::A_B_DIFF);
    // this record is the only case of an A_B_DIFF with no mdef set!
    return 1;
  }

  // special cases for array / matrix cases
  if(td->InheritsFrom(&TA_DataCol)) {
    return DiffMatrix(par_rec, ((DataCol*)a_obj)->AR(), ((DataCol*)b_obj)->AR());
  }
  if(td->InheritsFrom(&TA_taMatrix)) {
    return DiffMatrix(par_rec, (taMatrix*)a_obj, (taMatrix*)b_obj);
  }
  if(td->InheritsFrom(&TA_taArray_base)) {
    return DiffArray(par_rec, (taArray_base*)a_obj, (taArray_base*)b_obj);
  }

  int n_diffs = 0;
  
  MemberDef* last_md = NULL;    // special option for putting one member after others!
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(md->HasOption("NO_SAVE") || md->HasOption("READ_ONLY") ||
       md->HasOption("GUI_READ_ONLY") || md->HasOption("HIDDEN") ||
       md->HasOption("NO_DIFF") || md->type->HasOption("NO_DIFF") || md->is_static)
      continue;
    if(md->type->InheritsFrom(&TA_taList_impl)) { // not now
      continue;
    }
    if(md->HasOption("DIFF_LAST")) {
      last_md = md;
      continue;
    }
    if(md->name == "user_data_") {
      continue;                 // too much clutter for now..
    }
    int cnt = DiffMember(par_rec, a_obj, b_obj, md);
    n_diffs += cnt;
  }
  if(last_md) {
    int cnt = DiffMember(par_rec, a_obj, b_obj, last_md);
    n_diffs += cnt;
  }
  return n_diffs;
}

int ObjDiff::DiffMemberLists(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj) {
  // guaranteed to have same types -- sub lists from here
  TypeDef* td = a_obj->GetTypeDef();

  int n_diffs = 0;
  
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(!md->type->InheritsFrom(&TA_taList_impl)) { // now
      continue;
    }
    if(md->HasOption("NO_SAVE") || md->HasOption("READ_ONLY") ||
       md->HasOption("GUI_READ_ONLY") || md->HasOption("HIDDEN") ||
       md->HasOption("NO_DIFF") || md->type->HasOption("NO_DIFF") || md->is_static)
      continue;
    if(md->name == "user_data_") {
      continue;                 // too much clutter for now..
    }
    taList_impl* a_list = (taList_impl*)md->GetOff(a_obj);
    taList_impl* b_list = (taList_impl*)md->GetOff(b_obj);
    int rollback_idx = diffs.size; // rollback to here if no further diffs
    ObjDiffRec* new_par = NewParRec(par_rec, a_list, b_list, md);
    int cnt = DiffLists(new_par, a_list, b_list);
    if(cnt == 0) {
      RollBack(rollback_idx);
    }
    else {
      new_par->n_diffs = cnt;
    }
    n_diffs += cnt;
  }
  return n_diffs;
}

int ObjDiff::DiffMatrix(ObjDiffRec* par_rec, taMatrix* a_mat, taMatrix* b_mat) {
  MatrixGeom a_frame_geom = a_mat->geom;
  if(a_frame_geom.dims() > 1)
    a_frame_geom.SetDims(a_frame_geom.dims()-1);
  MatrixGeom b_frame_geom = b_mat->geom;
  if(b_frame_geom.dims() > 1)
    b_frame_geom.SetDims(b_frame_geom.dims()-1);
  if(a_mat->geom.dims() > 1 || b_mat->geom.dims() > 1) {
    if(a_frame_geom != b_frame_geom) {
      // co-opt the parent record now!
      par_rec->a_val = "frame geom: " + a_frame_geom.ToString();
      par_rec->b_val = "frame geom: " + b_frame_geom.ToString();
      par_rec->n_diffs = 1;
      par_rec->ClearDiffFlag(ObjDiffRec::PARENTS);
      par_rec->SetDiffFlag(ObjDiffRec::A_B_DIFF);
      return 1;
    }
  }
  if(a_mat != a_top && !a_top->InheritsFrom(&TA_DataTable) &&
     !a_top->InheritsFrom(&TA_DataCol)) {
    // we are not specifically diffing this guy -- just go based on size!
    if(a_mat->size != b_mat->size) {
      par_rec->a_val = "size: " + String(a_mat->size);
      par_rec->b_val = "size: " + String(b_mat->size);
      par_rec->n_diffs = 1;
      par_rec->ClearDiffFlag(ObjDiffRec::PARENTS);
      par_rec->SetDiffFlag(ObjDiffRec::A_B_DIFF);
      return 1;
    }
    return 0;                   // assume no diffs!
  }

  // we are specifically diffing this -- here we go!
  
  taStringDiff str_diff;

  HashMatrix(a_mat, str_diff.data_a.data);
  HashMatrix(b_mat, str_diff.data_b.data);

  str_diff.Diff_impl("", "");       // null strings, use int hash's

  int n_diffs = str_diff.diffs.size;
  if(n_diffs == 0) return 0;

  for(int i=0;i<n_diffs;i++) {
    taStringDiffItem& df = str_diff.diffs[i];
    
    if(df.delete_a == df.insert_b) {
      for(int l=0; l<df.delete_a; l++) {
        ObjDiffRec* rec = NewMatrixDiff(par_rec, ObjDiffRec::A_B_DIFF, a_mat, df.start_a+l,
                                        b_mat, df.start_b+l);
      }
    }
    else {
      if(df.delete_a > 0) {     // a records exist, b do not..
        for(int l=0; l<df.delete_a; l++) {
          ObjDiffRec* rec = NewMatrixDiff(par_rec, ObjDiffRec::A_NOT_B, a_mat, df.start_a + l,
                                        b_mat, df.start_b);
        }
      }
      if(df.insert_b > 0) {     // b records exist, a do not..
        for(int l=0; l<df.insert_b; l++) {
          ObjDiffRec* rec = NewMatrixDiff(par_rec, ObjDiffRec::B_NOT_A, a_mat, df.start_a,
                                          b_mat, df.start_b + l);
        }
      }
    }
  }
  return n_diffs;
}

void ObjDiff::HashMatrix(taMatrix* mat, int_PArray& array) {
  array.Reset();
  const int n_fr = mat->Frames();
  array.Alloc(n_fr);
  if(mat->dims() == 1) {
    TA_FOREACH_INDEX(i, *mat) {
      String val = mat->SafeElAsStr(i);
      taHashVal hash = taHashEl::HashCode_String(val);
      array.Add(hash);
    }
  }
  else {
    for(int i=0; i<n_fr; i++) {
      taMatrix* a_fr = mat->GetFrameSlice_(i);
      taBase::Ref(a_fr);
      String val = a_fr->GetValStr();
      taBase::unRefDone(a_fr);
      taHashVal hash = taHashEl::HashCode_String(val);
      array.Add(hash);
    }
  }
}

ObjDiffRec* ObjDiff::NewMatrixDiff
(ObjDiffRec* par_rec, int flags, taMatrix* a_mat, int a_idx, taMatrix* b_mat, int b_idx) {
  ObjDiffRec* rec = NewRec(par_rec, flags, a_idx, b_idx, a_mat, b_mat);
  rec->a_indep_obj = par_rec->a_indep_obj;
  rec->b_indep_obj = par_rec->b_indep_obj;
  rec->SetDiffFlag(ObjDiffRec::VALUES);
  rec->n_diffs = 1;

  if(a_mat->dims() > 1) {
    if(a_idx < a_mat->Frames()) {
      taMatrix* a_fr = a_mat->GetFrameSlice_(a_idx);
      taBase::Ref(a_fr);
      rec->a_val = a_fr->GetValStr();
      taBase::unRefDone(a_fr);
    }

    if(b_idx < b_mat->Frames()) {
      taMatrix* b_fr = b_mat->GetFrameSlice_(b_idx);
      taBase::Ref(b_fr);
      rec->b_val = b_fr->GetValStr();
      taBase::unRefDone(b_fr);
    }
  }
  else {
    rec->a_val = a_mat->SafeElAsStr_Flat_ElView(a_idx);
    rec->b_val = b_mat->SafeElAsStr_Flat_ElView(b_idx);
  }
  return rec;
}

int ObjDiff::DiffArray(ObjDiffRec* par_rec, taArray_base* a_mat, taArray_base* b_mat) {
  if(a_mat != a_top) {
    // we are not specifically diffing this guy -- just go based on size!
    if(a_mat->size != b_mat->size) {
      par_rec->a_val = "size: " + String(a_mat->size);
      par_rec->b_val = "size: " + String(b_mat->size);
      par_rec->n_diffs = 1;
      par_rec->ClearDiffFlag(ObjDiffRec::PARENTS);
      par_rec->SetDiffFlag(ObjDiffRec::A_B_DIFF);
      return 1;
    }
    return 0;                   // assume no diffs!
  }

  // we are specifically diffing this -- here we go!
  
  taStringDiff str_diff;

  HashArray(a_mat, str_diff.data_a.data);
  HashArray(b_mat, str_diff.data_b.data);

  str_diff.Diff_impl("", "");       // null strings, use int hash's

  int n_diffs = str_diff.diffs.size;
  if(n_diffs == 0) return 0;

  for(int i=0;i<n_diffs;i++) {
    taStringDiffItem& df = str_diff.diffs[i];
    
    if(df.delete_a == df.insert_b) {
      for(int l=0; l<df.delete_a; l++) {
        ObjDiffRec* rec = NewArrayDiff(par_rec, ObjDiffRec::A_B_DIFF, a_mat, df.start_a+l,
                                        b_mat, df.start_b+l);
      }
    }
    else {
      if(df.delete_a > 0) {     // a records exist, b do not..
        for(int l=0; l<df.delete_a; l++) {
          ObjDiffRec* rec = NewArrayDiff(par_rec, ObjDiffRec::A_NOT_B, a_mat, df.start_a + l,
                                        b_mat, df.start_b);
        }
      }
      if(df.insert_b > 0) {     // b records exist, a do not..
        for(int l=0; l<df.insert_b; l++) {
          ObjDiffRec* rec = NewArrayDiff(par_rec, ObjDiffRec::B_NOT_A, a_mat, df.start_a,
                                          b_mat, df.start_b + l);
        }
      }
    }
  }
  return n_diffs;
}

void ObjDiff::HashArray(taArray_base* mat, int_PArray& array) {
  array.Reset();
  array.Alloc(mat->size);
  for(int i=0; i<mat->size; i++) {
    String val = mat->FastElAsStr(i);
    taHashVal hash = taHashEl::HashCode_String(val);
    array.Add(hash);
  }
}

ObjDiffRec* ObjDiff::NewArrayDiff
(ObjDiffRec* par_rec, int flags, taArray_base* a_mat, int a_idx, taArray_base* b_mat, int b_idx) {
  ObjDiffRec* rec = NewRec(par_rec, flags, a_idx, b_idx, a_mat, b_mat);
  rec->a_indep_obj = par_rec->a_indep_obj;
  rec->b_indep_obj = par_rec->b_indep_obj;
  rec->SetDiffFlag(ObjDiffRec::VALUES);
  rec->n_diffs = 1;
  rec->a_val = a_mat->SafeElAsStr(a_idx);
  rec->b_val = b_mat->SafeElAsStr(b_idx);
  return rec;
}

void ObjDiff::FastIdxRemove(int_Array& ary, int idx) {
  int st = MIN(idx, ary.size-1); // always less than or equal to idx pos
  while(st >= 0) {
    if(ary.FastEl(st) == idx) {
      ary.RemoveIdx(st);
      return;
    }
    st--;
  }
}

void ObjDiff::RollBack(int rollback) {
  while(diffs.size > rollback) {
    diffs.Pop();
  }
}

ObjDiffRec* ObjDiff::NewParRec(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj, MemberDef* md) {
  if(!md) {
    if(!a_obj->InheritsFrom(&TA_taOBase) || !b_obj->InheritsFrom(&TA_taOBase)) {
      taMisc::Info("Diff cannot be performed on independent objects that are not at least a taOBase, bailing on obj of types, A:", a_obj->GetTypeDef()->name,
                   "B:", b_obj->GetTypeDef()->name);
      return NULL;
    }
  }
  
  ObjDiffRec* rec = NewRec(par_rec, ObjDiffRec::PARENTS, -1, -1, a_obj, b_obj);
  rec->mdef = md;
  if(md) {                      // we are not an independent parent
    rec->a_indep_obj = par_rec->a_indep_obj;
    rec->b_indep_obj = par_rec->b_indep_obj;
  }
  else {
    rec->a_indep_obj = a_obj;
    rec->b_indep_obj = b_obj;
  }
  return rec;
}

ObjDiffRec* ObjDiff::NewListDiff
(ObjDiffRec* par_rec, int flags, taList_impl* list_a, int a_idx,
 taList_impl* list_b, int b_idx) {
  taBase* a_obj = (taBase*)list_a->SafeEl_(a_idx);
  taBase* b_obj = (taBase*)list_b->SafeEl_(b_idx);
  if(a_obj && a_obj->GetOwner() != list_a) // don't want these!
    return NULL;
  if(b_obj && b_obj->GetOwner() != list_b) // don't want these!
    return NULL;
  ObjDiffRec* rec = NewRec(par_rec, flags, a_idx, b_idx, a_obj, b_obj);
  if(a_obj && a_obj->InheritsFrom(&TA_taOBase)) {
    rec->a_indep_obj = a_obj;
  }
  if(b_obj && b_obj->InheritsFrom(&TA_taOBase)) {
    rec->b_indep_obj = b_obj;
  }
  rec->SetDiffFlag(ObjDiffRec::OBJECTS);
  rec->n_diffs = 1;
  return rec;
}

int ObjDiff::DiffMemberStrings
(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj, MemberDef* md,
 const String& a_val, const String& b_val) {
  if(a_val == b_val) return 0;
  ObjDiffRec* rec = NewRec(par_rec, ObjDiffRec::A_B_DIFF, -1, -1, a_obj, b_obj);
  rec->SetDiffFlag(ObjDiffRec::MEMBERS);
  rec->mdef = md;
  rec->a_val = a_val;
  rec->b_val = b_val;
  rec->n_diffs = 1;             // always 1 for members
  rec->a_indep_obj = par_rec->a_indep_obj;
  rec->b_indep_obj = par_rec->b_indep_obj;
  return 1;                     // one diff
}


ObjDiffRec* ObjDiff::NewRec
(ObjDiffRec* par_rec, int flags, int a_idx, int b_idx, taBase* a_obj, taBase* b_obj) {
  ObjDiffRec* rec = (ObjDiffRec*)diffs.New(1);
  rec->flags = (ObjDiffRec::DiffFlags)flags;
  if(par_rec) {
    rec->nest_level = par_rec->nest_level + 1; // always inc
  }
  rec->a_idx = a_idx;
  rec->b_idx = b_idx;
  rec->a_obj = a_obj;
  rec->b_obj = b_obj;
  rec->par_rec = par_rec;
  return rec;
}

bool ObjDiff::DoDiffEdits(Patch* patch_a, Patch* patch_b) {
  return true;
}



/* static void DoDiffEdits_SetRelPath(taBase* par_obj, taObjDiffRec* srec, taObjDiffRec* drec,
                                   Patch* patch) {
  MemberDef* md;
  taBase* new_guy = par_obj->FindFromPath(srec->value, md);
  if(patch) {
    String str_val = "NULL";
    if(new_guy)
      str_val = new_guy->GetPathFromProj();
    PatchRec* pr = patch->NewPatchRec_Assign(par_obj, str_val);
    if(drec->mdef) {
      pr->mbr_path = drec->mdef->name;
    }
    return;
  }
  if(drec->type->IsPointer() && drec->type->IsTaBase()) {
    if(drec->mdef && drec->mdef->HasOption("OWN_POINTER")) {
      if(!drec->par_addr)
        taMisc::Warning("*** NULL parent for owned pointer:",drec->GetDisplayName());
      else
        taBase::OwnPointer((taBase**)drec->addr, new_guy, (taBase*)drec->par_addr);
    }
    else {
      if(drec->mdef && drec->mdef->HasOption("NO_SET_POINTER"))
        (*(taBase**)drec->addr) = new_guy;
      else
        taBase::SetPointer((taBase**)drec->addr, new_guy);
    }
  }
  else if(drec->type->InheritsFrom(TA_taSmartRef)) {
    ((taSmartRef*)drec->addr)->set(new_guy);
  }
  else if(drec->type->InheritsFrom(TA_taSmartPtr)) {
    ((taSmartPtr*)drec->addr)->set(new_guy);
  }
}

bool ObjDiff::DoDiffEdits(Patch* patch_a, Patch* patch_b) {
  StructUpdate(true);

  taProject* proj_a = (taProject*)diffs.tab_obj_a->GetOwner(&TA_taProject);
  taProject* proj_b = (taProject*)diffs.tab_obj_b->GetOwner(&TA_taProject);

  String save_str;              // string for saving files to
  
  //  for(int i=diffs.size-1; i>= 0; i--) { // go backwards to minimize knock-on effects
  for(int i=0; i < diffs.size; i++) { // forwards usually better actually
    taObjDiffRec* rec = diffs[i];
    if(!rec->HasDiffFlag(taObjDiffRec::ACT_MASK)) continue;
    if(!rec->addr || !rec->type) continue; // sanity checks..

    // NOTE: rec is A, rec->diff_odr is B
    
    bool ta_bases = false;
    bool tab_diff_typ = false;
    bool taptr = false;
    taBase* tab_a = NULL;
    taBase* tab_b = NULL;;
    // make sure pointers are still current
    if(rec->type->IsActualTaBase()) {
      if(rec->tabref) {
        if(!((taBaseRef*)rec->tabref)->ptr())
          continue;
        tab_a = (taBase*)rec->addr;
      }
      if(rec->diff_odr) {
        if(rec->diff_odr->tabref) {
          if(!((taBaseRef*)rec->diff_odr->tabref)->ptr())
            continue;
          tab_b = (taBase*)rec->diff_odr->addr;
        }
      }
      if(tab_a && tab_b)
        ta_bases = true;
      if((!rec->mdef && (rec->type != rec->diff_odr->type))) {
        if(!ta_bases) continue; // no can do
        tab_diff_typ = true;
      }
    }
    else if(((rec->type->IsPointer()) && rec->type->IsTaBase()) ||
            rec->type->InheritsFrom(TA_taSmartRef) ||
            rec->type->InheritsFrom(TA_taSmartPtr)) {
      taptr = true;
    }

    taBase* tab_par_a = NULL;
    taBase* tab_par_b = NULL;
    if(rec->par_type && rec->par_type->IsActualTaBase()) {
      // make sure *parent* pointer is still current
      if(rec->par_odr && rec->par_odr->tabref) {
        if(!((taBaseRef*)rec->par_odr->tabref)->ptr())
          continue;
        tab_par_a = (taBase*)rec->par_addr;
      }
    }
    if(rec->diff_odr && rec->diff_odr->par_type &&
       rec->diff_odr->par_type->IsActualTaBase()) {
      // make sure *parent* pointer is still current
      if(rec->diff_odr->par_odr && rec->diff_odr->par_odr->tabref) {
        if(!((taBaseRef*)rec->diff_odr->par_odr->tabref)->ptr())
          continue;
        tab_par_b = (taBase*)rec->diff_odr->par_addr;
      }
    }

    String tab_a_path;
    String tab_b_path;
    if(tab_par_a) tab_a_path = tab_par_a->DisplayPath();
    if(tab_par_b) tab_b_path = tab_par_b->DisplayPath();

    //////////////////////////////////
    // Copy

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_BA)) {
      taMisc::Info("Copying B -> A:", tab_b_path, "->", tab_a_path, "\n",
                   rec->GetDisplayName(), "=", rec->diff_odr->value);
      if(tab_diff_typ) {
        // need to replace old guy with new one
        if(patch_a) {
          tab_b->Save_String(save_str);
          PatchRec* pr = patch_a->NewPatchRec_Replace(tab_a, save_str);
        }
        else {
          taBase* down = tab_a->GetOwner();
          if(down) {
            down->CopyChildBefore(tab_b, tab_a);
            tab_a->Close();       // nuke old guy
          }
        }
      }
      else {
        if(rec->diff_odr->HasDiffFlag(taObjDiffRec::VAL_PATH_REL)) {
          DoDiffEdits_SetRelPath(diffs.tab_obj_a, rec->diff_odr, rec, patch_a);
        }
        else if(taptr) {
          DoDiffEdits_SetRelPath(proj_a, rec->diff_odr, rec, patch_a);
        }
        else {
          if(ta_bases) {
            if(patch_a) {
              tab_b->Save_String(save_str);
              PatchRec* pr = patch_a->NewPatchRec_Assign(tab_a, save_str);
            }
            else {
              rec->type->CopyFromSameType(rec->addr, rec->diff_odr->addr);
              tab_a->SetName(tab_b->GetName()); // need to copy names too!
            }
          }
          else {
            if(patch_a) {
              if(rec->mdef) {
                String sval = rec->diff_odr->type->GetValStr
                  (rec->diff_odr->addr, tab_b, rec->diff_odr->mdef, TypeDef::SC_STREAMING, true);
                PatchRec* pr = patch_a->NewPatchRec_Assign(tab_par_a, sval);
                pr->mbr_path = rec->mdef->name;
              }
              else {
                taMisc::Info("non-taBase non-member copy -- no can do..");
                continue;
              }
            }
            else {
              rec->type->CopyFromSameType(rec->addr, rec->diff_odr->addr);
            }
          }
        }
        if(!patch_a && tab_par_a) {
          tab_par_a->MemberUpdateAfterEdit(rec->mdef);
          tab_par_a->UpdateAfterEdit();
        }
      }
      continue;
    }

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_AB)) {
      taMisc::Info("Copying A -> B:", tab_a_path, "->", tab_b_path, "\n",
                   rec->diff_odr->GetDisplayName(), "=", rec->value);
      if(tab_diff_typ) {
        // need to replace old guy with new one
        if(patch_b) {
          tab_a->Save_String(save_str);
          PatchRec* pr = patch_b->NewPatchRec_Replace(tab_b, save_str);
        }
        else {
          taBase* down = tab_b->GetOwner();
          if(down) {
            down->CopyChildBefore(tab_a, tab_b);
            tab_b->Close();       // nuke old guy
          }
        }
      }
      else {
        if(rec->HasDiffFlag(taObjDiffRec::VAL_PATH_REL)) {
          DoDiffEdits_SetRelPath(diffs.tab_obj_b, rec, rec->diff_odr, patch_b);
        }
        else if(taptr) {
          DoDiffEdits_SetRelPath(proj_b, rec, rec->diff_odr, patch_b);
        }
        else {
          if(ta_bases) {
            if(patch_b) {
              tab_a->Save_String(save_str);
              PatchRec* pr = patch_b->NewPatchRec_Assign(tab_b, save_str);
            }
            else {
              rec->diff_odr->type->CopyFromSameType(rec->diff_odr->addr, rec->addr);
              tab_b->SetName(tab_a->GetName()); // need to copy names too!
            }
          }
          else {
            if(patch_b) {
              if(rec->diff_odr->mdef) {
                String sval = rec->type->GetValStr
                  (rec->addr, tab_a, rec->mdef, TypeDef::SC_STREAMING, true);
                PatchRec* pr = patch_b->NewPatchRec_Assign(tab_par_b, sval);
                pr->mbr_path = rec->diff_odr->mdef->name;
              }
              else {
                taMisc::Info("non-taBase non-member copy -- no can do..");
                continue;
              }
            }
            else {
              rec->diff_odr->type->CopyFromSameType(rec->diff_odr->addr, rec->addr);
            }
          }
          if(!patch_b && tab_par_b) {
            tab_par_b->MemberUpdateAfterEdit(rec->diff_odr->mdef);
            tab_par_b->UpdateAfterEdit();
          }
        }
      }
      continue;
    }

    if(!ta_bases) continue;     // only ta bases from this point on!

    //////////////////////////////////
    //          Add
    bool add = false;
    bool added = false;
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_ADD_A)) { // do add before del..
      taMisc::Info("Adding A to B:", tab_a_path, "\n", rec->GetDisplayName());
      add = true;
      // if(rec->diff_odr->nest_level == rec->nest_level) {
      taBase* down = tab_b->GetOwner();
      if(down) {
        if(patch_b) {
          tab_a->Save_String(save_str);
          PatchRec* pr = patch_b->NewPatchRec_Insert
            (tab_b, down, save_str, tab_a->GetTypeDef()->name);
        }
        else {
          down->CopyChildBefore(tab_a, tab_b);
        }
        added = true;
      }
    }
    //   }
    //   else if(tab_par_b) {
    //     // todo: not sure any of this logic makes sense -- we said to put it in b so
    //     // why doesn't it just go in b!?
    //     if(rec->diff_odr->nest_level < rec->nest_level) {
    //       // for last obj in list, dest is now another member in parent obj..
    //       taMisc::Info("diff nest -- rec:", String(rec->nest_level), "diff:",
    //                    String(rec->diff_odr->nest_level),"rec path:", tab_a_path,
    //                    "diff path:", tab_b_path);
    //       if(rec->par_odr->mdef) {
    //         // find member in dest par (parents always ta base..)
    //         if(rec->diff_odr->par_odr->mdef &&
    //            rec->diff_odr->par_odr->mdef->name == rec->par_odr->mdef->name) {
    //           // parent is the guy!
    //           if(patch_b) {
    //             tab_a->Save_String(save_str);
    //             PatchRec* pr = patch_a->NewPatchRec_Insert
    //               (tab_par_b, tab_par_b, save_str, tab_a->GetTypeDef()->name);
    //           }
    //           else {
    //             tab_par_b->CopyChildBefore(tab_a, NULL); // NULL goes to end..
    //           }
    //           added = true;
    //         }
    //         else {
    //           MemberDef* dmd;
    //           void* mbase = tab_par_b->FindMembeR(rec->par_odr->mdef->name, dmd);
    //           if(dmd && dmd->type->IsActualTaBase()) { // it should!
    //             taBase* down = (taBase*)mbase;
    //             down->CopyChildBefore(tab_a, NULL); // NULL goes to end..
    //             added = true;
    //           }
    //         }
    //       }
    //       else { // go one level higher
    //         taObjDiffRec* parpar_a = rec->par_odr->par_odr;
    //         if(parpar_a->mdef) {
    //           taBase* tabparpar_b = tab_par_b->GetOwner();
    //           // find member in dest par (parents always ta base..)
    //           MemberDef* dmd;
    //           void* mbase = tabparpar_b->FindMembeR(parpar_a->mdef->name, dmd);
    //           if(dmd && dmd->type->IsActualTaBase()) { // it should!
    //             taBase* down = (taBase*)mbase;
    //             down->CopyChildBefore(tab_a, NULL); // NULL goes to end..
    //             added = true;
    //           }
    //         }
    //       }
    //     }
    //     else if(rec->diff_odr->nest_level > rec->nest_level) {
    //       taBase* down = tab_par_b;
    //       for(int k=0;k < rec->diff_odr->nest_level - rec->nest_level;k++) {
    //         down = down->GetOwner();
    //         if(!down) break;
    //       }
    //       if(down) {
    //         down->CopyChildBefore(tab_a, NULL); // no ref info avail -- probably end
    //         added = true;
    //       }
    //     }
    //   }
    // }
    
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_ADD_B)) { // do add before del..
      taMisc::Info("Adding B to A:", tab_b_path, "\n", rec->GetDisplayName());
      add = true;
      // if(rec->diff_odr->nest_level == rec->nest_level) {
      taBase* down = tab_a->GetOwner();
      if(down) {
        if(patch_a) {
          tab_b->Save_String(save_str);
          PatchRec* pr = patch_a->NewPatchRec_Insert
            (tab_a, down, save_str, tab_b->GetTypeDef()->name);
        }
        else {
          down->CopyChildBefore(tab_b, tab_a);
        }
        added = true;
      }
    }

    if(add && !added) {
      taMisc::Warning("NOTE: Last Add was NOT successful!", rec->GetDisplayName());
    }

    //////////////////////////////////
    //          Del

    if(rec->tabref && !((taBaseRef*)rec->tabref)->ptr()) continue;
    // double-check

    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_DEL_A)) {
      taMisc::Info("Deleting A:", tab_a_path, "\n", rec->GetDisplayName());
      if(patch_a) {
        tab_a->Save_String(save_str);
        PatchRec* pr = patch_a->NewPatchRec_Delete(tab_a, save_str);
      }
      else {
        tab_a->Close();
      }
    }
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_DEL_B)) {
      taMisc::Info("Deleting B:", tab_b_path, "\n", rec->GetDisplayName());
      if(patch_b) {
        tab_b->Save_String(save_str);
        PatchRec* pr = patch_b->NewPatchRec_Delete(tab_b, save_str);
      }
      else {
        tab_b->Close();
      }
    }
  }
  StructUpdate(false);
  return true;
}

*/
