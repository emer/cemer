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

#include "So3DHeightField.h"
#include <taMatrix>
#include <taVector3i>
#include <taVector3f>
#include <MinMaxRange>
#include <T3Color>
#include <ColorScale>

#include <taMisc>


#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>

SO_NODE_SOURCE(So3DHeightField);

void So3DHeightField::initClass()
{
  SO_NODE_INIT_CLASS(So3DHeightField, SoSeparator, "SoSeparator");
}

So3DHeightField::So3DHeightField(taMatrix* mat, taMatrix* vec_mat_, ColorScale* sc,
				 MinMaxRange* vec_r, bool vec_nrm_,
				 float pvht, float min_a, float max_a,
				 float vln, float vwdth,
				 bool grid_, bool txt_) {
  SO_NODE_CONSTRUCTOR(So3DHeightField);

  transform_ = new SoTransform;
  addChild(transform_);
  shape_ = new SoIndexedTriangleStripSet;
  addChild(shape_);
  vtx_prop_ = new SoVertexProperty;
  shape_->vertexProperty.setValue(vtx_prop_);
  cell_text_ = NULL;
  cell_fnt_ = NULL;

  grid_sep_ = NULL;
  grid_style_ = NULL;
  grid_mat_ = NULL;
  grid_lines_ = NULL;

  vec_sep_ = NULL;
  vec_style_ = NULL;
  vec_vtx_ = NULL;
  vec_lines_ = NULL;

  vec_norms = vec_nrm_;
  matrix = mat;
  taBase::Ref(matrix);
  vec_matrix = NULL;
  vec_range = NULL;
  if(vec_mat_ && vec_r) {
    vec_matrix = vec_mat_;
    vec_range = vec_r;
    taBase::Ref(vec_matrix);
    setup_vec();
  }
  else if(vec_norms) {
    setup_vec();
  }
  scale = sc;
  plane_vht = pvht;
  min_alpha = min_a;
  max_alpha = max_a;
  vec_len = vln;
  vec_width = vwdth;

  grid_width = 1.0f;
  
  val_text = txt_;
  draw_grid = grid_;
  if(draw_grid)
    setup_grid();
  max_txt_len = 6;
  //  render(); // don't do this by default; often want to spec further guys
}

So3DHeightField::~So3DHeightField() {
  taBase::UnRef(matrix);
  if(vec_matrix)
    taBase::UnRef(vec_matrix);
  matrix = NULL;
  vec_matrix = NULL;
}


void So3DHeightField::setup_grid() {
  grid_sep_ = new SoSeparator();
  grid_style_ = new SoDrawStyle();
  grid_mat_ = new SoMaterial();
  grid_lines_ = new SoIndexedLineSet();
  grid_sep_->addChild(grid_style_);
  grid_sep_->addChild(grid_mat_);
  grid_lines_->vertexProperty.setValue(vtx_prop_); // we use existing ones!!
  grid_sep_->addChild(grid_lines_);
  addChild(grid_sep_);
}

void So3DHeightField::setup_vec() {
  vec_sep_ = new SoSeparator();
  vec_style_ = new SoDrawStyle();
  vec_vtx_ = new SoVertexProperty();
  vec_lines_ = new SoIndexedLineSet();
  vec_sep_->addChild(vec_style_);
  vec_lines_->vertexProperty.setValue(vec_vtx_);
  vec_sep_->addChild(vec_lines_);
  addChild(vec_sep_);
}

void So3DHeightField::setMatrix(taMatrix* mat) { 
  taBase::UnRef(matrix);
  matrix = mat; 
  taBase::Ref(matrix);
  render();
}

void So3DHeightField::setVecMatrix(taMatrix* mat, MinMaxRange* vec_r) { 
  if(vec_matrix)
    taBase::UnRef(matrix);
  else {
    setup_vec();
  }
  vec_matrix = mat; 
  vec_range = vec_r;
  if(vec_matrix)
    taBase::Ref(vec_matrix);
  render();
}

void So3DHeightField::setColorScale(ColorScale* cs) { 
  scale = cs;
  render();
}

void So3DHeightField::setValText(bool val_txt) { 
  val_text = val_txt;
  render();
}

void So3DHeightField::setGrid(bool grid) { 
  draw_grid = grid;
  if(draw_grid && !grid_sep_) {
    setup_grid();
  }
  render();
}

