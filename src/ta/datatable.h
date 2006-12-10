// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#ifndef datatable_h
#define datatable_h

#include "ta_datatable.h"
#include "ta_math.h"
#include "ta_geometry.h"
#include "minmax.h"
#include "colorscale.h" // for DT && DA Viewspecs
#include "ta_TA_type.h"


// forwards this file

class GridColViewSpec;
class GridTableViewSpec; //


/* these are the routines that used to be on DataTable:

  void			AddRowToArray(float_RArray& ar, int row_num) const;
  // add a row of the datatable to given array
  void			AggRowToArray(float_RArray& ar, int row_num, Aggregate& agg) const;
  // aggregate a row of the datatable to given array using parameters in agg
  float			AggRowToVal(int row_num, Aggregate& agg) const;
  // aggregate a row of the datatable to a value using parameters in agg
  void			AddArrayToRow(float_RArray& ar);
  // add contents of array to datatable
  void			AggArrayToRow(const float_RArray& ar, int row_num, Aggregate& agg);
  // aggregate contents of array to datatable at given row
  void			PutArrayToRow(const float_RArray& ar, int row_num);
  // just put array values into given row of data
  void			PutArrayToCol(const float_RArray& ar, int col);
  // just put array values into given column (if subgp >= 0, column is in given subgroup)
*/

/*
  Additional Display Options
    WIDTH=i (i: int) -- sets default column width to i chars
    NARROW -- in addition to base spec, also sets column with to 8 chars
*/

class TA_API GridColViewSpec : public DataColViewSpec {
  // information for display of a data array in a grid display
INHERITED(DataColViewSpec)
public:
  enum DisplayStyle {
    TEXT	= 0x01,	// Draw using text only (default for scalar cells)
    BLOCK	= 0x02,	// Draw using blocks only (default for matrix cells)
    TEXT_AND_BLOCK = 0x03, // Draw using both blocks and text
    IMAGE = 0x04	// Draw an image (assumes col type has BW or Color image data)
#ifndef __MAKETA__
    ,TEXT_MASK = 0x01 // mask to see if TEXT is in use
    ,BLOCK_MASK = 0x02 // mask to see if BLOCK is in use
#endif
  };

  enum MatrixLayout { // order of display for matrix cols
    BOT_ZERO, // row zero is displayed at bottom of cell (ex for patterns)
    TOP_ZERO // row zero is displayed at top of cell (ex. for images)
  };

  DisplayStyle  display_style;	// can display as text and/or block, or image
  int		text_width; // width of the column (or each matrix col) in chars; also the min width in chars
  String	text_format; // #CONDEDIT_ON_display_style:TEXT,TEXT_AND_BLOCK c-style format string (typically for numbers)
  short		num_prec; // #DEF_5 numeric precision (decimals) for floating numbers
  MatrixLayout	mat_layout; // #DEF_BOT_ZERO layout of matrix and image cells
  bool		scale_on; // #CONDEDIT_ON_display_style:BLOCK,TEXT_AND_BLOCK adjust overall colorscale to include this data
  
  float 	col_width; // #READ_ONLY #HIDDEN #NO_SAVE calculated col_width in geoms 
  float		row_height; // #READ_ONLY #HIDDEN #NO_SAVE calculated row height in geoms

  DATAVIEW_PARENT(GridTableViewSpec)
//GridColView*  parent() const;
  
  
  void	Copy_(const GridColViewSpec& cp);
  COPY_FUNS(GridColViewSpec, DataColViewSpec);
  TA_BASEFUNS(GridColViewSpec);
protected:
  void			UpdateAfterEdit_impl();
  override void		UpdateFromDataCol_impl(bool first_time);
  override void		DataColUnlinked(); // called if data is NULL or destroys
  override void 	Render_impl();
private:
  void 	Initialize();
  void	Destroy() {}
};

class TA_API GridTableViewSpec : public DataTableViewSpec {
  // information for display of a datatable in a grid display
INHERITED(DataTableViewSpec)
public:
  enum MatSizeModel { // convenience to set all mat metrics to commensurable values
    CUSTOM_METRICS,		// used to change individual values
    SMALL_BLOCKS,	// for "small" blocks -- use for large matrix cells and images
    MEDIUM_BLOCKS,	// for "medium" blocks -- use for average matrix cells and images
    LARGE_BLOCKS	// for "large" blocks -- use for small matrix cells and images
  };
  
  float		grid_margin_pts; // #DEF_4 #MIN_0 size of margin inside grid cells, in points
  float		grid_line_pts; // #DEF_3 #MIN_0.1 size of grid lines, in points (grid lines can be turned off in the viewer)
  MatSizeModel	mat_size_model; // sets all matrix metrics to convenient values
  float		mat_block_pts;	// #CONDEDIT_ON_mat_size_model:CUSTOM_METRICS #MIN_0.1 matrix block size, in points
  float		mat_border_pts; // #CONDEDIT_ON_mat_size_model:CUSTOM_METRICS size of border around matrix cells, in points
  float		mat_sep_pts; // #CONDEDIT_ON_mat_size_model:CUSTOM_METRICS sep between text and grid, etc
  float		mat_font_scale; // #CONDEDIT_ON_mat_size_model:CUSTOM_METRICS #MIN_0.1 amount to scale font for matrix cells
  float		pixel_pts;	// #CONDEDIT_ON_mat_size_model:CUSTOM_METRICS #MIN_0.1 image pixel size, in points (there is no border)
  
  inline int		colSpecCount() const {return col_specs.size;}
  GridColViewSpec*	colSpec(int idx) const 
    {return (GridColViewSpec*)col_specs.SafeEl(idx);} //
  
  //note: these are in the _qtso file
  float		gridMarginSize() const; 
  float		gridLineSize() const; 
  float		matBlockSize() const;
  float		matBorderSize() const; 
  float		matSepSize() const;
  float		pixelSize() const;
  float		textHeight() const; // text height, based on font
  
  virtual void	GetMinMaxScale(MinMax& mm, bool first=true); // get min and max data range for scaling
  override void		DataDestroying();

  void 	Initialize();
  void	Destroy();
  void	Copy_(const GridTableViewSpec& cp);
  COPY_FUNS(GridTableViewSpec, DataTableViewSpec);
  TA_BASEFUNS(GridTableViewSpec);
protected:
  override void 	UpdateAfterEdit_impl();
  override void		DataDataChanged_impl(int dcr, void* op1, void* op2);
  void			SetMatSizeModel_impl(MatSizeModel mm);
};

#endif // datatable_h
