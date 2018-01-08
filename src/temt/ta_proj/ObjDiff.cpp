// Copyright 2017-2018, Regents of the University of Colorado,
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

#include <taProject>
#include <Patch>
#include <Patch_Group>
#include <PatchRec>
#include <PatchRec_Group>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>

void ObjDiff::Initialize() {
  modify_a = false;
  modify_b = false;
  a_only = false;
  same_proj = false;
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

  taProject* a_proj = (taProject*)obj_a->GetThisOrOwner(&TA_taProject);
  taProject* b_proj = (taProject*)obj_b->GetThisOrOwner(&TA_taProject);
  same_proj = (a_proj != NULL && a_proj == b_proj);
  
  a_path_names = obj_a->GetPathFromProj();
  a_path_idx = obj_a->GetPath(a_proj);
  b_path_names = obj_b->GetPathFromProj();
  b_path_idx = obj_b->GetPath(b_proj);

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


ObjDiffRec* ObjDiff::NewListContext(ObjDiffRec* par_rec, int flags, taList_impl* list_a, int a_idx, taList_impl* list_b, int b_idx, int_Array& a_ok, int_Array& b_ok, int a_off, int b_off, int chunk) {
  taBase* a_obj = NULL;
  taBase* b_obj = NULL;
  int a_c_idx = -1; 
  int b_c_idx = -1;
  int a_ok_idx = FastIdxFind(a_ok, a_idx + a_off);
  if(a_ok_idx >= 0) {
    a_c_idx = a_idx + a_off;
    a_obj = (taBase*)list_a->SafeEl_(a_c_idx);
  }
  int b_ok_idx = FastIdxFind(b_ok, b_idx + b_off);
  if(b_ok_idx >= 0) {
    b_c_idx = b_idx + b_off;
    b_obj = (taBase*)list_b->SafeEl_(b_c_idx);
  }
  if(!a_obj && !b_obj) {
    return NULL;
  }

  ObjDiffRec* rec = NewRec(par_rec, flags, a_c_idx, b_c_idx, a_obj, b_obj);
  if(a_obj && a_obj->InheritsFrom(&TA_taOBase)) {
    rec->a_indep_obj = a_obj;
  }
  if(b_obj && b_obj->InheritsFrom(&TA_taOBase)) {
    rec->b_indep_obj = b_obj;
  }
  rec->n_diffs = 0;
  rec->chunk = chunk;

  // don't do this because it can affect the rollback to delete guys -- there just will
  // be some duplicated context!
  // if(a_off < 0) {               // look for possible dupe against previous context, for before context only
  //   int n_to_check = -a_off;    // 1 or 2
  //   for(int i=n_to_check; i>=1; i--) {
  //     if(diffs.size >= i + 1) {
  //       int chk_idx = diffs.size - i - 1;
  //       ObjDiffRec* last_rec = diffs.FastEl(chk_idx);
  //       if(last_rec->IsContext() && last_rec->CompareRecObjs(*rec)) {
  //         diffs.RemoveIdx(chk_idx); // get rid of after guy b/c before is more important
  //         return rec;               // don't do any more!
  //       }
  //     }
  //   }
  // }
  return rec;
}

void ObjDiff::AddListContext_Before(ObjDiffRec* par_rec, taList_impl* list_a, int a_idx, taList_impl* list_b, int b_idx, int_Array& a_ok, int_Array& b_ok, int chunk) {
  NewListContext(par_rec, ObjDiffRec::CONTEXT_B2, list_a, a_idx, list_b, b_idx,
                 a_ok, b_ok, -2, -2, chunk);
  NewListContext(par_rec, ObjDiffRec::CONTEXT_B1, list_a, a_idx, list_b, b_idx,
                 a_ok, b_ok, -1, -1, chunk);
}

void ObjDiff::AddListContext_After(ObjDiffRec* par_rec, taList_impl* list_a, int a_idx, taList_impl* list_b, int b_idx, int_Array& a_ok, int_Array& b_ok, int chunk) {
  NewListContext(par_rec, ObjDiffRec::CONTEXT_A1, list_a, a_idx, list_b, b_idx,
                 a_ok, b_ok, 0, 0, chunk);
  NewListContext(par_rec, ObjDiffRec::CONTEXT_A2, list_a, a_idx, list_b, b_idx,
                 a_ok, b_ok, 1, 1, chunk);
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

  // first update lists of valid indexes
  for(int i=0;i<n_diffs;i++) {
    taStringDiffItem& df = str_diff.diffs[i];
    if(df.delete_a != df.insert_b) {
      if(df.delete_a > 0) {     // a records exist, b do not..
        for(int l=0; l<df.delete_a; l++) {
          FastIdxRemove(a_ok, df.start_a + l); // don't sub-process A
        }
      }
      if(df.insert_b > 0) {     // b records exist, a do not..
        for(int l=0; l<df.insert_b; l++) {
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

  for(int i=0;i<n_diffs;i++) {
    taStringDiffItem& df = str_diff.diffs[i];
    
    if(df.delete_a == df.insert_b) {
      AddListContext_Before(par_rec, list_a, df.start_a, list_b, df.start_b, a_ok, b_ok, i);
      for(int l=0; l<df.delete_a; l++) {
        ObjDiffRec* rec = NewListDiff(par_rec, ObjDiffRec::A_B_DIFF, list_a, df.start_a+l,
                                      list_b, df.start_b+l, i);
        rec->chunk = i;
      }
      AddListContext_After(par_rec, list_a, df.start_a + df.delete_a, list_b,
                           df.start_b + df.delete_a, a_ok, b_ok, i);
    }
    else {
      if(df.delete_a > 0) {     // a records exist, b do not..
        AddListContext_Before(par_rec, list_a, df.start_a, list_b, df.start_b,
                              a_ok, b_ok, i);
        for(int l=0; l<df.delete_a; l++) {
          NewListDiff(par_rec, ObjDiffRec::A_NOT_B, list_a, df.start_a + l,
                                        list_b, df.start_b, i);
        }
        AddListContext_After(par_rec, list_a, df.start_a + df.delete_a, list_b, df.start_b,
                             a_ok, b_ok, i);
      }
      if(df.insert_b > 0) {     // b records exist, a do not..
        AddListContext_Before(par_rec, list_a, df.start_a, list_b, df.start_b,
                              a_ok, b_ok, i);
        for(int l=0; l<df.insert_b; l++) {
          NewListDiff(par_rec, ObjDiffRec::B_NOT_A, list_a, df.start_a,
                      list_b, df.start_b + l, i);
        }
        AddListContext_After(par_rec, list_a, df.start_a, list_b, df.start_b + df.insert_b,
                             a_ok, b_ok, i);
      }
    }
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
    taSubGroup* gp_a = &(((taGroup_impl*)list_a)->gp);
    taSubGroup* gp_b = &(((taGroup_impl*)list_b)->gp);
    ObjDiffRec* new_par = NewParRec(par_rec, gp_a, gp_b);
    int n_gp_diffs = DiffLists(new_par, gp_a, gp_b);
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
  else if(td->InheritsFrom(TA_taSmartRef)) {
    rbase = ((taSmartRef*)addr)->ptr();
  }
  else if(td->InheritsFrom(TA_taSmartPtr)) {
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
    // treat un-owned objs as strings:
    if(a_sub->GetOwner() == NULL || b_sub->GetOwner() == NULL) {
      String a_val = md->GetValStr(a_obj, TypeDef::SC_VALUE, false);
      String b_val = md->GetValStr(b_obj, TypeDef::SC_VALUE, false);
      return DiffMemberStrings(par_rec, a_obj, b_obj, md, a_val, b_val);
    }
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

  int n_diffs = 0;
  bool diff_string = false;
  
  if(td->HasOption("DIFF_STRING")) {
    diff_string = true;
    String a_val = a_obj->GetDiffString();
    String b_val = b_obj->GetDiffString();
    if(a_val != b_val) {
      // co-opt the parent record now!
      par_rec->a_val = a_val;
      par_rec->b_val = b_val;
      par_rec->n_diffs = 1;
      par_rec->ClearDiffFlag(ObjDiffRec::PARENTS);
      par_rec->SetDiffFlag(ObjDiffRec::OBJECTS);
      par_rec->SetDiffFlag(ObjDiffRec::A_B_DIFF);
      // this record is the only case of an A_B_DIFF with no mdef set!
      return 1;
    }
    // otherwise, we can still trigger on diffs between members marked DIFF
  }

  // special cases for array / matrix cases
  if(td->InheritsFrom(&TA_DataCol)) {
    DataCol* dc_a = (DataCol*)a_obj; 
    DataCol* dc_b = (DataCol*)b_obj;
    if(dc_a->rows() != dc_b->rows() && 
       !a_top->InheritsFrom(&TA_DataTable) && !a_top->InheritsFrom(&TA_DataCol))
      return 0;                 // row diffs already noted in DataTable
    return DiffMatrix(par_rec, dc_a->AR(), dc_b->AR());
  }
  if(td->InheritsFrom(&TA_taMatrix)) {
    return DiffMatrix(par_rec, (taMatrix*)a_obj, (taMatrix*)b_obj);
  }
  if(td->InheritsFrom(&TA_taArray_base)) {
    return DiffArray(par_rec, (taArray_base*)a_obj, (taArray_base*)b_obj);
  }

  MemberDef* last_md = NULL;    // special option for putting one member after others!
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("DIFF") && // positive diff overrides all..
       (md->HasNoSave() || md->IsGuiReadOnly() || md->IsInvisible() || md->HasNoDiff()
        || md->type->HasOption("NO_DIFF")))
      continue;
    if(md->type->InheritsFrom(&TA_taList_impl)) { // not now
      continue;
    }
    if(diff_string && !md->HasOption("DIFF")) { // string diffs can still have sub-diffs
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
    if(!md->HasOption("DIFF") &&
       (md->HasNoSave() || md->IsGuiReadOnly() || md->HasNoDiff() ||
        md->type->HasOption("NO_DIFF")))
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

int ObjDiff::FastIdxFind(int_Array& ary, int idx) {
  int st = MIN(idx, ary.size-1); // always less than or equal to idx pos
  while(st >= 0) {
    if(ary.FastEl(st) == idx) {
      return st;
    }
    st--;
  }
  return -1;
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
 taList_impl* list_b, int b_idx, int chunk) {
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
  rec->chunk = chunk;
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

/////////////////////////////////////////////////////////////////////////////
//              Patch Gen

bool ObjDiff::FixObjPaths(String& val, bool a_or_b) {
  if(!same_proj) return false;
  int n_rep = 0;
  if(a_or_b) {                  // a getting from b
    n_rep = val.gsub(b_path_idx, a_path_idx);
    n_rep += val.gsub(b_path_names, a_path_names);
  }
  else {                        // b getting from a
    n_rep = val.gsub(b_path_idx, a_path_idx);
    n_rep += val.gsub(b_path_names, a_path_names);
  }
  return (n_rep > 0);
}

void ObjDiff::GenPatch_CopyBA(ObjDiffRec* rec, Patch* patch) {
  if(!patch) return;
  if(rec->IsMembers()) {
    patch->NewRec_AssignMbr(this, true, rec->a_indep_obj, rec->a_obj, rec->mdef, rec->b_val);
  }
  else if(rec->IsObjects()) {
    if(rec->a_indep_obj->GetTypeDef() == rec->b_indep_obj->GetTypeDef()) { // just diff name
      patch->NewRec_AssignObj(this, true, rec->a_indep_obj, rec->b_indep_obj);
    }
    else {                      // replace
      taList_impl* own_obj = (taList_impl*)rec->par_rec->a_obj; // where to replace in a
      patch->NewRec_Replace(this, true, own_obj, rec->a_indep_obj, rec->b_indep_obj);
    }
  }
  // todo: value!
}

void ObjDiff::GenPatch_CopyAB(ObjDiffRec* rec, Patch* patch) {
  if(!patch) return;
  if(rec->IsMembers()) {
    patch->NewRec_AssignMbr(this, false, rec->b_indep_obj, rec->b_obj, rec->mdef, rec->a_val);
  }
  else if(rec->IsObjects()) {
    if(rec->b_indep_obj->GetTypeDef() == rec->a_indep_obj->GetTypeDef()) { // just diff name
      patch->NewRec_AssignObj(this, false, rec->b_indep_obj, rec->a_indep_obj);
    }
    else {                      // replace
      taList_impl* own_obj = (taList_impl*)rec->par_rec->b_obj; // where to replace in b
      patch->NewRec_Replace(this, false, own_obj, rec->b_indep_obj, rec->a_indep_obj);
    }
  }
  // todo: value!
}

void ObjDiff::GenPatch_DelA(ObjDiffRec* rec, Patch* patch) {
  if(!patch) return;
  if(rec->IsObjects()) {
    patch->NewRec_Delete(this, true, rec->a_indep_obj);
  }
  // todo: value!
}

void ObjDiff::GenPatch_DelB(ObjDiffRec* rec, Patch* patch) {
  if(!patch) return;
  if(rec->IsObjects()) {
    patch->NewRec_Delete(this, false, rec->b_indep_obj);
  }
  // todo: value!
}

void ObjDiff::GenPatch_AddA(ObjDiffRec* rec, Patch* patch, ObjDiffRec* prv_rec) {
  if(!patch) return;
  if(rec->IsObjects()) {
    taList_impl* own_obj = (taList_impl*)rec->par_rec->b_obj; // where to add in b

    taBase* aft_obj = NULL;
    if(prv_rec) {
      if(prv_rec->IsContext()) {
        aft_obj = prv_rec->b_indep_obj;
      }
      else if(prv_rec->HasDiffFlag(ObjDiffRec::ACT_ADD_A)) {
        aft_obj = prv_rec->a_indep_obj; // A that will be added..
      }
    }
    taBase* bef_obj = rec->b_obj;
    patch->NewRec_Insert(this, false, own_obj, rec->a_indep_obj, aft_obj, bef_obj);
  }
  // todo: value!
}

void ObjDiff::GenPatch_AddB(ObjDiffRec* rec, Patch* patch, ObjDiffRec* prv_rec) {
  if(!patch) return;
  if(rec->IsObjects()) {
    taList_impl* own_obj = (taList_impl*)rec->par_rec->a_obj; // where to add in a
    
    taBase* aft_obj = NULL;
    if(prv_rec) {
      if(prv_rec->IsContext()) {
        aft_obj = prv_rec->a_indep_obj;
      }
      else if(prv_rec->HasDiffFlag(ObjDiffRec::ACT_ADD_B)) {
        aft_obj = prv_rec->b_indep_obj; // B that will be added..
      }
    }
    taBase* bef_obj = rec->a_obj;
    patch->NewRec_Insert(this, true, own_obj, rec->b_indep_obj, aft_obj, bef_obj);
  }
  // todo: value!
}


void ObjDiff::SetCurSubgp(taBase* obj, taProject* proj) {
  Patch::cur_subgp = "Project";
  if(!obj) return;
  TypeDef* td = proj->GetTypeDef();
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_taGroup_impl) && !md->HasOption("NO_SAVE") &&
       !md->HasOption("NO_DIFF")) {
      taGroup_impl* gp = (taGroup_impl*)md->GetOff(proj);
      if(gp->IsParentOf(obj)) {
        Patch::cur_subgp = md->name;
        break;
      }
    }
  }
}

void ObjDiff::ReorderProjSubgps(Patch* pat) {
  if(!pat) return;
  // params need to have the stuff they point to in place already!
  taSubGroup& subgp = pat->patch_recs.gp;
  if(subgp.size < 2) return;
  int last_idx = subgp.size-1;
  int n_moved = 0;
  for(int i=0; i< subgp.size - n_moved; i++) {
    Patch_Group* gp = (Patch_Group*)subgp[i];
    if(gp->name == "ctrl_panels") {
      subgp.MoveIdx(i, last_idx);
      i--;
      n_moved++;
    }
    else if(gp->name == "active_params") {
      subgp.MoveIdx(i, last_idx);
      i--;
      n_moved++;
    }
    else if(gp->name == "archived_params") {
      subgp.MoveIdx(i, last_idx);
      i--;
      n_moved++;
    }
  }
}

void ObjDiff::NamePatch(Patch* patch, taBase* from_obj, taBase* to_obj) {
  if(!patch) return;
  String fmnm = from_obj->GetName();
  String tonm = to_obj->GetName();
  patch->from_name = fmnm;
  patch->to_name = tonm;
  String common = common_prefix(fmnm, tonm, 0);
  if(common.nonempty()) {
    tonm = tonm.after(common);
    if(tonm.startsWith("_"))
      tonm = tonm.after("_");
  }
  patch->SetName(fmnm + "_to_" + tonm);
}


int ObjDiff::GeneratePatches() {
  if(!a_top || !b_top) return -1; // nothing
  taProject* proj_a = (taProject*)a_top.ptr()->GetThisOrOwner(&TA_taProject);
  taProject* proj_b = (taProject*)b_top.ptr()->GetThisOrOwner(&TA_taProject);
  if(TestError(!proj_a || !proj_b, "GeneratePatches",
               "could not find project owners for top-level diff objects")) {
    return -1;
  }

  // note: proj could be same!
  Patch* patch_a = proj_a->patches.NewPatch();
  Patch* patch_b = NULL;

  NamePatch(patch_a, a_top, b_top);

  if(!a_only) {
    patch_b = proj_b->patches.NewPatch();
    NamePatch(patch_b, b_top, a_top);
  }

  Patch::cur_subgp = "";
  
  int n_patches = 0;

  bool proj_diff = false;
  if(a_top == proj_a) {
    proj_diff = true;
  }

  ObjDiffRec* prv_rec = NULL;
  for(int i=0; i < diffs.size; i++) {
    ObjDiffRec* rec = diffs[i];
    if(!rec->HasAct() || !rec->IsValid()) {
      prv_rec= rec;
      continue;
    }
    n_patches++;

    if(proj_diff) {
      if(rec->a_indep_obj)
        SetCurSubgp(rec->a_indep_obj, proj_a);
      else
        SetCurSubgp(rec->b_indep_obj, proj_b);
    }
    
    //////////////////////////////////
    // Copy

    if(rec->IsABDiff()) {
      if(rec->HasDiffFlag(ObjDiffRec::ACT_COPY_BA)) {
        GenPatch_CopyBA(rec, patch_a);
      }
      if(rec->HasDiffFlag(ObjDiffRec::ACT_COPY_AB)) {
        GenPatch_CopyAB(rec, patch_b);
      }
    }
    else if(rec->IsAnotB()) {
      if(rec->HasDiffFlag(ObjDiffRec::ACT_DEL_A)) {
        GenPatch_DelA(rec, patch_a);
      }
      if(rec->HasDiffFlag(ObjDiffRec::ACT_ADD_A)) {
        GenPatch_AddA(rec, patch_b, prv_rec);
      }
      if(rec->HasDiffFlag(ObjDiffRec::ACT_DEL_B) ||
         rec->HasDiffFlag(ObjDiffRec::ACT_ADD_B)) {
        taMisc::Warning("Incorrect action flags for AnotB difference:",
                        rec->GetDisplayName());
      }
    }
    else if(rec->IsBnotA()) {
      if(rec->HasDiffFlag(ObjDiffRec::ACT_DEL_B)) {
        GenPatch_DelB(rec, patch_b);
      }
      if(rec->HasDiffFlag(ObjDiffRec::ACT_ADD_B)) {
        GenPatch_AddB(rec, patch_a, prv_rec);
      }
      if(rec->HasDiffFlag(ObjDiffRec::ACT_DEL_A) ||
         rec->HasDiffFlag(ObjDiffRec::ACT_ADD_A)) {
        taMisc::Warning("Incorrect action flags for BnotA difference:",
                        rec->GetDisplayName());
      }
    }
    prv_rec = rec;
  }

  if(patch_a && patch_a->patch_recs.leaves == 0) {
    patch_a->Close();
    patch_a = NULL;
  }
  if(patch_b && patch_b->patch_recs.leaves == 0) {
    patch_b->Close();
    patch_b = NULL;
  }

  if(patch_a) {
    if(proj_diff) {
      ReorderProjSubgps(patch_a);
    }
    patch_a->SigEmitUpdated();
    if(modify_a) {
      patch_a->ApplyPatch(proj_a);
    }
    else {
      tabMisc::DelayedFunCall_gui(patch_a, "BrowserSelectMe");
    }
  }

  if(patch_b) {
    if(proj_diff) {
      ReorderProjSubgps(patch_b);
    }
    patch_b->SigEmitUpdated();
    if(modify_b) {
      patch_b->ApplyPatch(proj_b);
    }
    else {
      tabMisc::DelayedFunCall_gui(patch_b, "BrowserSelectMe");
    }
  }
  
  return n_patches;
}

