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

#include "taList_impl.h"

#include <taSigLink>
#include <int_Matrix>
#include <String_Matrix>
#include <byte_Matrix>
#include <Variant_Matrix>
#include <taBaseItr>
#include <taBase_Group>
#include <tabMisc>
#include <taRootBase>
#include <taObjDiffRec>
#include <taObjDiff_List>
#include <dumpMisc>
#include <MemberDef>
#include <iTreeViewItem>

#include <SigLinkSignal>
#include <taMisc>


#ifdef TA_GUI
#include  <QPixmap>

TA_BASEFUNS_CTORS_DEFN(taList_impl);

static const char* folder_closed_xpm[]={
    "16 16 9 1",
    "g c #808080",
    "b c #c0c000",
    "e c #c0c0c0",
    "# c #000000",
    "c c #ffff00",
    ". c None",
    "a c #585858",
    "f c #a0a0a4",
    "d c #ffffff",
    "..###...........",
    ".#abc##.........",
    ".#daabc#####....",
    ".#ddeaabbccc#...",
    ".#dedeeabbbba...",
    ".#edeeeeaaaab#..",
    ".#deeeeeeefe#ba.",
    ".#eeeeeeefef#ba.",
    ".#eeeeeefeff#ba.",
    ".#eeeeefefff#ba.",
    ".##geefeffff#ba.",
    "...##gefffff#ba.",
    ".....##fffff#ba.",
    ".......##fff#b##",
    ".........##f#b##",
    "...........####."};

static const char* folder_open_xpm[]={
    "16 16 11 1",
    "# c #000000",
    "g c #c0c0c0",
    "e c #303030",
    "a c #ffa858",
    "b c #808080",
    "d c #a0a0a4",
    "f c #585858",
    "c c #ffdca8",
    "h c #dcdcdc",
    "i c #ffffff",
    ". c None",
    "....###.........",
    "....#ab##.......",
    "....#acab####...",
    "###.#acccccca#..",
    "#ddefaaaccccca#.",
    "#bdddbaaaacccab#",
    ".eddddbbaaaacab#",
    ".#bddggdbbaaaab#",
    "..edgdggggbbaab#",
    "..#bgggghghdaab#",
    "...ebhggghicfab#",
    "....#edhhiiidab#",
    "......#egiiicfb#",
    "........#egiibb#",
    "..........#egib#",
    "............#ee#"};

//NOTE: can't create these statically, prior to the QApplication creation, so we must do it this way...
const QPixmap* folder_closed_pixmap() {
  static const QPixmap* pm = NULL;
  if  (pm == NULL)
    pm = new QPixmap(folder_closed_xpm);
  return pm;
}

const QPixmap* folder_open_pixmap() {
  static const QPixmap* pm = NULL;
  if  (pm == NULL)
    pm = new QPixmap(folder_open_xpm);
  return pm;
}

#endif // def TA_GUI

using namespace std;

void taList_impl::Initialize() {
  SetBaseType(&TA_taBase);
  el_def = 0;
  m_trg_load_size = -1;
  el_view_mode = IDX_UNK;
}


void taList_impl::Destroy() {
  CutLinks();
}

void taList_impl::CutLinks() {
  if (m_sig_link) m_sig_link->SigDestroying(); // do early
  RemoveAll();
  el_view.CutLinks();
  el_view_mode = IDX_UNK;
  inherited_taBase::CutLinks();
}

void taList_impl::CanCopy_impl(const taBase* cp_fm_, bool quiet,
  bool& ok, bool virt) const
{
  if (virt) {
    inherited::CanCopy_impl(cp_fm_, quiet, ok, virt);
    if (!ok) return; // no reason to continue, and could be bad to do so
  }
  const taList_impl* cp_fm = (const taList_impl*)cp_fm_; // is safe
  // we only allow list copies when the base types are the same,
  // since otherwise, we must be dealing with lists for different
  // purposes, and they cannot be considered assignable
  if (CheckError((el_base != cp_fm->el_base), quiet, ok,
    "Copy: Lists must have same el_base to be copyable")) return;
}

void taList_impl::Copy_(const taList_impl& cp) {
//old  if(!cp.name.empty()) name = cp.name;
//no    name = cp.name;
  el_base = cp.el_base;
  el_typ = cp.el_typ;
  el_def = cp.el_def;
  el_view = cp.el_view;
  el_view_mode = cp.el_view_mode;
  taPtrList_impl::Copy_Exact(cp);
}

void* taList_impl::El_CopyN_(void* to_, void* fm) {
  taBase* to = (taBase*)to_;
  to->StructUpdate(true);
  void* rval = El_Copy_(to_, fm);
  El_SetName_(to_, El_GetName_(fm));
  to->StructUpdate(false);
  return rval;
}

void taList_impl::UpdateAfterEdit(){
  inherited_taBase::UpdateAfterEdit();
  if(!el_typ->InheritsFrom(el_base)) el_typ = el_base;
  if(taMisc::is_loading) {
    taVersion v512(5, 1, 2);
    if(taMisc::loading_version < v512) { // enforce unique names prior to 5.1.2
      MakeElNamesUnique();
    }
  }
  else {
    MakeElNamesUnique();
  }
}

bool taList_impl::MakeElNamesUnique() {
  static bool in_process = false;
  if(size == 0) return true;
  if(el_base->GetInstance()) {
    if(!((taBase*)el_base->GetInstance())->HasName()) return true; // no names!
  }
  if(HasOption("NO_UNIQUE_NAMES")) return true;        // not this guy
  if(in_process) return true; // already in this function -- SetName calls this recursively so don't allow that to happen.. I know, it's ugly, but not worth adding whole new SetName interface..
  in_process = true;
  bool unique = true;
  String mynm = name;
  if(mynm.empty()) {
    if(GetOwner() != NULL) {
      mynm = GetOwner()->GetName();
      if(mynm.empty()) {
        mynm = GetPathNames();
      }
    }
  }
  mynm = taMisc::StringCVar(mynm); // make it legal

  for(int i=0; i<size; i++) {
    taBase* el1 = (taBase*)FastEl_(i);
    if (!el1 || (el1->GetOwner() != this)) continue;
    String nm1 = el1->GetName();
    if(nm1.empty()) {
      nm1 = mynm + "_" + (String)i; // give it a unique name
      el1->SetName(nm1);
      el1->UpdateAfterEdit();   // trigger update so visible
    }
    for(int j=i+1; j<size; j++) {
      taBase* el2 = (taBase*)FastEl_(j);
      if (!el2 || (el2->GetOwner() != this)) continue;
      String nm2 = el2->GetName();
      if(nm2.empty()) {
        nm2 = mynm + "_" + (String)j; // give it a unique name
        el2->SetName(nm2);
        el2->UpdateAfterEdit(); // trigger update so visible
      }
      if(nm2 == nm1) {
        String orig = nm2;
        nm2 = nm2 + "_" + (String)j;
        taMisc::Warning("taList_impl::MakeElNamesUnique",
                "names of items on the list must be unique -- renaming:",el2->GetPathNames(),"to:",nm2);
        el2->SetName(nm2);
        el2->UpdateAfterEdit(); // trigger update so visible
        unique = false;
      }
    }
  }
  in_process = false;
  return unique;
}

