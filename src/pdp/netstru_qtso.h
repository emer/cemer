/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// netstru_qtso.h -- qt and inventor controls/objects for network structures

// NOTE: functions inside "ifdef GUI" blocks are implemented in netstru_qtso.cc

#ifndef NETSTRU_QTSO_H
#define NETSTRU_QTSO_H

#include "colorscale.h"
#include "colorbar_qt.h"
#include "minmax.h"
#include "t3viewer.h"

#include "pdpbase.h"
#include "netstru.h"
#include "pdp_qtso.h"
//#include "ta_qtdata.h"
#include "pdp_TA_type.h"

#include "safeptr_so.h"
#include "netstru_so.h"

#ifndef __MAKETA__
  #include "irenderarea.h"

  #include <qwidget.h>
#endif

//#include "minmax.h" // for height_field

//externals
class Q3ListView; // #IGNORE

// forwards

class UnitGroupView; //
class LayerView; //
class PrjnView; //
class NetView; //
class NetViewPanel; //


/*
// note: an array is really what we want, but this is ok
class T3UnitNode_PtrList: public taPtrList<T3UnitNode> {
#ifndef __MAKETA__
typedef taPtrList<T3UnitNode> inherited;
#endif
public:
  ~T3UnitNode_PtrList();
protected:
  override void*	El_Ref_(void* it);	// when pushed
  override void* 	El_unRef_(void* it);	// when popped
} */

//////////////////////////
//   nvDataView		//
//////////////////////////

class PDP_API nvDataView: public T3DataViewPar { // #VIRT_BASE most children of NetView
#ifndef __MAKETA__
  typedef T3DataViewPar inherited;
#endif
public:
  NetView*		nv();

  override void		CutLinks();

  TA_BASEFUNS(nvDataView)
protected:
  NetView*		m_nv; // cache

private:
  void			Initialize();
  void			Destroy() {}
};


//////////////////////////
//   UnitView	//
//////////////////////////

// Note that UnitGroupView takes care of rendering all the Units;
// UnitView objects are only created and associated with a T3Unit object
// when the need arises (ex. user selects a T3Unit in the gui)

class PDP_API UnitView: public T3DataView {
#ifndef __MAKETA__
  typedef T3DataView inherited;
#endif
  friend class UnitGroupView;
public:
#ifndef __MAKETA__
  int		picked : 1;	// #IGNORE set when unit is picked for display (note: not same as selected)
#endif
  Unit*			unit() const {return (Unit*)m_data;}
  UnitGroupView*	ugrv() {return GET_MY_OWNER(UnitGroupView);}
  T3UnitNode*		node_so() const {return (T3UnitNode*)m_node_so.ptr();}
  NetView*		nv();

  override void		CutLinks();
  T3_DATAVIEWFUNS(UnitView, T3DataView)
protected:
  NetView*		m_nv; // cache
  override void 	Render_pre(taDataView* par); //
  //note: _impl is done by the UnitGroupView
private:
  void			Initialize();
  void			Destroy() {CutLinks();}
};

class PDP_API UnitViewData { // #IGNORE dynamically maintained data for each unit so we don't need UnitViews
public:
  void*		disp_base;	// #IGNORE base pointer used for display
  UnitViewData() {Initialize();}
  UnitViewData(const String&) {Initialize();} // dummy, for array template
  operator String() {return _nilString;} // dummy convertor for array template
  //NOTE: just use default bitwise copy constructor and copy operator
private:
  void		Initialize();
};

// following needed for the taPlainArray template
inline bool operator ==(const UnitViewData& a, const UnitViewData& b)
  {return ((a.disp_base == b.disp_base));}

class PDP_API UnitViewData_PArray: public taPlainArray<UnitViewData> { // #IGNORE
INHERITED(taPlainArray<UnitViewData>)
public:
  UnitViewData&		FastEl(const TwoDCoord& c) {return el[(c.y * m_x) + c.x];}
  void			SetGeom(TwoDCoord& c); // sets geom, and allocs values
  UnitViewData_PArray() {m_x = 0;}
protected:
  int		m_x; // retained for array lookups
};


//////////////////////////
//   UnitGroupView	//
//////////////////////////

// Note that UnitGroupView takes care of rendering all the Units;
// UnitView objects are only created and associated with a T3Unit object
// when the need arises (ex. user selects a T3Unit in the gui)
//
// The Units font is here, so we don't replicate it in every Unit

class PDP_API UnitGroupView: public nvDataView {
#ifndef __MAKETA__
  typedef nvDataView inherited;
#endif
public:
  static void		ValToDispText(float val, String& str); // renders the display text, typ 6 chars max

  UnitViewData_PArray	uvd_arr; // #IGNORE

  Unit_Group*		ugrp() const {return (Unit_Group*)m_data;}
  UnitViewData&		uvd(const TwoDCoord& co) {return uvd_arr.FastEl(co);} // #IGNORE
  T3UnitGroupNode*	node_so() const {return (T3UnitGroupNode*)m_node_so.ptr();}

  void			AllocUnitViewData(); // make sure we have correct space in uvd array
  override void		BuildAll(); // creates fully populated subviews
  float 		GetUnitDisplayVal(const TwoDCoord& co, int unit_md_flags); // get val for unit at co
  void 			UpdateUnitViewBase(MemberDef* disp_md, Unit* src_u); // set the base for the given md; src_u only used for s./r. values

