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
#include "ta_qtgroup.h"

#include "emergent_base.h"
#include "netstru.h"
#include "emergent_qtso.h"
//#include "ta_qtdata.h"

#include "safeptr_so.h"
#include "netstru_so.h"

#ifndef __MAKETA__
  #include "irenderarea.h"

  #include <qwidget.h>
#endif

// forwards

class ScaleRange;
class ScaleRange_List;
class UnitGroupView; //
class LayerView; //
class PrjnView; //
class NetView; //
class NetViewPanel; //


//////////////////////////
//   ScaleRange		//
//////////////////////////

class EMERGENT_API ScaleRange : public taOBase {
  // ##NO_TOKENS ##CAT_Display saves scale ranges for different variables viewed in netview
INHERITED(taOBase)
public:
  String	var_name;		// #AKA_name name of variable -- not name of object
  bool		auto_scale;
  float 	min;
  float		max;

  override bool	FindCheck(const String& nm) const
  { return (var_name == nm); }

  void		SetFromScale(ColorScale& cs);
  void		SetFromScaleRange(ColorScale& cs);

  TA_BASEFUNS(ScaleRange);
protected:
  override void		UpdateAfterEdit_impl();

private:
  void 		Copy_(const ScaleRange &cp)
    {auto_scale = cp.auto_scale; min = cp.min; max = cp.max; }
  void 		Initialize()	{ auto_scale = false; min = 0.0f; max = 0.0f;}
  void 		Destroy()	{ }
};


class EMERGENT_API ScaleRange_List : public taList<ScaleRange> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Display list of ScaleRange objects
INHERITED(taList<ScaleRange>)
public:
  void			Initialize() {SetBaseType(&TA_ScaleRange);}
  void 			Destroy() {};
  TA_BASEFUNS_NOCOPY(ScaleRange_List);
};


////////////////////////////////////////////////////
//   nvDataView	

class EMERGENT_API nvDataView: public T3DataViewPar { // #VIRT_BASE most children of NetView
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
  void Copy_(const nvDataView& cp) {m_nv = NULL;}
  void			Initialize();
  void			Destroy() {}
};


////////////////////////////////////////////////////
//   UnitView	

class EMERGENT_API UnitView: public T3DataView {
  // control the display of a single unit -- this is only for non-optimized unit display types (UDM_BLOCK is optimized and renders directly, without these sub-objects)
INHERITED(T3DataView)
  friend class LayerView;
public:
#ifndef __MAKETA__
  int		picked : 1;	// #IGNORE set when unit is picked for display (note: not same as selected)
#endif
  Unit*			unit() const {return (Unit*)data();}
  LayerView*		lay() {return GET_MY_OWNER(LayerView);}
  T3UnitNode*		node_so() const {return (T3UnitNode*)inherited::node_so();}
  NetView*		nv();

  override void		CutLinks();
  T3_DATAVIEWFUNS(UnitView, T3DataView)
protected:
  NetView*		m_nv; // cache
  override void 	Render_pre(); //
  // note: _impl is done by the LayerView
private:
  void Copy_(const UnitView& cp) {m_nv = NULL;}
  void			Initialize();
  void			Destroy() {CutLinks();}
};

////////////////////////////////////////////////////
//   UnitGroupView

class EMERGENT_API UnitGroupView: public nvDataView {
  // does all the rendering of unit values, either direct optimized 3D_BLOCK rendering or managing -- there is ONLY ONE of these objects per layer, and it manages all the units regardless of whether there are sub unit groups
INHERITED(nvDataView)
  friend class LayerView;
public:
  static void		ValToDispText(float val, String& str); // renders the display text, typ 6 chars max

  voidptr_Matrix	uvd_bases; // #IGNORE [x][y][nv->membs.size] void* base pointers to unit values -- computed during Init -- note that bases for all members are encoded, so switching members does not require recompute, and this also speeds history saving
  float_Matrix		uvd_hist; // #IGNORE [x][y][nv->membs.size][hist_max] buffer of history of previous value data -- last (frame) dimension uses circ buffer system for efficient storing and retrieval
  CircMatrix		uvd_hist_idx; // #IGNORE circular buffer indexing of uvd_hist -- last (frame) dimension uses circ buffer system for efficient storing and retrieval

