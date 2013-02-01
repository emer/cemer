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

#include "IObjectSelectable.h"
#include <taiObjectMimeFactory>
#include <taiClipData>
#include <taBase_PtrList>
#include <ISelectable_PtrList>
#include <taiSigLink>

taiClipData* IObjectSelectable::GetClipDataSingle(int src_edit_action,
  bool for_drag, GuiContext sh_typ) const
{
  // if it is taBase, we can make an object
  taBase* obj = this->taData(sh_typ);
  if (!obj) return NULL;

  taiObjectMimeFactory* mf = taiObjectMimeFactory::instance();
  taiClipData* rval = new taiClipData(src_edit_action);
  mf->AddSingleObject(rval, obj);
  return rval;
}

taiClipData* IObjectSelectable::GetClipDataMulti(const ISelectable_PtrList& sel_items,
    int src_edit_action, bool for_drag, GuiContext sh_typ) const
{
  taiClipData* rval = NULL;
  //note: although a bit sleazy, we just do this by optimistically
  // assuming all are taBase (which realistically, they are)
  taBase_PtrList* bl = new taBase_PtrList;
  bool do_it = true;
  for (int i = 0; i < sel_items.size; ++i) {
    ISelectable* dn = sel_items.FastEl(i);
    taBase* obj = dn->taData();
    if (!obj) {
      do_it = false;
      break;
    }
    bl->Add(obj);
  }
  if (do_it) {
    taiObjectMimeFactory* mf = taiObjectMimeFactory::instance();
    rval = new taiClipData(src_edit_action);
    mf->AddMultiObjects(rval, bl);
  }
  delete bl;
  return rval;
}

int IObjectSelectable::EditActionD_impl_(taiMimeSource* ms,
  int ea, GuiContext sh_typ)
{//note: follows same logic as the Query
  taiSigLink* pdl = clipParLink(sh_typ);
  //note: called routines must requery for allowed
  taiSigLink* link = this->effLink(sh_typ);
  int rval = taiClipData::ER_IGNORED;
  // we have to individually disambiguate the allowed, because we have
  // to make sure the right list or group guy handles things like PasteInto, etc.
  int allowed = 0;
  int forbidden = 0;
  int eax;
  if (pdl) {
//no    pdl->ChildQueryEditActions_impl(par_md(), link, ms, allowed, forbidden);
    pdl->ChildQueryEditActions_impl(NULL, link, ms, allowed, forbidden);
    eax = ea & (allowed & (~forbidden));
    if (eax)
//no      rval = pdl->ChildEditAction_impl(par_md(), link, ms, eax);
      rval = pdl->ChildEditAction_impl(NULL, link, ms, eax);
  }
  if (link && (rval == taiClipData::ER_IGNORED)) {
    allowed = forbidden = 0;
    link->ChildQueryEditActions_impl(this->md(), NULL, ms, allowed, forbidden);
    eax = ea & (allowed & (~forbidden));
    if (eax)
      rval = link->ChildEditAction_impl(this->md(), NULL, ms, eax);

    if (rval == taiClipData::ER_IGNORED) {
      allowed = forbidden = 0;
      link->QueryEditActions_impl(ms, allowed, forbidden); // ex. COPY
      eax = ea & (allowed & (~forbidden));
      if (eax)
        rval = link->EditAction_impl(ms, eax);
    }
  }
  return rval;
}

int IObjectSelectable::EditActionS_impl_(int ea, GuiContext sh_typ) {
//note: follows same logic as the Query
  taiSigLink* pdl = clipParLink(sh_typ);
  //note: called routines must requery for allowed
  taiSigLink* link = this->effLink(sh_typ);
  int rval = taiClipData::ER_IGNORED;
  if (pdl) {
//no    rval = pdl->ChildEditAction_impl(par_md(), link, NULL, ea);
    rval = pdl->ChildEditAction_impl(NULL, link, NULL, ea);
  }
  if (link) {
    //note: item-as-parent does not apply to src context, so we omit it
    if (rval == taiClipData::ER_IGNORED)
      rval = link->EditAction_impl(NULL, ea);
  }
  return rval;
}

void IObjectSelectable::QueryEditActionsD_impl_(taiMimeSource* ms,
  int& allowed, int& forbidden, GuiContext sh_typ) const
{
  // parent object will generally manage CUT, and DELETE
  // parent object normally passes on to child object
  taiSigLink* pdl = clipParLink(sh_typ);
  taiSigLink* link = this->effLink(sh_typ);
  if (pdl)
    pdl->ChildQueryEditActions_impl(NULL, link, ms, allowed, forbidden); // ex. DROP of child on another child, to reorder
  if (link) {
    link->ChildQueryEditActions_impl(this->md(), NULL, ms, allowed, forbidden); // ex. DROP of child on parent, to insert as first item
    link->QueryEditActions_impl(ms, allowed, forbidden); // ex. COPY
  }
}

void IObjectSelectable::QueryEditActionsS_impl_(int& allowed, int& forbidden,
  GuiContext sh_typ) const
{
  // parent object will generally manage CUT, and DELETE
  // parent object normally passes on to child object
  taiSigLink* pdl = clipParLink(sh_typ);
  taiSigLink* link = this->effLink(sh_typ);
  if (pdl)
    pdl->ChildQueryEditActions_impl(NULL, link, NULL, allowed, forbidden); // ex. CUT of child
  if (link) {
    // note: item-as-parent doesn't apply to src actions, so we omit that
    link->QueryEditActions_impl(NULL, allowed, forbidden); // ex. COPY
  }
}


