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
  float cl_x = 1.0f / (float)geom_x;	// how big each cell is
  float cl_y = 1.0f / (float)geom_y;
  max_xy = MAX(cl_x, cl_y);
  cl_spc = spacing * max_xy;
  blk_ht = block_height * max_xy;
  
  int n_geom = matptr->count();	// not always x * y due to spaces in display geom
  int n_per_vtx = 8;
  int tot_vtx =  n_geom * n_per_vtx;
  // tris->tris->vndata.SetGeom(3,2,tot_vtx);
  // tris->tris->colors.SetSize(tot_vtx);

  if(render_svg) {
    *svg_str << taSvg::Group();
  }

  bool build_text = false;
  float ufontsz = MIN(1.5f * (cl_x / (float)max_txt_len), cl_y);
  cell_text->removeAllChildren();

  String val_str;
  taVector2i pos;
  int v_idx = 0;
  int t_idx = 2;		// base color + font
  // these go in normal order; indexes are backwards

  tris->setNodeUpdating(true);
  
  if(matptr->dims() <= 2) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float xp = ((float)pos.x + cl_spc) * cl_x;
	float yp = ((float)pos.y + cl_spc) * cl_y;
	float xp1 = ((float)pos.x+1 - cl_spc) * cl_x;
	float yp1 = ((float)pos.y+1 - cl_spc) * cl_y;
	yp = 1.0f - yp; yp1 = 1.0f - yp1; // always flip y
	float zp = blk_ht;

        render_block_verts(xp, yp, xp1, yp1, zp);
        
	if(val_text) {
	  render_text(build_text, t_idx, xp, xp1, yp, yp1, zp, ufontsz);
	}
      }
    }
  }
  else if(matptr->dims() == 3) {
    int xmax = matptr->dim(0);
    int ymax = matptr->dim(1);
    int zmax = matptr->dim(2);
    for(int z=0; z<zmax; z++) {
      for(pos.y=0; pos.y<ymax; pos.y++) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  taVector2i apos = pos;
	  if(odd_y)
	    apos.y += z * (ymax+1);
	  else
	    apos.x += z * (xmax+1);
	  float xp = ((float)apos.x + cl_spc) * cl_x;
	  float yp = ((float)apos.y + cl_spc) * cl_y;
	  float xp1 = ((float)apos.x+1 - cl_spc) * cl_x;
	  float yp1 = ((float)apos.y+1 - cl_spc) * cl_y;
	  float zp = blk_ht;
	  yp = 1.0f - yp; yp1 = 1.0f - yp1;		// always flip y
          render_block_verts(xp, yp, xp1, yp1, zp);
	  if(val_text) {
	    render_text(build_text, t_idx, xp, xp1, yp, yp1, zp, ufontsz);
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
    for(opos.y=0; opos.y<yymax; opos.y++) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=0; pos.y<ymax; pos.y++) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    taVector2i apos = pos;
	    apos.x += opos.x * (xmax+1);
	    apos.y += opos.y * (ymax+1);
	    float xp = ((float)apos.x + cl_spc) * cl_x;
	    float yp = ((float)apos.y + cl_spc) * cl_y;
	    float xp1 = ((float)apos.x+1 - cl_spc) * cl_x;
	    float yp1 = ((float)apos.y+1 - cl_spc) * cl_y;
	    float zp = blk_ht;
	    yp = 1.0f - yp; yp1 = 1.0f - yp1;		  // always flip 

            render_block_verts(xp, yp, xp1, yp1, zp);

	    if(val_text) {
	      render_text(build_text, t_idx, xp, xp1, yp, yp1, zp, ufontsz);
	    }
          }
	}
      }
    }
  }
  
  // values of the cubes xy_[0,v]
  //     01_v   11_v   
  //   01_0   11_0     
  //     00_v   10_v    
  //   00_0   10_0     

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4

  if(matptr->dims() <= 2) {
    int nx = geom_x;
    for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	int mat_idx = (pos.y * nx + pos.x);
	int c00_0 = mat_idx * n_per_vtx;
        
	render_block_idx(c00_0);

        if(render_svg) {
          // this has to be here because of the backward ordering
          float xp = svg_off.x + svg_sz.x * ((float)pos.x + cl_spc) * cl_x;
          float yp = svg_off.y + svg_sz.y * ((float)pos.y + cl_spc) * cl_y;
          float xp1 = svg_off.x + svg_sz.x * ((float)pos.x+1 - cl_spc) * cl_x;
          float yp1 = svg_off.y + svg_sz.y * ((float)pos.y+1 - cl_spc) * cl_y;
          yp = 1.0f - yp; yp1 = 1.0f - yp1; // always flip y

          float val, sc_val;
          if(matptr->dims() == 1) { // 1d is arranged vertically, not horizontally!
            if(mat_layout == BOT_ZERO)
              val = matptr->FastElAsFloat(geom_y-1-pos.y);
            else
              val = matptr->FastElAsFloat(pos.y);
          }
          else {
            if(mat_layout == BOT_ZERO)
              val = matptr->FastElAsFloat(pos.x, geom_y-1-pos.y);
            else
              val = matptr->FastElAsFloat(pos.x, pos.y);
          }
          iColor fl;  iColor tx;
          colorscale->GetColor(val,sc_val,&fl,&tx);
          float zp = sc_val * blk_ht;
          float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
          fl.a = iColor::fc2ic(alpha);

          *svg_str << taSvg::Path(fl, -1.0f, true, fl)
                   << "M " << taSvg::Coords(xp, yp, 0.0f)
                   << "L " << taSvg::Coords(xp, yp1, 0.0f)
                   << "L " << taSvg::Coords(xp1, yp1, 0.0f)
                   << "L " << taSvg::Coords(xp1, yp, 0.0f)
                   << taSvg::PathEnd();
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
	  int mat_idx = matptr->FastElIndex(pos.x, pos.y, z);
	  int c00_0 = mat_idx * n_per_vtx;
	  render_block_idx(c00_0);
          if(render_svg) {
            taVector2i apos = pos;
            if(odd_y)
              apos.y += z * (ymax+1);
            else
              apos.x += z * (xmax+1);
            // this has to be here because of the backward ordering
            float xp = svg_off.x + svg_sz.x * ((float)apos.x + cl_spc) * cl_x;
            float yp = svg_off.y + svg_sz.y * ((float)apos.y + cl_spc) * cl_y;
            float xp1 = svg_off.x + svg_sz.x * ((float)apos.x+1 - cl_spc) * cl_x;
            float yp1 = svg_off.y + svg_sz.y * ((float)apos.y+1 - cl_spc) * cl_y;
            yp = 1.0f - yp; yp1 = 1.0f - yp1; // always flip y

            float val, sc_val;
            if(mat_layout == BOT_ZERO)
              val = matptr->FastElAsFloat(pos.x, ymax-1-pos.y, zmax-1-z);
            else
              val = matptr->FastElAsFloat(pos.x, pos.y, z);
            iColor fl;  iColor tx;
            colorscale->GetColor(val,sc_val,&fl,&tx);
            float zp = sc_val * blk_ht;
            float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
            fl.a = iColor::fc2ic(alpha);

            *svg_str << taSvg::Path(fl, -1.0f, true, fl)
                     << "M " << taSvg::Coords(xp, yp, 0.0f)
                     << "L " << taSvg::Coords(xp, yp1, 0.0f)
                     << "L " << taSvg::Coords(xp1, yp1, 0.0f)
                     << "L " << taSvg::Coords(xp1, yp, 0.0f)
                     << taSvg::PathEnd();
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
	    int mat_idx = matptr->FastElIndex(pos.x, pos.y, opos.x, opos.y);
	    int c00_0 = mat_idx * n_per_vtx;
	    render_block_idx(c00_0);

            if(render_svg) {
              taVector2i apos = pos;
              apos.x += opos.x * (xmax+1);
              apos.y += opos.y * (ymax+1);
              // this has to be here because of the backward ordering
              float xp = svg_off.x + svg_sz.x * ((float)apos.x + cl_spc) * cl_x;
              float yp = svg_off.y + svg_sz.y * ((float)apos.y + cl_spc) * cl_y;
              float xp1 = svg_off.x + svg_sz.x * ((float)apos.x+1 - cl_spc) * cl_x;
              float yp1 = svg_off.y + svg_sz.y * ((float)apos.y+1 - cl_spc) * cl_y;
              yp = 1.0f - yp; yp1 = 1.0f - yp1; // always flip y

              float val, sc_val;
              if(mat_layout == BOT_ZERO)
                val = matptr->FastElAsFloat(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
              else
                val = matptr->FastElAsFloat(pos.x, pos.y, opos.x, opos.y);
              iColor fl;  iColor tx;
              colorscale->GetColor(val,sc_val,&fl,&tx);
              float zp = sc_val * blk_ht;
              float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
              fl.a = iColor::fc2ic(alpha);

              *svg_str << taSvg::Path(fl, -1.0f, true, fl)
                       << "M " << taSvg::Coords(xp, yp, 0.0f)
                       << "L " << taSvg::Coords(xp, yp1, 0.0f)
                       << "L " << taSvg::Coords(xp1, yp1, 0.0f)
                       << "L " << taSvg::Coords(xp1, yp, 0.0f)
                       << taSvg::PathEnd();
            }    
          }
        }
      }
    }
  }

  renderValues();		// hand off to next guy..

  if(render_svg) {
    *svg_str << taSvg::GroupEnd();
  }
}

