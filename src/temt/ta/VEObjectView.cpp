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

#include "VEObjectView.h"
#include <VEObject>
#include <T3VEObject>
#include <VEObjCarouselView>
#include <VEBodyView>
#include <VEJointView>

#include <taMisc>

taTypeDef_Of(VEObjCarousel);

bool VEObjectView::isVisible() const {
  return (taMisc::use_gui );
}

void VEObjectView::Initialize(){
  data_base = &TA_VEObject;
}

void VEObjectView::Destroy() {
  CutLinks();
}

void VEObjectView::SetObject(VEObject* ob) {
  if (Object() == ob) return;
  SetData(ob);
  if (ob) {
    if (!name.contains(ob->name)) {
      SetName(ob->name);
    }
  }
}

void VEObjectView::BuildAll() {
  Reset();
  VEObject* obj = Object();
  if(!obj) return;

  FOREACH_ELEM_IN_GROUP(VEBody, bod, obj->bodies) {
    if(bod->HasBodyFlag(VEBody::OFF)) continue;
    if(bod->InheritsFrom(&TA_VEObjCarousel)) {
      VEObjCarouselView* ov = new VEObjCarouselView();
      ov->SetObjCarousel((VEObjCarousel*)bod);
      children.Add(ov);
      ov->BuildAll();
    }
    else {
      VEBodyView* ov = new VEBodyView();
      ov->SetBody(bod);
      children.Add(ov);
      ov->BuildAll();
    }
  }

  FOREACH_ELEM_IN_GROUP(VEJoint, jnt, obj->joints) {
    if(jnt->HasJointFlag(VEJoint::OFF)) continue;
    VEJointView* ov = new VEJointView();
    ov->SetJoint(jnt);
    children.Add(ov);
    ov->BuildAll();
  }
}

void VEObjectView::Render_pre() {
  setNode(new T3VEObject(this));
  inherited::Render_pre();
}

void VEObjectView::Render_impl() {
  inherited::Render_impl();

  T3VEObject* node_so = (T3VEObject*)this->node_so(); // cache
  if(!node_so) return;
  VEObject* ob = Object();
  if(!ob) return;
}
