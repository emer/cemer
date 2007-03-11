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


#include "ta_seledit.h"

#include "ta_type.h"
#include "ta_project.h"
#include "ta_qt.h"


//////////////////////////////////
//	SelectEditConfig	//
//////////////////////////////////

void SelectEditConfig::Initialize() {
  auto_edit = true;
}

void SelectEditConfig::Destroy() {
}

void SelectEditConfig::InitLinks() {
  taBase::InitLinks();
  taBase::Own(mbr_labels, this);
  taBase::Own(meth_labels, this);
}

void SelectEditConfig::Copy_(const SelectEditConfig& cp) {
  auto_edit = cp.auto_edit;
  mbr_labels = cp.mbr_labels;
  meth_labels = cp.meth_labels;
}


//////////////////////////////////
//	SelectEdit	//
//////////////////////////////////

void SelectEdit::BaseClosingAll(taBase* obj) {
  TokenSpace& ts = TA_SelectEdit.tokens;
  for (int i = ts.size - 1; i >= 0; --i) {
    SelectEdit* se = (SelectEdit*)ts.FastEl(i);
    se->BaseClosing(obj);
  }
}

void SelectEdit::BaseDataChangedAll(taBase* obj, int dcr, void* op1, void* op2) {
  TokenSpace& ts = TA_SelectEdit.tokens;
  for (int i = ts.size - 1; i >= 0; --i) {
    SelectEdit* se = (SelectEdit*)ts.FastEl(i);
    se->BaseDataChanged(obj, dcr, op1, op2);
  }
}

void SelectEdit::Initialize() {
//nn  edit_on_reopen = false;
//  mbr_bases.SetBaseType(&TA_taBase);
}

void SelectEdit::InitLinks() {
  taNBase::InitLinks();

  taBase::Own(config, this);

  taBase::Own(mbr_bases, this);
  taBase::Own(mbr_strs, this);
  taBase::Own(mbr_base_paths, this);

  taBase::Own(meth_bases, this);
  taBase::Own(meth_strs, this);
  taBase::Own(meth_base_paths, this);
}

void SelectEdit::Copy_(const SelectEdit& cp) {
  config = cp.config;

  mbr_bases.Borrow(cp.mbr_bases);
  members = cp.members;
  mbr_strs.Copy(cp.mbr_strs);

  meth_bases.Borrow(cp.meth_bases);
  methods = cp.methods;
  meth_strs.Copy(cp.meth_strs);
}

void SelectEdit::UpdateAfterEdit() {
//  taNBase::UpdateAfterEdit(); // prob shouldn't do this, because it makes the dialog go Apply/Revert
  if((mbr_base_paths.size > 0) || (meth_base_paths.size > 0)) {
    BaseChangeReShow();		// must have been saved, so reopen it!
  }
  config.mbr_labels.SetSize(mbr_bases.size);
  config.meth_labels.SetSize(meth_bases.size);
  mbr_strs.SetSize(mbr_bases.size);
  meth_strs.SetSize(meth_bases.size);
  if(taMisc::is_loading) {
    GetMembsFmStrs();
    GetMethsFmStrs();
  }
  ReShowEdit(true); //forced
}

void SelectEdit::BaseAdded(taBase* ta) {
//TODO???
}

void SelectEdit::BaseRemoved(taBase* ta) {
  int i = mbr_bases.FindEl(ta);
  if (i >= 0) return;
  i = meth_bases.FindEl(ta);
  if (i >= 0) return;
  //TODO ??
}
 
bool SelectEdit::BaseClosing(taBase* base) {
  bool gotone = false;
  int i;
  for(i=mbr_bases.size-1;i>=0;i--) {
    taBase* bs = mbr_bases.FastEl(i);
    char* staddr = (char*)bs;
    char* endaddr=staddr+bs->GetSize();
    char* vbase = (char*)base;
    if((vbase >= staddr) && (vbase <= endaddr)) {
      RemoveField_impl(i);
      gotone = true;
    }
  }

  for(i=meth_bases.size-1;i>=0;i--) {
    taBase* bs = meth_bases.FastEl(i);
    if(bs == base) {
      RemoveFun_impl(i);
      gotone = true;
    }
  }
  if (gotone) ReShowEdit(true);
  return gotone;
}