void So3DHeightField::setPlaneVHeight(float vh) { 
  plane_vht = vh;
  render();
}


void So3DHeightField::ValToDispText(float val, String& str) {
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

void So3DHeightField::render() {
  if(!matrix || !scale) return;
  if(!shape_ || !vtx_prop_) return; // something wrong..

  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  if(geom_x < 2 || geom_y < 2 || geom_z < 1) return;

  SoMFVec3f& vertex = vtx_prop_->vertex;
  SoMFVec3f& normal = vtx_prop_->normal;
  SoMFUInt32& color = vtx_prop_->orderedRGBA;

  // this means that the color and normals are always 1-to-1 with the vertex coords
  // system automatically does the proper blending
  vtx_prop_->normalBinding.setValue(SoNormalBinding::PER_VERTEX_INDEXED);
  vtx_prop_->materialBinding.setValue(SoMaterialBinding::PER_VERTEX_INDEXED);

  float cl_x = 1.0f / (float)(geom_x-1);
  float cl_y = 1.0f / (float)(geom_y-1);
  float cl_z = 1.0f / (float)(geom_z);

  // float eff_vht = cl_z * plane_vht;
  
  int n_geom = matrix->count();	// not always x * y due to spaces in display geom
  // adding an extra middle dot between each to make symmetric positions..
  int n_mids = (geom_x-1) * (geom_y-1) * geom_z;

  vertex.setNum(n_geom + n_mids);
  color.setNum(n_geom + n_mids);
  normal.setNum(n_geom + n_mids);

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
  taVector3i pos;
  int v_idx = 0;
  int t_idx = 2;		// base color + font
  // these go in normal order; indexes are backwards

  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float xp = (float)pos.x * cl_x;
	float yp = ((float)pos.y * cl_y) - 1.0f;
	float zp = (float)pos.z * cl_z;

	vertex_dat[v_idx++].setValue(xp, zp, yp); // z and y switched!!

	if(val_text) {
	  render_text(build_text, t_idx, xp, yp, zp, ufontsz);
	}
      }
    }
  }

  // now do the extra middle dot guys
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y-1; pos.y++) {
      for(pos.x=0; pos.x<geom_x-1; pos.x++) {
	float xp = ((float)pos.x +.5f) * cl_x;
	float yp = (((float)pos.y + .5f) * cl_y) - 1.0f;
	float zp = (float)pos.z * cl_z;

	vertex_dat[v_idx++].setValue(xp, zp, yp); // z and y switched!!
      }
    }
  }
  
  vertex.finishEditing();

  SoMFInt32& coords = shape_->coordIndex;
  // these guys all just derive from coords per vertex indexed
//   SoMFInt32& norms = shape_->normalIndex;
//   SoMFInt32& mats = shape_->materialIndex;
  int nc_per_idx = 10;		// number of coords per index
  coords.setNum(n_mids * nc_per_idx);

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4

  int32_t* coords_dat = coords.startEditing();
  int cidx = 0;
  int nxy = geom_x * geom_y;
  int nxy_mid = (geom_x -1) * (geom_y-1);
  
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=geom_y-2; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x-1; pos.x++) { // left to right
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	int idx_01 = idx_00 + geom_x;
	// middle guy:
	int idx_mid = n_geom + pos.z * nxy_mid + pos.y * (geom_x-1) + pos.x;

	// all around the back
	coords_dat[cidx++] = idx_00; // 1
	coords_dat[cidx++] = idx_01; // 2
	coords_dat[cidx++] = idx_mid; // 3
	coords_dat[cidx++] = idx_01 + 1; // 4
	coords_dat[cidx++] = idx_00 + 1; // 5
	coords_dat[cidx++] = -1; // 6
	// front face:
	coords_dat[cidx++] = idx_00; // 7 
	coords_dat[cidx++] = idx_00 + 1; // 8
	coords_dat[cidx++] = idx_mid; // 9
	coords_dat[cidx++] = -1; // 10
      }
    }
  }
  coords.finishEditing();

  renderGrid();
  renderVector();

  renderValues();		// hand off to next guy..
}

