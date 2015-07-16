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

#include "T3Frame.h"

#include <Qt3DRenderer/Buffer>
#include <Qt3DRenderer/QMeshData>


T3Frame::T3Frame(Qt3DNode* parent)
  : inherited(parent)
{
  addMesh(new T3FrameMesh(NULL, &size));

  Qt3D::QPhongMaterial* mt = new Qt3D::QPhongMaterial();
  mt->setAmbient(QColor(255, 80, 80));
  mt->setDiffuse(QColor(255, 80, 80));
  addMaterial(mt);
}

T3Frame::~T3Frame() {
  
}

void T3Frame::setSize(const QVector3D& sz) {
  size = sz;
  updateSize();
}

void T3Frame::updateSize() {
  ((T3FrameMesh*)mesh)->setSize(size);
}

///////////////////////////

T3FrameMesh::T3FrameMesh(Qt3DNode* parent, const QVector3D* sz)
  : Qt3D::QAbstractMesh(parent)
{
  if(sz) {
    size = *sz;
  }
}

T3FrameMesh::~T3FrameMesh() {
  
}

void T3FrameMesh::setSize(const QVector3D& sz) {
  size = sz;
  updateSize();
}

void T3FrameMesh::updateSize() {
  // todo??
}
  
void T3FrameMesh::copy(const Qt3DNode *ref) {
    Qt3D::QAbstractMesh::copy(ref);
    const T3FrameMesh* mesh = static_cast<const T3FrameMesh*>(ref);
    size = mesh->size;
}

Qt3D::QMeshDataPtr createFrameMesh(const QVector3D& size);

class FrameMeshFunctor : public Qt3D::QAbstractMeshFunctor {
public:
  QVector3D size;
  
  FrameMeshFunctor(const T3FrameMesh& mesh)
    : size(mesh.size)
  {
  }

  Qt3D::QMeshDataPtr operator ()() override {
    return createFrameMesh(size);
  }

  bool operator ==(const Qt3D::QAbstractMeshFunctor &other) const {
    const FrameMeshFunctor *otherFunctor = dynamic_cast<const FrameMeshFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return (otherFunctor->size == size);
    return false;
  }
};

Qt3D::QMeshDataPtr createFrameMesh(const QVector3D& size) {
  const int nVerts = 9;

  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray bufferBytes;
  bufferBytes.resize(stride * nVerts);
  float* fptr = reinterpret_cast<float*>(bufferBytes.data());

  const float xs = size.x() * .5f;
  const float ys = size.y() * .5f;
  const float zs = size.z() * .5f;

  for(int z=0; z<2; z++) {
    for(int y=0; y<2; y++) {
      for(int x=0; x<2; x++) {
        *fptr++ = (float)x * xs - xs;
        *fptr++ = (float)y * ys - ys;
        *fptr++ = (float)z * zs - zs;
      }
    }
  }

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
  // const int indicies = 12 * 3;  // 12 lines with end at each one
  // const int indices = 3 * faces;
  // Q_ASSERT(indices < std::numeric_limits<quint16>::max());
  // QByteArray indexBytes;
  // indexBytes.resize(indices * sizeof(quint16));
  // quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

  // // Iterate over z
  // for (int j = 0; j < resolution.height() - 1; ++j) {
  //   const int rowStartIndex = j * resolution.width();
  //   const int nextRowStartIndex = (j + 1) * resolution.width();

  //   // Iterate over x
  //   for (int i = 0; i < resolution.width() - 1; ++i) {
  //     // Split quad into two triangles
  //     *indexPtr++ = rowStartIndex + i;
  //     *indexPtr++ = nextRowStartIndex + i;
  //     *indexPtr++ = rowStartIndex + i + 1;

  //     *indexPtr++ = nextRowStartIndex + i;
  //     *indexPtr++ = nextRowStartIndex + i + 1;
  //     *indexPtr++ = rowStartIndex + i + 1;
  //   }
  // }

  // // Wrap the index bytes in a buffer
  // Qt3D::BufferPtr indexBuffer(new Qt3D::Buffer(QOpenGLBuffer::IndexBuffer));
  // indexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
  // indexBuffer->setData(indexBytes);

  // // Specify index data on the mesh
  // mesh->setIndexAttribute(AttributePtr(new Attribute(indexBuffer, GL_UNSIGNED_SHORT, indices, 0, 0)));

  mesh->computeBoundsFromAttribute(Qt3D::QMeshData::defaultPositionAttributeName());

  return mesh;
}

Qt3D::QAbstractMeshFunctorPtr T3FrameMesh::meshFunctor() const {
  return Qt3D::QAbstractMeshFunctorPtr(new FrameMeshFunctor(*this));
}
