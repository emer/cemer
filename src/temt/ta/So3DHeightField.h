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

#ifndef So3DHeightField_h
#define So3DHeightField_h 1

// parent includes:
#include "ta_def.h"
#include <Inventor/nodes/SoSeparator.h>

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class taMatrix;
class ColorScale;
class MinMaxRange;
class SoFont;
class SoDrawStyle;
class SoMaterial;
class SoIndexedLineSet;
class SoTransform;
class SoVertexProperty;
class SoIndexedTriangleStripSet;


class TA_API So3DHeightField: public SoSeparator {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS renders a matrix as a stack of 3d height-field layers -- matrix must be 3d -- X-Y is displayed as the height-field planes and Z is the stacking dimension -- actual rendering reverses Y and Z coordinates relative to inventor default (Z is vertical, Y is depth)
#ifndef __MAKETA__
typedef SoSeparator inherited;

  SO_NODE_HEADER(So3DHeightField);
#endif // def __MAKETA__
public:
  static void		initClass();

  taMatrix*	matrix;		// the 3d matrix data to display (required -- can be 0's to just display vec_matrix)
  taMatrix*	vec_matrix;	// an additional vector field (optional)
  ColorScale*	scale; 		// The color scale for this display (required!)
  MinMaxRange*	vec_range;	// range for vector data (abs max for value of separate components)
  float		plane_vht;	// #DEF_0.5 vertical height to use for height field (as proportion of layer vert height)
  float		min_alpha;	// #DEF_0.5 minimum alpha value (most transparent)
  float		max_alpha;	// #DEF_1 maximum alpha value (least transparent)
  float		vec_len;	// #DEF_0.5 maximum length of vector lines, as a fraction of teh relevant max cell value (x,y,z)
  bool		draw_grid;	// display the XY grid as black lines
  bool		vec_norms;	// display norm vectors instead of vec_matrix
  bool		val_text;	// display text representation of values?
  int		max_txt_len;	// #DEF_6 maximum text length (determines font size)
  float		grid_width;	// width of grid lines
  float		vec_width;	// width of vector lines

  void 		render();
  // basic render -- generates static structures and then calls renderValues
  void		renderValues();
  // optimized render update for when only the matrix values have changed (matrix geom MUST not have changed)
  void		renderGrid();

  void		renderVector();
  void		renderVectorValues(); // called by renderValues

  void		setMatrix(taMatrix* mat);
  void		setVecMatrix(taMatrix* mat, MinMaxRange* vec_r);
  void		setColorScale(ColorScale* cs);
  void		setValText(bool val_txt);
  void		setPlaneVHeight(float vht);
  void		setGrid(bool grid);

  void 		ValToDispText(float val, String& str);
  // get text representation of value

  SoTransform*	transform() const { return transform_; }
  // the master transform, for the whole entity

  So3DHeightField(taMatrix* mat_ = NULL, taMatrix* vec_mat_ = NULL, ColorScale* cs = NULL, 
		  MinMaxRange* vec_r = NULL, bool vec_nrm_ = false,
		  float pvht = 0.5f, float min_a = 0.5f, float max_a = 1.0f, 
		  float vec_ln = 2.0f, float vec_wdth = 1.0f, bool grid_ = false, bool txt_ = false);
  ~So3DHeightField();

protected:
  const char*  	getFileFormatName() const {return "Separator"; } 

  SoIndexedTriangleStripSet* shape_;
  SoTransform*		transform_; // for entire object: goes first in this
  SoVertexProperty*	vtx_prop_;
  SoSeparator* 		cell_text_; // cell text variables
  SoFont* 		cell_fnt_; //fnt we add to the text (not refed)

  SoSeparator* 		grid_sep_;  // optional grid lines
  SoDrawStyle* 		grid_style_;
  SoMaterial* 		grid_mat_; 
  SoIndexedLineSet* 	grid_lines_;

  SoSeparator* 		vec_sep_;  // optional vector lines
  SoDrawStyle* 		vec_style_;
  SoVertexProperty*	vec_vtx_;
  SoIndexedLineSet* 	vec_lines_;

  void		render_text(bool build_text, int& t_idx, float xp, float yp,
			    float zp, float ufontsz);
  // setup all the cell_text_ stuff

  void		setup_grid();	// init all the grid stuff
  void		setup_vec();	// init all the vec stuff
};

#endif // So3DHeightField_h