bool taList_impl::MakeElNameUnique(taBase* itm) {
  static bool in_process = false;
  if (!itm || !itm->HasName()) return true; // only if el's actually have names
  if(HasOption("NO_UNIQUE_NAMES")) return true;        // not this guy
  if(in_process) return true; // already in this function -- SetName calls this recursively so don't allow that to happen.. I know, it's ugly, but not worth adding whole new SetName interface..
  in_process = true;
  bool unique = true;
  String mynm = name;
  if(mynm.empty()) {
    if(GetOwner() != NULL) {
      mynm = GetOwner()->GetName();
      if(mynm.empty()) {
        mynm = GetPathNames();
      }
    }
  }
  mynm = taMisc::StringCVar(mynm); // make it legal

  String itmnm = itm->GetName();
  if(itmnm.empty()) {
    itmnm = mynm + "_" + (String)FindEl_(itm); // give it a unique name
    itm->SetName(itmnm);
    itm->UpdateAfterEdit();     // trigger update so visible
  }

  for(int i=0; i<size; i++) {
    taBase* el1 = (taBase*)FastEl_(i);
    if (!el1 || (el1->GetOwner() != this)) continue;
    if(el1 == itm) continue;
    String nm1 = el1->GetName();
    if(itmnm == nm1) {
      String orig = itmnm;
      itmnm = itmnm + "_" + (String)FindEl_(itm);
      taMisc::Warning("taList_impl::MakeElNameUnique",
                      "names of items on the list must be unique -- renaming:",itm->GetPathNames(),"to:",itmnm);
      itm->SetName(itmnm);
      itm->UpdateAfterEdit();   // trigger update so visible
      unique = false;
    }
  }
  in_process = false;
  return unique;
}

void taList_impl::CheckChildConfig_impl(bool quiet, bool& rval) {
  MakeElNamesUnique();
  //note: we have to process everyone, because this is the routine
  // that asserts or clears the state, even if an invalid found early
  for (int i = 0; i < size; ++i) {
    taBase* child = (taBase*)FastEl_(i);
    // we only include owned items, not linked
    if (!child || (child->GetOwner() != this))
      continue;
    child->CheckConfig(quiet, rval);
  }
}


void taList_impl::ChildUpdateAfterEdit(taBase* child, bool& handled) {
  inherited_taBase::ChildUpdateAfterEdit(child, handled);
  // otherwise, we assume it is an owned list member
  if (!handled) {
    SigEmit(SLS_LIST_ITEM_UPDATE, child);
    handled = true;
  }
}

Variant taList_impl::VarEl(int idx) const {
  taBase* tab = ElemLeaf(idx);
  if(tab && tab->InheritsFrom(&TA_taMatrix))
    return (Variant)((taMatrix*)tab);
  return (Variant)tab;
}

void taList_impl::LinkCopyLeaves(const taList_impl& cp) {
  Reset();
  if(!Alloc(cp.ElemCount())) return;
  // just want to make everything flat and remove all substructure, so we do that..
  for(int i=0; i < cp.ElemCount(); i++)
    Link_(cp.ElemLeaf(i));
}

bool taList_impl::SetElView(taMatrix* view_mat, IndexMode md) {
  if(!IterValidate(view_mat, md, 1)) return false;
  el_view = view_mat;
  el_view_mode = md;
  return true;
}

taList_impl* taList_impl::NewElView(taMatrix* view_mat, IndexMode md) const {
  if(!IterValidate(view_mat, md, 1)) return NULL;
  taList_impl* rval = (taList_impl*)MakeToken(); // make a token of me
  rval->LinkCopyLeaves(*this);         // make links to all my guys
  rval->SetElView(view_mat, md);
  return rval;
}

