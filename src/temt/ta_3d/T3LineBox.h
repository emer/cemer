// Copyright 2015-2018, Regents of the University of Colorado,
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

#ifndef T3LineBox_h
#define T3LineBox_h 1

// parent includes:
#include <T3ColorEntity>
#include <Qt3DRender/QGeometryRenderer>

// member includes:

// declare all other types mentioned but not required to include:

namespace Qt3DRender {
  class QBuffer;
  class QAttribute;
}

class TA_API T3LineBoxMesh : public Qt3DRender::QGeometryRenderer {
  // mesh for a 3D line box  -- good for bounding boxes etc
  Q_OBJECT
  INHERITED(Qt3DRender::QGeometryRenderer)
public:
  QVector3D     size;           // size of the frame box
    
  void  setSize(const QVector3D& sz);
  // set size and update mesh

  virtual void updateSize();
  
  explicit T3LineBoxMesh(Qt3DNode* parent = 0, const QVector3D* sz = 0);
  ~T3LineBoxMesh(); 
};


class LineBoxGeometry : public Qt3DRender::QGeometry {
  Q_OBJECT
public:
  explicit LineBoxGeometry(Qt3DCore::QNode *parent = NULL);
  ~LineBoxGeometry();

  void updateSize();

private:
  Qt3DRender::QAttribute *m_positionAttribute;
  Qt3DRender::QAttribute *m_indexAttribute;
  Qt3DRender::QBuffer *m_vertexBuffer;
  Qt3DRender::QBuffer *m_indexBuffer;
  T3LineBoxMesh* m_mesh;
};

class TA_API T3LineBox : public T3ColorEntity {
  // a 3D box of lines -- sets mesh to T3LineBoxMesh -- good for bounding boxes etc
  Q_OBJECT
  INHERITED(T3ColorEntity)
public:
  virtual void  setSize(const QVector3D& sz);
  // set new size and update
  
  void  setColor(const QColor& clr, float amb = 1.0f,
                 float spec = 0.95f, float shin = 150.0f) override
  { inherited::setColor(clr, amb, spec, shin); }
  // lines are all ambient, so change that default..

  T3LineBox(Qt3DNode* parent = 0);
  T3LineBox(Qt3DNode* parent, const QVector3D& sz);
  ~T3LineBox();

public slots:
  virtual void  updateSize(); // update to new size

protected:
  void init();
};

#endif // T3LineBox_h