void T3MatrixGrid::render_text(bool build_text, int& t_idx, float xp, float xp1,
			       float yp, float yp1, float zp, float ufontsz)
{
  // if(build_text || cell_text_->getNumChildren() <= t_idx) {
    // SoSeparator* tsep = new SoSeparator;
    // cell_text_->addChild(tsep);
    // SoTranslation* tr = new SoTranslation;
    // tsep->addChild(tr);
    // SoAsciiText* txt = new SoAsciiText();
    // txt->justification = SoAsciiText::CENTER;
    // tsep->addChild(txt);
  // }
  // SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
  // SoTranslation* tr = (SoTranslation*)tsep->getChild(0);
  // float xfp = .5f * (xp + xp1);
  // float yfp = .5f * (yp + yp1) - .5f * ufontsz;
  // tr->translation.setValue(xfp, yfp, MAX(zp,0.0f) + .01f);
  // SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
  // txt->string.setValue("0.0");	// placeholder; gets set later
  t_idx++;
}

void T3MatrixGrid::render_block_verts(float xp, float yp, float xp1, float yp1, float zp) {
  // todo -- we really need 45 degree normals here probably..
  QVector3D norm_bk(0.0f,  1.0f, 0.0f);
  QVector3D norm_rt(1.0f,  0.0f , 0.0f);
  QVector3D norm_lf(-1.0f, 0.0f , 0.0f);
  QVector3D norm_fr(0.0f,  -1.0f , 0.0f);
  QVector3D norm_tp(0.0f,  0.0f , 1.0f);

  tris->addVertex(QVector3D(xp, yp, 0.0f), norm_lf); // 00_0 = 0
  tris->addVertex(QVector3D(xp1, yp , 0.0f), norm_rt); // 10_0 = 0
  tris->addVertex(QVector3D(xp,  yp1, 0.0f), norm_fr); // 01_0 = 0
  tris->addVertex(QVector3D(xp1, yp1, 0.0f), norm_fr); // 11_0 = 0

  // zp will be updated later!
  tris->addVertex(QVector3D(xp,  yp , zp), norm_tp); // 00_v = 1
  tris->addVertex(QVector3D(xp1, yp , zp), norm_tp); // 10_v = 2
  tris->addVertex(QVector3D(xp,  yp1, zp), norm_tp); // 01_v = 3
  tris->addVertex(QVector3D(xp1, yp1, zp), norm_tp); // 11_v = 4

  for(int i=0;i<8;i++) {        // place holder..
    tris->addColor((uint32_t)0);
  }
}


