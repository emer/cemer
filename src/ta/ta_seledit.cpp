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
#include "ta_qt.h"
#include "ta_TA_type.h"

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

void SelectEdit::Initialize() {
//nn  edit_on_reopen = false;
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
  members.Borrow(cp.members);
  mbr_strs.Copy(cp.mbr_strs);

  meth_bases.Borrow(cp.meth_bases);
  methods.Borrow(cp.methods);
  meth_strs.Copy(cp.meth_strs);
}

void SelectEdit::UpdateAfterEdit() {
//  taNBase::UpdateAfterEdit(); // prob shouldn't do this, because it makes the dialog go Apply/Revert
  if((mbr_base_paths.size > 0) || (meth_base_paths.size > 0)) {
    BaseChangeReShow();		// must have been saved, so reopen it!
  }
  config.mbr_labels.EnforceSize(mbr_bases.size);
  config.meth_labels.EnforceSize(meth_bases.size);
  mbr_strs.EnforceSize(mbr_bases.size);
  meth_strs.EnforceSize(meth_bases.size);
  if(taMisc::is_loading) {
    GetMembsFmStrs();
    GetMethsFmStrs();
  }
  ReShowEdit(true); //forced
}

int SelectEdit::Dump_Load_Value(istream& strm, TAPtr par) {
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

int SelectEdit::Dump_Save_Value(ostream& strm, TAPtr par, int indent) {
  return taNBase::Dump_Save_Value(strm, par, indent);
}

void SelectEdit::GetMembsFmStrs() {
  int i;
  for(i=0;i<mbr_bases.size;i++) {
    TAPtr bs = mbr_bases.FastEl(i);
    if(bs == NULL) { // didn't get loaded, bail..
      taMisc::Error("*** SelectEdit: couldn't find object:", config.mbr_labels[i], mbr_strs[i], "in object to edit");
      mbr_bases.Remove(i);      mbr_strs.Remove(i);      config.mbr_labels.Remove(i);
      i--;
      continue;
    }
    String nm = mbr_strs.FastEl(i);
    MemberDef* md = bs->FindMember((const char*)nm);
    if(md == NULL) {
      taMisc::Error("*** SelectEdit: couldn't find member:", nm, "in object to edit:",bs->GetPath());
      mbr_bases.Remove(i);      mbr_strs.Remove(i);      config.mbr_labels.Remove(i);
      i--;
      continue;
    }
    members.Link(md);
  }
}

void SelectEdit::GetMethsFmStrs() {
  int i;
  for(i=0;i<meth_bases.size;i++) {
    TAPtr bs = meth_bases.FastEl(i);
    if(bs == NULL) { // didn't get loaded, bail..
      taMisc::Error("*** SelectEdit: couldn't find object:", config.meth_labels[i], meth_strs[i], "in object to edit");
      meth_bases.Remove(i);      meth_strs.Remove(i);      config.meth_labels.Remove(i);
      i--;
      continue;
    }
    String nm = meth_strs.FastEl(i);
    MethodDef* md = bs->GetTypeDef()->methods.FindName((const char*)nm);
    if(md == NULL) {
      taMisc::Error("*** SelectEdit: couldn't find method:", nm, "in object to edit:",bs->GetPath());
      meth_bases.Remove(i);      meth_strs.Remove(i);      config.meth_labels.Remove(i);
      i--;
      continue;
    }
    methods.Link(md);
  }
}

int SelectEdit::FindMbrBase(TAPtr base, MemberDef* md) {
  int i;
  for(i=0;i<mbr_bases.size;i++) {
    if((mbr_bases.FastEl(i) == base) && (members.FastEl(i) == md))
      return i;
  }
  return -1;
}

bool SelectEdit::SelectMember(TAPtr base, MemberDef* md, const char* lbl) {
  bool rval = false;
  int bidx = FindMbrBase(base, md);
  if (bidx >= 0) {
    config.mbr_labels[bidx] = lbl;
  } else {
    mbr_bases.Link(base);
    members.Link(md);
    mbr_strs.Add(md->name);
    config.mbr_labels.Add(lbl);
    rval = true;
  }
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMemberNm(TAPtr base, const char* md, const char* lbl) {
  if(base == NULL) return false;
  MemberDef* mda = (MemberDef*)base->FindMember(md);
  if(mda == NULL) return false;
  return SelectMember(base, mda, lbl);
}

int SelectEdit::FindMethBase(TAPtr base, MethodDef* md) {
  int i;
  for(i=0;i<meth_bases.size;i++) {
    if((meth_bases.FastEl(i) == base) && (methods.FastEl(i) == md))
      return i;
  }
  return -1;
}

bool SelectEdit::SelectMethod(TAPtr base, MethodDef* md, const char* lbl) {
  bool rval = false;
  int bidx = FindMethBase(base, md);
  if (bidx >= 0) {
    config.meth_labels[bidx] = lbl;
  } else {
    meth_bases.Link(base);
    methods.Link(md);
    meth_strs.Add(md->name);
    config.meth_labels.Add(lbl);
    rval = true;
  }
  ReShowEdit(true); //forced
  return rval;
}

bool SelectEdit::SelectMethodNm(TAPtr base, const char* md, const char* lbl) {
  if(base == NULL) return false;
  MethodDef* mda = (MethodDef*)base->GetTypeDef()->methods.FindName(md);
  if(mda == NULL) return false;
  return SelectMethod(base, mda, lbl);
}

void SelectEdit::UpdateAllBases() {
  int i;
  for(i=0;i<mbr_bases.size;i++) {
    TAPtr bs = mbr_bases.FastEl(i);
    if(bs == NULL) continue;
    bs->UpdateAfterEdit();
    taiMisc::Update(bs);
  }
  for(i=0;i<meth_bases.size;i++) {
    TAPtr bs = meth_bases.FastEl(i);
    if(bs == NULL) continue;
    bs->UpdateAfterEdit();
  }
}

void SelectEdit::RemoveField(int idx) {
  mbr_bases.Remove(idx);  members.Remove(idx);  mbr_strs.Remove(idx);  config.mbr_labels.Remove(idx);
  ReShowEdit(true); //forced
}

void SelectEdit::MoveField(int from, int to) {
  mbr_bases.Move(from, to);  members.Move(from, to);  mbr_strs.Move(from, to);  config.mbr_labels.Move(from, to);
  ReShowEdit(true); //forced
}

void SelectEdit::RemoveFun(int idx) {
  meth_bases.Remove(idx);  methods.Remove(idx);  meth_strs.Remove(idx);  config.meth_labels.Remove(idx);
  ReShowEdit(true); //forced
}

void SelectEdit::MoveFun(int from, int to) {
  meth_bases.Move(from, to);  methods.Move(from, to);  meth_strs.Move(from, to);  config.meth_labels.Move(from, to);
  ReShowEdit(true); //forced
}

void SelectEdit::NewEdit() {
  DataChanged(DCR_ITEM_REBUILT);
/*obs  CloseEdit();
  Edit(); */
}

bool SelectEdit::BaseClosing(TAPtr base) {
  bool gotone = false;
  int i;
  for(i=mbr_bases.size-1;i>=0;i--) {
    TAPtr bs = mbr_bases.FastEl(i);
    char* staddr = (char*)bs;
    char* endaddr=staddr+bs->GetSize();
    char* vbase = (char*)base;
    if((vbase >= staddr) && (vbase <= endaddr)) {
      mbr_bases.Remove(i);  members.Remove(i);  mbr_strs.Remove(i);  config.mbr_labels.Remove(i);
      gotone = true;
    }
  }

  for(i=meth_bases.size-1;i>=0;i--) {
    TAPtr bs = meth_bases.FastEl(i);
    if(bs == base) {
      meth_bases.Remove(i);  methods.Remove(i);  meth_strs.Remove(i);  config.meth_labels.Remove(i);
      gotone = true;
    }
  }

  return gotone;
}

bool SelectEdit::BaseClosingAll(TAPtr base) {
  bool got_one = false;
  int i;
  for(i=0;i<TA_SelectEdit.tokens.size;i++) {
    SelectEdit* se = (SelectEdit*)TA_SelectEdit.tokens.FastEl(i);
    if(se->BaseClosing(base))
      got_one = true;
  }
  return got_one;
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
      TAPtr bs = tabMisc::root->FindFromPath(path);
      if(bs == NULL) {
	taMisc::Error("SelectEdit::BaseChangeReOpen: could not find object from path:",path);
	members.Remove(i);  mbr_strs.Remove(i);  config.mbr_labels.Remove(i);  mbr_base_paths.Remove(i);
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
      TAPtr bs = tabMisc::root->FindFromPath(path);
      if(bs == NULL) {
	taMisc::Error("SelectEdit::BaseChangeReOpen: could not find object from path:",path);
	methods.Remove(i);  meth_strs.Remove(i);  config.meth_labels.Remove(i);  meth_base_paths.Remove(i);
	i--;
	continue;
      }
      meth_bases.Link(bs);
    }
    meth_base_paths.Reset();
  }

  ReShowEdit(true); //forced
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
