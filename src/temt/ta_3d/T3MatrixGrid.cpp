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

#include "T3MatrixGrid.h"

#include <taMatrix>
#include <T3Color>
#include <taVector2i>
#include <ColorScale>
#include <String_Matrix>
#include <taSvg>

#include <T3Misc>
#include <T3TriangleStrip>
#include <T3TwoDText>

#include <taMisc>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;

T3MatrixGrid::T3MatrixGrid(Qt3DNode* par, taMatrix* mat, int slice, bool oddy,
                           ColorScale* sc, MatrixLayout layout, bool val_txt)
  : inherited(par)
{
  tris = new T3TriangleStrip(this);
  cell_text = new T3Entity(this);
  matrix = mat;
  slice_idx = slice;
  odd_y = oddy;
  colorscale = sc;
  mat_layout = layout;
  val_text = val_txt;

  max_txt_len = 6;
  spacing = .1f;
  block_height = .2f;
  trans_max = 0.6f;
  user_data = NULL;
  render_svg = false;
  svg_str = NULL;

  norm_bk = QVector3D(0.0f,  1.0f, 0.0f);   // back = y up
  norm_fr = QVector3D(0.0f,  -1.0f , 0.0f); // front = y down
  norm_lf = QVector3D(-1.0f, 0.0f , 0.0f);  // left = x left
  norm_rt = QVector3D(1.0f,  0.0f , 0.0f);  // right = x right
  norm_tp = QVector3D(0.0f,  0.0f , 1.0f);  // top = z up
}

T3MatrixGrid::~T3MatrixGrid() {
  matrix.CutLinks();
}

void T3MatrixGrid::setMatrix(taMatrix* mat, bool oddy, int slice) { 
  matrix = mat; 
  odd_y = oddy;
  slice_idx = slice;
  render();
}

void T3MatrixGrid::setColorScale(ColorScale* cs) { 
  colorscale = cs;
  render();
}

void T3MatrixGrid::setLayout(MatrixLayout layout) { 
  mat_layout = layout;
  render();
}

void T3MatrixGrid::setValText(bool val_txt) { 
  val_text = val_txt;
  render();
}

void T3MatrixGrid::setBlockHeight(float bh) { 
  block_height = bh;
  render();
}

void T3MatrixGrid::render() {
  if(!matrix || !colorscale) return;

  taMatrixPtr matptr;           // actual matrix to plot -- could be slice
  if(slice_idx >= 0) {
    matptr = matrix->GetFrameSlice_(slice_idx);
  }
  else {
    matptr = matrix;
  }

  int geom_x, geom_y;
  matptr->geom.Get2DGeomGui(geom_x, geom_y, odd_y, 1);
  cl_x = 1.0f / (float)geom_x;	// how big each cell is
  cl_y = 1.0f / (float)geom_y;
  max_xy = MAX(cl_x, cl_y);
  cl_spc = spacing * max_xy;
  blk_ht = block_height * max_xy;
  
  ufontsz = MIN(1.5f * (cl_x / (float)max_txt_len), cl_y);
  cell_text->removeAllChildren();

  String val_str;
  taVector2i pos;

  tris->setNodeUpdating(true);
  tris->restart();
  
  if(matptr->dims() <= 2) {
    int nx = geom_x;
    for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x; pos.x++) { // left to right
        renderBlock(pos);
      }
    }
  }
  else if(matptr->dims() == 3) {
    int xmax = matptr->dim(0);
    int ymax = matptr->dim(1);
    int zmax = matptr->dim(2);
    for(int z=zmax-1; z>=0; z--) {
      for(pos.y=ymax-1; pos.y>=0; pos.y--) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  taVector2i apos = pos;
	  if(odd_y)
	    apos.y += z * (ymax+1);
	  else
	    apos.x += z * (xmax+1);
          renderBlock(apos);
        }
      }
    }
  }
  else if(matptr->dims() == 4) {
    int xmax = matptr->dim(0);
    int ymax = matptr->dim(1);
    int xxmax = matptr->dim(2);
    int yymax = matptr->dim(3);
    taVector2i opos;
    for(opos.y=yymax-1; opos.y>=0; opos.y--) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=ymax-1; pos.y>=0; pos.y--) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    taVector2i apos = pos;
	    apos.x += opos.x * (xmax+1);
	    apos.y += opos.y * (ymax+1);
            renderBlock(apos);
          }
        }
      }
    }
  }

  renderValues();		// hand off to next guy..
}

