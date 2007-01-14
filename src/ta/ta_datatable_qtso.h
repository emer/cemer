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
class DataColView;
class DataTableView;
class GridColView;
class GridTableView;

class iDataTableView_Panel;
class iGridTableView_Panel;

class tabDataTableViewType;
class iDataTableView;
class iDataTablePanel; //

// externals
class T3GridViewNode;

class TA_API DataColView: public T3DataView {
  // ##SCOPE_DataTableView base specification for the display of data columns
INHERITED(T3DataView)
friend class DataTableView;
public:
  String	name;		// name of column this guy is associated with
  bool		visible;	// is this column visible in display?
  bool		sticky; 	// #DEF_false set this to retain this colspec even if its column deletes

  DataArray_impl*	dataCol() const {return (DataArray_impl*)data();}
  void			setDataCol(DataArray_impl* value, bool first_time = false);
  
  DATAVIEW_PARENT(DataTableView)

  bool			isVisible() const; // bakes in check for datacol

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  override void		DataDestroying();
  
  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const DataColView& cp);
  COPY_FUNS(DataColView, inherited);
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
  
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const DataTableView& cp);
  COPY_FUNS(DataTableView, T3DataViewPar);
  T3_DATAVIEWFUNS(DataTableView, T3DataViewPar) //

protected:
  iDataTableView_Panel*	m_lvp; //note: will be a subclass of this, per the log type
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

  void			UpdateFromDataTable(bool first_time = false);
  // called if data set to table, or needs to be updated; calls _this then _child
  virtual void		UpdateFromDataTable_this(bool first); // does me (before kids)
  virtual void		UpdateFromDataTable_child(bool first);//does kids, usually not overridden
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
    NARROW -- in addition to base spec, also sets column with to 8 chars
*/

class TA_API GridColView : public DataColView {
  // information for display of a data column in a grid display.  scalar columns are always displayed as text, and matrix as blocks (with optional value text, controlled by overall table spec)
INHERITED(DataColView)
public:
  enum MatrixLayout { 	// order of display for matrix cols
    BOT_ZERO, 		// row zero is displayed at bottom of cell (default)
    TOP_ZERO 		// row zero is displayed at top of cell (ex. for images)
  };

  int		text_width; 	// width of the column (or each matrix col) in chars; also the min width in chars
  bool		scale_on; 	// adjust overall colorscale to include this data (if it is a matrix type)
  MatrixLayout	mat_layout; 	// #DEF_BOT_ZERO layout of matrix and image cells
  bool		mat_image;	// display matrix as an image instead of grid blocks
  bool		mat_odd_vert;	// how to arrange odd-dimensional matrix values (e.g., 1d or 3d) -- put the odd dimension in the Y (vertical) axis (else X, horizontal)
  
  float 	col_width; // #READ_ONLY #HIDDEN #NO_SAVE calculated col_width in chars
  float		row_height; // #READ_ONLY #HIDDEN #NO_SAVE calculated row height in chars

  virtual void		ComputeColSizes();
  // compute the column sizes

  override bool		selectEditMe() const { return true; }
  override String	GetDisplayName() const;

  DATAVIEW_PARENT(GridTableView)
  void	Copy_(const GridColView& cp);
  COPY_FUNS(GridColView, DataColView);
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
  MinMaxInt	col_range; 	// column range that is visible (max is the last col visible, not the last+1; range = col_n-1, except if columns are not visible range can be larger)

  float		width;		// how wide to make the display (height is always 1.0)
  bool		grid_on; 	// #DEF_true whether to show grid lines
  bool		header_on;	// #DEF_true is the table header visible?
  bool		row_num_on; 	// #DEF_true row number col visible?
  bool		two_d_font;	// #DEF_true use 2d font (easier to read, but doesn't scale) instead of 3d font
  float		two_d_font_scale; // #DEF_350 how to scale the two_d font relative to the computed 3d number
  bool		mat_val_text;	// also display text values for matrix blocks

