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

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QBufferFunctor>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QPerVertexColorMaterial>
#include <Qt3DRender/QPhongMaterial>

using namespace Qt3DCore;
using namespace Qt3DRender;

///////////////////////////////////////////////
//      render

// generates one plane -- from qcuboidmesh.cpp

enum PlaneNormal {
  PositiveX,
  NegativeX,
  PositiveY,
  NegativeY,
  PositiveZ,
  NegativeZ
};


//
//  a2,b2    a3,b3
//
//  a0,b0    a1,b1
//

void generatePlaneVertexData
(float a0, float b0, float a1, float b1, float a2, float b2, float a3, float b3, 
 float u0, float v0, float u1, float v1, float u2, float v2, float u3, float v3,
 PlaneNormal normal, float planeDistance, float*& vertices)
{
  const float as[4] = {a0, a1, a2, a3};
  const float bs[4] = {b0, b1, b2, b3};
  const float us[4] = {u0, u1, u2, u3};
  const float vs[4] = {v0, v1, v2, v3};

  float n = 1.0f;

  switch (normal) {
  case NegativeX:
    n = -1.0f; // fall through
  case PositiveX: {
    // Iterate over z
    int idx = 0;
    for (int j = 0; j < 2; ++j) {
      // Iterate over y
      for (int i = 0; i < 2; ++i, ++idx) {
        const float b = bs[idx];
        const float v = vs[idx];
        const float a = as[idx];
        const float u = us[idx];

        // position
        *vertices++ = planeDistance;
        *vertices++ = a;
        *vertices++ = b;

        // texture coordinates
        *vertices++ = u;
        *vertices++ = v;

        // normal
        *vertices++ = n;
        *vertices++ = 0.0f;
        *vertices++ = 0.0f;

        // tangent
        *vertices++ = 0.0f;
        *vertices++ = 0.0f;
        *vertices++ = 1.0f;
        *vertices++ = 1.0f;
      }
    }
    break;
  }

  case NegativeY:
    n = -1.0f;
  case PositiveY: {
    // Iterate over z
    for (int j = 0; j < 2; ++j) {
      // Iterate over x
      // This iterates in the opposite sense to the other directions
      // so that the winding order is correct
      for (int i = 1; i >= 0; --i) {
        int idx = j*2 + i;
        const float b = bs[idx];
        const float v = vs[idx];
        const float a = as[idx];
        const float u = us[idx];

        // position
        *vertices++ = a;
        *vertices++ = planeDistance;
        *vertices++ = b;

        // texture coordinates
        *vertices++ = u;
        *vertices++ = v;

        // normal
        *vertices++ = 0.0f;
        *vertices++ = n;
        *vertices++ = 0.0f;

        // tangent
        *vertices++ = 1.0f;
        *vertices++ = 0.0f;
        *vertices++ = 0.0f;
        *vertices++ = 1.0f;
      }
    }
    break;
  }

  case NegativeZ:
    n = -1.0f;
  case PositiveZ: {
    // Iterate over y
    int idx = 0;
    for (int j = 0; j < 2; ++j) {
      // Iterate over x
      for (int i = 0; i < 2; ++i, ++idx) {
        const float b = bs[idx];
        const float v = vs[idx];
        const float a = as[idx];
        const float u = us[idx];

        // position
        *vertices++ = a;
        *vertices++ = b;
        *vertices++ = planeDistance;

        // texture coordinates
        *vertices++ = u;
        *vertices++ = v;

        // normal
        *vertices++ = 0.0f;
        *vertices++ = 0.0f;
        *vertices++ = n;

        // tangent
        *vertices++ = 1.0f;
        *vertices++ = 0.0f;
        *vertices++ = 0.0f;
        *vertices++ = 1.0f;
      }
    }
    break;
  }
  } // switch (normal)
}

void generatePlaneIndexData(PlaneNormal normal, quint16*& indices, quint16& baseVertex) {
  bool neg = false;
  if(normal == NegativeX || normal == NegativeY || normal == NegativeZ) {
    neg = true;
  }
  
  // Populate indices taking care to get correct CCW winding on all faces
  if(!neg) {
    for (int j = 0; j < 1; ++j) {
      const int rowStartIndex = j * 2 + baseVertex;
      const int nextRowStartIndex = (j + 1) * 2 + baseVertex;

      // Iterate over x
      for (int i = 0; i < 2 - 1; ++i) {
        // Split quad into two triangles
        *indices++ = rowStartIndex + i;
        *indices++ = rowStartIndex + i + 1;
        *indices++ = nextRowStartIndex + i;

        *indices++ = nextRowStartIndex + i;
        *indices++ = rowStartIndex + i + 1;
        *indices++ = nextRowStartIndex + i + 1;
      }
    }
  }
  else {
    for (int j = 0; j < 1; ++j) {
      const int rowStartIndex = j * 2 + baseVertex;
      const int nextRowStartIndex = (j + 1) * 2 + baseVertex;

      // Iterate over x
      for (int i = 0; i < 1; ++i) {
        // Split quad into two triangles
        *indices++ = rowStartIndex + i;
        *indices++ = nextRowStartIndex + i;
        *indices++ = rowStartIndex + i + 1;

        *indices++ = nextRowStartIndex + i;
        *indices++ = nextRowStartIndex + i + 1;
        *indices++ = rowStartIndex + i + 1;
      }
    }
  }
  baseVertex += 4;
}

