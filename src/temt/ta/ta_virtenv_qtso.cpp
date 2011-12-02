// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "ta_virtenv_qtso.h"

#include "ta_imgproc.h"
#include "ta_platform.h"
#include "NewNetViewHelper.h"

#include <QImage>
#include <QGroupBox>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <QPixmap>
#include <QLabel>
#include <QFileInfo>
#include "iflowlayout.h"

#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSwitch.h>
#include "irenderarea.h"
#include <Inventor/SbViewportRegion.h>
#include <Inventor/VRMLnodes/SoVRMLImageTexture.h>
#include <Inventor/elements/SoGLCacheContextElement.h>

#include "SoCapsule.h"

#ifdef TA_OS_WIN
// following for msvc
# ifdef near
#   undef near
# endif
# ifdef far
#   undef far
# endif
#endif

///////////////////////////////////////////////////////////////////////
//              So configure classes defined in ta_virtenv.h

void VETexture::SetTexture(SoTexture2* sotx) {
  if(fname.empty()) return;
  taMisc::TestError(this, !SoImageEx::SetTextureFile(sotx, fname),
     "Could not set texture from fname:", fname);
  if(wrap_horiz == REPEAT)
    sotx->wrapS = SoTexture2::REPEAT;
  else
    sotx->wrapS = SoTexture2::CLAMP;
  if(wrap_vert == REPEAT)
    sotx->wrapT = SoTexture2::REPEAT;
  else
    sotx->wrapT = SoTexture2::CLAMP;
  switch (mode) {
  case MODULATE:
    sotx->model = SoTexture2::MODULATE;
    break;
  case DECAL:
    sotx->model = SoTexture2::DECAL;
    break;
  case BLEND:
    sotx->model = SoTexture2::BLEND;
    break;
  case REPLACE:
    sotx->model = SoTexture2::REPLACE;
    break;
  }
  if(mode == BLEND) {
    sotx->blendColor.setValue(blend_color.r, blend_color.g, blend_color.b);
  }
}

void VETexture::SetTransform(SoTexture2Transform* sotx) {
  sotx->translation.setValue(offset.x, offset.y);
  sotx->rotation.setValue(rot * taMath_float::rad_per_deg);
  sotx->scaleFactor.setValue(scale.x, scale.y);
  sotx->center.setValue(center.x, center.y);
}

bool VETexture::UpdateTexture() {
  if(idx < 0 || fname.empty()) return false;
  bool rval = false;
  VEWorld* wrld = GET_MY_OWNER(VEWorld);
  if(!wrld) return false;
  VEWorldView* wv = wrld->FindView();
  if(!wv || !wv->node_so()) return false;
  SoSwitch* tsw = ((T3VEWorld*)wv->node_so())->getTextureSwitch();
  if(tsw) {
    if(tsw->getNumChildren() > idx) {
      SoTexture2* tex = (SoTexture2*)tsw->getChild(idx);
      SetTexture(tex);
      rval = true;
    }
  }
  tsw = ((T3VEWorld*)wv->node_so())->getTextureXformSwitch();
  if(tsw) {
    if(tsw->getNumChildren() > idx) {
      SoTexture2Transform* tex = (SoTexture2Transform*)tsw->getChild(idx);
      SetTransform(tex);
      rval = true;
    }
  }
  return rval;
}


void VECamera::ConfigCamera(SoPerspectiveCamera* cam) {
  cam->position.setValue(cur_pos.x, cur_pos.y, cur_pos.z);
  cam->orientation.setValue(SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  cam->nearDistance = this->view_dist.near;
  cam->focalDistance = view_dist.focal;
  cam->farDistance = view_dist.far;
  cam->heightAngle = field_of_view * taMath_float::rad_per_deg;
}

SoLight* VELight::CreateLight() {
  SoLight* lgt = NULL;
  switch(light_type) {
  case DIRECTIONAL_LIGHT:
    lgt = new SoDirectionalLight;
    break;
  case POINT_LIGHT:
    lgt = new SoPointLight;
    break;
  case SPOT_LIGHT:
    lgt = new SoSpotLight;
    break;
  }
  ConfigLight(lgt);
  return lgt;
}

void VELight::ConfigLight(SoLight* lgt) {
  lgt->on = light.on;
  lgt->intensity = light.intensity;
  lgt->color.setValue(light.color.r, light.color.g, light.color.b);
  switch(light_type) {
  case DIRECTIONAL_LIGHT:
    ((SoDirectionalLight*)lgt)->direction.setValue(dir_norm.x, dir_norm.y, dir_norm.z);
    break;
  case POINT_LIGHT:
    ((SoPointLight*)lgt)->location.setValue(cur_pos.x, cur_pos.y, cur_pos.z);
    break;
  case SPOT_LIGHT:
    SoSpotLight* sl = (SoSpotLight*)lgt;
    sl->direction.setValue(dir_norm.x, dir_norm.y, dir_norm.z);
    sl->location.setValue(cur_pos.x, cur_pos.y, cur_pos.z);
    sl->dropOffRate = drop_off_rate;
    sl->cutOffAngle = cut_off_angle * taMath_float::rad_per_deg;
    break;
  }
}

bool VELight::UpdateLight() {
  bool rval = false;
  VEWorld* wrld = GET_MY_OWNER(VEWorld);
  if(!wrld) return false;
  VEWorldView* wv = wrld->FindView();
  if(!wv || !wv->node_so()) return false;
  SoGroup* lg = ((T3VEWorld*)wv->node_so())->getLightGroup();
  if(!lg || lg->getNumChildren() == 0) return false;
  if(wrld->light_0.ptr() == this) {
    SoLight* lgt = (SoLight*)lg->getChild(0);
    ConfigLight(lgt);
    rval = true;
  }
  else if(wrld->light_1.ptr() == this) {
    if(lg->getNumChildren() == 2) {
      SoLight* lgt = (SoLight*)lg->getChild(1);
      ConfigLight(lgt);
    }
    else {
      SoLight* lgt = (SoLight*)lg->getChild(0);
      ConfigLight(lgt);
    }
    rval = true;
  }
  return rval;
}

///////////////////////////////////////////////////////////////////////
//              T3 DataView Guys

void VEBodyView::Initialize(){
  data_base = &TA_VEBody;
}

void VEBodyView::Destroy() {
  CutLinks();
}

void VEBodyView::SetBody(VEBody* ob) {
  if (Body() == ob) return;
  SetData(ob);
  if (ob) {
    if (!name.contains(ob->name)) {
      SetName(ob->name);
    }
  }
}

void VEBodyView::Render_pre() {
  bool show_drag = false;       // default is off!
  T3ExaminerViewer* vw = GetViewer();
  if(vw)
    show_drag = vw->interactionModeOn();
  VEWorldView* wv = parent();
  if(!wv->drag_objs) show_drag = false;

  T3VEBody* obv = new T3VEBody(this, show_drag);
  setNode(obv);
  SoSeparator* ssep = obv->shapeSeparator();

  VEBody* ob = Body();
  if(ob) {
    if(ob->HasBodyFlag(VEBody::FM_FILE) && !ob->obj_fname.empty()) {
      SoInput in;
      QFileInfo qfi(ob->obj_fname);
      if(qfi.isFile() && qfi.isReadable() && in.openFile(ob->obj_fname)) {
        SoSeparator* root = SoDB::readAll(&in);
        if (root) {
          ssep->addChild(root);
          SoTransform* tx = obv->txfm_shape();
          ob->obj_xform.CopyTo(tx);
          goto finish;
        }
      }
      cerr << "object file: " << ob->obj_fname << " not found, reverting to shape" << endl;
      // NOTE: do NOT use Info or Error here: ProcessEvents at this point is BAD
//       ob->ClearBodyFlag(VEBody::FM_FILE);
    }

    if((bool)ob->texture && wv) {
      SoSwitch* tsw = ((T3VEWorld*)wv->node_so())->getTextureSwitch();
      SoSwitch* txfsw = ((T3VEWorld*)wv->node_so())->getTextureXformSwitch();
      VETexture* vtex = ob->texture.ptr();
      int idx = vtex->GetIndex();
      if(idx >= 0 && tsw->getNumChildren() > idx) {
        SoTexture2* tex = (SoTexture2*)tsw->getChild(idx);
        ssep->addChild(tex);
        if(vtex->NeedsTransform()) {
          SoTexture2Transform* ttx = (SoTexture2Transform*)txfsw->getChild(idx);
          ssep->addChild(ttx);
        }
      }
    }

    switch(ob->shape) {
    case VEBody::SPHERE: {
      SoSphere* sp = new SoSphere;
      sp->radius = ob->radius;
      ssep->addChild(sp);
      break;
    }
    case VEBody::CAPSULE: {
      SoCapsule* sp = new SoCapsule;
      sp->radius = ob->radius;
      sp->height = ob->length;
      ssep->addChild(sp);
      break;
    }
    case VEBody::CYLINDER: {
      SoCylinder* sp = new SoCylinder;
      sp->radius = ob->radius;
      sp->height = ob->length;
      ssep->addChild(sp);
      break;
    }
    case VEBody::BOX: {
      SoCube* sp = new SoCube;
      sp->width = ob->box.x;
      sp->depth = ob->box.z;
      sp->height = ob->box.y;
      ssep->addChild(sp);
      break;
    }
    case VEBody::NO_SHAPE: {
      break;
    }
    }
  }
 finish:

  FixOrientation(true);
  SetDraggerPos();

  inherited::Render_pre();
}

void VEBodyView::FixOrientation(bool force) {
  VEBody* ob = Body();
  T3VEBody* obv = (T3VEBody*)node_so();
  if(ob && (force || ob->IsCurShape())) {// only if we are currently the right shape, incl fm file flag
    switch(ob->shape) {
    case VEBody::CAPSULE: {
      SoTransform* tx = obv->txfm_shape();
      if(ob->long_axis == VEBody::LONG_X)
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708f);
      else if(ob->long_axis == VEBody::LONG_Y)
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
      else if(ob->long_axis == VEBody::LONG_Z)
        tx->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), 1.5708f);
      break;
    }
    case VEBody::CYLINDER: {
      SoTransform* tx = obv->txfm_shape();
      if(ob->long_axis == VEBody::LONG_X)
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708f);
      else if(ob->long_axis == VEBody::LONG_Y)
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
      else if(ob->long_axis == VEBody::LONG_Z)
        tx->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), 1.5708f);
      break;
    }
    }
  }
}