  Layer*		layer() const {return (Layer*)data();}
  T3UnitGroupNode*	node_so() const {return (T3UnitGroupNode*)inherited::node_so();}

  LayerView*		lv() const { return m_lv; }
  void			SetLayerView(LayerView* l) { m_lv = l; }

  void			AllocUnitViewData(); // make sure we have correct space in uvd storage
  override void		BuildAll(); // creates fully populated subviews
  virtual void		InitDisplay();

  float 		GetUnitDisplayVal(const TwoDCoord& co, void*& base);
  // get raw floating point value to display according to current nv settings, at given *logical* coordinate within the layer -- fills in base for this value as well (NULL if not set) -- uses history values if nv hist_idx > 0
  float 		GetUnitDisplayVal_Idx(const TwoDCoord& co, int midx, void*& base);
  // get raw floating point value to display at given member index (< membs.size), at given *logical* coordinate -- fills in base for this value as well (NULL if not set) -- does NOT use history vals ever
  void 			UpdateUnitViewBases(Unit* src_u);
  // update base void* for all current nv->membs, src_u only used for s./r. values
  virtual void		UpdateUnitValues();
  // *only* updates unit values 
  virtual void		UpdateUnitValues_blocks();
  // *only* updates unit values: optimized blocks mode
  virtual void		UpdateUnitValues_outnm();
  // output name mode update
  virtual void		UpdateUnitValues_snap_bord();
  // snap border
  virtual void		UpdateAutoScale(bool& updated);
  // update autoscale values
  virtual void		SaveHist();
  // save current values to usv_hist history -- saves all the possible variables, not just currently displayed one (expensive but worth it..)

  override void		InitLinks();
  override void		CutLinks();
  T3_DATAVIEWFUNS(UnitGroupView, nvDataView)
protected:
  LayerView*    	m_lv;
  void 		UpdateUnitViewBase_Unit_impl(int midx, MemberDef* disp_md); // for unit members
  void 		UpdateUnitViewBase_Sub_impl(int midx, MemberDef* disp_md); // for unit submembers
  void 		UpdateUnitViewBase_Con_impl(int midx, bool is_send, String nm, Unit* src_u);
  // for cons
  void 		UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md);
  // for bias vals
  override void		DoActionChildren_impl(DataViewAction acts);
  virtual void 		Render_impl_children(); // #IGNORE we trap this in DoActionChildren
  virtual void 		Render_impl_blocks(); // optimized blocks
  virtual void 		Render_impl_outnm(); // output name
  virtual void 		Render_impl_snap_bord(); // snap border

  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
private:
  NOCOPY(UnitGroupView)
  void			Initialize();
  void			Destroy();
};


class EMERGENT_API nvhDataView: public nvDataView { // #VIRT_BASE highlightable guys
INHERITED(nvDataView)
public:
  void			setDefaultColor() {DoHighlightColor(false);} 
    // restore to its default color
  void			setHighlightColor(const T3Color& color); 
    // used for highlighting in gui, esp lay/prjn
    
  TA_BASEFUNS(nvhDataView)
protected:
  T3Color 		m_hcolor; // hilight color (saved for rebuilds)
  
  virtual void		DoHighlightColor(bool apply) {} 

private:
  void Copy_(const nvhDataView& cp);
  void			Initialize() {}
  void			Destroy() {}
};


////////////////////////////////////////////////////
//     LayerView

class EMERGENT_API LayerView: public nvhDataView {
INHERITED(nvhDataView)
public:
  enum DispMode {
    DISP_UNITS,				// display units (standard)
    DISP_OUTPUT_NAME,			// display output_name field (what layer has generated)
    DISP_FRAME,				// only display outer frame -- useful when using another viewer for the layer data (e.g., a grid view)
  };

  DispMode		disp_mode; // how to display layer information


  override void		BuildAll(); // creates fully populated subviews
  virtual void		InitDisplay();
  virtual void		UpdateUnitValues(); // *only* updates unit values 

  Layer*		layer() const {return (Layer*)data();}
  T3LayerNode*		node_so() const {return (T3LayerNode*)inherited::node_so();}

