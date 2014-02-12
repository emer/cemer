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

#include "VESpaceView.h"
#include <VESpace>
#include <T3VESpace>
#include <VEStaticView>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(VESpaceView);


bool VESpaceView::isVisible() const {
  return (taMisc::use_gui );
}


void VESpaceView::Initialize(){
  data_base = &TA_VESpace;
}

void VESpaceView::Destroy() {
  CutLinks();
}

void VESpaceView::SetSpace(VESpace* ob) {
  if (Space() == ob) return;
  SetData(ob);
  if (ob) {
    if (!name.contains(ob->name)) {
      SetName(ob->name);
    }
  }
}

void VESpaceView::BuildAll() {
  Reset();
  VESpace* obj = Space();
  if(!obj) return;

  FOREACH_ELEM_IN_GROUP(VEStatic, bod, obj->static_els) {
    if(bod->HasStaticFlag(VEStatic::OFF)) continue;
    VEStaticView* ov = new VEStaticView();
    ov->SetStatic(bod);
    children.Add(ov);
    ov->BuildAll();
  }
}

void VESpaceView::Render_pre() {
  setNode(new T3VESpace(this));
  inherited::Render_pre();
}

void VESpaceView::Render_impl() {
  inherited::Render_impl();

  T3VESpace* obv = (T3VESpace*)this->node_so(); // cache
  if(!obv) return;
  VESpace* ob = Space();
  if(!ob) return;
}
