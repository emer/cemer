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

#include "T3Triangles.h"

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

QByteArray createTrianglesVertexData(int n_vndata, const float* vndata) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos + vec3 normal; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3 + 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray vertexBytes = QByteArray::fromRawData((const char*)vndata, stride * n_vndata);
  return vertexBytes;
}  

class TrianglesVertexBufferDataGenerator : public QBufferDataGenerator {
public:
  int     n_vndata;
  const float*  vndata;
  
  TrianglesVertexBufferDataGenerator(const T3TrianglesMesh& mesh)
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
    return createTrianglesVertexData(n_vndata, vndata);
  }

  bool operator ==(const QBufferDataGenerator &other) const final {
    return false;               // always update!!
    const TrianglesVertexBufferDataGenerator *otherFunctor =
      dynamic_cast<const TrianglesVertexBufferDataGenerator *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_vndata == n_vndata) &&
              (otherFunctor->vndata == vndata));
    return false;
  }

  QT3D_FUNCTOR(TrianglesVertexBufferDataGenerator)
};


QByteArray createTrianglesIndexData(int n_indexes, const int* indexes) {
  QByteArray indexBytes = QByteArray::fromRawData((const char*)indexes,
                                                  n_indexes *  sizeof(int));
  return indexBytes;
}

class TrianglesIndexBufferDataGenerator : public QBufferDataGenerator {
public:
  int     n_indexes;
  const int*    indexes;
  
  TrianglesIndexBufferDataGenerator(const T3TrianglesMesh& mesh)
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
    return createTrianglesIndexData(n_indexes, indexes);
  }

  bool operator ==(const QBufferDataGenerator &other) const final {
    const TrianglesIndexBufferDataGenerator *otherFunctor =
      dynamic_cast<const TrianglesIndexBufferDataGenerator *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_indexes == n_indexes) &&
              (otherFunctor->indexes == indexes));
    return false;
  }

  QT3D_FUNCTOR(TrianglesIndexBufferDataGenerator)
};

QByteArray createTrianglesColorData(int n_colors, const float* colors) {
  QByteArray colorBytes = QByteArray::fromRawData((const char*)colors,
                                                  n_colors *  4 * sizeof(float));
  return colorBytes;
}

class TrianglesColorBufferDataGenerator : public QBufferDataGenerator {
public:
  int     n_colors;
  const float*    colors;
  
  TrianglesColorBufferDataGenerator(const T3TrianglesMesh& mesh)
    : colors(mesh.colors.el)
  {
    if(mesh.node_updating) {
      n_colors = 0;
    }
    else {
      n_colors = mesh.colorCount();
    }
  }

  QByteArray operator ()() final {
    return createTrianglesColorData(n_colors, colors);
  }

  bool operator ==(const QBufferDataGenerator &other) const final {
    return false;               // always update!!
    const TrianglesColorBufferDataGenerator *otherFunctor =
      dynamic_cast<const TrianglesColorBufferDataGenerator *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_colors == n_colors) &&
              (otherFunctor->colors == colors));
    return false;
  }

  QT3D_FUNCTOR(TrianglesColorBufferDataGenerator)
};


////////////////////////////////////////////////////
//      Geometry

TrianglesGeometry::TrianglesGeometry(QNode *parent)
  : QGeometry(parent)
  , m_mesh((T3TrianglesMesh*)parent)
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
    (QBufferDataGeneratorPtr(new TrianglesVertexBufferDataGenerator(*m_mesh)));
  m_indexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TrianglesIndexBufferDataGenerator(*m_mesh)));
  m_colorBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TrianglesColorBufferDataGenerator(*m_mesh)));

  addAttribute(m_positionAttribute);
  addAttribute(m_normalAttribute);
  addAttribute(m_colorAttribute);
  addAttribute(m_indexAttribute);
}

TrianglesGeometry::~TrianglesGeometry() {
  // QGeometry::cleanup();
}

void TrianglesGeometry::updateIndices() {
  m_indexAttribute->setCount(m_mesh->indexCount());
  m_indexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TrianglesIndexBufferDataGenerator(*m_mesh)));
}

