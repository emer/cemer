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

#ifndef T3TriangleStrip_h
#define T3TriangleStrip_h 1

// parent includes:
#include <T3Entity>

// member includes:
#include <Qt3dRenderer/QAbstractMesh>
#include <float_Matrix>
#include <int_Array>
#include <taVector3f>

// declare all other types mentioned but not required to include:

class TA_API T3TriangleStripMesh : public Qt3D::QAbstractMesh {
  // mesh for an arbitrary triangle strip with verticies and indexes
  Q_OBJECT
  INHERITED(Qt3D::QAbstractMesh)
public:
  enum VertexNormal {
    VTX,                        // vertex data
    NRM,                        // normal data
  };
    
  Q_PROPERTY(bool node_updating READ nodeUpdating WRITE setNodeUpdating NOTIFY nodeUpdatingChanged)
  bool  node_updating;          // is the node currently updating its structure, and thus rendering should be blocked, or not?
  virtual void setNodeUpdating(bool updating);
  bool  nodeUpdating()  { return node_updating; }

  float_Matrix  vndata; // verticies and normal data -- geom is 3 x 2 x n (3d coords, vertex and normal, then outer is the "frame" dimension of points which can be increased dynamically)
  int_Array     colors; // optional per-vertex colors in 1-to-1 correspondence with the vertex data -- these are packed RGBA colors, each component taking one byte
  int_Array     indexes; // triangles defined by sequential indexes into vndata -- use 0xFFFF to stop one triangle strip and then start another
    
  Qt3D::QAbstractMeshFunctorPtr meshFunctor() const override;

  void  restart();
  // set sizes back to 0

  int  vertexCount() { return vndata.Frames(); } // number of vertexes
  int  colorCount()  { return colors.size; } // number of colors
  int  indexCount()  { return indexes.size; } // number of indexes
  
  int  addVertex(const QVector3D& pos, const QVector3D& norm);
  // add given vertex, normal at that vertex, return index to that point
  int  addVertex(const taVector3f& pos, const taVector3f& norm);
  // add given vertex, normal at that vertex, return index to that point

  int  addColor(uint32_t color);
  // add given color, return index to that point -- must keep in sync with adding points!
  int  addColor(const QColor& clr);
  // add given color, return index to that point -- must keep in sync with adding points!

  void  addTriangle(int v0, int v1, int v2);
  // add triangle composed of the given indexes into the vertex / normal data
  void addIndex(int v0);
  // add a single index 
  void  addBreak();
  // add a break between triangles -- restart a new triangle strip
    
  
  explicit T3TriangleStripMesh(Qt3DNode* parent = 0);
  ~T3TriangleStripMesh(); 

public slots:
  virtual void  updateMesh(); // update the rendered mesh
  
signals:
  void  nodeUpdatingChanged();
  
protected:
  void copy(const Qt3DNode* ref) override;
private:
  QT3D_CLONEABLE(T3TriangleStripMesh)
};


class TA_API T3TriangleStrip : public T3Entity {
  // strip of triangles, either all one color or with per-vertex color
  Q_OBJECT
  INHERITED(T3Entity)
public:
  T3TriangleStripMesh*     tris;

  void setNodeUpdating(bool updating) override;
  
  void  restart()                      { tris->restart(); }

  int  vertexCount() { return tris->vertexCount(); } // number of vertexes
  int  colorCount()  { return tris->colorCount(); } // number of colors
  int  indexCount()  { return tris->indexCount(); } // number of indexes
  
  int  addVertex(const QVector3D& pos, const QVector3D& norm)
  { return tris->addVertex(pos, norm); }
  // add given vertex, normal at that vertex, return index to that point
  int  addVertex(const taVector3f& pos, const taVector3f& norm)
  { return tris->addVertex(pos, norm); }
  // add given vertex, normal at that vertex, return index to that point

  int  addColor(uint32_t clr)
  { return tris->addColor(clr); }
  // add given color, return index to that point
  int  addColor(const QColor& clr)
  { return tris->addColor(clr); }

  void addTriangle(int v0, int v1, int v2) 
  { tris->addTriangle(v0, v1, v2); }
  // add triangle composed of the given indexes into the vertex / normal data
  void addIndex(int v0) 
  { tris->addIndex(v0); }
  void  addBreak()
  { tris->addBreak(); }
  // add a break between triangles -- restart a new triangle strip

  T3TriangleStrip(Qt3DNode* parent = 0);
  ~T3TriangleStrip();

public slots:
  virtual void  updateMesh(); // update to new mesh
  
};

#endif // T3TriangleStrip_h