Variant taList_impl::Elem(const Variant& idx, IndexMode mode) const {
  if(mode == IDX_UNK) {
    mode = IndexModeDecode(idx, 1);
    if(mode == IDX_UNK) return _nilVariant;
  }
  if(!IndexModeValidate(idx, mode, 1))
    return _nilVariant;
  switch(mode) {
  case IDX_IDX: {
    return VarEl(idx.toInt());
    break;
  }
  case IDX_NAME: {
    const String& nm = idx.toString();
    int_Matrix* imat = new int_Matrix(1,0);
    TA_FOREACH(vitm, *this) {   // use iterator so it is recursive on existing filtering
      taBase* itm = vitm.toBase();
      if(itm && itm->GetName().matches_wildcard(nm)) {
        imat->Add(FOREACH_itr.el_idx); // add absolute index of item
      }
    }
    if(imat->size == 1) {
      int j = imat->FastEl_Flat(0);
      delete imat;
      return VarEl(j);
    }
    taList_impl* nwvw = NewElView(imat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_NAMES: {
    String_Matrix* cmat = dynamic_cast<String_Matrix*>(idx.toMatrix());
    int_Matrix* imat = new int_Matrix(1,0);
    TA_FOREACH(vitm, *this) {   // use iterator so it is recursive on existing filtering
      taBase* itm = vitm.toBase();
      if(itm) {
        int el_idx = FOREACH_itr.el_idx; // get before occluded by next iterator
        TA_FOREACH(mitm, *cmat) { // use iterator on matrix so it can be filtered too
          const String nm = mitm.toString();
          if(itm->GetName().matches_wildcard(nm)) {
            imat->Add(el_idx); // add absolute index of item
          }
        }
      }
    }
    if(imat->size == 1) {
      int j = imat->FastEl_Flat(0);
      delete imat;
      return VarEl(j);
    }
    taList_impl* nwvw = NewElView(imat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_COORD: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    return VarEl(cmat->FastEl_Flat(0));
    break;
  }
  case IDX_COORDS:
  case IDX_FRAMES: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    if(cmat->size == 1)
      return VarEl(cmat->FastEl_Flat(0));
    int_Matrix* imat = new int_Matrix(1,0);
    TA_FOREACH(mitm, *cmat) { // use iterator on matrix so it can be filtered too
      int el_idx = mitm.toInt();
      if(el_idx < 0) el_idx += ElemCount();
      if(el_idx < 0 || el_idx >= ElemCount()) continue; // skip out of range at this point
      imat->Add(el_idx);
    }
    taList_impl* nwvw = NewElView(imat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_SLICE: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    int start = cmat->FastEl_Flat(0);
    int end = cmat->FastEl_Flat(1);
    int step = cmat->FastEl_Flat(2);
    if(step == 0) step = 1;
    if(!FixSliceValsFromSize(start, end, ElemCount()))
      return _nilVariant;
    int_Matrix* imat = new int_Matrix(1,0);
    if(step > 0) {
      for(int i = start; i < end; i += step) {
        imat->Add(i);
      }
    }
    else {
      for(int i = end-1; i >= start; i += step) {
        imat->Add(i);
      }
    }
    taList_impl* nwvw = NewElView(imat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_MASK: {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(idx.toMatrix());
    if(TestError(cmat->dim(0) != ElemCount(), "Elem::IDX_MASK",
                 "index matrix dim(0):", String(cmat->dim(0)),
                 "is not size of list:", String(ElemCount())))
      return false;
    if(el_view && el_view_mode == IDX_MASK) {
      // take intersection of the existing mask
      byte_Matrix* am = (byte_Matrix*)(*cmat && *ElView());
      taBase::Ref(am);
      cmat->Copy(am);
      taBase::UnRef(am);
    }
    taList_impl* nwvw = NewElView(cmat, IDX_MASK);
    return (Variant)nwvw;
    break;
  }
  case IDX_MISC: {
    Variant_Matrix* cmat = dynamic_cast<Variant_Matrix*>(idx.toMatrix());
    if(TestError(!cmat, "Elem::IDX_MISC",
                 "index matrix is NULL or not a Variant_Matrix"))
      return false;
    int_Matrix* imat = new int_Matrix(1,0);
    TA_FOREACH(vitm, *this) {   // use iterator so it is recursive on existing filtering
      taBase* itm = vitm.toBase();
      if(itm) {
        int el_idx = FOREACH_itr.el_idx; // get before occluded by next iterator
        TA_FOREACH(mitm, *cmat) { // use iterator on matrix so it can be filtered too
          // catch all for any kind of variant
          if(mitm.isTypeDef()) {
            if(itm->InheritsFrom(mitm.toTypeDef())) {
              imat->Add(el_idx); // add absolute index of item
            }
          }
          else if(mitm.isStringType()) {
            const String nm = mitm.toString();
            if(itm->GetName().matches_wildcard(nm)) {
              imat->Add(el_idx); // add absolute index of item
            }
          }
          else if(mitm.isNumeric()) {
            int el_idx = mitm.toInt();
            if(el_idx < 0) el_idx += ElemCount();
            if(el_idx < 0 || el_idx >= ElemCount()) continue; // skip out of range at this point
            imat->Add(el_idx); // add absolute index of item
          }
        }
      }
    }
    if(imat->size == 1)
      return VarEl(imat->FastEl_Flat(0));
    taList_impl* nwvw = NewElView(imat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_UNK:
    break;
  }
  return _nilVariant;
}

Variant taList_impl::IterElem(taBaseItr& itr) const {
  if(itr.Done()) return _nilVariant;
  return VarEl(itr.el_idx);
}

String taList_impl::ChildGetColText(void* child, TypeDef* typ, const KeyString& key,
  int itm_idx) const
{
  if (child && typ && typ->IsActualTaBase())
    return ChildGetColText_impl((taBase*)child, key, itm_idx);
  else return inherited_taPtrList::ChildGetColText(child, typ, key, itm_idx);
}

String taList_impl::ChildGetColText_impl(taBase* child, const KeyString& key, int itm_idx) const {
  return child->GetColText(key, itm_idx);
}

void taList_impl::SigEmit(int sls, void* op1, void* op2) {
  //note: set stale before notifies
  if (useStale() && ((sls >= SLS_LIST_ORDER_MIN) && (sls <= SLS_LIST_ORDER_MAX)))
    setStale();
  inherited_taBase::SigEmit(sls, op1, op2);
}

String taList_impl::GetColHeading(const KeyString& key) const {
  if (key == key_name) return String("Item");
  else if (key == key_type) return String("Type");
  else if (key == key_desc) return String("Description");
  else return _nilString;
}

const KeyString taList_impl::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_name;
  case 1: return key_type;
  case 2: return key_desc;
  default: return _nilKeyString;
  }
}

String taList_impl::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_disp_name) {
    // Bug 1289: Allow iTreeViewItem::GetColText() to set the display name
    // to a prettier default value (given_name / tree_nm).
    return "";
  }
  return inherited_taBase::GetColText(key, itm_idx);
}

bool taList_impl::ChangeType(int idx, TypeDef* new_type) {
  if(TestError(!new_type, "ChangeType", "new type is null")) return false;
  if(TestError(!InRange(idx), "ChangeType", "index is out of range")) return false;
  taBase* itm = (taBase*)el[idx];
  if(TestError(!itm, "ChangeType", "item is null")) return false;
  TypeDef* itd = itm->GetTypeDef();
  if(!new_type->InheritsFrom(itd) && !itm->InheritsFrom(new_type)) {
    // do they have a common parent? if so, convert to that first, then back to new_type
    if(itd->parents.size >= 1) {
      if(new_type->InheritsFrom(itd->GetParent())) {
        ChangeType(idx, itd->GetParent());
        itm = (taBase*)el[idx];
        RemoveIdx(size-1);                      // remove the last guy!
      }
      else if((itd->GetParent()->parents.size >= 1) &&
              new_type->InheritsFrom(itd->GetParent()->GetParent())) {
        ChangeType(idx, itd->GetParent()->GetParent());
        itm = (taBase*)el[idx];
        RemoveIdx(size-1);                      // remove the last guy!
      }
      else {
        TestError(true, "Changetype", "Cannot change to new type:",new_type->name,
                  "which does not inherit from:", itd->name,
                  "(or vice-versa)",itm->GetPathNames(NULL,this));
        return false;
      }
    }
    else {
      TestError(true, "ChangeType", "Cannot change to new type:",new_type->name,
                "which does not inherit from:", itd->name,
                "(or vice-versa)",itm->GetPathNames(NULL,this));
      return false;
    }
  }
  ++taMisc::is_changing_type;
  taBase* rval = taBase::MakeToken(new_type);
  if(!TestError(!rval, "ChangeType", "maketoken is null")) {
    Add(rval);          // add to end of list
    String orgnm = itm->GetName();
    rval->UnSafeCopy(itm);      // do the copy!
    SwapIdx(idx, size-1);               // switch positions, so old guy is now at end!
    rval->SetName(orgnm);               // also copy name -- otherwise not copied
    itm->UpdatePointersToMe(rval); // allow us to update all things that might point to us
    // then do a delayed remove of this object (in case called by itself!)
    itm->CloseLater();
  }
  --taMisc::is_changing_type;
  return true;
}

bool taList_impl::ChangeType(taBase* itm, TypeDef* new_type) {
  int idx = FindEl(itm);
  if(idx >= 0)
    return ChangeType(idx, new_type);
  TestWarning(true,"ChangeType","item not found");
  return false;
}

void taList_impl::Close() {
  if (size > 0) {
    RemoveAll();
  }
  inherited_taBase::Close();
}

bool taList_impl::Close_Child(taBase* obj) {
  return RemoveEl(obj);
}

bool taList_impl::CloseLater_Child(taBase* obj) {
  if (obj->refn <= 0) {
    DebugInfo("WARNING: taList_impl::CloseLater_Child: taBase refn <= 0 for item type=",
              obj->GetTypeDef()->name, "name=", obj->GetName().chars());
  }
  // just add to list -- it will then close us
  tabMisc::DelayedClose(obj);
  return true;
}

taBase* taList_impl::CopyChildBefore(taBase* src, taBase* child_pos) {
  int idx = FindEl(child_pos);
  if(idx < 0) idx = size;
  taBase* new_obj = src->MakeToken();
  Insert(new_obj, idx);
  new_obj->UnSafeCopy(src);
  new_obj->SetName(src->GetName());
  new_obj->UpdateAfterEdit();
  return new_obj;
}

String taList_impl::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
                              bool force_inline) const {
  String nm = " Size: ";
  nm += String(size);
  nm += String(" (") + el_typ->name + ")";
  return nm;
}

bool taList_impl::SetValStr(const String& val, void* par, MemberDef* memb_def,
                            TypeDef::StrContext sc, bool force_inline) {
  if(val != String::con_NULL) {
    String tmp = val;
    if(tmp.contains('(')) {
      tmp = tmp.after('(');
      tmp = tmp.before(')');
    }
    tmp.gsub(" ", "");
    TypeDef* td = taMisc::FindTypeName(tmp);
    if(td != NULL) {
      el_typ = td;
      return true;
    }
  }
  return false;
}

int taList_impl::ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
               void* par, TypeDef* par_typ, MemberDef* memb_def, TypeDef::StrContext sc) {
  int rval = inherited::ReplaceValStr(srch, repl, mbr_filt, par, par_typ, memb_def, sc);
  for(int i=0; i<size; i++) {
    taBase* itm = (taBase*)el[i];
    if(itm && itm->GetOwner() == this) { // only owned is key for preventing recursion
      rval += itm->ReplaceValStr(srch, repl, mbr_filt, this, GetTypeDef(), memb_def, sc);
    }
  }
  if(rval > 0)
    UpdateAfterEdit();
  return rval;
}

taObjDiffRec* taList_impl::GetObjDiffVal(taObjDiff_List& odl, int nest_lev,  MemberDef* memb_def,
          const void* par, TypeDef* par_typ, taObjDiffRec* par_od) const {

  // do NOT do the basic members on the list -- just a bunch of clutter
  // taObjDiffRec* odr = inherited::GetObjDiffVal(odl, nest_lev, memb_def, par, par_typ, par_od);

  // this is the rep of this item
  taObjDiffRec* lsodr = new taObjDiffRec(odl, nest_lev, GetTypeDef(), memb_def,
                                         (void*)this, (void*)par, par_typ, par_od);
  if(GetOwner()) {
    lsodr->tabref = new taBaseRef;
    ((taBaseRef*)lsodr->tabref)->set((taBase*)this);
  }

  odl.Add(lsodr);

  // lsodr->name = odr->name;
  // lsodr->value = odr->value + "_list";
  // lsodr->ComputeHashCode();

  for(int i=0; i<size; i++) {
    taBase* itm = (taBase*)el[i];
    if(itm && itm->GetOwner() == this) {
      itm->GetObjDiffVal(odl, nest_lev+1, NULL, this, GetTypeDef(), lsodr);
    }
  }
  return lsodr;
}

int taList_impl::Dump_Save_PathR(ostream& strm, taBase* par, int indent) {
   bool dump_my_path = !(this == par);
  // dump_my_path is a bit of a hack, to enable us to use this same
  // routine either for Dump_Save_PathR or when dumping list items
  // recursively -- when dump_my_path=false, we have already dumped the
  // item path one level above, so only need its members, and children

  // first save any sub-members (there usually aren't any)
  int rval = GetTypeDef()->Dump_Save_PathR(strm, (void*)this, (void*)par, indent);

//   if (IsEmpty())  return rval;
  // actually need to save this to be able to undo back to an empty group

  strm << "\n";                 // actually saving a path: put a newline
  if (dump_my_path) {
    taMisc::indent(strm, indent, 1);
    Dump_Save_Path(strm, par, indent); // save my path!
    strm << " = [" << size << "] {\n";
    ++indent; // bump up
  }
  // note: we bumped indent if it is truly nested...
  Dump_Save_PathR_impl(strm, this, indent);

  if (dump_my_path) {
    --indent;
    taMisc::indent(strm, indent, 1);
    strm << "};\n";
  }
  return true;
}

int taList_impl::Dump_Save_PathR_impl(ostream& strm, taBase* par, int indent) {
  if(!Dump_QuerySaveChildren()) return true;
  int cnt = 0;
  int i;
  for (i=0; i<size; i++) {
    taBase* itm = (taBase*)el[i];
    if(itm == NULL)
      continue;
    cnt++; // sure we are dumping something at this point

    if (El_Kind_(itm) == EK_LINK) { // a link, create a dummy placeholder
      taMisc::indent(strm, indent, 1);
      strm << itm->GetTypeDef()->name << " @[" << i << "] { };\n";
      continue;
    }
    taMisc::indent(strm, indent, 1);
    itm->Dump_Save_Path(strm, par, indent); // must be relative to parent!  not this!
    // can't put this in dump_save_path cuz don't want it during non PathR times..
    if (itm->InheritsFrom(TA_taList_impl)) {
      taList_impl* litm = (taList_impl*)itm;
      if(!litm->IsEmpty()) {
        strm << " = [" << litm->size << "]";
      }
    }
    strm << " { ";
    // NOTE: incredibly sleazy trick, we pass par=this to flag the outer routine
    // not to redo our path etc. -- no easy workaround or redesign
    if(itm->Dump_Save_PathR(strm, itm, indent+1))
      taMisc::indent(strm, indent, 1);
    strm << "};\n";
  }
  return cnt;
}

// actually save all the elements in the group
int taList_impl::Dump_SaveR(ostream& strm, taBase* par, int indent) {
  if(!Dump_QuerySaveChildren()) return true;
  String mypath = GetPath(NULL, par);
  int i;
  for(i=0; i<size; i++) {
    taBase* itm = (taBase*)FastEl_(i);
    if (!itm) continue;
    ElKind ek = El_Kind_(itm);
    if (ek == EK_OWN) {
      itm->Dump_Save_impl(strm, par, indent);
    }
    else if (ek == EK_LINK) {   // a link
      taMisc::indent(strm, indent, 1) << GetTypeDef()->name << " ";
      if(par != NULL) strm << "@";
      strm << mypath << " = ";
      strm << "[" << i << "] = ";
      if(itm->Dump_Save_Path(strm, NULL, indent))
        strm << ";\n";
      if(itm->HasOption("LINK_SAVE"))
        itm->Dump_Save_impl(strm, NULL, indent); // save even though its a link!
    }
  }
  return true;
}

void taList_impl::Dump_Load_pre() {
  inherited::Dump_Load_pre();   // called only when directly loading this group!
  //  Reset(); // actually can't do this because same fun is used for loading
  // entire list/group as for one new element in the group.. hmm.
}

taBase* taList_impl::Dump_Load_Path_parent(const String& el_path, TypeDef* ld_el_typ) {
  if(el_path.firstchar() != '[' || el_path.lastchar() != ']') {
    taMisc::Warning("*** Dump_Load_Path_parent: path is incorrectly formatted:",el_path,
                    "for parent obj:",GetPathNames());
    return NULL;
  }
  String elnm = el_path.between('[',']');
  bool has_nm = false;
  taBase* nw_el = NULL;
  int idx = -1;
  if(elnm.firstchar() == '\"') {
    has_nm = true;
    elnm = elnm.between('\"','\"');
    elnm = taMisc::StringCVar(elnm);
    nw_el = (taBase*)FindName_(elnm);
    if(nw_el)
      idx = FindEl_(nw_el);
    else
      idx = size;               // put it at the end!
  }
  else {
    idx = (int)elnm;
    if(InRange(idx)) {
      nw_el = (taBase*)FastEl_(idx);
      if(!nw_el)        RemoveIdx(idx); // somehow has a null guy on list.. nuke it
    }
  }
  if(nw_el) {
    // check for correct type, but allow a list to be created in a group
    // for backwards compatibility with changes from groups to lists
    if((nw_el->GetTypeDef() != ld_el_typ) &&
       !((nw_el->GetTypeDef() == &TA_taBase_List) && (ld_el_typ == &TA_taBase_Group))) {
      // object not the right type, try to create new one..
      if(taMisc::verbose_load >= taMisc::MESSAGES) {
        taMisc::Warning("*** Dump_Load_Path_parent: Object at path:",GetPathNames(),
                        "of type:",nw_el->GetTypeDef()->name,"is not the right type:",
                        ld_el_typ->name,", attempting to create new one");
      }
      nw_el = taBase::MakeToken(ld_el_typ);
      if(!nw_el) {
        taMisc::Warning("*** Dump_Load_Path_parent: Could not make new token of type:",
                        ld_el_typ->name,"for child item at path:",
                        el_path,"in parent list:",GetPathNames());
        return NULL;
      }
      ReplaceIdx(idx, nw_el);
      if(has_nm) {
        nw_el->SetName(elnm);
      }
    }
  }
  else {
    nw_el = New(1,ld_el_typ);
    // assuming that these are saved in order so we just add sequentially w/out checking
    // no clear action to take if this is not the case b/c we don't know how to
    // create intervening types..
    if(!nw_el) {
      taMisc::Warning("*** New: Could not make a token of:",ld_el_typ->name,"in:",GetPathNames());
      return NULL;
    }
    if(has_nm) {
      nw_el->SetName(elnm);
    }
  }
  if(m_trg_load_size >= 0 && idx+1 == m_trg_load_size) {
    // we are the last guy who is supposed to be loaded -- set the list size to be the
    // target size -- nukes any extra guys that might have been lingering!!
    SetSize(m_trg_load_size);
    m_trg_load_size = -1;       // reset for next time around
  }

  if(taMisc::verbose_load >= taMisc::TRACE) {
    String msg;
    msg << "Success: Leaving TypeDef::Dump_Load_Path_parent, type: " << ld_el_typ->name
        << ", parent path = " << GetPathNames()
        << ", el_path = " << el_path;
    taMisc::Info(msg);
  }
  return nw_el;
}

int taList_impl::Dump_Load_Value(istream& strm, taBase* par) {
  m_trg_load_size = -1;

  int c = taMisc::skip_white(strm);
  if(c == EOF)  return EOF;
  if(c == ';')  return 2;       // signal that its a path
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    return 2;
  }

  if(c == '{') {
    return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
  }
  else if(c == '=') {           // a link or the number of items to create
    c = taMisc::skip_white(strm);
    if(c == '[') {
      c = taMisc::read_word(strm);
      if(c == ']') {
        int idx = atoi(taMisc::LexBuf);
        c = taMisc::skip_white(strm, true); // peek
        if(c == '=') {          // this means its a link
          strm.get();
          taMisc::read_word(strm); // get type
          String typ_nm = taMisc::LexBuf;
          TypeDef* eltd = taMisc::FindTypeName(typ_nm);
          if(TestWarning((!eltd || !eltd->InheritsFrom(el_base)),
                         "Dump_Load_Value",
                         "Null or invalid type:",typ_nm,"to link into list of types:",
                         el_base->name)) return false;
          taMisc::read_till_rb_or_semi(strm);
          String lnk_path = taMisc::LexBuf;
          dumpMisc::path_subs.FixPath(eltd, tabMisc::root, lnk_path);
          MemberDef* md;
          taBase* tp = tabMisc::root->FindFromPath(lnk_path, md);
          if(idx < size)
            ReplaceLinkIdx(idx, tp); // if already room, replace it..
          else {
            Link(tp);           // otherwise, add it..
            idx = size-1;
          }
          if(tp == NULL) {
            dumpMisc::vpus.AddVPU((taBase**)&(el[idx]), (taBase*)NULL, lnk_path);
          }
          return true;
        }
        else if(c == '{') {     // no type information -- just the count
          strm.get();           // get the bracket
          Alloc(idx);           // just make sure we have the ptrs allocated to this size
          m_trg_load_size = idx; // target loading size
          if(m_trg_load_size == 0) {
            if(size > 0)
              taMisc::Info("load reset size:", String(size));
            taList_impl::RemoveAll();
            // normal load size enforcement occurs on last item loaded --
            // if no items to load, it never happens!  This is the enforcer!
            // also: for undo optimized loading (i.e., not saving networks), this count
            // is actually set accurately pre-filtering so it is fine
            // also, using list version b/c groups may have sub guys and don't want to nuke
            // those guys!!
          }
          return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
        }
        else {                  // type information -- create objects too!
          taMisc::read_word(strm, true); // get type
          String typ_nm = taMisc::LexBuf;
          TypeDef* eltd = taMisc::FindTypeName(typ_nm);
          if(TestWarning((!eltd || !eltd->InheritsFrom(el_base)),
                         "Dump_Load_Value",
                         "Null or invalid type:",typ_nm,"to link into list of types:",
                         el_base->name)) return false;
          el_typ = eltd;
          // ensure that enough items are present (don't do full enforce size)
//        if(size < idx)
//          New(idx - size, el_typ);
          // actually, probably much better to do full enforce size!  but this case is weird
          // not sure if it actually still gets processed -- why would it??
          // actually very important to set the exact size
          SetSize(idx);
          c = taMisc::skip_white(strm);
          if(c == '{') {
            return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
          }
        }
      }
    }
    TestWarning(true, "Dump_Load_Value", "Bad formatting for link");
    return false;
  }

  TestWarning(true, "Dump_Load_Value", "Missing '{', '=', or '[' in dump file");
  return false;
}

