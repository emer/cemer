// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "NetViewObjView.h"

void NetViewObjView::Initialize(){
  data_base = &TA_NetViewObj;
}

void NetViewObjView::Destroy() {
  CutLinks();
}

void NetViewObjView::SetObj(NetViewObj* ob) {
  if (Obj() == ob) return;
  SetData(ob);
  if (ob) {
    if (!name.contains(ob->name)) {
      SetName(ob->name);
    }
  }
}

void NetViewObjView::Render_pre() {
  T3ExaminerViewer* vw = GetViewer();
  bool show_drag = false;
  if(vw)
    show_drag = vw->interactionModeOn();

  NetView* nv = GET_MY_OWNER(NetView);
  if(nv && !nv->lay_mv) show_drag = false;

  setNode(new T3NetViewObj(this, show_drag));
  SoSeparator* ssep = node_so()->shapeSeparator();

  NetViewObj* ob = Obj();
  if(ob) {
    if(ob->obj_type == NetViewObj::OBJECT) {
      int acc = access(ob->obj_fname, F_OK);
      if (acc == 0) {
        SoInput in;
        if ((access(ob->obj_fname, F_OK) == 0) && in.openFile(ob->obj_fname)) {
          SoSeparator* root = SoDB::readAll(&in);
          if (root) {
            ssep->addChild(root);
            goto finish;
          }
        }
      }
      taMisc::Warning("object file:", ob->obj_fname, "not found or unable to be loaded!");
      ob->obj_type = NetViewObj::TEXT;
      ob->text = "file: " + taMisc::GetFileFmPath(ob->obj_fname) + " not loaded!";
    }
    if(ob->obj_type == NetViewObj::TEXT) {
      SoSeparator* tsep = new SoSeparator();
      SoComplexity* cplx = new SoComplexity;
      cplx->value.setValue(taMisc::text_complexity);
      tsep->addChild(cplx);
      SoFont* fnt = new SoFont();
      fnt->name = (const char*)taMisc::t3d_font_name;
      fnt->size.setValue(ob->font_size);
      tsep->addChild(fnt);
      SoAsciiText* txt = new SoAsciiText();
      SoMFString* mfs = &(txt->string);
      mfs->setValue(ob->text.chars());
      tsep->addChild(txt);
      ssep->addChild(tsep);
    }
  }
 finish:

  inherited::Render_pre();
}

void NetViewObjView::Render_impl() {
  inherited::Render_impl();

  T3NetViewObj* node_so = (T3NetViewObj*)this->node_so(); // cache
  if(!node_so) return;
  NetViewObj* ob = Obj();
  if(!ob) return;

  SoTransform* tx = node_so->transform();
  tx->translation.setValue(ob->pos.x, ob->pos.y, ob->pos.z);
  tx->scaleFactor.setValue(ob->scale.x, ob->scale.y, ob->scale.z);
  tx->rotation.setValue(SbVec3f(ob->rot.x, ob->rot.y, ob->rot.z), ob->rot.rot);

  if(ob->set_color) {
    SoMaterial* mat = node_so->material();
    mat->diffuseColor.setValue(ob->color.r, ob->color.g, ob->color.b);
    mat->transparency.setValue(1.0f - ob->color.a);
  }
}

// callback for netview transformer dragger
void T3NetViewObj_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3NetViewObj* nvoso = (T3NetViewObj*)userData;
  NetViewObjView* nvov = static_cast<NetViewObjView*>(nvoso->dataView());
  NetViewObj* nvo = nvov->Obj();
  NetView* nv = GET_OWNER(nvov, NetView);

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nvo->rot.x, nvo->rot.y, nvo->rot.z), nvo->rot.rot);

  SbVec3f trans = dragger->translation.getValue();
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  trans[0] *= nvo->scale.x;  trans[1] *= nvo->scale.y;  trans[2] *= nvo->scale.z;
  taVector3f tr(trans[0], trans[1], trans[2]);
  nvo->pos += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
  taVector3f sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
  nvo->scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    nvo->rot.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

//   float h = 0.04f; // nominal amount of height, so we don't vanish
  nvoso->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  nvoso->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  nvoso->txfm_shape()->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  nv->UpdateDisplay();
}

