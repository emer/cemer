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

#ifndef TA_DATATABLE_QTSO_H
#define TA_DATATABLE_QTSO_H

#include "ta_qttype.h"
#include "ta_qtviewer.h"
#include "ta_fontspec.h"

#include "ta_datatable.h"
#include "ta_geometry.h"
#include "colorscale.h"
#include "colorbar_qt.h"
#include "t3viewer.h"
#include "ta_matrix_qt.h"

#ifndef __MAKETA__
# include <QItemDelegate>
# include <QPointer>
# include <QTableView>
#endif

// forwards
class DataColView;
class DataTableView;
class iDataTableView_Panel;

class GridColView;
class GridTableView;
class iGridTableView_Panel;

class GraphColView;
class GraphAxisView;
class GraphTableView;
class iGraphTableView_Panel;

class tabDataTableViewType;
class iDataTableView;
class iDataTablePanel; //

// externals
class T3GridViewNode;
class T3Axis;
class T3GraphLine;
class T3GraphViewNode;
class taiListElsButton;

class TA_API DataColView: public T3DataView {
  // ##SCOPE_DataTableView base specification for the display of data columns
INHERITED(T3DataView)
friend class DataTableView;
public:
  String	name;		// name of column this guy is associated with
  bool		visible;	// is this column visible in display?
  bool		sticky; 	// #DEF_false set this to retain this colspec even if its column deletes

  DataCol*		dataCol() const {return (DataCol*)data();}
  void			setDataCol(DataCol* value, bool first_time = false);
  
  DATAVIEW_PARENT(DataTableView)

  bool			isVisible() const; // bakes in check for datacol

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  override void		DataDestroying();
  
  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const DataColView& cp);
  TA_BASEFUNS(DataColView);
protected:
  override void		Unbind_impl(); // unbinds col
  virtual void		DataColUnlinked() {} // called if data set to NULL or destroys
  void			UpdateFromDataCol(bool first_time = false);
  // called if data set to column, or we otherwise need to update
  virtual void		UpdateFromDataCol_impl(bool first_time); 
  void	Initialize();
  void	Destroy();
};

class TA_API DataTableView : public T3DataViewPar {
  // #VIRT_BASE #NO_TOKENS base class of grid and graph views
INHERITED(T3DataViewPar)
public:
  int		view_rows; 	// maximum number of rows visible
  MinMaxInt	view_range; 	// range of visible rows (max is the last row visible, not the last+1; range = view_rows-1)

  bool		display_on;  	// #DEF_true 'true' if display should be updated
  bool		manip_ctrl_on;	// #DEF_true display the manipulation controls on objects for positioning etc

  FloatTDCoord	table_pos;	// position of table in view
  FloatTDCoord	table_scale;	// scaling factors of table in view
  FloatRotation	table_orient;	// orientation of table in view
  
  virtual const String	caption() const; // what to show in viewer

  DataTable*		dataTable() const {return (DataTable*)data();}
  virtual void		setDataTable(DataTable* dt);
  // #MENU #NO_NULL build the view from the given table

  void			setDisplay(bool value); // use this to change display_on
  override void		setDirty(bool value); // set for all changes on us or below
  inline int		rows() const {return m_rows;}
  bool			isVisible() const; // gui_active, mapped and display_on

  DataColView*		colView(int i) const
  { return (DataColView*)children.SafeEl(i); } 
  inline int		colViewCount() const { return children.size;}

  /////////////////////////////////////////////
  //	Main interface: init/update (impl in subclasses)

  virtual void		InitDisplay(bool init_panel = true) { }; 
  // does a hard reset on the display, reinitializing variables etc.  Note does NOT do Updatedisplay -- that is a separate step
  virtual void		UpdateDisplay(bool update_panel = true) { };
  // full re-render of the display (generally calls Render_impl)

  virtual void		InitPanel();
  // lets panel init itself after struct changes
  virtual void		UpdatePanel();
  // after changes to props

  virtual void 		ClearData();
  // Clear the display and the data
  virtual void 		ViewRow_At(int start);
  // start viewing at indicated viewrange value

  override void		DataDestroying();
  override void		BuildAll();
  
  override String	GetLabel() const;
  override String	GetName() const;

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const DataTableView& cp);
  T3_DATAVIEWFUNS(DataTableView, T3DataViewPar) //

protected:
#ifndef __MAKETA__
  QPointer<iDataTableView_Panel> m_lvp; //note: will be a subclass of this, per the log type
#endif
  int			m_rows; // cached rows, we use to calc deltas etc.
  int			updating; // to prevent recursion

  override void 	UpdateAfterEdit_impl();

  virtual void		ClearViewRange();
  // sets view range back to beginning (grid adds cols, graph adds TBA)
  virtual void 		MakeViewRangeValid();
  // adjust row/col etc. to be valid
  
  virtual int		CheckRowsChanged(int& orig_rows);
  // check if datatable rows is same as last render (updates m_rows and returns any delta, 0 if no change)
  
  override void		Unbind_impl(); // unbinds table

  override void 	DataStructUpdateEnd_impl();
  override void		DataUpdateView_impl();
  override void		DataUpdateAfterEdit_impl();
  override void		DoActionChildren_impl(DataViewAction acts);

  void			UpdateFromDataTable(bool first_time = false);
  // called if data set to table, or needs to be updated; calls _child then _this
  virtual void		UpdateFromDataTable_child(bool first); 
  // does kids, usually not overridden
  virtual void		UpdateFromDataTable_this(bool first);
  // does me (*after* kids, so you can refer to them)
  virtual void		DataTableUnlinked(); // called if data is NULL or destroys

  override void		Render_pre();
  override void		Render_impl();
  override void 	Render_post();
  override void		Reset_impl();
};

////////////////////////////////////////////////////////////////////////////////
// 		Grid View

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

class TA_API GridColView : public DataColView {
  // information for display of a data column in a grid display.  scalar columns are always displayed as text, and matrix as blocks (with optional value text, controlled by overall table spec)
INHERITED(DataColView)
public:
  int		text_width; 	// width of the column (or each matrix col) in chars; also the min width in chars
  bool		scale_on; 	// adjust overall colorscale to include this data (if it is a matrix type)
  taMisc::MatrixView	mat_layout; 	// #DEF_BOT_ZERO layout of matrix and image cells
  bool		mat_image;	// display matrix as an image instead of grid blocks
  bool		mat_odd_vert;	// how to arrange odd-dimensional matrix values (e.g., 1d or 3d) -- put the odd dimension in the Y (vertical) axis (else X, horizontal)
  
