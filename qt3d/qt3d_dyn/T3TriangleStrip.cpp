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

#include "T3TriangleStrip.h"

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QBufferFunctor>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QPerVertexColorMaterial>
#include <Qt3DRender/QPhongMaterial>

#include <QVector3D>

using namespace Qt3DCore;
using namespace Qt3DRender;

QByteArray createTriangleStripVertexData(int n_vndata, const float* vndata) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos + vec3 normal; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3 + 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray vertexBytes = QByteArray::fromRawData((const char*)vndata, stride * n_vndata);
  return vertexBytes;
}  

class TriangleStripVertexBufferFunctor : public QBufferFunctor {
public:
  int     n_vndata;
  const float*  vndata;
  
  TriangleStripVertexBufferFunctor(const T3TriangleStripMesh& mesh)
    : vndata(mesh.vndata.data())
  {
    // if(mesh.node_updating) {
    //   n_vndata = 0;
    // }
    // else {
    n_vndata = mesh.vertexCount();
    // }
  }

  QByteArray operator ()() override {
    return createTriangleStripVertexData(n_vndata, vndata);
  }

  bool operator ==(const QBufferFunctor &other) const override {
    return false;               // always update!!
    const TriangleStripVertexBufferFunctor *otherFunctor =
      dynamic_cast<const TriangleStripVertexBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_vndata == n_vndata) &&
              (otherFunctor->vndata == vndata));
    return false;
  }

  QT3D_FUNCTOR(TriangleStripVertexBufferFunctor)
};


QByteArray createTriangleStripIndexData(int n_indexes, const int* indexes) {
  QByteArray indexBytes = QByteArray::fromRawData((const char*)indexes,
                                                  n_indexes *  sizeof(int));
  return indexBytes;
}

class TriangleStripIndexBufferFunctor : public QBufferFunctor {
public:
  int     n_indexes;
  const int*    indexes;
  
  TriangleStripIndexBufferFunctor(const T3TriangleStripMesh& mesh)
    : indexes(mesh.indexes.data())
  {
    // if(mesh.node_updating) {
    //   n_indexes = 0;
    // }
    // else {
      n_indexes = mesh.indexCount();
    // }
  }

  QByteArray operator ()() override {
    return createTriangleStripIndexData(n_indexes, indexes);
  }

  bool operator ==(const QBufferFunctor &other) const override {
    //    return false;               // always update!!
    const TriangleStripIndexBufferFunctor *otherFunctor =
      dynamic_cast<const TriangleStripIndexBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_indexes == n_indexes) &&
              (otherFunctor->indexes == indexes));
    return false;
  }

  QT3D_FUNCTOR(TriangleStripIndexBufferFunctor)
};

QByteArray createTriangleStripColorData(int n_colors, const float* colors) {
  QByteArray colorBytes = QByteArray::fromRawData((const char*)colors,
                                                  n_colors *  4 * sizeof(float));
  return colorBytes;
}

class TriangleStripColorBufferFunctor : public QBufferFunctor {
public:
  int     n_colors;
  const float*    colors;
  
  TriangleStripColorBufferFunctor(const T3TriangleStripMesh& mesh)
    : colors(mesh.colors.data())
  {
    // if(mesh.node_updating) {
    //   n_vndata = 0;
    //   n_indexes = 0;
    // }
    // else {
    n_colors = mesh.colorCount();
    // }
  }

  QByteArray operator ()() override {
    return createTriangleStripColorData(n_colors, colors);
  }

  bool operator ==(const QBufferFunctor &other) const override {
    // return false;               // always update!!
    const TriangleStripColorBufferFunctor *otherFunctor =
      dynamic_cast<const TriangleStripColorBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_colors == n_colors) &&
              (otherFunctor->colors == colors));
    return false;
  }

  QT3D_FUNCTOR(TriangleStripColorBufferFunctor)
};


////////////////////////////////////////////////////
//      Geometry

