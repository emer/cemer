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

#include "colorscale.h"
#include "colorbar_qt.h"
#include "datatable.h"
#include "fontspec.h"

#ifndef __MAKETA__
#include <qtable.h>
#endif

// forwards
class tabDataTableViewType;
class iDataTable;
class iDataTablePanel;

//////////////////////////
//  tabDataTableViewType//
//////////////////////////

class tabDataTableViewType: public tabGroupViewType {
#ifndef __MAKETA__
typedef tabGroupViewType inherited;
#endif
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabDataTableViewType, tabGroupViewType) //
protected:
//nn  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};


//////////////////////////
//    iDataTable	//
//////////////////////////

/*
You will also need to reimplement item(), setItem(), takeItem(), clearCell(), and insertWidget(), cellWidget() and clearCellWidget(). In almost every circumstance (for sorting, removing and inserting columns and rows, etc.), you also need to reimplement swapRows(), swapCells() and swapColumns(), including header handling.
*/

class iDataTable: public QTable { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS
  Q_OBJECT
INHERITED(QTable)
public:
  DataTable*		dt() {return m_dt;}
  void			setDataTable(DataTable* value);

  void			updateConfig(); // updates size, and header names

  iDataTable(QWidget* parent = NULL);
  ~iDataTable(); //

public: // overrides
  QWidget* 		cellWidget(int row, int col) const; // override
  void 			clearCellWidget(int row, int col); // override
  void 			clearCell(int row, int col); // override
  void 			setItem (int row, int col, QTableItem* item) {} // override stub out
  QTableItem* 		item(int row, int col) const  {return NULL;} // override stub out
  void 			paintCell (QPainter* p, int row, int col, const QRect& cr,
    bool selected, const QColorGroup& cg ); // override
  void 			resizeData (int len) {} // empty override, per using virtual table
  void 			takeItem(QTableItem* item) {} // override stub out

  void swapRows ( int row1, int row2, bool swapHeader = FALSE ) {} // TODO
  void swapColumns ( int col1, int col2, bool swapHeader = FALSE ) {} // TODO
  void swapCells ( int row1, int col1, int row2, int col2 )  {} // TODO

protected:
  DataTable*		m_dt;
  QByteArray*		col_align; // contains QT::Align bits
  mutable QWidget* 		cell_widge; // #IGNORE actually, a LineEdit

  QWidget* 		createEditor(int row, int col, bool initFromCell) const;  // override
  void 			insertWidget(int row, int col, QWidget* w); // override
  void 			setCellContentFromEditor(int row, int col); // override
};


//////////////////////////
//  iDatatableDataPanel //
//////////////////////////

class iDataTablePanel: public iDataPanelFrame {
  Q_OBJECT
#ifndef __MAKETA__
typedef iDataPanelFrame inherited;
#endif
public:
  DataTable*		dt() {return (m_link) ? (DataTable*)(link()->data()) : NULL;}

  QVBoxLayout*		layOuter;
  iDataTable*		idt;

  override String	panel_type() const; // this string is on the subpanel button for this panel

