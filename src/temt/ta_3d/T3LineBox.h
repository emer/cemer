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

#ifndef T3LineBox_h
#define T3LineBox_h 1

// parent includes:
#include <T3Entity>
#include <Qt3dRenderer/QAbstractMesh>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3LineBoxMesh : public Qt3D::QAbstractMesh {
  // mesh for a 3D line box  -- good for bounding boxes etc
  Q_OBJECT
  INHERITED(Qt3D::QAbstractMesh)
public:
  QVector3D     size;           // size of the frame box
    
  Qt3D::QAbstractMeshFunctorPtr meshFunctor() const override;

  void  setSize(const QVector3D& sz);
  // set size and update mesh
  
  explicit T3LineBoxMesh(Qt3DNode* parent = 0, const QVector3D* sz = 0);
  ~T3LineBoxMesh(); 

public slots:
  virtual void  updateSize(); // update the mesh when size is updated
  
protected:
  void copy(const Qt3DNode* ref) override;
private:
  QT3D_CLONEABLE(T3LineBoxMesh)
};


class TA_API T3LineBox : public T3Entity {
  // a 3D box of lines -- sets mesh to T3LineBoxMesh -- good for bounding boxes etc
  Q_OBJECT
  INHERITED(T3Entity)
public:
  void  setSize(const QVector3D& sz);
  // set new size and update
  
  T3LineBox(Qt3DNode* parent = 0);
  T3LineBox(Qt3DNode* parent, const QVector3D& sz);
  ~T3LineBox();

public slots:
  virtual void  updateSize(); // update to new size

protected:
  void init();
};

#endif // T3LineBox_h
