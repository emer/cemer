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

#include "BrowseViewerTaBase.h"
#include <iBrowseViewer>

BrowseViewerTaBase* BrowseViewerTaBase::New(taBase* root, MemberDef* root_md) {
  if (!root) return NULL; // nice try, buster
  BrowseViewerTaBase* rval = new BrowseViewerTaBase;
  rval->m_root = root;
  rval->root_typ = root->GetTypeDef();
  rval->root_md = root_md;
  return rval;
}

void BrowseViewerTaBase::InitLinks() {
  inherited::InitLinks();
  taBase::Own(m_root, this);
}

void BrowseViewerTaBase::CutLinks() {
  inherited::CutLinks();
}

void BrowseViewerTaBase::Copy_(const BrowseViewerTaBase& cp) {
  m_root = cp.m_root;
}

IViewerWidget* BrowseViewerTaBase::ConstrWidget_impl(QWidget* gui_parent) {
  return new iBrowseViewer(this, gui_parent);
}

void BrowseViewerTaBase::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  // if root has vanished, window must die
  //TODO: should prob propagate this up to the enclosing iMainWindowViewer???
  if (!m_root && dvwidget()) {
    CloseWindow();
  }
}