  float 	col_width; // #READ_ONLY #HIDDEN #NO_SAVE calculated col_width in chars
  float		row_height; // #READ_ONLY #HIDDEN #NO_SAVE calculated row height in chars

  virtual void		ComputeColSizes();
  // compute the column sizes

  virtual void		InitFromUserData();

  override bool		selectEditMe() const { return true; }
  override String	GetDisplayName() const;

  DATAVIEW_PARENT(GridTableView)
  void	Copy_(const GridColView& cp);
  TA_BASEFUNS(GridColView);
protected:
  void			UpdateAfterEdit_impl();
  override void		UpdateFromDataCol_impl(bool first_time);
  override void		DataColUnlinked(); // called if data is NULL or destroys

private:
  void 	Initialize();
  void	Destroy();
};

class TA_API GridTableView: public DataTableView {
  // the master view guy for entire grid view
INHERITED(DataTableView)
public:
  static GridTableView* New(DataTable* dt, T3DataViewFrame*& fr);

  int		col_n; 		// number of columns to display: determines sizes of everything automatically from this
  int_Array	vis_cols;	// #READ_ONLY #NO_SAVE indicies of visible columns
  MinMaxInt	col_range; 	// column range to display, in terms of the visible columns (contained in vis_cols index list)

  float		width;		// how wide to make the display (height is always 1.0)
  bool		grid_on; 	// #DEF_true whether to show grid lines
  bool		header_on;	// #DEF_true is the table header visible?
  bool		row_num_on; 	// #DEF_true row number col visible?
  bool		two_d_font;	// #DEF_true use 2d font (easier to read, but doesn't scale) instead of 3d font
  float		two_d_font_scale; // #DEF_350 how to scale the two_d font relative to the computed 3d number
  bool		mat_val_text;	// also display text values for matrix blocks

  ColorScale	colorscale; 	// contains current min,max,range,zero,auto_scale

  float		grid_margin; 	// #DEF_0.01 #MIN_0 size of margin between grid cells (in normalized units)
  float		grid_line_size; // #DEF_0.005 #MIN_0 size of grid lines (in normalized units)
  int		row_num_width;	// #DEF_4 width of row number column
  float		mat_block_spc;	// #DEF_0.1 space between matrix cell blocks, as a proportion of max of X, Y cell size
  float		mat_block_height; // #DEF_0.2 how tall (in Z dimension) to make the blocks (relative to the max of their X or Y size)
  float		mat_rot;	  // #DEF_0 rotation of the matrix in the Z plane (in degrees) - allows for vertical stacks of grids to be displayed in depth
  float		mat_trans;	  // #DEF_0.6 maximum transparency of zero values in matrix blocks -- set to 0 to make all blocks opaque

  MinMaxInt	mat_size_range;	// range of display sizes for matrix items relative to other text items.  each cell in a matrix counts as one character in size, within these ranges (smaller matricies are made larger to min size, and large ones are made smaller to max size)
  MinMax	text_size_range; // (default .02 - .05) minimum and maximum text size -- keeps things readable and not too big

  GridColView*		colVis(int i) const
  { return (GridColView*)colView(vis_cols.SafeEl(i)); }
  // get visible column based on vis_cols index

  override void	InitDisplay(bool init_panel = true);
  override void	UpdateDisplay(bool update_panel = true);

  void		SetColorSpec(ColorScaleSpec* color_spec);
  // #BUTTON set the color scale spec to determine the palette of colors representing values

  // view button/field callbacks
  void		setGrid(bool value);
  void		setHeader(bool value);
  void		setRowNum(bool value);
  void		set2dFont(bool value);
  void		set2dFontScale(float value);
  void		setValText(bool value);
  void		setWidth(float wdth);
  void		setRows(int rows);
  void		setCols(int cols);
  void		setMatTrans(float value);
  void		setMatRot(float value);
  void		setMatBlockHeight(float value);
  void		setAutoScale(bool value);
  void		setScaleData(bool auto_scale, float scale_min, float scale_max);
  // updates the values in us and the stored ones in the colorscale list

  // view control
  void		VScroll(bool left); // scroll left or right
  virtual void 	ViewCol_At(int start);	// start viewing at indicated column value
  
  iGridTableView_Panel*	lvp(){return (iGridTableView_Panel*)(iDataTableView_Panel*)m_lvp;}
  T3GridViewNode* node_so() const {return (T3GridViewNode*)m_node_so.ptr();}

  virtual void		InitFromUserData();

  override String	GetLabel() const;
  override String	GetName() const;

  override bool		selectEditMe() const { return true; }
  
  void	InitLinks();
  void 	CutLinks();
  void	Initialize();
  void	Destroy() {CutLinks();}
  void	Copy_(const GridTableView& cp);
  T3_DATAVIEWFUNS(GridTableView, DataTableView)

protected:
  float_Array		col_widths_raw; // raw widths of columns (original request)
  float_Array		col_widths; 	// scaled widths of columns (to unitary size)
  float			row_height_raw; // raw row height
  float			row_height; 	// unitary scaled row height
  float			head_height; 	// renderable portion of header (no margins etc.)
  float			font_scale;	// scale to set global font to

  virtual void		CalcViewMetrics(); // for entire view
  virtual void		GetScaleRange();   // get the current scale range based on auto scaled columns (only if auto_scale is on)

  virtual void		RemoveGrid();
  virtual void		RemoveHeader(); // remove the header
  virtual void  	RemoveLines(); // remove all lines

  virtual void		RenderGrid();
  virtual void		RenderHeader();
  virtual void		RenderLines(); // render all the view_range lines
  virtual void		RenderLine(int view_idx, int data_row); // add indicated line

  // view control:
  override void		ClearViewRange();
  override void 	MakeViewRangeValid();

  override void		OnWindowBind_impl(iT3DataViewFrame* vw);
  override void		Clear_impl();
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Render_post(); // #IGNORE

  override void		UpdateFromDataTable_this(bool first);

  override void 	UpdateAfterEdit_impl();
};


//////////////////////////
//  iDataTableView_Panel 	//
//////////////////////////

class TA_API iDataTableView_Panel: public iViewPanelFrame {
  // abstract base for logview panels -- just has the viewspace widget; everything else is up to the subclass
  INHERITED(iViewPanelFrame)
  Q_OBJECT
public:
  iT3ViewspaceWidget*	    t3vs; //note: created with call to Constr_T3Viewspace

//  override String	panel_type() const; // this string is on the subpanel button for this panel

