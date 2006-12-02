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



// datagraph.h -- the non-gui aspects of graphing

#ifndef DATAGRAPH_H
#define DATAGRAPH_H

#include "datatable.h"
#include "colorscale.h"
#include "ta_geometry.h"
#include "minmax.h"

/* Graphing has been completely reorganized from v3.2. In v4, the non-gui and gui elements have been
   separated into separate, parallel class hierarchies. The list below gives the correspondance:

   v4 class		v3.2
   AxisSpec		GraphButton, non-gui data
   Graph		GraphGraph, GraphEditor, non-gui stuff

   AxisView		AxisSpec, gui stuff
   GraphLines		Grapher_G -- visual container for the lines
   GraphView

*/

// forwards this file
class AxisSpec;
class XAxisSpec;
class YAxisSpec;
class ZAxisSpec;
class YAxisSpec_List; //
class GraphletSpec;
class GraphletSpec_List;
class GraphSpec;
//class Graph;


/*
  Additional Display Options (see DA_ViewSpec for common ones)

  MIN=x (x: float) -- for a col that is an axis, forces min range to be x
  MAX=x (x: float) -- for a col that is an axis, forces max range to be x
  DISP_STRING -- for string cols, sets !visible (HUH????) (ex. added by ClustNode for 'labels' col)
  STRING_COORDS=i (i: int) -- plot coords for strings is in col i of this group

  Axis options:
  X_AXIS -- explicitly marks this col as the X Axis
  Y_AXIS -- explicitly marks this col as a Y Axis
  Y_DATA -- explicitly marks this col as Y data, and typically indicates a slave to another Y axis
  Z_AXIS -- explicitly marks this col as a Z Axis (where appropriate)
  AXIS=i (i: int) -- links the col to a specific YAxis (i is the colspec index in same group
    of that axis); also, for Strings with no STRING_COORDS, specifies the string_coords col
    ex. ClustNode sets 'labels' col to use the Y axis col
  STRING_COORDS=i (i: int) -- plot coords for strings is in col i of this group

  Axis Binding Notes

  Col Type		axis_spec contains
  YData, axis		points to YAxisSpec, which is based on this col
  YData, slave		points to a YAxisSpec based on another col
  XAxis			points to a XAxisSpec
  ZAxis			points to a ZAxisSpec
  String		points to YAxisSpec that corresponds to its string_coord values

*/



class TA_API GraphLineSpec: public taDataView {
  // specifies one graph line
INHERITED(taDataView)
public:
  enum LineType {
    LINE,			// just a line, no pts
    POINTS,			// just pts, no line
    LINE_AND_POINTS, 		// both
    STRINGS,			// string (text) values -- no lines
    TRACE_COLORS,		// subsequent traces (repeats through same X values) are color coded using colorscale
    VALUE_COLORS,		// Y values are represented with colors using colorscale
    THRESH_POINTS		// Y values are thresholded with thresh parameter and displayed as points when above threshold
  };
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

  GraphSpec*	graph_spec;	// #READ_ONLY #NO_SAVE which (root)graph do I belong in (set in InitLinks)
  RGBA		line_color;	 // color of the line

  // spec attributes used directly by GraphLine (for cols that are y data)
  LineType	line_type;	// the way the line is drawn
  LineStyle	line_style;	// the style in which the line is drawn
  float		line_width;	// #DEF_0 width of the line in pixels; 0.0f means use default
  PointStyle	point_style;	// the style in which the points are drawn
  bool		negative_draw;	// continue same line when X value resets in negative axis direction?
  float		thresh;		// threshold for THRESH_POINTS line style
  GraphLineSpec* string_coords;  // column that contains vertical coordinate values for positioning String data labels

  const iColor*	def_color() const; // default color of lines
  inline GraphSpec* graphSpec() const {return parent();}
  DATAVIEW_PARENT(GraphSpec) // GraphSpec* parent() const;

  bool		is_string() const; // true if data is string

  virtual void	GpShareAxis();	// #BUTTON #CONFIRM make every element in this group share the same Y axis, which is the first in the group
  virtual void	GpSepAxes();	// #BUTTON #CONFIRM make every element in this group have its own Y axis

  virtual void 	FindStringCoords(); // find previous float_Data for default string coordinates

  virtual void	SetStringCoords(GraphLineSpec* as) {
    taBase::SetPointer((TAPtr*)&string_coords, as);
  }

