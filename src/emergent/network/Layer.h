// Copyright 2017, Regents of the University of Colorado,
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

#ifndef Layer_h
#define Layer_h 1

// parent includes:
#include <SpecUser>

// member includes:
#include <LayerRef>
#include <PosVector3i>
#include <PosVector2i>
#include <XYNGeom>
#include <Projection_Group>
#include <PRerrVals>
#include <String_Matrix>
#include <LayerSpec>
#include <UnitSpec>
#include <Average>
#include <DMemComm>
#include <DMemAggVars>

// declare all other types mentioned but not required to include:
class Network; //
class ProjectBase; //
class DataCol; //

#include <NetworkState_cpp>
#include <UnitState_cpp>

#include <State_main>

eTypeDef_Of(LayerDistances);

class E_API LayerDistances : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Network specifies distance from input/output layers
INHERITED(taOBase)
public:
  int   fm_input;               // how many layers between closest input layer and me (-1 if unknown)
  int   fm_output;              // how many layers between closest output layer and me (-1 if unknown)

  String       GetTypeDecoKey() const override { return "Layer"; }

  SIMPLE_COPY(LayerDistances);
  TA_BASEFUNS_LITE(LayerDistances);
private:
  void  Initialize()            { fm_input = -1; fm_output = -1; }
  void  Destroy()               { };
};

eTypeDef_Of(LayerRelPos);

class E_API LayerRelPos : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Network relative positioning of layers
INHERITED(taOBase)
public:
  enum RelPos {                 // position of this layer relative to other
    ABS_POS,                    // use absolute positioning in abs_pos, not layer-relative positioning
    RIGHT_OF,                   // this is to the right of other (preferred form)
    LEFT_OF,                    // this is to the left of other (use of RIGHT_OF is preferable -- this can lead to negative pos)
    BEHIND,                     // this is behind other (preferred form)
    FRONT_OF,                   // this is in front of other (use of BEHIND is preferable -- this can lead to negative pos)
    ABOVE,                      // for 3d only: this is above other (z+1) (preferred form)
    BELOW,                      // for 3d only: this is below other (z-1) (use of ABOVE is preferable, this can lead to negative pos)
  };

  enum XAlign {                 // alignment in X (horizontal) axis
    LEFT,                       // align left edge of this layer with left of other (+/- offset)
    MIDDLE,                     // align middle of this layer with middle of other (+/- offset)
    RIGHT,                      // align right edge of this layer with right of other (+/- offset)
  };

  enum YAlign {                 // alignment in Y axis
    FRONT,                      // align front edge of this layer with front of other (+/- offset)
    CENTER,                     // align center of this layer with center of other (+/- offset)
    BACK,                       // align back edge of this layer with back of other (+/- offset)
  };

  RelPos        rel;            // relative position of this layer compared to other
  LayerRef      other;          // other layer to position relative to
  XAlign        x_align;        // #CONDSHOW_ON_rel:FRONT_OF,BEHIND,ABOVE,BELOW horizontal (x-axis) alignment relative to other 
  int           x_off;          // #CONDSHOW_ON_rel:FRONT_OF,BEHIND,ABOVE,BELOW how much offset to add for horizontal (x-axis) alignment
  YAlign        y_align;        // #CONDSHOW_ON_rel:LEFT_OF,RIGHT_OF,ABOVE,BELOW Y-axis alignment relative to other
  int           y_off;          // #CONDSHOW_ON_rel:LEFT_OF,RIGHT_OF,ABOVE,BELOW how much offset to add for Y-axis alignment
  int           space;          // #MIN_0 how much space to add between layers -- for ABOVE, BELOW this is how much to add/subtract to y coordinate of this layer in 2D coordinates, to equate for the ABOVE or BELOW position in 3D coordinates that moves one z value up or down -- i.e., the max height of the row of layers that live within a given z coordinate
  
  String        GetTypeDecoKey() const override { return "Layer"; }

  inline bool   IsRel() { return (rel != ABS_POS && other); }
  // is layer using relative positioning

  inline void   SetAbsPos()  { rel = ABS_POS; }
  
  inline void   SetLeftAlign(int x_offset = 0) { x_align = LEFT; x_off = x_offset; }
  // set left X alignment with given offset
  inline void   SetMiddleAlign(int x_offset = 0) { x_align = MIDDLE; x_off = x_offset; }
  // set middle X alignment with given offset
  inline void   SetRightAlign(int x_offset = 0) { x_align = RIGHT; x_off = x_offset; }
  // set right X alignment with given offset

  inline void   SetFrontAlign(int y_offset = 0) { y_align = FRONT; y_off = y_offset; }
  // set front Y alignment with given offset
  inline void   SetCenterAlign(int y_offset = 0) { y_align = CENTER; y_off = y_offset; }
  // set center Y alignment with given offset
  inline void   SetBackAlign(int y_offset = 0) { y_align = BACK; y_off = y_offset; }
  // set back Y alignment with given offset


  virtual bool  ComputePos3D(taVector3i& pos, Layer* lay);
  // compute new 3d position for this layer relative to other layer -- returns false if not using relative positioning (including if other is not set)
  virtual bool  ComputePos2D(taVector2i& pos, Layer* lay);
  // compute new 3d position for this layer relative to other layer -- returns false if not using relative positioning (including if other is not set)
  virtual bool  ComputePos2D_impl(taVector2i& pos, Layer* lay, const taVector2i& oth_pos);
  // #IGNORE common parts of 2d that apply to 3d as well (i.e., not the above, below parts)

  TA_SIMPLE_BASEFUNS(LayerRelPos);
