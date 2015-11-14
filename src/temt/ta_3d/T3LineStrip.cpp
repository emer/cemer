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

#include "T3LineStrip.h"

#include <Qt3DRender/Buffer>
#include <Qt3DRender/QBufferFunctor>
#include <Qt3DRender/Attribute>
#include <Qt3DRender/QPhongMaterial>
#include <Qt3DRender/QPerVertexColorMaterial>

#include <taMisc>

/////////////////////////////////////////////
//      rendering

QByteArray createLineStripVertexData(int n_points, float* points) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray vertexBytes = QByteArray::fromRawData((const char*)points, stride * n_points);
  return vertexBytes;
}

class LineStripVertexBufferFunctor : public Qt3D::QBufferFunctor {
public:
  int     n_points;
  float*  points;
  
  LineStripVertexBufferFunctor(const T3LineStripMesh& mesh)
    : points(mesh.points.el)
  {
    if(mesh.node_updating) {
      n_points = 0;
    }
    else {
      n_points = mesh.pointCount();
    }
  }

  QByteArray operator ()() override {
    return createLineStripVertexData(n_points, points);
  }

  bool operator ==(const Qt3D::QBufferFunctor &other) const {
    // return false;               // always update!!!
    const LineStripVertexBufferFunctor *otherFunctor
      = dynamic_cast<const LineStripVertexBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_points == n_points) &&
              (otherFunctor->points == points));
    return false;
  }

  QT3D_FUNCTOR(LineStripVertexBufferFunctor)
};

QByteArray createLineStripIndexData(int n_indexes, int* indexes) {
  QByteArray indexBytes = QByteArray::fromRawData((const char*)indexes,
                                                  n_indexes *  sizeof(int));
  return indexBytes;
}

class LineStripIndexBufferFunctor : public Qt3D::QBufferFunctor {
public:
  int     n_indexes;
  int*    indexes;
  
  LineStripIndexBufferFunctor(const T3LineStripMesh& mesh)
    : indexes(mesh.indexes.el)
  {
    if(mesh.node_updating) {
      n_indexes = 0;
    }
    else {
      n_indexes = mesh.indexCount();
    }
  }

  QByteArray operator ()() override {
    return createLineStripIndexData(n_indexes, indexes);
  }

  bool operator ==(const Qt3D::QBufferFunctor &other) const {
    // return false;               // always update!!!
    const LineStripIndexBufferFunctor *otherFunctor
      = dynamic_cast<const LineStripIndexBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_indexes == n_indexes) &&
              (otherFunctor->indexes == indexes));
    return false;
  }
  
  QT3D_FUNCTOR(LineStripIndexBufferFunctor)
};

QByteArray createLineStripColorData(int n_colors, float* colors) {
  if(n_colors == 0) {
    return QByteArray();
  }
  QByteArray colorBytes = QByteArray::fromRawData((const char*)colors,
                                                  n_colors * 4 * sizeof(float));
  return colorBytes;
}

class LineStripColorBufferFunctor : public Qt3D::QBufferFunctor {
public:
  int     n_colors;
  float*  colors;
  
  LineStripColorBufferFunctor(const T3LineStripMesh& mesh)
    : colors(mesh.colors.el)
  {
    if(mesh.node_updating) {
      n_colors = 0;
    }
    else {
      n_colors = mesh.colorCount();
    }
  }

  QByteArray operator ()() override {
    return createLineStripColorData(n_colors, colors);
  }

  bool operator ==(const Qt3D::QBufferFunctor &other) const {
    // return false;               // always update!!!
    const LineStripColorBufferFunctor *otherFunctor
      = dynamic_cast<const LineStripColorBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_colors == n_colors) &&
              (otherFunctor->colors == colors));
    return false;
  }
  
  QT3D_FUNCTOR(LineStripColorBufferFunctor)
};


////////////////////////////////////////////////////
//      Geometry