  virtual void	UpdateNetLayDispMode();
  // update network's storing of the layer display mode value
  
  virtual void	DispUnits();
  // #BUTTON #VIEWMENU display standard representation of unit values
  virtual void	DispOutputName();
  // #BUTTON #VIEWMENU display contents of output_name on layer instead of unit values
  virtual void	UseViewer(T3DataViewMain* viewer);
  // #BUTTON #VIEWMENU #SCOPE_T3DataViewFrame replace usual unit view display with display from viewer (only displays frame of layer, and aligns given viewer with layer)

  virtual void		UpdateAutoScale(bool& updated);
  virtual void		SetHighlightSpec(BaseSpec* spec);

  override bool		hasViewProperties() const { return true; }

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); // don't save ugs and lower
  T3_DATAVIEWFUNS(LayerView, nvhDataView)
protected:
  override void		UpdateAfterEdit_impl();
  override void		DoHighlightColor(bool apply); 
  override void		DataUpdateAfterEdit_impl(); // also invoke for the connected prjns
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
private:
  NOCOPY(LayerView)
  void			Initialize();
  void			Destroy();
};


////////////////////////////////////////////////////
//   PrjnView	

class EMERGENT_API PrjnView: public nvhDataView {
INHERITED(nvhDataView)
friend class NetView;
public:
  Projection*		prjn() const {return (Projection*)data();}
  T3PrjnNode*		node_so() const {return (T3PrjnNode*)inherited::node_so();}

  virtual void		SetHighlightSpec(BaseSpec* spec);

  T3_DATAVIEWFUNS(PrjnView, nvhDataView)
protected:
  override void		DoHighlightColor(bool apply); 
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Reset_impl(); // #IGNORE
private:
  NOCOPY(PrjnView)
  void			Initialize();
  void			Destroy();
};


////////////////////////////////////////////////////
//     LayerGroupView

class EMERGENT_API LayerGroupView: public nvhDataView {
INHERITED(nvhDataView)
public:
  bool			root_laygp; // true if this is network.layers root layer group

  Layer_Group*		layer_group() const {return (Layer_Group*)data();}
  T3LayerGroupNode*	node_so() const {return (T3LayerGroupNode*)inherited::node_so();}

  override void		BuildAll(); // creates fully populated subviews
  virtual void		UpdateUnitValues(); // *only* updates unit values 
  virtual void		InitDisplay();

  virtual void		UpdateAutoScale(bool& updated);
  virtual void		SetHighlightSpec(BaseSpec* spec);

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); // don't save ugs and lower
  T3_DATAVIEWFUNS(LayerGroupView, nvhDataView)
protected:
  override void		DoHighlightColor(bool apply); 
  override void		DataUpdateAfterEdit_impl(); // also invoke for the connected prjns
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
private:
  NOCOPY(LayerGroupView)
  void			Initialize();
  void			Destroy();
};


////////////////////////////////////////////////////
//   NetViewObj	

class EMERGENT_API NetViewObjView: public T3DataView {
  // view of net view object
INHERITED(T3DataView)
public:
  NetViewObj*		Obj() const { return (NetViewObj*)data();}
  virtual void		SetObj(NetViewObj* ob);
  
  DATAVIEW_PARENT(NetView)

  void 	SetDefaultName() {} // leave it blank
  TA_BASEFUNS_NOCOPY(NetViewObjView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};


/////////////////////////////////////////////////////////////
//		Net View

class EMERGENT_API NetViewFontSizes : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Display network display font sizes
INHERITED(taOBase)
public:
  float	 net_name;	// #DEF_0.05 network name
  float	 net_vals;	// #DEF_0.05 network values (counters, stats)
  float	 layer;		// #DEF_0.04 layer names
  float	 layer_min;	// #DEF_0.01 minimum font size for layer -- can't go less
  float	 layer_vals;	// #DEF_0.03 layer values (stats)
  float  prjn;		// #DEF_0.01 projection names and values
  float	 unit;		// #DEF_0.02 unit names and values
  int	 un_nm_len;	// #DEF_3 unit name length -- used to compute output name font size

  override String 	GetTypeDecoKey() const { return "Network"; }

