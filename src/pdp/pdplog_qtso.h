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

#include "pdplog.h"
#include "pdp_qtso.h"
#include "datagraph.h"
#include "netstru_so.h"
#include "pdp_TA_type.h"

// externals
class GraphViews;
class iGraphButtons; // #IGNORE

// forwards
class LogView;
class LogViewAdapter; // #IGNORE

class iLogView_Panel;
class iGridLogViewBase_Panel;
class iTextLogView_Panel;
class iNetLogView_Panel;
class iGridLogView_Panel;
class iGraphLogView_Panel;


//////////////////////////
//   LogView		//
//////////////////////////

class PDP_API LogView : public T3DataViewPar {
  // #VIRT_BASE #NO_TOKENS parent class of all log views (Par is only for GraphLogView)
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
friend class LogViewAdapter;
public:
  int		view_bufsz;	// Maximum number of lines in visible buffer
//obs  float		view_shift;	// precentage of buffer to shift
  MinMaxInt	view_range;	// #NO_SAVE range of visible lines (in log lines)
  DT_ViewSpec*	viewspec;	// #BROWSE #READ_ONLY #NO_SAVE global view spec for all data

  taBase_List 	viewspecs;	// #NO_SHOW dummy group list to hold the single viewspec
  bool		display_toggle;  // #DEF_true 'true' if display should be updated
//nn  TDCoord	pos;		// origin of the log in 3d space (not used for panel view)
//nn  TDCoord	geom;		// space taken
  float		frame_inset;	// #DEF_0.05 inset of frame (used to calc actual inner space avail)
//obs  PDPLog* 	log_mgr;	// #READ_ONLY #SAVE Parent Log -- must be set before InitLinks

  PDPLog*		log() const {return (PDPLog*)m_data;}
  T3LogViewNode*	node_so() const {return (T3LogViewNode*)m_node_so.ptr();}

  virtual bool	IsMapped(); // return true if any panels/T3 views are mapped

  // log interface
  virtual void  NewHead(); // new header information is in log data
  virtual void  NewData() {} // new data information is in log's datatable

  // relation to the buffer
  virtual void	UpdateFromBuffer(); // update view from buffer; note: override _impl to implement


  virtual void 	ClearData();	// Clear the display and the data

  void		InitDisplay();
  virtual void	InitPanel();// lets panel init itself after struct changes
  void  UpdateDisplay(TAPtr updtr=NULL);
  void	UpdateCallback()	{ UpdateDisplay(NULL); } // #IGNORE

  virtual void 	ToggleDisplay();
  virtual void 	SetToggle(bool value);

  void		EditViewSpec(taBase* column);
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE #FROM_GROUP_viewspec edit view specs for given column
  void		SetVisibility(taBase* column, bool visible);
  // #MENU #FROM_GROUP_viewspec set whether this column of data is displayed in the log display
  void		SetLogging(taBase* column, bool log_data, bool also_chg_vis = true);
  // #MENU #FROM_GROUP_viewspec set whether this column of data is logged to a file (if also_chg_vis, then visibility in view is changed to match logging)
  void		UpdateDispLabels();
  // #MENU #CONFIRM update the display labels in the view using the display_labels stored in the log object

  // following are analysis routines that copy log data to a temporary environment
  // and then run the associated function on that environment