void VEBodyView::SetDraggerPos() {
  T3VEBody* obv = (T3VEBody*)node_so();
  if(!obv) return;
  VEBody* ob = Body();
  if(!ob) return;

  // set dragger position
  T3TransformBoxDragger* drag = obv->getDragger();
  if(!drag) return;

  switch(ob->shape) {
  case VEBody::SPHERE: {
    drag->xf_->translation.setValue(-ob->radius, -ob->radius, ob->radius);
    break;
  }
  case VEBody::CAPSULE:
  case VEBody::CYLINDER: {
    if(ob->long_axis == VEBody::LONG_X)
      drag->xf_->translation.setValue(-ob->length*.5f, -ob->radius, ob->radius);
    else if(ob->long_axis == VEBody::LONG_Y)
      drag->xf_->translation.setValue(-ob->radius, -ob->length*.5f, ob->radius);
    else if(ob->long_axis == VEBody::LONG_Z)
      drag->xf_->translation.setValue(-ob->radius, -ob->radius, ob->length*.5f);
    break;
  }
  case VEBody::BOX: {
    drag->xf_->translation.setValue(-ob->box.x*.5f, -ob->box.y*.5f, ob->box.z*.5f);
    break;
  }
  case VEBody::NO_SHAPE: {
    break;
  }
  }
}

void VEBodyView::Render_impl() {
  inherited::Render_impl();

  T3VEBody* obv = (T3VEBody*)this->node_so(); // cache
  if(!obv) return;
  VEBody* ob = Body();
  if(!ob) return;

  SoTransform* tx = obv->transform();
  tx->translation.setValue(ob->cur_pos.x, ob->cur_pos.y, ob->cur_pos.z);
  tx->rotation.setValue(SbVec3f(ob->cur_rot.x, ob->cur_rot.y, ob->cur_rot.z), ob->cur_rot.rot);

  if(ob->set_color) {
    SoMaterial* mat = obv->material();
    mat->diffuseColor.setValue(ob->color.r, ob->color.g, ob->color.b);
    mat->transparency.setValue(1.0f - ob->color.a);

    if(ob->full_colors) {
      mat->ambientColor.setValue(ob->ambient_color.r, ob->ambient_color.g, ob->ambient_color.b);
      if(ob->specular_color.a > 0.0f) {
        mat->specularColor.setValue(ob->specular_color.r, ob->specular_color.g, ob->specular_color.b);
        mat->shininess.setValue(ob->specular_color.a);
      }
      mat->emissiveColor.setValue(ob->emissive_color.r, ob->emissive_color.g, ob->emissive_color.b);
    }
  }
  else {
    SoMaterial* mat = obv->material();
    mat->transparency.setValue(0.0f);
  }

  SoSeparator* ssep = obv->shapeSeparator();

  float off_size = 1.0e-12f;    // tiny size if it is turned off..

  if(ob->IsCurShape()) {// only if we are currently the right shape, incl fm file flag
    if(ob->HasBodyFlag(VEBody::FM_FILE)) {
      SoTransform* tx = node_so()->txfm_shape();
      if(ob->HasBodyFlag(VEBody::OFF)) {
        FloatTransform off_tx = ob->obj_xform;
        off_tx.scale.SetXYZ(off_size, off_size, off_size);
        off_tx.CopyTo(tx);
      }
      else {
        ob->obj_xform.CopyTo(tx);
      }
    }
    else {
      switch(ob->shape) {
      case VEBody::SPHERE: {
        SoSphere* sp = (SoSphere*)ssep->getChild(ssep->getNumChildren()-1); // last thing
        if(ob->HasBodyFlag(VEBody::OFF))
          sp->radius = off_size;
        else
          sp->radius = ob->radius;
        break;
      }
      case VEBody::CAPSULE: {
        SoCapsule* sp = (SoCapsule*)ssep->getChild(ssep->getNumChildren()-1); // last thing
        if(ob->HasBodyFlag(VEBody::OFF)) {
          sp->radius = off_size;
          sp->height = off_size;
        }
        else {
          sp->radius = ob->radius;
          sp->height = ob->length;
        }
        break;
      }
      case VEBody::CYLINDER: {
        SoCylinder* sp = (SoCylinder*)ssep->getChild(ssep->getNumChildren()-1); // last thing
        if(ob->HasBodyFlag(VEBody::OFF)) {
          sp->radius = off_size;
          sp->height = off_size;
        }
        else {
          sp->radius = ob->radius;
          sp->height = ob->length;
        }
        break;
      }
      case VEBody::BOX: {
        SoCube* sp = (SoCube*)ssep->getChild(ssep->getNumChildren()-1); // last thing
        if(ob->HasBodyFlag(VEBody::OFF)) {
          sp->width = off_size;
          sp->depth = off_size;
          sp->height = off_size;
        }
        else {
          sp->width = ob->box.x;
          sp->depth = ob->box.z;
          sp->height = ob->box.y;
        }
        break;
      }
      case VEBody::NO_SHAPE: {
        break;
      }
      }
    }
  }

  FixOrientation();
}

