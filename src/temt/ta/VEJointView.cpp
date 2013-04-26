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

#include "VEJointView.h"
#include <VEJoint>
#include <T3VEJoint>

#include <T3ExaminerViewer>
#include <VEWorldView>
#include <T3VEWorld>
#include <T3TransformBoxDragger>

#include <taMisc>

#include <QFileInfo>

#include <SoCapsule>

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>


bool VEJointView::isVisible() const {
  return (taMisc::use_gui );
}

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

  T3VEJoint* obv = new T3VEJoint(this, show_drag, wv->drag_size);
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
      if(ssep->getNumChildren() >= 5) {
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
      }
      break;
    }
    default:
      break;
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
  taVector3f nw_anc = ob->anchor;
  bod1->cur_quat_raw.RotateVec(nw_anc); // use raw here -- otherwise cylinder stuff gets in way
  nw_anc += bod1->cur_pos;
  tx->translation.setValue(nw_anc.x, nw_anc.y, nw_anc.z);

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
  // TODO: write this
//  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
//  T3VEJoint* obso = (T3VEJoint*)userData;
//  VEJointView* obv = static_cast<VEJointView*>(obso->dataView());
//  VEJoint* ob = obv->Joint();
//  VEWorldView* wv = obv->parent();
}

