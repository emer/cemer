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

#ifndef GraphTableView_h
#define GraphTableView_h 1

// parent includes:
#include <DataTableView>

// member includes:
#include <GraphAxisView>
#include <GraphPlotView>
#include <ColorScale>
#include <taVector3f>
#include <taMisc>
#include <int_Array>

// declare all other types mentioned but not required to include:
class DataTable; // 
class ColorScaleSpec; // 
class iViewPanelOfGraphTable; // 
class T3Panel; //
class T3GraphViewNode; //
class T3GraphLine; //


taTypeDef_Of(GraphTableView);

class TA_API GraphTableView: public DataTableView {
  // the master view guy for entire graph view
INHERITED(DataTableView)
public:
  static GraphTableView* New(DataTable* dt, T3Panel*& fr);

  enum GraphType {
    XY,                         // standard XY(Z) plot -- plot value determines Y axis coordinate to plot (optional error bars as well, if turned on)
    BAR,                        // gar graph -- for integer/nominal X axis values (optional error bars as well, if turned on)
    RASTER,                     // raster plot (flat lines stacked up using raster_axis), typically with color representing plot data value or thresholded lines (spike raster)
    MATRIX,                     // plots all the values in a single matrix Y column -- see MatrixMode for options (a grid or depth plot)
  };

  enum PlotStyle {
    LINE,                       // just a line, no points
    POINTS,                     // just points, no line
    LINE_AND_POINTS,            // both line and points
    THRESH_LINE,                // draw a line when value is over threshold
    THRESH_POINT,               // draw a point when value is over threshold
  };

  enum PointSize {
    SMALL,
    MEDIUM,
    LARGE,
  };

  enum ColorMode {
    FIXED_COLOR,                // use the color specified in the plot view (shown in EXPERT mode)
    VALUE_COLOR,                // the data value determines the data drawing color, looked up on the color scale
    COLOR_AXIS,                 // use the data column specified by the color_axis to determine the drawing color
  };

  enum MatrixMode {             // how to display matrix grid data
    SEP_GRAPHS,                 // each value in the matrix gets a separate graph, with graphs configured in the same layout as the matrix
    Z_INDEX,                    // values in the matrix are drawn in the same graph, arrayed in depth along the z axis
  };

  static const int      max_plots = 8; // maximum number of y axis data plots (fixed by plot_x guys below)
  // IMPORTANT: must sync one in panel to be same as this one!!

  GraphType             graph_type;     // type of graph to draw
  PlotStyle             plot_style;     // how to plot the data
  bool                  negative_draw;  // continue same line when X value resets in negative axis direction?
  bool                  negative_draw_z; // continue same line when Z value resets in negative axis direction?
  float                 line_width;     // width of line -- 0 means use default
  float                 dev_pix_ratio;  // #NO_SAVE multiplier on display device -- for multiplying line_width 
  PointSize             point_size;     // size of point symbols
  int                   point_spacing;  // #CONDEDIT_OFF_plot_style:LINE #MIN_1 how frequently to display point markers
  float                 bar_space;      // #DEF_0.2 amount of space between bars
  float                 bar_depth;      // #DEF_0.01 how deep to make the 3d bars
  int                   label_spacing;  // how frequently to display text labels of the data values (-1 = never); if plotting a string column, the other data column (e.g. plot_2) is used to determine the y axis values
  float                 width;          // how wide to make the display (height is always 1.0)
  float                 depth;          // how deep to make the display (height is always 1.0)

  float                 axis_font_size; // #DEF_0.05 size to render axis text
  static float          tick_size;      // #DEF_0.05 size of tick marks
  float                 label_font_size;// #DEF_0.04 size to render value/string labels

  GraphAxisView         x_axis;         // the x axis (horizontal, left-to-right)
  GraphAxisView         z_axis;         // the z axis (in depth, front-to-back)
  GraphPlotView         plot_1;         // column of data to plot (optional)
  GraphPlotView         plot_2;         // column of data to plot (optional)
  GraphPlotView         plot_3;         // column of data to plot (optional)
  GraphPlotView         plot_4;         // column of data to plot (optional)
  GraphPlotView         plot_5;         // column of data to plot (optional)
  GraphPlotView         plot_6;         // column of data to plot (optional)
  GraphPlotView         plot_7;         // column of data to plot (optional)
  GraphPlotView         plot_8;         // column of data to plot (optional)

