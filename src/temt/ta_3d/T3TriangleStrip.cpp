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
#include <Qt3DRender/QBufferDataGenerator>
#include <Qt3DRender/QAttribute>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DExtras/QPhongMaterial>

#include <T3PerVertexTransMaterial>
#include <T3Misc>
#include <taMisc>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;
using namespace Qt3DExtras;

QByteArray createTriangleStripVertexData(int n_vndata, const float* vndata) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos + vec3 normal; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3 + 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray vertexBytes = QByteArray::fromRawData((const char*)vndata, stride * n_vndata);
  return vertexBytes;
}  

class TriangleStripVertexBufferDataGenerator : public QBufferDataGenerator {
public:
  int     n_vndata;
  const float*  vndata;
  
  TriangleStripVertexBufferDataGenerator(const T3TriangleStripMesh& mesh)
    : vndata(mesh.vndata.el)
  {
    // if(mesh.node_updating) {
    //   n_vndata = 0;
    // }
    // else {
    n_vndata = mesh.vertexCount();
    // }
  }

  QByteArray operator ()() final {
    return createTriangleStripVertexData(n_vndata, vndata);
  }

  bool operator ==(const QBufferDataGenerator &other) const final {
    return false;               // always update!!
    const TriangleStripVertexBufferDataGenerator *otherFunctor =
      dynamic_cast<const TriangleStripVertexBufferDataGenerator *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_vndata == n_vndata) &&
              (otherFunctor->vndata == vndata));
    return false;
  }

  QT3D_FUNCTOR(TriangleStripVertexBufferDataGenerator)
};


QByteArray createTriangleStripIndexData(int n_indexes, const int* indexes) {
  QByteArray indexBytes = QByteArray::fromRawData((const char*)indexes,
                                                  n_indexes *  sizeof(int));
  return indexBytes;
}

class TriangleStripIndexBufferDataGenerator : public QBufferDataGenerator {
public:
  int     n_indexes;
  const int*    indexes;
  
  TriangleStripIndexBufferDataGenerator(const T3TriangleStripMesh& mesh)
    : indexes(mesh.indexes.el)
  {
    // if(mesh.node_updating) {
    //   n_indexes = 0;
    // }
    // else {
      n_indexes = mesh.indexCount();
    // }
  }

  QByteArray operator ()() final {
    return createTriangleStripIndexData(n_indexes, indexes);
  }

  bool operator ==(const QBufferDataGenerator &other) const final {
    const TriangleStripIndexBufferDataGenerator *otherFunctor =
      dynamic_cast<const TriangleStripIndexBufferDataGenerator *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_indexes == n_indexes) &&
              (otherFunctor->indexes == indexes));
    return false;
  }

  QT3D_FUNCTOR(TriangleStripIndexBufferDataGenerator)
};

QByteArray createTriangleStripColorData(int n_colors, const float* colors) {
  QByteArray colorBytes = QByteArray::fromRawData((const char*)colors,
                                                  n_colors *  4 * sizeof(float));
  return colorBytes;
}

class TriangleStripColorBufferDataGenerator : public QBufferDataGenerator {
public:
  int     n_colors;
  const float*    colors;
  
  TriangleStripColorBufferDataGenerator(const T3TriangleStripMesh& mesh)
    : colors(mesh.colors.el)
  {
    // if(mesh.node_updating) {
    //   n_vndata = 0;
    //   n_indexes = 0;
    // }
    // else {
    n_colors = mesh.colorCount();
    // }
  }

  QByteArray operator ()() final {
    return createTriangleStripColorData(n_colors, colors);
  }

  bool operator ==(const QBufferDataGenerator &other) const final {
    // return false;               // always update!!
    const TriangleStripColorBufferDataGenerator *otherFunctor =
      dynamic_cast<const TriangleStripColorBufferDataGenerator *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_colors == n_colors) &&
              (otherFunctor->colors == colors));
    return false;
  }

  QT3D_FUNCTOR(TriangleStripColorBufferDataGenerator)
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

  m_vertexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TriangleStripVertexBufferDataGenerator(*m_mesh)));
  m_indexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TriangleStripIndexBufferDataGenerator(*m_mesh)));
  m_colorBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TriangleStripColorBufferDataGenerator(*m_mesh)));

  addAttribute(m_positionAttribute);
  addAttribute(m_normalAttribute);
  addAttribute(m_indexAttribute);
  addAttribute(m_colorAttribute);
}

TriangleStripGeometry::~TriangleStripGeometry() {
  // QGeometry::cleanup();
}

void TriangleStripGeometry::updateIndices() {
  m_indexAttribute->setCount(m_mesh->indexCount());
  m_indexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TriangleStripIndexBufferDataGenerator(*m_mesh)));
}

void TriangleStripGeometry::updateVertices() {
  m_positionAttribute->setCount(m_mesh->vertexCount());
  m_normalAttribute->setCount(m_mesh->vertexCount());
  m_vertexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TriangleStripVertexBufferDataGenerator(*m_mesh)));
}
  
