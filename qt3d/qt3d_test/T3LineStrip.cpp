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

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QBufferDataGenerator>
#include <Qt3DRender/QAttribute>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QPerVertexColorMaterial>

#include <QVector3D>

using namespace Qt3DCore;
using namespace Qt3DRender;

/////////////////////////////////////////////
//      rendering

QByteArray createLineStripVertexData(int n_points, const float* points) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray vertexBytes = QByteArray::fromRawData((const char*)points, stride * n_points);
  return vertexBytes;
}

class LineStripVertexBufferDataGenerator : public QBufferDataGenerator {
public:
  int     n_points;
  const float*  points;
  
  LineStripVertexBufferDataGenerator(const T3LineStripMesh& mesh)
    : points(mesh.points.data())
  {
    if(mesh.node_updating) {
      n_points = 0;
    }
    else {
      n_points = mesh.pointCount();
    }
  }

  QByteArray operator ()() final {
    return createLineStripVertexData(n_points, points);
  }

  bool operator ==(const QBufferDataGenerator &other) const final {
    // return false;               // always update!!!
    const LineStripVertexBufferDataGenerator *otherFunctor
      = dynamic_cast<const LineStripVertexBufferDataGenerator *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_points == n_points) &&
              (otherFunctor->points == points));
    return false;
  }

  QT3D_FUNCTOR(LineStripVertexBufferDataGenerator)
};

QByteArray createLineStripIndexData(int n_indexes, const int* indexes) {
  QByteArray indexBytes = QByteArray::fromRawData((const char*)indexes,
                                                  n_indexes *  sizeof(int));
  return indexBytes;
}

class LineStripIndexBufferDataGenerator : public QBufferDataGenerator {
public:
  int     n_indexes;
  const int*    indexes;
  
  LineStripIndexBufferDataGenerator(const T3LineStripMesh& mesh)
    : indexes(mesh.indexes.data())
  {
    if(mesh.node_updating) {
      n_indexes = 0;
    }
    else {
      n_indexes = mesh.indexCount();
    }
  }

  QByteArray operator ()() final {
    return createLineStripIndexData(n_indexes, indexes);
  }

  bool operator ==(const QBufferDataGenerator &other) const final {
    // return false;               // always update!!!
    const LineStripIndexBufferDataGenerator *otherFunctor
      = dynamic_cast<const LineStripIndexBufferDataGenerator *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_indexes == n_indexes) &&
              (otherFunctor->indexes == indexes));
    return false;
  }
  
  QT3D_FUNCTOR(LineStripIndexBufferDataGenerator)
};

QByteArray createLineStripColorData(int n_colors, const float* colors) {
  if(n_colors == 0) {
    return QByteArray();
  }
  QByteArray colorBytes = QByteArray::fromRawData((const char*)colors,
                                                  n_colors * 4 * sizeof(float));
  return colorBytes;
}

class LineStripColorBufferDataGenerator : public QBufferDataGenerator {
public:
  int     n_colors;
  const float*  colors;
  
  LineStripColorBufferDataGenerator(const T3LineStripMesh& mesh)
    : colors(mesh.colors.data())
  {
    if(mesh.node_updating) {
      n_colors = 0;
    }
    else {
      n_colors = mesh.colorCount();
    }
  }

  QByteArray operator ()() final {
    return createLineStripColorData(n_colors, colors);
  }

  bool operator ==(const QBufferDataGenerator &other) const final {
    // return false;               // always update!!!
    const LineStripColorBufferDataGenerator *otherFunctor
      = dynamic_cast<const LineStripColorBufferDataGenerator *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_colors == n_colors) &&
              (otherFunctor->colors == colors));
    return false;
  }
  
  QT3D_FUNCTOR(LineStripColorBufferDataGenerator)
};


////////////////////////////////////////////////////
//      Geometry

