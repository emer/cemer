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

#include "T3Entity.h"
#include <Qt3DInput/QMouseController>

#include <taiMisc>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;

T3Entity::T3Entity(Qt3DNode* parent)
  : QEntity(parent)
  , transform(new Qt3DCore::QTransform())
  , node_updating(false)
{
  mesh = NULL;
  material = NULL;
  mouse = NULL;
  addComponent(transform);
}

T3Entity::~T3Entity() {
  // todo: call cleanup here or not??
}

void T3Entity::addMouseInput(QMouseController* mouse_ctrl) {
  if(mouse) return;
  mouse = new QMouseInput();
  mouse->setController(mouse_ctrl);
  addComponent(mouse);
  // todo: clicked actually not implemented yet!
  // connect(mouse, SIGNAL(clicked(Q3DMouseEvent*)), this,
  //         SLOT(mouseClicked(Q3DMouseEvent*)));
  // connect(mouse, SIGNAL(doubleClicked(Q3DMouseEvent*)), this,
  //         SLOT(mouseDoubleClicked(Q3DMouseEvent*)));
  // others not needed so frequently -- add manually if wanted
  connect(mouse, SIGNAL(released(Q3DMouseEvent*)), this,
          SLOT(mouseClicked(Q3DMouseEvent*)));
}

void T3Entity::setNodeUpdating(bool updating) {
  bool should_render = false;
  if(!updating && node_updating) should_render = true;
  node_updating = updating;
  blockNotifications(node_updating); // block if updating
  if(should_render)
    emit nodeUpdatingChanged();
}

void T3Entity::TranslateXLeftTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setX(pos.x() + 0.5f * size.x() * transform->scale3D().x());
  Translate(ps);
}

void T3Entity::TranslateXRightTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setX(pos.x() - 0.5f * size.x() * transform->scale3D().x());
  Translate(ps);
}

void T3Entity::TranslateYBotTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setY(pos.y() + 0.5f * size.y() * transform->scale3D().y());
  Translate(ps);
}

void T3Entity::TranslateYTopTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setY(pos.y() - 0.5f * size.y() * transform->scale3D().y());
  Translate(ps);
}

void T3Entity::TranslateZFrontTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setZ(pos.z() - 0.5f * size.z() * transform->scale3D().z());
  Translate(ps);
}

void T3Entity::TranslateZBackTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setZ(pos.z() + 0.5f * size.z() * transform->scale3D().z());
  Translate(ps);
}

void T3Entity::TranslateLLFTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setX(pos.x() - 0.5f * size.x() * transform->scale3D().x());
  ps.setY(pos.y() - 0.5f * size.y() * transform->scale3D().y());
  ps.setZ(pos.z() - 0.5f * size.z() * transform->scale3D().z());
  Translate(ps);
}

void T3Entity::TranslateLLFSz1To(const QVector3D& pos, float width, float depth) {
  QVector3D ps = pos;
  ps.setX(pos.x() - 0.5f * width);
  ps.setY(pos.y() - 0.5f);
  ps.setZ(pos.z() - 0.5f * depth);
  Translate(ps);
}

void T3Entity::removeAllChildren() {
  // taiMisc::DeleteChildrenLater(this);
  const QObjectList& ol = children();
  for(int i = ol.count()-1; i >= 0; i--) {
    QNode* nd = dynamic_cast<QNode*>(ol.at(i));
    if(nd) {
      nd->setParent((QNode*)NULL);
      // nd->deleteLater(); // no deleting -- causes crashes -- threads still have these guys hanging around -- not sure what to do about the orphans though?  surely we'll be getting leaks?
    }
  }
}

void T3Entity::removeChildrenFrom(int idx) {
  // taiMisc::DeleteChildrenLater(this);
  const QObjectList& ol = children();
  for(int i = ol.count()-1; i >= idx; i--) {
    QNode* nd = dynamic_cast<QNode*>(ol.at(i));
    if(nd) {
      nd->setParent((QNode*)NULL);
      // nd->deleteLater(); // no deleting -- causes crashes -- threads still have these guys hanging around -- not sure what to do about the orphans though?  surely we'll be getting leaks?
    }
  }
}

