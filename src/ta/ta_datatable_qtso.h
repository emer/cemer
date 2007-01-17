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
  override void		DoActionChildren_impl(DataViewAction acts);

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
  int_Array	vis_cols;	// #READ_ONLY #NO_SAVE indicies of visible columns
  MinMaxInt	col_range; 	// column range to display, in terms of the visible columns (contained in vis_cols index list)

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
  float		max_text_sz;	// #DEF_0.05 maximum text size, enforced in cases where there are 

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

  override bool		selectEditMe() const { return true; }
  
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
  QPushButton*		    butSetColor;

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
  void 		butSetColor_pressed();
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
  iMatrixTableView*	  tvCell; // a matrix cell in the table

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
  
protected slots:
  void			tv_hasFocus(iTableView* sender); // for both tableviews
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

  iSize			m_flat_geom; 
  bool 			ReadInt(String& arg, int& val); // read a ; terminated int
  bool 			ExtractGeom(String& arg, iSize& val); // get the cols/rows
  bool			ReadTsvValue(istringstream& strm, String& val, TsvSep& sep); // reads value if possible, into val, returning true if a value read, and the separator encountered after the value in sep.
  virtual void		WriteTable_Generic(DataTable* tab, const CellRange& sel);
  
private:
  void	Initialize() {}
  void	Destroy() {}
};


class TA_API taiMatrixDataMimeItem: public taiTabularDataMimeItem { // this class handles Matrix -- optimized since we know the dims, and know the data is accurate
INHERITED(taiTabularDataMimeItem)
public: // i/f for tabular data guy  
  
  TA_BASEFUNS(taiMatrixDataMimeItem);
    
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
  
  TA_BASEFUNS(taiTsvMimeItem);
    
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
  inline int		maxRowGeom() const {return m_max_row_geom;}
  
  override void		WriteTable(DataTable* tab, const CellRange& sel);
  
  TA_BASEFUNS(taiTableDataMimeItem);
    
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