void So3DHeightField::render_text(bool build_text, int& t_idx, float xp, float yp,
				  float zp, float ufontsz)
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
  float yfp = yp - .5f * ufontsz;
  tr->translation.setValue(xp, MAX(zp,0.0f) + .01f, yfp); // y/z switched
  SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
  txt->string.setValue("0.0");	// placeholder; gets set later
  t_idx++;
}

void So3DHeightField::renderValues() {
  if(!matrix || !scale) return;
  if(!shape_ || !vtx_prop_) return; // something wrong..

  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  int nxy = geom_x * geom_y;

  float cl_x = 1.0f / (float)(geom_x-1);
  float cl_y = 1.0f / (float)(geom_y-1);
  float cl_z = 1.0f / (float)(geom_z);

  float max_xy = MAX(cl_x, cl_y);

  float eff_vht = cl_z * plane_vht;

  SoMFVec3f& vertex = vtx_prop_->vertex;
  SoMFUInt32& color = vtx_prop_->orderedRGBA;
  SoMFVec3f& normal = vtx_prop_->normal;

  SbVec3f* vertex_dat = vertex.startEditing();
  uint32_t* color_dat = color.startEditing();
  SbVec3f* normal_dat = normal.startEditing();

  String val_str;
  float sc_val;
  T3Color col;
  taVector3i pos;
  int v_idx = 0;
  int c_idx = 0;
  int t_idx = 2;		// base color + font

  float max_trans = 1.0f - min_alpha;

  // these go in normal order; indexes are backwards
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float val = matrix->FastElAsFloat(pos.x, pos.y, pos.z);

	iColor fl;  iColor tx;
	scale->GetColor(val,sc_val,&fl,&tx);
	float zp = sc_val * eff_vht + (float)pos.z * cl_z;
	vertex_dat[v_idx++][1] = zp;

	// sc_val = 1..0.  1- = 0..1  alpha 1 = max opaque
	// old eq:
	// a = mscd.min_a + mscd.max_a * mscd.min_a_c * fabs(val - .5);

	float alpha = max_alpha * (1.0f - ((1.0f - fabsf(sc_val)) * max_trans));
	color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
	if(val_text) {
	  SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	  SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	  ValToDispText(val, val_str);
	  txt->string.setValue(val_str.chars());
	  SoTranslation* tr = (SoTranslation*)tsep->getChild(0);
	  SbVec3f val = tr->translation.getValue();
	  val[1] = MAX(zp,0.0f) + .01f;
	  tr->translation.setValue(val); // y/z switched
	  t_idx++;
	}
      }
    }
  }

  // now do the extra middle dot guys
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y-1; pos.y++) {
      for(pos.x=0; pos.x<geom_x-1; pos.x++) {
	float val_00 = matrix->FastElAsFloat(pos.x, pos.y, pos.z);
	float val_10 = matrix->FastElAsFloat(pos.x+1, pos.y, pos.z);
	float val_01 = matrix->FastElAsFloat(pos.x, pos.y+1, pos.z);
	float val_11 = matrix->FastElAsFloat(pos.x+1, pos.y+1, pos.z);

	float val = .25f * (val_00 + val_10 + val_01 + val_11);

	iColor fl;  iColor tx;
	scale->GetColor(val,sc_val,&fl,&tx);
	float zp = sc_val * eff_vht + (float)pos.z * cl_z;
	vertex_dat[v_idx++][1] = zp;
	float alpha = max_alpha * (1.0f - ((1.0f - fabsf(sc_val)) * max_trans));
	color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
      }
    }
  }

  // normals
  int n_idx = 0;
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float my_zp = vertex_dat[n_idx][1];
	float xm_zp, xp_zp;
	if(pos.x > 0)	  xm_zp = vertex_dat[n_idx-1][1];
	else		  xm_zp = my_zp;
	if(pos.x < geom_x-1) xp_zp = vertex_dat[n_idx+1][1];
	else		     xp_zp = my_zp;

	float ym_zp, yp_zp;
	if(pos.y > 0)	  ym_zp = vertex_dat[n_idx-geom_x][1];
	else		  ym_zp = my_zp;
	if(pos.y < geom_y-1) yp_zp = vertex_dat[n_idx+geom_x][1];
	else		     yp_zp = my_zp;

        float dx = xm_zp - xp_zp;
        float dy = ym_zp - yp_zp;
	
	taVector3f n;
	n.x = dx;	n.y = dy;	n.z = max_xy;
	n.MagNorm();
	normal_dat[n_idx++].setValue(n.x, n.z, n.y); // yz switched
      }
    }
  }

  // now do the extra middle dot guys
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y-1; pos.y++) {
      for(pos.x=0; pos.x<geom_x-1; pos.x++) {
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	int idx_01 = idx_00 + geom_x;

	SbVec3f n_00 = normal_dat[idx_00];
	SbVec3f n_01 = normal_dat[idx_01];
	SbVec3f n_10 = normal_dat[idx_00+1];
	SbVec3f n_11 = normal_dat[idx_01+1];

	SbVec3f n = n_00 + n_01 + n_10 + n_11;
	n *= .25f;
	normal_dat[n_idx++].setValue(n.getValue());
      }
    }
  }

  vertex.finishEditing();
  color.finishEditing();
  normal.finishEditing();

  renderVectorValues();
}

