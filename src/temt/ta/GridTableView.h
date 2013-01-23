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

#ifndef GridTableView_h
#define GridTableView_h 1

// parent includes:
#include <DataTableView>

// member includes:
#include <int_Array>
#include <float_Array>
#include <MinMaxInt>
#include <ColorScale>
#include <MinMax>

// declare all other types mentioned but not required to include:
class DataTable; // 
class T3DataViewFrame; // 
class T3GridViewNode; //
class GridColView; // 
class ColorScaleSpec; // 
class iGridTableView_Panel; // 


TypeDef_Of(GridTableView);

class TA_API GridTableView: public DataTableView {
  // the master view guy for entire grid view
INHERITED(DataTableView)
public:
  static GridTableView* New(DataTable* dt, T3DataViewFrame*& fr);

  int           col_n;          // number of columns to display: determines sizes of everything automatically from this
  int_Array     vis_cols;       // #READ_ONLY #NO_SAVE indicies of visible columns
  MinMaxInt     col_range;      // column range to display, in terms of the visible columns (contained in vis_cols index list)

  float         width;          // how wide to make the display (height is always 1.0)
  bool          grid_on;        // #DEF_true whether to show grid lines
  bool          header_on;      // #DEF_true is the table header visible?
  bool          row_num_on;     // #DEF_false row number col visible?
  bool          two_d_font;     // #DEF_false use 2d font (easier to read, but doesn't scale) instead of 3d font
  float         two_d_font_scale; // #DEF_350 how to scale the two_d font relative to the computed 3d number
  bool          mat_val_text;   // also display text values for matrix blocks

  ColorScale    colorscale;     // contains current min,max,range,zero,auto_scale

  float         grid_margin;    // #DEF_0.01 #MIN_0 size of margin between grid cells (in normalized units)
  float         grid_line_size; // #DEF_0.005 #MIN_0 size of grid lines (in normalized units)
  int           row_num_width;  // #DEF_4 width of row number column
  float         mat_block_spc;  // #DEF_0.1 space between matrix cell blocks, as a proportion of max of X, Y cell size
  float         mat_block_height; // #DEF_0.2 how tall (in Z dimension) to make the blocks (relative to the max of their X or Y size)
  float         mat_rot;          // #DEF_0 rotation of the matrix in the Z plane (in degrees) - allows for vertical stacks of grids to be displayed in depth
  float         mat_trans;        // #DEF_0.6 maximum transparency of zero values in matrix blocks -- set to 0 to make all blocks opaque

  MinMaxInt     mat_size_range; // range of display sizes for matrix items relative to other text items.  each cell in a matrix counts as one character in size, within these ranges (smaller matricies are made larger to min size, and large ones are made smaller to max size)
  MinMax        text_size_range; // (default .02 - .05) minimum and maximum text size -- keeps things readable and not too big

  bool          click_vals;     // If on, then clicking on cell values in the grid view display in interact mode (red arrow) will change the values
  float         lmb_val;        // left mouse button value: Value that will be set in the cell if you click with the left mouse button (if Click Vals is on)
  float         mmb_val;        // middle mouse button value: Value that will be set in the cell if you click with the middle mouse button (if Click Vals is on)

  String        last_sel_col_nm; // #READ_ONLY #SHOW #NO_SAVE column name of the last selected point in graph to view values (if empty, then none)
  bool          last_sel_got;    // #READ_ONLY #SHOW #NO_SAVE got a last sel value
  float         last_sel_val;   // #READ_ONLY #SHOW #NO_SAVE value of last selected point
  int           last_sel_dims;  // #READ_ONLY #SHOW #NO_SAVE dims of last sel coords
  int           last_sel_coords[7];// #READ_ONLY #SHOW #NO_SAVE dim coords

  bool          scrolling_;     // #IGNORE currently scrolling (in scroll callback)

  GridColView*          colVis(int i) const
  { return (GridColView*)colView(vis_cols.SafeEl(i)); }
  // get visible column based on vis_cols index

  override void InitDisplay(bool init_panel = true);
  override void UpdateDisplay(bool update_panel = true);
  // note: we also don't update panel if it is updating

  void          ShowAllCols();
  // #BUTTON #CONFIRM #VIEWMENU show all columns in the data table (turn their visible flags on) -- this is the only way to turn visible back on once it is turned off!
  void          SetColorSpec(ColorScaleSpec* color_spec);
  // #BUTTON #VIEWMENU #INIT_ARGVAL_ON_colorscale.spec set the color scale spec to determine the palette of colors representing values

  // viewpanel accessors for complex members, we don't update though
  void          setWidth(float wdth);
  void          setScaleData(bool auto_scale, float scale_min, float scale_max);
  // updates the values in us and the stored ones in the colorscale list

  // view control
  void          VScroll(bool left); // scroll left or right
  virtual void  ViewCol_At(int start);  // start viewing at indicated column value

  virtual void          ColBackAll();
  virtual void          ColBackPg();
  virtual void          ColBack1();
  virtual void          ColFwd1();
  virtual void          ColFwdPg();
  virtual void          ColFwdAll();

  iGridTableView_Panel* lvp();
  inline T3GridViewNode* node_so() const {return (T3GridViewNode*)inherited::node_so();}

  virtual void          InitFromUserData();

  override void         UpdateName();
  override const String caption() const;
  override bool         hasViewProperties() const { return true; }

  const iColor          bgColor(bool& ok) const {
    ok = true; return colorscale.background;
  } // #IGNORE


  void                  CopyFromView(GridTableView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

  void  InitLinks();
  void  CutLinks();
  void  Initialize();
  void  Destroy() {CutLinks();}
  void  Copy_(const GridTableView& cp);
  T3_DATAVIEWFUNS(GridTableView, DataTableView)

protected:
  float_Array           col_widths_raw; // raw widths of columns (original request)
  float_Array           col_widths;     // scaled widths of columns (to unitary size)
  float                 row_height_raw; // raw row height
  float                 row_height;     // unitary scaled row height
  float                 head_height;    // renderable portion of header (no margins etc.)
  float                 font_scale;     // scale to set global font to

  virtual void          CalcViewMetrics(); // for entire view
  virtual void          GetScaleRange();   // get the current scale range based on auto scaled columns (only if auto_scale is on)
  virtual void          SetScrollBars();   // set scroll bar values

  virtual void          RemoveGrid();
  virtual void          RemoveHeader(); // remove the header
  virtual void          RemoveLines(); // remove all lines

  virtual void          RenderGrid();
  virtual void          RenderHeader();
  virtual void          RenderLines(); // render all the view_range lines
  virtual void          RenderLine(int view_idx, int data_row); // add indicated line

  // view control:
  override void         ClearViewRange();
  override void         MakeViewRangeValid();

  override void         OnWindowBind_impl(iT3DataViewFrame* vw);
  override void         Clear_impl();
  override void         Render_pre(); // #IGNORE
  override void         Render_impl(); // #IGNORE

  override void         UpdateFromDataTable_this(bool first);

  override void         UpdateAfterEdit_impl();
};

#endif // GridTableView_h