  ColorScale	scale; 		// contains current min,max,range,zero,auto_scale

  float		grid_margin; 	// #DEF_0.01 #MIN_0 size of margin between grid cells (in normalized units)
  float		grid_line_size; // #DEF_0.005 #MIN_0 size of grid lines (in normalized units)
  int		row_num_width;	// #DEF_4 width of row number column
  float		mat_block_spc;	// #DEF_0.1 space between matrix cell blocks, as a proportion of max of X, Y cell size
  float		mat_block_height; // #DEF_0.2 how tall (in Z dimension) to make the blocks (relative to the max of their X or Y size)
  float		mat_rot;	  // #DEF_0 rotation of the matrix in the Z plane (in degrees) - allows for vertical stacks of grids to be displayed in depth
  float		mat_trans;	  // #DEF_0.6 maximum transparency of zero values in matrix blocks -- set to 0 to make all blocks opaque

  MinMaxInt	mat_size_range;	// range of display sizes for matrix items relative to other text items.  each cell in a matrix counts as one character in size, within these ranges (smaller matricies are made larger to min size, and large ones are made smaller to max size)
  float		max_text_sz;	// #DEF_0.5 maximum text size, enforced in cases where there are 

  override void	InitDisplay(bool init_panel = true);
  override void	UpdateDisplay(bool update_panel = true);

  // view button/field callbacks
  void		setGrid(bool value);
  void		setHeader(bool value);
  void		setRowNum(bool value);
  void		set2dFont(bool value);
  void		setValText(bool value);
  void		setWidth(float wdth);
  void		setRows(int rows);
  void		setCols(int cols);
  void		setMatTrans(float value);
  void		setMatRot(float value);
  void		setAutoScale(bool value);
  void		setScaleData(bool auto_scale, float scale_min, float scale_max);
  // updates the values in us and the stored ones in the colorscale list

  // view control
  void		VScroll(bool left); // scroll left or right
  virtual void 	ViewCol_At(int start);	// start viewing at indicated column value
  
  iGridTableView_Panel*	lvp(){return (iGridTableView_Panel*)m_lvp;}

  T3GridViewNode* node_so() const {return (T3GridViewNode*)m_node_so.ptr();}
  
  void	InitLinks();
  void 	CutLinks();
  void	Initialize();
  void	Destroy() {CutLinks();}
  void	Copy_(const GridTableView& cp);
  COPY_FUNS(GridTableView, inherited);
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
  QCheckBox*		    chkValText;
  QPushButton*		    butRefresh;
  QPushButton*		    butClear;

  QHBoxLayout*		  layVals;
  QLabel*		    lblWidth;
  taiField*		    fldWidth; // width of the display (height is always 1.0)
  QLabel*		    lblRows;
  taiIncrField*		    fldRows; // number of rows to display
  QLabel*		    lblCols;
  taiIncrField*		    fldCols; // number of cols to display
  QLabel*		    lblTrans;
  taiField*		    fldTrans; // mat_trans parency
  QLabel*		    lblRot;
  taiField*		    fldRot; // mat_rot ation

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
  void 		chkRowNum_toggled(bool on);
  void 		chk2dFont_toggled(bool on);
  void 		chkValText_toggled(bool on);

  void 		butRefresh_pressed();
  void 		butClear_pressed();
  void 		fldWidth_textChanged();
  void 		fldRows_textChanged();
  void 		fldCols_textChanged();
  void 		fldTrans_textChanged();
  void 		fldRot_textChanged();

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

class TA_API iDataTableEditor: public QWidget, public ISelectableHost {
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
  
public: // ISelectableHost i/f
  override bool 	hasMultiSelect() const {return false;} // always
  override QWidget*	widget() {return this;} 
protected:
  override void		UpdateSelectedItems_impl(); 

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
  void			Mat_EditAction(taMatrix* mat, 
    const CellRange& selected, taiMimeSource* ms, int ea) const;
    // note: this does the requery to insure it is still legal
  taiClipData* 		Mat_GetClipData(taMatrix* mat,
    const CellRange& sel, int src_edit_action, bool for_drag = false) const;
  