  virtual void 	CopyToTable(taBase* data, taBase* labels, DataTable* dt);
  // TODO:OBS #MENU #MENU_ON_Analyze #FROM_GROUP_1_viewspec #NULL_OK output data (must be group) with labels to datatable dt (NULL = new table). WARNING: reformats table to fit data!)
/*obs
  virtual void	DistMatrixGrid(taBase* data, taBase* labels, GridLog* disp_log, float_RArray::DistMetric metric=float_RArray::HAMMING,
			   bool norm=false, float tol=0.0f);
  // #MENU #NULL_OK #FROM_GROUP_1_viewspec output to grid log (NULL=make new one) distance matrix for data (must be group) with labels
  virtual void	ClusterPlot(taBase* data, taBase* labels, GraphLog* disp_log,
			    float_RArray::DistMetric metric=float_RArray::EUCLIDIAN,
			    bool norm=false, float tol=0.0f);
  // #MENU #NULL_OK #FROM_GROUP_1_viewspec produce a cluster plot (in graph log, NULL = make a new one) of the given data (with labels)
  virtual void	CorrelMatrixGrid(taBase* data, taBase* labels, GridLog* disp_log);
  // #MENU #NULL_OK #FROM_GROUP_1_viewspec produce a correlation matrix of given data and plot in given grid log (NULL = make new one)
  virtual void	PCAEigenGrid(taBase* data, taBase* labels, GridLog* disp_log);
  // #MENU #NULL_OK #FROM_GROUP_1_viewspec plot eigenvectors of correlation matrix (principal components analysis = PCA) of given data in given grid log (NULL = make new one)
  virtual void	PCAPrjnPlot(taBase* data, taBase* labels, GraphLog* disp_log, int x_axis_component=0, int y_axis_component=1);
  // #MENU #NULL_OK #FROM_GROUP_1_viewspec plot projections of data onto two principal components (eigenvectors) into a graph log (NULL = make new one)
  virtual void	MDSPrjnPlot(taBase* data, taBase* labels, GraphLog* disp_log, int x_axis_component=0, int y_axis_component=1);
  // #MENU #NULL_OK #FROM_GROUP_1_viewspec plot multidimensional scaling of distance matrix of data onto two components into a graph log (NULL = make new one)
*/
  virtual void		CreateViewSpec(); // #IGNORE create view spec of appropriate type
  virtual TypeDef*	DT_ViewSpecType(); // #IGNORE get type of DataTable for this log
  virtual TypeDef*	DA_ViewSpecType(); // #IGNORE get type of DataArray for this log

  // winview stuff
  void			AddNotify(TAPtr ud); // #IGNORE
  void	 		RemoveNotify(TAPtr ud);	// #IGNORE
  void  		SetWinName();
//  override void		CloseWindow();
  QWidget*		GetPrintData();	// #IGNORE

  virtual void 		LogUpdateAfterEdit(); // called by log in its uae
  virtual void		Log_Clear() {} // called by log in its Clear()

  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_PDPLog); }

  void 	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const LogView& cp);
  COPY_FUNS(LogView, T3DataViewPar);
  T3_DATAVIEWFUNS(LogView) //

public:
  // view control -- row
  virtual void 	View_At(int start);	// start viewing at indicated viewrange value
  virtual void 	View_FF();	// forward to end
/*obs  virtual void 	View_F();	// forward by view_shift
  virtual void 	View_R();	// rewind by view_shift
  virtual void 	View_FSF();	// fast forward
  virtual void 	View_FSR();	// fast rewind
  virtual void 	View_FF();	// forward to end
  virtual void 	View_FR();	// rewind to begining */
  // view control -- col
  virtual void 	ViewC_At(int start) {}	// start viewing at indicated column value

protected:
  iLogView_Panel*	m_lvp; //note: will be a subclass of this, per the log type
  virtual void		InitDisplayParams() {} // called in _pre
  override void		OnWindowBind_impl(iT3DataViewer* vw);
  override void		Render_pre(taDataView* par = NULL); // #IGNORE -- we only set color of frame
  override void		Render_impl(); // #IGNORE
  override void 	Render_post();
  override void		Reset_impl();
  virtual void		UpdateFromBuffer_impl(); // update view from buffer
  virtual void		viewWin_selectionChanged(ISelectable_PtrList& sels) {} // TODO

};

class PDP_API LogViewAdapter: public taBaseAdapter { // ##IGNORE
  Q_OBJECT
public:
  LogViewAdapter(LogView* owner_): taBaseAdapter(owner_) {}
public slots:
  // view control -- rows
/*obs  void 	View_F() {if(owner) ((LogView*)owner)->View_F();}
  void 	View_R() {if(owner) ((LogView*)owner)->View_R();}
  void 	View_FSF() {if(owner) ((LogView*)owner)->View_FSF();}
  void 	View_FSR() {if(owner) ((LogView*)owner)->View_FSR();}
  void 	View_FF() {if(owner) ((LogView*)owner)->View_FF();}
  void 	View_FR() {if(owner) ((LogView*)owner)->View_FR();} */
  void	viewWin_selectionChanged(ISelectable_PtrList& sels)
    {if(owner) ((LogView*)owner)->viewWin_selectionChanged(sels);}
};

