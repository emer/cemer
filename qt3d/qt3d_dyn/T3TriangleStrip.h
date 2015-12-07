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
#include <Qt3DRender/QGeometryRenderer>

// member includes:
#include <QVector>

// declare all other types mentioned but not required to include:

namespace Qt3DRender {
  class QBuffer;
  class QAttribute;
}


class T3TriangleStripMesh : public Qt3DRender::QGeometryRenderer {
  // mesh for an arbitrary triangle strip with verticies and indexes
  Q_OBJECT
public:
  enum VertexNormal {
    VTX,                        // vertex data
    NRM,                        // normal data
  };
    
  bool  node_updating;          // is the node currently updating its structure, and thus rendering should be blocked, or not?
  virtual void setNodeUpdating(bool updating);

  QVector<float>  vndata; // verticies and normal data -- geom is 3 x 2 x n (3d coords, vertex and normal, then outer is the "frame" dimension of points which can be increased dynamically)
  QVector<float>  colors; // optional per-vertex colors in 1-to-1 correspondence with the vertex data -- these are 4 full floating-point colors RGBA per point -- packed RGBA not supported in shaders it seems..
  QVector<int>    indexes; // triangles defined by sequential indexes into vndata -- use 0xFFFFFFFF to stop one triangle strip and then start another
    
  void  restart();
  // set sizes back to 0

  int  vertexCount() const { return vndata.size() / 6; } // number of vertexes
  int  colorCount() const { return colors.size() / 4; } // number of colors
  int  indexCount() const { return indexes.size(); } // number of indexes
  
  int  addVertex(const QVector3D& pos, const QVector3D& norm);
  // add given vertex, normal at that vertex, return index to that point

  int  addColor(const QColor& clr);
  // add given color, return index to that point -- must keep in sync with adding points!

  void  addTriangle(int v0, int v1, int v2);
  // add triangle composed of the given indexes into the vertex / normal data
  void addIndex(int v0);
  // add a single index 
  void  addBreak();
  // add a break between triangles -- restart a new triangle strip
    
  void  setPointColor(int idx, const QColor& clr);
  
  explicit T3TriangleStripMesh(Qt3DCore::QNode* parent = 0);
  ~T3TriangleStripMesh(); 

public slots:
  virtual void  updateMesh(); // update the rendered mesh
};


class TriangleStripGeometry : public Qt3DRender::QGeometry {
   Q_OBJECT
public:
  explicit TriangleStripGeometry(Qt3DCore::QNode *parent = NULL);

  ~TriangleStripGeometry();

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
  T3TriangleStripMesh* m_mesh;
};


#endif // T3TriangleStrip_h
