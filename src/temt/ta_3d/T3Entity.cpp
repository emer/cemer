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

#include <taiMisc>

T3Entity::T3Entity(Qt3DNode* parent)
  : Qt3D::QEntity(parent)
  , transform(new Qt3D::QTransform())
  , translate(new Qt3D::QTranslateTransform())
  , scale(new Qt3D::QScaleTransform())
  , rotate(new Qt3D::QRotateTransform())
  , node_updating(false)
{
  mesh = NULL;
  material = NULL;
  // order MATTERS in setting these -- used in *reverse* order
  transform->addTransform(rotate);
  transform->addTransform(scale);
  transform->addTransform(translate);
  addComponent(transform);
}

T3Entity::~T3Entity() {
  // todo: call cleanup here or not??
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
  ps.setX(pos.x() + 0.5f * size.x() * scale->scale3D().x());
  translate->setTranslation(ps);
}

void T3Entity::TranslateXRightTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setX(pos.x() - 0.5f * size.x() * scale->scale3D().x());
  translate->setTranslation(ps);
}

void T3Entity::TranslateYBotTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setY(pos.y() + 0.5f * size.y() * scale->scale3D().y());
  translate->setTranslation(ps);
}

void T3Entity::TranslateYTopTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setY(pos.y() - 0.5f * size.y() * scale->scale3D().y());
  translate->setTranslation(ps);
}

void T3Entity::TranslateZFrontTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setZ(pos.z() - 0.5f * size.z() * scale->scale3D().z());
  translate->setTranslation(ps);
}

void T3Entity::TranslateZBackTo(const QVector3D& pos) {
  QVector3D ps = pos;
  ps.setZ(pos.z() + 0.5f * size.z() * scale->scale3D().z());
  translate->setTranslation(ps);
}

void T3Entity::removeAllChildren() {
  // taiMisc::DeleteChildrenLater(this);
  const QObjectList& ol = children();
  for(int i = ol.count()-1; i >= 0; i--) {
    Qt3D::QNode* nd = dynamic_cast<Qt3D::QNode*>(ol.at(i));
    if(nd) {
      nd->setParent((QNode*)NULL);
    }
  }
}
