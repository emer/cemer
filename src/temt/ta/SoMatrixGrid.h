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

#ifndef SoMatrixGrid_h
#define SoMatrixGrid_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <Inventor/nodes/SoSeparator.h>
#else
class SoSeparator; //
#endif

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class taMatrix;
class ColorScale;
class SoVertexProperty;
class SoFont;
class SoIndexedTriangleStripSet;
class SoTransform;


TypeDef_Of(SoMatrixGrid);

class TA_API SoMatrixGrid: public SoSeparator {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS renders a matrix as a grid of 3d blocks, in X-Y plane, with block height = Z axis.  size = 1x1 unit
INHERITED(SoSeparator)
#ifndef __MAKETA__
  SO_NODE_HEADER(SoMatrixGrid);
#endif // def __MAKETA__
public:
  enum MatrixLayout { 	// order of display for matrix cols
    BOT_ZERO, 		// row zero is displayed at bottom of cell (default)
    TOP_ZERO 		// row zero is displayed at top of cell (ex. for images)
  };

  static void		initClass();

  MatrixLayout	mat_layout; 	// #DEF_BOT_ZERO layout of matrix and image cells
  taMatrix*	matrix;	    	// the matrix to render (required!)
  bool		odd_y;		// how to deal with odd-dimensional matricies: put extra odd dimension in the y axis (else x)
  ColorScale*	scale; 		// The color scale for this display (required!)
  bool		val_text;	// display text representation of values?

  int		max_txt_len;	// #DEF_6 maximum text length (determines font size)
  float		spacing;	// #DEF_0.1 space between cell blocks, as a proportion of max of X, Y cell size
  float		block_height;	// #DEF_0.2 how tall (in Z dimension) to make the blocks (relative to the max of their X or Y size)
  float		trans_max;	// #DEF_0.6 maximum transparency value (for zero scaled values)
  void*		user_data;	// optional user data associated with this guy (e.g., DataCol)

  void		setMatrix(taMatrix* mat, bool oddy);
  void		setColorScale(ColorScale* cs);
  void		setLayout(MatrixLayout layout);
  void		setValText(bool val_txt);
  void		setBlockHeight(float blk_ht);

  void 		render();
  // basic render -- generates static structures and then calls renderValues
  void		renderValues();
  // optimized render update for when only the matrix values have changed (matrix geom MUST not have changed)

  void 		ValToDispText(float val, String& str);
  // get text representation of value

  SoTransform*	transform() const { return transform_; }
  // the master transform, for the whole entity
  USING(inherited::getMatrix)
  taMatrix*	getMatrix() const { return matrix; }

  SoMatrixGrid(taMatrix* mat = NULL, bool oddy = true, ColorScale* cs = NULL, MatrixLayout layout = BOT_ZERO, bool val_txt = false);
  ~SoMatrixGrid();

protected:
  const char*  	getFileFormatName() const {return "Separator"; } 

  SoIndexedTriangleStripSet* shape_;
  SoTransform*		transform_; // for entire object: goes first in this
  SoVertexProperty*	vtx_prop_;
  SoSeparator* 		cell_text_; // cell text variables
  SoFont* 		cell_fnt_; //fnt we add to the text (not refed)

  float		max_xy;		// max of unit x,y cell size
  float		cl_spc;		// computed cell spacing
  float		blk_ht;		// computed block height
  
  void		render_block_idx(int c00_0, int mat_idx, 
				 int32_t* coords_dat, int32_t* norms_dat,
				 int32_t* mats_dat, int& cidx, int& nidx, int& midx);
  // render one set of block indicies
  void		render_text(bool build_text, int& t_idx, float xp, float xp1, float yp,
			    float yp1, float zp, float ufontsz);
  // setup all the cell_text_ stuff
};

#endif // SoMatrixGrid_h