private:
  void  Initialize();
  void  Destroy()               { };
};


eTypeDef_Of(Layer);

class E_API Layer : public SpecUser {
  // ##EXT_lay ##COMPRESS ##CAT_Network ##SCOPE_Network ##HAS_CONDTREE layer containing units
INHERITED(SpecUser)
public:

#include <Layer_core>
  
  String                desc;           // #EDIT_DIALOG Description of this layer -- what functional role it plays, how it maps onto the brain, etc
  Network*              own_net;        // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Structure #NO_SET_POINTER Network this layer is in
  LayerRelPos           pos_rel;        // #CAT_Structure position this layer relative to another layer -- this is recommended and keeps positioning adaptive to layer sizes -- just start with one or a few "anchor" layers with absolute positioning, and position everything else relative to them
  PosVector3i           pos;            // #CAT_Structure #READ_ONLY position of layer relative to owning layer group, or overall network position if none (0,0,0 is lower left hand corner) -- see network ABS_POS flag for which position is used by default -- can use SetRelPos or SetAbsPos to set position either way
  PosVector3i           pos_abs;        // #CAT_Structure #CONDEDIT_ON_pos_rel.rel:ABS_POS absolute position of layer always relative to overall network position (0,0,0 is lower left hand corner) -- not relative to owning layer group
  PosVector2i       	pos2d;		// #CAT_Structure #READ_ONLY 2D network view display position of layer relative to owning layer group, or overall nework position if none (0,0 is lower left hand corner) -- see network ABS_POS flag for which position is used by default -- can use SetRelPos2d or SetAbsPos2d to set position either way
  PosVector2i       	pos2d_abs;	// #CAT_Structure #CONDEDIT_ON_pos_rel.rel:ABS_POS absolute 2D network view display position of layer always relative to the overall nework (0,0 is lower left hand corner)
  float                 disp_scale;     // #DEF_1 #CAT_Structure display scale factor for layer -- multiplies overall layer size -- 1 is normal, < 1 is smaller and > 1 is larger -- can be especially useful for shrinking very large layers to better fit with other smaller layers
  XYNGeom               un_geom;        // #AKA_geom #CAT_Structure two-dimensional layout and number of units within the layer or each unit group within the layer
  bool                  unit_groups;    // #CAT_Structure organize units into functional subgroups within the layer (akin to hypercolumns in cortex), with each unit group having the geometry specified by un_geom
  XYNGeom               gp_geom;        // #CONDSHOW_ON_unit_groups #CAT_Structure geometry of unit sub-groups (if unit_groups) -- this is the layout of the groups, with un_geom defining the layout of units within the groups
  PosVector2i           gp_spc;         // #CONDSHOW_ON_unit_groups #CAT_Structure spacing between unit sub-groups (if unit_groups) -- this is *strictly* for display purposes, and does not affect anything else in terms of projection connectivity calculations etc.
  XYNGeom               flat_geom;      // #EXPERT #READ_ONLY #CAT_Structure geometry of the units flattening out over unit groups -- same as un_geom if !unit_groups; otherwise un_geom * gp_geom -- this is in logical, structural (not display) sizes -- the n here is the total number of  units that will be created
  XYNGeom               disp_geom;      // #AKA_act_geom #HIDDEN #READ_ONLY #CAT_Structure actual view geometry, includes spaces and groups and everything: the full extent of units within the layer
  XYNGeom               scaled_disp_geom; // #AKA_scaled_act_geom #HIDDEN #READ_ONLY #CAT_Structure scaled actual view geometry: disp_scale * disp_geom -- use for view computations
  int                   n_units_built;   // #READ_ONLY #NO_SAVE number of units actually built in this layer -- use this for all iteration over units at the layer level!

