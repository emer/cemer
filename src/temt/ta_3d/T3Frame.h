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

#ifndef T3Frame_h
#define T3Frame_h 1

// parent includes:
#include <T3ColorEntity>
#include <Qt3DRender/QGeometryRenderer>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3FrameMesh : public Qt3DRender::QGeometryRenderer {
  // a picture-frame shape -- rectangular with a width and thickness, oriented in XY plane
  Q_OBJECT
  INHERITED(Qt3DRender::QGeometryRenderer)

  Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(float height READ height WRITE setHeight NOTIFY heightChanged)
  Q_PROPERTY(float depth READ depth WRITE setDepth NOTIFY depthChanged)
  Q_PROPERTY(float frame_width READ frameWidth WRITE setFrameWidth NOTIFY frameWidthChanged)

public:
  float		m_width;          // overall x dimension
  float		m_height;         // y dimension
  float		m_depth;          // z dimension
  float		m_frame_width;    // width of the frame -- goes in this amount from overall width, height

  void setWidth(float wd)
  { if(m_width != wd) { m_width = wd; updateGeometry(); } }
  float width() const { return m_width; }

  void setHeight(float wd)
  { if(m_height != wd) { m_height = wd; updateGeometry(); } }
  float height() const { return m_height; }

  void setDepth(float wd)
  { if(m_depth != wd) { m_depth = wd; updateGeometry(); } }
  float depth() const { return m_depth; }

  void setFrameWidth(float wd)
  { if(m_frame_width != wd) { m_frame_width = wd; updateGeometry(); } }
  float frameWidth() const { return m_frame_width; }

  virtual void updateGeometry();
  
  explicit T3FrameMesh(Qt3DNode *parent = 0);
  ~T3FrameMesh();

signals:
  void  widthChanged();
  void  heightChanged();
  void  depthChanged();
  void  frameWidthChanged();
};


class TA_API T3Frame : public T3ColorEntity {
  // a picture-frame shape -- rectangular with a width and thickness, oriented in XY plane
  Q_OBJECT
  INHERITED(T3ColorEntity)
public:
  float		width;          // overall x dimension
  float		height;         // y dimension
  float		depth;          // z dimension
  float		frame_width;    // width of the frame -- goes in this amount from overall width, height
  T3FrameMesh*  frame;          // frame mesh

  virtual void  setGeom(float wd, float ht, float dp, float frwd);
  // set new geometry

  T3Frame(Qt3DNode* parent = 0);
  ~T3Frame();
            
public slots:
  virtual void  updateGeom(); // update to new geometry

};

#endif // T3Frame_h
