// Copyright, 1995-2011, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef BRAIN_VIEW_H
#define BRAIN_VIEW_H

// could just #include "emergent_base.h" instead of the next two
#include "network_TA_type.h" // to get TA_BrainView, etc.
#include "network_def.h" // EMERGENT_API
#include "netstru_qtso.h" // overkill #include for UnitRef, ScaleRange, etc.
#include "t3viewer.h"
class Network;
class BrainViewPanel;
class BrainVolumeView;

class EMERGENT_API BrainView : public T3DataViewMain {
// ##DUMP_LOAD_POST
INHERITED(T3DataViewMain)

  friend class BrainViewPanel;

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

  static BrainView*	New(Network* net, T3DataViewFrame*& fr); // create a new instance and add to viewer

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

  Network*		net() const;
  T3NetNode*		node_so() const;
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
  virtual void 		UpdatePanel(); // updates nvp, esp. after UAE etc.

  ////////////////////////////////////////////////////////////////
  // misc util functions etc
  virtual void		GetMembs();
  virtual void		GetMaxSize(); // get max size from network

  void 			GetUnitColor(float val, iColor& col, float& sc_val);
  virtual void 		GetUnitDisplayVals(BrainVolumeView* bvv, TwoDCoord& co, float& val,
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

  virtual void		Layer_DataUAE(LayerView* lv);
  // send a DataUAE for all prjns for this layer

  virtual void		UpdateName();
  // update name from network

  virtual void		CopyFromView(BrainView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

  override void		Dump_Load_post();
  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); 
  override const iColor	bgColor(bool& ok) const;
  override void		InitLinks();
  override void		CutLinks();
  override void  	ChildUpdateAfterEdit(taBase* child, bool& handled);
  T3_DATAVIEWFUNS(BrainView, T3DataViewMain) //

// ISelectable i/f
  override GuiContext	shType() const;
  override bool		hasViewProperties() const; //TODO: NUKE, OBS

protected:
  T3DataView_PtrList	prjns; 		// #IGNORE list of prjn objects under us
  BrainViewPanel*	bvp; // created during first Render
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
  SIMPLE_COPY(BrainView)
  void			Initialize();
  void			Destroy();
};

#endif // BRAIN_VIEW_H