class PDP_API GridLogViewBase : public LogView {
  // #VIRT_BASE #NO_INSTANCE #NO_TOKENS common subclass of TextLogView and GridLogView
#ifndef __MAKETA__
typedef LogView inherited;
#endif
public:
  int			cols; // #IGNORE visible columns
  MinMaxInt		col_range;	// column range that is visible
  float_Array		col_widths;
  float			tot_col_widths; // total of all col widths
  float			row_height; // #IGNORE determined from font size, =xx% of font height

  T3GridLogViewBaseNode* node_so() const {return (T3GridLogViewBaseNode*)m_node_so.ptr();}

  override void  	NewHead(); // new header information is in log data
  override void  	NewData(); // new data information is in log's datatable

  void			VScroll(bool left); // scroll left or right

  override void 	ViewC_At(int start);	// start viewing at indicated column value

  override void		Log_Clear() {Render();} // called by log in its Clear()

  void	InitLinks();
  void 	CutLinks();
  void 	UpdateAfterEdit();
  void	Initialize();
  void	Destroy() {CutLinks();}
  void	Copy_(const GridLogViewBase& cp);
  COPY_FUNS(GridLogViewBase, LogView);
  T3_DATAVIEWFUNS(GridLogViewBase)

protected:
  virtual void		AdjustColView() {} // sets visible last column, and colsizes, based on .min, and geom
  virtual void		InitHead() {}
  virtual void		RenderHead() {}
  override void		Clear_impl();
  override void		Render_pre(taDataView* par = NULL); // #IGNORE
  override void		Render_post(); // #IGNORE
  override void		Reset_impl();
};

class PDP_API TextLogView : public GridLogViewBase {
  // #INSTANCE a textual view of the log data
#ifndef __MAKETA__
typedef GridLogViewBase inherited;
#endif
public:
  float		font_width;	// #IGNORE width of font spacing

  DT_ViewSpec*  viewSpec() {return (DT_ViewSpec*)viewspec;}
  iTextLogView_Panel*	lvp(){return (iTextLogView_Panel*)m_lvp;}

//obs  int		ShowMore(int newsize); // #IGNORE
//obs  int		ShowLess(int newsize); // #IGNORE

  TypeDef*	DT_ViewSpecType(); // #IGNORE get type of DataTable for this log
  TypeDef*	DA_ViewSpecType(); // #IGNORE get type of DataArray for this log

  // winview stuff
  QWidget*	GetPrintData();	// #IGNORE

  void		UpdateDisplay(TAPtr updtr=NULL);
  void 		InitDisplay();

  // gui implementation stuff
  virtual void  RemoveLine(int index);		  // remove line from scrollbox, -1 = all

  void 	UpdateAfterEdit();
  void	InitLinks();
  void 	CutLinks();
  void	Initialize();
  void	Destroy() {CutLinks();}
  T3_DATAVIEWFUNS(TextLogView)

protected:
  override void		OnWindowBind_impl(iT3DataViewer* vw);
  override void 	InitDisplayParams();
  override void		InitHead();
  override void		RenderHead();
  override void		AdjustColView(); // sets visible last column, and colsizes, based on .min, and geom
  override void 	UpdateFromBuffer_impl();
  void			UpdateFromBuffer_AddLine(int row, int buff_idx); // creates a data line
};

class PDP_API NetLogView : public LogView {
  // #INSTANCE displays log information in the network view window
#ifndef __MAKETA__
typedef LogView inherited;
#endif
public:
  Network*      network;		// Network to whose views  data is logged

  iNetLogView_Panel*	lvp(){return (iNetLogView_Panel*)m_lvp;}

  virtual void	SetNetwork(Network* net);
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE select given network as the one to update views on
  virtual void  ArrangeLabels(int cols = -1, int rows = -1, int width = 12, float left = 0.0, float top = .9);
  // #MENU arrange the display labels in the netview in a grid of rows and cols starting at given top-left position (as fraction of display size) -1 = auto-compute based on size
  virtual void	RemoveLabels();	// remove all my labels from the netview

  void 		NewHead();
  void 		NewData();
  void		UpdateDisplay(TAPtr updtr=NULL);
  void 		InitDisplay();
  void 		UpdateAfterEdit();
  TypeDef*	DA_ViewSpecType(); // #IGNORE get type of DataArray for this log

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  T3_DATAVIEWFUNS(NetLogView)

protected:
  override void		OnWindowBind_impl(iT3DataViewer* vw);
  void		UpdateFromBuffer_impl();
};