  GraphPlotView         err_1;          // data for error bars for plot values
  GraphPlotView         err_2;          // data for error bars for plot values
  GraphPlotView         err_3;          // data for error bars for plot values
  GraphPlotView         err_4;          // data for error bars for plot values
  GraphPlotView         err_5;          // data for error bars for plot values
  GraphPlotView         err_6;          // data for error bars for plot values
  GraphPlotView         err_7;          // data for error bars for plot values
  GraphPlotView         err_8;          // data for error bars for plot values
  int                   err_spacing;    // #CONDEDIT_ON_graph_type:XY_ERR spacing between
  float                 err_bar_width;  // half-width of error bars, in view plot units

  ColorMode             color_mode;     // how to determine the colors to draw
  GraphAxisView         color_axis;     // #CONDEDIT_ON_color_mode:COLOR_AXIS color axis, for determining color of lines when color_mode = COLOR_AXIS
  ColorScale            colorscale;     // contains current min,max,range,zero,auto_scale

  GraphAxisView         raster_axis;    // #CONDEDIT_ON_graph_type:RASTER raster axis, if doing a raster plot
  float                 thresh;         // #CONDEDIT_ON_plot_style:THRESH_LINE,THRESH_POINT threshold on raw data value for THRESH_LINE or THRESH_POINT plotting sytles
  float                 thr_line_len;   // #CONDEDIT_ON_plot_style:THRESH_LINE,THRESH_POINT length of line to draw when above threshold: value is subtracted and added to current X value to render line

  MatrixMode            matrix_mode;    // how to display matrix data for MATRIX mode (note that if a matrix column is selected, it is the only thing displayed)
  taMisc::MatrixView    mat_layout;     // #CONDEDIT_ON_matrix_mode:SEP_GRAPHS #DEF_BOT_ZERO layout of matrix graphs for SEP_GRAPHS mode
  bool                  mat_odd_vert;   // #CONDEDIT_ON_matrix_mode:SEP_GRAPHS how to arrange odd-dimensional matrix values (e.g., 1d or 3d) -- put the odd dimension in the Y (vertical) axis (else X, horizontal)

  bool                  two_d_font;     // #DEF_true use 2d font (easier to read, but doesn't scale) instead of 3d font
  float                 two_d_font_scale; // #DEF_350 how to scale the two_d font relative to the computed 3d number

  String                last_sel_col_nm; // #READ_ONLY #SHOW #NO_SAVE column name of the last selected point in graph to view values (if empty, then none)
  taVector3f            last_sel_pt;    // #READ_ONLY #SHOW #NO_SAVE values of last selected point

  bool          scrolling_;     // #IGNORE currently scrolling (in scroll callback)

  override void InitDisplay(bool init_panel = true);
  override void UpdateDisplay(bool update_panel = true);

  void          SetColorSpec(ColorScaleSpec* color_spec);
  // #BUTTON #VIEWMENU #INIT_ARGVAL_ON_colorscale.spec set the color scale spec to determine the palette of colors representing values

  virtual void  SetScrollBars();   // set scroll bar values

  ///////////////////////////////////////////////////
  // misc housekeeping

  void          FindDefaultXZAxes();
  // set X and Z axis columns to user data spec or the last INT columns -- if that doesn't work, then choose the first numeric columns
  void          FindDefaultPlot1();
  // set plot_1 as first float/double column (or user data spec)
  void          InitFromUserData();
  // set initial settings based on user data in columns and overall table

  ///////////////////////////////////////////////////
  // view button/field callbacks

  void          setWidth(float wdth);
  void          setScaleData(bool auto_scale, float scale_min, float scale_max);
  // updates the values in us and the stored ones in the colorscale list


  iViewPanelOfGraphTable*        lvp();
  // #IGNORE
  
  inline T3GraphViewNode* node_so() const {return (T3GraphViewNode*)inherited::node_so();}

