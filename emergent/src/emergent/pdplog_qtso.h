// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// pdplog_qtso.h : Qt and Inventor stuff for pdp logs

#ifndef PDPLOG_QTSO_H
#define PDPLOG_QTSO_H

#include "ta_qttype.h"
#include "t3viewer.h"

#include "datagraph.h"
#include "pdp_def.h"
#include "pdp_TA_type.h"

// externals
class GraphViews;
class iGraphButtons; // #IGNORE
class T3GraphTableViewNode;

// forwards
class iGraphTableView_Panel;


/*TODO: rework so doesn't need an adapter
class PDP_API GridTableViewAdapter: public TableViewAdapter {
  Q_OBJECT
public:
  GridTableViewAdapter(GridTableView* owner_): TableViewAdapter(owner_) {}
public slots:
  void 	ColorBar_execute() {if(owner) ((GridTableView*)owner)->ColorBar_execute();}
};*/


class PDP_API GraphTableView : public TableView {
  // #INSTANCE View log data as a graph of lines
INHERITED(TableView)
public:
  static GraphTableView* NewGraphTableView(DataTable* dt, T3DataViewFrame* fr); // creates graph
  
  int			x_axis_index;	// index of index in array
  MinMaxInt		actual_range;	// #HIDDEN #NO_SAVE range in actual lines of data
  ColorScaleSpec* 	colorspec; 	// The color spectrum for this display (for TRACE_COLOR or VALUE_COLOR line displays)
  bool			separate_graphs; // draw each group of lines sharing a Y axis using separate graphs
  PosTwoDCoord		graph_layout; 	// arrangement of graphs for separate graphs
  bool			anim_stop; 	// #IGNORE
  GraphViews*		graphs; // #READ_ONLY #NO_SAVE
  iGraphTableView_Panel*	lvp(){return (iGraphTableView_Panel*)m_lvp;}
  T3GraphTableViewNode* 	node_so() const;
  GraphSpec*  	viewSpec() {return (GraphSpec*)viewspec;}

  virtual void	Animate(int msec_per_point = 500);
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE animate the display by incrementally displaying each new row of data, waiting given amount of time between points.  This will capture images to animation files if anim.capture is on.
  override void		BuildAll();
  virtual void	StopAnimate();
  // #MENU stop the animation!

  virtual void 	SetColorSpec(ColorScaleSpec* colors);
  // #MENU #MENU_ON_Settings #NULL_OK set the color spectrum to use for color-coding values (NULL = use default)
  virtual void 	SetBackground(RGBA background);
  // #MENU set the display's background to the given color
  virtual void	UpdateLineFeatures();
  // #MENU #CONFIRM update color, line type, point type, etc of lines in accordance with the current settings for the ordering of these features.  Only visible lines are updated
  virtual void	SetLineFeatures
  (GraphSpec::ColorType color_type,
   GraphSpec::SequenceType sequence1=GraphSpec::COLORS,
   GraphSpec::SequenceType sequence2=GraphSpec::LINES,
   GraphSpec::SequenceType sequence3=GraphSpec::POINTS,
   bool update_only_visible=true);
  // #MENU set color, line type, point type of lines by cycling through values

  virtual void	SetLineWidths(float line_width);
  // #MENU #MENU_SEP_BEFORE set the widths of all lines in the graph to given value
  virtual void	SetLineType(GraphColSpec::LineType line_type);
  // #MENU set all line types to given type

  virtual void	ShareAxisAfter(taBase* axis_var);
  // #MENU #MENU_SEP_BEFORE #FROM_GROUP_viewspec make all displayed variables after given axis_var share Y axis with axis_var
  virtual void	ShareAxes();
  // #MENU #CONFIRM make all groups of columns share the same Y axis
  virtual void	SeparateAxes();
  // #MENU #CONFIRM each column of data gets its own Y axis

  virtual void	SeparateGraphs(int x_layout, int y_layout);
  // #MENU #MENU_SEP_BEFORE draw each group of lines sharing the same Y axis using separate graphs, with the given layout of graphs in the display
  virtual void	OneGraph();
  // #MENU #CONFIRM draw all data in one graph (default mode)

  virtual void	StackTraces();
  // #MENU #CONFIRM arrange subsequent traces of data (pass through the same X axis values) in non-overlapping vertically-arranged stacks
  virtual void	UnStackTraces();
  // #MENU #CONFIRM subsequent traces of data (pass through the same X axis values) are plotted overlapping on top of each other
  virtual void	StackSharedAxes();
  // #MENU #CONFIRM arrange lines that share the same Y axis in non-overlapping vertically-arranged stacks
  virtual void	UnStackSharedAxes();
  // #MENU #CONFIRM lines that share the same Y axis are plotted overlapping on top of each other

  virtual void	SpikeRaster(float thresh);
  // #MENU #MENU_SEP_BEFORE display spike rasters with given threshold for cutting a spike (trace_incr.y = 1, vertical = NO_VERTICAL, line_type = TRESH_POINTS)
  virtual void	ColorRaster();
  // #MENU display values as rasters of lines colored according to the values of the lines
  virtual void	StandardLines();
  // #MENU get rid of raster-style display and return to 'standard' graph line display

  void  	NewHead();
  void  	NewData();
  virtual int	SetXAxis(const String& nm); // set the x axis to be this field
  virtual void	UpdateViewRange();  // update view range info from log

  TypeDef*	DT_ViewSpecType(); // #IGNORE get type of DataTable for this log
  TypeDef*	DA_ViewSpecType(); // #IGNORE get type of DataArray for this log

  void		InitColors();
  void 		GetBodyRep();	// #IGNORE
//  void		CloseWindow();
  void 		UpdateDisplay(TAPtr updtr=NULL);
  void 		InitDisplay();

  override void		Log_Clear(); // called by log in its Clear()

  QWidget*	GetPrintData();	// #IGNORE

  void 	UpdateAfterEdit();
  void	Initialize();
  void  Destroy();
  void	CutLinks();
  void 	InitLinks();
  void	Copy_(const GraphTableView& cp);
  T3_DATAVIEWFUNS(GraphTableView, LogtView)

protected:
  override void 	ChildAdding(T3DataView* child);
  override void 	ChildRemoving(T3DataView* child);
  override void		Clear_impl();
  override void		OnWindowBind_impl(iT3DataViewFrame* vw);
  override void		Render_impl(); // #IGNORE
  override void		Render_pre(); // #IGNORE
  override void		UpdateFromBuffer_impl();
};



class PDP_API iGraphTableView_Panel: public iTableView_Panel {
  Q_OBJECT
INHERITED(iTableView_Panel)
public:
  QVBoxLayout*		layToolGraphButtons;// layout on left of graph
//  QLayout*		  layToolButtons;// layout containing tool buttons
  QVBoxLayout*		  layGraphButtons;// layout containing tool buttons
  QLabel*		    lblGraphButtons;
  iGraphButtons*	    gbs;//  graph buttons

  override String	panel_type() const; // this string is on the subpanel button for this panel
  GraphTableView*		glv() {return (GraphTableView*)m_dv;}

//  override int 		EditAction(int ea);
//  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)

  iGraphTableView_Panel(GraphTableView* glv);
  ~iGraphTableView_Panel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iGraphTableView_Panel;}
protected:
  override void 	GetImage_impl();
  override void		InitPanel_impl(); // called on structural changes
//  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

//protected slots:
//  void			list_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col);
//  void			list_selectionChanged(); //note: must use this parameterless version in Multi mode
};


#endif