  DataTableView*		lv() {return (DataTableView*)m_dv;}
  SoQtRenderArea* 	ra() {return m_ra;}

  virtual void 		InitPanel();
  // called on structural changes 
  virtual void 		UpdatePanel();
  // call when data added/removed, or view is scrolled, or other non-structural changes

  void 			viewAll(); // zooms to fit entire scenegraph in window

  iDataTableView_Panel(DataTableView* lv);
  ~iDataTableView_Panel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iDataTableView_Panel;}

protected:
  SoQtRenderArea* 	m_ra;
  SoPerspectiveCamera*	m_camera;
  SoLightModel*		m_lm;

  void 			Constr_T3ViewspaceWidget(QWidget* widg);

  virtual void 		InitPanel_impl() {}
  // subclasses define these to do the actual work
  virtual void 		UpdatePanel_impl() {}
  // subclasses define these to do the actual work
};

class TA_API iGridTableView_Panel: public iDataTableView_Panel {
  Q_OBJECT
INHERITED(iDataTableView_Panel)
public:
  QWidget*		widg;
  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layTopCtrls;
  QCheckBox*		    chkDisplay;
  QCheckBox*		    chkHeaders;
  QCheckBox*		    chkRowNum;
  QCheckBox*		    chk2dFont;
  QLabel*		    lblFontScale;
  taiField*		    fldFontScale;
  QPushButton*		    butRefresh;
  QPushButton*		    butClear; // not used

  QHBoxLayout*		  layVals;
  QLabel*		    lblRows;
  taiIncrField*		    fldRows; // number of rows to display
  QLabel*		    lblCols;
  taiIncrField*		    fldCols; // number of cols to display
  QLabel*		    lblWidth;
  taiField*		    fldWidth; // width of the display (height is always 1.0)
  QLabel*		    lblTxtMin;
  taiField*		    fldTxtMin;
  QLabel*		    lblTxtMax;
  taiField*		    fldTxtMax;

  QHBoxLayout*		  layMatrix;
  QLabel*		    lblMatrix;
  QCheckBox*		    chkValText;
  QLabel*		    lblTrans;
  taiField*		    fldTrans; // mat_trans parency
  QLabel*		    lblRot;
  taiField*		    fldRot; // mat_rot ation
  QLabel*		    lblBlockHeight;
  taiField*		    fldBlockHeight; // mat_block_height

  QHBoxLayout*		  layColorScale;
  QCheckBox*		    chkAutoScale;
  ScaleBar*		    cbar;	      // colorbar
  QPushButton*		    butSetColor;

  QHBoxLayout*		  layViewspace;

  override String	panel_type() const; // this string is on the subpanel button for this panel
  GridTableView*	glv() {return (GridTableView*)m_dv;}

  iGridTableView_Panel(GridTableView* glv);
  ~iGridTableView_Panel();

public slots:
  void 			horScrBar_valueChanged(int value);
  void 			verScrBar_valueChanged(int value);

protected:
  override void		InitPanel_impl(); // called on structural changes
  override void		UpdatePanel_impl(); // called on structural changes

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iGridTableView_Panel;}

protected slots:

  void 		chkDisplay_toggled(bool on);
  void 		chkHeaders_toggled(bool on);
  void 		chkRowNum_toggled(bool on);
  void 		chk2dFont_toggled(bool on);
  void 		fldFontScale_textChanged();

  void 		butRefresh_pressed();
  void 		butClear_pressed();
  void 		butSetColor_pressed();

  void 		fldRows_textChanged();
  void 		fldCols_textChanged();
  void 		fldWidth_textChanged();
  void 		fldTxtMin_textChanged();
  void 		fldTxtMax_textChanged();

  void 		chkValText_toggled(bool on);
  void 		fldTrans_textChanged();
  void 		fldRot_textChanged();
  void 		fldBlockHeight_textChanged();

  void 		chkAutoScale_toggled(bool on);
  void		cbar_scaleValueChanged();
};


////////////////////////////////////////////////////////////////////////////////
// 		Graph View

/*
  User Data for Columns:

  MIN=x (x: float) -- forces min range to be x
  MAX=x (x: float) -- forces max range to be x

  Axis options:
  X_AXIS -- set as X Axis
  Z_AXIS -- set as Z Axis
  PLOT_1 -- set as plot_1 data (first data to be plotted)
  PLOT_2 -- set as plot_2 data
  ERR_1 -- set as err_1 data
  ERR_2 -- set as err_2 data
  COLOR_AXIS -- set as color_axis
  RASTER_AXIS -- set as raster_axis
*/

class TA_API GraphColView : public DataColView {
  // information for graph display of a column: note that the axis handles all the key display, so not much happens with this guy
INHERITED(DataColView)
public:
  FixedMinMax		fixed_range;	// fixed min/max range values for display (if not fixed, automatically set to min/max of data)
  MinMax		data_range;     // #READ_ONLY actual min and max of data (including fixed range) 

  override bool		selectEditMe() const { return true; }
  override String	GetDisplayName() const;

  DATAVIEW_PARENT(GraphTableView)

  void InitLinks();
  SIMPLE_COPY(GraphColView);
  TA_BASEFUNS(GraphColView);
protected:

private:
  void 	Initialize();
  void	Destroy();
};


class TA_API GraphAxisBase : public T3DataView {
  // ##INLINE base class for data about axis on a graph
INHERITED(T3DataView)
public:
  enum AxisType { X, Y, Z };

  bool			on;		// is this axis active for displaying info
  AxisType		axis;		// #READ_ONLY #SHOW type of axis this is, for rendering purposes
  GraphColView*		col_lookup; 	// #NULL_OK #FROM_GROUP_col_list lookup a column of data for this axis -- only for lookup purposes -- fills in the name and is reset to NULL -- name is what is actually used
  String		col_name;	// name of column of data for this axis
  FixedMinMax		fixed_range;	// fixed min/max range values for display (if not fixed, automatically set to min/max of data)

  RGBA			color;		// #EXPERT color of the line and points

  MinMax		data_range;     // #READ_ONLY actual min and max of data (including fixed range) 
  MinMax		range;		// #READ_ONLY actual display range of the axis data

