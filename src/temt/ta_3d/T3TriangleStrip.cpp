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

#include <Qt3DRenderer/Buffer>
#include <Qt3DRenderer/QMeshData>
#include <Qt3DRenderer/QPerVertexColorMaterial>
#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/Attribute>

#include <T3Misc>
#include <taMisc>

T3TriangleStripMesh::T3TriangleStripMesh(Qt3DNode* parent)
  : Qt3D::QAbstractMesh(parent)
{
  vndata.SetGeom(3,3,2,0);        // 0 = extensible frames
}

T3TriangleStripMesh::~T3TriangleStripMesh() {
  Qt3D::QNode::cleanup();
}

void T3TriangleStripMesh::setNodeUpdating(bool updating) {
  bool should_render = false;
  if(!updating && node_updating) should_render = true;
  node_updating = updating;
  blockNotifications(node_updating); // block if updating
  // nothing seems to work here...
  if(should_render) {
    if(colors.size > 0 && colors.size != vndata.Frames()) {
      taMisc::Warning("T3TriangleStripMesh: colors != vndata vertices",
                      String(colors.size), " vs. ", String(vndata.Frames()));
    }
    emit nodeUpdatingChanged();
    //    emit parentChanged();
  }
}

void T3TriangleStripMesh::restart() {
  vndata.EnforceFrames(0);
  colors.SetSize(0);
  indexes.SetSize(0);
}

int T3TriangleStripMesh::addVertex(const QVector3D& pos, const QVector3D& norm) {
  vndata.AddFrame();
  int idx = vndata.Frames()-1;
  if(idx == 0xFFFF) {
    vndata.AddFrame();          // skip!!!
    idx = vndata.Frames()-1;
  }
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
  if(idx == 0xFFFF) {
    vndata.AddFrame();          // skip!!!
    idx = vndata.Frames()-1;
  }
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
  indexes.Add(0xFFFF);        // stop
}

int T3TriangleStripMesh::addColor(uint32_t clr) {
  colors.Add(clr);
  return colors.size -1;
}

int T3TriangleStripMesh::addColor(const QColor& clr) {
  colors.Add(T3Misc::makePackedRGBA(clr));
  return colors.size -1;
}

void T3TriangleStripMesh::updateMesh() {
  setNodeUpdating(true);
  setNodeUpdating(false);
}
  
void T3TriangleStripMesh::copy(const Qt3DNode *ref) {
    Qt3D::QAbstractMesh::copy(ref);
    const T3TriangleStripMesh* mesh = static_cast<const T3TriangleStripMesh*>(ref);
    vndata = mesh->vndata;
    indexes = mesh->indexes;
    colors = mesh->colors;
}

Qt3D::QMeshDataPtr createTriangleStrip(int n_vndata, float* vndata, int n_indexes,
                                       int* indexes, int n_colors, int* colors);

class TriangleStripFunctor : public Qt3D::QAbstractMeshFunctor {
public:
  int     n_vndata;
  float*  vndata;
  int     n_indexes;
  int*    indexes;
  int     n_colors;
  int*    colors;
  
  TriangleStripFunctor(const T3TriangleStripMesh& mesh)
    : vndata(mesh.vndata.el)
    , indexes(mesh.indexes.el)
    , colors(mesh.colors.el)
  {
    // if(mesh.node_updating) {
    //   n_vndata = 0;
    //   n_indexes = 0;
    // }
    // else {
    n_vndata = mesh.vndata.Frames();
    n_indexes = mesh.indexes.size;
    n_colors = mesh.colors.size;
    // }
  }

  Qt3D::QMeshDataPtr operator ()() override {
    return createTriangleStrip(n_vndata, vndata, n_indexes, indexes, n_colors, colors);
  }

