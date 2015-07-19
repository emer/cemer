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
#include <T3Entity>
#include <Qt3dRenderer/QAbstractMesh>
#include <float_Matrix>
#include <int_Array>
#include <taVector3f>

#include <QColor>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3LineStripMesh : public Qt3D::QAbstractMesh {
  // mesh for an arbitrary line strip with verticies and indexes
  Q_OBJECT
  INHERITED(Qt3D::QAbstractMesh)
public:
  Q_PROPERTY(bool node_updating READ nodeUpdating WRITE setNodeUpdating NOTIFY nodeUpdatingChanged)
  bool  node_updating;          // is the node currently updating its structure, and thus rendering should be blocked, or not?
  virtual void setNodeUpdating(bool updating);
  bool  nodeUpdating()  { return node_updating; }

  float_Matrix  points; // 3d points (verticies) for lines -- geom is 3 x n (outer is the "frame" dimension which can be increased dynamically)
  int_Array     indexes; // lines defined by sequential indexes into points -- use 0xFFFF to stop one line strip and then start another
    
  Qt3D::QAbstractMeshFunctorPtr meshFunctor() const override;

  void  restart();
  // set sizes back to 0
  
  int  addPoint(const QVector3D& pos);
  // add given point, return index to that point
  void  moveTo(const QVector3D& pos);
  // add given point to points, and index of it to indexes -- starts a new line if indexes.size > 0 by adding a stop
  void  lineTo(const QVector3D& pos);
  // add given point to points, and index of it to indexes

  int  addPoint(const taVector3f& pos);
  // add given point, return index to that point
  void  moveTo(const taVector3f& pos);
  // add given point to points, and index of it to indexes -- starts a new line if indexes.size > 0 by adding a stop
  void  lineTo(const taVector3f& pos);
  // add given point to points, and index of it to indexes
  
  explicit T3LineStripMesh(Qt3DNode* parent = 0);
  ~T3LineStripMesh(); 

public slots:
  virtual void  updateLines(); // update the rendered lines
  
signals:
  void  nodeUpdatingChanged();
  
protected:
  void copy(const Qt3DNode* ref) override;
private:
  QT3D_CLONEABLE(T3LineStripMesh)
};


class TA_API T3LineStrip : public T3Entity {
  // strip of lines
  Q_OBJECT
  INHERITED(T3Entity)
public:
  QColor               color;
  T3LineStripMesh*     lines;

  virtual void  setColor(const QColor& clr);

  void setNodeUpdating(bool updating) override;
  
  void  restart()                      { lines->restart(); }

  int   addPoint(const QVector3D& pos) { return lines->addPoint(pos); }
  void  moveTo(const QVector3D& pos)   { lines->moveTo(pos); }
  void  lineTo(const QVector3D& pos)   { lines->lineTo(pos); }   

  int   addPoint(const taVector3f& pos){ return lines->addPoint(pos); }
  void  moveTo(const taVector3f& pos)  { lines->moveTo(pos); }  
  void  lineTo(const taVector3f& pos)  { lines->lineTo(pos); }   

  T3LineStrip(Qt3DNode* parent = 0);
  ~T3LineStrip();

public slots:
  virtual void  updateLines(); // update to new lines
  
};

#endif // T3LineStrip_h
