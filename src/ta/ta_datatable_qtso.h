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

#ifndef __MAKETA__
# include <QItemDelegate>
# include <QTableView>
#endif

// forwards
class TableView;
class GridTableView;

class iTableView_Panel;
class iGridTableView_Panel;

class tabDataTableViewType;
class iDataTableView;
class iDataTablePanel; //

// externals
class T3GridViewNode;

class TA_API TableView : public T3DataViewPar {
  // #VIRT_BASE #NO_TOKENS base class of grid and graph views; the data is its own embedded DataTableViewSpec
INHERITED(T3DataViewPar)
public:
  int		view_rows; 	// maximum number of rows visible
  MinMaxInt	view_range; 	// range of visible rows (max is the last row visible, not the last+1; range = view_rows-1)

  bool		display_on;  	// #DEF_true 'true' if display should be updated

  FloatTDCoord	table_pos;	// position of table in view
  FloatTDCoord	table_scale;	// scaling factors of table in view
  FloatRotation	table_orient;	// orientation of table in view
  
  virtual const String	caption() const; // what to show in viewer
  virtual DataTable*	dataTable() const {return viewSpecBase()->dataTable();}
    //note: can override for more efficient direct reference

  void			setDataTable(DataTable* dt); // convenience, for building
  void			setDisplay(bool value); // use this to change display_on
  override void		setDirty(bool value); // set for all changes on us or below
  inline int		rows() const {return m_rows;}
  virtual DataTableViewSpec* viewSpecBase() const 
    {return (DataTableViewSpec*)data();}
  bool			isVisible() const; // gui_active, mapped and display_on

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

  virtual void		DataChanged_DataTable(int dcr, void* op1, void* op2);
  // forwarded when DataTable notifies; forwards to correct handler

  virtual void 		ClearData();
  // Clear the display and the data
  virtual void 		ViewRow_At(int start);
  // start viewing at indicated viewrange value
  
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const TableView& cp);
  COPY_FUNS(TableView, T3DataViewPar);
  T3_DATAVIEWFUNS(TableView, T3DataViewPar) //

protected:
  iTableView_Panel*	m_lvp; //note: will be a subclass of this, per the log type
  int			m_rows; // cached rows, we use to calc deltas etc.
  int			updating; // to prevent recursion

  override void 	UpdateAfterEdit_impl();
  virtual void 		InitViewSpec();
  // called to (re)init the viewspecs

  virtual void		ClearViewRange();
  // sets view range back to beginning (grid adds cols, graph adds TBA)
  virtual void 		MakeViewRangeValid();
  // adjust row/col etc. to be valid
  
  // routines for handling data changes -- only one should be called in any change context
  virtual void  	DataChange_StructUpdate(); 
  // when structure or src of data changes
  virtual void  	DataChange_NewRows(int rows_added);
  // we received new data (update) -- this predominant use-case can be optimized (rather than nuking/rebuilding each row)
  virtual void  	DataChange_Other();
  // all other changes

  virtual int		CheckRowsChanged();
  // check if datatable rows is same as last render (updates m_rows and returns any delta, 0 if no change)
  
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void 	Render_post();
  override void		Reset_impl();

  override void 	DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);
};

class TA_API GridTableView: public TableView {
  // the master view guy for entire grid view
INHERITED(TableView)
public:
  static GridTableView* New(DataTable* dt, T3DataViewFrame*& fr);

  int		col_n; 		// number of columns to display: determines sizes of everything automatically from this
  MinMaxInt	col_range; 	// column range that is visible (max is the last col visible, not the last+1; range = col_n-1, except if columns are not visible range can be larger)

  float		width;		// how wide to make the display (height is always 1.0)
  bool		grid_on; 	// whether to show grid lines
  bool		header_on;	// is the table header visible?
  bool		row_num_on; 	// row number col visible?
  ColorScale	scale; 		// contains current min,max,range,zero,auto_scale

  GridTableViewSpec view_spec;  // #SHOW_TREE baked in spec 

  override void	InitDisplay(bool init_panel = true);
  override void	UpdateDisplay(bool update_panel = true);

  // view button/field callbacks
  void		setGrid(bool value);
  void		setHeader(bool value);
  void		setWidth(float wdth);
  void		setRows(int rows);
  void		setCols(int cols);
  void		setAutoScale(bool value);
  void		setScaleData(bool auto_scale, float scale_min, float scale_max);
  // updates the values in us and the stored ones in the colorscale list

  // view control
  void		VScroll(bool left); // scroll left or right
  virtual void 	ViewCol_At(int start);	// start viewing at indicated column value
  