  SIMPLE_COPY(NetViewFontSizes);
  TA_BASEFUNS(NetViewFontSizes);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class EMERGENT_API NetViewParams : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Display misc parameters for the network display
INHERITED(taOBase)
public:
  enum PrjnDisp {		// how to display projections
    L_R_F,			// all in front: sender is at left of layer, receiver is right
    L_R_B,			// all in back: sender is at left of layer, receiver is right
    B_F,			// sender is at back of layer, receiver is front
  };

  bool		xy_square;	// keep the x and y dimensions of the network square (same) -- makes the units square
  float		unit_spacing;	// #DEF_0.05 spacing between units (as a proportion of total space available to render the unit)
  PrjnDisp	prjn_disp;	// how to arrange projection arrows to convey sender/receiver info
  bool		prjn_name;	// #DEF_false whether to display the projection name
  float		prjn_width;	// #DEF_0.002 width of the projection arrows
  float		prjn_trans;	// #DEF_0.5 transparency of the projection arrows
  float		lay_trans;	// #DEF_0.5 transparency of the layer border
  float		unit_trans;	// #DEF_0.6 transparency of the units
  float		laygp_width;	// #DEF_1 width of the layer group lines (as a line width, not as a geometry size relative to normalized network size)
  bool		show_laygp;	// #DEF_true whether to display layer group boxes in first place

  override String 	GetTypeDecoKey() const { return "Network"; }

  TA_SIMPLE_BASEFUNS(NetViewParams);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

/*
 * Note that we keep simple ptr lists separately of the Layers, Prjns, etc., for ease
 * of iteration
*/

class EMERGENT_API NetView: public T3DataViewMain {
// ##DUMP_LOAD_POST
INHERITED(T3DataViewMain)
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


  bool			display;       	// whether to update the display when values change (under control of programs)
  bool			lay_mv;       	// whether to display layer move controls when the arrow button is pressed (can get in the way of viewing weights)
  bool			net_text;       // whether to display text box below network with counters etc
  FloatTransform	net_text_xform;  // transform of coordinate system for the net text display element
  float			net_text_rot;	 // rotation of the text in the Z plane (in degrees) - default is upright, but if text area is rotated, then a different angle might work better
  MemberSpace		membs;		// #NO_SAVE #NO_COPY #READ_ONLY list of all the members possible in units; note: all items are new clones
  String_Array	  	cur_unit_vals;  // #NO_COPY #READ_ONLY currently selected unit values to display -- theoretically can display multiple values, but this is not currently supported, so it always just has one entry at most
  UnitRef		unit_src; 	// #NO_SAVE #NO_COPY #READ_ONLY unit last picked (if any) for display
  String		unit_src_path;	// ##READ_ONLY path of unit_src unit relative to the network -- used for saving and reloading
  bool			unit_con_md;    // #NO_SAVE #NO_COPY #READ_ONLY true if memberdef is from a connection as opposed to a direct unit var
  MemberDef*		unit_disp_md;   // #NO_SAVE #NO_COPY #READ_ONLY memberdef (if any) of Unit (or Connection) to display
  int			unit_disp_idx;	// #NO_SAVE #NO_COPY #READ_ONLY index of memberdef (if any) of Unit (or Connection) to display
  int			n_counters;	// #NO_SAVE #NO_COPY #READ_ONLY number of counter variables on the network object
  String_Matrix		ctr_hist; 	// #NO_SAVE #NO_COPY #READ_ONLY [n_counters][hist_max] buffer of history of previous counter data -- saved as text -- used in net_text display
  CircMatrix		ctr_hist_idx; 	// #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for ctr_hist
  int			hist_idx;	// #READ_ONLY history index -- when rewinding backwards, how many steps back from current time to view -- -1 = present
  bool			hist_save;	// whether to save history right now or not -- can slow things down so need quick option to turn off
  int			hist_max;	// #MIN_1 how much history of state information to store -- updated each time UpdateUnitValues is called
  int			hist_ff;	// #MIN_2 how many steps to take in the fast forward/ fast backward interface
  bool			hist_reset_req; // #NO_SAVE #NO_COPY #READ_ONLY reset of history index position requested because of data reset in one or more of the unit groups

