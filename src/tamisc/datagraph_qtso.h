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



#ifndef DATAGRAPH_QTSO_H
#define DATAGRAPH_QTSO_H


#include "ta_qttype.h"
#include "colorbar_so.h"
#include "datagraph.h"
#include "t3viewer.h"

#include "safeptr_so.h"
#include "datagraph_so.h"

//#include "graphic.h"
//#include "graphic_viewer.h"

#ifndef __MAKETA__
#include <qsizepolicy.h>
#include <qwidget.h>
#endif

// forwards this file
class AxisView;
class GraphLine; //
class GraphView;
class GraphViews; //
class iGraphButton; // #IGNORE
class iGraphButtons; // #IGNORE

/*
 * AxisView - position tick marks as on axis
 */


class TA_API AxisView : public T3DataView { // an axis on a graph -- note: mdata is the AxisSpec
#ifndef __MAKETA__
typedef T3DataView inherited;
#endif
public:
  enum Axis { //note: s only be X,Y,Z, same as T3Axis::Axis
    X,
    Y,
    Z
  };

  static AxisView* 	New(AxisSpec* axis_spec);

  float			axis_length; // #NO_SAVE in view units
  float			start_tick;	// #NO_SAVE first tick mark here
  float       		tick_incr;	// #NO_SAVE increment for tick marks
  int			act_n_ticks;	// #NO_SAVE actual number of ticks
  double		units;		// #NO_SAVE order of the units displayed (i.e. divide by this)
  float			range_scale;	// re-scaling of range over actual based on zooming
  float			min_percent;	// percent of new (zoomed) range above min
  float			max_percent;	// percent of new (zoomed) range below max

  Axis		axis(); // determined in Render_pre, based on axis_spec.spec

  AxisSpec*		axis_spec() {return (AxisSpec*)m_data;}
  T3Axis*		node_so() const {return (T3Axis*)m_node_so.ptr();}

  virtual void		ComputeTickIncr();
  void			DataToPoint(float data, float& pt);
    // converts a data value to its corresponding graphical coord (relative to origin)
  bool 			InitUpdateAxis(bool init);
    // checks if needs update, updates and returns true if so -- only called for Y axes

  virtual float 	GetMin();	// get effective min value (includes min_percent)
  virtual float 	GetMax();	// get effective max value (includes max_percent)
  virtual float		GetRange() 	{ return GetMax() - GetMin(); }

  virtual void 		UpdateAxis(); // action of Render_impl  and ReInit_impl-- can also be called to redraw axis


  void Copy_(const AxisView& cp);
  COPY_FUNS(AxisView, T3DataView);
  T3_DATAVIEWFUNS(AxisView, T3DataView)
protected:
  void 			UpdateAxis_X(T3Axis* t3ax, AxisSpec* as); //#IGNORE
  void 			UpdateAxis_Y(T3Axis* t3ax, AxisSpec* as); //#IGNORE
  void 			UpdateAxis_Z(T3Axis* t3ax, AxisSpec* as); //#IGNORE
//  void 			ReInit_impl(); NOTE: owner-graphview/s deals with this
  override void 	Render_pre(); //
  override void 	Render_impl(); //
private:
  void			Initialize();
  void			Destroy();
};

class TA_API GraphLine: public T3DataView { // a line on a graph -- mdata is the colspec -- lines are deleted if any of their AxisView refs get deleted
#ifndef __MAKETA__
typedef T3DataView inherited;
#endif
public:
  static GraphLine*	New(GraphLineSpec* data);

  iVec3f	last_data;	// #IGNORE values of last data point plotted; 0=init value
  int		last_da_idx; 	// #IGNORE index of last data array point plotted; -1=init value
  int		n_traces;	// #IGNORE number of traces through same X points; 1=init value
  int		trace_idx;	// #IGNORE current trace being drawn
  int		last_coord;	// #IGNORE last coordinate selected to view
  int		n_shared;	// #IGNORE number of lines sharing same Y axis
  int		share_i;	// #IGNORE index within set of shared Y axis points
//obs  FloatTwoDCoord esc;		// #IGNORE extra scaling of lines (from stacking, trace_incr)
//obs  FloatTwoDCoord eoff;		// #IGNORE extra offsets of lines (from stacking, trace_incr)
//obs  float		plwd;		// #IGNORE one line width, in data coordinates
//obs  float		plht;		// #IGNORE one line height, in data coordinates

