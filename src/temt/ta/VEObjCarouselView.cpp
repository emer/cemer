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

#include "VEObjCarouselView.h"


void VEObjCarouselView::Initialize(){
  data_base = &TA_VEObjCarousel;
}

void VEObjCarouselView::Destroy() {
  CutLinks();
}

void VEObjCarouselView::SetObjCarousel(VEObjCarousel* ob) {
  if (ObjCarousel() == ob) return;
  SetData(ob);
  if (ob) {
    if (!name.contains(ob->name)) {
      SetName(ob->name);
    }
  }
}

void VEObjCarouselView::Render_pre() {
  VEObjCarousel* ob = ObjCarousel();
  if(!ob || !ob->HasBodyFlag(VEBody::FM_FILE) || !ob->obj_table) {
    VEBodyView::Render_pre();   // fall back on basic code
    return;
  }

  bool show_drag = false;
  T3ExaminerViewer* vw = GetViewer();
  if(vw)
    show_drag = vw->interactionModeOn();
  VEWorldView* wv = parent();
  if(!wv->drag_objs) show_drag = false;

  T3VEBody* obv = new T3VEBody(this, show_drag, wv->drag_size);
  setNode(obv);
  SoSeparator* ssep = obv->shapeSeparator();

  if(ob) {
    ob->MakeSwitch();           // ensures
    ssep->addChild(ob->obj_switch);
    SoTransform* tx = obv->txfm_shape();
    ob->obj_xform.CopyTo(tx);
  }

  SetDraggerPos();

  T3DataView::Render_pre();     // note: skipping over VEBodyView render!
}

void VEObjCarouselView::Render_impl() {
  VEObjCarousel* ob = ObjCarousel();
  if(!ob || !ob->HasBodyFlag(VEBody::FM_FILE) || !ob->obj_table) {
    VEBodyView::Render_impl();  // fall back on basic code
    return;
  }

  T3DataView::Render_impl();    // note: skipping over VEBodyView render!

  T3VEBody* obv = (T3VEBody*)this->node_so(); // cache
  if(!obv) return;

  SoTransform* tx = obv->transform();
  tx->translation.setValue(ob->cur_pos.x, ob->cur_pos.y, ob->cur_pos.z);
  tx->rotation.setValue(ob->cur_quat.x, ob->cur_quat.y, ob->cur_quat.z, ob->cur_quat.s);

  SoTransform* shtx = obv->txfm_shape();
  ob->obj_xform.CopyTo(shtx);

  SoSwitch* sw = ob->obj_switch;
  if(sw->getNumChildren() > 0) {
    sw->whichChild = ob->cur_obj_no;
  }
  else {
    sw->whichChild = -1;
  }
}