  void			AddMatDesc(QMimeData* md,
    taMatrix* mat, const CellRange& selected) const;

/*  virtual void		AddSingleMimeData(QMimeData* md, taBase* obj);
    // used for putting one object on the clipboard
  virtual void		AddMultiMimeData(QMimeData* md, taPtrList_impl* obj_list);
    // used for putting multiple objects on the clipboard */
    
  TA_MFBASEFUNS(taiTabularDataMimeFactory);
protected:
  void			AddDims(const CellRange& sel, String& str) const;

/*  void			InitHeader(int cnt, QString& str); // common for single/multi
  void			AddHeaderDesc(taBase* obj, QString& str);
    // add entry for one object
  void			AddObjectData(QMimeData* md, taBase* obj, int idx); 
    // add mime entry for one obj */
private:
  void	Initialize() {}
  void	Destroy() {}
};

class TA_API taiTabularDataMimeItem: public taiMimeItem { // base for matrix and table data
INHERITED(taiMimeItem)
public: // i/f for tabular data guy
/*TODO  override bool		isMatrix() const;
  override bool		isTable() const;
  
  const CellRange	cellRange() const; // the range of the data
  virtual void		GetDataGeom(int& cols, int& rows) const = 0;
    // number of cols/rows in the overall data
  virtual void		GetColGeom(int col, int& cols, int& rows) const = 0;
    // 2-d geom of the indicated column; always 1x1 (scalar) for matrix data
  virtual void		GetMaxRowGeom(int& max_row) const = 0;
    // longest cell geom determines overall row geom
    */
  TA_BASEFUNS(taiTabularDataMimeItem);
    
protected:
//  int			m_data_type; // one of ST_MATRIX_DATA or TABLE_DATA
//  override void 	GetFormats_impl(QStringList& list, int idx) const; 
private:
  void	Initialize() {}
  void	Destroy() {}
};


/*
class TA_API taiMatDataMimeItem: public taiMimeItem { // for matrix and table data
INHERITED(taiMimeItem)
  Q_OBJECT
public:
  
public: // i/f for tabular data guy
  override bool		isMatrix() const;
  override bool		isTable() const;
  virtual void		GetDataGeom(int& cols, int& rows) const = 0;
    // number of cols/rows in the overall data
  virtual void		GetColGeom(int col, int& cols, int& rows) const = 0;
    // 2-d geom of the indicated column; always 1x1 (scalar) for matrix data
  virtual void		GetMaxRowGeom(int& max_row) const = 0;
    // longest cell geom determines overall row geom
    
protected:
  int			m_data_type; // one of ST_MATRIX_DATA or TABLE_DATA
  override void 	GetFormats_impl(QStringList& list, int idx) const; 
  taiMatDataMimeItem(int data_type);
}; 


class TA_API taiRcvMatDataMimeItem: public taiMatDataMimeItem { 
  // for received mat or table data, or compatible foreign mat data
INHERITED(taiMatDataMimeItem)
  Q_OBJECT
friend class taiExtMimeSource;
public:
  
public: // i/f for tabular data guy
  void			GetDataGeom(int& cols, int& rows) const
    {cols = m_cols;  rows = m_rows;}
  void			GetColGeom(int col, int& cols, int& rows) const;
  void			GetMaxRowGeom(int& max_row) const {max_row = m_max_row;} 

protected:
  int			m_cols;
  int			m_rows;
  int			m_max_row;
  taBase_List		m_geoms; // list of GeomData
  
  void			DecodeMatrixDesc(String& arg); // same for both
  void			DecodeTableDesc(String& arg); // the extra stuff
  
  taiRcvMatDataMimeItem(int data_type);
};*/


#endif