// callback for transformer dragger
void T3VEBody_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3VEBody* obso = (T3VEBody*)userData;
  VEBodyView* obv = static_cast<VEBodyView*>(obso->dataView());
  VEBody* ob = obv->Body();
  VEWorldView* wv = obv->parent();

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(ob->cur_rot.x, ob->cur_rot.y, ob->cur_rot.z), ob->cur_rot.rot);

  SbVec3f trans = dragger->translation.getValue();
//   cerr << "trans: " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  FloatTDCoord tr(trans[0], trans[1], trans[2]);
  ob->cur_pos += tr;
  ob->init_pos = ob->cur_pos;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
//   cerr << "scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
  FloatTDCoord sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
  ob->radius *= sc.x;
  ob->length *= sc.x;
  ob->box *= sc;
  ob->obj_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
//   cerr << "orient: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << endl;
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    // todo: does this need to undo cylinder/capsule stuff???
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
//     ob->FixExtRotation(nw_rot);
    ob->cur_rot.SetXYZR(axis[0], axis[1], axis[2], angle);
    ob->init_rot = ob->cur_rot;
  }

//   float h = 0.04f; // nominal amount of height, so we don't vanish
  obso->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  obso->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  obso->txfm_shape()->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  obv->FixOrientation();

  wv->UpdateDisplay();
}

//////////////////////////////////////////////////////////
//              VEObjCarousel

void VEObjCarousel::Destroy() {
  if(obj_switch) {
    obj_switch->unref();
    obj_switch = NULL;
  }
}

void VEObjCarousel::MakeSwitch() {
  if(!obj_switch) {
    obj_switch = new SoSwitch;
    obj_switch->ref();  // ref it so it hangs around
  }
}

bool VEObjCarousel::LoadObjs(bool force) {
  if(TestError(!(bool)obj_table, "LoadObjs", "obj_table is not set -- cannot load objs!"))
    return false;

  // this delay is deadly for viewing!
  SoVRMLImageTexture::setDelayFetchURL(false);

  MakeSwitch();
  SoSwitch* sw = obj_switch;

  if(!force && (sw->getNumChildren() == obj_table->rows))
    return false;               // already good

  sw->removeAllChildren();

  DataCol* fpathcol = obj_table->FindColName("FilePath", true); // yes err msg
  if(!fpathcol) return false;

  cout << "Loading ObjCarousel " << name << " object files, total n = " << obj_table->rows
       << " -- can take a long time for a large number." << endl;
  taMisc::FlushConsole();

  sw->whichChild = -1;

  for(int i=0; i< obj_table->rows; i++) {
    String fpath = fpathcol->GetValAsString(i);

    SoInput in;
    QFileInfo qfi(fpath);
    if(qfi.isFile() && qfi.isReadable() && in.openFile(fpath)) {
      cout << "Loading " << fpath << "..." << endl;
      taMisc::FlushConsole();
      SoSeparator* root = SoDB::readAll(&in);
      if (root) {
        sw->addChild(root);
        continue;
      }
    }
    taMisc::Warning("object file:", fpath, "at row:", String(i), "not found");
  }
  return true;
}

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

  T3VEBody* obv = new T3VEBody(this, show_drag);
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
  tx->rotation.setValue(SbVec3f(ob->cur_rot.x, ob->cur_rot.y, ob->cur_rot.z), ob->cur_rot.rot);

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

//////////////////////////////////////////////
//              VE Joint

void VEJointView::Initialize(){
  data_base = &TA_VEJoint;
}

void VEJointView::Destroy() {
  CutLinks();
}

void VEJointView::SetJoint(VEJoint* ob) {
  if (Joint() == ob) return;
  SetData(ob);
  if (ob) {
    if (!name.contains(ob->name)) {
      SetName(ob->name);
    }
  }
}

void VEJointView::Render_pre() {
  bool show_drag = false;
  T3ExaminerViewer* vw = GetViewer();
  if(vw)
    show_drag = vw->interactionModeOn();
  VEWorldView* wv = parent();
  if(!wv->drag_objs) show_drag = false;

  show_drag = false;            // disable for now -- not that much time on my hands

  if(!wv->show_joints) return;

  T3VEJoint* obv = new T3VEJoint(this, show_drag);
  setNode(obv);
  SoSeparator* ssep = obv->shapeSeparator();

  VEJoint* ob = Joint();
  if(ob) {
    VEBody* bod1 = ob->body1.ptr();
    if(!bod1) goto finalize;

    switch(ob->joint_type) {
    case VEJoint::BALL: {
      SoSphere* sp = new SoSphere;
      sp->radius = ob->vis_size * .5f;
      ssep->addChild(sp);
      break;
    }
    case VEJoint::HINGE:
    case VEJoint::SLIDER: {
      SoCylinder* sp = new SoCylinder;
      sp->radius = ob->vis_size * .1f;
      sp->height = ob->vis_size;
      ssep->addChild(sp);
      break;
    }
    case VEJoint::UNIVERSAL:
    case VEJoint::HINGE2: {
      SoCylinder* sp = new SoCylinder;
      sp->radius = ob->vis_size * .1f;
      sp->height = ob->vis_size;
      // put each in separate  seps with own tx's
      SoSeparator* sep2 = new SoSeparator;
      ssep->addChild(sep2);
      SoTransform* tx = new SoTransform;
      sep2->addChild(tx);
      sep2->addChild(sp);
      sep2 = new SoSeparator;
      ssep->addChild(sep2);
      tx = new SoTransform;
      sep2->addChild(tx);
      sep2->addChild(sp);
      break;
    }
    case VEJoint::FIXED: {
      SoCube* sp = new SoCube;
      sp->width = ob->vis_size *.1f;
      sp->depth = ob->vis_size *.1f;
      sp->height = ob->vis_size *.1f;
      ssep->addChild(sp);
      break;
    }
    case VEJoint::NO_JOINT: {
      break;
    }
    }
  }

 finalize:
  FixOrientation(true);
  SetDraggerPos();

  inherited::Render_pre();
}