  AxisView*		x_axis; // #READ_ONLY #SAVE
  AxisView*		y_axis; // #READ_ONLY #SAVE
  AxisView*		z_axis; // #READ_ONLY #SAVE  if 3D

  XAxisSpec*		x_axis_spec() const
    {return (&((GraphLineSpec*)m_data)->graph_spec->x_axis_spec);}	// our y axis
  YAxisSpec*		y_axis_spec() const {return (YAxisSpec*)(((GraphLineSpec*)m_data)->axis_spec);}	// our y axis
  ZAxisSpec*		z_axis_spec() const
    {return (&((GraphLineSpec*)m_data)->graph_spec->z_axis_spec);}	// our z axis (3D only)
  GraphLineSpec*		col_spec() const {return (GraphLineSpec*)m_data;}
  GraphSpec*		graph_spec() const {return ((GraphLineSpec*)m_data)->graph_spec;}
  T3GraphLine*		node_so() const {return (T3GraphLine*)m_node_so.ptr();}

  void			DataToPoint(const iVec3f& data, iVec3f& pt);
   // convert from a data values to graph coords

  void			Redraw(); // #MENU  #MENU_ON_Actions called when axes have changed, or at beginning -- redraws everything in viewrange

  virtual bool		DrawLastPoint(); // draw last point added to line: returns true if needs full redraw (ex. new trace detected)

  virtual const iColor* GetValueColor(float val);	// get color from value
  virtual const iColor* GetTraceColor(int trace);	// get color from trace count

  // use this interface instead of add_point to add points to the line: updates relevant stuff
/*TODO  void		draw_gs(ivCanvas* c, Graphic* gs);
  void 		drawclipped_gs(ivCanvas* c, ivCoord l, ivCoord b,
			       ivCoord r, ivCoord t, Graphic* gs);

  bool		grasp_move(const ivEvent&, Tool&,ivCoord ix, ivCoord iy);
  bool		manip_move(const ivEvent&, Tool&,ivCoord ix, ivCoord iy,
			   ivCoord lx, ivCoord ly,
			   ivCoord cx, ivCoord cy);
  bool		effect_move(const ivEvent&, Tool&,ivCoord ix,
			    ivCoord iy, ivCoord fx, ivCoord fy);

  bool		grasp_scale(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,ivCoord,
			    ivCoord,ivCoord) {return true;}
  bool		manip_scale(const ivEvent&,Tool&,ivCoord,ivCoord,ivCoord,ivCoord,
			    ivCoord,ivCoord,ivCoord,ivCoord) {return true;}
  bool 		effect_border(); */

  T3_DATAVIEWFUNS(GraphLine, T3DataView)
protected:
  bool			val_color_mode; // true whenever entire line/pointset might not be same color


  override void		ReInit_impl();	// clear existing points/markers etc.

  override void 	Render_pre(); //
  override void 	Render_impl(); //

  // note: routines below are internal, so we pass many of the cached/decoded objects

  bool			RenderPoint(int row_num); // common subcode -- true if requires redraw

protected: // all the following values are only valid during RenderPoint, and only for internal use
  DataArray_impl* 	da_x;
  DataArray_impl* 	da_y; // note: for STRINGS, this is the plot col, not the string data
  DataArray_impl* 	da_z;
  DataArray_impl* 	da_string; // only set for STRINGS mode, otherwise null

private:
  void			Initialize();
  void			Destroy();
};

/*nn class TA_API GraphViewer : public GlyphViewer {
protected:
  void rate_zoom();
  void grab_scroll();
public:
  GraphEditor* 	editor;
  GraphView*	graph;
  ivTransformer* init_xform;	// cached root transform for scaling axes

//TODO  GraphicMaster* root() { return GlyphViewer::root(); }
  void root(GraphicMaster*);

  virtual void 	ReScaleAxes(bool redraw=true);  // updates axes scales based on what is displayed

  GraphViewer(GraphEditor* e, GraphView* gg, float w, float h, const iColor* bg);
  ~GraphViewer();
}; */