  int          		n_ticks;	// #EXPERT #DEF_10 number of ticks desired
  float			axis_length; 	// #READ_ONLY in view units (width or depth)
  float			start_tick;	// #READ_ONLY #NO_SAVE first tick mark here
  float       		tick_incr;	// #READ_ONLY #NO_SAVE increment for tick marks
  int			act_n_ticks;	// #READ_ONLY #NO_SAVE actual number of ticks
  double		units;		// #READ_ONLY #NO_SAVE order of the units displayed (i.e. divide by this)

  T3DataView_List*	col_list; 	// #READ_ONLY #NO_SAVE list of columns for the col_lookup

  virtual void		SetColPtr(GraphColView* cgv);
  GraphColView* 	GetColPtr(); // get column pointer from col_name
  DataCol*		GetDAPtr();  // get dataarray ptr
  GraphTableView*	GetGTV() 	{ return (GraphTableView*)owner; }

  ///////////////////////////////////////////////////
  // 	Range Management

  virtual void		SetRange_impl(float first, float last);
  // set range to known good starting range values (fixed vals will still override); calls UpdateRange_impl to get "nice" vals still
  virtual bool 		UpdateRange_impl(float first, float last);
  // update range with new min/max data -- returns true if range actually changed -- finds a "nice number" for min and max based on n_ticks, etc
  virtual void 		ComputeRange();
  // compute range information based on data column, call UpdateRange_impl
  virtual bool 		UpdateRange();
  // update range information based on last cell in data column, call UpdateRange_impl & returns true if a new range update

  virtual void		ComputeTicks();
  // compute the start_tick, tick_incr, and act_n_ticks vals, based on current range info

  ///////////////////////////////////////////////////
  // 	Rendering

  inline float		DataToPlot(float data) // convert data value to plotting value
  { if(range.Range() == 0.0f) return 0.0f; return axis_length * range.Normalize(data); }
  virtual void 		RenderAxis(T3Axis* t3ax, int n_ax = 0, bool ticks_only=false);
  // draw the actual axis in a given direction -- if n_ax > 0 then it is an alternative one (only for Y)

  ///////////////////////////////////////////////////
  // 	Misc

  virtual void		InitFromUserData();
  // initialize various settings from the user data of the data column
  override bool		selectEditMe() const { return true; }

  void InitLinks();
  void CutLinks();
  SIMPLE_COPY(GraphAxisBase);
  T3_DATAVIEWFUNS(GraphAxisBase, T3DataView)
protected:
  void 			RenderAxis_X(T3Axis* t3ax, bool ticks_only=false);
  void 			RenderAxis_Z(T3Axis* t3ax, bool ticks_only=false);
  void 			RenderAxis_Y(T3Axis* t3ax, int n_ax = 0, bool ticks_only=false);

  override void 	UpdateAfterEdit_impl();
private:
  void			Initialize();
  void			Destroy();
};


class TA_API GraphPlotView : public GraphAxisBase {
  // parameters for plotting one column of data -- contains Y axis data as well
INHERITED(GraphAxisBase)
public:

  enum LineStyle {
    SOLID,			// -----
    DOT,			// .....
    DASH,			// - - -
    DASH_DOT			// _._._
#ifndef __MAKETA__
    ,LineStyle_MIN = SOLID, // also the default
    LineStyle_MAX = DASH_DOT
#endif
  };

  //NOTE: if PointStyle changed, must change T3GraphLine::MarkerStyle
  // NOTE: 0 is used as a special "NONE" pseudo-value during assignment of styles
  enum PointStyle {
    CIRCLE = 1,			// o
    SQUARE,			// []
    DIAMOND,			// <>
    TRIANGLE,
    MINUS,			// -
    BACKSLASH,
    BAR,			// |
    SLASH,			// /
    PLUS,			// +
    CROSS,			// x
    STAR			// *
#ifndef __MAKETA__
    ,PointStyle_NONE = 0, // pseudo value, only used during assignment of styles
    PointStyle_MIN = CIRCLE, // also the default
    PointStyle_MAX = STAR
#endif
  };

  LineStyle	line_style;	// #EXPERT the style in which the line is drawn
  PointStyle	point_style;	// #EXPERT the style in which the points are drawn

//   void InitLinks();
//   void CutLinks();
  SIMPLE_COPY(GraphPlotView);
  T3_DATAVIEWFUNS(GraphPlotView, GraphAxisBase)
protected:
  override void 	UpdateAfterEdit_impl();

private:
  void			Initialize();
  void			Destroy() { };
};

class TA_API GraphAxisView : public GraphAxisBase {
  // a non-Y axis (X, Z, etc)
INHERITED(GraphAxisBase)
public:

  bool		row_num; 	// display row number instead of column value for this axis

  override void 	ComputeRange();
  override bool 	UpdateRange();

  SIMPLE_COPY(GraphAxisView);
  T3_DATAVIEWFUNS(GraphAxisView, GraphAxisBase)
protected:
  override void 	UpdateAfterEdit_impl();

private:
  void			Initialize();
  void			Destroy() { };
};


class TA_API GraphTableView: public DataTableView {
  // the master view guy for entire graph view
INHERITED(DataTableView)
public:
  static GraphTableView* New(DataTable* dt, T3DataViewFrame*& fr);

  enum GraphType {
    XY,				// standard XY(Z) plot -- plot value determines Y axis coordinate to plot (optional error bars as well, if turned on)
    RASTER,			// raster plot (flat lines stacked up using raster_axis), typically with color representing plot data value or thresholded lines (spike raster)
  };

  enum PlotStyle {
    LINE,			// just a line, no points
    POINTS,			// just points, no line
    LINE_AND_POINTS, 		// both line and points
    THRESH_LINE,		// draw a line when value is over threshold
    THRESH_POINT,		// draw a point when value is over threshold
  };

  enum PointSize {
    SMALL,
    MEDIUM,
    LARGE,
  };

  enum ColorMode {
    FIXED_COLOR,		// use the color specified in the plot view (shown in EXPERT mode)
    VALUE_COLOR,		// the data value determines the data drawing color, looked up on the color scale
    COLOR_AXIS,			// use the data column specified by the color_axis to determine the drawing color
  };

  enum MatrixMode {		// how to display matrix data
    SEP_GRAPHS,			// each value in the matrix gets a separate graph, with graphs configured in the same layout as the matrix
    Z_INDEX,			// values in the matrix are drawn in the same graph, arrayed in depth along the z axis
  };

