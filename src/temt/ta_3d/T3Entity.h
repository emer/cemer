// Co2018ght 2015-2017, Regents of the University of Colorado,
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
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QMaterial>
#include <Qt3DInput/QMouseHandler>
#include <QColor>

namespace Qt3DExtras {
  class QPhongMaterial;
}

// for maketa:
typedef Qt3DCore::QNode Qt3DNode; 


#include <taMath_float>

class TA_API T3Entity : public Qt3DCore::QEntity {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS Qt3D entity that retains pointers to the standard components for rendering, for convenience, and automatically adds the standard transforms (scale, translate, rotation) in the constructor
  Q_OBJECT
  INHERITED(Qt3DCore::QEntity)
public:
  Q_PROPERTY(bool node_updating READ nodeUpdating WRITE setNodeUpdating NOTIFY nodeUpdatingChanged)
  // use this property to control notifcation of updates to a node -- calls blockNotifications(true) if setNodeUpdating(true) is called, and when the corresponding false call is made, then it also calls nodeUpdatingChanged signal, which then triggers an actual node update -- blockNotifications(false) should do this but it doesn't!

  bool  node_updating;          // is the node currently updating its structure, and thus rendering should be blocked, or not?
  virtual void setNodeUpdating(bool updating);
  bool  nodeUpdating()  { return node_updating; }
  
  Qt3DCore::QTransform*          transform; // overall transform applied to this node -- contains each of the following items:
  Qt3DRender::QGeometryRenderer*   mesh;      // mesh component for this node
  Qt3DRender::QMaterial*           material;  // material for this node
  Qt3DInput::QMouseHandler*        mouse;     // mouse input object if created
  QVector3D                  size;      // overall size of the object (if known) -- 0 if not
  
  void  addMesh(Qt3DRender::QGeometryRenderer* msh)
  { mesh = msh; addComponent(mesh); }
  // adds mesh component, records the last one added

  void  addMaterial(Qt3DRender::QMaterial* mat)
  { material = mat; addComponent(material); }
  // adds material component, records the last one added

  void  removeMaterial(Qt3DRender::QMaterial* mat)
  { removeComponent(mat); if(material == mat) material = NULL; }
  // remove material component

  void  addMouseInput(Qt3DInput::QMouseDevice* mouse_dev);
  // add a QMouseHandler component and send its clicked, double-clicked signals to this object's mouse* slots -- needs the mouse_dev from the T3ExaminerViewer
  
  inline void   Translate(const QVector3D& pos)
  { transform->setTranslation(pos); }
  inline void   Translate(float dx, float dy, float dz)
  { transform->setTranslation(QVector3D(dx, dy, dz)); }
  inline void   Scale(float sc)
  { transform->setScale(sc); }
  inline void   Scale3D(const QVector3D& sc)
  { transform->setScale3D(sc); }
  inline void   Scale3D(float sx, float sy, float sz)
  { transform->setScale3D(QVector3D(sx, sy, sz)); }
  inline void   RotateDeg(const QVector3D& axis, float ang_deg)
  { transform->setRotation(Qt3DCore::QTransform::fromAxisAndAngle
                           (axis, ang_deg)); }
  inline void   RotateRad(const QVector3D& axis, float ang_rad)
  { transform->setRotation(Qt3DCore::QTransform::fromAxisAndAngle
                           (axis, taMath_float::rad_to_deg(ang_rad))); }
  inline void   RotateDeg(float ax, float ay, float az, float ang_deg)
  { RotateDeg(QVector3D(ax, ay, az), ang_deg); }
  inline void   RotateRad(float ax, float ay, float az, float ang_rad)
  { RotateRad(QVector3D(ax, ay, az), ang_rad); }

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
  virtual void  removeChildrenFrom(int idx);
  // remove all the children of this node starting from given index, deleting them (i.e., number of children will now be equal to idx)
  
  T3Entity(Qt3DNode* parent = 0);
  ~T3Entity();


public slots:
  // these are all mouse actions -- first two enabled when addMouseInput is connected -- others can be added with connect method from mouse object
  virtual void mouseClicked(Qt3DInput::QMouseEvent* mouse) { };
  virtual void mouseDoubleClicked(Qt3DInput::QMouseEvent* mouse) { };

  virtual void mouseEntered() { };
  virtual void mouseExited() { };

  virtual void mousePressed(Qt3DInput::QMouseEvent* mouse)  { };
  virtual void mouseReleased(Qt3DInput::QMouseEvent* mouse) { };
  virtual void mousePressAndHold(Qt3DInput::QMouseEvent* mouse) { };
  virtual void mousePositionChanged(Qt3DInput::QMouseEvent* mouse) { };
  virtual void mouseWheel(Qt3DInput::QWheelEvent* wheel) { };
  
signals:
  void  nodeUpdatingChanged();
};

#endif  // __MAKETA__

#endif // T3Entity_h