void VEJointView::FixOrientation(bool force) {
  VEJoint* ob = Joint();
  T3VEJoint* obv = (T3VEJoint*)node_so();
  if(ob && obv && (force || ob->IsCurType())) {// only if we are currently the right type
    SoSeparator* ssep = obv->shapeSeparator();
    switch(ob->joint_type) {
    case VEJoint::HINGE:
    case VEJoint::SLIDER: {
      SoTransform* tx = obv->txfm_shape();
      SbRotation netrot;
      // construct rotation that rotates from Y axis to desired target axis
      netrot.setValue(SbVec3f(0.0f, 1.0f, 0.0f), SbVec3f(ob->axis.x, ob->axis.y, ob->axis.z));
      tx->rotation.setValue(netrot);
      break;
    }
    case VEJoint::UNIVERSAL:
    case VEJoint::HINGE2: {
      SoSeparator* sep2 = (SoSeparator*)ssep->getChild(ssep->getNumChildren()-2);
      SoTransform* tx2 = (SoTransform*)sep2->getChild(0);
      SbRotation netrot;
      // construct rotation that rotates from Y axis to desired target axis
      netrot.setValue(SbVec3f(0.0f, 1.0f, 0.0f), SbVec3f(ob->axis.x, ob->axis.y, ob->axis.z));
      tx2->rotation.setValue(netrot);
      // next joint
      sep2 = (SoSeparator*)ssep->getChild(ssep->getNumChildren()-1);
      tx2 = (SoTransform*)sep2->getChild(0);
      // construct rotation that rotates from Y axis to desired target axis
      netrot.setValue(SbVec3f(0.0f, 1.0f, 0.0f), SbVec3f(ob->axis2.x, ob->axis2.y, ob->axis2.z));
      tx2->rotation.setValue(netrot);
      break;
    }
    }
  }
}

void VEJointView::SetDraggerPos() {
  T3VEJoint* obv = (T3VEJoint*)node_so();
  if(!obv) return;
  VEJoint* ob = Joint();
  if(!ob) return;

  // set dragger position
  T3TransformBoxDragger* drag = obv->getDragger();
  if(!drag) return;

//   switch(ob->shape) {
//   case VEJoint::SPHERE: {
//     drag->xf_->translation.setValue(-ob->radius, -ob->radius, ob->radius);
//     break;
//   }
//   case VEJoint::CAPSULE:
//   case VEJoint::CYLINDER: {
//     if(ob->long_axis == VEJoint::LONG_X)
//       drag->xf_->translation.setValue(-ob->length*.5f, -ob->radius, ob->radius);
//     else if(ob->long_axis == VEJoint::LONG_Y)
//       drag->xf_->translation.setValue(-ob->radius, -ob->length*.5f, ob->radius);
//     else if(ob->long_axis == VEJoint::LONG_Z)
//       drag->xf_->translation.setValue(-ob->radius, -ob->radius, ob->length*.5f);
//     break;
//   }
//   case VEJoint::BOX: {
//     drag->xf_->translation.setValue(-ob->box.x*.5f, -ob->box.y*.5f, ob->box.z*.5f);
//     break;
//   }
//   case VEJoint::NO_SHAPE: {
//     break;
//   }
//   }
}

void VEJointView::Render_impl() {
  inherited::Render_impl();

  T3VEJoint* obv = (T3VEJoint*)this->node_so(); // cache
  if(!obv) return;
  VEJoint* ob = Joint();
  if(!ob) return;

  VEBody* bod1 = ob->body1.ptr();
  if(!bod1) return;

  SoTransform* tx = obv->transform();
  SbRotation sbrot;
  sbrot.setValue(SbVec3f(bod1->cur_rot.x, bod1->cur_rot.y, bod1->cur_rot.z), bod1->cur_rot.rot);
  // rotate the anchor too!
  SbVec3f anchor(ob->anchor.x, ob->anchor.y, ob->anchor.z);
  SbVec3f nw_anc;
  sbrot.multVec(anchor, nw_anc);
  tx->translation.setValue(bod1->cur_pos.x + nw_anc[0], bod1->cur_pos.y + nw_anc[1],
                           bod1->cur_pos.z + nw_anc[2]);
//   tx->rotation.setValue(SbVec3f(ob->cur_rot.x, ob->cur_rot.y, ob->cur_rot.z), ob->cur_rot.rot);

  SoMaterial* mat = obv->material();
  mat->diffuseColor.setValue(1.0f, 0.0f, 0.0f);
  mat->transparency.setValue(0.5f);

  SoSeparator* ssep = obv->shapeSeparator();
  if(ob->IsCurType()) {
    switch(ob->joint_type) {
    case VEJoint::BALL: {
      SoSphere* sp = (SoSphere*)ssep->getChild(ssep->getNumChildren()-1); // last thing
      sp->radius = ob->vis_size * .5f;
      break;
    }
    case VEJoint::HINGE:
    case VEJoint::SLIDER: {
      SoCylinder* sp = (SoCylinder*)ssep->getChild(ssep->getNumChildren()-1); // last thing
      sp->radius = ob->vis_size * .1f;
      sp->height = ob->vis_size;
      break;
    }
    case VEJoint::UNIVERSAL:
    case VEJoint::HINGE2: {
      SoSeparator* sep2 = (SoSeparator*)ssep->getChild(ssep->getNumChildren()-2);
      SoCylinder* sp = (SoCylinder*)sep2->getChild(sep2->getNumChildren()-1);
      sp->radius = ob->vis_size * .1f;
      sp->height = ob->vis_size;
      break;
    }
    case VEJoint::FIXED: {
      SoCube* sp = (SoCube*)ssep->getChild(ssep->getNumChildren()-1); // last thing
      sp->width = ob->vis_size *.1f;
      sp->depth = ob->vis_size *.1f;
      sp->height = ob->vis_size *.1f;
      break;
    }
    case VEJoint::NO_JOINT: {
      break;
    }
    }
  }

  FixOrientation();
}

// callback for transformer dragger
void T3VEJoint_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3VEJoint* obso = (T3VEJoint*)userData;
  VEJointView* obv = static_cast<VEJointView*>(obso->dataView());
  VEJoint* ob = obv->Joint();
  VEWorldView* wv = obv->parent();

//   SbRotation cur_rot;
//   cur_rot.setValue(SbVec3f(ob->cur_rot.x, ob->cur_rot.y, ob->cur_rot.z), ob->cur_rot.rot);

//   SbVec3f trans = dragger->translation.getValue();
// //   cerr << "trans: " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
//   cur_rot.multVec(trans, trans); // rotate the translation by current rotation
//   FloatTDCoord tr(trans[0], trans[1], trans[2]);
//   ob->cur_pos += tr;
//   ob->init_pos = ob->cur_pos;

//   const SbVec3f& scale = dragger->scaleFactor.getValue();
// //   cerr << "scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
//   FloatTDCoord sc(scale[0], scale[1], scale[2]);
//   if(sc < .1f) sc = .1f;     // prevent scale from going to small too fast!!
//   ob->radius *= sc.x;
//   ob->length *= sc.x;
//   ob->box *= sc;
//   ob->obj_xform.scale *= sc;

//   SbVec3f axis;
//   float angle;
//   dragger->rotation.getValue(axis, angle);
// //   cerr << "orient: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << endl;
//   if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
//     // todo: does this need to undo cylinder/capsule stuff???
//     SbRotation rot;
//     rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
//     SbRotation nw_rot = rot * cur_rot;
//     nw_rot.getValue(axis, angle);
// //     ob->FixExtRotation(nw_rot);
//     ob->cur_rot.SetXYZR(axis[0], axis[1], axis[2], angle);
//     ob->init_rot = ob->cur_rot;
//   }

// //   float h = 0.04f; // nominal amount of height, so we don't vanish
//   obso->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
//   obso->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
//   obso->txfm_shape()->translation.setValue(0.0f, 0.0f, 0.0f);
//   dragger->translation.setValue(0.0f, 0.0f, 0.0f);
//   dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
//   dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

//   obv->FixOrientation();

//   wv->UpdateDisplay();
}


//////////////////////////
//   VEObjectView       //
//////////////////////////

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
      taMisc::FlushConsole();
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

//////////////////////////
//   VEStaticView       //
//////////////////////////

