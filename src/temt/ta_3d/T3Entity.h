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

#ifndef T3Entity_h
#define T3Entity_h 1

// parent includes:
#include "ta_def.h"

#ifdef __MAKETA__

class T3Entity; // #IGNORE
class Qt3DNode; // #IGNORE

#else

// Qt3D native 3D coordinates, positive directions shown:
//     |Y
//     |   X
//     -----
//    /
//   /Z
//
// in general stuff of interest is located at the origin, and the camera is at 0,0,5 or so

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DRenderer/QAbstractMesh>
#include <Qt3DRenderer/QMaterial>
#include <QColor>

namespace Qt3D {
  class QPhongMaterial;
}

// for maketa:
typedef Qt3D::QNode Qt3DNode; 

class TA_API T3Entity : public Qt3D::QEntity {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS Qt3D entity that retains pointers to the standard components for rendering, for convenience, and automatically adds the standard transforms (scale, translate, rotation) in the constructor
  Q_OBJECT
  INHERITED(Qt3D::QEntity)
public:
  Q_PROPERTY(bool node_updating READ nodeUpdating WRITE setNodeUpdating NOTIFY nodeUpdatingChanged)
  // use this property to control notifcation of updates to a node -- calls blockNotifications(true) if setNodeUpdating(true) is called, and when the corresponding false call is made, then it also calls nodeUpdatingChanged signal, which then triggers an actual node update -- blockNotifications(false) should do this but it doesn't!

  bool  node_updating;          // is the node currently updating its structure, and thus rendering should be blocked, or not?
  virtual void setNodeUpdating(bool updating);
  bool  nodeUpdating()  { return node_updating; }
  
  Qt3D::QTransform*          transform; // overall transform applied to this node -- contains each of the following items:
  Qt3D::QScaleTransform*     scale;     // overall scale transform applied to this node
  Qt3D::QTranslateTransform* translate; // overall translation transform applied to this node
  Qt3D::QRotateTransform*    rotate;    // overall rotation transform applied to this node
  Qt3D::QAbstractMesh*       mesh;      // mesh component for this node
  Qt3D::QMaterial*           material;  // material for this node
  QVector3D                  size;      // overall size of the object (if known) -- 0 if not
  
  void  addMesh(Qt3D::QAbstractMesh* msh)
  { mesh = msh; addComponent(mesh); }
  // adds mesh component, records the last one added

  void  addMaterial(Qt3D::QMaterial* mat)
  { material = mat; addComponent(material); }
  // adds material component, records the last one added

  inline void   Translate(const QVector3D& pos)
  { translate->setTranslation(pos); }
  inline void   Scale(float sc)
  { scale->setScale(sc); }
  inline void   Scale3D(const QVector3D& sc)
  { scale->setScale3D(sc); }
  inline void   RotateDeg(const QVector3D& axis, float ang_deg)
  { rotate->setAxis(axis); rotate->setAngleDeg(ang_deg); }
  inline void   RotateRad(const QVector3D& axis, float ang_rad)
  { rotate->setAxis(axis); rotate->setAngleRad(ang_rad); }

  virtual void TranslateXLeftTo(const QVector3D& pos);
  // move the X dim left edge of object to given position -- assumes zero point position of entity is at center of object, and requires size to be set
  virtual void TranslateXRightTo(const QVector3D& pos);
  // move the X dim right edge of object to given position -- assumes zero point position of entity is at center of object, and requires size to be set
  virtual void TranslateYBotTo(const QVector3D& pos);
  // move the Y dim bottom edge of object to given position -- assumes zero point position of entity is at center of object, and requires size to be set
  virtual void TranslateYTopTo(const QVector3D& pos);
  // move the Y dim top edge of object to given position -- assumes zero point position of entity is at center of object, and requires size to be set
  virtual void TranslateZFrontTo(const QVector3D& pos);
  // move the Z dim front edge of object to given position -- assumes zero point position of entity is at center of object, and requires size to be set
  virtual void TranslateZBackTo(const QVector3D& pos);
  // move the Z dim back edge of object to given position -- assumes zero point position of entity is at center of object, and requires size to be set
  virtual void TranslateLLFTo(const QVector3D& pos);
  // move the lower-left-front point of object to given position -- assumes zero point position of entity is at center of object, and requires size to be set
  virtual void TranslateLLFSz1To(const QVector3D& pos, float width = 1.0f, float depth = 1.0f);
  // move the lower-left-front point of object to given position -- assumes zero point position of entity is at center of object, and assumes size is 1.0 (with optional variable width and depth parameters) -- this is for co-registering an entity within a unit-sized larger entity to position relative to the 0,0,0 coordinate of that larger entity

  virtual void  addChild(T3Entity* chld)
  { chld->setParent(this); }
  // add a child to this graph
  virtual void  removeAllChildren();
  // remove all the children of this node, deleting them
  
  T3Entity(Qt3DNode* parent = 0);
  ~T3Entity();

signals:
  void  nodeUpdatingChanged();
};

class TA_API T3ColorEntity : public T3Entity {
  // manages color for a T3Entity, e.g., using a Phong Material
  Q_OBJECT
  INHERITED(T3Entity)
public:
  Qt3D::QPhongMaterial* phong;  // phong material -- guaranteed to exist
  QColor        color;          // basic color of the object
  float         ambient;        // how much to diminish the color values to create ambient color -- always based off of color -- a value of 0 means ambient is black
  float         specular;       // how bright is the specular (shiny reflection) component (always white)
  float         shininess;      // how shiny is the surface -- larger values create smaller specular highlight, and vice-versa
    
  virtual void  setColor(const QColor& color, float ambient = 0.2f,
                         float specular = 0.95f, float shininess = 150.0f);
  // set the color parameters and update display
  
  T3ColorEntity(Qt3DNode* parent = 0);
  ~T3ColorEntity();

public slots:
  virtual void  updateColor(); // update to new color

};

#endif  // __MAKETA__

#endif // T3Entity_h