void T3MatrixGrid::renderValues() {
  if(!matrix || !colorscale) return;

  taMatrixPtr matptr;           // actual matrix to plot -- could be slice
  if(slice_idx >= 0) {
    matptr = matrix->GetFrameSlice_(slice_idx);
  }
  else {
    matptr = matrix;
  }

  int geom_x, geom_y;
  matptr->geom.Get2DGeomGui(geom_x, geom_y, odd_y, 1);

  tris->setNodeUpdating(true);

  if(render_svg) {
    *svg_str << taSvg::Group();
  }

  float val = 0.0f;
  taVector2i pos;
  int c_idx = 0;
  int t_idx = 0;
  iColor clr;
  String str_val;

  bool use_str_val = val_text && (matptr->GetDataValType() == taBase::VT_STRING);
  
  if(matptr->dims() <= 2) {
    for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x; pos.x++) { // left to right
        if(matptr->dims() == 1) { // 1d is arranged vertically, not horizontally!
          if(mat_layout == BOT_ZERO) {
            if(use_str_val)
              str_val = ((String_Matrix*)matptr.ptr())->FastEl1d(geom_y-1-pos.y);
            else
              val = matptr->FastElAsFloat(geom_y-1-pos.y);
          }
          else {
            if(use_str_val)
              str_val = ((String_Matrix*)matptr.ptr())->FastEl1d(pos.y);
            else
              val = matptr->FastElAsFloat(pos.y);
          }
        }
        else {
          if(mat_layout == BOT_ZERO) {
            if(use_str_val)
              str_val = ((String_Matrix*)matptr.ptr())->FastEl2d(pos.x, geom_y-1-pos.y);
            else
              val = matptr->FastElAsFloat(pos.x, geom_y-1-pos.y);
          }
          else {
            if(use_str_val)
              str_val = ((String_Matrix*)matptr.ptr())->FastEl2d(pos.x, pos.y);
            else
              val = matptr->FastElAsFloat(pos.x, pos.y);
          }
        }
        renderValue(matptr, val, c_idx, clr, t_idx, use_str_val, str_val);
        if(render_svg) {
          renderSvg(matptr, pos, clr);
        }
      }
    }
  }
  else if(matptr->dims() == 3) {
    int xmax = matptr->dim(0);
    int ymax = matptr->dim(1);
    int zmax = matptr->dim(2);
    for(int z=zmax-1; z>=0; z--) {
      for(pos.y=ymax-1; pos.y>=0; pos.y--) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  if(mat_layout == BOT_ZERO) {
            if(use_str_val)
              str_val = ((String_Matrix*)matptr.ptr())->FastEl3d(pos.x, ymax-1-pos.y, zmax-1-z);
            else
              val = matptr->FastElAsFloat(pos.x, ymax-1-pos.y, zmax-1-z);
          }
	  else {
            if(use_str_val)
              str_val = ((String_Matrix*)matptr.ptr())->FastEl3d(pos.x, pos.y, z);
            else
              val = matptr->FastElAsFloat(pos.x, pos.y, z);
          }
          renderValue(matptr, val, c_idx, clr, t_idx, use_str_val, str_val);
          if(render_svg) {
            taVector2i apos = pos;
            if(odd_y)
              apos.y += z * (ymax+1);
            else
              apos.x += z * (xmax+1);
            renderSvg(matptr, apos, clr);
          }    
	}
      }
    }
  }
  else if(matptr->dims() == 4) {
    int xmax = matptr->dim(0);
    int ymax = matptr->dim(1);
    int xxmax = matptr->dim(2);
    int yymax = matptr->dim(3);
    taVector2i opos;
    for(opos.y=yymax-1; opos.y>=0; opos.y--) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=ymax-1; pos.y>=0; pos.y--) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    if(mat_layout == BOT_ZERO) {
              if(use_str_val)
                str_val = ((String_Matrix*)matptr.ptr())->FastEl4d(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
              else
                val = matptr->FastElAsFloat(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
            }
	    else {
              if(use_str_val)
                str_val = ((String_Matrix*)matptr.ptr())->FastEl4d(pos.x, pos.y, opos.x, opos.y);
              else
                val = matptr->FastElAsFloat(pos.x, pos.y, opos.x, opos.y);
            }
            renderValue(matptr, val, c_idx, clr, t_idx, use_str_val, str_val);
            if(render_svg) {
              taVector2i apos = pos;
              apos.x += opos.x * (xmax+1);
              apos.y += opos.y * (ymax+1);
              renderSvg(matptr, apos, clr);
            }    
	  }
	}
      }
    }
  }

  if(render_svg) {
    *svg_str << taSvg::GroupEnd();
  }
  tris->setNodeUpdating(false);
}