class TA_API GraphView : public T3DataViewPar {
  // represents one entire graph object, including axes and lines -- mdata is the graphlet list
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
friend class GraphViews;
public:
  static GraphView*	New(GraphletSpec* data);
  
  DataTableRef		data_table;	// the data itself

  AxisView*		x_axis;		// #READ_ONLY #NO_SAVE the x axis
  AxisView*		z_axis;		// #READ_ONLY #NO_SAVE the z axis (3D only)
  bool			nodisp_x_axis; 	// do not display x axis
  T3DataView_PtrList	y_axes;	      	// #READ_ONLY #NO_SAVE the y axis(s) -- note: autoset when added to children
  T3DataView_PtrList	lines;	      	// #READ_ONLY #NO_SAVE the lines -- note: autoset when added to children
  iVec3f 		origin;		// #READ_ONLY #NO_SAVE origin of lines, in our container's units
  FloatTDCoord		geom;		// #READ_ONLY #NO_SAVE size, in Graph units

  GraphletSpec*		graphlet() {return (GraphletSpec*)m_data;}
  GraphSpec*		graph_spec();
  T3Graph*		node_so() const {return (T3Graph*)m_node_so.ptr();}

  override void		BuildAll();  // generate initial data members (axes)

  void 			Redraw(); //  #MENU  #MENU_ON_Actions
  override void		InitLinks();
  override void		CutLinks();
  T3_DATAVIEWFUNS(GraphView, T3DataViewPar)
protected:
  GraphSpec*	m_graph_spec; // cache

  bool			AddLastPoint(); // called when GraphViews thinks we can just add the point; returns true if redraw situation was detected and Redraw was called
  void			RedrawLines(); // called when we should redraw the lines -- axes must be current
  bool			InitUpdateYAxes(bool init); // update y axes, returns true if any was updated

  override void 	ChildAdding(taDataView* child);
  override void 	ChildRemoving(taDataView* child);
  override void 	ReInit_impl();
  override void		Reset_impl(); //
  override void 	Render_pre(); //
  override void		Render_impl();
private:
  void			Initialize();
  void			Destroy();
};

class TA_API GraphViews : public T3DataViewPar {
  // container to hold one (single view) or more (multi view) graphs (.children contains GraphView objs)
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
public:
  static GraphViews*	New(GraphSpec* data);

  GraphSpec*		graph_spec() {return (GraphSpec*)m_data;}
  T3NodeParent*		node_so() {return (T3NodeParent*)m_node_so.ptr();}
  T3CBar*		bar_so() {return (T3CBar*)m_bar_so.ptr();} //note: only created if needed

  PosTwoDCoord		eff_layout; 	// effective arrangement of graphs for separate graphs
  FloatTDCoord		geom;		// #READ_ONLY #NO_SAVE size, in PDP units
  T3DataView_PtrList	graphs; // #IGNORE cached list of GraphView children

  override void		BuildAll(); // create all the graphs


  bool			AddLastPoint(); // call to just add most recent data point; returns true if redraw is needed instead
  void			Redraw(); // call to completely redraw all lines/axes in the viewspec.viewrange

  override void		InitLinks();
  override void		CutLinks();
  T3_DATAVIEWFUNS(GraphViews, T3DataViewPar)
protected:
  T3NodePtr		m_bar_so; // color bar, if used

  bool			InitUpdateXZAxes(bool init); // update x/z axes, returns true if x or z was updated

  override void 	ChildAdding(taDataView* child);
  override void 	ChildRemoving(taDataView* child);
  override void		Clear_impl(); //
  override void 	ReInit_impl(); // note: we deal with the x and z axes below us
  override void		Reset_impl(); //
  override void 	Render_pre(); //
  override void 	Render_impl(); //
private:
  void			Initialize();
  void			Destroy();
};

class iAxisButton; // #IGNORE QPushbutton-derivitive defined in .cc file