bool SelectEdit::BaseDataChanged(taBase* base,
    int dcr, void* op1_, void* op2_) 
{
  bool rval = false;
  int i;
  for(i=mbr_bases.size-1;i>=0;i--) {
    taBase* bs = mbr_bases.FastEl(i);
    char* staddr = (char*)bs;
    char* endaddr=staddr+bs->GetSize();
    char* vbase = (char*)base;
    if((vbase >= staddr) && (vbase <= endaddr)) {
      rval = true;
      break;
    }
  }

  if (!rval) for(i=meth_bases.size-1;i>=0;i--) {
    taBase* bs = meth_bases.FastEl(i);
    if(bs == base) {
      rval = true;
      break;
    }
  }
  if (rval) DataChanged(DCR_ITEM_UPDATED, NULL, NULL);

  return rval;
}


void SelectEdit::BaseChangeSave() {
//nn  if((mbr_bases.size > 0) || (meth_bases.size > 0))
//    edit_on_reopen = CloseEdit();

  GetAllPaths();
  if(mbr_bases.size > 0)
    mbr_bases.Reset();
  if(meth_bases.size > 0)
    meth_bases.Reset();
}

void SelectEdit::BaseChangeReShow() {
  if((mbr_base_paths.size == 0) && (meth_base_paths.size == 0)) return;

  if(mbr_base_paths.size > 0) {
    mbr_bases.Reset();		// get rid of the mbr_bases!
    int i;
    for(i=0;i<mbr_base_paths.size;i++) {
      String path = mbr_base_paths.FastEl(i);
      taBase* bs = tabMisc::root->FindFromPath(path);
      if(bs == NULL) {
	taMisc::Error("SelectEdit::BaseChangeReOpen: could not find object from path:",path);
	members.RemoveIdx(i);  mbr_strs.RemoveIdx(i);  config.mbr_labels.RemoveIdx(i);  mbr_base_paths.RemoveIdx(i);
	i--;
	continue;
      }
      mbr_bases.Link(bs);
    }
    mbr_base_paths.Reset();
  }

  if(meth_base_paths.size > 0) {
    meth_bases.Reset();		// get rid of the meth_bases!
    int i;
    for(i=0;i<meth_base_paths.size;i++) {
      String path = meth_base_paths.FastEl(i);
      taBase* bs = tabMisc::root->FindFromPath(path);
      if(bs == NULL) {
	taMisc::Error("SelectEdit::BaseChangeReOpen: could not find object from path:",path);
	methods.RemoveIdx(i);  meth_strs.RemoveIdx(i);  config.meth_labels.RemoveIdx(i);  meth_base_paths.RemoveIdx(i);
	i--;
	continue;
      }
      meth_bases.Link(bs);
    }
    meth_base_paths.Reset();
  }

  ReShowEdit(true); //forced
}

int SelectEdit::Dump_Load_Value(istream& strm, taBase* par) {
  members.Reset();
  mbr_bases.Reset();
  mbr_strs.Reset();
  config.mbr_labels.Reset();

  methods.Reset();
  meth_bases.Reset();
  meth_strs.Reset();
  config.meth_labels.Reset();

  return taNBase::Dump_Load_Value(strm, par);
  ReShowEdit(true); //forced
}

void SelectEdit::GetAllPaths() {
  if(mbr_bases.size > 0) {
    mbr_base_paths.Reset();
    for(int i=0;i<mbr_bases.size;i++) {
      mbr_base_paths.Add(mbr_bases.FastEl(i)->GetPath());
    }
  }
  if(meth_bases.size > 0) {
    meth_base_paths.Reset();
    int i;
    for(i=0;i<meth_bases.size;i++) {
      meth_base_paths.Add(meth_bases.FastEl(i)->GetPath());
    }
  }
}

int SelectEdit::Dump_Save_Value(ostream& strm, taBase* par, int indent) {
  return taNBase::Dump_Save_Value(strm, par, indent);
}

void SelectEdit::GetMembsFmStrs() {
  int i;
  for(i=0;i<mbr_bases.size;i++) {
    taBase* bs = mbr_bases.FastEl(i);
    if(bs == NULL) { // didn't get loaded, bail..
      taMisc::Warning("*** SelectEdit: couldn't find object:", config.mbr_labels[i], mbr_strs[i], "in object to edit");
      mbr_bases.RemoveIdx(i);      mbr_strs.RemoveIdx(i);      config.mbr_labels.RemoveIdx(i);
      i--;
      continue;
    }
    String nm = mbr_strs.FastEl(i);
    MemberDef* md = bs->FindMember((const char*)nm);
    if(md == NULL) {
      taMisc::Warning("*** SelectEdit: couldn't find member:", nm, "in object to edit:",bs->GetPath());
      mbr_bases.RemoveIdx(i);      mbr_strs.RemoveIdx(i);      config.mbr_labels.RemoveIdx(i);
      i--;
      continue;
    }
    members.Add(md);
  }
}

