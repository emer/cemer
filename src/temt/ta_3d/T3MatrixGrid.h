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

#ifndef T3MatrixGrid_h
#define T3MatrixGrid_h 1

// parent includes:
#include <T3Entity>

// member includes:
#include <taString>
#include <taVector3f>
#include <taMatrix>

// declare all other types mentioned but not required to include:
class ColorScale;
class T3TriangleStrip;

class TA_API T3MatrixGrid : public T3Entity {
  Q_OBJECT
  INHERITED(T3Entity)
public:
  enum MatrixLayout { 	// order of display for matrix cols
    BOT_ZERO, 		// row zero is displayed at bottom of cell (default)
    TOP_ZERO 		// row zero is displayed at top of cell (ex. for images)
  };

  MatrixLayout	mat_layout; 	// #DEF_BOT_ZERO layout of matrix and image cells
  taMatrixRef	matrix;	    	// the matrix to render (required!)
  int           slice_idx;      // if >= 0, then we are actually plotting a slice of matrix, at this index, instead of the full matrix
  bool		odd_y;		// how to deal with odd-dimensional matricies: put extra odd dimension in the y axis (else x)
  ColorScale*	colorscale; 	// The color scale for this display (required!)
  bool		val_text;	// display text representation of values?

  int		max_txt_len;	// #DEF_6 maximum text length (determines font size)
  float		spacing;	// #DEF_0.1 space between cell blocks, as a proportion of max of X, Y cell size
  float		block_height;	// #DEF_0.2 how tall (in Z dimension) to make the blocks (relative to the max of their X or Y size)
  float		trans_max;	// #DEF_0.6 maximum transparency value (for zero scaled values)
  void*		user_data;	// optional user data associated with this guy (e.g., DataCol)

  bool          render_svg;     // if true, render to svg
  String*       svg_str;        // string to render to
  taVector3f    svg_off;        // position offset for svg rendering output
  taVector3f    svg_sz;         // sizes

  float		max_xy;		// max of unit x,y cell size
  float		cl_spc;		// computed cell spacing
  float		cl_x;		// computed cell size
  float		cl_y;		// computed cell size
  float         ufontsz;        // computed font sz
  float		blk_ht;		// computed block height

  T3TriangleStrip*      tris;
  T3Entity*             cell_text;

  QVector3D norm_bk;
  QVector3D norm_rt;
  QVector3D norm_lf;
  QVector3D norm_fr;
  QVector3D norm_tp;
  
  virtual void		setMatrix(taMatrix* mat, bool oddy, int slice = -1);
  virtual void		setColorScale(ColorScale* cs);
  virtual void		setLayout(MatrixLayout layout);
  virtual void		setValText(bool val_txt);
  virtual void		setBlockHeight(float blk_ht);

  virtual void 		render();
  // basic render -- generates static structures and then calls renderValues
  virtual void		renderValues();
  // optimized render update for when only the matrix values have changed (matrix geom MUST not have changed)

  virtual void 		ValToDispText(float val, String& str);
  // get text representation of value


  T3MatrixGrid(Qt3DNode* parent = 0, taMatrix* mat = NULL, int slice = -1,
               bool oddy = true, ColorScale* cs = NULL, MatrixLayout layout = BOT_ZERO,
               bool val_txt = false);
  ~T3MatrixGrid();

  void mouseClicked(Qt3D::Q3DMouseEvent* mouse) override;
  void mouseDoubleClicked(Qt3D::Q3DMouseEvent* mouse) override;
  
protected:
  void renderBlock(const taVector2i& pos);
  void renderText(float xp, float xp1, float yp, float yp1, float zp);
  void renderValue(taMatrix* matptr, float val, int& c_idx, iColor& clr, int& t_idx,
                   bool use_str_val, const String& str_val);
  void renderSvg(taMatrix* matptr, const taVector2i& pos, iColor& clr);
};

#endif // T3MatrixGrid_h
