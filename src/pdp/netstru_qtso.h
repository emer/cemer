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

#include "pdp_base.h"
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
INHERITED(T3DataViewPar)
public:
  NetView*		nv();
  
  virtual void		setDefaultColor() {} // restore to its default color
  virtual void		setHighlightColor(const T3Color& color) {} 
    // used for highlighting in gui, esp lay/prjn

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
  Unit*			unit() const {return (Unit*)data();}
  UnitGroupView*	ugrv() {return GET_MY_OWNER(UnitGroupView);}
  T3UnitNode*		node_so() const {return (T3UnitNode*)m_node_so.ptr();}
  NetView*		nv();

  override void		CutLinks();
  T3_DATAVIEWFUNS(UnitView, T3DataView)
protected:
  NetView*		m_nv; // cache
  override void 	Render_pre(); //
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

  Unit_Group*		ugrp() const {return (Unit_Group*)data();}
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
  void 			UpdateUnitViewBase_Bias_impl(MemberDef* disp_md); // for bias vals
  override void		DoActionChildren_impl(DataViewAction acts);
  virtual void 		Render_impl_children(); // #IGNORE we trap this in DoActionChildren
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
private:
  void			Initialize();
  void			Destroy();
};


class PDP_API nvhDataView: public nvDataView { // #VIRT_BASE highlightable guys
INHERITED(nvDataView)
public:
  void			setDefaultColor() {DoHighlightColor(false);} 
    // restore to its default color
  void			setHighlightColor(const T3Color& color); 
    // used for highlighting in gui, esp lay/prjn
    
  void Copy_(const nvhDataView& cp);
  COPY_FUNS(nvhDataView, inherited);
  TA_BASEFUNS(nvhDataView)
protected:
  T3Color 		m_hcolor; // hilight color (saved for rebuilds)
  
  virtual void		DoHighlightColor(bool apply) {} 

private:
  void			Initialize() {}
  void			Destroy() {}
};



//////////////////////////
//   LayerView	//
//////////////////////////

class PDP_API LayerView: public nvhDataView {
INHERITED(nvhDataView)
public:
  T3DataView_PtrList	ugrps; // #NO_SAVE

  Layer*		layer() const {return (Layer*)data();}
  T3LayerNode*		node_so() const {return (T3LayerNode*)m_node_so.ptr();}

  override void		BuildAll(); // creates fully populated subviews

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); // don't save ugs and lower
  T3_DATAVIEWFUNS(LayerView, nvDataView)
protected:
  override void 	ChildRemoving(taDataView* child); // #IGNORE also remove from aux list
  override void		DoHighlightColor(bool apply); 
  override void		DataUpdateAfterEdit_impl(); // also invoke for the connected prjns
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
private:
  void			Initialize();
  void			Destroy();
};


//////////////////////////
//   PrjnView		//
//////////////////////////

class PDP_API PrjnView: public nvhDataView {
INHERITED(nvhDataView)
friend class NetView;
public:
  Projection*		prjn() const {return (Projection*)data();}
  T3PrjnNode*		node_so() const {return (T3PrjnNode*)m_node_so.ptr();}

  T3_DATAVIEWFUNS(PrjnView, nvDataView)
protected:
  override void		DoHighlightColor(bool apply); 
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
private:
  void			Initialize();
  void			Destroy();
};


//////////////////////////
//   NetView		//
//////////////////////////

/*
 * Note that we keep simple ptr lists separately of the Layers, Prjns, etc., for ease
 * of iteration
*/

class PDP_API NetView: public T3DataViewPar {
INHERITED(T3DataViewPar)
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

  static NetView*	New(Network* net, T3DataViewFrame*& fr); // create a new instance and add to viewer


  T3DataView_PtrList	layers; // #NO_SAVE
  T3DataView_PtrList	prjns; // #NO_SAVE
  ColorScale		scale; //contains current min,max,range,zero,auto_scale
  ScaleRange_List 	scale_ranges;  // Auto ranges for member buttons
  bool			display;       // whether to update the display when values change (under control of programs)
  MemberSpace		membs;		// #NO_SAVE list of all the members possible in units
  int_Array	  	ordered_uvg_list; // #HIDDEN selected var buttons
  // unit display flags
  Unit*			unit_src; // #NO_SAVE unit last picked (if any) for display
  MemberDef*		unit_disp_md; // #NO_SAVE memberdef (if any) of Unit (or Connection) to display
  ScaleRange*		unit_sr; // #NO_SAVE scalerange of disp_md
  MDFlags		unit_md_flags; // #NO_SAVE type to display in units
  UnitDisplayMode	unit_disp_mode; // how to display unit values
  UnitTextDisplay	unit_text_disp; // what labels to display with units
  FloatTDCoord		max_size;	// maximum size in each dimension of the net
  NetViewFontSizes	font_sizes;	// font sizes for various items
  NetViewParams		view_params;	// misc view parameters 

  Network*		net() const {return (Network*)data();}
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
  virtual void		GetMaxSize(); // get max size from network

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
  override void		DataUpdateAfterEdit_Child_impl(taDataView* chld); // called by lays and prjns
  override void		OnWindowBind_impl(iT3DataViewFrame* vw);
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
  void 			UpdateAutoScale(); // #IGNORE prepass updates scale from values
  void			viewWin_NotifySignal(ISelectableHost* src, int op);
private:
  void			Initialize();
  void			Destroy();
};


//////////////////////////
//   NetViewPanel 	//
//////////////////////////

class PDP_API NetViewPanel: public iViewPanelFrame {
  // frame for gui interface to a NetView -- usually posted by the netview
INHERITED(iViewPanelFrame)
  Q_OBJECT
public:

  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layDispCheck;
  QCheckBox*		    chkDisplay;
  QLabel*		    lblUnitText;
  taiComboBox*		    cmbUnitText;
  QLabel*		    lblDispMode;
  taiComboBox*		    cmbDispMode;
  iMethodButtonFrame*	    widCmdButtons;

  QGroupBox*		  gbDisplayValues;
  QVBoxLayout*		  layDisplayValues;
  QHBoxLayout*		    layColorScaleCtrls;
  QCheckBox*		      chkAutoScale;       // autoscale ck_box
  QPushButton*		      butScaleDefault;    // revert to default
  ScaleBar*		    cbar;	      // colorbar
  Q3ListView*		    lvDisplayValues;
  
  QGroupBox*		  gbSpecs;
  QVBoxLayout*		  laySpecs;
  iTreeView*		    tvSpecs;
  
  QPushButton*		butNewLayer;
  QPushButton*		butBuildAll;
  QPushButton*		butConnectAll;

  NetView*		nv() {return (NetView*)m_dv;} //

  void			AddCmdButton(QWidget* but); // adds button or other ctrl to cmd grid
  void 			ColorScaleFromData();
  void 			ExpandAllSpecs();
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
  BaseSpec*		m_cur_spec; // cur spec chosen -- only compared, so ok if stale
  override void		GetImage_impl();
  void 			setHighlightSpec(BaseSpec* spec, bool force = false);

public slots:
  void			viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview

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
  void			tvSpecs_ItemSelected(iTreeViewItem* item); 

};

#endif // net_qt_h

