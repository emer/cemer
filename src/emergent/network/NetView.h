// Copyright 2013-2018, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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

#ifndef NetView_h
#define NetView_h 1

// parent includes:
#include <taOBase>
#include <T3DataViewMain>

// member includes:
#include <taTransform>
#include <String_Array>
#include <Network>
#include <LayerRef>
#include <String_Matrix>
#include <CircMatrix>
#include <ColorScale>
#include <ScaleRange_List>
#include <NameVar_Array>
#include <NetStateText>
#include <Completions>

// declare all other types mentioned but not required to include:
class iViewPanelOfNetwork; //
class UnitView; //
class UnitGroupView; //
class LayerView; //
class T3Color;
class T3NetNode; //
class BaseSpec; //
class QKeyEvent; //


eTypeDef_Of(NetViewFontSizes);

class E_API NetViewFontSizes : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Display network display font sizes
INHERITED(taOBase)
public:
  float  net_name;      // #DEF_0.05 network name
  float  net_vals;      // #DEF_0.05 network values (counters, stats)
  float  layer;         // #DEF_0.04 layer names
  float  layer_min;     // #DEF_0.01 minimum font size for layer -- can't go less
  float  layer_vals;    // #DEF_0.03 layer values (stats)
  float  prjn;          // #DEF_0.01 projection names and values
  float  unit;          // #DEF_0.02 unit names and values
  int    un_nm_len;     // #DEF_3 unit name length -- used to compute output name font size
  int    un_val_prec;   // #DEF_2 precision unit value string to generate -- can prevent strings from being too long

  String       GetTypeDecoKey() const override { return "Network"; }

  SIMPLE_COPY(NetViewFontSizes);
  TA_BASEFUNS(NetViewFontSizes);
private:
  void  Initialize();
  void  Destroy()               { };
};

eTypeDef_Of(NetViewParams);

class E_API NetViewParams : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Display misc parameters for the network display
INHERITED(taOBase)
public:
  enum PrjnDisp {               // how to display projections
    L_R_F,                      // all in front: sender is at left of layer, receiver is right
    L_R_B,                      // all in back: sender is at left of layer, receiver is right
    B_F,                        // sender is at back of layer, receiver is front
    NONE,                       // don't display projection lines at all
  };

  bool          xy_square;      // keep the x and y dimensions of the network square (same) -- makes the units square
  float         unit_spacing;   // #DEF_0.05 spacing between units (as a proportion of total space available to render the unit)
  PrjnDisp      prjn_disp;      // how to arrange projection arrows to convey sender/receiver info
  bool          prjn_name;      // #DEF_false whether to display the projection name
  float         prjn_width;     // #DEF_0.002 width of the projection arrows
  float         prjn_trans;     // #DEF_0.5 transparency of the projection arrows
  float         lay_trans;      // #DEF_0.5 transparency of the layer border
  float         unit_trans;     // #DEF_0.6 transparency of the units
  float         laygp_width;    // #DEF_1 width of the layer group lines (as a line width, not as a geometry size relative to normalized network size)
  bool          show_laygp;     // #DEF_true whether to display layer group boxes in first place

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(NetViewParams);
private:
  void  Initialize();
  void  Destroy()               { };
};


/*
 * Note that we keep simple ptr lists separately of the Layers, Prjns, etc., for ease
 * of iteration
*/


eTypeDef_Of(NetView);

class E_API NetView: public T3DataViewMain {
// ##DUMP_LOAD_POST #INSTANCE network view main object -- represents the network in view, manages all sub-types
INHERITED(T3DataViewMain)
friend class iViewPanelOfNetwork;
public:
  enum MDFlags { // indicates type that unit.disp_base points to
    MD_FLOAT            = 0x0000,
    MD_DOUBLE           = 0x0001,
    MD_INT              = 0x0002,
    MD_UNKNOWN          = 0x000F
  };

