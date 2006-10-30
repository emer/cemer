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



#ifndef datatable_qtso_h
#define datatable_qtso_h

#include "ta_qttype.h"
#include "ta_qtviewer.h"
#include "ta_fontspec.h"

#include "colorscale.h"
#include "colorbar_qt.h"
#include "datatable.h"
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
class T3TableViewNode;
class T3GridTableViewNode;

class TAMISC_API TableView : public T3DataViewPar {
  // #VIRT_BASE #NO_TOKENS base class of grid and graph views; the data is its own embedded DataTableViewSpec
INHERITED(T3DataViewPar)
public: //
    
  int		view_bufsz; // #READ_ONLY #SAVE Maximum number of lines in visible buffer
  MinMaxInt	view_range;	// range of visible lines

  bool		display_on;  // #DEF_true 'true' if display should be updated
  TDCoord	pos;		// origin of the view in 3d space (not used for panel view)
  TDCoord	geom;		// space taken
  float		frame_inset;	// #DEF_0.05 inset of frame (used to calc actual inner space avail)
  
  virtual DataTable*	dataTable() const {return viewSpecBase()->dataTable();}
    //note: can override for more efficient direct reference
  void			setDisplay(bool value); // use this to change display_on
  T3TableViewNode*	node_so() const {return (T3TableViewNode*)m_node_so.ptr();}
  inline int		rows() const {return m_rows;}
  virtual DataTableViewSpec* viewSpecBase() const 
    {return (DataTableViewSpec*)m_data.ptr();}
  bool			isVisible() const; // gui_active, mapped and display_on
  
  virtual void		ViewRangeChanged(); // called when view_range changed (override _impl to implement)
  virtual void 		ClearData();	// Clear the display and the data

  
  virtual void		InitPanel();// lets panel init itself after struct changes
  virtual void		UpdatePanel();// after changes to props

// routines for handling data changes -- only one should be called in any change context
  virtual void  	DataChange_StructUpdate(); 
   // when structure or src of data changes
  virtual void  	DataChange_NewRows(); // we received new data (update) -- this predominant use-case can be optimized (rather than nuking/rebuilding each row)
  virtual void  	DataChange_Other(); // all other changes
  
  virtual void		InitNew(DataTable* dt, T3DataViewFrame* fr); // #IGNORE common code for creating a new one -- creates new fr if necessary -- called virtually after construction
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const TableView& cp);
  COPY_FUNS(TableView, T3DataViewPar);
  T3_DATAVIEWFUNS(TableView, T3DataViewPar) //

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

protected:
// view control:
  virtual void		ClearViewRange(); // sets view range back to beginning (grid adds cols, graph adds TBA)


  iTableView_Panel*	m_lvp; //note: will be a subclass of this, per the log type
  int			m_rows; // cached rows, we use to calc deltas etc.
  override void 	UpdateAfterEdit_impl();
  virtual void 		InitViewSpec();	// called to (re)init the viewspecs
  void			InitDisplay(); // called to (re)do all the viewing params
  virtual void		InitDisplay_impl() {} // type-specific impl
  
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void 	Render_post();
  override void		Reset_impl();
  virtual void		ViewRangeChanged_impl(){} // update view from buffer

};

class TAMISC_API GridTableView: public TableView {
  // #VIRT_BASE #NO_TOKENS 
INHERITED(TableView)
public:
  static GridTableView* NewGridTableView(DataTable* dt,
    T3DataViewFrame* fr);

  int		col_bufsz; // #READ_ONLY #SAVE visible columns
  MinMaxInt	col_range;	// column range that is visible
  float		tot_col_widths; // total of all (visible) col_widths

  bool		header_on;	// is the table header visible?
  bool		auto_scale;	// whether to auto-scale on color block values or not
  
  ColorScale	scale; 		// The color scale for this display
  MinMax        scale_range;	// #HIDDEN range of scalebar
  MinMaxInt	actual_range;	// #HIDDEN #NO_SAVE range in actual lines of data

  GridTableViewSpec view_spec; // #SHOW_TREE baked in spec 

  float			colWidth(int idx) const; // looked up from the specs
  override DataTable*	dataTable() const {return view_spec.dataTable();}
  override DataTableViewSpec* viewSpecBase() const 
    {return const_cast<GridTableViewSpec*>(&view_spec);}
  inline GridTableViewSpec* viewSpec() const 
    {return const_cast<GridTableViewSpec*>(&view_spec);}
  iGridTableView_Panel*	lvp(){return (iGridTableView_Panel*)m_lvp;}
  T3GridTableViewNode* node_so() const {return (T3GridTableViewNode*)m_node_so.ptr();}

  virtual void	SetBlockFill(
    GridColViewSpec::BlockColor color = GridColViewSpec::COLOR,
    GridColViewSpec::BlockFill fill = GridColViewSpec::FILL);
  // #MENU set the fill style of the grid blocks
  virtual void	SetBlockSizes(float block_sz = 4.0f, float border_sz = 1.0f);
  // #MENU set the MAXIMUM sizes of all blocks (could be smaller), and the border space between blocks

  virtual void		SetViewFontSize(int point_size = 10);
  // #MENU #MENU_SEP_BEFORE set the point size of the font used for labels in the display
  void			AllBlockTextOn() {AllBlockText_impl(true);}
  // #MENU turn text on for all block displayed items
  void			AllBlockTextOff() {AllBlockText_impl(false);}
  // #MENU turn text off for all block displayed items