void SelectEdit::GetMethsFmStrs() {
  int i;
  for(i=0;i<meth_bases.size;i++) {
    taBase* bs = meth_bases.FastEl(i);
    if(bs == NULL) { // didn't get loaded, bail..
      taMisc::Warning("*** SelectEdit: couldn't find object:", config.meth_labels[i], meth_strs[i], "in object to edit");
      meth_bases.RemoveIdx(i);      meth_strs.RemoveIdx(i);      config.meth_labels.RemoveIdx(i);
      i--;
      continue;
    }
    String nm = meth_strs.FastEl(i);
    MethodDef* md = bs->GetTypeDef()->methods.FindName((const char*)nm);
    if(md == NULL) {
      taMisc::Warning("*** SelectEdit: couldn't find method:", nm, "in object to edit:",bs->GetPath());
      meth_bases.RemoveIdx(i);      meth_strs.RemoveIdx(i);      config.meth_labels.RemoveIdx(i);
      i--;
      continue;
    }
    methods.Add(md);
  }
}

int SelectEdit::FindMbrBase(taBase* base, MemberDef* md) {
  int i;
  for(i=0;i<mbr_bases.size;i++) {
    if((mbr_bases.FastEl(i) == base) && (members.FastEl(i) == md))
      return i;
  }
  return -1;
}

bool SelectEdit::SelectMember_impl(taBase* base, MemberDef* md, const char* lbl) {
  bool rval = false;
  int bidx = FindMbrBase(base, md);
  if (bidx >= 0) {
    config.mbr_labels[bidx] = lbl;
  } else {
    mbr_bases.Link(base);
    members.Add(md);
    mbr_strs.Add(md->name);
    config.mbr_labels.Add(lbl);
    BaseAdded(base);
    rval = true;
  }
  return rval;
}

