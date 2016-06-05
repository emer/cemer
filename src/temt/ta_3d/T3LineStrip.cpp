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

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QBufferDataGenerator>
#include <Qt3DRender/QAttribute>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QPerVertexColorMaterial>

#include <taMisc>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;
using namespace Qt3DExtras;


T3LineStripMesh::T3LineStripMesh(Qt3DNode* parent)
  : inherited(parent)
{
  line_points.SetGeom(2,3,0);        // 0 = extensible frames
  line_colors.SetGeom(2,4,0);
  line_width = 0.01f;
  line_width_adj = 0.5f;
}

T3LineStripMesh::~T3LineStripMesh() {
  // QNode::cleanup();
}

void T3LineStripMesh::restart() {
  inherited::restart();
  line_points.EnforceFrames(0);
  line_colors.EnforceFrames(0);
  line_indexes.Reset();
}

int T3LineStripMesh::addPoint(const QVector3D& pos) {
  int idx = line_points.Frames();
  line_points.AddFrame();
  line_points.Set(pos.x(), 0, idx);
  line_points.Set(pos.y(), 1, idx);
  line_points.Set(pos.z(), 2, idx);
  return idx;
}

void T3LineStripMesh::moveTo(const QVector3D& pos) {
  int idx = addPoint(pos);
  if(line_indexes.size > 0)
    line_indexes.Add(0x7FFFFFFF);        // stop
  line_indexes.Add(idx);
}

void T3LineStripMesh::lineTo(const QVector3D& pos) {
  int idx = addPoint(pos);
  line_indexes.Add(idx);
}

int T3LineStripMesh::addPoint(const taVector3f& pos) {
  line_points.AddFrame();
  int idx = line_points.Frames()-1;
  line_points.Set(pos.x, 0, idx);
  line_points.Set(pos.y, 1, idx);
  line_points.Set(pos.z, 2, idx);
  return idx;
}

void T3LineStripMesh::moveTo(const taVector3f& pos) {
  int idx = addPoint(pos);
  if(line_indexes.size > 0)
    line_indexes.Add(0x7FFFFFFF);        // stop
  line_indexes.Add(idx);
}

void T3LineStripMesh::lineTo(const taVector3f& pos) {
  int idx = addPoint(pos);
  line_indexes.Add(idx);
}

void T3LineStripMesh::moveToIdx(int idx) {
  if(line_indexes.size > 0)
    line_indexes.Add(0x7FFFFFFF);        // stop
  line_indexes.Add(idx);
}

void T3LineStripMesh::lineToIdx(int idx) {
  line_indexes.Add(idx);
}

int T3LineStripMesh::addColor(const QColor& clr) {
  line_colors.AddFrame();
  int idx = line_colors.Frames()-1;
  line_colors.Set(clr.redF(), 0, idx);
  line_colors.Set(clr.greenF(), 1, idx);
  line_colors.Set(clr.blueF(), 2, idx);
  line_colors.Set(clr.alphaF(), 3, idx);
  return idx;
}

void T3LineStripMesh::setPointColor(int idx, const QColor& clr) {
  if(idx < 0 || idx >= line_colors.Frames()) return;
  line_colors.Set(clr.redF(), 0, idx);
  line_colors.Set(clr.greenF(), 1, idx);
  line_colors.Set(clr.blueF(), 2, idx);
  line_colors.Set(clr.alphaF(), 3, idx);
}

void T3LineStripMesh::setPoint(int idx, const QVector3D& pos) {
  if(idx < 0 || idx >= line_points.Frames()) return;
  line_points.Set(pos.x(), 0, idx);
  line_points.Set(pos.y(), 1, idx);
  line_points.Set(pos.z(), 2, idx);
}

void T3LineStripMesh::setPoint(int idx, const taVector3f& pos) {
  if(idx < 0 || idx >= line_points.Frames()) return;
  line_points.Set(pos.x, 0, idx);
  line_points.Set(pos.y, 1, idx);
  line_points.Set(pos.z, 2, idx);
}