  ScaleRange*		unit_sr; 	// #NO_SAVE #NO_COPY #READ_ONLY scalerange of disp_md
  MDFlags		unit_md_flags;  // #NO_SAVE #READ_ONLY type to display in units
  UnitDisplayMode	unit_disp_mode; // how to display unit values
  UnitTextDisplay	unit_text_disp; // what labels to display with units
  FloatTDCoord		max_size;	// #NO_COPY #READ_ONLY maximum size in each dimension of the net
  NetViewFontSizes	font_sizes;	// font sizes for various items
  NetViewParams		view_params;	// misc view parameters 
  bool			wt_line_disp;	// display weights from selected unit as lines?
  float			wt_line_width;	// width of weight lines
  float			wt_line_thr;	// threshold on fabs(wt) value -- don't display below this value
  bool			wt_line_swt;	// plot sending weights instead of recv weights
  float			wt_prjn_k_un;	// number of top-k units to project weights through
  float			wt_prjn_k_gp;	// number of top-k unit groups to project weights through
  LayerRef		wt_prjn_lay; 	// #NO_COPY layer to display projected weights for
  bool			snap_bord_disp;	// display snapshot value snap as a border around units
  float			snap_bord_width; // width of snapshot border lines
  ColorScale		scale;		// contains current min,max,range,zero,auto_scale
  ScaleRange_List 	scale_ranges;  	// #NO_COPY Auto ranges for member buttons
  NameVar_Array		lay_disp_modes; // #READ_ONLY layer display modes (not properly saved otherwise, due to reset construction of LayerViews)

  Network*		net() const {return (Network*)data();}
  T3NetNode*		node_so() const {return (T3NetNode*)inherited::node_so();}
  void 			setUnitSrc(UnitView* uv, Unit* unit); // updates picked unit
  void			setUnitDisp(int value); // sets a new md to display, index in membs
  void			setUnitDispMd(MemberDef* md); // sets a new md to display, lookup/set scale values
  void			UpdateViewerModeForMd(MemberDef* md);
  // update viewer interaction/viewer mode based on type of member def to view -- only call for interactive view changes

  ////////////////////////////////////////////////////////////////
  // display updating & rendering

  override void		BuildAll();
  // creates fully populated subviews (but not So -- that is done in Render)
  virtual void		InitDisplay(bool init_panel = true);
  // hard reset of display, esp. Unit values -- also calls BuildAll.  Note this does not call Render -- that is done by UpdateDisplay, so a full reset is InitDisplay followed by UpdateDisplay
  virtual void		InitPanel();
  // hard reset of panel, esp. membr vars

  virtual void		UpdateDisplay(bool update_panel = true);
  // re-renders entire display (calls Render_impl) -- assumes structure is still same but various display elements may have changed.  if structure is different, then an InitDisplay is required first
  virtual void		UpdateUnitValues();
  // *only* updates unit values -- display and structure must be the same as last time
  virtual void		InitCtrHist(bool force = false);
  // initialize counter history based on current settings -- this also serves as master for all history -- if force, then always reset history index positions too
  virtual void		SaveCtrHist();
  // save counter history -- called in UpdateUnitValues()
  virtual void 		UpdatePanel(); // updates nvp, esp. after UAE etc.

  ////////////////////////////////////////////////////////////////
  // misc util functions etc
  virtual void		GetMembs();
  virtual void		GetMaxSize(); // get max size from network

  void 			GetUnitColor(float val, iColor& col, float& sc_val);
  virtual void 		GetUnitDisplayVals(UnitGroupView* ugrv, TwoDCoord& co, float& val,
					   T3Color& col, float& sc_val);
  void			InitScaleRange(ScaleRange& sr);
  // initialize sr to its defaults; used when creating, and if user clicks 'default' button for the scale

  UnitView*		FindUnitView(Unit* unit); // find the uv for the unit
  virtual void		SelectVar(const char* var_name, bool add=false, bool update = true);
  // select given variable for viewing on units (add to currently disp vars if add)
  void			SetScaleData(bool auto_scale, float scale_min, float scale_max,
    bool update_panel = true); // updates the values in us and the stored ones in the colorscale list
  void 			SetScaleDefault(); //revert scale to its default