bool SelectEdit::SelectMember(taBase* base, MemberDef* md, const char* lbl) {
  bool rval = SelectMember_impl(base, md, lbl);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMemberNm(taBase* base, const char* md, const char* lbl) {
  if(base == NULL) return false;
  MemberDef* mda = (MemberDef*)base->FindMember(md);
  if(mda == NULL) return false;
  return SelectMember(base, mda, lbl);
}

int SelectEdit::FindMethBase(taBase* base, MethodDef* md) {
  int i;
  for(i=0;i<meth_bases.size;i++) {
    if((meth_bases.FastEl(i) == base) && (methods.FastEl(i) == md))
      return i;
  }
  return -1;
}

bool SelectEdit::SelectMethod_impl(taBase* base, MethodDef* md, const char* lbl) {
  bool rval = false;
  int bidx = FindMethBase(base, md);
  if (bidx >= 0) {
    config.meth_labels[bidx] = lbl;
  } else {
    meth_bases.Link(base);
    methods.Add(md);
    meth_strs.Add(md->name);
    config.meth_labels.Add(lbl);
    BaseAdded(base);
    rval = true;
  }
  return rval;
}

bool SelectEdit::SelectMethod(taBase* base, MethodDef* md, const char* lbl) {
  bool rval = SelectMethod_impl(base, md, lbl);
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMethodNm(taBase* base, const char* md, const char* lbl) {
  if(base == NULL) return false;
  MethodDef* mda = (MethodDef*)base->GetTypeDef()->methods.FindName(md);
  if(mda == NULL) return false;
  return SelectMethod(base, mda, lbl);
}

void SelectEdit::UpdateAllBases() {
  int i;
  for(i=0;i<mbr_bases.size;i++) {
    taBase* bs = mbr_bases.FastEl(i);
    if(bs == NULL) continue;
    bs->UpdateAfterEdit();
    taiMisc::Update(bs);
  }
  for(i=0;i<meth_bases.size;i++) {
    taBase* bs = meth_bases.FastEl(i);
    if(bs == NULL) continue;
    bs->UpdateAfterEdit();
  }
}

void SelectEdit::RemoveField_impl(int idx) {
  taBase* base = mbr_bases.FastEl(idx);
  mbr_bases.RemoveIdx(idx);
  members.RemoveIdx(idx);
  mbr_strs.RemoveIdx(idx);
  config.mbr_labels.RemoveIdx(idx);
  BaseRemoved(base);
}

void SelectEdit::RemoveField(int idx) {
  RemoveField_impl(idx);
  ReShowEdit(true); //forced
}

void SelectEdit::MoveField(int from, int to) {
  mbr_bases.MoveIdx(from, to);  members.MoveIdx(from, to);  mbr_strs.MoveIdx(from, to);  config.mbr_labels.MoveIdx(from, to);
  ReShowEdit(true); //forced
}

void SelectEdit::RemoveFun_impl(int idx) {
  taBase* base = mbr_bases.FastEl(idx);
  meth_bases.RemoveIdx(idx);
  methods.RemoveIdx(idx);
  meth_strs.RemoveIdx(idx);
  config.meth_labels.RemoveIdx(idx);
  BaseRemoved(base);
}

void SelectEdit::RemoveFun(int idx) {
  RemoveFun_impl(idx);
  ReShowEdit(true); //forced
}

void SelectEdit::MoveFun(int from, int to) {
  meth_bases.MoveIdx(from, to);  methods.MoveIdx(from, to);  meth_strs.MoveIdx(from, to);  config.meth_labels.MoveIdx(from, to);
  ReShowEdit(true); //forced
}

void SelectEdit::NewEdit() {
  DataChanged(DCR_ITEM_REBUILT);
/*obs  CloseEdit();
  Edit(); */
}

int SelectEdit::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int nchg = 0;
  // NOTE: we update notify list by just doing a series of Remove/Add
  for(int j=mbr_bases.size-1; j>=0; j--) {
    taBase* itm = mbr_bases[j];
    taBase* old_own = itm->GetOwner(old_par->GetTypeDef());
    if(old_own != old_par) continue;
    String old_path = itm->GetPath(NULL, old_par);
    taBase* nitm = new_par->FindFromPath(old_path);
    if(nitm != NULL) {
      mbr_bases.ReplaceLinkIdx(j, nitm);
      BaseRemoved(itm);
      BaseAdded(nitm);
      nchg++;
    }
    else {
      RemoveField_impl(j);	// this is why this is diff from std case!
    }
  }
  for(int j=meth_bases.size-1; j>=0; j--) {
    taBase* itm = meth_bases[j];
    String old_path = itm->GetPath(NULL, old_par);
    taBase* nitm = new_par->FindFromPath(old_path);
    if(nitm != NULL) {
      meth_bases.ReplaceLinkIdx(j, nitm);
      BaseRemoved(itm);
      BaseAdded(nitm);
      nchg++;
    }
    else {
      RemoveFun_impl(j);	// this is why this is diff from std case!
    }
  }
  return nchg;
}

String SelectEdit::GetMbrLabel(int i) {
  String lbl;
  if(config.mbr_labels.size > i)
    lbl = config.mbr_labels.FastEl(i);
  String nm = String(i) + ": " + lbl;
  if(!lbl.empty()) nm += " ";
  nm += members.FastEl(i)->GetLabel();
  return nm;
}

String SelectEdit::GetMethLabel(int i) {
  String lbl;
  if(config.meth_labels.size > i)
    lbl = config.meth_labels.FastEl(i);
  String nm = String(i) + ": " + lbl;
  if(!lbl.empty()) nm += " ";
  nm += methods.FastEl(i)->GetLabel();
  return nm;
}

int SelectEdit::SearchMembers(taNBase* obj, const String& memb_contains) {
  if(TestError(!obj || memb_contains.empty(), "SearchMembers", 
	       "null object or empty search")) return -1;
  SelectEdit* se = this;
  return obj->SelectForEditSearch(memb_contains, se);
}

int SelectEdit::CompareObjs(taBase* obj_a, taBase* obj_b) {
  if(TestError(!obj_a || !obj_b, "CompareObjs", "null object(s)")) return -1;
  if(TestError(obj_a->GetTypeDef() != obj_b->GetTypeDef(), "CompareObjs",
	       "objects must have the exact same type to be able to be compared")) return -1;
  Member_List mds;
  void_PArray trg_bases;
  void_PArray src_bases;
  obj_a->CompareSameTypeR(mds, trg_bases, src_bases, obj_b);
  for(int i=0;i<mds.size;i++) {
    taBase* itma = (taBase*)trg_bases[i];
    taBase* itmb = (taBase*)src_bases[i];
    String nma = "A: " + itma->GetDisplayName().elidedTo(20);
    String nmb = "B: " + itmb->GetDisplayName().elidedTo(20);
    SelectMember_impl(itma, mds[i], nma);
    SelectMember_impl(itmb, mds[i], nmb);
  }
  return mds.size;
}
