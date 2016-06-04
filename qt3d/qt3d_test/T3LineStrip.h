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

#ifndef T3LineStrip_h
#define T3LineStrip_h 1

// parent includes:
#include <Qt3DRender/QGeometryRenderer>

// member includes:
#include <QVector>

// declare all other types mentioned but not required to include:

namespace Qt3DRender {
  class QBuffer;
  class QAttribute;
}

class T3LineStripMesh : public Qt3DRender::QGeometryRenderer {
  // mesh for an arbitrary line strip with verticies and indexes
  Q_OBJECT
public:
  bool  node_updating;          // is the node currently updating its structure, and thus rendering should be blocked, or not?
  virtual void setNodeUpdating(bool updating);

  QVector<float>  points; // 3d points (verticies) for lines -- geom is 3 x n 
  QVector<float>  colors; // optional per-vertex colors in 1-to-1 correspondence with the point data -- these are 4 full floating-point colors RGBA per point -- packed RGBA not supported in shaders it seems..
  QVector<int>    indexes; // lines defined by sequential indexes into points -- uses 0xFFFFFFFF to stop one line strip and then start another
    
  void  restart();
  // set sizes back to 0
  
  int  pointCount() const { return points.size() / 3; } // number of points
  int  colorCount() const { return colors.size() / 4; } // number of colors
  int  indexCount() const { return indexes.size(); } // number of indexes
  
  int  addPoint(const QVector3D& pos);
  // add given point, return index to that point
  void  moveTo(const QVector3D& pos);
  // add given point to points, and index of it to indexes -- starts a new line if indexes.size > 0 by adding a stop
  void  lineTo(const QVector3D& pos);
  // add given point to points, and index of it to indexes

  void  moveToIdx(int idx);
  void  lineToIdx(int idx);
  
  int   addColor(const QColor& clr);
  // add given color -- must keep in sync with adding points!

  void  setPointColor(int idx, const QColor& clr);
  void  setPoint(int idx, const QVector3D& pos);
  
  explicit T3LineStripMesh(Qt3DCore::QNode* parent = 0);
  ~T3LineStripMesh(); 

public slots:
  virtual void  updateLines(); // update the rendered lines
};


class LineStripGeometry : public Qt3DRender::QGeometry {
  Q_OBJECT
public:
  explicit LineStripGeometry(Qt3DCore::QNode *parent = NULL);

  ~LineStripGeometry();
  
  void updateIndices();
  void updateVertices();
  void updateColors();
  void updateAll();
  
private:
  Qt3DRender::QAttribute *m_positionAttribute;
  Qt3DRender::QAttribute *m_indexAttribute;
  Qt3DRender::QAttribute *m_colorAttribute;
  Qt3DRender::QBuffer *m_vertexBuffer;
  Qt3DRender::QBuffer *m_indexBuffer;
  Qt3DRender::QBuffer *m_colorBuffer;
  T3LineStripMesh* m_mesh;
};


#endif // T3LineStrip_h
