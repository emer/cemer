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

#include <Qt3DRenderer/Buffer>
#include <Qt3DRenderer/QMeshData>
#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/Attribute>


T3LineStripMesh::T3LineStripMesh(Qt3DNode* parent)
  : Qt3D::QAbstractMesh(parent)
{
  points.SetGeom(2,3,0);        // 0 = extensible frames
}

T3LineStripMesh::~T3LineStripMesh() {

}

void T3LineStripMesh::setNodeUpdating(bool updating) {
  bool should_render = false;
  if(!updating && node_updating) should_render = true;
  node_updating = updating;
  blockNotifications(node_updating); // block if updating
  // if(should_render)
    emit nodeUpdatingChanged();
    emit parentChanged();
}

void T3LineStripMesh::restart() {
  points.EnforceFrames(0);
  indexes.SetSize(0);
}

int T3LineStripMesh::addPoint(const QVector3D& pos) {
  points.AddFrame();
  int idx = points.Frames()-1;
  if(idx == 0xFFFF) {
    points.AddFrame();          // skip!!!
    idx = points.Frames()-1;
  }
  points.Set(pos.x(), 0, idx);
  points.Set(pos.y(), 1, idx);
  points.Set(pos.z(), 2, idx);
  return idx;
}

void T3LineStripMesh::moveTo(const QVector3D& pos) {
  int idx = addPoint(pos);
  if(indexes.size > 0)
    indexes.Add(0xFFFF);        // stop
  indexes.Add(idx);
}

void T3LineStripMesh::lineTo(const QVector3D& pos) {
  int idx = addPoint(pos);
  indexes.Add(idx);
}

int T3LineStripMesh::addPoint(const taVector3f& pos) {
  points.AddFrame();
  int idx = points.Frames()-1;
  if(idx == 0xFFFF) {
    points.AddFrame();          // skip!!!
    idx = points.Frames()-1;
  }
  points.Set(pos.x, 0, idx);
  points.Set(pos.y, 1, idx);
  points.Set(pos.z, 2, idx);
  return idx;
}

void T3LineStripMesh::moveTo(const taVector3f& pos) {
  int idx = addPoint(pos);
  if(indexes.size > 0)
    indexes.Add(0xFFFF);        // stop
  indexes.Add(idx);
}

void T3LineStripMesh::lineTo(const taVector3f& pos) {
  int idx = addPoint(pos);
  indexes.Add(idx);
}

void T3LineStripMesh::updateLines() {
  setNodeUpdating(true);
  setNodeUpdating(false);
}
  
void T3LineStripMesh::copy(const Qt3DNode *ref) {
    Qt3D::QAbstractMesh::copy(ref);
    const T3LineStripMesh* mesh = static_cast<const T3LineStripMesh*>(ref);
    points = mesh->points;
    indexes = mesh->indexes;
}

Qt3D::QMeshDataPtr createLineStrip(int n_points, float* points, int n_indexes,
                                   int* indexes);

class LineStripFunctor : public Qt3D::QAbstractMeshFunctor {
public:
  int     n_points;
  float*  points;
  int     n_indexes;
  int*    indexes;
  
  LineStripFunctor(const T3LineStripMesh& mesh)
    : points(mesh.points.el)
    , indexes(mesh.indexes.el)
  {
    // if(mesh.node_updating) {
    //   n_points = 0;
    //   n_indexes = 0;
    // }
    // else {
    n_points = mesh.points.Frames();
    n_indexes = mesh.indexes.size;
    // }
  }

  Qt3D::QMeshDataPtr operator ()() override {
    return createLineStrip(n_points, points, n_indexes, indexes);
  }

  bool operator ==(const Qt3D::QAbstractMeshFunctor &other) const {
    const LineStripFunctor *otherFunctor = dynamic_cast<const LineStripFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
      return ((otherFunctor->n_points == n_points) &&
              (otherFunctor->n_indexes == n_indexes) &&
              (otherFunctor->points == points) &&
              (otherFunctor->indexes == indexes));
    return false;
  }
};

Qt3D::QMeshDataPtr createLineStrip(int n_points, float* points, int n_indexes,
                                   int* indexes) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos; // not: vec2 texCoord, vec3 normal, vec4 tangent
  const quint32 elementSize = 3; // + 2 + 3 + 4;
  const quint32 stride = elementSize * sizeof(float);

  // Wrap the raw bytes in a buffer
  Qt3D::BufferPtr buf(new Qt3D::Buffer(QOpenGLBuffer::VertexBuffer));
  buf->setUsage(QOpenGLBuffer::StaticDraw);
  QByteArray bufferBytes = QByteArray::fromRawData((const char*)points, stride * n_points);
  buf->setData(bufferBytes);

  // Create the mesh data, specify the vertex format and data
  Qt3D::QMeshDataPtr mesh(new Qt3D::QMeshData(Qt3D::QMeshData::LineStrip));
  quint32 offset = 0;
  mesh->addAttribute(Qt3D::QMeshData::defaultPositionAttributeName(),
     Qt3D::AttributePtr(new Qt3D::Attribute(buf, GL_FLOAT_VEC3, n_points, offset, stride)));

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

Qt3D::QAbstractMeshFunctorPtr T3LineStripMesh::meshFunctor() const {
  return Qt3D::QAbstractMeshFunctorPtr(new LineStripFunctor(*this));
}


///////////////////////

T3LineStrip::T3LineStrip(Qt3DNode* parent)
  : inherited(parent)
{
  lines = new T3LineStripMesh();
  addMesh(lines);

  Qt3D::QPhongMaterial* mt = new Qt3D::QPhongMaterial();
  mt->setAmbient(color);
  mt->setDiffuse(color);
  addMaterial(mt);
}

T3LineStrip::~T3LineStrip() {
  
}

void T3LineStrip::setNodeUpdating(bool updating) {
  lines->setNodeUpdating(updating);
  inherited::setNodeUpdating(updating);
}

void T3LineStrip::setColor(const QColor& clr) {
  color = clr;
  Qt3D::QPhongMaterial* mt = dynamic_cast<Qt3D::QPhongMaterial*>(material);
  mt->setAmbient(color);
  mt->setDiffuse(color);
}

void T3LineStrip::updateLines() {
  lines->updateLines();
}