  void			SetColorSpec(ColorScaleSpec* color_spec);
  // #BUTTON #INIT_ARGVAL_ON_scale.spec set the color scale spec to determine the palette of colors representing values

  virtual void		SetLayDispMode(const String& lay_nm, int disp_md);
  // set the layer display mode value for given layer name (called by LayerView UAE)
  virtual int		GetLayDispMode(const String& lay_nm);
  // get the layer display mode value for given layer name (called by LayerView BuildAll)
  virtual void		SetHighlightSpec(BaseSpec* spec);
  static bool		UsesSpec(taBase* obj, BaseSpec* spec);

  virtual void		NewLayer(int x = 3, int y = 3);
  virtual void		Layer_DataUAE(LayerView* lv);
  // send a DataUAE for all prjns for this layer

  virtual void		UpdateName();
  // update name from network

  virtual void		CopyFromView(NetView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

  virtual String	HistMemUsed();
  // #BUTTON #USE_RVAL returns amount of memory used to store history in user-readable format

  virtual bool		HistBackAll();
  // move through history of saved values -- back all the way
  virtual bool		HistBackF();
  // move through history of saved values -- back fast 
  virtual bool		HistBack1();
  // move through history of saved values -- back 1
  virtual bool		HistFwd1();
  // move through history of saved values -- forward 1
  virtual bool		HistFwdF();
  // move through history of saved values -- forward fast
  virtual bool		HistFwdAll();
  // move through history of saved values -- forward all the way

  virtual void		HistMovie(int x_size=640, int y_size=480, 
				  const String& fname_stub = "movie_img_");
  // #BUTTON record individual frames of the netview display from current position through to the end of the history buffer, as movie frames -- use mjpeg tools http://mjpeg.sourceforge.net/ (pipe png2yuv into mpeg2enc) to compile the individual PNG frames into an MPEG movie, which can then be transcoded (e.g., using VLC) into any number of other formats

  override void		Dump_Load_post();
  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); 
  override const iColor	bgColor(bool& ok) const;
  override void		InitLinks();
  override void		CutLinks();
  override void  	ChildUpdateAfterEdit(taBase* child, bool& handled);
  T3_DATAVIEWFUNS(NetView, T3DataViewMain) //

// ISelectable i/f
  override GuiContext	shType() const {return GC_DUAL_DEF_VIEW;} 
  override bool		hasViewProperties() const { return true; } //TODO: NUKE, OBS

protected:
  T3DataView_PtrList	prjns; 		// #IGNORE list of prjn objects under us
  NetViewPanel*		nvp; // created during first Render
  bool			no_init_on_rerender; // set by some routines to prevent init on render to avoid losing history data -- only when known to be safe..

  override void 	ChildAdding(taDataView* child); // #IGNORE also add to aux list
  override void 	ChildRemoving(taDataView* child); // #IGNORE also remove from aux list
  override void		DataUpdateView_impl();
  override void		DataUpdateAfterEdit_impl(); //
  override void		DataUpdateAfterEdit_Child_impl(taDataView* chld); // called by lays and prjns
  override void		OnWindowBind_impl(iT3DataViewFrame* vw);
  override void		Render_pre(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  void			Render_net_text();
  void			Render_wt_lines();
  override void		Reset_impl(); // #IGNORE
  void 			UpdateAutoScale(); // #IGNORE prepass updates scale from values
  void			viewWin_NotifySignal(ISelectableHost* src, int op);
private:
  SIMPLE_COPY(NetView)
  void			Initialize();
  void			Destroy();
};


//////////////////////////
//   NetViewPanel 	//
//////////////////////////

class EMERGENT_API NetViewPanel: public iViewPanelFrame {
  // frame for gui interface to a NetView -- usually posted by the netview
INHERITED(iViewPanelFrame)
  Q_OBJECT
public:
  QWidget*		widg;
//  QVBoxLayout*		layWidg;