void T3MatrixGrid::renderBlock(const taVector2i& pos) {
  float zp0 = 0.0f;
  // front is yp0, back is yp1
  // left is xp0, right is xp0
  // bottom is zp0, top is zp1

  // triangle strip order is 0 1 2, 2 1 3
  // back order: clockwise
  // 1 3  10 11 = 00, 10, 01, 11
  // 0 2  00 01
  // front order: counter-clockwise
  // 2 3        = 00, 01, 10, 11
  // 0 1
  
  float xp0 = ((float)pos.x + cl_spc) * cl_x;
  float yp0 = ((float)pos.y + cl_spc) * cl_y;
  float xp1 = ((float)pos.x+1 - cl_spc) * cl_x;
  float yp1 = ((float)pos.y+1 - cl_spc) * cl_y;
  yp0 = 1.0f - yp0; yp1 = 1.0f - yp1; // always flip y
  float zp1 = blk_ht;
  
  int st_idx = tris->vertexCount();

  // have to do this face-by-face to make the normals work out -- go back to front
  
  // back face = yp1, x,z -- clock
  tris->addVertex(QVector3D(xp0, yp1, zp0), norm_bk); // 00
  tris->addVertex(QVector3D(xp1, yp1, zp0), norm_bk); // 10
  tris->addVertex(QVector3D(xp0, yp1, zp1), norm_bk); // 01
  tris->addVertex(QVector3D(xp1, yp1, zp1), norm_bk); // 11

  // left face = xp0, y,z -- clock
  tris->addVertex(QVector3D(xp0, yp0, zp0), norm_lf); // 00
  tris->addVertex(QVector3D(xp0, yp1, zp0), norm_lf); // 10
  tris->addVertex(QVector3D(xp0, yp0, zp1), norm_lf); // 01
  tris->addVertex(QVector3D(xp0, yp1, zp1), norm_lf); // 11

  // right face = xp1, y,z -- counter
  tris->addVertex(QVector3D(xp1, yp0, zp0), norm_rt); // 00
  tris->addVertex(QVector3D(xp1, yp0, zp1), norm_rt); // 01
  tris->addVertex(QVector3D(xp1, yp1, zp0), norm_rt); // 10
  tris->addVertex(QVector3D(xp1, yp1, zp1), norm_rt); // 11

  // front face = yp0, x,z -- counter
  tris->addVertex(QVector3D(xp0, yp0, zp0), norm_fr); // 00
  tris->addVertex(QVector3D(xp0, yp0, zp1), norm_fr); // 01
  tris->addVertex(QVector3D(xp1, yp0, zp0), norm_fr); // 10
  tris->addVertex(QVector3D(xp1, yp0, zp1), norm_fr); // 11

  // top face = zp1, x,y -- counter
  tris->addVertex(QVector3D(xp0, yp0, zp1), norm_tp); // 00
  tris->addVertex(QVector3D(xp0, yp1, zp1), norm_tp); // 01
  tris->addVertex(QVector3D(xp1, yp0, zp1), norm_tp); // 10
  tris->addVertex(QVector3D(xp1, yp1, zp1), norm_tp); // 11

  for(int i=0;i<20;i++) {        
    tris->addColor((uint32_t)0); // place holder..
    tris->addIndex(st_idx++);    // indexes are 1-to-1..
    if((i+1) % 4 == 0) {
      tris->addBreak();         // all we really are using them for is the break..
    }
  }

  if(val_text) {
    renderText(xp0, xp1, yp0, yp1, zp1);
  }
}