void taList_impl::El_SetDefaultName_(void* item_, int idx) {
  taBase* item = (taBase*)item_;
  if (item->HasOption("DEF_NAME_LIST")) {
    item->SetDefaultName_impl(idx);
  }
}

void taList_impl::EnforceSameStru(const taList_impl& cp) {
  int i;
  for(i=0; i<cp.size; i++) {
    taBase* citm = (taBase*)cp.el[i];
    if(citm == NULL) continue;
    if(size <= i) {
      taBase* itm = taBase::MakeToken(citm->GetTypeDef());
      if(itm != NULL)
        Add_(itm);
    }
    else {
      taBase* itm = (taBase*)el[i];
      if(citm->GetTypeDef() == itm->GetTypeDef())
        continue;
      taBase* rval = taBase::MakeToken(citm->GetTypeDef());
      if(rval != NULL)
        ReplaceIdx(i, rval);
    }
  }
  if(size > cp.size)
    for(i=size-1; i>=cp.size; i--)
      RemoveIdx(i);
}

void taList_impl::SetSize(int sz) {
  if(size < sz)
    New(sz - size);
  else {
    Trim(sz);
  }
}

void taList_impl::EnforceType() {
  int i;
  for(i=0; i<size; i++) {
    taBase* itm = (taBase*)el[i];
    if((itm == NULL) || (itm->GetTypeDef() == el_typ))
      continue;

    taBase* rval = taBase::MakeToken(el_typ);
    if(rval != NULL)
      ReplaceIdx(i, rval);
  }
}