void VEStaticView::Initialize(){
  data_base = &TA_VEStatic;
}

void VEStaticView::Destroy() {
  CutLinks();
}

void VEStaticView::SetStatic(VEStatic* ob) {
  if (Static() == ob) return;
  SetData(ob);
  if (ob) {
    if (!name.contains(ob->name)) {
      SetName(ob->name);
    }
  }
}

void VEStaticView::Render_pre() {
  bool show_drag = false;
  T3ExaminerViewer* vw = GetViewer();
  if(vw)
    show_drag = vw->interactionModeOn();
  VEWorldView* wv = parent();
  if(!wv->drag_objs) show_drag = false;

  setNode(new T3VEStatic(this, show_drag));
  SoSeparator* ssep = node_so()->shapeSeparator();

  VEStatic* ob = Static();
  if(ob) {
    if(ob->HasStaticFlag(VEStatic::FM_FILE) && !ob->obj_fname.empty()) {
      SoInput in;
      QFileInfo qfi(ob->obj_fname);
      if(qfi.isFile() && qfi.isReadable() && in.openFile(ob->obj_fname)) {
        SoSeparator* root = SoDB::readAll(&in);
        if (root) {
          ssep->addChild(root);
          SoTransform* tx = node_so()->txfm_shape();
          ob->obj_xform.CopyTo(tx);
          goto finish;
        }
      }
      cerr << "object file: " << ob->obj_fname << " not found, reverting to shape" << endl;
      // NOTE: do NOT use Info or Error here: ProcessEvents at this point is BAD
//       ob->ClearStaticFlag(VEStatic::FM_FILE);
    }

    if((bool)ob->texture && wv) {
      SoSwitch* tsw = ((T3VEWorld*)wv->node_so())->getTextureSwitch();
      SoSwitch* txfsw = ((T3VEWorld*)wv->node_so())->getTextureXformSwitch();
      VETexture* vtex = ob->texture.ptr();
      int idx = vtex->GetIndex();
      if(idx >= 0 && tsw->getNumChildren() > idx) {
        SoTexture2* tex = (SoTexture2*)tsw->getChild(idx);
        ssep->addChild(tex);
        if(vtex->NeedsTransform()) {
          SoTexture2Transform* ttx = (SoTexture2Transform*)txfsw->getChild(idx);
          ssep->addChild(ttx);
        }
      }
    }

    switch(ob->shape) {
    case VEStatic::SPHERE: {
      SoSphere* sp = new SoSphere;
      sp->radius = ob->radius;
      ssep->addChild(sp);
      break;
    }
    case VEStatic::CAPSULE: {
      SoCapsule* sp = new SoCapsule;
      sp->radius = ob->radius;
      sp->height = ob->length;
      ssep->addChild(sp);
      break;
    }
    case VEStatic::CYLINDER: {
      SoCylinder* sp = new SoCylinder;
      sp->radius = ob->radius;
      sp->height = ob->length;
      ssep->addChild(sp);
      break;
    }
    case VEStatic::BOX: {
      SoCube* sp = new SoCube;
      sp->width = ob->box.x;
      sp->depth = ob->box.z;
      sp->height = ob->box.y;
      ssep->addChild(sp);
      break;
    }
    case VEStatic::PLANE: {
      SoCube* sp = new SoCube;
      sp->width = ob->plane_vis_size.x;
      sp->depth = ob->plane_vis_size.y;
      sp->height = .01f;
      ssep->addChild(sp);
      SoTransform* tx = node_so()->txfm_shape();
      switch (ob->plane_norm) {
      case VEStatic::NORM_X:
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708f);
        break;
      case VEStatic::NORM_Y:
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
        break;
      case VEStatic::NORM_Z:
        tx->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), 1.5708f);
        break;
      }
      break;
    }
    case VEStatic::NO_SHAPE: {
      break;
    }
    }
  }
 finish:

  SetDraggerPos();
  FixOrientation(true);

  inherited::Render_pre();
}

void VEStaticView::FixOrientation(bool force) {
  VEStatic* ob = Static();
  T3VEStatic* obv = (T3VEStatic*)node_so();
  if(ob && (force || ob->IsCurShape())) {// only if we are currently the right shape, incl fm file flag
    switch(ob->shape) {
    case VEStatic::CAPSULE: {
      SoTransform* tx = obv->txfm_shape();
      if(ob->long_axis == VEStatic::LONG_X)
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708f);
      else if(ob->long_axis == VEStatic::LONG_Y)
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
      else if(ob->long_axis == VEStatic::LONG_Z)
        tx->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), 1.5708f);
      break;
    }
    case VEStatic::CYLINDER: {
      SoTransform* tx = obv->txfm_shape();
      if(ob->long_axis == VEStatic::LONG_X)
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708f);
      else if(ob->long_axis == VEStatic::LONG_Y)
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
      else if(ob->long_axis == VEStatic::LONG_Z)
        tx->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), 1.5708f);
      break;
    }
    case VEStatic::PLANE: {
      SoTransform* tx = node_so()->txfm_shape();
      switch (ob->plane_norm) {
      case VEStatic::NORM_X:
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708f);
        break;
      case VEStatic::NORM_Y:
        tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
        break;
      case VEStatic::NORM_Z:
        tx->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), 1.5708f);
        break;
      }
      break;
    }
    }
  }
}

void VEStaticView::SetDraggerPos() {
  T3VEStatic* obv = (T3VEStatic*)node_so();
  if(!obv) return;
  VEStatic* ob = Static();
  if(!ob) return;

  // set dragger position
  T3TransformBoxDragger* drag = obv->getDragger();
  if(!drag) return;
  switch(ob->shape) {
  case VEStatic::SPHERE: {
    drag->xf_->translation.setValue(-ob->radius, -ob->radius, ob->radius);
    break;
  }
  case VEStatic::CAPSULE:
  case VEStatic::CYLINDER: {
    if(ob->long_axis == VEStatic::LONG_X)
      drag->xf_->translation.setValue(-ob->length*.5f, -ob->radius, ob->radius);
    else if(ob->long_axis == VEStatic::LONG_Y)
      drag->xf_->translation.setValue(-ob->radius, -ob->length*.5f, ob->radius);
    else if(ob->long_axis == VEStatic::LONG_Z)
      drag->xf_->translation.setValue(-ob->radius, -ob->radius, ob->length*.5f);
    break;
  }
  case VEStatic::BOX: {
    drag->xf_->translation.setValue(-ob->box.x*.5f, -ob->box.y*.5f, ob->box.z*.5f);
    break;
  }
  case VEStatic::PLANE: {
    break;
  }
  case VEStatic::NO_SHAPE: {
    break;
  }
  }
}

