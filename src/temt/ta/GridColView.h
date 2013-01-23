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

#ifndef GridColView_h
#define GridColView_h 1

// parent includes:
#include <DataColView>

// member includes:
#include <taMisc>

// declare all other types mentioned but not required to include:
class GridTableView; // 
TypeDef_Of(GridTableView);
class TypeDef; // 
class T3GridColViewNode; //

/*
  Additional Display Options
    WIDTH=i (i: int) -- sets default column width to i chars
    TOP_ZERO  -- override BOT_ZERO default
    IMAGE     -- display as IMAGE (only for matrix columns)

  For the overall Table:
    N_ROWS = number of view rows
    AUTO_SCALE = set colorscale.auto_scale on
    WIDTH = width of display
    SCALE_MIN = set colorscale.min (float)
    SCALE_MAX = set colorscale.max (float)
    BLOCK_HEIGHT = mat_block_height
*/

TypeDef_Of(GridColView);

class TA_API GridColView : public DataColView {
  // information for display of a data column in a grid display.  scalar columns are always displayed as text, and matrix as blocks (with optional value text, controlled by overall table spec)
INHERITED(DataColView)
friend class GridTableView;
public:
  int           text_width;     // width of the column (or each matrix col) in chars; also the min width in chars
  bool          scale_on;       // adjust overall colorscale to include this data (if it is a matrix type)
  taMisc::MatrixView    mat_layout;     // #DEF_BOT_ZERO layout of matrix and image cells
  bool          mat_image;      // display matrix as an image instead of grid blocks
  bool          mat_odd_vert;   // how to arrange odd-dimensional matrix values (e.g., 1d or 3d) -- put the odd dimension in the Y (vertical) axis (else X, horizontal)

  float         col_width; // #READ_ONLY #HIDDEN #NO_SAVE calculated col_width in chars
  float         row_height; // #READ_ONLY #HIDDEN #NO_SAVE calculated row height in chars


  virtual void          SetTextWidth(int text_wdth=16);
  // #BUTTON #VIEWMENU #INIT_ARGVAL_ON_text_width set the text width for this column (default is 16) -- can adjust to fit more items in the display or allow existing text to fit better

  virtual void          ComputeColSizes();
  // compute the column sizes

  virtual void          InitFromUserData();

  override bool         hasViewProperties() const { return true; }
  override String       GetDisplayName() const;

  void                  CopyFromView(GridColView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

  DATAVIEW_PARENT(GridTableView)
  void  Copy_(const GridColView& cp);
  TA_BASEFUNS(GridColView);
protected:
  void                  UpdateAfterEdit_impl();
  override void         UpdateFromDataCol_impl(bool first_time);
  override void         DataColUnlinked(); // called if data is NULL or destroys

  T3GridColViewNode*    MakeGridColViewNode(); // non-standard api/semantics -- makes/sets the node; only called by the GridTableView


private:
  void  Initialize();
  void  Destroy();
};

#endif // GridColView_h