class PDP_API GridLogView : public LogView {
  // #INSTANCE displays log information as colored grids
#ifndef __MAKETA__
typedef LogView inherited;
#endif
public:
  DT_GridViewSpec::BlockFill	fill_type;
  // how the grid blocks are filled in to display their value
  ColorScaleSpec* colorspec; 	// The color spectrum for this display
  int		block_size;	// *maximum* block size -- blocks will be smaller if needed to fit
  int		block_border_size; // size of the border around the blocks
  bool		header_on;	// is the log header visible?
  bool		auto_scale;	// whether to auto-scale on values or not
  FontSpec	view_font;	// the font to use for the labels in the display
  MinMax        scale_range;	// #HIDDEN range of scalebar
  MinMaxInt	actual_range;	// #HIDDEN #NO_SAVE range in actual lines of data

  // gui implementation
//TODO  DTEditor* 	editor;		// #IGNORE
  QPushButton*	auto_sc_but;	// #IGNORE toggle auto-scale button
  QPushButton*	head_tog_but;	// #IGNORE header toggle button

  iGridLogView_Panel*	lvp(){return (iGridLogView_Panel*)m_lvp;}

  void  	NewHead();
  void  	NewData();
  void 		Clear_impl();

  TypeDef*	DT_ViewSpecType(); // #IGNORE get type of DataTable for this log
  TypeDef*	DA_ViewSpecType(); // #IGNORE get type of DataArray for this log

  // winview stuff
  void 		GetBodyRep();		// #IGNORE
  void		UpdateDisplay(TAPtr updtr=NULL);
  void 		InitDisplay();
//  void 		CloseWindow();
  QWidget*	GetPrintData();	// #IGNORE

  virtual void 	SetColorSpec(ColorScaleSpec* colors);
  // #MENU #MENU_ON_Actions #NULL_OK #MENU_SEP_BEFORE set the color spectrum to use for color-coding values (NULL = use default)
  virtual void	SetBlockFill(DT_GridViewSpec::BlockFill fill_typ=DT_GridViewSpec::COLOR);
  // #MENU set the fill style of the grid blocks
  virtual void	SetBlockSizes(int block_sz = 8, int border_sz = 1);
  // #MENU set the MAXIMUM sizes of the blocks (could be smaller), and the border space between blocks
  virtual void	UpdateGridLayout(DT_GridViewSpec::MatrixLayout grid_layout=DT_GridViewSpec::LFT_RGT_BOT_TOP);
  // #MENU #ARGC_1 arrange columns to fit without gaps, according to geometry

  virtual void	SetViewFontSize(int point_size = 10);
  // #MENU #MENU_SEP_BEFORE set the point size of the font used for labels in the display
  virtual void	AllBlockTextOn();
  // #MENU turn text on for all block displayed items
  virtual void	AllBlockTextOff();
  // #MENU turn text off for all block displayed items

  virtual void	ToggleHeader();  // toggle header on or off
  virtual void	ToggleAutoScale();  // toggle header on or off

  void 	UpdateAfterEdit();
  void  InitLinks();
  void  CutLinks();
  void	Initialize();
  void 	Destroy();
  void	Copy_(const GridLogView& cp);
  COPY_FUNS(GridLogView, LogView);
  T3_DATAVIEWFUNS(GridLogView)

public:
  void ColorBar_execute();

protected:
  override void		OnWindowBind_impl(iT3DataViewer* vw);
  override void		UpdateFromBuffer_impl();
};

class PDP_API GridLogViewAdapter: public LogViewAdapter {
  Q_OBJECT
public:
  GridLogViewAdapter(GridLogView* owner_): LogViewAdapter(owner_) {}
public slots:
  void 	ColorBar_execute() {if(owner) ((GridLogView*)owner)->ColorBar_execute();}
};


/*nnclass GraphLogViewLabel : public ViewLabel {
  // view labels for graph logs
public:
  void		GetMasterViewer();

  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_BASEFUNS(GraphLogViewLabel);
};*/