void VEStaticView::Render_impl() {
  inherited::Render_impl();

  T3VEStatic* obv = (T3VEStatic*)this->node_so(); // cache
  if(!obv) return;
  VEStatic* ob = Static();
  if(!ob) return;

  SoTransform* tx = obv->transform();
  tx->translation.setValue(ob->pos.x, ob->pos.y, ob->pos.z);
  tx->rotation.setValue(SbVec3f(ob->rot.x, ob->rot.y, ob->rot.z), ob->rot.rot);

  if(ob->set_color) {
    SoMaterial* mat = obv->material();
    mat->diffuseColor.setValue(ob->color.r, ob->color.g, ob->color.b);
    mat->transparency.setValue(1.0f - ob->color.a);

    if(ob->full_colors) {
      mat->ambientColor.setValue(ob->ambient_color.r, ob->ambient_color.g, ob->ambient_color.b);
      if(ob->specular_color.a > 0.0f) {
        mat->specularColor.setValue(ob->specular_color.r, ob->specular_color.g, ob->specular_color.b);
        mat->shininess.setValue(ob->specular_color.a);
      }
      mat->emissiveColor.setValue(ob->emissive_color.r, ob->emissive_color.g, ob->emissive_color.b);
    }
  }

  SoSeparator* ssep = obv->shapeSeparator();

  float off_size = 1.0e-12f;    // tiny size if it is turned off..

  if(ob->IsCurShape()) {// only if we are currently the right shape, incl fm file flag
    if(ob->HasStaticFlag(VEStatic::FM_FILE)) {
      if(ob->HasStaticFlag(VEStatic::OFF)) {
        FloatTransform off_tx = ob->obj_xform;
        off_tx.scale.SetXYZ(off_size, off_size, off_size);
        off_tx.CopyTo(tx);
      }
      else {
        SoTransform* tx = obv->txfm_shape();
        ob->obj_xform.CopyTo(tx);
      }
    }
    else {
      switch(ob->shape) {
      case VEStatic::SPHERE: {
        SoSphere* sp = (SoSphere*)ssep->getChild(ssep->getNumChildren()-1); // last thing
        if(ob->HasStaticFlag(VEStatic::OFF))
          sp->radius = off_size;
        else
          sp->radius = ob->radius;
        break;
      }
      case VEStatic::CAPSULE: {
        SoCapsule* sp = (SoCapsule*)ssep->getChild(ssep->getNumChildren()-1); // last thing
        if(ob->HasStaticFlag(VEStatic::OFF)) {
          sp->radius = off_size;
          sp->height = off_size;
        }
        else {
          sp->radius = ob->radius;
          sp->height = ob->length;
        }
        break;
      }
      case VEStatic::CYLINDER: {
        SoCylinder* sp = (SoCylinder*)ssep->getChild(ssep->getNumChildren()-1); // last thing
        if(ob->HasStaticFlag(VEStatic::OFF)) {
          sp->radius = off_size;
          sp->height = off_size;
        }
        else {
          sp->radius = ob->radius;
          sp->height = ob->length;
        }
        break;
      }
      case VEStatic::BOX: {
        SoCube* sp = (SoCube*)ssep->getChild(ssep->getNumChildren()-1); // last thing
        if(ob->HasStaticFlag(VEStatic::OFF)) {
          sp->width = off_size;
          sp->depth = off_size;
          sp->height = off_size;
        }
        else {
          sp->width = ob->box.x;
          sp->depth = ob->box.z;
          sp->height = ob->box.y;
        }
        break;
      }
      case VEStatic::PLANE: {
        SoCube* sp = (SoCube*)ssep->getChild(ssep->getNumChildren()-1); // last thing
        if(ob->HasStaticFlag(VEStatic::OFF)) {
          sp->height = off_size;
          sp->width = off_size;
          sp->depth = off_size;
        }
        else {
          sp->height = .01f;
          sp->width = ob->plane_vis_size.x;
          sp->depth = ob->plane_vis_size.y;
        }
        break;
      }
      case VEStatic::NO_SHAPE: {
        break;
      }
      }
    }
  }

  FixOrientation();
}

// callback for transformer dragger
void T3VEStatic_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3VEStatic* obso = (T3VEStatic*)userData;
  VEStaticView* obv = static_cast<VEStaticView*>(obso->dataView());
  VEStatic* ob = obv->Static();
  VEWorldView* wv = obv->parent();

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(ob->rot.x, ob->rot.y, ob->rot.z), ob->rot.rot);

  SbVec3f trans = dragger->translation.getValue();
//   cerr << "trans: " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  FloatTDCoord tr(trans[0], trans[1], trans[2]);
  ob->pos += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
//   cerr << "scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
  FloatTDCoord sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
  ob->radius *= sc.x;
  ob->length *= sc.x;
  ob->box *= sc;
  ob->obj_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
//   cerr << "orient: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << endl;
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    ob->rot.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

//   float h = 0.04f; // nominal amount of height, so we don't vanish
  obso->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  obso->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  obso->txfm_shape()->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  obv->FixOrientation();

  wv->UpdateDisplay();
}

//////////////////////////
//   VESpaceView        //
//////////////////////////

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

//////////////////////////
//   VEWorld            //
//////////////////////////

VEWorldView* VEWorld::NewView(T3DataViewFrame* fr) {
  return VEWorldView::New(this, fr);
}

// Add a new VEWorldView object to the frame for the given VEWorld.
VEWorldView* VEWorldView::New(VEWorld* wl, T3DataViewFrame*& fr) {
  NewNetViewHelper new_net_view(fr, wl, "world");
  if (!new_net_view.isValid()) return NULL;

  VEWorldView* vw = new VEWorldView;
  fr->AddView(vw);
  vw->SetWorld(wl);
  // make sure we get it all setup!
  vw->BuildAll();

  new_net_view.showFrame();
  return vw;
}

VEWorldView* VEWorld::FindView() {
  taDataLink* dl = data_link();
  if(dl) {
    taDataLinkItr itr;
    VEWorldView* el;
    FOR_DLC_EL_OF_TYPE(VEWorldView, el, dl, itr) {
//       if(!el->node_so()) continue;
      return el;
    }
  }
  return NULL;
}

void VEWorld::UpdateView() {
  VEWorldView* vew = FindView();
  if(!vew) return;
  vew->UpdateDisplay(false);    // no update panel
}

//////////////////////////
//   VEWorldView        //
//////////////////////////

void VEWorldView::Initialize() {
  display_on = true;
  drag_objs = true;
  show_joints = true;
  data_base = &TA_VEWorld;
//   children.SetBaseType(&TA_VEObjectView);
  cam_renderer = NULL;
}

void VEWorldView::InitLinks() {
  inherited::InitLinks();
//   taBase::Own(table_orient, this);
}

void VEWorldView::CutLinks() {
  inherited::CutLinks();
  if(cam_renderer) delete cam_renderer;
  cam_renderer = NULL;
}

void VEWorldView::Copy_(const VEWorldView& cp) {
  display_on = cp.display_on;
  drag_objs = cp.drag_objs;
}

void VEWorldView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void VEWorldView::UpdateName() {
  VEWorld* wl = World();
  if (wl) {
    if (!name.contains(wl->name))
      SetName(wl->name + "_View");
  }
  else {
    if (name.empty())
      SetName("no_world");
  }
}

void VEWorldView::DataUpdateAfterEdit_impl() {
  UpdateName();
  inherited::DataUpdateAfterEdit_impl();
}

const String VEWorldView::caption() const {
  VEWorld* wl = World();
  String rval;
  if (wl) {
    rval = wl->GetDisplayName();
  } else rval = "(no world)";
  return rval;
}

void VEWorldView::SetWorld(VEWorld* wl) {
  if (World() == wl) return;
  if (wl) {
    SetData(wl);
    if (m_wvp) m_wvp->UpdatePanel(); // to update name
  } else {
    Unbind(); // also does kids
  }
}

void VEWorldView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_wvp) {
    m_wvp = new VEWorldViewPanel(this);
    vw->RegisterPanel(m_wvp);
  }
}