  T3_DATAVIEWFUNS(UnitGroupView, nvDataView)
protected:
  void 			UpdateUnitViewBase_Unit_impl(MemberDef* disp_md); // for unit members
  void 			UpdateUnitViewBase_Sub_impl(MemberDef* disp_md); // for unit submembers
  void 			UpdateUnitViewBase_Con_impl(bool is_send, String nm, Unit* src_u); // for cons
  override void		Render_pre(taDataView* par = NULL); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void 	Render_impl_children(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
private:
  void			Initialize();
  void			Destroy();
};


//////////////////////////
//   LayerView	//
//////////////////////////

class PDP_API LayerView: public nvDataView {
INHERITED(nvDataView)
public:
  T3DataView_PtrList	ugrps; // #NO_SAVE

  Layer*		layer() const {return (Layer*)m_data;}
  T3LayerNode*		node_so() const {return (T3LayerNode*)m_node_so.ptr();}

  override void		BuildAll(); // creates fully populated subviews

  override bool		Dump_QuerySaveMember(MemberDef* md); // don't save ugs and lower
  T3_DATAVIEWFUNS(LayerView, nvDataView)
protected:
  override void 	ChildRemoving(taDataView* child); // #IGNORE also remove from aux list
  override void		DataUpdateAfterEdit_impl(); // also invoke for the connected prjns
  override void		Render_pre(taDataView* par = NULL); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
private:
  void			Initialize();
  void			Destroy();
};


//////////////////////////
//   PrjnView		//
//////////////////////////

class PDP_API PrjnView: public nvDataView {
#ifndef __MAKETA__
  typedef nvDataView inherited;
#endif
friend class NetView;
public:
  Projection*		prjn() const {return (Projection*)m_data;}
  T3PrjnNode*		node_so() const {return (T3PrjnNode*)m_node_so.ptr();}

  T3_DATAVIEWFUNS(PrjnView, nvDataView)
protected:
  override void		Render_pre(taDataView* par = NULL); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
private:
  void			Initialize();
  void			Destroy();
};


//////////////////////////
//   NetView		//
//////////////////////////

class PDP_API NetViewAdapter: public taBaseAdapter {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
  Q_OBJECT
friend class NetView;
public:
  NetView*		nv() {return (NetView*)owner;}
  NetViewAdapter(NetView* owner_): taBaseAdapter((taOBase*)owner_) {}
public slots:
  void			viewWin_selectionChanged(ISelectable_PtrList& sels);
};

/*
 * Note that we keep simple ptr lists separately of the Layers, Prjns, etc., for ease
 * of iteration
*/

class PDP_API NetView: public T3DataViewPar {
#ifndef __MAKETA__
  typedef T3DataViewPar inherited;
#endif
friend class NetViewAdapter;
friend class NetViewPanel;
public:
  enum MDFlags { // indicates type that unit.disp_base points to
    MD_FLOAT		= 0x0000,
    MD_DOUBLE		= 0x0001,
    MD_INT		= 0x0002,
    MD_UNKNOWN 		= 0x000F
  };

  // NOTE: do not reorder UTD enums == the VALUES and NAMES values are used as bits
  enum UnitTextDisplay {	// how to display unit text
    UTD_NONE		= 0,	// #LABEL_None no text (default)
    UTD_VALUES		= 1,	// #LABEL_Values unit values only
    UTD_NAMES		= 2,	// #LABEL_Names unit names only
    UTD_BOTH		= 3 	// #LABEL_Both both unit values and names
  };

  enum UnitDisplayMode {
    UDM_CIRCLE,	 	// #LABEL_2d_Circle just color, no 3d -- best for when displaying labels
    UDM_RECT,	 	// #LABEL_2d_Rect just color, no 3d -- best for when displaying labels
    UDM_BLOCK,		// #LABEL_3d_Block
    UDM_CYLINDER	// #LABEL_3d_Cylinder
  };

  static NetView*	New(T3DataViewer* viewer, Network* net); // create a new instance and add to viewer


  T3DataView_PtrList	layers; // #NO_SAVE
  T3DataView_PtrList	prjns; // #NO_SAVE
  ColorScale		scale; //contains current min,max,range,zero,auto_scale
  ScaleRange_List 	scale_ranges;  // Auto ranges for member buttons
  bool			display;
  MemberSpace		membs;		// #NO_SAVE list of all the members possible in units
  int_Array	  	ordered_uvg_list; // #HIDDEN selected var buttons
  // unit display flags
  Unit*			unit_src; // #NO_SAVE unit last picked (if any) for display
  MemberDef*		unit_disp_md; // #NO_SAVE memberdef (if any) of Unit (or Connection) to display
  ScaleRange*		unit_sr; // #NO_SAVE scalerange of disp_md
  MDFlags		unit_md_flags; // #NO_SAVE type to display in units
  UnitDisplayMode	unit_disp_mode;
  UnitTextDisplay	unit_text_disp;

