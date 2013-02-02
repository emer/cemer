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

#include "TypeInfoBrowseViewer.h"
#include <iTypeInfoBrowseViewer>
#include <taMisc>

TypeInfoBrowseViewer* TypeInfoBrowseViewer::New(void* root, TypeDef* root_typ, MemberDef* root_md) {
  TypeInfoBrowseViewer* rval = new TypeInfoBrowseViewer();
  rval->setRoot(root, root_typ, root_md);
  return rval;
}

void TypeInfoBrowseViewer::Initialize() {
  m_root = NULL;
}

void TypeInfoBrowseViewer::Copy_(const TypeInfoBrowseViewer& cp) {
  m_root = cp.m_root;
  RootToStr();
}

IDataViewWidget* TypeInfoBrowseViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iTypeInfoBrowseViewer(this, gui_parent);
}

void TypeInfoBrowseViewer::UpdateAfterEdit() {
  if (taMisc::is_loading) {
    // reget the actual guy, based on the saved string descr
    StrToRoot();
  }
  inherited::UpdateAfterEdit();
}

void TypeInfoBrowseViewer::RootToStr() {
  if (!m_root || !root_typ) {
    root_str = _nilString;
    return;
  }
  //TODO: full description of type, requires support from ta_type for searching/converting
  // for now, we only support the root typespace
  if (m_root == &taMisc::types)
    root_str = ".types";
  else
    root_str = _nilString; // TEMP
}

void TypeInfoBrowseViewer::setRoot(void* root_, TypeDef* root_typ_, MemberDef* root_md_) {
  m_root = root_;
  root_typ = root_typ_;
  root_md = root_md_;
  RootToStr();
}

void TypeInfoBrowseViewer::StrToRoot() {
  //TODO: full description of type, requires support from ta_type for searching/converting
  // for now, we only support the root typespace
  TypeItem::TypeInfoKinds tik = taMisc::TypeToTypeInfoKind(root_typ);
  if ((tik == TypeItem::TIK_TYPESPACE) && (root_str == ".types")) {
    m_root = &taMisc::types;
    root_md = TA_taMisc.members.FindName("types");
  } else {
    m_root = NULL;
  }
}