  QVBoxLayout*		layTopCtrls;
  QVBoxLayout*		layViewParams;
  QHBoxLayout*		  layDispCheck;
  QCheckBox*		    chkDisplay;
  QCheckBox*		    chkLayMove;
  QCheckBox*		    chkNetText;
  QLabel*		    lblTextRot;
  taiField*		    fldTextRot;
  QLabel*		    lblUnitText;
  taiComboBox*		    cmbUnitText;
  QLabel*		    lblDispMode;
  taiComboBox*		    cmbDispMode;
  QLabel*		    lblPrjnDisp;
  taiComboBox*		    cmbPrjnDisp;
  QLabel*		    lblPrjnWdth;
  taiField*		    fldPrjnWdth;

  QHBoxLayout*		  layFontsEtc;
  QLabel*		    lblUnitTrans;
  taiField*		    fldUnitTrans;
  QLabel*		    lblUnitFont;
  taiField*		    fldUnitFont;
  QLabel*		    lblLayFont;
  taiField*		    fldLayFont;
  QLabel*		    lblMinLayFont;
  taiField*		    fldMinLayFont;
  QCheckBox*		    chkXYSquare;
  QCheckBox*		    chkLayGp;

  QVBoxLayout*		layDisplayValues;
  QHBoxLayout*		  layColorScaleCtrls;
  QCheckBox*		    chkAutoScale;       // autoscale ck_box
  QPushButton*		    butScaleDefault;    // revert to default  
  QCheckBox*		    chkWtLines;
  QCheckBox*		    chkWtLineSwt;
  QLabel*		    lblWtLineWdth;
  taiField*		    fldWtLineWdth;
  QLabel*		    lblWtLineThr;
  taiField*		    fldWtLineThr;
  QLabel*		    lblWtPrjnKUn;
  taiField*		    fldWtPrjnKUn;
  QLabel*		    lblWtPrjnKGp;
  taiField*		    fldWtPrjnKGp;
  QLabel*		    lblWtPrjnLay;
  taiGroupElsButton*	    gelWtPrjnLay;

  QHBoxLayout*		 layColorBar;
  QCheckBox*		    chkSnapBord;
  QLabel*		    lblSnapBordWdth;
  taiField*		    fldSnapBordWdth;
  QLabel*		    lblUnitSpacing;
  taiField*		    fldUnitSpacing;
  ScaleBar*		   cbar;	      // colorbar
  QPushButton*		   butSetColor;

  QHBoxLayout*		 layHistory;
  QToolBar*		   histTB;
  QCheckBox*		   chkHist;
  taiField*		   fldHistMax;
  taiField*		   fldHistFF;
  QAction* 		   actBack_All;
  QAction* 		   actBack_F;
  QAction* 		   actBack;
  QAction* 		   actFwd;
  QAction* 		   actFwd_F;
  QAction* 		   actFwd_All;
  QLabel*		   lblHist;
  QAction* 		   actMovie;
  
  QTabWidget* 		tw; 
  QTreeWidget*		  lvDisplayValues;
  iTreeView*		  tvSpecs;
  
  iMethodButtonMgr*	meth_but_mgr;
  QWidget*		widCmdButtons;

  NetView*		nv() {return (NetView*)m_dv;} //

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
  BaseSpec*		m_cur_spec; // cur spec chosen -- only compared, so ok if stale
  bool			req_full_render; // when updating, call Render on netview
  bool			req_full_build;	 // when updating, call Build on netview
  override void		UpdatePanel_impl();
  override void		GetValue_impl();
  override void		CopyFrom_impl();
  void 			setHighlightSpec(BaseSpec* spec, bool force = false);

public slots:
  void			viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview
  void			dynbuttonActivated(int but_no); // for hot member buttons

protected slots:
  void			butScaleDefault_pressed();
  void 			butSetColor_pressed();
  void 			hist_back();
  void 			hist_back_f();
  void 			hist_back_all();
  void 			hist_fwd();
  void 			hist_fwd_f();
  void 			hist_fwd_all();
  void 			hist_movie();
  void			lvDisplayValues_selectionChanged();
  void			tvSpecs_CustomExpandFilter(iTreeViewItem* item,
						   int level, bool& expand);
  void			tvSpecs_ItemSelected(iTreeViewItem* item); 
  // note: this one seems a bit defunct for the iTreeView -- replaced with Notify below
  void			tvSpecs_Notify(ISelectableHost* src, int op);

};

#endif // net_qt_h