void So3DHeightField::renderGrid() {
  if(!matrix || !scale) return;
  if(!grid_sep_) return;

  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  if(geom_x < 2 || geom_y < 2 || geom_z < 1) return;

 SoMFInt32& coords = grid_lines_->coordIndex;

  if(!draw_grid) {
    coords.setNum(0);
    return;
  }

  grid_style_->style = SoDrawStyleElement::LINES;
  grid_style_->lineWidth = grid_width;
  grid_mat_->diffuseColor.setValue(0.0f, 0.0f, 0.0f); // black

  int n_coord = (geom_x + 1) * geom_y * geom_z + (geom_y + 1) * geom_x * geom_z;
  coords.setNum(n_coord);

  int32_t* coords_dat = coords.startEditing();
  int cidx = 0;

  int nxy = geom_x * geom_y;
  taVector3i pos;
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    // X guys
    for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x; pos.x++) { // left to right
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	coords_dat[cidx++] = idx_00;
      }
      coords_dat[cidx++] = -1;	// end that line
    }
    // Y guys
    for(pos.x=0; pos.x<geom_x; pos.x++) { // left to right
      for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	coords_dat[cidx++] = idx_00;
      }
      coords_dat[cidx++] = -1;	// end that line
    }
  }

  coords.finishEditing();
}

void So3DHeightField::renderVector() {
  if(!matrix || !scale) return;
  if(!vec_sep_) return;

  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  if(geom_x < 2 || geom_y < 2 || geom_z < 1) return;

  float cl_x = 1.0f / (float)(geom_x-1);
  float cl_y = 1.0f / (float)(geom_y-1);
  float cl_z = 1.0f / (float)(geom_z);

  SoMFVec3f& vertex = vec_vtx_->vertex;
  SoMFUInt32& color = vec_vtx_->orderedRGBA;
  SoMFInt32& coords = vec_lines_->coordIndex;

  vec_vtx_->materialBinding.setValue(SoMaterialBinding::PER_VERTEX_INDEXED);

  if(!vec_norms && (!vec_matrix || !vec_range)) {
    vertex.setNum(0);
    color.setNum(0);
    coords.setNum(0);
    return;
  }

  vec_style_->style = SoDrawStyleElement::LINES;
  vec_style_->lineWidth = vec_width;

  int n_geom = matrix->count();	// not always x * y due to spaces in display geom
  int n_vtx = n_geom*2;		// 2 points per point
  int n_coord = n_geom*3;	// 3 coords per point (-1 term)
  vertex.setNum(n_vtx);
  color.setNum(n_vtx);
  coords.setNum(n_coord);

  SbVec3f* vertex_dat = vertex.startEditing();

  taVector3i pos;
  int v_idx = 0;
  // just standard points
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float xp = (float)pos.x * cl_x;
	float yp = ((float)pos.y * cl_y) - 1.0f;
	float zp = (float)pos.z * cl_z;
	vertex_dat[v_idx++].setValue(xp, zp, yp); // z and y switched!!
      }
    }
  }
  // the rest are all end points and are rendered in values!

  vertex.finishEditing();

  int32_t* coords_dat = coords.startEditing();

  int cidx = 0;
  int nxy = geom_x * geom_y;
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x; pos.x++) { // left to right
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	coords_dat[cidx++] = idx_00;
	coords_dat[cidx++] = n_geom + idx_00; // 2nd point in end-space
	coords_dat[cidx++] = -1;	// end that line
      }
    }
  }

  coords.finishEditing();
}