  GraphAxisView		x_axis; 	// the x axis (horizontal, left-to-right)
  GraphAxisView		z_axis;		// the z axis (in depth, front-to-back)
  GraphPlotView		plot_1;		// first column of data to plot
  GraphPlotView		plot_2;		// second column of data to plot (optional)
  bool			share_y_axis;	// plot both values on same Y axis (else separate)

  GraphType		graph_type; 	// type of graph to draw
  PlotStyle		plot_style;	// how to plot the data
  float			line_width;	// width of line -- 0 means use default
  PointSize		point_size;	// size of point symbols
  int 			point_spacing;	// #CONDEDIT_OFF_plot_style:LINE how frequently to display point markers 
  ColorMode		color_mode;	// how to determine the colors to draw
  bool			negative_draw;	// continue same line when X value resets in negative axis direction?
  int 			label_spacing;	// how frequently to display text labels of the data values (-1 = never); if plotting a string column, the other data column (e.g. plot_2) is used to determine the y axis values
  MatrixMode		matrix_mode;	// how to display matrix data (note that if a matrix column is selected, it is the only thing displayed)
  taMisc::MatrixView		mat_layout; 	// #CONDEDIT_ON_matrix_mode:SEP_GRAPHS #DEF_BOT_ZERO layout of matrix graphs for SEP_GRAPHS mode
  bool			mat_odd_vert;	// #CONDEDIT_ON_matrix_mode:SEP_GRAPHS how to arrange odd-dimensional matrix values (e.g., 1d or 3d) -- put the odd dimension in the Y (vertical) axis (else X, horizontal)

  GraphPlotView		err_1;		// data for error bars for plot_1 values
  GraphPlotView		err_2;		// data for error bars for plot_2 values
  int			err_spacing;	// #CONDEDIT_ON_graph_type:XY_ERR spacing between
  float			err_bar_width;	// half-width of error bars, in view plot units

  GraphAxisView		color_axis;	// #CONDEDIT_ON_color_mode:COLOR_AXIS color axis, for determining color of lines when color_mode = COLOR_AXIS
  ColorScale		colorscale; 	// contains current min,max,range,zero,auto_scale

  GraphAxisView		raster_axis;	// #CONDEDIT_ON_graph_type:RASTER raster axis, if doing a raster plot
  float			thresh;		// #CONDEDIT_ON_plot_style:THRESH_LINE,THRESH_POINT threshold on raw data value for THRESH_LINE or THRESH_POINT plotting sytles 
  float			thr_line_len;	// length of line to draw when above threshold: value is subtracted and added to current X value to render line
  float			width;		// how wide to make the display (height is always 1.0)
  float			depth;		// how deep to make the display (height is always 1.0)

  bool			two_d_font;	// #DEF_true use 2d font (easier to read, but doesn't scale) instead of 3d font
  float			two_d_font_scale; // #DEF_350 how to scale the two_d font relative to the computed 3d number

  override void	InitDisplay(bool init_panel = true);
  override void	UpdateDisplay(bool update_panel = true);

  void		SetColorSpec(ColorScaleSpec* color_spec);
  // #BUTTON set the color scale spec to determine the palette of colors representing values

  ///////////////////////////////////////////////////
  // misc housekeeping

  void		FindDefaultXZAxes();
  // set X and Z axis columns to user data spec or the last INT columns -- if that doesn't work, then choose the first numeric columns
  void		FindDefaultPlot1();
  // set plot_1 as first float/double column (or user data spec)
  void		InitFromUserData();
  // set initial settings based on user data in columns and overall table

  ///////////////////////////////////////////////////
  // view button/field callbacks

  void		setGraphType(GraphType value);
  void		setPlotStyle(PlotStyle value);
  void		setRows(int rows);
  void		setColorMode(ColorMode value);
  void		setLineWidth(float value);
  void		setPointSpacing(int value);
  void		setLabelSpacing(int value);
  void		setErrSpacing(int value);
  void		setThresh(float value);
  void		setNegDraw(bool value);
  void		set2dFont(bool value);
  void		setWidth(float wdth);
  void		setScaleData(bool auto_scale, float scale_min, float scale_max);
  // updates the values in us and the stored ones in the colorscale list

  void		setXAxis(GraphColView* value);
  void		setZAxis(GraphColView* value);
  void		setPlot1(GraphColView* value);
  void		setPlot2(GraphColView* value);
  void		setShareAxis(bool value);
  void		setErr1(GraphColView* value);
  void		setErr2(GraphColView* value);
  void		setColorAxis(GraphColView* value);
  void		setRasterAxis(GraphColView* value);

  iGraphTableView_Panel*	lvp(){return (iGraphTableView_Panel*)(iDataTableView_Panel*)m_lvp;}
  T3GraphViewNode* node_so() const {return (T3GraphViewNode*)m_node_so.ptr();}

  override String	GetLabel() const;
  override String	GetName() const;

  override bool		selectEditMe() const { return true; }
  
  void	InitLinks();
  void 	CutLinks();
  SIMPLE_COPY(GraphTableView);
  T3_DATAVIEWFUNS(GraphTableView, DataTableView)

protected:
  int			n_plots; // updated during rendergraph -- number of plots
  T3Axis* 		t3_x_axis;
  T3Axis* 		t3_x_axis_top; // tick-only top version of x
  T3Axis* 		t3_x_axis_far;
  T3Axis* 		t3_x_axis_far_top;
  T3Axis* 		t3_y_axis;
  T3Axis* 		t3_y_axis_rt; // tick-only rt version of y (if not 2nd y)
  T3Axis* 		t3_y_axis_far;
  T3Axis* 		t3_y_axis_far_rt;
  T3Axis* 		t3_z_axis;
  T3Axis* 		t3_z_axis_rt;
  T3Axis* 		t3_z_axis_top;
  T3Axis* 		t3_z_axis_top_rt;

  ///////////////////////////////////////////////////
  // 	Rendering

  virtual void		RenderGraph();
  virtual void		RenderGraph_Scalar();
  virtual void		RenderGraph_Matrix_Sep();
  virtual void		RenderGraph_Matrix_Zi();

  virtual void 		ComputeAxisRanges();
  // compute range information based on data column, call UpdateRange_impl

  virtual const iColor* GetValueColor(GraphAxisBase* ax_clr, float val);
  // get color from value using given axis

  virtual void		RenderAxes();	   // get all the axis info

  virtual void  	RemoveGraph(); // remove all lines