  Projection_Group      projections;    // #CAT_Structure group of receiving projections
  Projection_Group      send_prjns;     // #CAT_Structure #HIDDEN #LINK_GROUP #NO_DIFF #NO_SEARCH group of sending projections
  UnitSpec_SPtr         unit_spec;      // #CAT_Structure default unit specification for units in this layer

  LayerDistances        dist;           // #CAT_Structure #READ_ONLY #SHOW distances from closest input/output layers to this layer

  String                output_name;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)
  String_Matrix         gp_output_names; // #NO_SAVE #TREE_SHOW #CAT_Statistic #CONDTREE_ON_unit_groups output_name's for unit subgroups -- name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)
  bool                  units_lesioned; // #GUI_READ_ONLY if units were lesioned in this group, don't complain about rebuilding!
  bool                  gp_unit_names_4d; // #CONDSHOW_ON_unit_groups&&flags:SAVE_UNIT_NAMES if there are unit subgroups, create a 4 dimensional set of unit names which allows for distinct names for each unit in the layer -- otherwise a 2d set of names is created of size un_geom, all unit groups have the same repeated set of names
  String_Matrix         unit_names;     // #TREE_SHOW set unit names from corresponding items in this matrix (dims=2 for no group layer or to just label main group, dims=4 for grouped layers, dims=0 to disable)

  String                brain_area;     // #CAT_Structure #REGEXP_DIALOG #TYPE_BrainAtlasRegexpPopulator Which brain area this layer's units should be mapped to in a brain view.  Must match a label from the atlas chosen for the network.  Layer will not render to brain view if LESIONED flag is checked.
  float                 voxel_fill_pct; // #CAT_Structure #MIN_0 #MAX_1 Percent of brain_area voxels to be filled by units in this layer.

  ProjectBase*          project(); // #IGNORE this layer's project

  NetworkState_cpp*     GetValidNetState() const;
  // #CAT_State get our network state -- only will be returned if network is built and intact

  UnitState_cpp* UnitAtDispCoord(int x, int y) const;
  // #CAT_Access get unitstate at given *display* coordinates relative to layer -- this takes into account spaces between groups etc

  ////////////  layer display position computation

  bool          InLayerSubGroup();
  // #CAT_Structure is this layer in a layer subgroup or directly in network.layers main layer group?
  void          AddRelPos(taVector3i& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above
  void          AddRelPos2d(taVector2i& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above
  virtual void  UpdateLayerGroupGeom();
  // #IGNORE update our owning layer group geometry (auto called after repositioning layers)

  virtual void  PositionUsRelativeToDropped(Layer* lay, LayerRelPos::RelPos rel);
  // #CAT_IGNORE #DROP1 position this layer relative to the dropped layer -- set the rel_pos other for this layer from the dropped layer -- for drag-and-drop configuring
  virtual void  PositionDroppedRelativeToUs(Layer* lay, LayerRelPos::RelPos rel);
  // #CAT_IGNORE #DROP1 position the dropped layer relative to this layer -- set the rel_pos other for the dropped layer from this layer -- for drag-and-drop configuring
  virtual void  ConnectUsFromDropped(Layer* lay);
  // #CAT_IGNORE #DROP1 create a new receiving connection from the layer that was dropped onto this one
  virtual void  ConnectDroppedFromUs(Layer* lay);
  // #CAT_IGNORE #DROP1 create a new receiving connection in the dropped layer from us
  virtual void  ConnectDroppedBidir(Layer* lay);
  // #CAT_IGNORE #DROP1 create new bidrectional connections between the dropped layer and us

  inline void   GetAbsPos(taVector3i& abs_pos) { abs_pos = pos_abs; }
  // #CAT_Structure get absolute pos, which factors in offsets from layer groups
  inline void   GetRelPos(taVector3i& rel_pos) { rel_pos = pos; }
  // #CAT_Structure get relativeabsolute pos, which factors in offsets from layer groups
  inline void   GetAbsPos2d(taVector2i& abs_pos) { abs_pos = pos2d_abs; }
  // #CAT_Structure get absolute pos, which factors in offsets from layer groups
  virtual void  SetRelPos(taVector3i& ps);
  // #CAT_Structure set position of layer -- if in a layer group, this is relative to the owning layer group position, otherwise relative to network 0,0,0
  virtual void  SetRelPos(int x, int y, int z);
  // #CAT_Structure set position of layer -- if in a layer group, this is relative to the owning layer group position, otherwise relative to network 0,0,0
  virtual void  SetAbsPos(taVector3i& ps);
  // #CAT_Structure set absolute position of layer, regardless of whether it is in a layer group or not - always relative to network 0,0,0
  virtual void  SetAbsPos(int x, int y, int z);
  // #CAT_Structure set absolute position of layer, regardless of whether it is in a layer group or not - always relative to network 0,0,0
  virtual void  SetRelPos2d(taVector2i& ps);
  // #CAT_Structure set 2D position of layer -- if in a layer group, this is relative to the owning layer group position, otherwise relative to network 0,0
  virtual void  SetRelPos2d(int x, int y);
  // #CAT_Structure set 2D position of layer -- if in a layer group, this is relative to the owning layer group position, otherwise relative to network 0,0
  virtual void  SetAbsPos2d(taVector2i& ps);
  // #CAT_Structure set absolute 2D position of layer, regardless of whether it is in a layer group or not - always relative to network 0,0
  virtual void  SetAbsPos2d(int x, int y);
  // #CAT_Structure set absolute 2D position of layer, regardless of whether it is in a layer group or not - always relative to network 0,0
  virtual void  MovePos(int x, int y, int z=0);
  // #CAT_Structure move position of layer given increment from where it currently is, in 3d space
  virtual void  MovePos2d(int x, int y);
  // #CAT_Structure move 2d position of layer given increment from where it currently is
  
  virtual void  SetDefaultPos();
  // #IGNORE initialize position of layer

  virtual void  PositionRightOf(Layer* lay, int space = 2);
  // #CAT_Structure position this layer to the right of given other layer -- does this for both 3D and 2D displays, with given amount of space -- sets the pos_rel settings
  virtual void  PositionBehind(Layer* lay, int space = 2);
  // #CAT_Structure position this layer behind other layer -- does this for both 3D and 2D displays, with given amount of space -- sets the pos_rel settings
  virtual void  PositionAbove(Layer* lay, int space = 2);
  // #CAT_Structure position this layer above other layer -- does this for both 3D and 2D displays, with given amount of space -- sets the pos_rel settings
  
  virtual void  SetNUnits(int n_units);
  // #CAT_Structure set number of units in layer in the un_geom member -- attempts to lay out geometry in closest to a square that fits all the units evenly, if possible.  note: does NOT rebuild the network.  also does not make any changes if current number of units is same as arg, or arg <= 0 (e.g., for startup arg, just init n_units to -1 prior to getting arg val so it won't have any effect if arg not passed)
  virtual void  SetNUnitGroups(int n_groups);
  // #CAT_Structure set number of unit groups in layer in the gp_geom member -- attempts to lay out geometry in closest to a square that fits all the groups evenly, if possible.  note: does NOT rebuild the network.  also does not make any changes if current number of groups is same as arg, or arg <= 0 (e.g., for startup arg, just init n_groups to -1 prior to getting arg val so it won't have any effect if arg not passed)

  virtual void  CheckSpecs();
  // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig
  virtual void  UpdatePrjnIdxs();
  // #IGNORE update the recv_idx, send_idx guys for active projections, and count number of prjns of each type
  virtual void  UpdateGeometry();
  // #CAT_Structure recompute the layer's full set of size values based on un_geom, gp_geom, gp_space settings
  virtual bool  UpdatePosition();
  // #CAT_Structure recompute the layer's positioning in 3D space (based on relative positioning) -- returns true if any changes
  virtual void  ConnectFrom(Layer* lay);
  // #DYN12N #CAT_Structure connect from one or more other layers to this layer (receive from other layer(s)) -- in network view, receiver is FIRST layer selected -- makes a new projection between layers
  virtual void  ConnectBidir(Layer* lay);
  // #DYN12N #CAT_Structure bidirectionally connect with one or more other layers to (receive from and send to other layer(s)) -- makes new projections between layers
  virtual void  ConnectSelf();
  // #MENU #CONFIRM #CAT_Structure #MENU_CONTEXT create a new self-connection within this layer (a projection to/from this layer)
  virtual void  SyncSendPrjns();
  // #EXPERT #CAT_Structure synchronize sending projections with the recv projections so everyone's happy
  virtual void  UpdateSendPrjnNames();
  // #EXPERT #CAT_Structure update sending prjn names to reflect any name change that might have occured with this layer
  virtual void  UpdateAllPrjns();
  // #EXPERT #CAT_Structure update all the sending and recv prjns for this layer -- e.g., after lesions
  virtual void  DisConnect();
  // #MENU #CONFIRM #CAT_Structure disconnect layer from all others
    virtual void  DisConnect_impl();
    // #IGNORE impl with no gui updates

  virtual void  SyncLayerState();
  // #CAT_State synchronize layer main state with LayerState computational state object -- each variable is either on one side or the other, and sync copies in proper direction
  
  virtual void  SetLayUnitExtFlags(int flg);
  // #CAT_Activation set external input data flags for layer and all units in the layer

  virtual void  ApplyInputData
    (taMatrix* data, ExtFlags ext_flags = NO_EXTERNAL,
     Random* ran = NULL, const PosVector2i* offset = NULL, bool na_by_range=false);
  // #CAT_Activation apply the 2d or 4d external input pattern to the network, optional random additional values, and offsetting; uses a flat 2-d model where grouped layer or 4-d data are flattened to 2d; frame<0 means from end; na_by_range means that values are not applicable if they fall outside act_range on unit spec, and thus don't have flags or values set
  virtual void  TriggerContextUpdate() {} // for algorithms/specs that suport context layers (copy of previous state) this manually triggers an update

  virtual void  Init_Weights(bool recv_cons);
  // #CAT_Learning #MENU #MENU_ON_State #MENU_SEP_BEFORE initialize weights for all of the projections into (recv_cons = true) or out of (recv_cons = false) this layer 
  virtual void  Copy_Weights(Layer* src, bool recv_cons);
  // #CAT_State copy weights from other layer, going projection-by-projection in order by index (only sensible if the layers have matching projection structure) -- either recv or send

  
  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  virtual void  PropagateInputDistance();
  // #EXPERT #CAT_Structure propagate my input distance (dist.fm_input) to layers I send to
  virtual void  PropagateOutputDistance();
  // #EXPERT #CAT_Structure propagate my output distance (dist.fm_output) to layers I receive from
  virtual void  Compute_PrjnDirections();
  // #CAT_Structure compute the directions of projections based on the relative distances from input/output layers

  virtual bool  SetUnitNames(bool force_use_unit_names = false);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Structure update unit names geometry -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
  virtual bool  SetUnitNamesFromDataCol(const DataCol* unit_names_col, int max_unit_chars=-1);
  // #MENU #CAT_Structure set unit names from unit names table column (string matrix with one row) -- max_unit_chars is max length of name to apply to unit (-1 = all)
  virtual void  GetLocalistName();
  // #EXPERT #CAT_Structure look for a receiving projection from a single unit, which has a name: if found, set our unit name to that name (also sets unit_names)
  virtual int   FindUnitNamedIdx(const String& nm, bool err = true) const;
  // #CAT_Structure look in unit_names for given name -- returns flat index of the corresponding unit in the layer if err = true, issues an error if not found
  virtual UnitState_cpp* FindUnitNamed(const String& nm, bool err = true) const;
  // #CAT_Structure look in unit_names for given name -- returns flat index of the corresponding unit in the layer if err = true, issues an error if not found

  virtual String GetUnitNameIdx(int un_idx) const;
  // #CAT_Structure get name for given unit index within layer
  virtual String GetUnitName(UnitState_cpp* un) const;
  // #CAT_Structure get name for given unit
  virtual void   SetUnitNameIdx(int un_idx, const String& nm);
  // #CAT_Structure set name for given unit index within layer -- turns on unit name saving if not otherwise engaged yet
  virtual void   SetUnitName(UnitState_cpp* un, const String& nm);
  // #CAT_Structure set name for given unit

  virtual void  TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_ON_State #CAT_Learning apply given transformation to weights -- must call Init_Weights_post at network level after running this!
  virtual void  AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification -- must call Init_Weights_post at network level after running this!
  virtual int   PruneCons(const SimpleMathSpec& pre_proc, Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int   ProbAddCons(float p_add_con, float init_wt = 0.0);
  // #MENU #MENU_ON_Structure #USE_RVAL #CAT_Structure probabilistically add new connections (assuming prior pruning), init_wt = initial weight value of new connection
  virtual int   LesionCons(float p_lesion, bool permute=true);
  // #MENU #MENU_ON_Structure #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)
  virtual int   LesionUnits(float p_lesion, bool permute=true);
  // #MENU #MENU_ON_Structure #USE_RVAL #CAT_Structure turn on unit LESIONED flags with prob p_lesion (permute = fixed no. lesioned)
  virtual void  UnLesionUnits();
  // #MENU #USE_RVAL #CAT_Structure un-lesion units: turn off all unit LESIONED flags
  virtual void  UpdtAfterNetModIfNecc();
  // #IGNORE call network UpdtAfterNetMod only if it is not otherwise being called at a higher level

  virtual void  Iconify();
  // #MENU #MENU_ON_State #DYN1 #CAT_Display iconify this layer in the network display (shrink to size of 1 unit, and make them invisible if lesioned)
  virtual void  DeIconify();
  // #MENU #MENU_ON_State #DYN1 #CAT_Display de-iconify this layer in the network display (make full size)
  inline void   SetDispScale(float disp_sc)     { disp_scale = disp_sc; UpdateAfterEdit(); }
  // #MENU #MENU_ON_State #DYN1 #CAT_Display set the display scale for the layer -- can change how much space it takes up relative to other layers

  virtual void  SetLayerUnitGeom(int x, int y, bool n_not_xy = false, int n = 0);
  // set layer unit geometry (convenience function for programs)
  virtual void  SetLayerUnitGpGeom(int x, int y, bool n_not_xy = false, int n = 0);
  // set layer unit group geometry (convenience function for programs)

  bool ApplySpecToMe(BaseSpec* spec) override;
  
  virtual bool  UnitSpecUpdated();
  // #CAT_Structure update unit specs for all units in the layer to use unit_spec (only if changed from last update -- force = do regardless); returns true if changed and all units can use given spec

  virtual bool  SetLayerSpec(LayerSpec* layspec);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_spec.spec set the layer specification
  virtual LayerSpec* GetMainLayerSpec() const { return (LayerSpec*)NULL; }
  // #CAT_Structure get the layer spec for this layer (if used)
  virtual bool  SetUnitSpec(UnitSpec* unitspec);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_unit_spec.spec set unit spec for all units in layer
  inline UnitSpec* GetMainUnitSpec() const { return unit_spec.SPtr(); }
  // #CAT_Structure get the unit spec for this unit -- this is controlled entirely by the layer and all units in the layer have the same unit spec

  inline Unit* GetUnitIdx(int un_idx) const
  { NetworkState_cpp* net = GetValidNetState(); if(net) return (Unit*)GetUnitStateSafe(net, un_idx); return NULL; }
  // #CAT_Access get unit state at given unit index (0..n_units) -- preferred Program interface as no NetworkState arg is required
  inline Unit* GetUnitFlatXY(int flat_x, int flat_y) const
  { NetworkState_cpp* net = GetValidNetState(); if(net) return (Unit*)GetUnitStateFlatXY(net, flat_x, flat_y); return NULL; }
  // #CAT_Access get unit state at given flat X,Y coordinates -- preferred Program interface as no NetworkState arg is required
  inline Unit* GetUnitGpUnIdx(int gp_idx, int un_idx) const
  { NetworkState_cpp* net = GetValidNetState(); if(net) return (Unit*)GetUnitStateGpUnIdx(net, gp_idx, un_idx); return NULL; }
  // #CAT_Access get unit state at given group and unit indexes -- preferred Program interface as no NetworkState arg is required
  inline Unit* GetUnitGpXYUnIdx(int gp_x, int gp_y, int un_idx) const
  { NetworkState_cpp* net = GetValidNetState(); if(net) return (Unit*)GetUnitStateGpXYUnIdx(net, gp_x, gp_y, un_idx); return NULL; }
  // #CAT_Access get the unit state at given group X,Y coordinate and unit indexes -- preferred Program interface as no NetworkState arg is required
  inline Unit* GetUnitGpIdxUnXY(int gp_idx, int un_x, int un_y) const
  { NetworkState_cpp* net = GetValidNetState(); if(net) return (Unit*)GetUnitStateGpIdxUnXY(net, gp_idx, un_x, un_y); return NULL; }
  // #CAT_Access get the unit state at given group index and unit X,Y coordinate -- preferred Program interface as no NetworkState arg is required
  inline Unit* GetUnitGpUnXY(int gp_x, int gp_y, int un_x, int un_y) const
  { NetworkState_cpp* net = GetValidNetState(); if(net) return (Unit*)GetUnitStateGpUnXY(net, gp_x, gp_y, un_x, un_y); return NULL;  }
  // #CAT_Access get the unit state at given group X,Y and unit X,Y coordinates -- preferred Program interface as no NetworkState arg is required

  virtual bool EditState();
  // #MENU_BUTTON #MENU_ON_Edit edit the layer state values that drive actual C++ computation
  virtual bool EditLayUnGpState();
  // #MENU_BUTTON #MENU_ON_Edit edit the layer unit group state values that drive actual C++ computation
  virtual bool EditUnGpState(int un_gp_no);
  // #MENU_BUTTON #MENU_ON_Edit edit the unit group state values for given unit group
  virtual bool EditUnitState(int unit_no);
  // #MENU_BUTTON #MENU_ON_Edit edit the unit state values for given unit number (0 .. n_units-1)
  virtual bool EditConState(int unit_no, int prjn_idx, bool recv=true);
  // #MENU_BUTTON #MENU_ON_Edit edit the connection state object containing connections for given unit and projection number, for either recv or sending projections
  
  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #DYN1 #CAT_Statistic monitor (record in a datatable) the given variable on this layer (can be a variable on the units or connections -- in which case a matrix with a value for each will be created -- e.g., 'act' will monitor activations of all units within the layer)
  virtual bool  Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #BUTTON #CAT_Statistic take a snapshot of given variable: assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.
  Unit*         MostActiveUnit(int& idx) const;
  // #CAT_Statistic Return the unit with the highest activation (act) value -- index of unit is returned in idx

  virtual void  Lesion();
  // #BUTTON #MENU #MENU_ON_Structure #DYN1 #MENU_SEP_BEFORE #CAT_Structure set the lesion flag on layer -- removes it from all processing operations
  virtual void  LesionIconify();
  // #MENU #MENU_ON_Structure #DYN1 #CAT_Structure set the lesion flag on layer -- removes it from all processing operations -- also iconifies the layer
  virtual void  UnLesion();
  // #BUTTON #MENU #DYN1 #CAT_Structure un-set the lesion flag on layer -- restores it to engage in normal processing
  virtual void  UnLesionDeIconify();
  // #MENU #DYN1 #CAT_Structure un-set the lesion flag on layer -- restores it to engage in normal processing -- also de-iconifies the layer

  virtual int   ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // #CAT_Structure switch any units/layers using old_sp to using new_sp
  virtual int   ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int   ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp
  virtual int   ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp);
  // #CAT_Structure switch any layers using old_sp to using new_sp

  virtual DataTable*    WeightsToTable(DataTable* dt, Layer* send_lay);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistic send entire set of weights from sending layer to given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit
  virtual DataTable*    VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistic send given variable to data table -- number of columns depends on variable (for projection variables, specify prjns.; for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual DataTable*    ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
               const String& var3 = "", const String& var4 = "", const String& var5 = "",
               const String& var6 = "", const String& var7 = "", const String& var8 = "",
               const String& var9 = "", const String& var10 = "", const String& var11 = "",
               const String& var12 = "", const String& var13 = "", const String& var14 = "",
               PrjnState_cpp* prjn=NULL);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistic record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var" -- prjn restricts to that prjn
  virtual DataTable*    PrjnsToTable(DataTable* dt = NULL, bool sending = false);
  // #MENU #MENU_ON_Structure #MENU_SEP_BEFORE #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistic record the layer projections (receiving unless sending clicked) to given data table, with one row per projection, including the connection and projection specs used

  virtual bool  VarToVarCopy(const String& dest_var, const String& src_var);
  // #CAT_Structure copy one unit variable to another (un->dest_var = un->src_var) for all units within this layer (must be a float type variable)
  virtual bool  VarToVal(const String& dest_var, float val);
  // #CAT_Structure set variable to given value for all units within this layer (must be a float type variable)

  void  SaveWeights(const String& fname="");
  // #BUTTON #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  bool  LoadWeights(const String& fname="", bool quiet = false);
  // #BUTTON #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (fmt is read from file) (leave fname empty to pull up file chooser)

  
#ifdef DMEM_COMPILE
  DMemAggVars   dmem_agg_sum;           // #IGNORE aggregation of layer variables using SUM op (currently only OP in use -- add others as needed)
  virtual void  DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void  DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate layer variables across procs for trial-level dmem
#endif

  String        GetDesc() const       override { return desc; }
  int           GetEnabled() const    override { return !lesioned(); }
  void          SetEnabled(bool value) override { SetLayerFlagState(LESIONED, !value); }
  String        GetTypeDecoKey() const override { return "Layer"; }
  int           GetSpecialState() const override;
  String        GetToolbarName() const override { return "layer"; }

  bool ChangeMyType(TypeDef* new_type) override;

  DumpQueryResult Dump_QuerySaveMember(MemberDef* md) override;

  void  InitLinks() override;
  void  CutLinks() override;
  void  Copy_(const Layer& cp);
  TA_BASEFUNS(Layer); //

protected:
  UnitSpec*     m_prv_unit_spec; // previous unit spec set for units in layer
  LayerFlags    m_prv_layer_flags; // previous layer flags in layer

  void         UpdateAfterEdit_impl() override;
  void         UpdateAfterMove_impl(taBase* old_owner) override;
  virtual void ApplyLayerFlags(NETWORK_STATE* net, ExtFlags act_ext_flags);
  // #IGNORE set layer flag to reflect the kind of input received
  virtual void ApplyInputData_1d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                 Random* ran, bool na_by_range=false);
  // #IGNORE 1d data -- just go in order -- offsets ignored
  virtual void ApplyInputData_2d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                 Random* ran, const taVector2i& offs, bool na_by_range=false);
  // #IGNORE 2d data is always treated the same: UnitAtCoord deals with unit grouping
  virtual void ApplyInputData_Flat4d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                     Random* ran, const taVector2i& offs, bool na_by_range=false);
  // #IGNORE flat layer, 4d data
  virtual void ApplyInputData_Gp4d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                   Random* ran, bool na_by_range=false);
  // #IGNORE grouped layer, 4d data -- note this cannot have offsets..

  void         CheckThisConfig_impl(bool quiet, bool& rval) override;
  // #IGNORE this is the guy that *additionally* delegates to the Spec
  void         CheckChildConfig_impl(bool quiet, bool& rval) override;// #IGNORE
private:
  void  Initialize();
  void  Destroy()       { CutLinks(); }
};


#endif // Layer_h