  // NOTE: do not reorder UTD enums == the VALUES and NAMES values are used as bits
  enum UnitTextDisplay {        // how to display unit text
    UTD_NONE            = 0,    // #LABEL_None no text (default)
    UTD_VALUES          = 1,    // #LABEL_Values unit values only
    UTD_NAMES           = 2,    // #LABEL_Names unit names only
    UTD_BOTH            = 3     // #LABEL_Both both unit values and names
  };

  enum UnitDisplayMode {
    UDM_CIRCLE,         // #LABEL_2d_Circle just color, no 3d -- best for when displaying labels
    UDM_RECT,           // #LABEL_2d_Rect just color, no 3d -- best for when displaying labels
    UDM_BLOCK,          // #LABEL_3d_Block
    UDM_CYLINDER        // #LABEL_3d_Cylinder
  };

  enum LayerLayout {            // Visual mode of layer position/view
    TWO_D,                      // #LABEL_2D all z = 0, no skew
    THREE_D                     // #LABEL_3D z = layer index, default skew
  };

  // programmers: extend this as needed -- ConSpec::GetPrjnName sets custom names, and this value is decoded in UnitGroupView::UpdateUnitViewBase_Con_impl
  enum ConType {                // where there are multiple types of connections between units, this allows selection of which type to display
    ANY_CON,                    // #LABEL_Any just show the first connection shown -- don't select by name at all
    STD_CON,                    // #LABEL_Std only show standard connections -- projection names start with Fm_
    INHIB_CON,                  // #LABEL_Inhib only show inhib connections -- projection names start with Inhib_
    CTXT_CON,                   // #LABEL_Ctxt only show context connections -- projection names start with Ctxt_
    DEEP_CON,                   // #LABEL_Deep only show deep connections -- projection names start with Deep_
    MARKER_CON,                 // #LABEL_Marker only show MarkerConSpec connections -- projection names start with Marker_
  };

  static NetView*       New(Network* net, T3Panel*& fr); // create a new instance and add to viewer

  bool                  display;        // whether to update the display when values change (under control of programs)
  LayerLayout           lay_layout;     // how to display layers -- 2d or 3d
  bool                  lay_mv;         // whether to display layer move controls when the arrow button is pressed (can get in the way of viewing weights)
  bool                  net_text;       // whether to display network counters, stats, etc
  NetStateText          net_state_text; // holds the list of net text items, which to display, etc.
  bool                  state_items_stale; // #NO_SAVE #NO_COPY #READ_ONLY set to true to trigger updating of state items

  bool                  show_iconified; // show iconified layers -- otherwise they are removed entirely
  MemberSpace           membs;          // #NO_SAVE #NO_COPY #READ_ONLY list of all the members possible in units; note: all items are new clones
  String_Array          cur_unit_vals;  // #NO_COPY #READ_ONLY currently selected unit values to display -- theoretically can display multiple values, but this is not currently supported, so it always just has one entry at most
  String_Array          hot_vars;       // current "hot" variables shown directly in explorer view
  UnitState_cpp*        unit_src;       // #NO_SAVE #NO_COPY #READ_ONLY unit last picked (if any) for display
  String                unit_src_path;  // ##READ_ONLY path of unit_src unit relative to the network -- used for saving and reloading
  String                last_sel_unit_val;   // #READ_ONLY #SHOW #NO_SAVE value of last selected unit (for display)