void TriangleStripGeometry::updateColors() {
  m_colorAttribute->setCount(m_mesh->colorCount());
  m_colorBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TriangleStripColorBufferDataGenerator(*m_mesh)));
}

void TriangleStripGeometry::updateAll() {
  updateIndices();
  updateVertices();
  updateColors();
}

///////////////////////////////////////////////////////////////////
//              Main Mesh


T3TriangleStripMesh::T3TriangleStripMesh(Qt3DNode* parent)
  : inherited(parent)
{
  vndata.SetGeom(3,3,2,0);        // 0 = extensible frames
  colors.SetGeom(2,4,0);

  setPrimitiveType(TriangleStrip);
  setPrimitiveRestartEnabled(true);
  setRestartIndexValue(0xFFFFFFFF);
  
  TriangleStripGeometry* geometry = new TriangleStripGeometry(this);
  inherited::setGeometry(geometry);
}

T3TriangleStripMesh::~T3TriangleStripMesh() {
  // QNode::cleanup();
}

void T3TriangleStripMesh::setNodeUpdating(bool updating) {
  bool should_render = false;
  if(!updating && node_updating) should_render = true;
  node_updating = updating;
  blockNotifications(node_updating); // block if updating
  if(should_render) {
#ifdef DEBUG
    // taMisc::Info("TriangleStrip mesh update");
    if(colors.Frames() > 0 && colors.Frames() != vndata.Frames()) {
      taMisc::Warning("T3TriangleStripMesh: colors != vndata vertices, colors:",
                      String(colors.Frames()), " vs. verticies:", String(vndata.Frames()));
    }
#endif
    updateMesh();
    emit nodeUpdatingChanged();
  }
}

void T3TriangleStripMesh::restart() {
  vndata.EnforceFrames(0);
  colors.EnforceFrames(0);
  indexes.SetSize(0);
}

int T3TriangleStripMesh::addVertex(const QVector3D& pos, const QVector3D& norm) {
  vndata.AddFrame();
  int idx = vndata.Frames()-1;
  vndata.Set(pos.x(), 0, VTX, idx);
  vndata.Set(pos.y(), 1, VTX, idx);
  vndata.Set(pos.z(), 2, VTX, idx);

  vndata.Set(norm.x(), 0, NRM, idx);
  vndata.Set(norm.y(), 1, NRM, idx);
  vndata.Set(norm.z(), 2, NRM, idx);
  return idx;
}

int T3TriangleStripMesh::addVertex(const taVector3f& pos, const taVector3f& norm) {
  vndata.AddFrame();
  int idx = vndata.Frames()-1;
  vndata.Set(pos.x, 0, VTX, idx);
  vndata.Set(pos.y, 1, VTX, idx);
  vndata.Set(pos.z, 2, VTX, idx);

  vndata.Set(norm.x, 0, NRM, idx);
  vndata.Set(norm.y, 1, NRM, idx);
  vndata.Set(norm.z, 2, NRM, idx);
  return idx;
}

void T3TriangleStripMesh::addTriangle(int v0, int v1, int v2) {
  indexes.Add(v0);
  indexes.Add(v1);
  indexes.Add(v2);
}

void T3TriangleStripMesh::addIndex(int v0) {
  indexes.Add(v0);
}

void T3TriangleStripMesh::addBreak() {
  indexes.Add(0xFFFFFFFF);        // stop
}

int T3TriangleStripMesh::addColor(const QColor& clr) {
  colors.AddFrame();
  int idx = colors.Frames()-1;
  colors.Set(clr.redF(), 0, idx);
  colors.Set(clr.greenF(), 1, idx);
  colors.Set(clr.blueF(), 2, idx);
  colors.Set(clr.alphaF(), 3, idx);
  return idx;
}

void T3TriangleStripMesh::setPointColor(int idx, const QColor& clr) {
  if(idx < 0 || idx >= colors.Frames()) return;
  colors.Set(clr.redF(), 0, idx);
  colors.Set(clr.greenF(), 1, idx);
  colors.Set(clr.blueF(), 2, idx);
  colors.Set(clr.alphaF(), 3, idx);
}

void T3TriangleStripMesh::updateMesh() {
  static_cast<TriangleStripGeometry *>(geometry())->updateAll();
}
  

///////////////////////

T3TriangleStrip::T3TriangleStrip(Qt3DNode* parent)
  : inherited(parent)
{
  tris = new T3TriangleStripMesh();
  addMesh(tris);

  // not too big of a spot..
  shininess = 10000.0f;
  specular = 0.5f;
  
  color_type = PER_VERTEX_TRANS; // default
  updateColor();
}

T3TriangleStrip::~T3TriangleStrip() {
  
}

void T3TriangleStrip::setNodeUpdating(bool updating) {
  tris->setNodeUpdating(updating);
  inherited::setNodeUpdating(updating);
}

void T3TriangleStrip::updateMesh() {
  tris->updateMesh();
}