QByteArray createFrameVertexData(float w, float h, float d, float fw) {
  //  x0,y1          x1,y1
  //    xf0,yf1  xf1,yf1    
  //            
  //    xf0,yf0  xf1,yf0  
  //  x0,y0          x1,y0

  const float z0 = -0.5f * d;
  const float z1 = 0.5f * d;
  
  const float x0 = -0.5f * w;
  const float y0 = -0.5f * h;
  const float x1 = 0.5f * w;
  const float y1 = 0.5f * h;

  const float xf0 = -0.5f * (w - fw);
  const float yf0 = -0.5f * (h - fw);
  const float xf1 = 0.5f * (w - fw);
  const float yf1 = 0.5f * (h - fw);

  const float u0 = 0.0f;
  const float v0 = 0.0f;
  const float u1 = 1.0f;
  const float v1 = 1.0f;

  const float uf0 = w / fw;
  const float vf0 = h / fw;
  const float uf1 = 1.0f - (w / fw);
  const float vf1 = 1.0f - (h / fw);

  // planes: fr*4, bk*4, tp, bt, lf, rt, itp, ibt, ilf, irt
  const int nPlanes = 16;
  const int nVerts = nPlanes * 4;     // 4 verts per plane

  // Allocate a buffer with the interleaved per-vertex data with
  // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3 + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  QByteArray vertexBytes;
  vertexBytes.resize(stride * nVerts);
  float* vertices = reinterpret_cast<float*>(vertexBytes.data());

  // back, left
  generatePlaneVertexData(x0, y0, xf0, yf0, x0,y1, xf0, yf1,
                          u0, v0, uf0, vf0, u0,v1, uf0, vf1,
                          NegativeZ, z0, vertices);
  // back, top
  generatePlaneVertexData(xf0, yf1, xf1, yf1, x0,y1, x1, y1,
                          uf0, vf1, uf1, vf1, u0,v1, u1, v1,
                          NegativeZ, z0, vertices);
  // back, right
  generatePlaneVertexData(xf1, yf0, x1, y0, xf1,yf1, x1, y1,
                          uf1, vf0, u1, v0, uf1,vf1, u1, v1,
                          NegativeZ, z0, vertices);
  // back, bottom
  generatePlaneVertexData(x0, y0, x1, y0, xf0,yf0, xf1, yf0,
                          u0, v0, u1, v0, uf0,vf0, uf1, vf0,
                          NegativeZ, z0, vertices);

  // left edge
  generatePlaneVertexData(y0 , z0, y1, z0, y0,z1, y1, z1,
                          u0, v0, u1, v0, u0,v1, u1, v1,
                          NegativeX, x0, vertices);
  // top edge
  generatePlaneVertexData(x0, z0, x1, z0, x0,z1, x1, z1,
                          u0, v0, u1, v0, u0,v1, u1, v1,
                          PositiveY, y1, vertices);
  // right edge
  generatePlaneVertexData(y0, z0, y1, z0, y0,z1, y1, z1,
                          u0, v0, u1, v0, u0,v1, u1, v1,
                          PositiveX, x1, vertices);
  // bottom edge
  generatePlaneVertexData(x0, z0, x1, z0, x0,z1, x1, z1,
                          u0, v0, u1, v0, u0,v1, u1, v1,
                          NegativeY, y0, vertices);

  // left inside edge
  generatePlaneVertexData(yf0, z0, yf1, z0, yf0,z1, yf1, z1,
                          u0, v0, u1, v0, u0,v1, u1, v1,
                          PositiveX, xf0, vertices);
  // top inside edge
  generatePlaneVertexData(xf0, z0, xf1, z0, xf0,z1, xf1, z1,
                          u0, v0, u1, v0, u0,v1, u1, v1,
                          NegativeY, yf1, vertices);
  // right inside edge
  generatePlaneVertexData(yf0, z0, yf1, z0, yf0,z1, yf1, z1,
                          u0, v0, u1, v0, u0,v1, u1, v1,
                          NegativeX, xf1, vertices);
  // bottom inside edge
  generatePlaneVertexData(xf0, z0, xf1, z0, xf0,z1, xf1, z1,
                          u0, v0, u1, v0, u0,v1, u1, v1,
                          PositiveY, yf0, vertices);

  // front, left
  generatePlaneVertexData(x0, y0, xf0, yf0, x0,y1, xf0, yf1,
                          u0, v0, uf0, vf0, u0,v1, uf0, vf1,
                          PositiveZ, z1, vertices);
  // front, top
  generatePlaneVertexData(xf0, yf1, xf1, yf1, x0,y1, x1, y1,
                          uf0, vf1, uf1, vf1, u0,v1, u1, v1,
                          PositiveZ, z1, vertices);
  // front, right
  generatePlaneVertexData(xf1, yf0, x1, y0, xf1,yf1, x1, y1,
                          uf1, vf0, u1, v0, uf1,vf1, u1, v1,
                          PositiveZ, z1, vertices);
  // front, bottom
  generatePlaneVertexData(x0, y0, x1, y0, xf0,yf0, xf1, yf0,
                          u0, v0, u1, v0, uf0,vf0, uf1, vf0,
                          PositiveZ, z1, vertices);

  return vertexBytes;
}