  virtual void 		PlotData_XY(GraphPlotView& plv, GraphPlotView& erv, GraphPlotView& yax,
				    T3GraphLine* t3gl, int mat_cell = -1);
  // plot XY data from given plot view column (and err view column) into given line, using given yaxis values, if from matrix then mat_cell >= 0)
  virtual void 		PlotData_String(GraphPlotView& plv_str, GraphPlotView& plv_y, T3GraphLine* t3gl);
  // plot string data from given plot view column using Y values from given Y column

  override void		OnWindowBind_impl(iT3DataViewFrame* vw);
  override void		Clear_impl();
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Render_post(); // #IGNORE

  override void		UpdateFromDataTable_this(bool first);

  override void 	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
};

class TA_API iGraphTableView_Panel: public iDataTableView_Panel {
  Q_OBJECT
INHERITED(iDataTableView_Panel)
public:
  QWidget*		widg;
  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layTopCtrls;
  QCheckBox*		    chkDisplay;
  QLabel*		    lblGraphType;
  taiComboBox*		    cmbGraphType;
  QLabel*		    lblPlotStyle;
  taiComboBox*		    cmbPlotStyle;

  QPushButton*		    butRefresh;
  QPushButton*		    butClear;

  QHBoxLayout*		  layVals;
  QLabel*		    lblLineWidth;
  taiField*		    fldLineWidth;
  QLabel*		    lblPointSpacing;
  taiField*		    fldPointSpacing;
  QLabel*		    lblLabelSpacing;
  taiField*		    fldLabelSpacing;
  QCheckBox*		    chkNegDraw;

  QLabel*		    lblWidth;
  taiField*		    fldWidth; // width of the display (height is always 1.0)

  QHBoxLayout*		  layXAxis;
  QLabel*		    lblXAxis;
  taiListElsButton*	    lelXAxis; // list element chooser
  QCheckBox*		    rncXAxis; // row number checkbox
  taiPolyData*	    	    pdtXAxis; // fixed_range polydata (inline)

  QHBoxLayout*		  layZAxis;
  QCheckBox*		    oncZAxis; // on checkbox
  QLabel*		    lblZAxis;
  taiListElsButton*	    lelZAxis; // list element chooser
  QCheckBox*		    rncZAxis; // row number checkbox
  taiPolyData*	    	    pdtZAxis; // fixed_range polydata (inline)

  QHBoxLayout*		  lay1Axis;
  QLabel*		    lbl1Axis;
  taiListElsButton*	    lel1Axis; // list element chooser
  taiPolyData*	    	    pdt1Axis; // fixed_range polydata (inline)

  QHBoxLayout*		  lay2Axis;
  QCheckBox*		    onc2Axis; // on checkbox
  QLabel*		    lbl2Axis;
  taiListElsButton*	    lel2Axis; // list element chooser
  taiPolyData*	    	    pdt2Axis; // fixed_range polydata (inline)

  QHBoxLayout*		  layPlots;
  QCheckBox*		    chkShareAxis;
  QLabel*		    lbl1Err;
  taiListElsButton*	    lel1Err;
  QCheckBox*		    onc1Err; // on checkbox
  QLabel*		    lbl2Err;
  taiListElsButton*	    lel2Err;
  QCheckBox*		    onc2Err; // on checkbox
  QLabel*		    lblErrSpacing;
  taiField*		    fldErrSpacing;

  QHBoxLayout*		  layCAxis;
  QLabel*		    lblColorMode;
  taiComboBox*		    cmbColorMode;
  QLabel*		    lblCAxis;
  taiListElsButton*	    lelCAxis; // list element chooser
  QLabel*		    lblThresh;
  taiField*		    fldThresh;

  QHBoxLayout*		  layColorScale;
  ScaleBar*		    cbar;	      // colorbar
  QPushButton*		    butSetColor;

  QHBoxLayout*		  layRAxis;
  QLabel*		    lblRAxis;
  taiListElsButton*	    lelRAxis; // list element chooser
  taiPolyData*	    	    pdtRAxis; // fixed_range polydata (inline)

  QHBoxLayout*		  layScroll;
  QLabel*		    lblRows;
  taiIncrField*		    fldRows; // number of rows to display
  QLabel*		    lblView;
  QScrollBar*	            scrView; // scrollbar for view range (instead of using viewspace)

  override String	panel_type() const; // this string is on the subpanel button for this panel
  GraphTableView*	glv() {return (GraphTableView*)m_dv;}

  iGraphTableView_Panel(GraphTableView* glv);
  ~iGraphTableView_Panel();

public slots:
  void 			scrView_valueChanged(int value);

protected:
  override void		InitPanel_impl(); // called on structural changes
  override void		UpdatePanel_impl(); // called on structural changes

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iGraphTableView_Panel;}

protected slots:

  void 		chkDisplay_toggled(bool on);
  void 		cmbGraphType_itemChanged(int itm);
  void 		chkShareAxis_toggled(bool on);
  void 		chkNegDraw_toggled(bool on);
  void 		fldRows_textChanged();

  void 		butRefresh_pressed();
  void 		butClear_pressed();
  void 		butSetColor_pressed();

  void 		cmbPlotStyle_itemChanged(int itm);
  void 		cmbColorMode_itemChanged(int itm);
  void 		fldLineWidth_textChanged();
  void 		fldPointSpacing_textChanged();
  void 		fldLabelSpacing_textChanged();

  void 		fldWidth_textChanged();

  void		cbar_scaleValueChanged();

  void 		lelXAxis_dataChanged(taiData*);
  void 		rncXAxis_toggled(bool);
  void 		pdtXAxis_dataChanged(taiData*);

  void 		lelZAxis_dataChanged(taiData*);
  void 		oncZAxis_toggled(bool);
  void 		rncZAxis_toggled(bool);
  void 		pdtZAxis_dataChanged(taiData*);

  void 		lel1Axis_dataChanged(taiData*);
  void 		pdt1Axis_dataChanged(taiData*);

  void 		lel2Axis_dataChanged(taiData*);
  void 		onc2Axis_toggled(bool);
  void 		pdt2Axis_dataChanged(taiData*);

  void 		lel1Err_dataChanged(taiData*);
  void 		onc1Err_toggled(bool);
  void 		lel2Err_dataChanged(taiData*);
  void 		onc2Err_toggled(bool);
  void 		fldErrSpacing_textChanged();
  void 		fldThresh_textChanged();

  void 		lelCAxis_dataChanged(taiData*);

  void 		lelRAxis_dataChanged(taiData*);
  void 		pdtRAxis_dataChanged(taiData*);
};