  Network*		net() const {return (Network*)m_data;}
  T3NetNode*		node_so() const {return (T3NetNode*)m_node_so.ptr();}
  void 			setUnitSrc(UnitView* uv, Unit* unit); // updates picked unit
  void			setUnitDisp(int value); // sets a new md to display, index in membs
  void			setUnitDispMd(MemberDef* md); // sets a new md to display, lookup/set scale values

  override void		BuildAll(); // creates fully populated subviews

  UnitView*		FindUnitView(Unit* unit); // find the uv for the unit
  virtual void		GetMembs(); //note: called/controlled by the NetViewPanel TODO: net updates???
//obs  ScaleRange* 		GetCurScaleRange(); // returns current NVSR; creates if needed, and copies current values (auto_scale, scale.min/max)
  void 			GetUnitColor(float val,  iColor& col);
  virtual void 		GetUnitDisplayVals(UnitGroupView* ugrv, TwoDCoord& co, float& val,  T3Color& col);

  void			InitScaleRange(ScaleRange& sr); // initialize sr to its defaults; used when creating, and if user clicks 'default' button for the scale
  virtual void		InitDisplay(bool init_panel = true); // hard reset of display, esp. Unit values
  void			InitDisplay_Layer(LayerView* lv, bool check_build = true);
  void			InitDisplay_UnitGroup(UnitGroupView* ugrv, bool check_build = true);
  virtual void		InitPanel(); // hard reset of panel, esp. membr vars
  virtual void		Layer_DataUAE(LayerView* lv); // send a DataUAE for all prjns for this layer
  virtual void		NewLayer(int x = 3, int y = 3);

  virtual void		SelectVar(const char* var_name, bool add=false, bool update = true);
  // select given variable for viewing on units (add to currently disp vars if add)
  void			SetScaleData(bool auto_scale, float scale_min, float scale_max,
    bool update_panel = true); // updates the values in us and the stored ones in the colorscale list
  void 			SetScaleDefault(); //revert scale to its default
  virtual void		UpdateDisplay(bool update_panel = true); // updates dynamic values, esp. Unit values
  virtual void 		UpdatePanel(); // updates nvp, esp. after UAE etc.

  override void		InitLinks();
  override void		CutLinks();
  override void  	ChildUpdateAfterEdit(TAPtr child, bool& handled);
  T3_DATAVIEWFUNS(NetView, T3DataViewPar)

protected:
  Unit*			m_unit_src; // #IGNORE unit last picked (if any) for display
  NetViewPanel*		nvp; // created during first Render
  override void 	ChildAdding(taDataView* child); // #IGNORE also add to aux list
  override void 	ChildRemoving(taDataView* child); // #IGNORE also remove from aux list
  override void		DataUpdateAfterEdit_impl(); //
  override void		OnWindowBind_impl(iT3DataViewer* vw);
  override void		Render_pre(taDataView* par = NULL); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
  void 			UpdateAutoScale(); // #IGNORE prepass updates scale from values
  void			viewWin_selectionChanged(ISelectable_PtrList& sels);
private:
  void			Initialize();
  void			Destroy();
};


//////////////////////////
//   NetViewPanel 	//
//////////////////////////

class PDP_API NetViewPanel: public iViewPanelFrame {
  // frame for gui interface to a NetView -- usually posted by the netview
#ifndef __MAKETA__
typedef iViewPanelFrame inherited;
#endif
  Q_OBJECT
public:

  QHBoxLayout*		layOuter;
  QVBoxLayout*		  layCtrls;
  QHBoxLayout*		    layDispCheck;
  QCheckBox*		      chkDisplay;
  QScrollArea*		    scrCmdButtons;
  QWidget*		    widCmdButtons;
  QGridLayout*		      layCmdButtons;
  QLabel*		    lblDisplayValues;
  Q3ListView*		    lvDisplayValues;
  QVBoxLayout*		  layColorScale;
  QLabel*		    lblUnitText;
  taiComboBox*		    cmbUnitText;
  QLabel*		    lblDispMode;
  taiComboBox*		    cmbDispMode;
  QCheckBox*		    chkAutoScale;       // autoscale ck_box
  QPushButton*		    butScaleDefault;    // revert to default
  ScaleBar*		    cbar;	      // colorbar

  QPushButton*		butNewLayer;
  QPushButton*		butBuildAll;
  QPushButton*		butConnectAll;

  NetView*		nv() {return (NetView*)m_dv;} //

  void			AddCmdButton(QWidget* but); // adds button or other ctrl to cmd grid
  void 			ColorScaleFromData();
  virtual void		GetVars();
  virtual void		InitPanel();

  NetViewPanel(NetView* dv_);
  ~NetViewPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;} //
//  override void		DataLinkDestroying(taDataLink* dl);
//  override void		DataLinkClientRemoving(taDataLink* dl, IDataLinkClient* dlc);
  override TypeDef*	GetTypeDef() const {return &TA_NetViewPanel;}

protected:
  int			cmd_x; // current coords of where to place next button/ctrl
  int			cmd_y;
  override void		GetImage_impl();

protected slots:
  void			butBuildAll_pressed();
  void			butConnectAll_pressed();
  void			butNewLayer_pressed();
  void			butScaleDefault_pressed();
  virtual void 		chkAutoScale_toggled(bool on);
  virtual void 		chkDisplay_toggled(bool on);
  virtual void 		cmbUnitText_itemChanged(int itm);
  virtual void 		cmbDispMode_itemChanged(int itm);
  virtual void		cbar_scaleValueChanged();
  virtual void		lvDisplayValues_selectionChanged();

};


