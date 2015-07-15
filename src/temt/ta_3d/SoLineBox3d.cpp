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

#include "SoLineBox3d.h"

SO_NODE_SOURCE(SoLineBox3d);

void SoLineBox3d::initClass()
{
  SO_NODE_INIT_CLASS(SoLineBox3d, SoIndexedLineSet, "SoIndexedLineSet");
}

SoLineBox3d::SoLineBox3d(float wd, float ht, float dp, bool ctr) {
  SO_NODE_CONSTRUCTOR(SoLineBox3d);

  center = ctr;
  width = wd;
  height = ht;
  depth = dp;

  vertexProperty.setValue(new SoVertexProperty); // note: vp refs/unrefs automatically
  render();
}

void SoLineBox3d::render() {
  SoMFVec3f& vtx = ((SoVertexProperty*)vertexProperty.getValue())->vertex;
  vtx.setNum(8);
  SbVec3f* v_dat = vtx.startEditing();
  int idx = 0;
  if(center) {
    float hx = 0.5f * width; float hy = 0.5f * height; float hz = 0.5f * depth;
    v_dat[idx++].setValue(-hx,-hy,-hz);	// around the back
    v_dat[idx++].setValue(hx,-hy,-hz);
    v_dat[idx++].setValue(hx,hy,-hz);
    v_dat[idx++].setValue(-hx,hy,-hz);
    v_dat[idx++].setValue(-hx,-hy,hz);	// around the front
    v_dat[idx++].setValue(hx,-hy,hz);
    v_dat[idx++].setValue(hx,hy,hz);
    v_dat[idx++].setValue(-hx,hy,hz);
  }
  else {
    v_dat[idx++].setValue(0.0f,0.0f,-depth);	// around the back
    v_dat[idx++].setValue(width,0.0f,-depth);
    v_dat[idx++].setValue(width,height,-depth);
    v_dat[idx++].setValue(0.0f,height,-depth);
    v_dat[idx++].setValue(0.0f,0.0f,0.0f);	// around the front
    v_dat[idx++].setValue(width,0.0f,0.0f);
    v_dat[idx++].setValue(width,height,0.0f);
    v_dat[idx++].setValue(0.0f,height,0.0f);
  }
  vtx.finishEditing();

  coordIndex.setNum(24);
  int32_t* c_dat = coordIndex.startEditing();
  idx = 0;
  c_dat[idx++] = 0;		// around the back
  c_dat[idx++] = 1;
  c_dat[idx++] = 2;
  c_dat[idx++] = 3;
  c_dat[idx++] = 0;
  c_dat[idx++] = -1;		// 6

  c_dat[idx++] = 4;		// around the front
  c_dat[idx++] = 5;
  c_dat[idx++] = 6;
  c_dat[idx++] = 7;
  c_dat[idx++] = 4;
  c_dat[idx++] = -1;		// 6

  c_dat[idx++] = 0;		// fb poles
  c_dat[idx++] = 4;
  c_dat[idx++] = -1;		// 3
  c_dat[idx++] = 1;
  c_dat[idx++] = 5;
  c_dat[idx++] = -1;		// 3
  c_dat[idx++] = 2;
  c_dat[idx++] = 6;
  c_dat[idx++] = -1;		// 3
  c_dat[idx++] = 3;
  c_dat[idx++] = 7;
  c_dat[idx++] = -1;		// 3
  // total = 2*6 + 4*3 = 24
  coordIndex.finishEditing();
}