  void 	InitLinks();
  void  CutLinks();
  void  UpdateAfterEdit();
  void Copy_(const GraphLineSpec& cp);
  COPY_FUNS(GraphLineSpec, inherited);
  TA_BASEFUNS(GraphLineSpec);
private:
  void 	Initialize();
  void	Destroy();
};

class TA_API GraphLineSpec_List: public DataView_List {
INHERITED(DataView_List)
public:

  
  TA_DATAVIEWLISTFUNS(GraphLineSpec_List, inherited, GraphLineSpec)
private:
  void 	Initialize() {SetBaseType(&TA_GraphLineSpec);}
  void	Destroy() {}
};


class TA_API AxisSpec: public taDataView { // #VIRT_BASE specs of the axis on a graph, name is cloned from primary col
INHERITED(taDataView)
public:
  enum Axis {X, Y, Z}; // invariant, used internally

  MinMax		range;		// display range of the axis data
  MinMax		true_range;     // actual min and max of data (including fixed range) 
  MinMax		eff_range;     	// effective range: can be changed if display contains different info than basic graph (e.g., multi-traces)
  FixedMinMax		fixed_range;	// fixed range: optionally constrained range values
  int          		n_ticks;	// #DEF_10 number of ticks desired

  virtual Axis		axis() const {return X;}
  virtual const iColor*	def_color() const; // default color of axis -- depends on type of axis
  inline GraphSpec*	graphSpec() const {return parent();}
  DATAVIEW_PARENT(GraphSpec) // GraphSpec* parent() const;

  virtual bool 		InitUpdateAxis(bool init) {return false;}  // do an update range from data, update axis if yest

  virtual void		InitData(); // partial initializer called internally and externally in several places
  virtual void		InitRange(float first, float last);
  // initialize range to known good starting range values (fixed vals will still override)
  virtual bool 		UpdateRange(float first, float last);
  // update range with new min/max data -- returns true if range actually changed

  override void		UpdateAfterEdit();
  override void		InitLinks();
  override void		CutLinks();
  void Copy_(const AxisSpec& cp);
  COPY_FUNS(AxisSpec, inherited);
  TA_BASEFUNS(AxisSpec)
protected:
  static MinMax		temp_range;     // #IGNORE scratch pad
   
  static iColor      	m_def_color; // black, for x and z

private:
  void			Initialize();
  void			Destroy();
};

class TA_API XAxisSpec: public AxisSpec { // specs of the X axis on a graph, name is cloned from primary col
INHERITED(AxisSpec)
public:
  enum XAxisType { // source of data for the X axis; determines ranges, etc.
    COL_VALUE,	// the content of the col in the data table (legacy mode)
    ROW_NUM		// the ordinal index of the data value (1, 2, 3... etc.)
  };

  XAxisType		axis_type;

  override void		Config(GraphLineSpec* spec = NULL); // called after create or update
  override bool 	InitUpdateAxis(bool init);

  void Copy_(const XAxisSpec& cp);
  COPY_FUNS(XAxisSpec, AxisSpec);
  TA_BASEFUNS(XAxisSpec)
private:
  void			Initialize();
  void			Destroy() {}
};

class TA_API YAxisSpec: public AxisSpec { // specs of the Y axis on a graph, name is cloned from primary col
INHERITED(AxisSpec)
friend class YAxisSpec_List;
public:
  bool			no_vertical;  // don't draw any vertical dimension at all (for VALUE_COLORS or THRESH_POINTS line_type

  override Axis		axis() const {return Y;}
  override const iColor*	def_color() const; // default color of axis -- depends on type of axis

  override bool 	InitUpdateAxis(bool init);

  void Copy_(const YAxisSpec& cp);
  COPY_FUNS(YAxisSpec, AxisSpec);
  TA_BASEFUNS(YAxisSpec)
private:
  void			Initialize();
  void			Destroy() {}
};

class TA_API ZAxisSpec: public AxisSpec { // specs of the Z axis on a graph, name is cloned from primary col
INHERITED(AxisSpec)
public:
  int			n_traces;	// #READ_ONLY for STACK_TRACES and STACK_LINES, # of traces

  override Axis		axis() const {return Z;}

//nn  override void		Config(GraphLineSpec* spec = NULL); // called after create or update
  override bool 	InitUpdateAxis(bool init);