/*obs: following not used anymore
class tabNetworkDataView: public tabDataView {
  // DataView for taList objects -- note that it also manages the ListView nodes
public:
  Network*		data() const {return (Network*)mdata;}
//  override int		NumListCols() const; // number of columns in a list view for this item type
//  override String	GetColHeading(int col) const; // header text for the indicated column
//  override String	ChildGetColText(taiDataView* child, int col, int itm_idx = -1) const;
//  virtual bool		ShowMember(MemberDef* md); // asks this type if we should show the md member
  tabNetworkDataView(Network* data_, taiDataBrowser* browser_);

//public:
//  override void		fileNew(); // this section for all the delegated menu commands

protected:
  override taiDataPanel* CreateDataPanel(T3Node* sel_node);
//TODO  override void		DataChanged_impl(T3Node* nd, int dcr, void* op1, void* op2);
};


//TODO: following not used anymore
class taiNetworkDataPanel: public taiDataPanel {
//  Q_OBJECT
public:
  QVBoxLayout*		layOuter;
  T3ViewspaceWidget*	t3vsw;
//  iRenderAreaWrapper*	ra_w; // render area wrapper
  SoQtRenderArea* 	ra();
  Network*		net() {return ((tabNetworkDataView*)link)->data();}
//TODO  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
//  int 			EditAction_impl(taiMimeSource* ms, int ea, taiDataNode* single_sel_node = NULL);
//  void			emit_SetEditActionsEnabled(int ea); // param is one of the taiClipData EditAction values
//  taiClipData*		GetClipData(taPtrList_impl* sel_list, int src_edit_action, bool for_drag = false);
    // gets clipboard data (called on Cut/Copy or Drag)
//  int			GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
//  taPtrList_impl*	GetSelectedItems(); // list of the selected datanodes -- caller must delete list
  taiDataNode* 		parentItem() {return mparentItem;}
  taiNetworkDataPanel(tabNetworkDataView* link_, T3Node* sel_node_, QWidget* parent = 0, const char* name = 0);
  ~taiNetworkDataPanel();
#ifndef __MAKETA__
signals:
  void			SetEditActionsEnabled(int ea); // sent to Clipboard server; param is one of the taiClipData EditAction values
#endif


protected:
  taiDataNode* 		mparentItem;
  SoSeparator* 		root; //TEMP
  T3NetView*		net_so;
  void			Render();
};

*/




/*TODO

//~replaced by T3NetView
class Network_G : public GraphicMaster {
public:
  Network*	net;
  NetEditor*	owner;
  iColor*	border;	        // 'unselected' border color
  const iBrush* unitbrush;
  TDCoord	max_size;	// for the whole network (sets scaling)
  TDTransform	tdp;		// projection operator
  float		skew;
  MemberSpace	membs;		// list of all the members possible in units
  int		cur_mbr;	// current member to display
  bool		update_scale_only;
  // indicates whether to update only the scale on the next update_from_state()
  bool		changed_pickgroup;
  bool		reinit_display;	// subobjs set this flag to reinit after popping out

  virtual void	UpdateSelect(); // Updates the graphics to reflect the state
  virtual void	UpdatePick();	// of the pickgroup and selectgroup;

  virtual void	SelectObject(TAPtr obj); // selects just this one object
  virtual void	SelectObject_impl(TAPtr obj); // does the hard work..

  bool 		select(const ivEvent& e, Tool& tool, bool);
  bool		effect_select(bool set_select);

  bool		pick_me(const ivEvent& e, Tool& tool, bool);
  virtual void	GetMembs();
  virtual void	SetMax(TDCoord& max);
  virtual void	Reset();
  virtual void	Build();
  virtual void	RePick();

  virtual void	FixProjection(Projection* p, int index);
  virtual void	FixUnit(Unit* u, Layer* lay);
  virtual void	FixUnit_Group(Unit_Group* ug, Layer* lay);
  virtual void	FixLayer(Layer* lay);
  virtual void	RepositionPrjns();

  virtual void	RemoveGraphics();
  virtual bool	LoadGraphic(const char* nm);
  // load a graphic to display in the background of the network display

  virtual iColor* GetLabelColor();

  bool		update_from_state(ivCanvas* c);
  void 		allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext);

  virtual void	SetMyTransform(const ivAllocation& a);
  // set my transformer based on allocation or any frozen transformer on the netview
  virtual void 	ReCenter();
  virtual void 	ScaleCenter(const ivAllocation& a);

  Network_G(Network*);
  ~Network_G();
  GRAPHIC_BASEFUNS(Network_G);
}; */