void* taList_impl::FindMembeR(const String& nm, MemberDef*& ret_md) const {
  ret_md = NULL;

  // first look for special list index syntax
  String idx_str = nm;
  idx_str = idx_str.before(']');
  if(idx_str.nonempty()) {
    idx_str = idx_str.after('[');
    if(idx_str.contains('\"')) {
      String elnm = idx_str.between('\"','\"');
      elnm = taMisc::StringCVar(elnm);
      if(TestWarning(elnm.empty(), "FindMembeR","empty string index name:", idx_str))
        return NULL;
      return FindName_(elnm);
    }
    else {
      int idx = atoi(idx_str);
      if((idx >= size) || (idx < 0)) {
        return NULL;
      }
      return el[idx];           // don't have an md for this guy
    }
  }

  // then look for items in the list itself, by name or type
  taBase* fnd = FindNameType_(nm);
  if(fnd)
    return fnd;

  // then look on members of list obj itself, recursively
  void* rval = inherited::FindMembeR(nm, ret_md);
  if(rval)
    return rval;

  // finally, look recursively on owned objs on list
  //  int max_srch = MIN(taMisc::search_depth, size);
  // these days, it just doesn't make sense to restrict!
  for(int i=0; i<size; i++) {
    taBase* itm = (taBase*)FastEl_(i);
    if(itm && itm->GetOwner() == this) {
      rval = itm->FindMembeR(nm, ret_md);
      if(rval)
        return rval;
    }
  }
  return NULL;
}