class FrameVertexBufferFunctor : public QBufferFunctor {
public:
  float		width;          // overall x dimension
  float		height;         // y dimension
  float		depth;          // z dimension
  float		frame_width;    // width of the frame -- goes in this amount from overall
  
  FrameVertexBufferFunctor(const T3FrameMesh& mesh)
    : width(mesh.m_width)
    , height(mesh.m_height)
    , depth(mesh.m_depth)
    , frame_width(mesh.m_frame_width)
  {
  }

  QByteArray operator ()() override {
    return createFrameVertexData(width, height, depth, frame_width);
  }

  bool operator ==(const QBufferFunctor &other) const override {
    const FrameVertexBufferFunctor *otherFunctor =
      dynamic_cast<const FrameVertexBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)      return ((otherFunctor->width == width) &&
              (otherFunctor->height == height) &&
              (otherFunctor->depth == depth) &&
              (otherFunctor->frame_width == frame_width));
    return false;
  }

  QT3D_FUNCTOR(FrameVertexBufferFunctor)
};


QByteArray createFrameIndexData() {
  // planes: fr*4, bk*4, tp, bt, lf, rt, itp, ibt, ilf, irt
  const int nPlanes = 16;

  // Allocate a buffer for the index data
  const int idxperpln = 2 * 3;
  const int indexCount = nPlanes * idxperpln;
  Q_ASSERT(indexCount < (std::numeric_limits<quint16>::max)());
  QByteArray indexBytes;
  indexBytes.resize(indexCount * sizeof(quint16));
  quint16* indices = reinterpret_cast<quint16*>(indexBytes.data());
  // Populate vertex and index buffers
  quint16 baseVertex = 0;

  // back, left
  generatePlaneIndexData(NegativeZ, indices, baseVertex);
  // back, top
  generatePlaneIndexData(NegativeZ, indices, baseVertex);
  // back, right
  generatePlaneIndexData(NegativeZ, indices, baseVertex);
  // back, bottom
  generatePlaneIndexData(NegativeZ, indices, baseVertex);

  // left edge
  generatePlaneIndexData(NegativeX, indices, baseVertex);
  // top edge
  generatePlaneIndexData(PositiveY, indices, baseVertex);
  // right edge
  generatePlaneIndexData(PositiveX, indices, baseVertex);
  // bottom edge
  generatePlaneIndexData(NegativeY, indices, baseVertex);

  // left inside edge
  generatePlaneIndexData(PositiveX,  indices, baseVertex);
  // top inside edge
  generatePlaneIndexData(NegativeY,  indices, baseVertex);
  // right inside edge
  generatePlaneIndexData(NegativeX,  indices, baseVertex);
  // bottom inside edge
  generatePlaneIndexData(PositiveY,  indices, baseVertex);

  // front, left
  generatePlaneIndexData(PositiveZ, indices, baseVertex);
  // front, top
  generatePlaneIndexData(PositiveZ, indices, baseVertex);
  // front, right
  generatePlaneIndexData(PositiveZ, indices, baseVertex);
  // front, bottom
  generatePlaneIndexData(PositiveZ, indices, baseVertex);

  return indexBytes;
}

class FrameIndexBufferFunctor : public QBufferFunctor {
public:
  
  FrameIndexBufferFunctor(const T3FrameMesh& mesh) {
  }

  QByteArray operator ()() override {
    return createFrameIndexData();
  }