  virtual void	ToggleHeader();  // toggle header on or off
  virtual void	ToggleAutoScale();  // toggle header on or off

// view control
  void			VScroll(bool left); // scroll left or right
  virtual void 		ViewC_At(int start);	// start viewing at indicated column value

  
  void	InitLinks();
  void 	CutLinks();
  void	Initialize();
  void	Destroy() {CutLinks();}
  void	Copy_(const GridTableView& cp);
  COPY_FUNS(GridTableView, inherited);
  T3_DATAVIEWFUNS(GridTableView, TableView)

public:
  void ColorBar_execute();

protected:
// grid-specific metrics and rendering primitives:
  float			head_height; // based on font size (or 0 if off)	
  float			row_height; // #IGNORE determined from max of all cols
  virtual void		CalcViewMetrics(); // for entire view
  virtual void		CalcColMetrics(); // when col start changes
  virtual void		RemoveHeader(); // remove the header
  virtual void  	RemoveLine(int idx); // remove line, -1 = all
  void			RemoveLines() {RemoveLine(-1);} // remove all lines
  virtual void		RenderHeader();
  virtual void		RenderLines(); // render all the view_range lines
  virtual void		RenderLine(int view_idx, int data_row); // add indicated line

// view control:
  override void		ClearViewRange();
  
  override void  	InitDisplay_impl();
  override void		ViewRangeChanged_impl(); // note: only called if mapped
  
  void 			AllBlockText_impl(bool on);
  override void		OnWindowBind_impl(iT3DataViewFrame* vw);
  override void		Clear_impl();
  override void		Render_pre(); // #IGNORE
  override void		Render_post(); // #IGNORE
  override void		Reset_impl();
};


//////////////////////////
//  iTableView_Panel 	//
//////////////////////////

class TAMISC_API iTableView_Panel: public iViewPanelFrame {
  // abstract base for logview panels
INHERITED(iViewPanelFrame)
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

  QWidget*		widg;
  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layTopCtrls;
  QCheckBox*		    chkDisplay;
  QCheckBox*		    chkAuto; // NOTE: only created in GridTableView
  QCheckBox*		    chkHeaders; // NOTE: only created in GridTableView
  QButtonGroup*		    bgpTopButtons; // NOTE: not a widget
  QHBoxLayout*		    layVcrButtons;
  QHBoxLayout*		    layInitButtons;
  QHBoxLayout*		  layContents; // subclasses put their actual content here
  iT3ViewspaceWidget*	    t3vs; //note: created with call to Constr_T3Viewspace

//  override String	panel_type() const; // this string is on the subpanel button for this panel

  TableView*		lv() {return (TableView*)m_dv;}
  SoQtRenderArea* 	ra() {return m_ra;}
//  override int 		EditAction(int ea);
//  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)

  void 			viewAll(); // zooms to fit entire scenegraph in window

  virtual void 		BufferUpdated() {} //called when data added/removed, or view is scrolled

  iTableView_Panel(TableView* lv);
  ~iTableView_Panel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iTableView_Panel;}

protected:
  SoQtRenderArea* 	m_ra;
  SoPerspectiveCamera*	m_camera;
  SoLightModel*		m_lm;

  iTableView_Panel(bool is_grid_log, TableView* lv); // only used by GridTableView
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


class TAMISC_API iGridTableView_Panel: public iTableView_Panel {
  Q_OBJECT
INHERITED(iTableView_Panel)
public:
  override String	panel_type() const; // this string is on the subpanel button for this panel
  GridTableView*	glv() {return (GridTableView*)m_dv;}

//  override int 		EditAction(int ea);
//  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  override void 	BufferUpdated();

  iGridTableView_Panel(GridTableView* glv);
  ~iGridTableView_Panel();

public slots:
  void 			horScrBar_valueChanged(int value);
  void 			verScrBar_valueChanged(int value);

protected:
  override void		InitPanel_impl(); // called on structural changes

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iGridTableView_Panel;}
protected:
//  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);
  override void 	GetImage_impl();

protected slots:
protected slots:
  void 			chkAuto_toggled(bool on);
  void 			chkHeaders_toggled(bool on);
//  void			list_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col);
//  void			list_selectionChanged(); //note: must use this parameterless version in Multi mode
};


class TAMISC_API tabDataTableViewType: public tabOViewType {
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


class TAMISC_API DataTableDelegate: public QItemDelegate {
  Q_OBJECT
INHERITED(QItemDelegate)
public:
  DataTableRef		dt; // we maintain a ref to get modal information
  
  DataTableDelegate(DataTable* dt);
  ~DataTableDelegate();
};

#ifndef __MAKETA__ // too much crud to parse
class TAMISC_API iDataTableView: public QTableView {
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

class TAMISC_API iDataTableEditor: public QWidget {
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
  
  iDataTableEditor(QWidget* parent = NULL);
  ~iDataTableEditor();
  
public slots:
  void			tvTable_currentChanged(const QModelIndex& index); // #IGNORE
  
protected:
  DataTableRef		m_dt;
  taMatrixPtr		m_cell; // current cell TODO: this ref will prevent col from growing for new row
};


class TAMISC_API iDataTablePanel: public iDataPanelFrame {
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

/*protected slots: */
};


#endif