class LineStripGeometry : public Qt3D::QGeometry {
  // Q_OBJECT
public:
  explicit LineStripGeometry(Qt3D::QNode *parent)
    : Qt3D::QGeometry(parent)
    , m_mesh((T3LineStripMesh*)parent)
    , m_positionAttribute(new Qt3D::QAttribute(this))
    , m_indexAttribute(new Qt3D::QAttribute(this))
    , m_colorAttribute(new Qt3D::QAttribute(this))
    , m_vertexBuffer(new Qt3D::QBuffer(Qt3D::QBuffer::VertexBuffer, this))
    , m_indexBuffer(new Qt3D::QBuffer(Qt3D::QBuffer::IndexBuffer, this))
    , m_colorBuffer(new Qt3D::QBuffer(Qt3D::QBuffer::VertexBuffer, this))
  {
    m_positionAttribute->setName(Qt3D::QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setDataType(Qt3D::QAttribute::Float);
    m_positionAttribute->setDataSize(3);
    m_positionAttribute->setAttributeType(Qt3D::QAttribute::VertexAttribute);
    m_positionAttribute->setBuffer(m_vertexBuffer);
    m_positionAttribute->setByteStride(3 * sizeof(float));
    m_positionAttribute->setCount(m_mesh->pointCount());

    m_indexAttribute->setAttributeType(Qt3D::QAttribute::IndexAttribute);
    m_indexAttribute->setDataType(Qt3D::QAttribute::UnsignedInt);
    m_indexAttribute->setBuffer(m_indexBuffer);
    m_indexAttribute->setCount(m_mesh->indexCount());

    m_colorAttribute->setName(Qt3D::QAttribute::defaultColorAttributeName());
    m_colorAttribute->setDataType(Qt3D::QAttribute::Float);
    m_colorAttribute->setDataSize(4);
    m_colorAttribute->setAttributeType(Qt3D::QAttribute::VertexAttribute);
    m_colorAttribute->setBuffer(m_colorBuffer);
    m_colorAttribute->setByteStride(4 * sizeof(float));
    m_colorAttribute->setCount(m_mesh->colorCount());

    
    m_vertexBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new LineStripVertexBufferFunctor(*m_mesh)));
    m_indexBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new LineStripIndexBufferFunctor(*m_mesh)));
    m_colorBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new LineStripColorBufferFunctor(*m_mesh)));

    addAttribute(m_positionAttribute);
    addAttribute(m_indexAttribute);
    addAttribute(m_colorAttribute);
  }

  ~LineStripGeometry() {
    Qt3D::QGeometry::cleanup();
  }

  void updateIndices() {
    m_indexAttribute->setCount(m_mesh->indexCount());
    m_indexBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new LineStripIndexBufferFunctor(*m_mesh)));
  }

  void updateVertices() {
    m_positionAttribute->setCount(m_mesh->pointCount());
    m_vertexBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new LineStripVertexBufferFunctor(*m_mesh)));
  }
  
  void updateColors() {
    m_colorAttribute->setCount(m_mesh->colorCount());
    m_colorBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new LineStripColorBufferFunctor(*m_mesh)));
  }

  void updateAll() {
    updateIndices();
    updateVertices();
    updateColors();
  }
  
private:
  Qt3D::QAttribute *m_positionAttribute;
  Qt3D::QAttribute *m_indexAttribute;
  Qt3D::QAttribute *m_colorAttribute;
  Qt3D::QBuffer *m_vertexBuffer;
  Qt3D::QBuffer *m_indexBuffer;
  Qt3D::QBuffer *m_colorBuffer;
  T3LineStripMesh* m_mesh;
};


///////////////////////////////////////////////////////////////////
//              Main Mesh

T3LineStripMesh::T3LineStripMesh(Qt3DNode* parent)
  : inherited(parent)
{
  points.SetGeom(2,3,0);        // 0 = extensible frames
  colors.SetGeom(2,4,0);

  setPrimitiveType(LineStrip);
  setPrimitiveRestart(true);
  setRestartIndex(0xFFFFFFFF);
  
  LineStripGeometry* geometry = new LineStripGeometry(this);
  setGeometry(geometry);
}

T3LineStripMesh::~T3LineStripMesh() {
  Qt3D::QNode::cleanup();
}

void T3LineStripMesh::setNodeUpdating(bool updating) {
  bool should_render = false;
  if(!updating && node_updating) should_render = true;
  node_updating = updating;
  blockNotifications(node_updating); // block if updating
  if(should_render) {
#ifdef DEBUG    
    // taMisc::Info("LineStrip mesh update");
    if(colors.Frames() > 0 && colors.Frames() != points.Frames()) {
      taMisc::Warning("T3LineStripMesh: colors != points vertices, colors:",
                      String(colors.Frames()), " vs. points:", String(points.Frames()));
    }
#endif
    updateLines();
  }
}