  override void         DataUnitsXForm(taVector3f& pos, taVector3f& size);

  override const iColor bgColor(bool& ok) const; // #IGNORE
  override void         UpdateName();
  override const String caption() const;

  override bool         hasViewProperties() const { return true; }

  virtual void          CopyFromView(GraphTableView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

  void  InitLinks();
  void  CutLinks();
  SIMPLE_COPY(GraphTableView);
  T3_DATAVIEWFUNS(GraphTableView, DataTableView)

public:
  // NOTE: following are for read-only access only!!

  int                   n_plots;                // #IGNORE number of active plots
  GraphPlotView*        all_plots[max_plots];   // #IGNORE just pointers to all the plot_ guys for ease
  GraphPlotView*        all_errs[max_plots];    // #IGNORE just pointers to all the err_ guys for ease
  int_Array             main_y_plots;           // #IGNORE indicies of active guys using main y axis
  int_Array             alt_y_plots;            // #IGNORE indicies of active guys using alt y axis

protected:
  GraphPlotView*        mainy;                  // main y axis guy (first main_y_plots)
  GraphPlotView*        alty;                   // alt y axis guy (first alt_y_plots)
  int                   first_mat;              // which is the matrix guy to plot

  float                 bar_width; // width of bar for bar charts
  T3Axis*               t3_x_axis;
  T3Axis*               t3_x_axis_top; // tick-only top version of x
  T3Axis*               t3_x_axis_far;
  T3Axis*               t3_x_axis_far_top;
  T3Axis*               t3_y_axis;
  T3Axis*               t3_y_axis_rt; // tick-only rt version of y (if not 2nd y)
  T3Axis*               t3_y_axis_far;
  T3Axis*               t3_y_axis_far_rt;
  T3Axis*               t3_z_axis;
  T3Axis*               t3_z_axis_rt;
  T3Axis*               t3_z_axis_top;
  T3Axis*               t3_z_axis_top_rt;

  ///////////////////////////////////////////////////
  //    Rendering

  virtual void          RenderGraph();
  virtual void          RenderGraph_XY();
  virtual void          RenderGraph_Bar();
  virtual void          RenderGraph_Matrix_Sep();
  virtual void          RenderGraph_Matrix_Zi();

  virtual void          ComputeAxisRanges();
  // compute range information based on data column, call UpdateRange_impl

  virtual const iColor GetValueColor(GraphAxisBase* ax_clr, float val);
  // get color from value using given axis

  virtual void          RenderAxes();
  virtual void          RenderLegend();
  virtual void          RenderLegend_Ln(GraphPlotView& plv, T3GraphLine* t3gl);

  virtual void          RemoveGraph(); // remove all lines

  virtual void          PlotData_XY(GraphPlotView& plv, GraphPlotView& erv,
                                    GraphPlotView& yax,
                                    T3GraphLine* t3gl, int mat_cell = -1, 
                                    int clr_idx = -1);
  // plot XY data from given plot view column (and err view column) into given line, using given yaxis values, if from matrix then mat_cell >= 0), and color override if clr_idx >= 0
  virtual void          PlotData_Bar(SoSeparator* gr1, GraphPlotView& plv,
                                     GraphPlotView& erv, GraphPlotView& yax,
                                     T3GraphLine* t3gl, float bar_off, int mat_cell = -1,
                                     int clr_idx = -1);
  // plot bar data from given plot view column (and err view column) into given line, using given yaxis values, if from matrix then mat_cell >= 0), and color override if clr_idx >= 0
  virtual void          PlotData_String(GraphPlotView& plv_str, GraphPlotView& plv_y,
                                        T3GraphLine* t3gl);
  // plot string data from given plot view column using Y values from given Y column

  override void         OnWindowBind_impl(iT3Panel* vw);
  override void         Clear_impl();
  override void         Render_pre(); // #IGNORE
  override void         Render_impl(); // #IGNORE
  override void         Render_post(); // #IGNORE

  override void         UpdateFromDataTable_this(bool first);
  override void         SigRecvUpdateView_impl();

  override void         UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy() {CutLinks();}
};

#endif // GraphTableView_h
