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

#include "SoMatrixGrid.h"
#include <taMatrix>
#include <T3Color>
#include <taVector2i>
#include <ColorScale>
#include <String_Matrix>
#include <taSvg>

#include <taMisc>


#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoAsciiText.h>

SO_NODE_SOURCE(SoMatrixGrid);

void SoMatrixGrid::initClass()
{
  SO_NODE_INIT_CLASS(SoMatrixGrid, SoSeparator, "SoSeparator");
}

SoMatrixGrid::SoMatrixGrid(taMatrix* mat, bool oddy, ColorScale* sc, MatrixLayout layout, bool val_txt) {
  SO_NODE_CONSTRUCTOR(SoMatrixGrid);

  transform_ = new SoTransform;
  addChild(transform_);
  shape_ = new SoIndexedTriangleStripSet;
  addChild(shape_);
  vtx_prop_ = new SoVertexProperty;
  shape_->vertexProperty.setValue(vtx_prop_);
  cell_text_ = NULL;
  cell_fnt_ = NULL;

  matrix = mat;
  odd_y = oddy;
  scale = sc;
  mat_layout = layout;
  val_text = val_txt;

  max_txt_len = 6;
  spacing = .1f;
  block_height = .2f;
  trans_max = 0.6f;
  user_data = NULL;
  render_svg = false;
  svg_str = NULL;
  //  render(); // don't do this by default; often want to spec further guys
}

SoMatrixGrid::~SoMatrixGrid() {
  matrix.CutLinks();
}

void SoMatrixGrid::setMatrix(taMatrix* mat, bool oddy) { 
  matrix = mat; 
  odd_y = oddy;
  render();
}

void SoMatrixGrid::setColorScale(ColorScale* cs) { 
  scale = cs;
  render();
}

void SoMatrixGrid::setLayout(MatrixLayout layout) { 
  mat_layout = layout;
  render();
}

void SoMatrixGrid::setValText(bool val_txt) { 
  val_text = val_txt;
  render();
}

void SoMatrixGrid::setBlockHeight(float bh) { 
  block_height = bh;
  render();
}