void T3MatrixGrid::renderText(float xp0, float xp1, float yp0, float yp1, float zp1) {
  T3TwoDText* txt = new T3TwoDText(cell_text);
  txt->align = T3_ALIGN_CENTER;
  float xfp = .5f * (xp0 + xp1);
  float yfp = .5f * (yp0 + yp1) - .5f * ufontsz;
  float zfp = MAX(zp1,0.0f) + .01f;
  txt->Translate(xfp, yfp, zfp);
  txt->Scale(ufontsz);
}

void T3MatrixGrid::renderValue(taMatrix* matptr, float val, int& c_idx, iColor& clr,
                               int& t_idx, bool use_str_val, const String& str_val) {
  String val_str;
  float sc_val;
  iColor tx;
  colorscale->GetColor(val,sc_val,&clr,&tx);
  float zp1 = sc_val * blk_ht;

  float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
  clr.setAlpha(alpha);
  for(int i=0;i<20;i++) {
    tris->setPointColor(c_idx + i, clr);
  }

  // update verticies
  const int n_zverts = 12;
  int zverts[n_zverts] = {2,3,6,7,9,11,13,15,16,17,18,19};
  for(int i=0;i<n_zverts;i++) {
    tris->tris->vndata.FastEl3d(2, 0, c_idx + zverts[i]) = zp1;
  }
  c_idx += 20;
  
  if(val_text) {
    if(use_str_val) {
      val_str = str_val;
    }
    else {
      ValToDispText(val, val_str);
    }
    T3TwoDText* txt = dynamic_cast<T3TwoDText*>(cell_text->children().at(t_idx++));
    txt->setText(val_str);
  }
}

void T3MatrixGrid::ValToDispText(float val, String& str) {
  float val_abs = fabsf(val);
  if ((val_abs < .0001) || (val_abs >= 10000))
    str.convert(val, "%7.1e"); //note: 7 chars
  if (val_abs < 1)
    str.convert(val, "%#6.4f");
  else if (val_abs < 10)
    str.convert(val, "%#6.3f");
  else if (val_abs < 100)
    str.convert(val, "%#6.2f");
  else if (val_abs < 1000)
    str.convert(val, "%#6.1f");
  else //must be: if (val_abs < 10000)
    str.convert(val, "6.0f");
}

void T3MatrixGrid::renderSvg(taMatrix* matptr, const taVector2i& pos, iColor& clr) {
  float xp0 = svg_off.x + svg_sz.x * ((float)pos.x + cl_spc) * cl_x;
  float yp0 = svg_off.y + svg_sz.y * ((float)pos.y + cl_spc) * cl_y;
  float xp1 = svg_off.x + svg_sz.x * ((float)pos.x+1 - cl_spc) * cl_x;
  float yp1 = svg_off.y + svg_sz.y * ((float)pos.y+1 - cl_spc) * cl_y;
  yp0 = 1.0f - yp0; yp1 = 1.0f - yp1; // always flip y

  *svg_str << taSvg::Path(clr, -1.0f, true, clr)
           << "M " << taSvg::Coords(xp0, yp0, 0.0f)
           << "L " << taSvg::Coords(xp0, yp1, 0.0f)
           << "L " << taSvg::Coords(xp1, yp1, 0.0f)
           << "L " << taSvg::Coords(xp1, yp0, 0.0f)
           << taSvg::PathEnd();
}

void T3MatrixGrid::mouseClicked(Q3DMouseEvent* mouse) {
  int xp = mouse->x();
  int yp = mouse->y();
  taMisc::Info("mouse: ", String(xp), ",", String(yp));
}

void T3MatrixGrid::mouseDoubleClicked(Q3DMouseEvent* mouse) {
  
}