  override DataTable*	dataTable() const {return view_spec.dataTable();}

  override DataTableViewSpec* viewSpecBase() const 
  { return const_cast<GridTableViewSpec*>(&view_spec);}
    
  inline GridTableViewSpec* viewSpec() const 
  { return const_cast<GridTableViewSpec*>(&view_spec);}

  iGridTableView_Panel*	lvp(){return (iGridTableView_Panel*)m_lvp;}

  T3GridViewNode* node_so() const {return (T3GridViewNode*)m_node_so.ptr();}
  
  void	InitLinks();
  void 	CutLinks();
  void	Initialize();
  void	Destroy() {CutLinks();}
  void	Copy_(const GridTableView& cp);
  COPY_FUNS(GridTableView, inherited);
  T3_DATAVIEWFUNS(GridTableView, TableView)

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

  override void  	DataChange_NewRows(int rows_added);
  
  override void		OnWindowBind_impl(iT3DataViewFrame* vw);
  override void		Clear_impl();
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Render_post(); // #IGNORE
};


//////////////////////////
//  iTableView_Panel 	//
//////////////////////////

class TA_API iTableView_Panel: public iViewPanelFrame {
  // abstract base for logview panels -- just has the viewspace widget; everything else is up to the subclass
  INHERITED(iViewPanelFrame)
  Q_OBJECT
public:
  iT3ViewspaceWidget*	    t3vs; //note: created with call to Constr_T3Viewspace

//  override String	panel_type() const; // this string is on the subpanel button for this panel

  TableView*		lv() {return (TableView*)m_dv;}
  SoQtRenderArea* 	ra() {return m_ra;}

  virtual void 		InitPanel();
  // called on structural changes 
  virtual void 		UpdatePanel();
  // call when data added/removed, or view is scrolled, or other non-structural changes

  void 			viewAll(); // zooms to fit entire scenegraph in window

  iTableView_Panel(TableView* lv);
  ~iTableView_Panel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iTableView_Panel;}

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

class TA_API iGridTableView_Panel: public iTableView_Panel {
  Q_OBJECT
INHERITED(iTableView_Panel)
public:
  QWidget*		widg;
  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layTopCtrls;
  QCheckBox*		    chkDisplay;
  QCheckBox*		    chkHeaders;
  QPushButton*		    butRefresh;
  QPushButton*		    butClear;

  QHBoxLayout*		  layVals;
  QLabel*		    lblWidth;
  taiField*		    fldWidth; // width of the display (height is always 1.0)
  QLabel*		    lblRows;
  taiIncrField*		    fldRows; // number of rows to display
  QLabel*		    lblCols;
  taiIncrField*		    fldCols; // number of cols to display

  QHBoxLayout*		  layColorScale;
  QCheckBox*		    chkAutoScale;
  ScaleBar*		    cbar;	      // colorbar

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
  void 		butRefresh_pressed();
  void 		butClear_pressed();
  void 		fldWidth_textChanged();
  void 		fldRows_textChanged();
  void 		fldCols_textChanged();

  void 		chkAutoScale_toggled(bool on);
  void		cbar_scaleValueChanged();
};


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
class TA_API iDataTableView: public QTableView {
  // widget with some customizations to display submatrix views
INHERITED(QTableView)
  Q_OBJECT
public:
  iDataTableView(QWidget* parent = NULL);

signals:
  void 			currentChanged(const QModelIndex& current);
  
protected:
  void 			currentChanged(const QModelIndex& current,
    const QModelIndex& previous); // override
};
#endif // MAKETA

class TA_API iDataTableEditor: public QWidget {
  Q_OBJECT
INHERITED(QWidget)
public:
  QVBoxLayout*		layOuter;
  QSplitter*		splMain;
  iDataTableView*	  tvTable; // the main table
  QTableView*		  tvCell; // a matrix cell in the table

  DataTable*		dt() const {return m_dt;}
  void			setDataTable(DataTable* dt);
  DataTableModel*	dtm() const {return (m_dt.ptr()) ? m_dt->GetDataModel() : NULL;}
  
  void			Refresh(); // for manual refresh
  
  iDataTableEditor(QWidget* parent = NULL);
  ~iDataTableEditor();
  
public slots:
  void			tvTable_currentChanged(const QModelIndex& index); // #IGNORE
  
protected:
  DataTableRef		m_dt;
  taMatrixPtr		m_cell; // current cell TODO: this ref will prevent col from growing for new row
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
  COPY_FUNS(DataTableGridViewWizard, taWizard);
  TA_BASEFUNS(DataTableGridViewWizard); //
protected:
//  override void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};
*/
#endif
