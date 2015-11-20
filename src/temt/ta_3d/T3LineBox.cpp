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

#include "T3LineBox.h"

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QBufferFunctor>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QPhongMaterial>

#include <T3Misc>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;


T3LineBox::T3LineBox(Qt3DNode* parent)
  : inherited(parent)
{
  size = QVector3D(1.0f, 1.0f, 1.0f);
  init();
}

T3LineBox::T3LineBox(Qt3DNode* parent, const QVector3D& sz)
  : inherited(parent)
{
  size = sz;
  init();
}

void T3LineBox::init() {
  ambient = 1.0f;               // lines are all ambient..
  addMesh(new T3LineBoxMesh(NULL, &size));
}

T3LineBox::~T3LineBox() {

}

void T3LineBox::setSize(const QVector3D& sz) {
  size = sz;
  updateSize();
}

void T3LineBox::updateSize() {
  ((T3LineBoxMesh*)mesh)->setSize(size);
}

/////////////////////////////////////////////////
//      Mesh, impl


QByteArray createLineBoxVertexData(const QVector3D& size) {
  const int nVerts = 8;

  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray vertexBytes;
  vertexBytes.resize(stride * nVerts);
  float* fptr = reinterpret_cast<float*>(vertexBytes.data());

  const float xs = size.x();
  const float ys = size.y();
  const float zs = size.z();
  const float x0 = xs * .5f;
  const float y0 = ys * .5f;
  const float z0 = zs * .5f;

  for(int z=0; z<2; z++) {
    for(int y=0; y<2; y++) {
      for(int x=0; x<2; x++) {
        *fptr++ = (float)x * xs - x0;
        *fptr++ = (float)y * ys - y0;
        *fptr++ = (float)z * zs - z0;
      }
    }
  }
  return vertexBytes;
}
  
class LineBoxVertexBufferFunctor : public QBufferFunctor {
public:
  QVector3D size;
  
  LineBoxVertexBufferFunctor(const T3LineBoxMesh& mesh)
    : size(mesh.size)
  {
  }

  QByteArray operator ()() override {
    return createLineBoxVertexData(size);
  }

  bool operator ==(const QBufferFunctor &other) const {
    const LineBoxVertexBufferFunctor *otherFunctor =
      dynamic_cast<const LineBoxVertexBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return (otherFunctor->size == size);
    return false;
  }

  QT3D_FUNCTOR(LineBoxVertexBufferFunctor)
};


QByteArray createLineBoxIndexData(const QVector3D& size) {
  // vertex positions
  //    6   7
  //  2   3
  //    4   5
  //  0   1 

  // Create the index data. 
  const int indicies = 24;
  QByteArray indexBytes;
  indexBytes.resize(indicies * sizeof(quint16));
  quint16* idxPtr = reinterpret_cast<quint16*>(indexBytes.data());

  // depends on having set 0xFFFF as glPrimitiveResetIndex in opengl context
  // faces = 6 * 2 = 12
  // front face
  *idxPtr++ = 0; *idxPtr++ = 1; *idxPtr++ = 3;  *idxPtr++ = 2;  *idxPtr++ = 0;
  *idxPtr++ = 0xFFFF; 
  // back face
  *idxPtr++ = 4; *idxPtr++ = 5; *idxPtr++ = 7;  *idxPtr++ = 6;  *idxPtr++ = 4;
  *idxPtr++ = 0xFFFF;
  // front-back connectors -- 4 x 3 = 12
  for(int i=0; i<4; i++) {      
     *idxPtr++ = i; *idxPtr++ = 4 + i; *idxPtr++ = 0xFFFF;
  }
  return indexBytes;
}

class LineBoxIndexBufferFunctor : public QBufferFunctor {
public:
  QVector3D size;
  
  LineBoxIndexBufferFunctor(const T3LineBoxMesh& mesh)
    : size(mesh.size)
  {
  }

  QByteArray operator ()() override {
    return createLineBoxIndexData(size);
  }

  bool operator ==(const QBufferFunctor &other) const {
    const LineBoxIndexBufferFunctor *otherFunctor =
      dynamic_cast<const LineBoxIndexBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return (otherFunctor->size == size);
    return false;
  }

  QT3D_FUNCTOR(LineBoxIndexBufferFunctor)
};

////////////////////////////////////////////////////
//      Geometry

class LineBoxGeometry : public QGeometry {
  // Q_OBJECT
public:
  explicit LineBoxGeometry(QNode *parent)
    : QGeometry(parent)
    , m_mesh((T3LineBoxMesh*)parent)
    , m_positionAttribute(new QAttribute(this))
    , m_indexAttribute(new QAttribute(this))
    , m_vertexBuffer(new QBuffer(QBuffer::VertexBuffer, this))
    , m_indexBuffer(new QBuffer(QBuffer::IndexBuffer, this))
  {
    m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setDataType(QAttribute::Float);
    m_positionAttribute->setDataSize(3);
    m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_positionAttribute->setBuffer(m_vertexBuffer);
    m_positionAttribute->setByteStride(3 * sizeof(float));
    m_positionAttribute->setCount(8);

    m_indexAttribute->setAttributeType(QAttribute::IndexAttribute);
    m_indexAttribute->setDataType(QAttribute::UnsignedShort);
    m_indexAttribute->setBuffer(m_indexBuffer);
    m_indexAttribute->setCount(24);

    m_vertexBuffer->setBufferFunctor
      (QBufferFunctorPtr(new LineBoxVertexBufferFunctor(*m_mesh)));
    m_indexBuffer->setBufferFunctor
      (QBufferFunctorPtr(new LineBoxIndexBufferFunctor(*m_mesh)));

    addAttribute(m_positionAttribute);
    addAttribute(m_indexAttribute);
  }

  ~LineBoxGeometry() {
    QGeometry::cleanup();
  }

  void updateSize() {
    m_vertexBuffer->setBufferFunctor
      (QBufferFunctorPtr(new LineBoxVertexBufferFunctor(*m_mesh)));
  }

private:
  QAttribute *m_positionAttribute;
  QAttribute *m_indexAttribute;
  QBuffer *m_vertexBuffer;
  QBuffer *m_indexBuffer;
  T3LineBoxMesh* m_mesh;
};


///////////////////////////////////
//      Mesh

T3LineBoxMesh::T3LineBoxMesh(Qt3DNode* parent, const QVector3D* sz)
  : inherited(parent)
{
  if(sz) {
    size = *sz;
  }

  setPrimitiveType(LineStrip);
  setPrimitiveRestart(true);
  setRestartIndex(0xFFFF);
  
  LineBoxGeometry* geometry = new LineBoxGeometry(this);
  inherited::setGeometry(geometry);
}

T3LineBoxMesh::~T3LineBoxMesh() {
  QNode::cleanup();
}

void T3LineBoxMesh::setSize(const QVector3D& sz) {
  if(size != sz) {
    size = sz;
    updateSize();
  }
}

void T3LineBoxMesh::updateSize() {
  static_cast<LineBoxGeometry *>(geometry())->updateSize();
}