/////////////////////////////////////////////////////////////////////
//	Other DataTable GUI Stuff

class TA_API tabDataTableViewType: public tabOViewType {
INHERITED(tabOViewType)
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabDataTableViewType, tabOViewType) //
protected:
//nn  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};


class TA_API DataTableDelegate: public QItemDelegate {
  Q_OBJECT
INHERITED(QItemDelegate)
public:
  DataTableRef		dt; // we maintain a ref to get modal information
  
  DataTableDelegate(DataTable* dt);
  ~DataTableDelegate();
};

#ifndef __MAKETA__ // too much crud to parse
class TA_API iDataTableView: public iTableView {
  // widget with some customizations to display submatrix views
INHERITED(iTableView)
  Q_OBJECT
public:
  DataTable*		dataTable() const;
  
  iDataTableView(QWidget* parent = NULL);

public: // cliphandler i/f
  override void 	EditAction(int ea);
  override void		GetEditActionsEnabled(int& ea);

signals:
  void 			sig_currentChanged(const QModelIndex& current);
  void			sig_dataChanged(const QModelIndex& topLeft,
    const QModelIndex & bottomRight); // #IGNORE
  
protected:
  override void 	currentChanged(const QModelIndex& current,
    const QModelIndex& previous); // override
  override void 	dataChanged(const QModelIndex& topLeft,
    const QModelIndex & bottomRight); // refresh mat cell if in here
};
#endif // MAKETA

class TA_API iDataTableEditor: public QWidget, public ISelectableHost {
  Q_OBJECT // ##NO_CSS
INHERITED(QWidget)
public:
  QVBoxLayout*		layOuter;
  QSplitter*		splMain;
  iDataTableView*	  tvTable; // the main table
  iMatrixTableView*	  tvCell; // a matrix cell in the table (only shown if needed)

  DataTable*		dt() const {return m_dt;}
  void			setDataTable(DataTable* dt);
  DataTableModel*	dtm() const {return (m_dt.ptr()) ? m_dt->GetDataModel() : NULL;}
  
  void			Refresh(); // for manual refresh
  
  iDataTableEditor(QWidget* parent = NULL);
  ~iDataTableEditor();
  
public slots:
  void			tvTable_currentChanged(const QModelIndex& index); // #IGNORE
  void			tvTable_dataChanged(const QModelIndex& topLeft,
    const QModelIndex & bottomRight); // #IGNORE
  void 			tvTable_layoutChanged(); // #IGNORE

  
public: // ISelectableHost i/f
  override bool 	hasMultiSelect() const {return false;} // always
  override QWidget*	widget() {return this;} 
protected:
  override void		UpdateSelectedItems_impl(); 

protected:
  DataTableRef		m_dt;
  taMatrixPtr		m_cell; // current cell TODO: this ref will prevent col from growing for new row
  QModelIndex		m_cell_index; // we keep this to refresh cell if data changes
  void			ConfigView(); // setup or change view, esp after col ins/deletes
};


class TA_API iDataTablePanel: public iDataPanelFrame {
  Q_OBJECT
INHERITED(iDataPanelFrame)
public:
  iDataTableEditor*	dte; 
  
  DataTable*		dt() {return (m_link) ? (DataTable*)(link()->data()) : NULL;}
  override String	panel_type() const; // this string is on the subpanel button for this panel

  override int 		EditAction(int ea);
  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  void			GetSelectedItems(ISelectable_PtrList& lst); // list of the selected cells

  iDataTablePanel(taiDataLink* dl_);
  ~iDataTablePanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iDataTablePanel;}
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

protected:
  override void		Render_impl();
  override void		Refresh_impl();
  
protected slots:
  void			tv_hasFocus(iTableView* sender); // for both tableviews
  void			mb_View(); // View button on minibar
};

/* TODO
class TA_API DataTableGridViewWizard: public taWizard {
  // wizard for automating construction of DataTableGridView objects
INHERITED(taWizard)
public:

  DataTableRef		dt; // #NO_NULL the data table being viewed
  

  void 	InitLinks();
  void	CutLinks(); 
  SIMPLE_COPY(DataTableGridViewWizard);
  TA_BASEFUNS(DataTableGridViewWizard); //
protected:
//  override void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};
*/

/*
  MIME TYPE "tacss/matrixdesc" -- description of matrix data (no content)

    <flat_cols>;<flat_rows>;\n
      
      
    The data itself (text/plain) is in TSV format.
    
    flat_cols/rows (>=1) indicate the flattend 2D rep of the data
    
    Note that this format is primarily to make decoding of the data faster
    and more definite where tacss is the source of the data, compared with
    just parsing the text/plain data (which the decoder can do, to import
    spreadsheet data.)
    .
    
  MIME TYPE "tacss/tabledesc" -- description of table data (no content)

    <flat_cols>;<flat_rows>;\n
    <mat_cols>;<mat_rows>;\n
    <col0_flat_cols>;<col0_flat_rows>;<is_image>;\n
    ...
    <colN-1-flat_cols>;<colN-1_flat_rows>;<is_image>;\n
    
    for scalar cols: colx-cols=colx-rows=1
      
    The data itself (text/plain) is in a TSV tabular form, of total
    Sigma(colx-cols)x=0:N-1 by <rows> * Max(colx-rows) -- non-existent values
    will just have blank entries.
      

*/

class TA_API taiTabularDataMimeFactory: public taiMimeFactory {
// this factory handles both Matrix and Table clipboard formats
INHERITED(taiMimeFactory)
public:
  static const String 	tacss_matrixdesc; // "tacss/matrixdesc"
  static const String 	tacss_tabledesc; // "tacss/tabledesc" 
//static taiTabularDataMimeFactory* instance(); // provided by macro

  void			Mat_QueryEditActions(taMatrix* mat, 
    const CellRange& selected, taiMimeSource* ms,
    int& allowed, int& forbidden) const; // determine ops based on clipboard and selected; ms=NULL for source only
    
  void			Mat_EditActionD(taMatrix* mat, 
    const CellRange& selected, taiMimeSource* ms, int ea) const;
    // dest edit actions; note: this does the requery to insure it is still legal
  void			Mat_EditActionS(taMatrix* mat, 
    const CellRange& selected, int ea) const;
    // src edit actions; note: this does the requery to insure it is still legal
    
