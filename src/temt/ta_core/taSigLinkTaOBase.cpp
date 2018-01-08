// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "taSigLinkTaOBase.h"

#include <taiTreeNodeDefChild>
#include <taiViewType>


taSigLinkTaOBase::taSigLinkTaOBase(taOBase* data_)
  : inherited((taBase*)data_, *(data_->addr_sig_link()))
{
}

String taSigLinkTaOBase::ChildGetColText(taSigLink* child, const KeyString& key, int itm_idx) const
{
  if (!list()) return inherited::ChildGetColText(child, key, itm_idx);
  return list()->ChildGetColText(child->data(), child->GetDataTypeDef(), key, itm_idx);
}

taiTreeNode* taSigLinkTaOBase::CreateTreeDataNode_impl(MemberDef* md, taiTreeNode* nodePar,
  iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags)
{
  if (!list()) return inherited::CreateTreeDataNode_impl(md, nodePar,
    tvPar, after, node_name, dn_flags);
  taiTreeNode* rval = NULL;
  if (nodePar)
    rval = new taiTreeNodeDefChild(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taiTreeNodeDefChild(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}


String taSigLinkTaOBase::GetColHeading(const KeyString& key) const {
  if (!list()) return inherited::GetColHeading(key);
  return list()->GetColHeading(key);
}

const KeyString taSigLinkTaOBase::GetListColKey(int col) const {
  if (!list()) return inherited::GetListColKey(col);
  return list()->GetListColKey(col);
}

taiSigLink* taSigLinkTaOBase::GetListChild(int itm_idx) {
  if (!list()) return inherited::GetListChild(itm_idx);
  if ((itm_idx < 0) || (itm_idx >= list()->size))
    return NULL;
  taBase* el = (taBase*)list()->SafeEl_(itm_idx);
  if (!el) return NULL;
  TypeDef* typ = el->GetTypeDef();
  taiSigLink* dl = taiViewType::StatGetSigLink(el, typ);
  return dl;
}

taiSigLink* taSigLinkTaOBase::GetListChild(void* el) {
  if (!list()) return inherited::GetListChild(el);
  if (!el) return NULL;
  TypeDef* typ = list()->El_GetType_(el);
  if (typ == NULL) return NULL;
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->IsActualTaBase()) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  // get the link
  taiSigLink* dl = taiViewType::StatGetSigLink(el, typ);
  return dl;
}

int taSigLinkTaOBase::NumListCols() const {
  if (!list()) return inherited::NumListCols();
  return list()->NumListCols();
}

MemberDef* taSigLinkTaOBase::GetDataMemberDef() const {
  if (!m_data) return NULL;
  taBase* owner = data()->GetOwner();
  if (owner) return owner->FindMemberBase(data());
  else return NULL;
}

