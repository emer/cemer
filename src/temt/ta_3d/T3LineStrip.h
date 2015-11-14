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
#include <T3ColorEntity>

// member includes:
#include <Qt3DRender/QGeometryRenderer>
#include <float_Matrix>
#include <int_Array>
#include <taVector3f>
#include <T3Misc>

// declare all other types mentioned but not required to include:

class TA_API T3LineStripMesh : public Qt3DRender::QGeometryRenderer {
  // mesh for an arbitrary line strip with verticies and indexes
  Q_OBJECT
  INHERITED(Qt3DRender::QGeometryRenderer)
public:
  bool  node_updating;          // is the node currently updating its structure, and thus rendering should be blocked, or not?
  virtual void setNodeUpdating(bool updating);

  float_Matrix  points; // 3d points (verticies) for lines -- geom is 3 x n (outer is the "frame" dimension which can be increased dynamically)
  float_Matrix  colors; // optional per-vertex colors in 1-to-1 correspondence with the point data -- these are 4 full floating-point colors RGBA per point -- packed RGBA not supported in shaders it seems..
  int_Array     indexes; // lines defined by sequential indexes into points -- uses 0xFFFFFFFF to stop one line strip and then start another
    
  void  restart();
  // set sizes back to 0
  
  int  pointCount() const { return points.Frames(); } // number of points
  int  colorCount() const { return colors.Frames(); } // number of colors
  int  indexCount() const { return indexes.size; } // number of indexes
  
  int  addPoint(const QVector3D& pos);
  // add given point, return index to that point
  void  moveTo(const QVector3D& pos);
  // add given point to points, and index of it to indexes -- starts a new line if indexes.size > 0 by adding a stop
  void  lineTo(const QVector3D& pos);
  // add given point to points, and index of it to indexes

  int   addPoint(const taVector3f& pos);
  // add given point, return index to that point
  void  moveTo(const taVector3f& pos);
  // add given point to points, and index of it to indexes -- starts a new line if indexes.size > 0 by adding a stop
  void  lineTo(const taVector3f& pos);
  // add given point to points, and index of it to indexes

  void  moveToIdx(int idx);
  void  lineToIdx(int idx);
  
  int   addColor(const QColor& clr);
  // add given color -- must keep in sync with adding points!

  void  setPointColor(int idx, const QColor& clr);
  void  setPoint(int idx, const QVector3D& pos);
  void  setPoint(int idx, const taVector3f& pos);
  
  explicit T3LineStripMesh(Qt3DNode* parent = 0);
  ~T3LineStripMesh(); 

public slots:
  virtual void  updateLines(); // update the rendered lines
};


class TA_API T3LineStrip : public T3ColorEntity {
  // strip of lines, either all one color or with per-vertex color
  Q_OBJECT
  INHERITED(T3ColorEntity)
public:
  bool          per_vertex_color;       // if true, then using per-vertex color
  float         line_width;             // width of lines to draw
  T3LineStripMesh*     lines;

  void setNodeUpdating(bool updating) override;
  
  void  restart()                      { lines->restart(); }

  int  pointCount() const { return lines->pointCount(); } // number of vertexes
  int  colorCount() const { return lines->colorCount(); } // number of colors
  int  indexCount() const { return lines->indexCount(); } // number of indexes
  
  int   addPoint(const QVector3D& pos) { return lines->addPoint(pos); }
  void  moveTo(const QVector3D& pos)   { lines->moveTo(pos); }
  void  lineTo(const QVector3D& pos)   { lines->lineTo(pos); }   

  int   addPoint(const taVector3f& pos){ return lines->addPoint(pos); }
  void  moveTo(const taVector3f& pos)  { lines->moveTo(pos); }  
  void  lineTo(const taVector3f& pos)  { lines->lineTo(pos); }   

  void  moveTo(float xp, float yp, float zp)   { lines->moveTo(QVector3D(xp, yp, zp)); }
  void  lineTo(float xp, float yp, float zp)   { lines->lineTo(QVector3D(xp, yp, zp)); }   

  // just add an index to an existing point!
  void  moveToIdx(int idx)              { lines->moveToIdx(idx); }
  void  lineToIdx(int idx)              { lines->lineToIdx(idx); }
  
  int  addColor(const QColor& clr)
  { return lines->addColor(clr); }

  void setPointColor(int idx, const QColor& clr)
  { lines->setPointColor(idx, clr); }

  void setPoint(int idx, const QVector3D& pos)
  { lines->setPoint(idx, pos); }
  void setPoint(int idx, float xp, float yp, float zp)
  { lines->setPoint(idx, QVector3D(xp, yp, yp)); }   
  
  void  setColor(const QColor& clr, float amb = 1.0f,
                 float spec = 0.95f, float shin = 150.0f) override
  { inherited::setColor(clr, amb, spec, shin); }
  // lines are all ambient, so change that default..

  virtual void  setPerVertexColor(bool per_vtx);
  // set whether we're using per-vertex color or not
  
  T3LineStrip(Qt3DNode* parent = 0);
  ~T3LineStrip();

public slots:
  virtual void  updateLines(); // update to new lines
  
};

#endif // T3LineStrip_h