TriangleStripGeometry::TriangleStripGeometry(QNode *parent)
  : QGeometry(parent)
  , m_mesh((T3TriangleStripMesh*)parent)
  , m_positionAttribute(new QAttribute(this))
  , m_normalAttribute(new QAttribute(this))
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
  m_positionAttribute->setByteStride(3 * 2 * sizeof(float));
  m_positionAttribute->setCount(m_mesh->vertexCount());

  m_normalAttribute->setName(QAttribute::defaultNormalAttributeName());
  m_normalAttribute->setDataType(QAttribute::Float);
  m_normalAttribute->setDataSize(3);
  m_normalAttribute->setAttributeType(QAttribute::VertexAttribute);
  m_normalAttribute->setBuffer(m_vertexBuffer);
  m_normalAttribute->setByteStride(3 * 2 * sizeof(float));
  m_normalAttribute->setByteOffset(3 * sizeof(float));
  m_normalAttribute->setCount(m_mesh->vertexCount());

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

  m_vertexBuffer->setBufferFunctor
    (QBufferFunctorPtr(new TriangleStripVertexBufferFunctor(*m_mesh)));
  m_indexBuffer->setBufferFunctor
    (QBufferFunctorPtr(new TriangleStripIndexBufferFunctor(*m_mesh)));
  m_colorBuffer->setBufferFunctor
    (QBufferFunctorPtr(new TriangleStripColorBufferFunctor(*m_mesh)));

  addAttribute(m_positionAttribute);
  addAttribute(m_normalAttribute);
  addAttribute(m_indexAttribute);
  addAttribute(m_colorAttribute);
}

TriangleStripGeometry::~TriangleStripGeometry() {
  QGeometry::cleanup();
}

void TriangleStripGeometry::updateIndices() {
  m_indexAttribute->setCount(m_mesh->indexCount());
  m_indexBuffer->setBufferFunctor
    (QBufferFunctorPtr(new TriangleStripIndexBufferFunctor(*m_mesh)));
}

void TriangleStripGeometry::updateVertices() {
  m_positionAttribute->setCount(m_mesh->vertexCount());
  m_normalAttribute->setCount(m_mesh->vertexCount());
  m_vertexBuffer->setBufferFunctor
    (QBufferFunctorPtr(new TriangleStripVertexBufferFunctor(*m_mesh)));
}
  
void TriangleStripGeometry::updateColors() {
  m_colorAttribute->setCount(m_mesh->colorCount());
  m_colorBuffer->setBufferFunctor
    (QBufferFunctorPtr(new TriangleStripColorBufferFunctor(*m_mesh)));
}

void TriangleStripGeometry::updateAll() {
  updateIndices();
  updateVertices();
  updateColors();
}

///////////////////////////////////////////////////////////////////
//              Main Mesh


T3TriangleStripMesh::T3TriangleStripMesh(QNode* parent)
  : QGeometryRenderer(parent)
{
  setPrimitiveType(TriangleStrip);
  setPrimitiveRestart(true);
  setRestartIndex(0xFFFFFFFF);
  
  TriangleStripGeometry* geometry = new TriangleStripGeometry(this);
  QGeometryRenderer::setGeometry(geometry);
}

T3TriangleStripMesh::~T3TriangleStripMesh() {
  QNode::cleanup();
}

void T3TriangleStripMesh::setNodeUpdating(bool updating) {
  bool should_render = false;
  if(!updating && node_updating) should_render = true;
  node_updating = updating;
  blockNotifications(node_updating); // block if updating
  if(should_render) {
    updateMesh();
  }
}

void T3TriangleStripMesh::restart() {
  vndata.resize(0);
  colors.resize(0);
  indexes.resize(0);
}

int T3TriangleStripMesh::addVertex(const QVector3D& pos, const QVector3D& norm) {
  int idx = vertexCount();
  vndata.append(pos.x());
  vndata.append(pos.y());
  vndata.append(pos.z());

  vndata.append(norm.x());
  vndata.append(norm.y());
  vndata.append(norm.z());
  return idx;
}

void T3TriangleStripMesh::addTriangle(int v0, int v1, int v2) {
  indexes.append(v0);
  indexes.append(v1);
  indexes.append(v2);
}

void T3TriangleStripMesh::addIndex(int v0) {
  indexes.append(v0);
}

void T3TriangleStripMesh::addBreak() {
  indexes.append(0xFFFFFFFF);        // stop
}

int T3TriangleStripMesh::addColor(const QColor& clr) {
  int idx = colorCount();
  colors.append(clr.redF());
  colors.append(clr.greenF());
  colors.append(clr.blueF());
  colors.append(clr.alphaF());
  return idx;
}

void T3TriangleStripMesh::setPointColor(int idx, const QColor& clr) {
  if(idx < 0 || idx >= colorCount()) return;
  int stidx = 4 * idx;
  colors[stidx + 0] = clr.redF();
  colors[stidx + 1] = clr.greenF();
  colors[stidx + 2] = clr.blueF();
  colors[stidx + 3] = clr.alphaF();
}

void T3TriangleStripMesh::updateMesh() {
  static_cast<TriangleStripGeometry *>(geometry())->updateAll();
}
  