  void Copy_(const ZAxisSpec& cp);
  COPY_FUNS(ZAxisSpec, AxisSpec);
  TA_BASEFUNS(ZAxisSpec)
private:
  void			Initialize();
  void			Destroy() {}
};


class TA_API YAxisSpec_List: public DataView_List {
INHERITED(DataView_List)
public:
  YAxisSpec*		FindBySpec(const GraphLineSpec* spec, int* idx = NULL);
    // find an axis with given spec, optionally returning its index as well (NULL/-1 if not found)
  TA_DATAVIEWLISTFUNS(YAxisSpec_List, DataView_List, YAxisSpec)

private:
  void Initialize() {SetBaseType(&TA_YAxisSpec);}
  void Destroy() {}
};


class TA_API GraphletSpec: public YAxisSpec_List {
  // link list of axes, plus additional per-graphlet info
INHERITED(YAxisSpec_List)
public:

  TA_BASEFUNS(GraphletSpec)

private:
  void			Initialize() {}
  void			Destroy() {}
};

class TA_API GraphletSpec_List: public taList<GraphletSpec> {
INHERITED(taList<GraphletSpec>)
public:
  TA_BASEFUNS(GraphletSpec_List)

private:
  void			Initialize() {SetBaseType(&TA_GraphletSpec);}
  void			Destroy() {}
};


class TA_API GraphSpec : public DataTableViewSpec {
  // controls display of datatable in a graph format
INHERITED(DataTableViewSpec)
public:
  enum GraphType { // overall type of the graph
    TWOD,		// #LABEL_2D standard 2d graph
    THREED,		// #LABEL_3D standard 3d graph
    STACK_TRACES,	// #LABEL_3D_Stack_Traces each new trace gets plotted along z-axis
    STACK_LINES		// #LABEL_3D_Stack_Lines each line in a group gets plotted along z-axis
  };

  enum ColorType { // defines standard color sequences
    C_RAINBOW,		// color, rainbow
    C_GREYSCALE,	// color, greyscale
    M_MONO,		// monochrome
    P_RAINBOW,		// printer, rainbow
    P_GREYSCALE,	// printer, greyscale
    P_MONO,		// printer, monochrome
    CUSTOM 		// custom, let user set colors
  };

  enum SequenceType { // defines sequences of line features
    COLORS,		// use sequential colors
    LINES,		// use sequential line styles
    POINTS,		// use sequential point styles
    NONE 		// no features
  };

  enum AxisSharing { // default configuration for graphs
    SHARED_AXES,			// all lines/axes on one graph
    SEPARATE_AXES,			// all y axes on a separate graph
    CUSTOM_AXES			// user allocs them
  };

  GraphType		graph_type;
  RGBA			background;	// background color
  TAColor		bg_color;	// #IGNORE actual color object
  ColorType		color_type;	// palette of colors to use
  bool			use_cbar; 	// if true, actually display the cbar (if some kind of colors display used)
  SequenceType		sequence_1;	// first (innermost) sequence of features
  SequenceType		sequence_2;	// second sequence of features
  SequenceType		sequence_3;	// third sequence of features

  YAxisSpec_List	y_axes; // all the y axes (can be shared by many cols)
  XAxisSpec		x_axis_spec; // #SHOW_TREE spec info of the x axis -- is shared by all cols
  ZAxisSpec		z_axis_spec;
    //  #SHOW_TREE spec info of the z axis (3D only) -- is shared by all cols
  GraphLineSpec_List	line_specs;

  AxisSharing		axis_sharing; // draw each group of lines sharing a Y axis using separate graphs
  PosTwoDCoord		graph_layout; 	// arrangement of graphs for separate graphs
  GraphletSpec_List	graphlets; // list of list of linked axes, describing 1 or more simultaneous graphs

  //following were previously in GraphEditor in v3.2
  int			last_col_cnt; 	// no. of columns we had last time we updated
  int			last_row_cnt; 	// no. of rows we had (in x axis) last time we updated

  int			view_bufsz; 	// maximum number of lines of data to view
  MinMaxInt		view_range; 	// range of data to view
  int			last_pt_offset;	// where to get the last point of data from for AddLastPt function (0 = actual last point, higher #'s go further back..)

  ColorScale*		scale;		// internal colorscale

  bool			is3d() {return (graph_type != TWOD);}
  bool			isUpdatingChildren() {return updating_children;}