void T3LineStripMesh::updateLines() {
  T3TrianglesMesh::restart();
  float hwd = .5f * line_width * line_width_adj;
  for(int i=1; i<line_indexes.size; i++) {
    int st_idx = line_indexes[i-1];
    if(st_idx == 0x7FFFFFFF) continue;
    int ed_idx = line_indexes[i];
    if(ed_idx == 0x7FFFFFFF) continue;
    taVector3f st_pos(line_points.FastEl2d(0, st_idx), line_points.FastEl2d(1, st_idx),
                      line_points.FastEl2d(2, st_idx));
    taVector3f ed_pos(line_points.FastEl2d(0, ed_idx), line_points.FastEl2d(1, ed_idx),
                      line_points.FastEl2d(2, ed_idx));
    
    int st_vtx = vertexCount();

    taVector3f del = ed_pos - st_pos;
    taVector3f dabs = del; dabs.Abs();
    if(dabs.x >= dabs.y && dabs.x >= dabs.z) { // x-oriented
      taVector3f norm1(0.0f,1.0f,1.0f);  norm1.MagNorm();
      addVertex(taVector3f(st_pos.x, st_pos.y-hwd, st_pos.z-hwd), norm1); // 00 counter
      addVertex(taVector3f(ed_pos.x, ed_pos.y-hwd, ed_pos.z-hwd), norm1); // 01
      addVertex(taVector3f(st_pos.x, st_pos.y+hwd, st_pos.z+hwd), norm1); // 10
      addVertex(taVector3f(ed_pos.x, ed_pos.y+hwd, ed_pos.z+hwd), norm1); // 11
      taVector3f norm2(0.0f,1.0f,-1.0f);  norm2.MagNorm();
      addVertex(taVector3f(st_pos.x, st_pos.y-hwd, st_pos.z+hwd), norm2); // 00 counter
      addVertex(taVector3f(ed_pos.x, ed_pos.y-hwd, ed_pos.z+hwd), norm2); // 01
      addVertex(taVector3f(st_pos.x, st_pos.y+hwd, st_pos.z-hwd), norm2); // 10
      addVertex(taVector3f(ed_pos.x, ed_pos.y+hwd, ed_pos.z-hwd), norm2); // 11
    }
    else if(dabs.y >= dabs.x && dabs.y >= dabs.z) { // y-oriented
      taVector3f norm1(1.0f,0.0f,1.0f);  norm1.MagNorm();
      addVertex(taVector3f(st_pos.x-hwd, st_pos.y, st_pos.z-hwd), norm1); // 00 clock
      addVertex(taVector3f(st_pos.x+hwd, st_pos.y, st_pos.z+hwd), norm1); // 10
      addVertex(taVector3f(ed_pos.x-hwd, ed_pos.y, ed_pos.z-hwd), norm1); // 01
      addVertex(taVector3f(ed_pos.x+hwd, ed_pos.y, ed_pos.z+hwd), norm1); // 11
      taVector3f norm2(1.0f,0.0f,-1.0f);  norm2.MagNorm();
      addVertex(taVector3f(st_pos.x-hwd, st_pos.y, st_pos.z+hwd), norm2); // 00 clock
      addVertex(taVector3f(st_pos.x+hwd, st_pos.y, st_pos.z-hwd), norm2); // 10
      addVertex(taVector3f(ed_pos.x-hwd, ed_pos.y, ed_pos.z+hwd), norm2); // 01
      addVertex(taVector3f(ed_pos.x+hwd, ed_pos.y, ed_pos.z-hwd), norm2); // 11
      
    }
    else { // z-oriented
      taVector3f norm1(1.0f,1.0f,0.0f);  norm1.MagNorm();
      addVertex(taVector3f(st_pos.x-hwd, st_pos.y-hwd, st_pos.z), norm1); // 00
      addVertex(taVector3f(ed_pos.x-hwd, ed_pos.y-hwd, ed_pos.z), norm1); // 01
      addVertex(taVector3f(st_pos.x+hwd, st_pos.y+hwd, st_pos.z), norm1); // 10
      addVertex(taVector3f(ed_pos.x+hwd, ed_pos.y+hwd, ed_pos.z), norm1); // 11
      taVector3f norm2(-1.0f,1.0f,0.0f);  norm2.MagNorm();
      addVertex(taVector3f(st_pos.x+hwd, st_pos.y-hwd, st_pos.z), norm2); // 00
      addVertex(taVector3f(ed_pos.x+hwd, ed_pos.y-hwd, ed_pos.z), norm2); // 01
      addVertex(taVector3f(st_pos.x-hwd, st_pos.y+hwd, st_pos.z), norm2); // 10
      addVertex(taVector3f(ed_pos.x-hwd, ed_pos.y+hwd, ed_pos.z), norm2); // 11
    }

    for(int k=0; k<2; k++) {
      for(int j=0;j<3;j++) {
        addIndex(st_vtx + j);
      }
      addIndex(st_vtx + 2); addIndex(st_vtx + 1); addIndex(st_vtx + 3);
      st_vtx += 4;
    }
  }
}
  

///////////////////////

T3LineStrip::T3LineStrip(Qt3DNode* parent)
  : inherited(parent)
{
  line_width = 1.0f;
  ambient = 1.0f;               // lines are all ambient..
  specular = 0.0f;
  per_vertex_color = false;
  color_type = PHONG;
  color = Qt::black;
  updateColor();
  lines = new T3LineStripMesh();
  addMesh(lines);

  // todo: needs a new material with QCullFace set to turn off culling, so
  // that lines are visible from all angles
}

T3LineStrip::~T3LineStrip() {
  
}

void T3LineStrip::setNodeUpdating(bool updating) {
  if(!updating) {
    updateLines();
  }
  lines->setNodeUpdating(updating);
  inherited::setNodeUpdating(updating);
}

void T3LineStrip::updateLines() {
  lines->updateLines();
}

void T3LineStrip::setPerVertexColor(bool per_vtx) {
  per_vertex_color = per_vtx;
  if(per_vertex_color) {
    color_type = PER_VERTEX;
    updateColor();
  }
  else {
    color_type = PHONG;
    updateColor();
  }
}