  bool operator ==(const QBufferFunctor &other) const override {
    const FrameIndexBufferFunctor *otherFunctor =
      dynamic_cast<const FrameIndexBufferFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return true;
    return false;
  }

  QT3D_FUNCTOR(FrameIndexBufferFunctor)
};



////////////////////////////////////////////////////
//      Geometry

FrameGeometry::FrameGeometry(QNode *parent)
  : QGeometry(parent)
  , m_mesh((T3FrameMesh*)parent)
  , m_positionAttribute(new QAttribute(this))
  , m_texCoordAttribute(new QAttribute(this))
  , m_normalAttribute(new QAttribute(this))
  , m_tangentAttribute(new QAttribute(this))
  , m_indexAttribute(new QAttribute(this))
  , m_vertexBuffer(new QBuffer(QBuffer::VertexBuffer, this))
  , m_indexBuffer(new QBuffer(QBuffer::IndexBuffer, this))
{
  const quint32 elementSize = 3 + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);
  const int nVerts = 64;
  const int indexCount = 96;

  m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
  m_positionAttribute->setDataType(QAttribute::Float);
  m_positionAttribute->setDataSize(3);
  m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
  m_positionAttribute->setBuffer(m_vertexBuffer);
  m_positionAttribute->setByteStride(stride);
  m_positionAttribute->setCount(nVerts);

  m_texCoordAttribute->setName
    (QAttribute::defaultTextureCoordinateAttributeName());
  m_texCoordAttribute->setDataType(QAttribute::Float);
  m_texCoordAttribute->setDataSize(3);
  m_texCoordAttribute->setAttributeType(QAttribute::VertexAttribute);
  m_texCoordAttribute->setBuffer(m_vertexBuffer);
  m_texCoordAttribute->setByteStride(stride);
  m_texCoordAttribute->setByteOffset(3 * sizeof(float));
  m_texCoordAttribute->setCount(nVerts);

  m_normalAttribute->setName(QAttribute::defaultNormalAttributeName());
  m_normalAttribute->setDataType(QAttribute::Float);
  m_normalAttribute->setDataSize(3);
  m_normalAttribute->setAttributeType(QAttribute::VertexAttribute);
  m_normalAttribute->setBuffer(m_vertexBuffer);
  m_normalAttribute->setByteStride(stride);
  m_normalAttribute->setByteOffset(5 * sizeof(float));
  m_normalAttribute->setCount(nVerts);

  m_tangentAttribute->setName(QAttribute::defaultTangentAttributeName());
  m_tangentAttribute->setDataType(QAttribute::Float);
  m_tangentAttribute->setDataSize(3);
  m_tangentAttribute->setAttributeType(QAttribute::VertexAttribute);
  m_tangentAttribute->setBuffer(m_vertexBuffer);
  m_tangentAttribute->setByteStride(stride);
  m_tangentAttribute->setByteOffset(8 * sizeof(float));
  m_tangentAttribute->setCount(nVerts);

  m_indexAttribute->setAttributeType(QAttribute::IndexAttribute);
  m_indexAttribute->setDataType(QAttribute::UnsignedShort);
  m_indexAttribute->setBuffer(m_indexBuffer);
  m_indexAttribute->setCount(indexCount);

  m_vertexBuffer->setBufferFunctor
    (QBufferFunctorPtr(new FrameVertexBufferFunctor(*m_mesh)));
  m_indexBuffer->setBufferFunctor
    (QBufferFunctorPtr(new FrameIndexBufferFunctor(*m_mesh)));

  addAttribute(m_positionAttribute);
  addAttribute(m_texCoordAttribute);
  addAttribute(m_normalAttribute);
  addAttribute(m_tangentAttribute);
  addAttribute(m_indexAttribute);
}

FrameGeometry::~FrameGeometry() {
  QGeometry::cleanup();
}

void FrameGeometry::updateGeometry() {
  m_vertexBuffer->setBufferFunctor
    (QBufferFunctorPtr(new FrameVertexBufferFunctor(*m_mesh)));
}


////////////////////////////////////////
//              Mesh

T3FrameMesh::T3FrameMesh(QNode* parent)
  : QGeometryRenderer(parent)
{
  m_width = 1.0f;
  m_height = 1.0f;
  m_depth = 0.05f;
  m_frame_width = 0.1f;

  setPrimitiveType(Triangles);
  
  FrameGeometry* geometry = new FrameGeometry(this);
  QGeometryRenderer::setGeometry(geometry);
}

T3FrameMesh::~T3FrameMesh() {
  QNode::cleanup();
}

void T3FrameMesh::updateGeometry() {
  static_cast<FrameGeometry *>(geometry())->updateGeometry();
}


