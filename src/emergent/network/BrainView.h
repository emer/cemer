// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef BrainView_h
#define BrainView_h 1

// parent includes:
#include "network_def.h"
#include <T3DataViewMain>

// member includes:
#include <UnitRef>
#include <ColorScale>
#include <ScaleRange_List>
#include <taVector3f>
#include <T3DataView_List>
#include <String_Array>
#include <taVector3i>

// declare all other types mentioned but not required to include:
class Network; //
class BrainViewPanel; //
class BrainVolumeView; //
class NiftiReader; // #IGNORE
class TalairachAtlas; // #IGNORE
class T3NetNode; // #IGNORE
class T3Color;

TypeDef_Of(NetViewFontSizes);

class EMERGENT_API BrainViewParams : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Display network display font sizes
INHERITED(taOBase)
public:
  float  net_name;      // #DEF_0.05 network name font size
  float  net_vals;      // #DEF_0.05 network values font size (counters, stats)
  float  unit_trans;    // #DEF_0.6 transparency of the units
  float  laygp_width;    // #DEF_1 width of the layer group lines (as a line width, not as a geometry size relative to normalized network size)

  override String       GetTypeDecoKey() const { return "Network"; }

  SIMPLE_COPY(BrainViewParams);
  TA_BASEFUNS(BrainViewParams);
private:
  void  Initialize();
  void  Destroy()               { };
};

TypeDef_Of(BrainView);

class EMERGENT_API BrainView : public T3DataViewMain {
  // ##DUMP_LOAD_POST main view object for glass brain view
INHERITED(T3DataViewMain)

  friend class BrainViewPanel;
  friend class BrainVolumeView;

public:
  enum MDFlags { // indicates type that unit.disp_base points to
    MD_FLOAT    = 0x0000,
    MD_DOUBLE   = 0x0001,
    MD_INT      = 0x0002,
    MD_UNKNOWN  = 0x000F
  };
  
  enum AnatomicalPlane // the anatomical plane to display
  {
    AXIAL       = 0x0000,
    SAGITTAL    = 0x0001,
    CORONAL     = 0x0002
  };
  
  enum StateChange // the type of view state change which determines whether full render is required
  {
    NONE        = 0x0000,
    MINOR       = 0x0001,
    MAJOR       = 0x0002
  };
  
  void                  AsyncRenderUpdate();

  static BrainView*     New(Network* net, T3DataViewFrame*& fr); // create a new instance and add to viewer

  bool                  display;        // whether to update the display when values change (under control of programs)
  bool                  lay_mv;         // keep this..
  bool                  net_text;       // whether to display text box below network with counters etc
  taTransform           net_text_xform;  // transform of coordinate system for the net text display element
  float                 net_text_rot;    // rotation of the text in the Z plane (in degrees) - default is upright, but if text area is rotated, then a different angle might work better
  MemberSpace           membs;          // #NO_SAVE #NO_COPY #READ_ONLY list of all the members possible in units; note: all items are new clones
  String_Array          cur_unit_vals;  // #NO_COPY #READ_ONLY currently selected unit values to display -- theoretically can display multiple values, but this is not currently supported, so it always just has one entry at most
  UnitRef               unit_src;       // #NO_SAVE #NO_COPY #READ_ONLY unit last picked (if any) for display
  String                unit_src_path;  // ##READ_ONLY path of unit_src unit relative to the network -- used for saving and reloading
  bool                  unit_con_md;    // #NO_SAVE #NO_COPY #READ_ONLY true if memberdef is from a connection as opposed to a direct unit var
  MemberDef*            unit_disp_md;   // #NO_SAVE #NO_COPY #READ_ONLY memberdef (if any) of Unit (or Connection) to display
  int                   unit_disp_idx;  // #NO_SAVE #NO_COPY #READ_ONLY index of memberdef (if any) of Unit (or Connection) to display
  int                   n_counters;     // #NO_SAVE #NO_COPY #READ_ONLY number of counter variables on the network object

  ScaleRange*           unit_sr;        // #NO_SAVE #NO_COPY #READ_ONLY scalerange of disp_md
  MDFlags               unit_md_flags;  // #NO_SAVE #READ_ONLY type to display in units
  BrainViewParams       view_params;    // misc view parameters
  ColorScale            scale;          // contains current min,max,range,zero,auto_scale
  ScaleRange_List       scale_ranges;   // #NO_COPY Auto ranges for member buttons

  Network*              net() const;
  T3NetNode*            node_so() const;
  void                  setUnitSrc(Unit* unit); // updates picked unit
  void                  setUnitDisp(int value); // sets a new md to display, index in membs
  void                  setUnitDispMd(MemberDef* md); // sets a new md to display, lookup/set scale values
  void                  UpdateViewerModeForMd(MemberDef* md);
  // update viewer interaction/viewer mode based on type of member def to view -- only call for interactive view changes

  ////////////////////////////////////////////////////////////////
  // display updating & rendering

  override void         BuildAll();
  // creates fully populated subviews (but not So -- that is done in Render)
  virtual void          InitDisplay(bool init_panel = true);
  // hard reset of display, esp. Unit values -- also calls BuildAll.  Note this does not call Render -- that is done by UpdateDisplay, so a full reset is InitDisplay followed by UpdateDisplay
  virtual void          InitPanel();
  // hard reset of panel, esp. membr vars