class PDP_API GraphLogView : public LogView {
  // #INSTANCE View log data as a graph of lines
#ifndef __MAKETA__
typedef LogView inherited;
#endif
public:
  int			x_axis_index;	// index of index in array
//  Graph*		graph;		// data/configuration portion of graph
//  ViewLabel_List	labels;		// misc labels in the graph view
  MinMaxInt		actual_range;	// #HIDDEN #NO_SAVE range in actual lines of data
  ColorScaleSpec* 	colorspec; 	// The color spectrum for this display (for TRACE_COLOR or VALUE_COLOR line displays)
  bool			separate_graphs; // draw each group of lines sharing a Y axis using separate graphs
  PosTwoDCoord		graph_layout; 	// arrangement of graphs for separate graphs
  bool			anim_stop; 	// #IGNORE
  GraphViews*		graphs; // #READ_ONLY #NO_SAVE
  iGraphLogView_Panel*	lvp(){return (iGraphLogView_Panel*)m_lvp;}
  T3GraphLogViewNode* 	node_so() const {return (T3GraphLogViewNode*)m_node_so.ptr();}
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
  virtual int	SetXAxis(char* nm); // set the x axis to be this field
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
  void	Copy_(const GraphLogView& cp);
  COPY_FUNS(GraphLogView, LogView);
  T3_DATAVIEWFUNS(GraphLogView)

protected:
  override void 	ChildAdding(T3DataView* child);
  override void 	ChildRemoving(T3DataView* child);
  override void		Clear_impl(taDataView* par = NULL);
  override void		OnWindowBind_impl(iT3DataViewer* vw);
  override void		Render_impl(); // #IGNORE
  override void		Render_pre(taDataView* par = NULL); // #IGNORE
  override void		UpdateFromBuffer_impl();
};



//////////////////////////
//   iLogView_DataPanel //
//////////////////////////

class PDP_API iLogView_Panel: public iViewPanelFrame {
  // abstract base for logview panels
#ifndef __MAKETA__
typedef iViewPanelFrame inherited;
#endif
  Q_OBJECT
public:
  enum ButtonIds {
    BUT_BEG_ID,
    BUT_FREV_ID,
    BUT_REV_ID,
    BUT_FWD_ID,
    BUT_FFWD_ID,
    BUT_END_ID,

    BUT_UPDATE,
    BUT_INIT,
    BUT_CLEAR
  };

  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layTopCtrls;
  QCheckBox*		    chkDisplay;
  QCheckBox*		    chkAuto; // NOTE: only created in GridLogView
  QCheckBox*		    chkHeaders; // NOTE: only created in GridLogView
  QButtonGroup*		    bgpTopButtons; // NOTE: not a widget
  QHBoxLayout*		    layVcrButtons;
  QHBoxLayout*		    layInitButtons;
  QHBoxLayout*		  layContents; // subclasses put their actual content here
  iT3ViewspaceWidget*	    t3vs; //note: created with call to Constr_T3Viewspace



/* obs graphical implementation stuff
  iButton*	disp_tog_but;	// #IGNORE display toggle button
  iButton*	frev_but;	// #IGNORE full rev
  iButton*	fsrev_but;	// #IGNORE fast rev
  iButton*	rev_but;	// #IGNORE rev
  iButton*	fwd_but;	// #IGNORE fwd
  iButton*	fsfwd_but;	// #IGNORE fast fwd
  iButton*	ffwd_but;	// #IGNORE full fwd
  iButton*	clear_but;	// #IGNORE
  iButton*	init_but;	// #IGNORE
  iButton*	update_but;	// #IGNORE
*/
//  override String	panel_type() const; // this string is on the subpanel button for this panel

  LogView*		lv() {return (LogView*)m_dv;}
  SoQtRenderArea* 	ra() {return m_ra;}
//  override int 		EditAction(int ea);
//  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)

  void 			viewAll(); // zooms to fit entire scenegraph in window

  virtual void 		BufferUpdated() {} //called when data added/removed, or view is scrolled

  iLogView_Panel(LogView* lv);
  ~iLogView_Panel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iLogView_Panel;}

protected:
  SoQtRenderArea* 	m_ra;
  SoPerspectiveCamera*	m_camera;
  SoLightModel*		m_lm;

  iLogView_Panel(bool is_grid_log, LogView* lv); // only used by GridLogView
  void 			Constr_T3ViewspaceWidget();
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);
  override void 	GetImage_impl();

protected slots:
  virtual void		buttonClicked(int id);
  virtual void 		chkDisplay_toggled(bool on);
//  void			list_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col);
//  void			list_selectionChanged(); //note: must use this parameterless version in Multi mode
private:
  void			init(bool is_grid_log);
};


//////////////////////////
// iGridLogViewBase_Panel//
//////////////////////////