  bool operator ==(const Qt3D::QAbstractMeshFunctor &other) const {
    const TriangleStripFunctor *otherFunctor = dynamic_cast<const TriangleStripFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_vndata == n_vndata) &&
              (otherFunctor->n_indexes == n_indexes) &&
              (otherFunctor->vndata == vndata) &&
              (otherFunctor->indexes == indexes) &&
              (otherFunctor->n_colors == n_colors) &&
              (otherFunctor->colors == colors));
    return false;
  }
};

Qt3D::QMeshDataPtr createTriangleStrip(int n_vndata, float* vndata, int n_indexes,
                                       int* indexes, int n_colors, int* colors) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos + vec3 normal; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3 + 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);

  if(n_colors > 0 && (n_vndata != n_colors)) { // not in a good state
    n_indexes = 0;
    n_vndata = 0;
    n_colors = 0;
  }
  
  // Wrap the raw bytes in a buffer
  Qt3D::BufferPtr buf(new Qt3D::Buffer(QOpenGLBuffer::VertexBuffer));
  buf->setUsage(QOpenGLBuffer::StaticDraw);
  QByteArray bufferBytes = QByteArray::fromRawData((const char*)vndata, stride * n_vndata);
  buf->setData(bufferBytes);

  // Create the mesh data, specify the vertex format and data
  Qt3D::QMeshDataPtr mesh(new Qt3D::QMeshData(Qt3D::QMeshData::TriangleStrip));
  quint32 offset = 0;
  mesh->addAttribute(Qt3D::QMeshData::defaultPositionAttributeName(),
     Qt3D::AttributePtr(new Qt3D::Attribute(buf, GL_FLOAT_VEC3, n_vndata, offset, stride)));
  offset = 3 * sizeof(float);
  mesh->addAttribute(Qt3D::QMeshData::defaultNormalAttributeName(),
     Qt3D::AttributePtr(new Qt3D::Attribute(buf, GL_FLOAT_VEC3, n_vndata, offset, stride)));

  if(n_colors > 0) {
    Qt3D::BufferPtr colorBuffer(new Qt3D::Buffer(QOpenGLBuffer::VertexBuffer));
    colorBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    QByteArray colorBytes = QByteArray::fromRawData((const char*)colors,
                                                    n_colors *  sizeof(int));
    colorBuffer->setData(colorBytes);
    mesh->addAttribute(Qt3D::QMeshData::defaultColorAttributeName(),
     Qt3D::AttributePtr(new Qt3D::Attribute(colorBuffer, GL_UNSIGNED_INT,
                                            n_colors, 0, 0)));
  }

  // Wrap the index bytes in a buffer
  Qt3D::BufferPtr indexBuffer(new Qt3D::Buffer(QOpenGLBuffer::IndexBuffer));
  indexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
  QByteArray indexBytes = QByteArray::fromRawData((const char*)indexes, n_indexes *  sizeof(int));
  indexBuffer->setData(indexBytes);

  // Specify index data on the mesh
  mesh->setIndexAttribute(Qt3D::AttributePtr(new Qt3D::Attribute
                                             (indexBuffer, GL_UNSIGNED_INT,
                                              n_indexes, 0, 0)));

  mesh->computeBoundsFromAttribute(Qt3D::QMeshData::defaultPositionAttributeName());

  return mesh;
}

Qt3D::QAbstractMeshFunctorPtr T3TriangleStripMesh::meshFunctor() const {
  return Qt3D::QAbstractMeshFunctorPtr(new TriangleStripFunctor(*this));
}


///////////////////////

T3TriangleStrip::T3TriangleStrip(Qt3DNode* parent)
  : inherited(parent)
{
  // temp -- not working!
  // addMaterial(new Qt3D::QPerVertexColorMaterial);

  Qt3D::QPhongMaterial* phong = new Qt3D::QPhongMaterial;
  addMaterial(phong);

  QColor color = Qt::green;
  phong->setDiffuse(color);
  phong->setAmbient(color.darker((int)(100.0f / 0.2f)));
  phong->setSpecular(QColor(250, 250, 250, 255));
  phong->setShininess(2);
  
  tris = new T3TriangleStripMesh();
  addMesh(tris);
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