void T3MatrixGrid::render_block_idx(int c00_0) {
  int c10_0 = c00_0 + 1; int c01_0 = c00_0 + 2; int c11_0 = c00_0 + 3;
  int c00_v = c00_0 + 4; int c10_v = c00_0 + 5; int c01_v = c00_0 + 6;
  int c11_v = c00_0 + 7;

  // todo: front and right sides are culled -- needs to be wound the other way..
  
  // back - right
  //     1    3
  //   0    2     
  //     x    5  
  //   x    4   

  tris->addTriangle(c01_0, c01_v, c11_0); // 0 1 2
  tris->addTriangle(c11_v, c10_0, c10_v); // 3 4 5
  tris->addBreak();                       // 7 total

  // left - front
  //     1    x 
  //   0    x   
  //     3    5
  //   2    4   

  tris->addTriangle(c01_0, c01_v, c00_0); // 0 1 2
  tris->addTriangle(c00_v, c10_0, c10_v); // 3 4 5
  // tris->addTriangle(c00_0, c01_v, c01_0); // 0 1 2
  // tris->addTriangle(c10_v, c10_0, c00_v); // 3 4 5
  tris->addBreak();                       // 7 total

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4
  // top
  //     0    1
  //   x    x  
  //     2    3
  //   x    x  

  tris->addTriangle(c01_v, c11_v, c00_v); // 0 1 2 
  tris->addIndex(c10_v); // 3
  tris->addBreak(); // 5 total

  // total coords = 7 + 7 + 5 = 19
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

  String val_str;
  float val;
  float sc_val;
  taVector2i pos;
  int v_idx = 0;
  int c_idx = 0;
  int t_idx = 0;

  // these go in normal order; indexes are backwards
  if(matptr->dims() == 1) {
    pos.x = 0;
    int ymax = matptr->dim(0);	// assumes odd_y
    for(pos.y=0; pos.y<ymax; pos.y++) {
      if(mat_layout == BOT_ZERO)
	val = matptr->FastElAsFloat(ymax-1-pos.y);
      else
	val = matptr->FastElAsFloat(pos.y);

      iColor fl;  iColor tx;
      colorscale->GetColor(val,sc_val,&fl,&tx);
      float zp = sc_val * blk_ht;
      v_idx+=4;			// skip the _0 cases
      for(int i=0;i<4;i++) {
	tris->tris->vndata.FastEl3d(2, 0, v_idx++) = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
      }
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
      int clr = T3Misc::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
      for(int i=0;i<8;i++) {
        tris->tris->colors.FastEl(c_idx++) = clr;
      }
      if(val_text) {
	if(matptr->GetDataValType() == taBase::VT_STRING) {
	  if(mat_layout == BOT_ZERO) {
	    val_str = ((String_Matrix*)matptr.ptr())->
              FastEl1d(ymax-1-pos.y).elidedTo(max_txt_len);
          }
	  else {
	    val_str = ((String_Matrix*)matptr.ptr())->
              FastEl1d(pos.y).elidedTo(max_txt_len);
          }
	}
	else {
	  ValToDispText(val, val_str);
	}
	// txt->string.setValue(val_str.chars());
	t_idx++;
      }
    }
  }
  else if(matptr->dims() == 2) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	if(mat_layout == BOT_ZERO)
	  val = matptr->FastElAsFloat(pos.x, geom_y-1-pos.y);
	else
	  val = matptr->FastElAsFloat(pos.x, pos.y);
	iColor fl;  iColor tx;
	colorscale->GetColor(val,sc_val,&fl,&tx);
	float zp = sc_val * blk_ht;
	v_idx+=4;			// skip the _0 cases
	for(int i=0;i<4;i++) {
	  tris->tris->vndata.FastEl3d(2, 0, v_idx++) = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
        }
	float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
        int clr = T3Misc::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
        for(int i=0;i<8;i++) {
          tris->tris->colors.FastEl(c_idx++) = clr;
        }
	if(val_text) {
	  // SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	  // SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	  if(matptr->GetDataValType() == taBase::VT_STRING) {
	    if(mat_layout == BOT_ZERO) {
	      val_str = ((String_Matrix*)matptr.ptr())->
                FastEl2d(pos.x, geom_y-1-pos.y).elidedTo(max_txt_len);
            }
	    else {
	      val_str = ((String_Matrix*)matptr.ptr())->
                FastEl2d(pos.x, pos.y).elidedTo(max_txt_len);
            }
	  }
	  else {
	    ValToDispText(val, val_str);
	  }
	  // txt->string.setValue(val_str.chars());
	  t_idx++;
	}
      }
    }
  }
  else if(matptr->dims() == 3) {
    int xmax = matptr->dim(0);
    int ymax = matptr->dim(1);
    int zmax = matptr->dim(2);
    for(int z=0; z<zmax; z++) {
      for(pos.y=0; pos.y<ymax; pos.y++) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  if(mat_layout == BOT_ZERO)
	    val = matptr->FastElAsFloat(pos.x, ymax-1-pos.y, zmax-1-z);
	  else
	    val = matptr->FastElAsFloat(pos.x, pos.y, z);
	  iColor fl;  iColor tx;
	  colorscale->GetColor(val,sc_val,&fl,&tx);
	  float zp = sc_val * blk_ht;
	  v_idx+=4;			// skip the _0 cases
	  for(int i=0;i<4;i++) {
	    tris->tris->vndata.FastEl3d(2, 0, v_idx++) = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
          }
	  float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
          int clr = T3Misc::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
          for(int i=0;i<8;i++) {
            tris->tris->colors.FastEl(c_idx++) = clr;
          }
	  if(val_text) {
	    // SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	    // SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	    if(matptr->GetDataValType() == taBase::VT_STRING) { // todo: replicate if compiles
	      if(mat_layout == BOT_ZERO) {
		val_str = ((String_Matrix*)matptr.ptr())->
                  FastEl3d(pos.x, ymax-1-pos.y, zmax-1-z).elidedTo(max_txt_len);
              }
	      else {
		val_str = ((String_Matrix*)matptr.ptr())->
                  FastEl3d(pos.x, pos.y, z).elidedTo(max_txt_len);
              }
	    }
	    else {
	      ValToDispText(val, val_str);
	    }
	    // txt->string.setValue(val_str.chars());
	    t_idx++;
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
    for(opos.y=0; opos.y<yymax; opos.y++) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=0; pos.y<ymax; pos.y++) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    if(mat_layout == BOT_ZERO)
	      val = matptr->FastElAsFloat(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
	    else
	      val = matptr->FastElAsFloat(pos.x, pos.y, opos.x, opos.y);
	    iColor fl;  iColor tx;
	    colorscale->GetColor(val,sc_val,&fl,&tx);
	    float zp = sc_val * blk_ht;
	    v_idx+=4;			// skip the _0 cases
	    for(int i=0;i<4;i++) {
              tris->tris->vndata.FastEl3d(2, 0, v_idx++) = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
            }
	    float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
            int clr = T3Misc::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
            for(int i=0;i<8;i++) {
              tris->tris->colors.FastEl(c_idx++) = clr;
            }
	    if(val_text) {
	      // SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	      // SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	      if(matptr->GetDataValType() == taBase::VT_STRING) {
		if(mat_layout == BOT_ZERO) {
		  val_str = ((String_Matrix*)matptr.ptr())->
                    FastEl4d(pos.x, ymax-1-pos.y, opos.x,
                             yymax-1-opos.y).elidedTo(max_txt_len);
                }
		else {
		  val_str = ((String_Matrix*)matptr.ptr())->
                    FastEl4d(pos.x, pos.y, opos.x, opos.y).elidedTo(max_txt_len);
                }
	      }
	      else {
		ValToDispText(val, val_str);
	      }
	      // txt->string.setValue(val_str.chars());
	      t_idx++;
	    }
	  }
	}
      }
    }
  }

  tris->setNodeUpdating(false);
}