int taList_impl::FindTypeIdx(TypeDef* it) const {
  for(int i=0; i < size; i++) {
    taBase* tmp = (taBase*)el[i];
    if(tmp && tmp->InheritsFrom(it)) {
      return i;
    }
  }
  return -1;
}

taBase* taList_impl::FindType_(TypeDef* it) const {
  int idx = FindTypeIdx(it);
  if(idx >= 0) return (taBase*)el[idx];
  return NULL;
}

int taList_impl::FindNameContainsIdx(const String& item_nm) const {
  for(int i=0; i < size; i++) {
    taBase* it = (taBase*)el[i];
    if(it && it->GetName().contains(item_nm)) {
      return i;
    }
  }
  return -1;
}

taBase* taList_impl::FindNameContains_(const String& item_nm) const {
  int idx = FindNameContainsIdx(item_nm);
  if(idx >= 0) return (taBase*)el[idx];
  return NULL;
}

int taList_impl::FindNameTypeIdx(const String& item_nm) const {
  for(int i=0; i < size; i++) {
    taBase* it = (taBase*)el[i];
    if(it && (it->FindCheck(item_nm) || it->InheritsFromName(item_nm))) {
      return i;
    }
  }
  return -1;
}

taBase* taList_impl::FindNameType_(const String& item_nm) const {
  int idx = FindNameTypeIdx(item_nm);
  if(idx >= 0) return (taBase*)el[idx];
  return NULL;
}

taBase* taList_impl::FindMakeNameType_(const String& item_nm, TypeDef* td, bool& made_new) {
  made_new = false;
  taBase* rval = (taBase*)FindName_(item_nm);
  if(rval) {
    if(td && !rval->InheritsFrom(td)) {
      taBase* nv = taBase::MakeToken(td);
      if(nv) {
        nv->SetName(item_nm);
        ReplaceEl(rval, nv);
        made_new = true;
      }
      return nv;
    }
    return rval;
  }
  made_new = true;
  rval = New(1,td);
  if(rval) {
    rval->SetName(item_nm);
  }
  return rval;
}