inline static float so3dhf_get_sc_val(float max, float val) {
  float nrm_val = 0.0f;
  if(max > 0.0f)
    nrm_val = val / max;
  return nrm_val;
}

void So3DHeightField::renderVectorValues() {
  if(!matrix || !scale) return;
  if(!vec_sep_) return;

  if(!vec_norms && (!vec_matrix || !vec_range)) {
    return;
  }
  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  if(geom_x < 2 || geom_y < 2 || geom_z < 1) return;

  float cl_x = 1.0f / (float)(geom_x-1);
  float cl_y = 1.0f / (float)(geom_y-1);
  float cl_z = 1.0f / (float)(geom_z);

  taVector3f vec_ln;
  vec_ln.x = cl_x * vec_len;
  vec_ln.y = cl_y * vec_len;
  vec_ln.z = cl_z * vec_len;

  vec_ln.z = MIN(vec_ln.x, vec_ln.z); // not too big..
  float eff_vht = cl_z * plane_vht;

  int n_geom = matrix->count();	// not always x * y due to spaces in display geom

  SoMFVec3f& vertex = vec_vtx_->vertex;
  SoMFUInt32& color = vec_vtx_->orderedRGBA;
  SoMFVec3f& normal = vtx_prop_->normal;

  SbVec3f* vertex_dat = vertex.startEditing();
  uint32_t* color_dat = color.startEditing();
  
  SbVec3f* normal_dat = NULL;
  if(vec_norms)
    normal_dat = normal.startEditing();

  T3Color col;
  taVector3i pos;
  int v_idx = 0;
  int c_idx = 0;
  float sc_val;
  taVector3f vec;
  taVector3f sp;
  taVector3f ep;

//   float max_trans = 1.0f - min_alpha;

  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	sp.x = (float)pos.x * cl_x;
	sp.y = ((float)pos.y * cl_y) - 1.0f;
	sp.z = (float)pos.z * cl_z;

	float val = matrix->FastElAsFloat(pos.x, pos.y, pos.z);
	if(vec_norms) {
	  vec.x = so3dhf_get_sc_val(1.0f, normal_dat[v_idx][0]);
	  vec.y = so3dhf_get_sc_val(1.0f, normal_dat[v_idx][2]);
	  vec.z = so3dhf_get_sc_val(1.0f, normal_dat[v_idx][1]);
	}
	else {
	  vec.x = so3dhf_get_sc_val(vec_range->max, vec_matrix->FastElAsFloat(0, pos.x, pos.y, pos.z));
	  vec.y = so3dhf_get_sc_val(vec_range->max, vec_matrix->FastElAsFloat(1, pos.x, pos.y, pos.z));
	  vec.z = so3dhf_get_sc_val(vec_range->max, vec_matrix->FastElAsFloat(2, pos.x, pos.y, pos.z));
	}

	iColor fl;  iColor tx;
	scale->GetColor(val,sc_val,&fl,&tx);
	sp.z += sc_val * eff_vht;
	vertex_dat[v_idx][1] = sp.z;

	ep = sp + vec_ln * vec;
	vertex_dat[n_geom + v_idx].setValue(ep.x, ep.z, ep.y); // xy switch
	v_idx++;

// 	float alpha = max_alpha * (1.0f - ((1.0f - fabsf(sc_val)) * max_trans));
// 	color_dat[c_idx] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);

	float vec_mag = vec.Mag();
	if(vec_mag > 1.0f) vec_mag = 1.0f;
	if(vec_mag < 0.0f) vec_mag = 0.0f;
	if(vec.Sum() < 0.0f)
	  vec_mag *= -1.0f;
	int clr_idx = (int)((.5f + .5f * vec_mag) * (float)(scale->chunks-1.0f));
	fl = scale->GetColor(clr_idx);

// 	alpha = max_alpha * (1.0f - ((1.0f - fabsf(vec_mag)) * max_trans));
	// just keeping the same color, no transparency..
	float alpha = 1.0f;
	uint32_t clr = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
 	color_dat[c_idx] = clr;
	color_dat[n_geom + c_idx] = clr;
	
	c_idx++;
      }
    }
  }

  vertex.finishEditing();
  color.finishEditing();
  if(normal_dat)
    normal.finishEditing();
}