  ConType               con_type;       // what type of connections should be shown (where there are multiple connections between two units)
  String                prjn_starts_with; // #NO_SAVE #NO_COPY #READ_ONLY based on the con_type setting, what the projection name should start with
  MemberDef*            unit_disp_md;   // #NO_SAVE #NO_COPY #READ_ONLY memberdef (if any) of Unit (or Connection) to display
  int                   unit_disp_idx;  // #NO_SAVE #NO_COPY #READ_ONLY index of memberdef (if any) of Unit (or Connection) to display
  int                   n_counters;     // #NO_SAVE #NO_COPY #READ_ONLY number of counter variables on the network object
  String_Matrix         ctr_hist;       // #NO_SAVE #NO_COPY #READ_ONLY [n_counters][hist_max] buffer of history of previous counter data -- saved as text -- used in net_text display
  CircMatrix            ctr_hist_idx;   // #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for ctr_hist
  int                   hist_idx;       // #READ_ONLY history index -- when rewinding backwards, how many steps back from current time to view -- -1 = present
  bool                  hist_save;      // whether to save history right now or not -- can slow things down so need quick option to turn off
  int                   hist_max;       // #MIN_1 how much history of state information to store -- updated each time UpdateUnitValues is called
  int                   hist_ff;        // #MIN_2 how many steps to take in the fast forward/ fast backward interface
  bool                  hist_reset_req; // #NO_SAVE #NO_COPY #READ_ONLY reset of history index position requested because of data reset in one or more of the unit groups
  PosVector2i           movie_size;     // size of movie frame to save for Movie button in netview

  ScaleRange*           unit_scrng;     // #NO_SAVE #NO_COPY #READ_ONLY scalerange of disp_md
  MDFlags               unit_md_flags;  // #NO_SAVE #READ_ONLY type to display in units
  bool                  unit_con_md;    // #NO_SAVE #NO_COPY #READ_ONLY true if memberdef is from a connection as opposed to a direct unit var
  bool                  unit_wt_act;    // #NO_SAVE #NO_COPY #READ_ONLY true if memberdef is r.wt*act 
  UnitDisplayMode       unit_disp_mode; // how to display unit values
  UnitTextDisplay       unit_text_disp; // what labels to display with units
  taVector3f            max_size;       // #NO_COPY #READ_ONLY maximum size in each dimension of the net
  taVector3f            max_size2d;     // #NO_COPY #READ_ONLY maximum size in each dimension of the net for 2D display purposes (using pos2d)
  taVector3f            eff_max_size;   // #NO_COPY #READ_ONLY effective maximum size in each dimension of the net for current view
  NetViewFontSizes      font_sizes;     // font sizes for various items
  NetViewParams         view_params;    // misc view parameters
  bool                  wt_line_disp;   // display weights from selected unit as lines?
  float                 wt_line_width;  // width of weight lines
  float                 wt_line_thr;    // threshold on fabs(wt) value -- don't display below this value
  bool                  wt_line_swt;    // plot sending weights instead of recv weights
  float                 wt_prjn_k_un;   // number of top-k units to project weights through
  float                 wt_prjn_k_gp;   // number of top-k unit groups to project weights through
  LayerRef              wt_prjn_lay;    // #NO_COPY layer to display projected weights for
  bool                  snap_bord_disp; // display snapshot value snap as a border around units
  float                 snap_bord_width; // width of snapshot border lines
  ColorScale            scale;          // contains current min,max,range,zero,auto_scale
  ScaleRange_List       scale_ranges;   // #NO_COPY Auto ranges for member buttons
  NameVar_Array         lay_disp_modes; // #READ_ONLY layer display modes (not properly saved otherwise, due to reset construction of LayerViews)
  String                selected_unit_var;   // when adding a unit to monitor this is the var to monitor
  
  Network*              net() const {return (Network*)data();}
  T3NetNode*            node_so() const {return (T3NetNode*)inherited::node_so();}
  void                  setUnitSrc(UnitView* uv, UnitState_cpp* unit); // updates picked unit
  void                  setUnitDisp(int value); // sets a new md to display, index in membs
  void                  setUnitDispMd(MemberDef* md); // sets a new md to display, lookup/set scale values
  void                  UpdateViewerModeForMd(MemberDef* md);
  // update viewer interaction/viewer mode based on type of member def to view -- only call for interactive view changes

  ////////////////////////////////////////////////////////////////
  // display updating & rendering