String taList_impl::GetPath(taBase* ta, taBase* par_stop) const {
  if ((((taBase*) this) == par_stop) && (ta == NULL))
    return ".";
  String rval;

  taBase* par = GetOwner();
  if (par == NULL) {
    if (ta == NULL) rval = "root";
  }
  else if (((taBase*) this) != par_stop) {
    rval = par->GetPath((taBase*)this, par_stop);
  }

  if (ta != NULL) {
    if (MemberDef *md = FindMember(ta)) {
      rval += "." + md->name;
    }
    else if (MemberDef *md = FindMemberPtr(ta)) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      int gidx = FindEl_(ta);
      if (gidx >= 0)
        rval += "[" + String(gidx) + "]";
      else
        rval += "[?]";
    }
  }
  return rval;
}

String taList_impl::GetPathNames(taBase* ta, taBase* par_stop) const {
  if(taMisc::is_undo_saving) return GetPath(ta, par_stop); // use indexes for undo

  if((((taBase*) this) == par_stop) && (ta == NULL))
    return ".";
  String rval;

  taBase* par = GetOwner();
  if (par == NULL) {
    if (ta == NULL) rval = "root";
  }
  else if (((taBase*) this) != par_stop) {
    rval = par->GetPathNames((taBase*)this, par_stop);
  }

  if (ta != NULL) {
    if (MemberDef *md = FindMember(ta)) {
      rval += "." + md->name;
    }
    else if (MemberDef *md = FindMemberPtr(ta)) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      String obj_nm = ta->GetName();
      if (obj_nm.empty() || !ta->HasName()) { // only use real nbase names.
        int gidx = FindEl_(ta);
        if (gidx >= 0)
          rval += "[" + String(gidx) + "]";
        else
          rval += "[?]";
      }
      else {
        rval += "[\"" + obj_nm + "\"]";
      }
    }
  }
  return rval;
}

String taList_impl::GetPath_Long(taBase* ta, taBase* par_stop) const {
  if((((taBase*) this) == par_stop) && (ta == NULL))
    return ".";
  String rval;

  taBase* par = GetOwner();
  if (par == NULL) {
    if (ta == NULL) rval = "root";
  }
  else if (((taBase*) this) != par_stop) {
    rval = par->GetPath_Long((taBase*)this, par_stop);
  }

  if (GetName() != "")
    rval += "(" + GetName() + ")";

  if (ta != NULL) {
    if (MemberDef *md = FindMember(ta)) {
      rval += "." + md->name;
    }
    else if (MemberDef *md = FindMemberPtr(ta)) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      int gidx = FindEl_(ta);
      if (gidx >= 0)
        rval += "[" + String(gidx) + "]";
      else
        rval += "[?]";
    }
  }
  return rval;
}

taBase* taList_impl::New_impl(int no, TypeDef* typ, const String& name_) {
  if(typ == NULL)
    typ = el_typ;
  if(TestWarning(!typ->InheritsFrom(el_base), "New",
                 "Attempt to create type:", typ->name,
                 "in list with base type:", el_base->name)) return NULL;
  if(TestError(typ->HasOption("VIRT_BASE"),
                 "You cannot create a token of type:", typ->name,
                 "because it is a 'virtual' base type -- you must create a more specific subtype of it instead")) return NULL;
  if(no > 10)                   // adding large numbers is slow!
    StructUpdate(true);
  taBase* rval = NULL;
  Alloc(size + no);             // pre-allocate!
  if((size == 0) || (no > 1))
    el_typ = typ;       // first item or multiple items set el_typ
  int i;
  for(i=0; i < no; i++) {
    rval = taBase::MakeToken(typ);
    if(TestError(!rval, "New", "Could not make a token of type",typ->name,"(probably has #NO_INSTANCE in object header comment directive)"))
      goto exit;
    if (name_.nonempty() && (name_ != "(default name)")) {
      String cnm = taMisc::StringCVar(name_);
      // if only 1 inst, then name is literal, else it is a base
      if (no == 1) {
        rval->SetName(cnm);
      } else {
        rval->SetName(cnm + "_" + String(i));
      }
    }
    Add_(rval);
  }
 exit:
  if(no > 10)
    StructUpdate(false);
  return rval;
}

taBase* taList_impl::New_gui(int no, TypeDef* typ, const String& name_) {
  if(!el_typ->InheritsFrom(el_base)) el_typ = el_base;
  taBase* rval = New(no, typ, name_);
  if (rval) {
    if (taMisc::gui_active && !taMisc::no_auto_expand) {
      if(!HasOption("NO_EXPAND_ALL") && !rval->HasOption("NO_EXPAND_ALL"))
      {
        tabMisc::DelayedFunCall_gui(rval, "BrowserExpandAll");
        tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
      }
    }
  }
  return rval;
}

String& taList_impl::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << "Elements of List: " << GetDisplayName() << " [" << size << "] {\n";
  String_PArray nms;
  nms.Alloc(size);
  TA_FOREACH(vitm, *this) {     // use iterator so it is recursive on existing filtering
    taBase* itm = vitm.toBase();
    if(itm) {
      nms.Add(itm->GetName());
    }
    else {
      nms.Add("NULL");
    }
  }
  taMisc::FancyPrintList(strm, nms, indent+1);
  taMisc::IndentString(strm, indent);
  strm << "}";
  return strm;
}

bool taList_impl::RemoveIdx(int i) {
  // default could be out of range..
  if(el_def >= size-1)
    el_def = 0;
  return inherited_taPtrList::RemoveIdx(i);
}

bool taList_impl::Transfer(taBase* item) {
  taBase* oldo = item->GetOwner();
  bool rval = Transfer_((void*)item);
  item->UpdateAfterMove(oldo);
  return rval;
}

int taList_impl::ReplaceType(TypeDef* old_type, TypeDef* new_type) {
  int nchanged = 0;
  int sz = size;                // only go to current size
  int i;
  for(i=0;i<sz;i++) {
    if(((taBase*)el[i])->GetTypeDef() != old_type) continue;
    if(ChangeType(i, new_type)) nchanged++;
  }
  return nchanged;
}

void taList_impl::SetBaseType(TypeDef* it) {
  el_base = it;
  el_typ = it;
}

int taList_impl::SetDefaultEl(taBase* it) {
  int idx = FindEl(it);
  if(idx >= 0)    el_def = idx;
  return idx;
}