  taiClipData* 		Mat_GetClipData(taMatrix* mat,
    const CellRange& sel, int src_edit_action, bool for_drag = false) const;
  
  void			Mat_Clear(taMatrix* mat,
    const CellRange& sel) const;
  void			AddMatDesc(QMimeData* md,
    taMatrix* mat, const CellRange& selected) const;


  void			Table_QueryEditActions(DataTable* tab, 
    const CellRange& selected, taiMimeSource* ms,
    int& allowed, int& forbidden) const; // determine ops based on clipboard and selected; ms=NULL for source only
    
  void			Table_EditActionD(DataTable* tab, 
    const CellRange& selected, taiMimeSource* ms, int ea) const;
    // dest edit actions; note: this does the requery to insure it is still legal
  void			Table_EditActionS(DataTable* tab, 
    const CellRange& selected, int ea) const;
    // src edit actions; note: this does the requery to insure it is still legal
    
  taiClipData* 		Table_GetClipData(DataTable* tab,
    const CellRange& sel, int src_edit_action, bool for_drag = false) const;
   
  void			Table_Clear(DataTable* tab,
    const CellRange& sel) const;
  
  void			AddTableDesc(QMimeData* md,
    DataTable* tab, const CellRange& selected) const;


  TA_MFBASEFUNS(taiTabularDataMimeFactory);
protected:
  void			AddDims(const CellRange& sel, String& str) const;

private:
  void	Initialize();
  void	Destroy() {}
};

class TA_API taiTabularDataMimeItem: public taiMimeItem { 
  // #NO_INSTANCE #VIRT_BASE base for matrix, tsv, and table data; this class is not itself instantiated
INHERITED(taiMimeItem)
public: // i/f for tabular data guy
  iSize			flatGeom() const {return m_flat_geom;} // the (flat) size of the data in rows/cols
  inline int		flatRows() const {return m_flat_geom.h;}
  inline int		flatCols() const {return m_flat_geom.w;}
  
  virtual void		WriteMatrix(taMatrix* mat, const CellRange& sel);
  virtual void		WriteTable(DataTable* tab, const CellRange& sel);
  
  TA_ABSTRACT_BASEFUNS(taiTabularDataMimeItem);

protected:
  enum TsvSep { // for reading tsv text streams
    TSV_TAB,  // tab -- item separator
    TSV_EOL,  // eol -- row separator
    TSV_EOF   // eof -- end of file
  };
  
  static TsvSep		no_sep; // when ignored

  iSize			m_flat_geom; 
  bool 			ReadInt(String& arg, int& val); // read a ; terminated int
  bool 			ExtractGeom(String& arg, iSize& val); // get the cols/rows
  bool			ReadTsvValue(istringstream& strm, String& val,
     TsvSep& sep = no_sep); // reads value if possible, into val, returning true if a value read, and the separator encountered after the value in sep.
  virtual void		WriteTable_Generic(DataTable* tab, const CellRange& sel);
  
private:
  NOCOPY(taiTabularDataMimeItem);
  void	Initialize() {}
  void	Destroy() {}
};


class TA_API taiMatrixDataMimeItem: public taiTabularDataMimeItem { // this class handles Matrix -- optimized since we know the dims, and know the data is accurate
INHERITED(taiTabularDataMimeItem)
public: // i/f for tabular data guy  
  
  TA_BASEFUNS_NOCOPY(taiMatrixDataMimeItem);
    
public: // TAI_xxx instance interface -- used for dynamic creation
  override taiMimeItem* Extract(taiMimeSource* ms, 
    const String& subkey = _nilString);
protected:
  override bool 	Constr_impl(const String&);
  override void		DecodeData_impl();
private:
  void	Initialize() {}
  void	Destroy() {}
};

class TA_API taiTsvMimeItem: public taiTabularDataMimeItem { // this class handles generic TSV data, ex. from Excel and other spreadsheets
INHERITED(taiTabularDataMimeItem)
public: // i/f for tabular data guy  
  
  TA_BASEFUNS_NOCOPY(taiTsvMimeItem);
    
public: // TAI_xxx instance interface -- used for dynamic creation
  override taiMimeItem* Extract(taiMimeSource* ms, 
    const String& subkey = _nilString); //NOTE: we typically only ask for this type if we *don't* get a Matrix or Table, so we don't waste time decoding manually
protected:
//nn  override bool 	Constr_impl(const String&);
//nn  override void		DecodeData_impl();
private:
  void	Initialize();
  void	Destroy() {}
};


class TA_API taiTableColDesc { // #NO_CSS #NO_MEMBERS value class to hold col data
public:
  iSize		flat_geom;
  bool		is_image;
  taiTableColDesc() {is_image = false;} //
  
public: // ops to keep the Array templ happy
  friend bool	operator>(const taiTableColDesc& a, const taiTableColDesc& b)
    {return false;}
  friend bool	operator==(const taiTableColDesc& a, const taiTableColDesc& b)
    {return false;}
};

class TA_API taiTableColDesc_PArray: public taPlainArray<taiTableColDesc> {
 // #NO_CSS #NO_MEMBERS
public:
  taiTableColDesc_PArray() {} // keeps maketa happy
};


class TA_API taiTableDataMimeItem: public taiTabularDataMimeItem { // for DataTable data
INHERITED(taiTabularDataMimeItem)
public:
  iSize			tabGeom() const {return m_tab_geom;} // the table size of the data in table rows/cols (same as flat, if all cols are scalar)
  inline int		tabRows() const {return m_tab_geom.h;}
  inline int		tabCols() const {return m_tab_geom.w;}

  virtual void		GetColGeom(int col, int& cols, int& rows) const;
    // 2-d geom of the indicated column; always 1x1 (scalar) for matrix data
  inline int		maxCellRows() const {return m_max_row_geom;}
  
  override void		WriteTable(DataTable* tab, const CellRange& sel);
  
  TA_BASEFUNS_NOCOPY(taiTableDataMimeItem);
    
public: // TAI_xxx instance interface -- used for dynamic creation
  override taiMimeItem* Extract(taiMimeSource* ms, 
    const String& subkey = _nilString); 
protected:
  iSize			m_tab_geom;
  int			m_max_row_geom;
  taiTableColDesc_PArray col_descs;
  override bool 	Constr_impl(const String&);
  override void		DecodeData_impl();
private:
  void	Initialize();
  void	Destroy() {}
}; 




#endif