  virtual void		AssertGraphlets();
  virtual YAxisSpec*	FindYAxis(GraphLineSpec* spec);
    // given a col spec, find an associated axis
  virtual YAxisSpec*	MakeYAxis(GraphLineSpec* spec);
    // given a col spec, make an associated axis -- should not already exist for spec
  virtual YAxisSpec*	FindMakeYAxis(GraphLineSpec* spec);
    // given a col spec, find and config associated axis, else make axis
  virtual char*	ColorName(int color_no);
  // #MENU #MENU_ON_Actions #USE_RVAL gets color name for given line number for color_type
  virtual int 	ColorCount();		 // number of colors in palatte
  virtual void	SetBgColor();
  // sets background color based on color type
  virtual void	UpdateLineFeatures(bool visible_only=true);
  // #MENU #MENU_ON_Actions apply specified sequences to update line features
  virtual void	ApplyOneFeature(GraphLineSpec* dagv, SequenceType seq, int val,
				bool& set_ln, bool& set_pt);
  // #IGNORE

  virtual void	SetLineWidths(float line_width);
  // #MENU set the line widths of all lines to this value
  virtual void	SetLineType(GraphLineSpec::LineType line_type);
  // #MENU set all line types to given type

  virtual void	ShareAxes();
  // #MENU #MENU_SEP_BEFORE #CONFIRM make all columns share the same Y axis (first axis in group)
  virtual void	SeparateAxes();
  // #MENU #CONFIRM each column of data gets its own Y axis
  virtual void	CustomAxes();
  // #MENU #CONFIRM distribute axes in a user specified pattern

  virtual void	StackTraces();
  // #MENU #CONFIRM arrange subsequent traces of data (pass through the same X axis values) in non-overlapping vertically-arranged stacks
  virtual void	UnStackTraces();
  // #MENU #CONFIRM subsequent traces of data (pass through the same X axis values) are plotted overlapping on top of each other
  virtual void	StackSharedAxes();
  // #MENU #CONFIRM arrange lines that share the same Y axis in non-overlapping vertically-arranged stacks
  virtual void	UnStackSharedAxes();
  // #MENU #CONFIRM lines that share the same Y axis are plotted overlapping on top of each other

  override void		ReBuildFromDataTable();
  virtual void		ReBuildAxes();
    // builds or rebuilds axes when cols change (called from BFDT and RBFDT)

  virtual void	UpdateAfterEdit_ColSpec(GraphLineSpec* cs); // delegated from the ColSpec
  virtual void	UpdateAfterEdit_AxisSpec(AxisSpec* as); // delegated from the AxisSpec

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const GraphSpec& cp);
  COPY_FUNS(GraphSpec, inherited);
  TA_BASEFUNS(GraphSpec);
protected:
  int 			updating_children; // non-zero when we are in the UAE called from children
  void 			ReBuildAxes_DoPlotRows();
  bool			ReBuildAxes_GetXZAxis(AxisSpec::Axis axis, AxisSpec& axis,
    bool force = true); // called from RebuildAxes to get an X or Z axis
  virtual void		UpdateLineFeatures_impl(bool visible_only); // #IGNORE
};

/*
class TA_API Graph: public taNBase {
  // represents one entire graph object, including axes
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  static float_Data* 	GetYData(GraphLineSpec* spec, String_Data*& str_ar); // get y-axis data array for this column (and string array if string)
  static void		SetAxisFromSpec(AxisSpec* ax, GraphLineSpec* spec);

  Axis_List		axes;	      	// all the axes (x and y) -- note: autoset when added to children
  int			x_axis_index;	// index of xaxis in array; -1 if none/not found
  XAxis*		x_axis;		// (cached) x_axis
  float_Data*		x_axis_ar; 	// (cached) data array for the x axis

  float_RArray		rows_x_axis; 	// x axis values for plot rows mode

  void			(*SetOwnerXAxis)(TAPtr, int);

  virtual void		Configure();
//  virtual void		SetXAxis(XAxis* axis);  // set x axis
  virtual void		UpdateAxes(); // makes or updates all the axes

  override void		InitLinks();
  override void		CutLinks();
  TA_BASEFUNS(Graph)
protected:
//  virtual void		AddYAxis(YAxis* axis);  // add a y axis to this graph
private:
  void			Initialize();
  void			Destroy() {CutLinks();}
};

*/
#endif // datagraph_h