  override int 		EditAction(int ea);
  void			FillList();
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























/*
//externals

class ivGlyph;			// #IGNORE
class tbScrollBox;		// #IGNORE
class ivPolyGlyph;		// #IGNORE
class ivPatch;			// #IGNORE
class HCScaleBar;		// #IGNORE
class ColorScale;		// #IGNORE
class ivButton;			// #IGNORE
class HiLightButton;		// #IGNORE
class taivMenu;			// #IGNORE
class ivInputHandler;		// #IGNORE
class ivDeck;			// #IGNORE


// forwards this file

class DataLineBox_G;
class DataLine_G;
class Grid_G;
class DTViewer;
class DTEditor;


class DataTable_G : public GraphicMaster {
  // ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS
public:
  DataTable*	dt;		// the data table itself
  DT_GridViewSpec* dtvsp;	// and its grid view spec
  DTEditor*	owner;
  iColor*	border;	        // 'unselected' border color
  const ivBrush* defbrush;	// default brush
  FloatTDCoord	max_size;	// for the whole DataTable (sets scaling)
  bool		reinit_display;	// subobjs set this flag to reinit after popping out
  ivTransformer* tx;

  virtual void	Reset();
  virtual void	Build();

  virtual iColor* GetLabelColor();

  virtual void	AddOneLine();	// add one new line to display
  virtual void	ScrollLines();	// update existing boxes to use current line info

  bool		update_from_state(ivCanvas* c);

  bool 		select(const ivEvent& e, Tool& tool, bool);
  bool		effect_select(bool set_select);

  void 		allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext);
  void 		ReCenter();
  void 		ScaleCenter(const ivAllocation& a);

  // translate x,y unit-wise coordinates into screen coords
  ivCoord xcscale(ivCoord xval) { return xval / max_size.x; }
  ivCoord ycscale(ivCoord yval) { return yval / max_size.y; }

  // translate x,y unit-wise coordinates into text coords
  ivCoord xtscale(ivCoord xval) { return GetCurrentXformScale() * xcscale(xval); }
  ivCoord ytscale(ivCoord yval) { return GetCurrentYformScale() * ycscale(yval); }

  DataTable_G(DataTable* d, DT_GridViewSpec* dtv);
  ~DataTable_G();
  GRAPHIC_BASEFUNS(DataTable_G);
};

class DataLine_G : public PolyGraphic {
  // represents one line of data (or its template)
public:
  int 			line_no; // line of data to represent (FROM END), -1 = template
  DataTable_G*		dtg;

  virtual void	Build();
  virtual void	BuildTable(DT_GridViewSpec* dtv);

  virtual void	ScrollLines(int nw_ln);	// set new line number for all my elements

  bool		selectable() { return false; }
  bool		graspable() { return false; }

  bool     	select(const ivEvent& e, Tool& tool, bool unselect);
  bool		effect_select(bool set_select);

  ivGlyph*	clone() const;
  DataLine_G(int lno, DataTable_G* ng);
  virtual ~DataLine_G();
  GRAPHIC_BASEFUNS(DataLine_G);
};

class DataLineBox_G : public Polygon {
  // box representing DataLine (actually draws as a horizontal line)
public:
  DataLine_G*		dlg;
  DataTable_G* 		dtg;
  bool			editb_used; // the edit-button was used (right mouse button)

  virtual void 	SetPointsBox(ivCoord x,ivCoord y);
  virtual void 	SetPointsLine(ivCoord x,ivCoord y);

  bool		selectable() { return false; }
  bool		graspable() { return false; }

  bool     	select(const ivEvent& e, Tool& tool, bool unselect);
  bool		effect_select(bool set_select);

  DataLineBox_G(DataLine_G* eg, DataTable_G* dg);
  GRAPHIC_BASEFUNS(DataLineBox_G);
};

class TextData_G : public NoScale_Text_G {
  // represents textual data element
public:
  int 			line_no; // line of data to represent (FROM END), -1 = template
  DA_GridViewSpec* 	daspec;
  DataTable_G*		dtg;
  TDCoord		move_pos;
  bool			editb_used; // the edit-button was used (right mouse button)

  virtual void	Position();

  bool  select(const ivEvent& e, Tool& tool, bool unselect);
  bool	effect_select(bool set_select);

  bool	update_from_state(ivCanvas* c);

  bool	grasp_move(const ivEvent&,Tool&,ivCoord, ivCoord);
  bool	manip_move(const ivEvent& ,Tool&,ivCoord,ivCoord,ivCoord,
		   ivCoord,ivCoord,ivCoord);
  bool 	effect_move(const ivEvent& ev, Tool& tl, ivCoord ix,
		    ivCoord iy, ivCoord fx, ivCoord fy);

  bool	grasp_stretch(const ivEvent&, Tool&,ivCoord, ivCoord) { return true; }
  bool	manip_stretch(const ivEvent&, Tool&,ivCoord, ivCoord, ivCoord, ivCoord,
    ivCoord, ivCoord) { return true; }
  bool	effect_stretch(const ivEvent&, Tool&,ivCoord,ivCoord, ivCoord, ivCoord)
    { return true; }

  TextData_G(int lno, DA_GridViewSpec* sp, DataTable_G* dg, const ivFont* , const iColor* ,
		 const char*, ivTransformer*);
  GRAPHIC_BASEFUNS(TextData_G);
};

class Block_G : public Polygon {
  // represents a colored block data element
public:
  int 			line_no; // line of data to represent (FROM END), -1 = template
  DA_GridViewSpec* 	daspec;
  DataTable_G*		dtg;
  TDCoord		move_pos;
  bool			editb_used; // the edit-button was used (right mouse button)

  virtual void	Build();
  virtual void	Position();

  virtual void 	SetPoints(ivCoord x0,ivCoord y0, ivCoord x1, ivCoord y1,
			ivCoord x2,ivCoord y2, ivCoord x3, ivCoord y3);

  bool  select(const ivEvent& e, Tool& tool, bool unselect);
  bool	effect_select(bool set_select);

  void 	draw_gs (ivCanvas* c, Graphic* gs);
  bool	update_from_state(ivCanvas* c);

  virtual void render_text(ivCanvas* c, ScaleBar* cbar, float val, String& str,
			   FloatTwoDCoord& ll, FloatTwoDCoord& ur);
  virtual void render_color(ivCanvas* c, ScaleBar* cbar, float val,
			    FloatTwoDCoord& ll, FloatTwoDCoord& ur);
  virtual void render_area(ivCanvas* c, ScaleBar* cbar, float val,
			   FloatTwoDCoord& ll, FloatTwoDCoord& ur);
  virtual void render_linear(ivCanvas* c, ScaleBar* cbar, float val,
			     FloatTwoDCoord& ll, FloatTwoDCoord& ur);

  bool	grasp_move(const ivEvent&,Tool&,ivCoord, ivCoord);
  bool	manip_move(const ivEvent& ,Tool&,ivCoord,ivCoord,ivCoord,
		   ivCoord,ivCoord,ivCoord);
  bool 	effect_move(const ivEvent& ev, Tool& tl, ivCoord ix,
		    ivCoord iy, ivCoord fx, ivCoord fy);

  bool	grasp_stretch(const ivEvent&, Tool&,ivCoord, ivCoord) { return true; }
  bool	manip_stretch(const ivEvent&, Tool&,ivCoord, ivCoord, ivCoord, ivCoord,
    ivCoord, ivCoord) { return true; }
  bool	effect_stretch(const ivEvent&, Tool&,ivCoord,ivCoord, ivCoord, ivCoord)
    { return true; }

  ivGlyph*	clone() const;
  Block_G(int lno, DA_GridViewSpec* sp, DataTable_G* dg);
  virtual ~Block_G();
  GRAPHIC_BASEFUNS(Block_G);
};

class Grid_G : public Block_G {
  // represents a grid data element (colored squares)
public:
  DT_GridViewSpec* 	dtspec;

  virtual void	Build();
  virtual void	Position();

  bool  select(const ivEvent& e, Tool& tool, bool unselect);
  bool	effect_select(bool set_select);

  void 	draw_gs (ivCanvas* c, Graphic* gs);
  bool	update_from_state(ivCanvas* c);

  bool	grasp_move(const ivEvent&,Tool&,ivCoord, ivCoord);
  bool	manip_move(const ivEvent& ,Tool&,ivCoord,ivCoord,ivCoord,
		   ivCoord,ivCoord,ivCoord);
  bool 	effect_move(const ivEvent& ev, Tool& tl, ivCoord ix,
		    ivCoord iy, ivCoord fx, ivCoord fy);

  bool	grasp_stretch(const ivEvent&, Tool&,ivCoord ix, ivCoord iy);
  bool	manip_stretch(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
    ivCoord cx, ivCoord cy);
  bool	effect_stretch(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy);

  ivGlyph*	clone() const;
  Grid_G(int lno, DT_GridViewSpec* sp, DataTable_G* dg);
  virtual ~Grid_G();
  GRAPHIC_BASEFUNS(Grid_G);
};

class DTViewer : public GlyphViewer {
  // ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS viewer for DataTable
public:
  void 		press(const ivEvent &);
  void 		release(const ivEvent &);
  void 		init_graphic() 		{ initgraphic();}
  void 		allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext);
  void		pick(ivCanvas* c, const ivAllocation& a, int depth, ivHit& h);
  DTViewer(float w, float h, const iColor* bg);
};

class DTEditor {
  // ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS
  // DT editor: contains all the junk for making a datatable editor, not part of glyph hierarchy
public:
  DTViewer* 		viewer;
  DataTable_G*		dtg;
  ivWindow*		win;
  DataTable* 		dt;
  DT_GridViewSpec*	dtvsp;

  MinMaxInt 		view_range; 	// range in the datatable itself for what to view
  int			disp_lines; 	// actual number of viewable lines in the display (computed)
  DT_GridViewSpec::BlockFill fill_type;	// how to display block value
  int			block_size; 	// *maximum* block size -- blocks will be smaller if needed to fit
  int			block_border_size; // how much of a border to put around blocks
  bool			header_on; 	// whether to display header or not
  bool			auto_scale; 	// whether to auto-scale based on data or not
  FontSpec	  	view_font;	// the font to use for the labels in the display
  MinMax		scale_range; 	// current range of scale

  ivGlyph*	  	body;		// the entire contents of window
  taivMenu*		vtextmenu; 	// value text menu
  taivMenu*		dispmdmenu; 	// display mode menu

  ivPatch*	  	print_patch;	// encapsulates the printable region of viewer
  ivInputHandler* 	data;		// enables fieldeditors

  ColorScale*		scale;		// internal colorscale
  HCScaleBar* 		cbar;		// colorbar at bottom of screen

  virtual void		Init(); 	// initialize
  virtual ivGlyph* 	GetLook();	// gets the whole thing

  virtual void	update_from_state();
  virtual void	InitDisplay(); 		// redo the whole display
  virtual void	UpdateDisplay(); 	// update
  virtual void	AddOneLine();	// add one new line to display
  virtual void	ScrollLines();	// update existing boxes to use current line info

  virtual void	CBarAdjustNotify();
  virtual void	UpdateMinMaxScale();	// get display range info!

  virtual void	SetSelect(); 	// sets cursor and tool mode to Select
  virtual void	SetMove();	// sets cursor and tool mode to Move
  virtual void	SetReShape();
  virtual void	SetReScale();
  virtual void	SetRotate();
  virtual void	SetAlter();

  virtual void	EditSelections();

  DTEditor(DataTable* d, DT_GridViewSpec* sp, ivWindow* w);
  virtual ~DTEditor();
};


*/
#endif // datatable_iv_h



