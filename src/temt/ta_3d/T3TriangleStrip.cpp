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

#include <Qt3DRender/Buffer>
#include <Qt3DRender/QBufferFunctor>
#include <Qt3DRender/Attribute>
#include <Qt3DRender/QMeshData>
#include <Qt3DRender/QPerVertexColorMaterial>
#include <Qt3DRender/QPhongMaterial>

#include <T3PerVertexTransMaterial>
#include <T3Misc>
#include <taMisc>


QByteArray createTriangleStripVertexData(int n_vndata, float* vndata) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos + vec3 normal; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3 + 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray vertexBytes = QByteArray::fromRawData((const char*)vndata, stride * n_vndata);
  return vertexBytes;
}  

class TriangleStripVertexBufferFunctor : public Qt3D::QBufferFunctor {
public:
  int     n_vndata;
  float*  vndata;
  
  TriangleStripVertexBufferFunctor(const T3TriangleStripMesh& mesh)
    : vndata(mesh.vndata.el)
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

  bool operator ==(const Qt3D::QBufferFunctor &other) const {
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


QByteArray createTriangleStripIndexData(int n_indexes, int* indexes) {
  QByteArray indexBytes = QByteArray::fromRawData((const char*)indexes,
                                                  n_indexes *  sizeof(int));
  return indexBytes;
}

class TriangleStripIndexBufferFunctor : public Qt3D::QBufferFunctor {
public:
  int     n_indexes;
  int*    indexes;
  
  TriangleStripIndexBufferFunctor(const T3TriangleStripMesh& mesh)
    : indexes(mesh.indexes.el)
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

  bool operator ==(const Qt3D::QBufferFunctor &other) const {
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

QByteArray createTriangleStripColorData(int n_colors, float* colors) {
  QByteArray colorBytes = QByteArray::fromRawData((const char*)colors,
                                                  n_colors *  4 * sizeof(float));
  return colorBytes;
}

class TriangleStripColorBufferFunctor : public Qt3D::QBufferFunctor {
public:
  int     n_vndata;
  float*  vndata;
  int     n_indexes;
  int*    indexes;
  int     n_colors;
  float*    colors;
  
  TriangleStripColorBufferFunctor(const T3TriangleStripMesh& mesh)
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

  QByteArray operator ()() override {
    return createTriangleStripColorData(n_colors, colors);
  }

  bool operator ==(const Qt3D::QBufferFunctor &other) const {
    return false;               // always update!!
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

class TriangleStripGeometry : public Qt3D::QGeometry {
  //  Q_OBJECT
public:
  explicit TriangleStripGeometry(Qt3D::QNode *parent)
    : Qt3D::QGeometry(parent)
    , m_mesh((T3TriangleStripMesh*)parent)
    , m_positionAttribute(new Qt3D::QAttribute(this))
    , m_normalAttribute(new Qt3D::QAttribute(this))
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
    m_positionAttribute->setByteStride(3 * 2 * sizeof(float));
    m_positionAttribute->setCount(m_mesh->vertexCount());

    m_normalAttribute->setName(Qt3D::QAttribute::defaultNormalAttributeName());
    m_normalAttribute->setDataType(Qt3D::QAttribute::Float);
    m_normalAttribute->setDataSize(3);
    m_normalAttribute->setAttributeType(Qt3D::QAttribute::VertexAttribute);
    m_normalAttribute->setBuffer(m_vertexBuffer);
    m_normalAttribute->setByteStride(3 * 2 * sizeof(float));
    m_normalAttribute->setByteOffset(3 * sizeof(float));
    m_normalAttribute->setCount(m_mesh->vertexCount());

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
      (Qt3D::QBufferFunctorPtr(new TriangleStripVertexBufferFunctor(*m_mesh)));
    m_indexBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new TriangleStripIndexBufferFunctor(*m_mesh)));
    m_colorBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new TriangleStripColorBufferFunctor(*m_mesh)));

    addAttribute(m_positionAttribute);
    addAttribute(m_normalAttribute);
    addAttribute(m_indexAttribute);
    addAttribute(m_colorAttribute);
  }

  ~TriangleStripGeometry() {
    Qt3D::QGeometry::cleanup();
  }

  void updateIndices() {
    m_indexAttribute->setCount(m_mesh->indexCount());
    m_indexBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new TriangleStripIndexBufferFunctor(*m_mesh)));
  }

  void updateVertices() {
    m_positionAttribute->setCount(m_mesh->vertexCount());
    m_normalAttribute->setCount(m_mesh->vertexCount());
    m_vertexBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new TriangleStripVertexBufferFunctor(*m_mesh)));
  }
  
  void updateColors() {
    m_colorAttribute->setCount(m_mesh->colorCount());
    m_colorBuffer->setBufferFunctor
      (Qt3D::QBufferFunctorPtr(new TriangleStripColorBufferFunctor(*m_mesh)));
  }

  void updateAll() {
    updateIndices();
    updateVertices();
    updateColors();
  }
  
private:
  Qt3D::QAttribute *m_positionAttribute;
  Qt3D::QAttribute *m_normalAttribute;
  Qt3D::QAttribute *m_indexAttribute;
  Qt3D::QAttribute *m_colorAttribute;
  Qt3D::QBuffer *m_vertexBuffer;
  Qt3D::QBuffer *m_indexBuffer;
  Qt3D::QBuffer *m_colorBuffer;
  T3TriangleStripMesh* m_mesh;
};


///////////////////////////////////////////////////////////////////
//              Main Mesh


T3TriangleStripMesh::T3TriangleStripMesh(Qt3DNode* parent)
  : inherited(parent)
{
  vndata.SetGeom(3,3,2,0);        // 0 = extensible frames
  colors.SetGeom(2,4,0);

  setPrimitiveType(TriangleStrip);
  setPrimitiveRestart(true);
  setRestartIndex(0xFFFFFFFF);
  
  TriangleStripGeometry* geometry = new TriangleStripGeometry(this);
  inherited::setGeometry(geometry);
}

T3TriangleStripMesh::~T3TriangleStripMesh() {
  Qt3D::QNode::cleanup();
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