class TA_API iGraphButton : public QWidget {
  // #NO_CSS #NO_INSTANCE the button is used as a gui for each column of data
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
friend class iGraphButtons;
friend class iAxisButton;
  Q_OBJECT
public:
  iGraphButtons*	parent; // set when added
  GraphLineSpec*	line;		// the column for this field (almost always exists)
  AxisSpec*	axis;		// axis for this field, if it is an axis, else NULL

  void			setShowLine(bool show); //called by owner to set state
  QSizePolicy   sizePolicy () const; // #IGNORE override
  QSize		sizeHint() const; // #IGNORE override

  virtual void	ShowHideLine();	// toggle line on/off
//  virtual void	SetXAxis();	// set this one to be the x axis (middle button)


  bool		IsChosen();	// is this button selected


  void		SetColAxis(int field, GraphLineSpec* line, AxisSpec* axis, bool force = false);
  // used to change the col/axis; force causes everything to be updated

  iGraphButton(int field, GraphLineSpec* col, AxisSpec* axis, QWidget* parent = NULL);
    //note: normally no parent on create
  ~iGraphButton();

public slots:
  void		chkShow_toggled(bool on); // process btn press
  void		btnAxis_pressed();
  void		btnLine_pressed();

protected:
  int		field;	// leaf index in table; -1 if n/a (ex. zombie z)
  iCheckBox*	chkShow;	// the actual button itself
  iAxisButton*  btnAxis; // axis button, when applicable
  QPushButton*	btnLine;  // col button, when applicable (always, except for orphan z axis)

  int 		updating; // used to suppress spurious events

  void		init();
  void		configure(); // this is called by init, and whenever col/axis changes
//  virtual void	SetAxis(); // set the righthand axis rep appropriately TODO: change to paintXxx
  void 		paintEvent(QPaintEvent* ev); // override
  void		paintLine(QPainter& p, const QRect& r); //note: p is translated
  void		paintAxis(QPainter& p, const QRect& r); //note: p is translated; axis buttons also call this
#ifndef __MAKETA__
  void		paintLegend(QPainter& p, const QRect& r, const iColor* color, String symbol,
    bool draw_line = true, Qt::PenStyle ps = Qt::SolidLine);
#endif

  void 		resizeEvent(QResizeEvent* ev);
};

/*
  iGraphButtons have columns as follows:
    Line -- indicates line color, or strings, or n/a
    Visible -- check box to control visibility (sometimes it is disabled)
    Axis -- indicates axis color and type, if col *is* an axis, this is a prop button
    Col -- button with col name, to invoke column properties
*/

class GraphButtonsDataView; // #IGNORE pvt class, taDataView

class TA_API iGraphButtons: public QWidget {
  //  #NO_CSS #NO_INSTANCE container for iGraphButtons; provides header
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
friend class GraphButtonsDataView; // internal class
  Q_OBJECT
public:

  QScrollArea*		scr; // always has vert scrolls; never hor ones
  QWidget*		  buts; //
  QVBoxLayout*		  layWidg;
  Q3Header*		    hdr;

  int			buttonCount(); // number of buttons
  iGraphButton*		button(int i); // get indicated button (NULL if out of bounds)
  GraphSpec*		graph() {return m_graph;}	// which graph do I belong in
  void			setGraph(GraphSpec* graph, bool force_rebuild = false); // sets graph, and builds

  void			AddGraphButton(iGraphButton* gb);
  void 			ButtonDeleting(iGraphButton* gb); // called by the button when deleting
  void			Rebuild(); // makes sure we are in sync with graph
  void			RemoveGraphButton(int i);
  QSizePolicy   	sizePolicy () const; // #IGNORE override
  QSize			sizeHint() const; // #IGNORE override

  iGraphButtons(QWidget* parent = NULL);
  ~iGraphButtons();
protected:
  int 			updating;
  QObjectList*		buttons;
  GraphSpec*		m_graph;		// which graph do I belong in
  GraphButtonsDataView*		dv_graph; // generic dataview for updating -- set in setGraph
  void 			resizeEvent(QResizeEvent* ev);
};


#endif // datagraph_h