int taList_impl::SetDefaultElName(const String& nm) {
  int idx = FindNameIdx(nm);
  if(idx >= 0)    el_def = idx;
  return idx;
}

int taList_impl::SetDefaultElType(TypeDef* it) {
  int idx = FindTypeIdx(it);
  if(idx >= 0)    el_def = idx;
  return idx;
}

int taList_impl::AddToControlPanelSearch(const String& memb_contains, ControlPanel*& editor) {
  int nfound = taOBase::AddToControlPanelSearch(memb_contains, editor);
  for(int i=0;i<size;i++) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    if(itm->GetOwner() == this) { // for guys we own (not links; prevents loops)
      nfound += itm->AddToControlPanelSearch(memb_contains, editor);
    }
  }
  return nfound;
}

void taList_impl::Search_impl(const String& srch, taBase_PtrList& items,
                         taBase_PtrList* owners,
                         bool contains, bool case_sensitive,
                         bool obj_name, bool obj_type,
                         bool obj_desc, bool obj_val,
                         bool mbr_name, bool type_desc) {
  int st_sz = items.size;
  taOBase::Search_impl(srch, items, owners, contains, case_sensitive, obj_name, obj_type,
                       obj_desc, obj_val, mbr_name, type_desc);
  bool already_added_me = false;
  if(items.size > st_sz)
    already_added_me = true;
  for(int i=0; i<size; i++) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    if(itm->GetOwner() == this) { // for guys we own (not links; prevents loops)
      if(SearchTestItem_impl(itm, srch, contains, case_sensitive, obj_name, obj_type,
                             obj_desc, obj_val, mbr_name, type_desc)) {
        items.Link(itm);
      }
      itm->Search_impl(srch, items, owners, contains, case_sensitive, obj_name, obj_type,
                       obj_desc, obj_val, mbr_name, type_desc);
    }
  }
  if(owners && (items.size > st_sz) && !already_added_me) { // we added somebody somewhere..
    owners->Link(this);
  }
}

void taList_impl::CompareSameTypeR(Member_List& mds, TypeSpace& base_types,
                                   voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                                   taBase* cp_base, int show_forbidden,
                                   int show_allowed, bool no_ptrs) {
  if(!cp_base) return;
  if(GetTypeDef() != cp_base->GetTypeDef()) return; // must be same type..

  taOBase::CompareSameTypeR(mds, base_types, trg_bases, src_bases, cp_base,
                            show_forbidden, show_allowed, no_ptrs);
  // then recurse..
  taList_impl* cp_lst = (taList_impl*)cp_base;
  int mxsz = MIN(size, cp_lst->size);
  for(int i=0; i<mxsz; i++) {
    taBase* itm = (taBase*)FastEl_(i);
    taBase* cp_itm = (taBase*)cp_lst->FastEl_(i);
    if(!itm || !cp_itm) continue;
    if((itm->GetOwner() == this) && (cp_itm->GetOwner() == cp_lst)) {
       // for guys we own (not links; prevents loops)
      itm->CompareSameTypeR(mds, base_types, trg_bases, src_bases, cp_itm,
                            show_forbidden, show_allowed, no_ptrs);
    }
  }
}

int taList_impl::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int nchg = taOBase::UpdatePointers_NewPar(old_par, new_par);
  for(int i=size-1; i>=0; i--) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    if(itm->GetOwner() == this) { // for guys we own (not links; prevents loops)
      nchg += itm->UpdatePointers_NewPar(old_par, new_par);
    }
    else {                      // linked item: could be to something outside of scope!
      taBase* old_own = itm->GetOwner(old_par->GetTypeDef());
      if(old_own != old_par) continue;
      String old_path = itm->GetPath(NULL, old_par);
      MemberDef* md;
      taBase* nitm = new_par->FindFromPath(old_path, md);
      if(nitm) {
        ReplaceLinkIdx_(i, nitm);
        nchg++;
      }
      else {
        RemoveIdx(i);
      }
    }
  }
  return nchg;
}

int taList_impl::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) {
  int nchg = taOBase::UpdatePointers_NewParType(par_typ, new_par);
  for(int i=size-1; i>=0; i--) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    if(itm->GetOwner() == this) { // for guys we own (not links; prevents loops)
      nchg += itm->UpdatePointers_NewParType(par_typ, new_par);
    }
    else {                      // linked item: could be to something outside of scope!
      taBase* old_own = itm->GetOwner(par_typ);
      String old_path = itm->GetPath(NULL, old_own);
      MemberDef* md;
      taBase* nitm = new_par->FindFromPath(old_path, md);
      if(nitm) {
        ReplaceLinkIdx_(i, nitm);
        nchg++;
      }
    }
  }
  return nchg;
}

int taList_impl::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  int nchg = taOBase::UpdatePointers_NewObj(old_ptr, new_ptr);
  if(old_ptr && (old_ptr->GetOwner() == this)) return 0;
  // do not walk down the guy itself -- its a gonner
  for(int i=size-1; i>=0; i--) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    bool we_own = (itm->GetOwner() == this);
    if(itm == old_ptr) {           // if it is the old guy, it is by defn a link because we're not the owner..
      if(!new_ptr)                 // if replacement is null, just remove it
        RemoveIdx(i);
      else
        ReplaceLinkIdx_(i, new_ptr);    // it is a link to old guy; replace it!
      nchg++;
    }
    else if(we_own) {           // only for guys we own (not links; prevents loops)
      nchg += itm->UpdatePointers_NewObj(old_ptr, new_ptr);
    }
  }
  return nchg;
}

int taList_impl::UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr) {
  int nchg = taOBase::UpdatePointersToMyKids_impl(scope_obj, new_ptr);
  taList_impl* new_list = (taList_impl*)new_ptr;
  for(int i=size-1; i>=0; i--) {
    taBase* oitm = (taBase*)el[i];
    if(!oitm) continue;
    taBase* nitm = NULL;
    if(new_list && (new_list->size > i))
      nitm = (taBase*)new_list->el[i];
    if(nitm && (nitm->GetTypeDef() != oitm->GetTypeDef())) nitm = NULL; // not the same
    nchg += oitm->UpdatePointersToMe_impl(scope_obj, nitm);     // do it all over on this guy
  }
  return nchg;
}

#ifdef TA_GUI
const QPixmap* taList_impl::GetDataNodeBitmap(int bmf, int& flags_supported) const {
  flags_supported |= NBF_FOLDER_OPEN;
  if (bmf & NBF_FOLDER_OPEN)
    return folder_open_pixmap();
  else
    return folder_closed_pixmap();
}


#endif