void SoMatrixGrid::ValToDispText(float val, String& str) {
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

void SoMatrixGrid::render() {
  if(!matrix || !scale) return;
  if(!shape_ || !vtx_prop_) return; // something wrong..

  SoMFVec3f& vertex = vtx_prop_->vertex;
  SoMFVec3f& normal = vtx_prop_->normal;
  SoMFUInt32& color = vtx_prop_->orderedRGBA;

  vtx_prop_->normalBinding.setValue(SoNormalBinding::PER_FACE_INDEXED);
  vtx_prop_->materialBinding.setValue(SoMaterialBinding::PER_PART_INDEXED);

  normal.setNum(5);
  SbVec3f* normal_dat = normal.startEditing();
  int idx=0;
  normal_dat[idx++].setValue(0.0f,  -1.0f, 0.0f); // back = 0
  normal_dat[idx++].setValue(1.0f,  0.0f , 0.0f); // right = 1
  normal_dat[idx++].setValue(-1.0f, 0.0f , 0.0f); // left = 2
  normal_dat[idx++].setValue(0.0f,  1.0f , 0.0f); // front = 3
  normal_dat[idx++].setValue(0.0f,  0.0f , 1.0f); // top = 4
  normal.finishEditing();

  int geom_x, geom_y;
  matrix->geom.Get2DGeomGui(geom_x, geom_y, odd_y, 1);
//SB?  matrix->geom.Get2DGeom(geom_x, geom_y);
  float cl_x = 1.0f / (float)geom_x;	// how big each cell is
  float cl_y = 1.0f / (float)geom_y;
  max_xy = MAX(cl_x, cl_y);
  cl_spc = spacing * max_xy;
  blk_ht = block_height * max_xy;
  
  int n_geom = matrix->count();	// not always x * y due to spaces in display geom
  int n_per_vtx = 8;
  int tot_vtx =  n_geom * n_per_vtx;
  vertex.setNum(tot_vtx);
  color.setNum(n_geom);

  if(render_svg) {
    *svg_str << taSvg::Group();
  }

  bool build_text = false;
  float ufontsz = MIN(1.5f * (cl_x / (float)max_txt_len), cl_y);
  if(!val_text) {
    if (cell_text_) {
      removeChild(cell_text_);
      cell_fnt_ = NULL;
      cell_text_ = NULL;
    }
  }
  else {
    if(!cell_text_) {
      cell_text_ = new SoSeparator;
      build_text = true;
      addChild(cell_text_);
      SoBaseColor* bc = new SoBaseColor;
      bc->rgb.setValue(0, 0, 0); //black is default for text
      cell_text_->addChild(bc);
      cell_fnt_ = new SoFont();
      cell_fnt_->name = (const char*)taMisc::t3d_font_name;
      cell_text_->addChild(cell_fnt_);
    }
    cell_fnt_->size.setValue(ufontsz);
  }

  SbVec3f* vertex_dat = vertex.startEditing();

  String val_str;
  T3Color col;
  taVector2i pos;
  int v_idx = 0;
  int t_idx = 2;		// base color + font
  // these go in normal order; indexes are backwards

  if(matrix->dims() <= 2) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float xp = ((float)pos.x + cl_spc) * cl_x;
	float yp = ((float)pos.y + cl_spc) * cl_y;
	float xp1 = ((float)pos.x+1 - cl_spc) * cl_x;
	float yp1 = ((float)pos.y+1 - cl_spc) * cl_y;
	yp = 1.0f - yp; yp1 = 1.0f - yp1; // always flip y
	float zp = blk_ht;
	vertex_dat[v_idx++].setValue(xp,  yp , 0.0f); // 00_0 = 0
	vertex_dat[v_idx++].setValue(xp1, yp , 0.0f); // 10_0 = 0
	vertex_dat[v_idx++].setValue(xp,  yp1, 0.0f); // 01_0 = 0
	vertex_dat[v_idx++].setValue(xp1, yp1, 0.0f); // 11_0 = 0

	// zp will be updated later!
	vertex_dat[v_idx++].setValue(xp,  yp , zp); // 00_v = 1
	vertex_dat[v_idx++].setValue(xp1, yp , zp); // 10_v = 2
	vertex_dat[v_idx++].setValue(xp,  yp1, zp); // 01_v = 3
	vertex_dat[v_idx++].setValue(xp1, yp1, zp); // 11_v = 4

	if(val_text) {
	  render_text(build_text, t_idx, xp, xp1, yp, yp1, zp, ufontsz);
	}
      }
    }
  }
  else if(matrix->dims() == 3) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int zmax = matrix->dim(2);
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
	  vertex_dat[v_idx++].setValue(xp,  yp , 0.0f); // 00_0 = 0
	  vertex_dat[v_idx++].setValue(xp1, yp , 0.0f); // 10_0 = 0
	  vertex_dat[v_idx++].setValue(xp,  yp1, 0.0f); // 01_0 = 0
	  vertex_dat[v_idx++].setValue(xp1, yp1, 0.0f); // 11_0 = 0

	  // zp will be updated later!
	  vertex_dat[v_idx++].setValue(xp,  yp , zp); // 00_v = 1
	  vertex_dat[v_idx++].setValue(xp1, yp , zp); // 10_v = 2
	  vertex_dat[v_idx++].setValue(xp,  yp1, zp); // 01_v = 3
	  vertex_dat[v_idx++].setValue(xp1, yp1, zp); // 11_v = 4

	  if(val_text) {
	    render_text(build_text, t_idx, xp, xp1, yp, yp1, zp, ufontsz);
	  }
	}
      }
    }
  }
  else if(matrix->dims() == 4) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int xxmax = matrix->dim(2);
    int yymax = matrix->dim(3);
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
	    vertex_dat[v_idx++].setValue(xp,  yp , 0.0f); // 00_0 = 0
	    vertex_dat[v_idx++].setValue(xp1, yp , 0.0f); // 10_0 = 0
	    vertex_dat[v_idx++].setValue(xp,  yp1, 0.0f); // 01_0 = 0
	    vertex_dat[v_idx++].setValue(xp1, yp1, 0.0f); // 11_0 = 0

	    // zp will be updated later!
	    vertex_dat[v_idx++].setValue(xp,  yp , zp); // 00_v = 1
	    vertex_dat[v_idx++].setValue(xp1, yp , zp); // 10_v = 2
	    vertex_dat[v_idx++].setValue(xp,  yp1, zp); // 01_v = 3
	    vertex_dat[v_idx++].setValue(xp1, yp1, zp); // 11_v = 4

	    if(val_text) {
	      render_text(build_text, t_idx, xp, xp1, yp, yp1, zp, ufontsz);
	    }
          }
	}
      }
    }
  }
  
  vertex.finishEditing();

  // todo: could cleanup cell_text_ child list if extras, but not clear if needed

  SoMFInt32& coords = shape_->coordIndex;
  SoMFInt32& norms = shape_->normalIndex;
  SoMFInt32& mats = shape_->materialIndex;
  int nc_per_idx = 19;		// number of coords per index
  int nn_per_idx = 10;		// number of norms per index
  int nm_per_idx = 3;		// number of mats per index
  coords.setNum(n_geom * nc_per_idx);
  norms.setNum(n_geom * nn_per_idx);
  mats.setNum(n_geom * nm_per_idx);

  // values of the cubes xy_[0,v]
  //     01_v   11_v   
  //   01_0   11_0     
  //     00_v   10_v    
  //   00_0   10_0     

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4

  int32_t* coords_dat = coords.startEditing();
  int32_t* norms_dat = norms.startEditing();
  int32_t* mats_dat = mats.startEditing();
  int cidx = 0;
  int nidx = 0;
  int midx = 0;
  if(matrix->dims() <= 2) {
    int nx = geom_x;
    for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	int mat_idx = (pos.y * nx + pos.x);
	int c00_0 = mat_idx * n_per_vtx;
	render_block_idx(c00_0, mat_idx, coords_dat, norms_dat, mats_dat, cidx, nidx,
                         midx);

        if(render_svg) {
          // this has to be here because of the backward ordering
          float xp = svg_off.x + svg_sz.x * ((float)pos.x + cl_spc) * cl_x;
          float yp = svg_off.y + svg_sz.y * ((float)pos.y + cl_spc) * cl_y;
          float xp1 = svg_off.x + svg_sz.x * ((float)pos.x+1 - cl_spc) * cl_x;
          float yp1 = svg_off.y + svg_sz.y * ((float)pos.y+1 - cl_spc) * cl_y;
          yp = 1.0f - yp; yp1 = 1.0f - yp1; // always flip y

          float val, sc_val;
          if(matrix->dims() == 1) { // 1d is arranged vertically, not horizontally!
            if(mat_layout == BOT_ZERO)
              val = matrix->FastElAsFloat(geom_y-1-pos.y);
            else
              val = matrix->FastElAsFloat(pos.y);
          }
          else {
            if(mat_layout == BOT_ZERO)
              val = matrix->FastElAsFloat(pos.x, geom_y-1-pos.y);
            else
              val = matrix->FastElAsFloat(pos.x, pos.y);
          }
          iColor fl;  iColor tx;
          scale->GetColor(val,sc_val,&fl,&tx);
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
  else if(matrix->dims() == 3) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int zmax = matrix->dim(2);
    for(int z=zmax-1; z>=0; z--) {
      for(pos.y=ymax-1; pos.y>=0; pos.y--) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  int mat_idx = matrix->FastElIndex(pos.x, pos.y, z);
	  int c00_0 = mat_idx * n_per_vtx;
	  render_block_idx(c00_0, mat_idx, coords_dat, norms_dat, mats_dat,
                           cidx, nidx, midx);
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
              val = matrix->FastElAsFloat(pos.x, ymax-1-pos.y, zmax-1-z);
            else
              val = matrix->FastElAsFloat(pos.x, pos.y, z);
            iColor fl;  iColor tx;
            scale->GetColor(val,sc_val,&fl,&tx);
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
  else if(matrix->dims() == 4) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int xxmax = matrix->dim(2);
    int yymax = matrix->dim(3);
    taVector2i opos;
    for(opos.y=yymax-1; opos.y>=0; opos.y--) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=ymax-1; pos.y>=0; pos.y--) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    int mat_idx = matrix->FastElIndex(pos.x, pos.y, opos.x, opos.y);
	    int c00_0 = mat_idx * n_per_vtx;
	    render_block_idx(c00_0, mat_idx, coords_dat, norms_dat, mats_dat,
                             cidx, nidx, midx);

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
                val = matrix->FastElAsFloat(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
              else
                val = matrix->FastElAsFloat(pos.x, pos.y, opos.x, opos.y);
              iColor fl;  iColor tx;
              scale->GetColor(val,sc_val,&fl,&tx);
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
  coords.finishEditing();
  norms.finishEditing();
  mats.finishEditing();

  renderValues();		// hand off to next guy..

  if(render_svg) {
    *svg_str << taSvg::GroupEnd();
  }
}

void SoMatrixGrid::render_text(bool build_text, int& t_idx, float xp, float xp1,
			       float yp, float yp1, float zp, float ufontsz)
{
  if(build_text || cell_text_->getNumChildren() <= t_idx) {
    SoSeparator* tsep = new SoSeparator;
    cell_text_->addChild(tsep);
    SoTranslation* tr = new SoTranslation;
    tsep->addChild(tr);
    SoAsciiText* txt = new SoAsciiText();
    txt->justification = SoAsciiText::CENTER;
    tsep->addChild(txt);
  }
  SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
  SoTranslation* tr = (SoTranslation*)tsep->getChild(0);
  float xfp = .5f * (xp + xp1);
  float yfp = .5f * (yp + yp1) - .5f * ufontsz;
  tr->translation.setValue(xfp, yfp, MAX(zp,0.0f) + .01f);
  SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
  txt->string.setValue("0.0");	// placeholder; gets set later
  t_idx++;
}

void SoMatrixGrid::render_block_idx(int c00_0, int mat_idx,
				    int32_t* coords_dat, int32_t* norms_dat,
				    int32_t* mats_dat, int& cidx, int& nidx, int& midx) {
  int c10_0 = c00_0 + 1; int c01_0 = c00_0 + 2; int c11_0 = c00_0 + 3;
  int c00_v = c00_0 + 4; int c10_v = c00_0 + 5; int c01_v = c00_0 + 6;
  int c11_v = c00_0 + 7;

  // back - right
  //     1    3
  //   0    2     
  //     x    5  
  //   x    4   

  coords_dat[cidx++] = (c01_0); // 0
  coords_dat[cidx++] = (c01_v); // 1
  coords_dat[cidx++] = (c11_0); // 2
  coords_dat[cidx++] = (c11_v); // 3
  coords_dat[cidx++] = (c10_0); // 4
  coords_dat[cidx++] = (c10_v); // 5
  coords_dat[cidx++] = (-1); // -1  -- 7 total

  norms_dat[nidx++] = (0); // back
  norms_dat[nidx++] = (0); // back
  norms_dat[nidx++] = (1); // right
  norms_dat[nidx++] = (1); // right -- 4 total

  mats_dat[midx++] = (mat_idx);

  // left - front
  //     1    x 
  //   0    x   
  //     3    5
  //   2    4   

  coords_dat[cidx++] = (c01_0); // 0
  coords_dat[cidx++] = (c01_v); // 1
  coords_dat[cidx++] = (c00_0); // 2
  coords_dat[cidx++] = (c00_v); // 3
  coords_dat[cidx++] = (c10_0); // 4
  coords_dat[cidx++] = (c10_v); // 5
  coords_dat[cidx++] = (-1); // -1 -- 7 total

  norms_dat[nidx++] = (2); // left
  norms_dat[nidx++] = (2); // left
  norms_dat[nidx++] = (3); // front
  norms_dat[nidx++] = (3); // front -- 4 total

  mats_dat[midx++] = (mat_idx);

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4
  // top
  //     0    1
  //   x    x  
  //     2    3
  //   x    x  

  coords_dat[cidx++] = (c01_v); // 0
  coords_dat[cidx++] = (c11_v); // 1
  coords_dat[cidx++] = (c00_v); // 2
  coords_dat[cidx++] = (c10_v); // 3
  coords_dat[cidx++] = (-1); // -1 -- 5 total

  norms_dat[nidx++] = (4); // top
  norms_dat[nidx++] = (4); // top -- 2 total

  mats_dat[midx++] = (mat_idx);

  // total coords = 7 + 7 + 5 = 19
  // total norms = 4 + 4 + 2 = 10
  // total mats = 3
}

void SoMatrixGrid::renderValues() {
  if(!matrix || !scale) return;
  if(!shape_ || !vtx_prop_) return; // something wrong..

  int geom_x, geom_y;
  matrix->geom.Get2DGeomGui(geom_x, geom_y, odd_y, 1);
//SB?  matrix->geom.Get2DGeom(geom_x, geom_y);

  SoMFVec3f& vertex = vtx_prop_->vertex;
  SoMFUInt32& color = vtx_prop_->orderedRGBA;

  SbVec3f* vertex_dat = vertex.startEditing();
  uint32_t* color_dat = color.startEditing();

  String val_str;
  float val;
  float sc_val;
  T3Color col;
  taVector2i pos;
  int v_idx = 0;
  int c_idx = 0;
  int t_idx = 2;		// base color + font

  // these go in normal order; indexes are backwards
  // note: only 2d case right yet..
  if(matrix->dims() == 1) {
    pos.x = 0;
    int ymax = matrix->dim(0);	// assumes odd_y
    for(pos.y=0; pos.y<ymax; pos.y++) {
      if(mat_layout == BOT_ZERO)
	val = matrix->FastElAsFloat(ymax-1-pos.y);
      else
	val = matrix->FastElAsFloat(pos.y);

      iColor fl;  iColor tx;
      scale->GetColor(val,sc_val,&fl,&tx);
      float zp = sc_val * blk_ht;
      v_idx+=4;			// skip the _0 cases
      for(int i=0;i<4;i++)
	vertex_dat[v_idx++][2] = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
      color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
      if(val_text) {
	SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	if(matrix->GetDataValType() == taBase::VT_STRING) {
	  if(mat_layout == BOT_ZERO) {
	    val_str = ((String_Matrix*)matrix.ptr())->
              FastEl1d(ymax-1-pos.y).elidedTo(max_txt_len);
          }
	  else {
	    val_str = ((String_Matrix*)matrix.ptr())->
              FastEl1d(pos.y).elidedTo(max_txt_len);
          }
	}
	else {
	  ValToDispText(val, val_str);
	}
	txt->string.setValue(val_str.chars());
	t_idx++;
      }
    }
  }
  else if(matrix->dims() == 2) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	if(mat_layout == BOT_ZERO)
	  val = matrix->FastElAsFloat(pos.x, geom_y-1-pos.y);
	else
	  val = matrix->FastElAsFloat(pos.x, pos.y);
	iColor fl;  iColor tx;
	scale->GetColor(val,sc_val,&fl,&tx);
	float zp = sc_val * blk_ht;
	v_idx+=4;			// skip the _0 cases
	for(int i=0;i<4;i++)
	  vertex_dat[v_idx++][2] = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
	float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
	color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
	if(val_text) {
	  SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	  SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	  if(matrix->GetDataValType() == taBase::VT_STRING) {
	    if(mat_layout == BOT_ZERO) {
	      val_str = ((String_Matrix*)matrix.ptr())->
                FastEl2d(pos.x, geom_y-1-pos.y).elidedTo(max_txt_len);
            }
	    else {
	      val_str = ((String_Matrix*)matrix.ptr())->
                FastEl2d(pos.x, pos.y).elidedTo(max_txt_len);
            }
	  }
	  else {
	    ValToDispText(val, val_str);
	  }
	  txt->string.setValue(val_str.chars());
	  t_idx++;
	}
      }
    }
  }
  else if(matrix->dims() == 3) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int zmax = matrix->dim(2);
    for(int z=0; z<zmax; z++) {
      for(pos.y=0; pos.y<ymax; pos.y++) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  if(mat_layout == BOT_ZERO)
	    val = matrix->FastElAsFloat(pos.x, ymax-1-pos.y, zmax-1-z);
	  else
	    val = matrix->FastElAsFloat(pos.x, pos.y, z);
	  iColor fl;  iColor tx;
	  scale->GetColor(val,sc_val,&fl,&tx);
	  float zp = sc_val * blk_ht;
	  v_idx+=4;			// skip the _0 cases
	  for(int i=0;i<4;i++)
	    vertex_dat[v_idx++][2] = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
	  float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
	  color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
	  if(val_text) {
	    SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	    SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	    if(matrix->GetDataValType() == taBase::VT_STRING) { // todo: replicate if compiles
	      if(mat_layout == BOT_ZERO) {
		val_str = ((String_Matrix*)matrix.ptr())->
                  FastEl3d(pos.x, ymax-1-pos.y, zmax-1-z).elidedTo(max_txt_len);
              }
	      else {
		val_str = ((String_Matrix*)matrix.ptr())->
                  FastEl3d(pos.x, pos.y, z).elidedTo(max_txt_len);
              }
	    }
	    else {
	      ValToDispText(val, val_str);
	    }
	    txt->string.setValue(val_str.chars());
	    t_idx++;
	  }
	}
      }
    }
  }
  else if(matrix->dims() == 4) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int xxmax = matrix->dim(2);
    int yymax = matrix->dim(3);
    taVector2i opos;
    for(opos.y=0; opos.y<yymax; opos.y++) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=0; pos.y<ymax; pos.y++) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    if(mat_layout == BOT_ZERO)
	      val = matrix->FastElAsFloat(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
	    else
	      val = matrix->FastElAsFloat(pos.x, pos.y, opos.x, opos.y);
	    iColor fl;  iColor tx;
	    scale->GetColor(val,sc_val,&fl,&tx);
	    float zp = sc_val * blk_ht;
	    v_idx+=4;			// skip the _0 cases
	    for(int i=0;i<4;i++)
	      vertex_dat[v_idx++][2] = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
	    float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
	    color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
	    if(val_text) {
	      SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	      SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	      if(matrix->GetDataValType() == taBase::VT_STRING) {
		if(mat_layout == BOT_ZERO) {
		  val_str = ((String_Matrix*)matrix.ptr())->
                    FastEl4d(pos.x, ymax-1-pos.y, opos.x,
                             yymax-1-opos.y).elidedTo(max_txt_len);
                }
		else {
		  val_str = ((String_Matrix*)matrix.ptr())->
                    FastEl4d(pos.x, pos.y, opos.x, opos.y).elidedTo(max_txt_len);
                }
	      }
	      else {
		ValToDispText(val, val_str);
	      }
	      txt->string.setValue(val_str.chars());
	      t_idx++;
	    }
	  }
	}
      }
    }
  }

  vertex.finishEditing();
  color.finishEditing();
}