void TrianglesGeometry::updateVertices() {
  m_positionAttribute->setCount(m_mesh->vertexCount());
  m_normalAttribute->setCount(m_mesh->vertexCount());
  m_vertexBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TrianglesVertexBufferDataGenerator(*m_mesh)));
}
  
void TrianglesGeometry::updateColors() {
  m_colorAttribute->setCount(m_mesh->colorCount());
  m_colorBuffer->setDataGenerator
    (QBufferDataGeneratorPtr(new TrianglesColorBufferDataGenerator(*m_mesh)));
}

void TrianglesGeometry::updateAll() {
  updateIndices();
  updateColors();
  updateVertices();
}

///////////////////////////////////////////////////////////////////
//              Main Mesh


T3TrianglesMesh::T3TrianglesMesh(Qt3DNode* parent)
  : inherited(parent)
{
  vndata.SetGeom(3,3,2,0);        // 0 = extensible frames
  colors.SetGeom(2,4,0);

  setPrimitiveType(Triangles);
  
  TrianglesGeometry* geometry = new TrianglesGeometry(this);
  inherited::setGeometry(geometry);
}

T3TrianglesMesh::~T3TrianglesMesh() {
  // QNode::cleanup();
}

void T3TrianglesMesh::setNodeUpdating(bool updating) {
  bool should_render = false;
  if(!updating && node_updating) should_render = true;
  node_updating = updating;
  if(should_render) {
#ifdef DEBUG
    // taMisc::Info("Triangles mesh update");
    // if(colors.Frames() > 0 && colors.Frames() != vndata.Frames()) {
      // taMisc::DebugInfo("T3TrianglesMesh: colors != vndata vertices, colors:",
      //                 String(colors.Frames()), " vs. verticies:", String(vndata.Frames()));
    // }
#endif
    blockNotifications(false);
    updateMesh();
    emit nodeUpdatingChanged();
    // taMisc::DebugInfo("tris updating:", String(updating),
    //                   "render:", String(should_render));
  }
  else {
    blockNotifications(node_updating); // block if updating
    // taMisc::DebugInfo("tris updating:", String(updating),
    //                   "render:", String(should_render));
  }
}

void T3TrianglesMesh::restart() {
  vndata.EnforceFrames(0);
  colors.EnforceFrames(0);
  indexes.SetSize(0);
}

int T3TrianglesMesh::addVertex(const QVector3D& pos, const QVector3D& norm) {
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

int T3TrianglesMesh::addVertex(const taVector3f& pos, const taVector3f& norm) {
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

void T3TrianglesMesh::addTriangle(int v0, int v1, int v2) {
  indexes.Add(v0);
  indexes.Add(v1);
  indexes.Add(v2);
}

void T3TrianglesMesh::addIndex(int v0) {
  indexes.Add(v0);
}

int T3TrianglesMesh::addColor(const QColor& clr) {
  colors.AddFrame();
  int idx = colors.Frames()-1;
  colors.Set(clr.redF(), 0, idx);
  colors.Set(clr.greenF(), 1, idx);
  colors.Set(clr.blueF(), 2, idx);
  colors.Set(clr.alphaF(), 3, idx);
  return idx;
}

void T3TrianglesMesh::setPointColor(int idx, const QColor& clr) {
  if(idx < 0 || idx >= colors.Frames()) return;
  colors.Set(clr.redF(), 0, idx);
  colors.Set(clr.greenF(), 1, idx);
  colors.Set(clr.blueF(), 2, idx);
  colors.Set(clr.alphaF(), 3, idx);
}

void T3TrianglesMesh::updateMesh() {
  static_cast<TrianglesGeometry *>(geometry())->updateAll();
}
  

///////////////////////

T3Triangles::T3Triangles(Qt3DNode* parent)
  : inherited(parent)
{
  tris = new T3TrianglesMesh();
  addMesh(tris);

  // not too big of a spot..
  shininess = 150.0f;
  specular = 0.001f;
  
  color_type = PER_VERTEX_TRANS; // default
  updateColor();
}

T3Triangles::~T3Triangles() {
  
}

void T3Triangles::setNodeUpdating(bool updating) {
  tris->setNodeUpdating(updating);
  inherited::setNodeUpdating(updating);
}

void T3Triangles::updateMesh() {
  tris->updateMesh();
}