/*

/////////////////////
// NetViewGraphic  //
/////////////////////

class NetViewGraphic_G : public PolyGraphic {
public:
  NetViewGraphic* 	netvg;

  bool		pickable() { return false; }


  bool	select(const ivEvent& e, Tool& tool, bool unselect);

  bool	effect_stretch(const ivEvent& e, Tool&,
		       ivCoord ix,ivCoord iy,
		       ivCoord fx, ivCoord fy);

  bool	effect_move(const ivEvent& e, Tool& t,
		    ivCoord ix,ivCoord iy,
		    ivCoord fx, ivCoord fy);

  virtual bool	effect_scale(const ivEvent& e, Tool& t,
			     ivCoord ix,ivCoord iy,
			     ivCoord fx, ivCoord fy,
			     ivCoord ctrx, ivCoord ctry);

  virtual bool	effect_rotate(const ivEvent& e, Tool& t ,
			      ivCoord ideg, ivCoord fdeg,
			      ivCoord ctrx, ivCoord ctry);

  virtual void  flush();

  NetViewGraphic_G(NetViewGraphic* n, Graphic* g= nil);
  virtual ~NetViewGraphic_G();
  GRAPHIC_BASEFUNS(NetViewGraphic_G);
};

class NetViewLabelGroup_G : public PolyGraphic {
public:
  bool		pickable() { return false; }

  bool 		effect_move(const ivEvent& ev, Tool& tl, ivCoord ix,
				    ivCoord iy, ivCoord fx, ivCoord fy);

  NetViewLabelGroup_G(Graphic* g);
  virtual ~NetViewLabelGroup_G();
  GRAPHIC_BASEFUNS(NetViewLabelGroup_G);
};

class Layer_G : public PolyGraphic {
public:
  enum laysides { TOP,RIGHT,BOTTOM,LEFT,INDEPENDENT };
  Layer* 	lay;
  Network_G*	netg;
  TDCoord	move_pos;
  ivPolyGlyph*	proj_gs;	// projection glyphs associate with this layer

  virtual void	Position();
  virtual void	Build();
  virtual void	RePick();

  virtual void	RemoveProj_G(Projection_G* pjn_g);
  virtual void	RedistributeSide(int index);
  virtual void	FixText();
  virtual void	FixBox();
  virtual void	FixUnits();
  virtual void	FixUnit_Group(Unit_Group* ug);
  virtual void	FixUnit(Unit* u);
  virtual void	FixAll()	{ FixBox(); FixText(); FixUnits();}

  virtual LayerBox_G* GetActualLayerBox();
  virtual float GetCurrentXformScale();
  virtual float GetCurrentYformScale();
  bool     	select(const ivEvent& e, Tool& tool, bool unselect);
  bool		grasp_move(const ivEvent& e,Tool& tool,ivCoord, ivCoord);
  bool		manip_move(const ivEvent& e,Tool& tool,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
				   ivCoord cx, ivCoord cy);
  // this applies to the layer
  bool 		effect_move(const ivEvent& ev, Tool& tl, ivCoord ix,
				    ivCoord iy, ivCoord fx, ivCoord fy);
  virtual bool	grasp_stretch(const ivEvent&, Tool&,ivCoord ix, ivCoord iy);
  bool		manip_stretch(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
				   ivCoord cx, ivCoord cy);
  bool		effect_stretch(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy);
  virtual bool	effect_scale(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy,
				     ivCoord ctrx, ivCoord ctry);
  virtual void	damage_me(ivCanvas* c);
  void 		draw_gs (ivCanvas* c, Graphic* gs);

  void		flush()	 { };

  ivGlyph*	clone() const;
  Layer_G(Layer* lyr, Network_G* ng);
  virtual ~Layer_G();

  GRAPHIC_BASEFUNS(Layer_G);
};

class LayerNameEdit : public taNBase {
  // used by LayerText_G to edit layer name
public:
  Layer*	layer;		// #HIDDEN the layer for this name

  void	SetLayer(Layer*);
  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy();
  TA_BASEFUNS(LayerNameEdit);
};

class LayerText_G : public NoScale_Text_G {
public:
  Layer_G* 	lay_g;
  LayerNameEdit* lne;		// edit this layer name

  bool		pickable() { return false; }

  ivTransformer* transformer();
  void	transformer(ivTransformer* t);
  bool	effect_move(const ivEvent&, Tool&,ivCoord ix,ivCoord iy,
			    ivCoord fx, ivCoord fy);

  bool 	effect_select(bool set_select);

  LayerText_G(Layer_G* l,const ivFont* font, const iColor* stroke,
	       const char* c, ivTransformer* t);
  ~LayerText_G();
  GRAPHIC_BASEFUNS(LayerText_G);
};

class LayerBox_G : public Polygon {
  // box representing layer and subgroups within layer
public:
  TAPtr		object;		// object that box represents
  PosTDCoord*	geom;		// geometry controlled by this
  Network_G* 	netg;

  bool		pickable() { return false; }

  virtual void 	draw_gs(ivCanvas*, Graphic*);
  bool		effect_select(bool set_select);

  void		SetPointsFmGeom();	// from geometry
  void 		SetPoints(ivCoord x0,ivCoord y0, ivCoord x1, ivCoord y1,
			  ivCoord x2,ivCoord y2, ivCoord x3, ivCoord y3);
  void 		GetPoints(ivCoord* x0,ivCoord* y0, ivCoord* x1, ivCoord* y1,
			  ivCoord* x2,ivCoord* y2, ivCoord* x3, ivCoord* y3);

  // cancel these out
  bool	grasp_move(const ivEvent&,Tool&,ivCoord, ivCoord){return true;}
  bool	manip_move(const ivEvent& ,Tool&,ivCoord,ivCoord,ivCoord,
			   ivCoord,ivCoord,ivCoord) {return true;}
  bool	grasp_scale(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,ivCoord,
			    ivCoord,ivCoord) {return true;}
  bool	manip_scale(const ivEvent&,Tool&,ivCoord,ivCoord,ivCoord,ivCoord,
			    ivCoord,ivCoord,ivCoord,ivCoord) {return true;}

  LayerBox_G(TAPtr obj, PosTDCoord* gm, Network_G* ng,
	      const iBrush* brush, const iColor* stroke,
	      const iColor* fill, ivCoord* x, ivCoord* y,
	      int ctrlpts, ivTransformer*);
  GRAPHIC_BASEFUNS(LayerBox_G);
};

class Unit_Group_G : public PolyGraphic {
public:
  Unit_Group*	group;
  Layer_G*	layg;
  TDCoord	move_pos;

  virtual void	Position();
  virtual void	Build();
  virtual void	RePick();

  virtual void  FixUnits();
  virtual void	FixUnit(Unit* u);

  bool		pickable() { return false; }

  bool		grasp_move(const ivEvent& , Tool& ,ivCoord ix, ivCoord iy);
  bool		manip_move(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
				   ivCoord cx, ivCoord cy);
  bool		effect_move(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord fx, ivCoord fy);
  bool    	select(const ivEvent& e, Tool& tool, bool unselect);

  virtual bool	grasp_stretch(const ivEvent&, Tool&,ivCoord ix, ivCoord iy);
  bool		manip_stretch(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
				   ivCoord cx, ivCoord cy);
  bool		effect_stretch(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy);

  ivGlyph*	clone() const;
  Unit_Group_G(Unit_Group* gp, Layer_G* lg);
  virtual ~Unit_Group_G()			{ };

  GRAPHIC_BASEFUNS(Unit_Group_G);
};

class Projection_G : public PolyGraphic {
public:
  Projection*	pjn;
  Layer_G*	from_g;
  Layer_G*	to_g;
  Network_G*	net_g;
  float		mid_dist;	// distance between midpoints of to/from layers

  ivCoord	l;		// temporary storage for grasp_move ect
  ivCoord	r;		// temporary storage
  ivCoord	t;		// temporary storage
  ivCoord	b;		// temporary storage
  int		move_end;	// 0 = from/send , 1 = to/receive

  bool		pickable() { return false; }

  virtual void	RemoveMe();
  virtual void  GetClosestSides(int& from_side_index,int& to_side_index);
  virtual void  GetSelfProjSides(int& from_side_index,int& to_side_index);
  virtual void 	RecomputePoints();
  virtual void	NewProjection(Projection* p, Layer_G *tg,Layer_G *fg,Network_G* n);

  // finds the closest points
  bool	grasp_move(const ivEvent&, Tool&,ivCoord ix, ivCoord iy);
  bool	manip_move(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
			   ivCoord cx, ivCoord cy);
  bool	effect_move(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy);

  Projection_G();
 ~Projection_G();
  GRAPHIC_BASEFUNS(Projection_G);
};

class PLine_G : public Polyline {
public:
  Projection_G*  proj_g;
  bool		editb_used;	// whether edit button was used

  virtual int   StartIndex() 	{ return 0; }
  virtual int   EndIndex() 	{ return 1; }

  virtual void	SetX(int i,ivCoord val) { _x[i] = val; recompute_shape(); }
  virtual void  SetStartX(ivCoord val) 	{ SetX(StartIndex(),val); }
  virtual void  SetEndX(ivCoord val)	{ SetX(EndIndex(),val); }
  virtual void	AddX(int i,ivCoord val)
  { SetX(i,val+_x[i]); recompute_shape(); }
  virtual void  MaxX(int i,ivCoord val)
  { if (_x[i] > val) SetX(i,val); recompute_shape(); }
  virtual void  MinX(int i,ivCoord val)
  { if (_x[i] < val) SetX(i,val); recompute_shape(); }
  ivCoord 	GetX(int i) { return _x[i]; }
  ivCoord 	GetStartX() { return _x[StartIndex()]; }
  ivCoord 	GetEndX()   { return _x[EndIndex()]; }

  virtual void	SetY(int i,ivCoord val) { _y[i] = val; recompute_shape(); }
  virtual void  SetStartY(ivCoord val) 	{ SetY(StartIndex(),val); }
  virtual void  SetEndY(ivCoord val) 	{ SetY(EndIndex(),val); }
  virtual void	AddY(int i,ivCoord val)
  { _y[i] += val; recompute_shape(); }
  virtual void  MaxY(int i,ivCoord val)
  { if (_y[i] > val) _y[i] = val; recompute_shape(); }
  virtual void  MinY(int i,ivCoord val)
  { if (_y[i] < val) _y[i] = val; recompute_shape(); }
  ivCoord 	GetY(int i) { return _y[i]; }
  ivCoord 	GetStartY() { return _y[StartIndex()]; }
  ivCoord 	GetEndY()   { return _y[EndIndex()]; }

  virtual void 	Set_Start_End(ivCoord x0,ivCoord y0, ivCoord x1, ivCoord y1)
  { SetStartX(x0); SetEndX(x1); SetStartY(y0); SetEndY(y1); }

  virtual void 	Xform_gs(ivCoord* tx, ivCoord* ty, Graphic* gs);
  virtual void 	Xform_Point(ivCoord ix, ivCoord iy, ivCoord* tx, ivCoord* ty,int side);

  void 	draw_gs(ivCanvas*, Graphic*);
  void	getextent_gs(ivCoord& l, ivCoord& b, ivCoord& cx, ivCoord& cy,
  		     ivCoord& tol, Graphic* gs);
  bool 	intersects_gs (BoxObj& userb, Graphic* gs);

  bool	pickable() { return false; }

  bool  select(const ivEvent& e, Tool& tool, bool unselect);
  bool 	effect_select(bool set_select);

  // cancel these out
  bool	grasp_move(const ivEvent&, Tool&,ivCoord,ivCoord){ return true;}
  bool	manip_move(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,ivCoord,
			   ivCoord,ivCoord){ return true;}
  bool	effect_move(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,
			    ivCoord ){return true;}
  bool	grasp_stretch(const ivEvent&, Tool&,ivCoord,ivCoord){return true;}
  bool	manip_stretch(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,
			      ivCoord,ivCoord,ivCoord){return true;}
  bool	effect_stretch(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,
			       ivCoord){return true;}

  PLine_G(Projection_G* pg, const iBrush* brush,
	  const iColor* stroke,  const iColor* fill,
	  ivCoord* x, ivCoord* y, int size,ivTransformer* tx);
  GRAPHIC_BASEFUNS(PLine_G);
};

class SelfCon_PLine_G : public PLine_G {
  // four points plus 2 arrow points
public:
  int 		side_index;

  bool		pickable() { return false; }

  int   	StartIndex() 	{ return 0;}
  int  	 	EndIndex() 	{ return 3;}

  void		SetX(int i,ivCoord val);
  void		SetY(int i,ivCoord val);
  void  	ComputeArrowPoints();
  void  	Xform_gs(ivCoord* tx, ivCoord* ty, Graphic* gs);

  void 		draw_gs(ivCanvas*, Graphic*);

  void 		Initialize()	{ side_index = 0; }
  SelfCon_PLine_G(Projection_G* pg, const iBrush* brush,
	  const iColor* stroke,  const iColor* fill,
	  ivCoord* x, ivCoord* y, int size,ivTransformer* tx);
  GRAPHIC_BASEFUNS(SelfCon_PLine_G);
};

class Unit_G : public PolyGraphic {
  // represents a unit as a collection of unitvalue_g graphics, one for each value being displayed
public:
  Unit*		unit;
  Network_G*	netg;
  ivCoord	x[4]; // x corners
  ivCoord	y[4]; // y corners
  UnitValue_G*	cur_uvg; // for manip_alter
  ivCanvas*	lastcanvas;

  virtual UnitValue_G* NewUnitValue(MemberDef* md);
  virtual void	AddMD(MemberDef* md, bool chosen=true);
  virtual void	ChangeMD(MemberDef* md); // changes the last membdef to md
  virtual void	RemoveMD(MemberDef* md);
  virtual int	FindValue_G(UnitValue_G* uvg);
  virtual void	Fix_UVGS();

  bool 		manipulating(const ivEvent&, Tool&);
  bool		grasp_move(const ivEvent& e,Tool& tool,ivCoord, ivCoord);
  bool		manip_move(const ivEvent& e,Tool& tool,ivCoord ix, ivCoord iy,
			   ivCoord lx, ivCoord ly, ivCoord cx, ivCoord cy);
  bool		effect_move(const ivEvent& e,Tool& tool,ivCoord ix, ivCoord iy,
			    ivCoord fx, ivCoord fy);

  bool		grasp_alter(const ivEvent&, Tool&,
			    ivCoord ix, ivCoord iy, ivCoord ctrx, ivCoord ctry,
			    ivCoord w, ivCoord h);
  bool		manip_alter(const ivEvent&, Tool&,
			    ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
			   ivCoord cx, ivCoord cy, ivCoord ctrx, ivCoord ctry);
  bool		effect_alter(const ivEvent&, Tool&,
			     ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy,
			     ivCoord ctrx, ivCoord ctry);

  bool 		select(const ivEvent& e, Tool& tool, bool unselect);
  virtual void	spread_pick(const ivEvent& e, Tool& tool,bool pick_state);
  // an element was picked, spread to others

  virtual void	Position(); // compute corners of unit space
  virtual void	DistributeUVGS(int index=-1);

  Unit_G(Network_G* ng,Unit* u);

  Unit_G();
 ~Unit_G()		{ };
  GRAPHIC_BASEFUNS(Unit_G);
};

class UnitValue_G : public Graphic {
  // ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS base type for graphical unit rep, handles all value info
public:
  enum MDType {
    UNKNOWN = -1,
    FLOAT,
    DOUBLE,
    INT
  };

  Unit_G* 		ug;
  MemberDef*		spec_md; // memberdef specified as the one to view
  MemberDef*		act_md;	// actual md on unit, NULL if spec_md is not on given unit
  void*			base;
  int			pickindex; // if a con md, this is the index of the picked unit
  MDType		md_type;
  float			prv_val; 	// previous value
  bool			displabel;	// whether this unit displays the label info

  virtual void	UpdateConBase(); // get the base offset for connection objects
  virtual void	SetCurMD(MemberDef* md);
  virtual void	FixType();
  virtual float	GetDisplayVal();

  void 		Position();
  virtual void	RePick();

  virtual void	SetPos(ivCoord x0, ivCoord y0, ivCoord x1,ivCoord y1,
		       ivCoord x2, ivCoord y2, ivCoord x3,ivCoord y3);

  virtual void 	render_text(ivCanvas* c, ScaleBar* cbar, float val, String& str,
			    bool from_top=false);
  // this actually does the drawing
  virtual void 	draw_text(ivCanvas* c, float val);
  // this is the overall control function for draw routine to draw text

  bool		update_from_state(ivCanvas*);

  bool 		select(const ivEvent& e, Tool& tool, bool unselect);
  bool 		pick_me(const ivEvent& e, Tool& tool, bool unselect);

  UnitValue_G(Unit_G* unitg, MemberDef* md, int num, bool curved);
  GRAPHIC_BASEFUNS(UnitValue_G);
};

class SquareUnit_G : public UnitValue_G {
public:
  void 		draw_gs(ivCanvas*, Graphic*);
  virtual void	SetPos(ivCoord x0, ivCoord y0, ivCoord x1,ivCoord y1,
		       ivCoord x2, ivCoord y2, ivCoord x3,ivCoord y3);
  SquareUnit_G(Unit_G*, MemberDef* md=NULL);
  GRAPHIC_BASEFUNS(SquareUnit_G);
};

class AreaUnit_G : public SquareUnit_G {
public:
  iColor* 	fg_color;
  iColor* 	bg_color;

  virtual void	GetColors(ivCanvas* c, Graphic*gs);
  void 		draw_gs(ivCanvas*, Graphic*);
  void		draw_text(ivCanvas* c, float val); // draw text in better contrast color
  ~AreaUnit_G();
  AreaUnit_G(Unit_G* g, MemberDef* md=NULL);
  GRAPHIC_BASEFUNS(AreaUnit_G);
};

class LinearUnit_G : public AreaUnit_G {
public:
  void 		draw_gs(ivCanvas*, Graphic*);
  LinearUnit_G(Unit_G* g, MemberDef* md=NULL);
  GRAPHIC_BASEFUNS(LinearUnit_G);
};

class FillUnit_G : public AreaUnit_G {
public:
  void 		draw_gs(ivCanvas*, Graphic*);
  FillUnit_G(Unit_G* g, MemberDef* md=NULL);
  GRAPHIC_BASEFUNS(FillUnit_G);
};

class DirFillUnit_G : public AreaUnit_G {
public:
  void 		draw_gs(ivCanvas*, Graphic*);
  DirFillUnit_G(Unit_G* g, MemberDef* md=NULL);
  GRAPHIC_BASEFUNS(DirFillUnit_G);
};

class ThreeDUnit_G : public SquareUnit_G {
public:
  float 	last_height;
  void 		draw_gs(ivCanvas* c, Graphic* g);
  void 		getextent_gs(ivCoord& l, ivCoord& b,
			     ivCoord& cx, ivCoord& cy, ivCoord& tol, Graphic* gs);
  ThreeDUnit_G(Unit_G* g, MemberDef* md=NULL);
  GRAPHIC_BASEFUNS(ThreeDUnit_G);
};

class RoundUnit_G : public UnitValue_G {
public:
  void 		draw_gs(ivCanvas*, Graphic*);
  void		SetPos(ivCoord x0, ivCoord y0, ivCoord x1,ivCoord y1,
		       ivCoord x2, ivCoord y2, ivCoord x3,ivCoord y3);
  RoundUnit_G(Unit_G*, MemberDef* md=NULL);
  GRAPHIC_BASEFUNS(RoundUnit_G);
};

class HgtFieldUnit_G : public SquareUnit_G {
public:
  float		height;
  float		oldval;
  float		corner_heights[4];
  ivPolyGlyph 	neighbors;

  virtual void	GetNeighbors();
  virtual void	ComputeCorners();
  virtual void	ComputeHeight();
  virtual void  UpdateNeighbors(ivCanvas* c);

  void		SetPos(ivCoord x0, ivCoord y0, ivCoord x1,ivCoord y1,
		       ivCoord x2, ivCoord y2, ivCoord x3,ivCoord y3);
  bool		update_from_state(ivCanvas*);
  void 		draw_gs(ivCanvas* c, Graphic* g);
  void 		getextent_gs(ivCoord& l, ivCoord& b,
			     ivCoord& cx, ivCoord& cy, ivCoord& tol, Graphic* gs);

  HgtFieldUnit_G(Unit_G* g, MemberDef* md=NULL);
  GRAPHIC_BASEFUNS(HgtFieldUnit_G);
};

class HgtPeakUnit_G : public HgtFieldUnit_G {
public:
  void 		draw_gs(ivCanvas* c, Graphic* g);
  HgtPeakUnit_G(Unit_G* g, MemberDef* md=NULL);
  GRAPHIC_BASEFUNS(HgtPeakUnit_G);
};

*/



#endif // net_qt_h