class PDP_API iGridLogViewBase_Panel: public iLogView_Panel {
  Q_OBJECT
#ifndef __MAKETA__
typedef iLogView_Panel inherited;
#endif
public:
  GridLogViewBase*	lv() {return (GridLogViewBase*)m_dv;}


  override void 	BufferUpdated();

  iGridLogViewBase_Panel(GridLogViewBase* tlv);
  ~iGridLogViewBase_Panel();

public slots:
  void 			horScrBar_valueChanged(int value);
  void 			verScrBar_valueChanged(int value);

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iGridLogViewBase_Panel;}

protected:
  override void		InitPanel_impl(); // called on structural changes
};

//////////////////////////
// iTextLogView_Panel//
//////////////////////////

class PDP_API iTextLogView_Panel: public iGridLogViewBase_Panel {
  Q_OBJECT
#ifndef __MAKETA__
typedef iGridLogViewBase_Panel inherited;
#endif
public:
  override String	panel_type() const; // this string is on the subpanel button for this panel
  TextLogView*		lv() {return (TextLogView*)m_dv;}

//  override int 		EditAction(int ea);
//  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)

  iTextLogView_Panel(TextLogView* tlv);
  ~iTextLogView_Panel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iTextLogView_Panel;}

protected:
  override void		InitPanel_impl(); // called on structural changes
//  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

//protected slots:
//  void			list_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col);
//  void			list_selectionChanged(); //note: must use this parameterless version in Multi mode
};


//////////////////////////
// iNetLogView_Panel//
//////////////////////////

class PDP_API iNetLogView_Panel: public iLogView_Panel {
  Q_OBJECT
#ifndef __MAKETA__
typedef iLogView_Panel inherited;
#endif
public:
  override String	panel_type() const; // this string is on the subpanel button for this panel
  NetLogView*		nlv() {return (NetLogView*)m_dv;}

//  override int 		EditAction(int ea);
//  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)

  iNetLogView_Panel(NetLogView* nlv);
  ~iNetLogView_Panel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iNetLogView_Panel;}
protected:
//  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

};


//////////////////////////
// iGridLogView_Panel//
//////////////////////////

class PDP_API iGridLogView_Panel: public iLogView_Panel {
  Q_OBJECT
#ifndef __MAKETA__
typedef iLogView_Panel inherited;
#endif
public:
  override String	panel_type() const; // this string is on the subpanel button for this panel
  GridLogView*		glv() {return (GridLogView*)m_dv;}

//  override int 		EditAction(int ea);
//  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)

  iGridLogView_Panel(GridLogView* glv);
  ~iGridLogView_Panel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iGridLogView_Panel;}
protected:
//  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

protected slots:
protected slots:
  void 			chkAuto_toggled(bool on);
  void 			chkHeaders_toggled(bool on);
//  void			list_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col);
//  void			list_selectionChanged(); //note: must use this parameterless version in Multi mode
};


//////////////////////////
// iGraphLogView_Panel//
//////////////////////////

class PDP_API iGraphLogView_Panel: public iLogView_Panel {
  Q_OBJECT
#ifndef __MAKETA__
typedef iLogView_Panel inherited;
#endif
public:
  QVBoxLayout*		layToolGraphButtons;// layout on left of graph
//  QLayout*		  layToolButtons;// layout containing tool buttons
  QVBoxLayout*		  layGraphButtons;// layout containing tool buttons
  QLabel*		    lblGraphButtons;
  iGraphButtons*	    gbs;//  graph buttons

  override String	panel_type() const; // this string is on the subpanel button for this panel
  GraphLogView*		glv() {return (GraphLogView*)m_dv;}

//  override int 		EditAction(int ea);
//  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)

  iGraphLogView_Panel(GraphLogView* glv);
  ~iGraphLogView_Panel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iGraphLogView_Panel;}
protected:
  override void 	GetImage_impl();
  override void		InitPanel_impl(); // called on structural changes
//  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

//protected slots:
//  void			list_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col);
//  void			list_selectionChanged(); //note: must use this parameterless version in Multi mode
};

/* nn
class LogView_ViewType: public tabOViewType { // we group all together, and dynamically decide at runtime
#ifndef __MAKETA__
typedef tabOViewType inherited;
#endif
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(LogView_ViewType, tabOViewType)
protected:
//  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
}; */



#endif