  void                  BuildAll() override;
  // creates fully populated subviews (but not So -- that is done in Render)
  virtual void          InitDisplay(bool init_panel = true);
  // hard reset of display, esp. Unit values -- also calls BuildAll.  Note this does not call Render -- that is done by UpdateDisplay, so a full reset is InitDisplay followed by UpdateDisplay
  virtual void          InitPanel();
  // hard reset of panel, esp. membr vars
  virtual void          UpdateDisplay(bool update_panel = true);
  // re-renders entire display (calls Render_impl) -- assumes structure is still same but various display elements may have changed.  if structure is different, then an InitDisplay is required first
  virtual void          UpdateUnitValues();
  // *only* updates unit values -- display and structure must be the same as last time
 
  virtual void          InitCtrHist(bool force = false);
  // initialize counter history based on current settings -- this also serves as master for all history -- if force, then always reset history index positions too
  virtual void          SaveCtrHist();
  // save counter history -- called in UpdateUnitValues()
  virtual void          UpdatePanel(); // updates nvp, esp. after UAE etc.

  ////////////////////////////////////////////////////////////////
  // for text display of net variable values
  virtual void          GetNetTextItems();
  virtual void          MonitorUpdate();  // some item added or deleted from monitor
  virtual int           GetNetTextItemWidth(const String& name) override;
  // return the display width in chars of the value portion of the state text item
  virtual void          SetNetTextItemWidth(const String& name, int width) override;
  // set the display width in chars of the value portion of the state text item
  virtual void          GetUnitMonitorVar(const String& variable);
  // #BUTTON user has selected a unit to monitor - ask for the variable to monitor
  virtual void          MonitorUnit(Layer* layer, int unit, const String& var);
  // call on monitor to monitor the unit
  String                GetArgForCompletion(const String& method, const String& arg) override;
  virtual void          GetArgCompletionList(const String& method, const String& arg, const String_Array& arg_values, taBase* arg_obj, const String& cur_txt, Completions& completions) override;

  ////////////////////////////////////////////////////////////////
  // misc util functions etc
  virtual void          GetMembs();
  virtual void          GetMaxSize(); // get max size from network
  virtual UnitState_cpp* UnitSrc(); // get the currently picked unit -- source unit for viewing weights -- based on unit_src_path

  void                  GetUnitColor(float val, iColor& col, float& sc_val);
  // #IGNORE 
  virtual void          GetUnitDisplayVals
    (UnitGroupView* ugrv, const taVector2i& co, float& val, iColor& col, float& sc_val);
  void                  InitScaleRange(ScaleRange& sr);
  // initialize sr to its defaults; used when creating, and if user clicks 'default' button for the scale

  UnitView*             FindUnitView(UnitState_cpp* unit); // find the uv for the unit
  virtual void          SelectVar(const char* var_name, bool add=false, bool update = true);
  // select given variable for viewing on units (add to currently disp vars if add)
  void                  SetScaleData(bool auto_scale, float scale_min, float scale_max,
    bool update_panel = true); // updates the values in us and the stored ones in the colorscale list
  void                  SetScaleDefault(); //revert scale to its default

  void                  SetColorSpec(ColorScaleSpec* color_spec);
  // #BUTTON #INIT_ARGVAL_ON_scale.spec set the color scale spec to determine the palette of colors representing values

  virtual void          SetLayDispMode(const String& lay_nm, int disp_md);
  // set the layer display mode value for given layer name (called by LayerView UAE)
  virtual int           GetLayDispMode(const String& lay_nm);
  // get the layer display mode value for given layer name (called by LayerView BuildAll)
  virtual void          SetHighlightSpec(BaseSpec* spec);
  virtual bool          InitHotVars(); // init default "hot" vars from directives
  static bool           UsesSpec(taBase* obj, BaseSpec* spec);

  virtual void          NewLayer(int x = 3, int y = 3);
  virtual void          Layer_DataUAE(LayerView* lv);
  // send a DataUAE for all prjns for this layer