LineStripGeometry::LineStripGeometry(QNode *parent)
  : QGeometry(parent)
  , m_positionAttribute(new QAttribute(this))
  , m_mesh((T3LineStripMesh*)parent)
  , m_indexAttribute(new QAttribute(this))
  , m_colorAttribute(new QAttribute(this))
  , m_vertexBuffer(new QBuffer(QBuffer::VertexBuffer, this))
  , m_indexBuffer(new QBuffer(QBuffer::IndexBuffer, this))
  , m_colorBuffer(new QBuffer(QBuffer::VertexBuffer, this))
{
  m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
  m_positionAttribute->setDataType(QAttribute::Float);
  m_positionAttribute->setDataSize(3);
  m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
  m_positionAttribute->setBuffer(m_vertexBuffer);
  m_positionAttribute->setByteStride(3 * sizeof(float));
  m_positionAttribute->setCount(m_mesh->pointCount());

  m_indexAttribute->setAttributeType(QAttribute::IndexAttribute);
  m_indexAttribute->setDataType(QAttribute::UnsignedInt);
  m_indexAttribute->setBuffer(m_indexBuffer);
  m_indexAttribute->setCount(m_mesh->indexCount());

  m_colorAttribute->setName(QAttribute::defaultColorAttributeName());
  m_colorAttribute->setDataType(QAttribute::Float);
  m_colorAttribute->setDataSize(4);
  m_colorAttribute->setAttributeType(QAttribute::VertexAttribute);
  m_colorAttribute->setBuffer(m_colorBuffer);
  m_colorAttribute->setByteStride(4 * sizeof(float));
  m_colorAttribute->setCount(m_mesh->colorCount());

    
  m_vertexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new LineStripVertexBufferDataGenerator(*m_mesh)));
  m_indexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new LineStripIndexBufferDataGenerator(*m_mesh)));
  m_colorBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new LineStripColorBufferDataGenerator(*m_mesh)));

  addAttribute(m_positionAttribute);
  addAttribute(m_indexAttribute);
  addAttribute(m_colorAttribute);
}

LineStripGeometry::~LineStripGeometry() {
  // QGeometry::cleanup();
}

void LineStripGeometry::updateIndices() {
  m_indexAttribute->setCount(m_mesh->indexCount());
  m_indexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new LineStripIndexBufferDataGenerator(*m_mesh)));
}

void LineStripGeometry::updateVertices() {
  m_positionAttribute->setCount(m_mesh->pointCount());
  m_vertexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new LineStripVertexBufferDataGenerator(*m_mesh)));
}
  
void LineStripGeometry::updateColors() {
  m_colorAttribute->setCount(m_mesh->colorCount());
  m_colorBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new LineStripColorBufferDataGenerator(*m_mesh)));
}

void LineStripGeometry::updateAll() {
  updateIndices();
  updateVertices();
  updateColors();
}


///////////////////////////////////////////////////////////////////
//              Main Mesh

T3LineStripMesh::T3LineStripMesh(Qt3DCore::QNode* parent)
  : QGeometryRenderer(parent)
{
  setPrimitiveType(LineStrip);
  setPrimitiveRestartEnabled(true);
  setRestartIndexValue(0xFFFFFFFF);
  
  LineStripGeometry* geometry = new LineStripGeometry(this);
  setGeometry(geometry);
}

T3LineStripMesh::~T3LineStripMesh() {
  // QNode::cleanup();
}

void T3LineStripMesh::setNodeUpdating(bool updating) {
  bool should_render = false;
  if(!updating && node_updating) should_render = true;
  node_updating = updating;
  blockNotifications(node_updating); // block if updating
  if(should_render) {
    updateLines();
  }
}

void T3LineStripMesh::restart() {
  points.resize(0);
  indexes.resize(0);
  colors.resize(0);
}

int T3LineStripMesh::addPoint(const QVector3D& pos) {
  int idx = pointCount();
  points.append(pos.x());
  points.append(pos.y());
  points.append(pos.z());
  return idx;
}

void T3LineStripMesh::moveTo(const QVector3D& pos) {
  int idx = addPoint(pos);
  if(indexes.size() > 0)
    indexes.append(0xFFFFFFFF);        // stop
  indexes.append(idx);
}

void T3LineStripMesh::lineTo(const QVector3D& pos) {
  int idx = addPoint(pos);
  indexes.append(idx);
}

void T3LineStripMesh::moveToIdx(int idx) {
  if(indexes.size() > 0)
    indexes.append(0xFFFFFFFF);        // stop
  indexes.append(idx);
}

void T3LineStripMesh::lineToIdx(int idx) {
  indexes.append(idx);
}

int T3LineStripMesh::addColor(const QColor& clr) {
  int idx = colorCount();
  colors.append(clr.redF());
  colors.append(clr.greenF());
  colors.append(clr.blueF());
  colors.append(clr.alphaF());
  return idx;
}

void T3LineStripMesh::setPointColor(int idx, const QColor& clr) {
  if(idx < 0 || idx >= colorCount()) return;
  int stidx = idx * 4;
  colors[stidx + 0] = clr.redF();
  colors[stidx + 1] = clr.greenF();
  colors[stidx + 2] = clr.blueF();
  colors[stidx + 3] = clr.alphaF();
}

void T3LineStripMesh::setPoint(int idx, const QVector3D& pos) {
  if(idx < 0 || idx >= pointCount()) return;
  int stidx = idx * 3;
  points[stidx + 0] = pos.x();
  points[stidx + 1] = pos.y();
  points[stidx + 2] = pos.z();
}

void T3LineStripMesh::updateLines() {
  static_cast<LineStripGeometry *>(geometry())->updateAll();
}
  