void VEWorldView::InitDisplay(bool init_panel) {
  if (init_panel) {
    InitPanel();
    UpdatePanel();
  }
  // descend into sub items
//   FOREACH_ELEM_IN_GROUP(LayerView, lv, children) {
//     InitDisplay_Layer(lv, false);
//   }
}

void VEWorldView::DataUpdateView_impl() {
  if(!display_on) return;
  UpdateDisplay(true);
}

void VEWorldView::UpdateDisplay(bool update_panel) {
  if (update_panel) UpdatePanel();
  Render_impl();
}

void VEWorldView::InitPanel() {
  if (m_wvp)
    m_wvp->InitPanel();
}

void VEWorldView::UpdatePanel() {
  if (m_wvp)
    m_wvp->UpdatePanel();
}

void VEWorldView::BuildAll() {
  Reset();
  VEWorld* wl = World();
  if(!wl) return;

  // do background spaces first..
  FOREACH_ELEM_IN_GROUP(VESpace, spc, wl->spaces) {
    VESpaceView* ov = new VESpaceView();
    ov->SetSpace(spc);
    children.Add(ov);
    ov->BuildAll();
  }

  FOREACH_ELEM_IN_GROUP(VEObject, obj, wl->objects) {
    VEObjectView* ov = new VEObjectView();
    ov->SetObject(obj);
    children.Add(ov);
    ov->BuildAll();
  }
}

void VEWorldView::Render_pre() {
  InitPanel();

  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
  }
  // this delay is deadly for viewing!
  SoVRMLImageTexture::setDelayFetchURL(false);

  setNode(new T3VEWorld(this));

  CreateLights();
  CreateTextures();

  inherited::Render_pre();
}

void VEWorldView::CreateLights() {
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  VEWorld* wl = World();

  SoGroup* lgt_group = obv->getLightGroup();
  int n_lgt = 0;
  if(wl->light_0) {
    VELight* vlgt = wl->light_0.ptr();
    SoLight* lt = vlgt->CreateLight();
    lgt_group->addChild(lt);
    n_lgt++;
  }
  if((n_lgt == 1) && (bool)wl->light_1) {
    VELight* vlgt = wl->light_1.ptr();
    SoLight* lt = vlgt->CreateLight();
    lgt_group->addChild(lt);
    n_lgt++;
  }
}

void VEWorldView::CreateTextures() {
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  VEWorld* wl = World();

  SoSwitch* texsw = obv->getTextureSwitch();
  SoSwitch* texxfsw = obv->getTextureXformSwitch();
  for(int i=0;i<wl->textures.size;i++) {
    VETexture* vtex = wl->textures.FastEl(i);
    SoTexture2* tex = new SoTexture2;
    vtex->SetTexture(tex);
    texsw->addChild(tex);

    SoTexture2Transform* texxf = new SoTexture2Transform;
    vtex->SetTransform(texxf);
    texxfsw->addChild(texxf);
  }
}

void VEWorldView::Render_impl() {
  inherited::Render_impl();

  // these tests are so the subroutines don't need them
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  if(!obv) return;
  VEWorld* wl = World();
  if(!wl) return;

  // don't set the caption!!  just gets in the way!
//   SoFont* font = obv->captionFont(true);
//   float font_size = 0.4f;
//   font->size.setValue(font_size); // is in same units as geometry units of network
//   obv->setCaption(caption().chars());

  SetupCameras();
  SetupLights();

  UpdatePanel();
}

void VEWorldView::SetupCameras() {
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  VEWorld* wl = World();

  SoSwitch* cam_switch = obv->getCameraSwitch();
  int n_cam = 0;
  VECamera* cam_light = NULL;
  if(wl->camera_0) {
    VECamera* vecam = wl->camera_0.ptr();
    if(cam_switch->getNumChildren() == 0) {
      SoPerspectiveCamera* cam = new SoPerspectiveCamera;
      vecam->ConfigCamera(cam);
      cam_switch->addChild(cam);
    }
    if(vecam->light.on)
      cam_light = vecam;
    n_cam++;
  }
  if((n_cam == 1) && (bool)wl->camera_1) {
    VECamera* vecam = wl->camera_1.ptr();
    if(cam_switch->getNumChildren() == 1) {
      SoPerspectiveCamera* cam = new SoPerspectiveCamera;
      vecam->ConfigCamera(cam);
      cam_switch->addChild(cam);
    }
    if(!cam_light && vecam->light.on)
      cam_light = vecam;
    n_cam++;
  }
  if(n_cam == 0) {
    cam_switch->removeAllChildren();
  }

  if(cam_light) {
    obv->setCamLightOn(true);
    obv->setCamLightDir(cam_light->dir_norm.x, cam_light->dir_norm.y,
                            -cam_light->dir_norm.z);
  }
  else {
    obv->setCamLightOn(false);
  }

}

void VEWorldView::SetupLights() {
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  VEWorld* wl = World();

  SoGroup* lgt_group = obv->getLightGroup();
  int n_lgt = 0;
  if(lgt_group->getNumChildren() > 0) {
    if(wl->light_0) {
      VELight* vlgt = wl->light_0.ptr();
      SoLight* lt = (SoLight*)lgt_group->getChild(0);
      vlgt->ConfigLight(lt);
      n_lgt++;
    }
    if((n_lgt == 1) && (bool)wl->light_1 && (lgt_group->getNumChildren() > 1)) {
      VELight* vlgt = wl->light_1.ptr();
      SoLight* lt = (SoLight*)lgt_group->getChild(1);
      vlgt->ConfigLight(lt);
      n_lgt++;
    }
  }

  if(wl->sun_light.on) {
    obv->setSunLightOn(true);
    obv->setSunLightDir(0.0f, -1.0f, 0.0f);
    SoDirectionalLight* slt = obv->getSunLight();
    slt->intensity = wl->sun_light.intensity;
    slt->color.setValue(wl->sun_light.color.r, wl->sun_light.color.g, wl->sun_light.color.b);
  }
  else {
    obv->setSunLightOn(false);
  }
}

// void VEWorldView::setDisplay(bool value) {
//   if (display_on == value) return;
//   display_on = value;
//   UpdateDisplay(false);              //
// }

QImage VEWorld::GetCameraImage(int cam_no) {
  QImage img;

  if(cam_no == 0) {
    if(TestError(!camera_0, "GetCameraImage", "camera_0 not set -- cannot be rendered!"))
      return img;
  }
  else if(cam_no == 1) {
    if(TestError(!camera_1, "GetCameraImage", "camera_1 not set -- cannot be rendered!"))
      return img;
  }
  else {
    TestError(true, "GetCameraImage", "only 2 cameras (0 or 1) supported!");
    return img;
  }

  taDataLink* dl = data_link();
  if(TestError(!dl, "GetCameraImage", "data link not found -- could not find views (should not happen -- please report as a bug!"))
    return img;

  taDataLinkItr itr;
  VEWorldView* el;
  FOR_DLC_EL_OF_TYPE(VEWorldView, el, dl, itr) {
    return el->GetCameraImage(cam_no);
  }

  TestError(true, "GetCameraImage", "No View of this world found -- must create View in order to get camera images");

  return img;
}