  virtual void          UpdateName();
  // update name from network
  void                  ClearCaption();
  
  virtual void          CopyFromView(NetView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

  virtual String        HistMemUsed();
  // #BUTTON #USE_RVAL returns amount of memory used to store history in user-readable format

  virtual bool          HistBackAll();
  // move through history of saved values -- back all the way
  virtual bool          HistBackF();
  // move through history of saved values -- back fast
  virtual bool          HistBack1();
  // move through history of saved values -- back 1
  virtual bool          HistFwd1();
  // move through history of saved values -- forward 1
  virtual bool          HistFwdF();
  // move through history of saved values -- forward fast
  virtual bool          HistFwdAll();
  // move through history of saved values -- forward all the way

  virtual void          HistMovie(int x_size=720, int y_size=720,
                                  const String& fname_stub = "movie_img_");
  // #BUTTON record individual frames of the netview display from current position through to the end of the history buffer, as movie frames -- use ffmpeg http://ffmpeg.org to compile the individual PNG frames into an mp4 movie -- e.g., ffmpeg -framerate 10 -i movie_img_%05d.png -vcodec libx264 -pix_fmt yuv420p -crf 25 movie.mp4
  
  virtual void          unTrappedKeyPressEvent(QKeyEvent* e);
  // #IGNORE process key presses from examiner viewer -- for arrow-key navigation

  inline taVector3f  LayerPosToCoin3D(const taVector3i& pos)
  { taVector3f posn = pos; posn /= eff_max_size;
    float t = posn.z; posn.z = -posn.y; posn.y = t; return posn; }
  // #IGNORE convert to coin coordinate system

  bool         ShowDraggers() const override { return lay_mv; }
  void         DataUnitsXForm(taVector3f& pos, taVector3f& size) override;

  void         SaveImageSVG(const String& svg_fname) override;

  void         Dump_Load_post() override;
#ifndef __MAKETA__
  DumpQueryResult Dump_QuerySaveMember(MemberDef* md) override;
  GuiContext   shType() const override {return GC_DUAL_DEF_VIEW;}
#endif
  const iColor bgColor(bool& ok) const override; // #IGNORE 
  void         InitLinks() override;
  void         CutLinks() override;
  void         ChildUpdateAfterEdit(taBase* child, bool& handled) override;
  T3_DATAVIEWFUNS(NetView, T3DataViewMain) //

// ISelectable i/f
  bool         hasViewProperties() const override { return true; } //TODO: NUKE, OBS

protected:
  T3DataView_PtrList    prjns;          // #IGNORE list of prjn objects under us
  iViewPanelOfNetwork*  nvp; // created during first Render
  bool                  no_init_on_rerender; // set by some routines to prevent init on render to avoid losing history data -- only when known to be safe..
  LayerLayout           prev_lay_layout;  // #IGNORE previous layer layout -- for detecting changes

  void         UpdateAfterEdit_impl() override;
  void         ChildAdding(taDataView* child) override; // #IGNORE also add to aux list
  void         ChildRemoving(taDataView* child) override; // #IGNORE also remove from aux list
  void         SigRecvUpdateView_impl() override;
  void         SigRecvUpdateAfterEdit_impl() override; //
  void         SigRecvUpdateAfterEdit_Child_impl(taDataView* chld) override; // called by lays and prjns
  void         OnWindowBind_impl(iT3Panel* vw) override;
  void         Render_pre() override; // #IGNORE
  void         Render_impl() override; // #IGNORE
  void         RenderStateValues();
  void         Render_wt_lines();
  void         Reset_impl() override; // #IGNORE
  void         UpdateAutoScale(); // #IGNORE prepass updates scale from values
  void         viewWin_NotifySignal(ISelectableHost* src, int op);
  
private:
  SIMPLE_COPY(NetView)
  void         Initialize();
  void         Destroy();
};

#endif // NetView_h

