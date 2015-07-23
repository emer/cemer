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

#include <Qt3DRenderer/Buffer>
#include <Qt3DRenderer/QMeshData>
#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/Attribute>

#include <T3Misc>

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

///////////////////////////

T3LineBoxMesh::T3LineBoxMesh(Qt3DNode* parent, const QVector3D* sz)
  : Qt3D::QAbstractMesh(parent)
{
  if(sz) {
    size = *sz;
  }
}

T3LineBoxMesh::~T3LineBoxMesh() {
  
}

void T3LineBoxMesh::setSize(const QVector3D& sz) {
  size = sz;
  updateSize();
}

void T3LineBoxMesh::updateSize() {
  // todo??
}
  
void T3LineBoxMesh::copy(const Qt3DNode *ref) {
    Qt3D::QAbstractMesh::copy(ref);
    const T3LineBoxMesh* mesh = static_cast<const T3LineBoxMesh*>(ref);
    size = mesh->size;
}

Qt3D::QMeshDataPtr createLineBox(const QVector3D& size);

class LineBoxFunctor : public Qt3D::QAbstractMeshFunctor {
public:
  QVector3D size;
  
  LineBoxFunctor(const T3LineBoxMesh& mesh)
    : size(mesh.size)
  {
  }

  Qt3D::QMeshDataPtr operator ()() override {
    return createLineBox(size);
  }

  bool operator ==(const Qt3D::QAbstractMeshFunctor &other) const {
    const LineBoxFunctor *otherFunctor = dynamic_cast<const LineBoxFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return (otherFunctor->size == size);
    return false;
  }
};

Qt3D::QMeshDataPtr createLineBox(const QVector3D& size) {
  const int nVerts = 8;

  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray bufferBytes;
  bufferBytes.resize(stride * nVerts);
  float* fptr = reinterpret_cast<float*>(bufferBytes.data());

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

  // vertex positions
  //    6   7
  //  2   3
  //    4   5
  //  0   1 

  // Wrap the raw bytes in a buffer
  Qt3D::BufferPtr buf(new Qt3D::Buffer(QOpenGLBuffer::VertexBuffer));
  buf->setUsage(QOpenGLBuffer::StaticDraw);
  buf->setData(bufferBytes);

  // Create the mesh data, specify the vertex format and data
  Qt3D::QMeshDataPtr mesh(new Qt3D::QMeshData(Qt3D::QMeshData::LineStrip));
  quint32 offset = 0;
  mesh->addAttribute(Qt3D::QMeshData::defaultPositionAttributeName(),
     Qt3D::AttributePtr(new Qt3D::Attribute(buf, GL_FLOAT_VEC3, nVerts, offset, stride)));

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
  
  // Wrap the index bytes in a buffer
  Qt3D::BufferPtr indexBuffer(new Qt3D::Buffer(QOpenGLBuffer::IndexBuffer));
  indexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
  indexBuffer->setData(indexBytes);

  // Specify index data on the mesh
  mesh->setIndexAttribute(Qt3D::AttributePtr(new Qt3D::Attribute
                                             (indexBuffer, GL_UNSIGNED_SHORT,
                                              indicies, 0, 0)));

  mesh->computeBoundsFromAttribute(Qt3D::QMeshData::defaultPositionAttributeName());

  return mesh;
}

Qt3D::QAbstractMeshFunctorPtr T3LineBoxMesh::meshFunctor() const {
  return Qt3D::QAbstractMeshFunctorPtr(new LineBoxFunctor(*this));
}