QImage VEWorldView::GetCameraImage(int cam_no) {
  QImage img;
  VEWorld* wl = World();
  if(!wl) return img;

  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  if(!obv) {
    if(taMisc::gui_no_win) {    // offscreen rendering mode -- need to build a new worldview
      BuildAll();
      Render_pre();
      Render_impl();
      Render_post();
      obv = (T3VEWorld*)this->node_so(); // cache
    }
    if(!obv) {          // still didn't work
      return img;
    }
  }

  SoSwitch* cam_switch = obv->getCameraSwitch();
  if(cam_switch->getNumChildren() <= cam_no) return img; // not ready yet

  VECamera* vecam = NULL;
  if(cam_no == 0) {
    if(TestError(!wl->camera_0, "GetCameraImage", "camera_0 not set -- cannot be rendered!"))
      return img;
    vecam = wl->camera_0.ptr();
  }
  else if(cam_no == 1) {
    if(TestError(!wl->camera_1, "GetCameraImage", "camera_1 not set -- cannot be rendered!"))
      return img;
    vecam = wl->camera_1.ptr();
  }
  else {
    TestError(true, "GetCameraImage", "only 2 cameras (0 or 1) supported!");
    return img;
  }

  TwoDCoord cur_img_size = vecam->img_size;

  SbViewportRegion vpreg;
  vpreg.setWindowSize(cur_img_size.x, cur_img_size.y);

  static TwoDCoord last_img_size;

  if(!cam_renderer) {
    cam_renderer = new SoOffscreenRendererQt(vpreg);
    SoGLRenderAction* action = cam_renderer->getGLRenderAction();
    action->setSmoothing(true);
    action->setTransparencyType(SoGLRenderAction::BLEND);

    last_img_size = cur_img_size;
  }

  if(cur_img_size != last_img_size) {
    cam_renderer->setViewportRegion(vpreg);
    last_img_size = cur_img_size;
  }

  cam_renderer->setBackgroundColor(SbColor(wl->bg_color.r, wl->bg_color.g, wl->bg_color.b));

  cam_switch->whichChild = cam_no;

  SoPerspectiveCamera* cam = (SoPerspectiveCamera*)cam_switch->getChild(cam_no);
  vecam->ConfigCamera(cam);

  // to auto compute -- probably more expensive than its worth
//   SoGetBoundingBoxAction action(vpregion);
//   action.apply(sceneroot);
//   SbBox3f box = action.getBoundingBox();

  bool ok = cam_renderer->render(obv);

  cam_switch->whichChild = -1;  // switch off for regular viewing!

  if(TestError(!ok, "GetCameraImage", "offscreen render failed!")) return img;

  img = cam_renderer->getImage();
//   img = QImage(cur_img_size.x, cur_img_size.y, QImage::Format_RGB32);

//   uchar* gbuf = (uchar*)cam_renderer->getBuffer();

//   int idx = 0;
//   if(vecam->color_cam) {
//     for(int y=cur_img_size.y-1; y>= 0; y--) {
//       for(int x=0;x<cur_img_size.x;x++) {
//      int r = gbuf[idx++]; int g = gbuf[idx++]; int b = gbuf[idx++];
//      img.setPixel(x,y, qRgb(r,g,b));
//       }
//     }
//   }
//   else {
//     for(int y=cur_img_size.y-1; y>= 0; y--) {
//       for(int x=0;x<cur_img_size.x;x++) {
//      int r = gbuf[idx++]; int g = gbuf[idx++]; int b = gbuf[idx++];
//      img.setPixel(x,y, qGray(r,g,b));
//       }
//     }
//   }

  return img;
}


bool VEWorld::GetCameraTaImage(taImage& ta_img, int cam_no) {
  QImage img = GetCameraImage(cam_no);
  bool rval = (bool)img.isNull();
  ta_img.SetImage(img);
  return rval;
}

////////////////////////////////////////////////////////////

VEWorldViewPanel::VEWorldViewPanel(VEWorldView* dv_)
:inherited(dv_)
{
  req_full_redraw = false;

//   int font_spec = taiMisc::fonMedium;
  QWidget* widg = new QWidget();
  //note: we don't set the values of all controls here, because dv does an immediate refresh
  layOuter = new QVBoxLayout(widg);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vsep_c);

  ////////////////////////////////////////////////////////////////////////////
  layDispCheck = new QHBoxLayout; layOuter->addLayout(layDispCheck);
  chkDisplay = new QCheckBox("Display", widg);
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkDisplay);

  chkDragObjs = new QCheckBox("Drag Objs", widg);
  connect(chkDragObjs, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkDragObjs);

  chkShowJoints = new QCheckBox("Show Joints", widg);
  connect(chkShowJoints, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkShowJoints);

  ////////////////////////////////////////////////////////////////////////////
  layCams = new QHBoxLayout; layOuter->addLayout(layCams);

  layCam0 = new QVBoxLayout; layCams->addLayout(layCam0);
  layCam0->setMargin(0);
  labcam0 = new QLabel(widg);
  layCam0->addWidget(labcam0);
  labcam0_txt = new QLabel(widg);
  labcam0_txt->setText("Camera 0");
  layCam0->addWidget(labcam0_txt);

  layCam1 = new QVBoxLayout; layCams->addLayout(layCam1);
  layCam1->setMargin(0);
  labcam1 = new QLabel(widg);
  layCam1->addWidget(labcam1);
  labcam1_txt = new QLabel(widg);
  labcam1_txt->setText("Camera 1");
  layCam1->addWidget(labcam1_txt);

  ////////////////////////////////////////////////////////////////////////////
  // Command Buttons
  widCmdButtons = new QWidget(widg);
  iFlowLayout* fl = new iFlowLayout(widCmdButtons);
//  layTopCtrls->addWidget(widCmdButtons);
  layOuter->addWidget(widCmdButtons);

  meth_but_mgr = new iMethodButtonMgr(widCmdButtons, fl, widCmdButtons);
  meth_but_mgr->Constr(wv()->World());

  setCentralWidget(widg);
  MakeButtons(layOuter);
}

VEWorldViewPanel::~VEWorldViewPanel() {
  VEWorldView* wv_ = wv();
  if (wv_) {
    wv_->m_wvp = NULL;
  }
}

void VEWorldViewPanel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();
  VEWorldView* wv_ = wv();
  if(!wv_) return;

  VEWorld* wl = wv_->World();
  if(!wl) return;

  if (req_full_redraw) {
    req_full_redraw = false;
    wv_->Reset();
    wv_->BuildAll();
    wv_->Render();
  }

  chkDisplay->setChecked(wv_->display_on);
  chkDragObjs->setChecked(wv_->drag_objs);
  chkShowJoints->setChecked(wv_->show_joints);

  if(wv_->display_on) {
    if(wl->camera_0) {
      QImage img = wv_->GetCameraImage(0);
      if(!img.isNull()) {
        QPixmap pm = QPixmap::fromImage(img);
        labcam0->setPixmap(pm);
      }
      else {
        labcam0->setText("Render Failed!");
      }
    }
    else {
      labcam0->setText("Not Set");
    }

    if(wl->camera_1) {
      QImage img = wv_->GetCameraImage(1);
      if(!img.isNull()) {
        QPixmap pm = QPixmap::fromImage(img);
        labcam1->setPixmap(pm);
      }
      else {
        labcam1->setText("Render Failed!");
      }
    }
    else {
      labcam1->setText("Not Set");
    }
  }
}

void VEWorldViewPanel::GetValue_impl() {
  inherited::GetValue_impl();
  VEWorldView* wv_ = wv();
  if (!wv_) return;

  wv_->display_on = chkDisplay->isChecked();
  wv_->drag_objs = chkDragObjs->isChecked();
  wv_->show_joints = chkShowJoints->isChecked();

  req_full_redraw = true;       // not worth micro-managing: MOST changes require full redraw!
}