void T3LineStripMesh::restart() {
  points.EnforceFrames(0);
  indexes.SetSize(0);
  colors.EnforceFrames(0);
}

int T3LineStripMesh::addPoint(const QVector3D& pos) {
  points.AddFrame();
  int idx = points.Frames()-1;
  points.Set(pos.x(), 0, idx);
  points.Set(pos.y(), 1, idx);
  points.Set(pos.z(), 2, idx);
  return idx;
}

void T3LineStripMesh::moveTo(const QVector3D& pos) {
  int idx = addPoint(pos);
  if(indexes.size > 0)
    indexes.Add(0xFFFFFFFF);        // stop
  indexes.Add(idx);
}

void T3LineStripMesh::lineTo(const QVector3D& pos) {
  int idx = addPoint(pos);
  indexes.Add(idx);
}

int T3LineStripMesh::addPoint(const taVector3f& pos) {
  points.AddFrame();
  int idx = points.Frames()-1;
  points.Set(pos.x, 0, idx);
  points.Set(pos.y, 1, idx);
  points.Set(pos.z, 2, idx);
  return idx;
}

void T3LineStripMesh::moveTo(const taVector3f& pos) {
  int idx = addPoint(pos);
  if(indexes.size > 0)
    indexes.Add(0xFFFFFFFF);        // stop
  indexes.Add(idx);
}

void T3LineStripMesh::lineTo(const taVector3f& pos) {
  int idx = addPoint(pos);
  indexes.Add(idx);
}

void T3LineStripMesh::moveToIdx(int idx) {
  if(indexes.size > 0)
    indexes.Add(0xFFFFFFFF);        // stop
  indexes.Add(idx);
}

void T3LineStripMesh::lineToIdx(int idx) {
  indexes.Add(idx);
}

int T3LineStripMesh::addColor(const QColor& clr) {
  colors.AddFrame();
  int idx = colors.Frames()-1;
  colors.Set(clr.redF(), 0, idx);
  colors.Set(clr.greenF(), 1, idx);
  colors.Set(clr.blueF(), 2, idx);
  colors.Set(clr.alphaF(), 3, idx);
  return idx;
}

void T3LineStripMesh::setPointColor(int idx, const QColor& clr) {
  if(idx < 0 || idx >= colors.Frames()) return;
  colors.Set(clr.redF(), 0, idx);
  colors.Set(clr.greenF(), 1, idx);
  colors.Set(clr.blueF(), 2, idx);
  colors.Set(clr.alphaF(), 3, idx);
}

void T3LineStripMesh::setPoint(int idx, const QVector3D& pos) {
  if(idx < 0 || idx >= points.Frames()) return;
  points.Set(pos.x(), 0, idx);
  points.Set(pos.y(), 1, idx);
  points.Set(pos.z(), 2, idx);
}

void T3LineStripMesh::setPoint(int idx, const taVector3f& pos) {
  if(idx < 0 || idx >= points.Frames()) return;
  points.Set(pos.x, 0, idx);
  points.Set(pos.y, 1, idx);
  points.Set(pos.z, 2, idx);
}

void T3LineStripMesh::updateLines() {
  static_cast<LineStripGeometry *>(geometry())->updateAll();
}
  

///////////////////////

T3LineStrip::T3LineStrip(Qt3DNode* parent)
  : inherited(parent)
{
  line_width = 1.0f;
  ambient = 1.0f;               // lines are all ambient..
  per_vertex_color = false;
  lines = new T3LineStripMesh();
  addMesh(lines);
}

T3LineStrip::~T3LineStrip() {
  
}

void T3LineStrip::setNodeUpdating(bool updating) {
  lines->setNodeUpdating(updating);
  inherited::setNodeUpdating(updating);
}

void T3LineStrip::updateLines() {
  lines->updateLines();
}

void T3LineStrip::setPerVertexColor(bool per_vtx) {
  per_vertex_color = per_vtx;
  if(per_vertex_color) {
    color_type = PER_VERTEX;
    updateColor();
  }
  else {
    color_type = PHONG;
    updateColor();
  }
}
