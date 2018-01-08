// Copyright 2016-2017, Regents of the University of Colorado,
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

#ifndef T3Triangles_h
#define T3Triangles_h 1

// parent includes:
#include <T3ColorEntity>

// member includes:
#include <Qt3DRender/QGeometryRenderer>
#include <float_Matrix>
#include <int_Array>
#include <taVector3f>

// declare all other types mentioned but not required to include:

namespace Qt3DRender {
  class QBuffer;
  class QAttribute;
}


class TA_API T3TrianglesMesh : public Qt3DRender::QGeometryRenderer {
  // mesh for an arbitrary list of triangles with verticies and indexes
  Q_OBJECT
  INHERITED(Qt3DRender::QGeometryRenderer)
public:
  enum VertexNormal {
    VTX,                        // vertex data
    NRM,                        // normal data
  };
    
  Q_PROPERTY(bool node_updating READ nodeUpdating WRITE setNodeUpdating NOTIFY nodeUpdatingChanged)
  // use this property to control notifcation of updates to a node -- calls blockNotifications(true) if setNodeUpdating(true) is called, and when the corresponding false call is made, then it also calls nodeUpdatingChanged signal, which then triggers an actual node update -- blockNotifications(false) should do this but it doesn't!

  bool  node_updating;          // is the node currently updating its structure, and thus rendering should be blocked, or not?
  virtual void setNodeUpdating(bool updating);
  bool  nodeUpdating()  { return node_updating; }

  float_Matrix  vndata; // verticies and normal data -- geom is 3 x 2 x n (3d coords, vertex and normal, then outer is the "frame" dimension of points which can be increased dynamically)
  float_Matrix  colors; // optional per-vertex colors in 1-to-1 correspondence with the vertex data -- these are 4 full floating-point colors RGBA per point -- packed RGBA not supported in shaders it seems..
  int_Array     indexes; // triangles defined by sequential indexes into vndata
    
  virtual void  restart();
  // set sizes back to 0

  int  vertexCount() const { return vndata.Frames(); } // number of vertexes
  int  colorCount() const { return colors.Frames(); } // number of colors
  int  indexCount() const { return indexes.size; } // number of indexes
  
  virtual int  addVertex(const QVector3D& pos, const QVector3D& norm);
  // add given vertex, normal at that vertex, return index to that point
  virtual int  addVertex(const taVector3f& pos, const taVector3f& norm);
  // add given vertex, normal at that vertex, return index to that point

  virtual int  addColor(const QColor& clr);
  // add given color, return index to that point -- must keep in sync with adding points!

  virtual void  addTriangle(int v0, int v1, int v2);
  // add triangle composed of the given indexes into the vertex / normal data
  virtual void  addIndex(int v0);
  // add a single index 
    
  virtual void  setPointColor(int idx, const QColor& clr);
  
  explicit T3TrianglesMesh(Qt3DNode* parent = 0);
  ~T3TrianglesMesh(); 

public slots:
  virtual void  updateMesh(); // update the rendered mesh
  
signals:
  void  nodeUpdatingChanged();
};


class TrianglesGeometry : public Qt3DRender::QGeometry {
   Q_OBJECT
public:
  explicit TrianglesGeometry(Qt3DCore::QNode *parent = NULL);

  ~TrianglesGeometry();

  void updateIndices();
  void updateVertices();  
  void updateColors();
  void updateAll();
  
private:
  Qt3DRender::QAttribute *m_positionAttribute;
  Qt3DRender::QAttribute *m_normalAttribute;
  Qt3DRender::QAttribute *m_indexAttribute;
  Qt3DRender::QAttribute *m_colorAttribute;
  Qt3DRender::QBuffer *m_vertexBuffer;
  Qt3DRender::QBuffer *m_indexBuffer;
  Qt3DRender::QBuffer *m_colorBuffer;
  T3TrianglesMesh* m_mesh;
};


class TA_API T3Triangles : public T3ColorEntity {
  // triangles, either all one color or with per-vertex color
  Q_OBJECT
  INHERITED(T3ColorEntity)
public:
  T3TrianglesMesh*     tris;

  void setNodeUpdating(bool updating) override;
  
  void  restart()    { tris->restart(); }

  int  vertexCount() const { return tris->vertexCount(); } // number of vertexes
  int  colorCount() const { return tris->colorCount(); } // number of colors
  int  indexCount() const { return tris->indexCount(); } // number of indexes
  
  int  addVertex(const QVector3D& pos, const QVector3D& norm)
  { return tris->addVertex(pos, norm); }
  // add given vertex, normal at that vertex, return index to that point
  int  addVertex(const taVector3f& pos, const taVector3f& norm)
  { return tris->addVertex(pos, norm); }
  // add given vertex, normal at that vertex, return index to that point

  int  addColor(const QColor& clr)
  { return tris->addColor(clr); }

  void addTriangle(int v0, int v1, int v2) 
  { tris->addTriangle(v0, v1, v2); }
  // add triangle composed of the given indexes into the vertex / normal data
  void addIndex(int v0) 
  { tris->addIndex(v0); }

  void setPointColor(int idx, const QColor& clr)
  { tris->setPointColor(idx, clr); }

  T3Triangles(Qt3DNode* parent = 0);
  ~T3Triangles();

public slots:
  virtual void  updateMesh(); // update to new mesh
  
};

#endif // T3Triangles_h