  virtual void          UpdateDisplay(bool update_panel = true);
  // re-renders entire display (calls Render_impl) -- assumes structure is still same but various display elements may have changed.  if structure is different, then an InitDisplay is required first
  virtual void          UpdateUnitValues();
  // *only* updates unit values -- display and structure must be the same as last time
  virtual void          UpdatePanel(); // updates nvp, esp. after UAE etc.

  
  ////////////////////////////////////////////////////////////////
  // view state functions etc
  bool            IsValid() const;
  QString         DataName() const;
  taVector3i         Dimensions() const;
  AnatomicalPlane ViewPlane() const;
  int             SliceStart() const;
  int             SliceEnd() const;
  bool            NumSlicesAreLocked() const;
  int             SliceSpacing() const;
  int             SliceTransparency() const;
  int             UnitValuesTransparency() const;
  int             NumSlicesValid() const;
  int             NumSlices() const;
  int             MaxSlices() const;
  bool            ColorBrain() const;
  QString         ColorBrainRegexp() const;
  bool            ViewAtlas() const;
  QString         ViewAtlasRegexp() const;
  QString         NetworkBrainAreas() const;

  float           SliceTransparencyXformed() const;
  // transformed slice transparency for use in actual rendering	
    
  void            SetDataName(const QString& data_name);
  void            SetDimensions(const taVector3i& dimensions);
  void            SetViewPlane( AnatomicalPlane plane );
  void            SetViewPlane( int plane );
  void            SetSliceStart(int start);
  void            SetSliceEnd(int end);
  void            SetLockSlices(int state);
  void            SetSliceSpacing(int spacing);
  void            SetSliceTransparency(int transparency);
  void            SetUnitValuesTransparency(int transparency);
  void            SetColorBrain(int state);
  void            SetColorBrainRegexp(const QString& regexp);
  void            SetViewAtlas(int state);
  void            SetViewAtlasRegexp(const QString& regexp);

  bool          state_valid;
  String        brain_data_name;
  taVector3i    brain_dimensions;
  AnatomicalPlane   view_plane;
  int           slice_start;
  int           slice_end;
  int           num_slices;
  bool          lock_num_slices;
  int           slice_spacing;
  int           slice_transparency;
  int           unit_val_transparency;
  int           last_state_change;
  bool          color_brain;
  String        color_brain_regexp;
  bool          show_atlas;
  String        brain_area_regexp;
  
  bool          ValidSliceStart() const;
  bool          ValidSliceEnd() const;
  void          ValidateState();
  void          EmitAndClearState();
  void          EmitAll();
  
  ////////////////////////////////////////////////////////////////
  // misc util functions etc
  virtual void          GetMembs();

  void                  GetUnitColor(float val, iColor& col, float& sc_val);
  virtual void          GetUnitDisplayVals(BrainVolumeView* bvv, Unit* u, float& val,
                                       T3Color& col, float& sc_val);
  virtual void          GetUnitDisplayVals(BrainVolumeView* bvv, taVector2i& co, float& val,
                                           T3Color& col, float& sc_val);
  void                  InitScaleRange(ScaleRange& sr);
  // initialize sr to its defaults; used when creating, and if user clicks 'default' button for the scale

  virtual void          SelectVar(const char* var_name, bool add=false, bool update = true);
  // select given variable for viewing on units (add to currently disp vars if add)
  void                  SetScaleData(bool auto_scale, float scale_min, float scale_max,
    bool update_panel = true); // updates the values in us and the stored ones in the colorscale list
  void                  SetScaleDefault(); //revert scale to its default

  void                  SetColorSpec(ColorScaleSpec* color_spec);
  // #BUTTON #INIT_ARGVAL_ON_scale.spec set the color scale spec to determine the palette of colors representing values

  virtual void          UpdateName();
  // update name from network

  virtual void          CopyFromView(BrainView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

  override void         Dump_Load_post();
#ifndef __MAKETA__
  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md);
  override GuiContext   shType() const;
#endif
  override const iColor bgColor(bool& ok) const;
  override void         InitLinks();
  override void         CutLinks();
  override void         ChildUpdateAfterEdit(taBase* child, bool& handled);
  T3_DATAVIEWFUNS(BrainView, T3DataViewMain) //

// ISelectable i/f
  override bool         hasViewProperties() const; //TODO: NUKE, OBS

protected:
  T3DataView_List       prjns;          // #IGNORE list of prjn objects under us
  BrainViewPanel*       bvp; // created during first Render
  override void         DataUpdateView_impl();
  override void         DataUpdateAfterEdit_impl(); //
  override void         DataUpdateAfterEdit_Child_impl(taDataView* chld); // called by lays and prjns
  override void         OnWindowBind_impl(iT3DataViewFrame* vw);
  override void         Render_pre(); // #IGNORE
  override void         Render_impl(); // #IGNORE
  void                  Render_net_text();
  override void         Reset_impl(); // #IGNORE
  void                  UpdateAutoScale(); // #IGNORE prepass updates scale from values
  void                  viewWin_NotifySignal(ISelectableHost* src, int op);

private:
  SIMPLE_COPY(BrainView)
  void                  Initialize();
  void                  Destroy();
};

#endif // BrainView_h
