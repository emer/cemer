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

#ifndef Layer_h
#define Layer_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <LayerRef>
#include <PosVector3i>
#include <PosVector2i>
#include <XYNGeom>
#include <Projection_Group>
#include <Unit_Group>
#include <PRerrVals>
#include <String_Matrix>
#include <LayerSpec>
#include <DMemShare>

// declare all other types mentioned but not required to include:
class Network; //
class ProjectBase; //
class DataCol; //
class DMemShare; //

// use this macro for iterating over either unit groups one-by-one, or the 
// global layer, and applying 'code' to either
// code uses acc_md and gpidx plus the lay->UnitAccess(acc_md, idx, gpidx) function
// to access units -- e.g., calling a _ugp function as such:
//
// UNIT_GP_ITR(lay, MySpecialFun_ugp(lay, acc_md, gpidx););

#define UNIT_GP_ITR(lay, code) \
  if(lay->unit_groups) { \
    for(int gpidx=0; gpidx < lay->gp_geom.n; gpidx++) { \
      Layer::AccessMode acc_md = Layer::ACC_GP; \
      code \
    } \
  } \
  else { \
    Layer::AccessMode acc_md = Layer::ACC_LAY;  int gpidx = 0; \
    code \
  } 


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


eTypeDef_Of(Layer);

class E_API Layer : public taNBase {
  // ##EXT_lay ##COMPRESS ##CAT_Network ##SCOPE_Network ##HAS_CONDTREE layer containing units
INHERITED(taNBase)
public:
  enum DMemDist {
    DMEM_DIST_DEFAULT,          // distribute units to different processors for distributed memory processing according to the default sequential scheme
    DMEM_DIST_UNITGP            // distribute units according to unit groups, which can be less even but allows for shared weights by unit group
  }; //

  enum LayerType {      // type of layer, used to determine various default settings
    HIDDEN,             // layer does not receive external input of any form
    INPUT,              // layer receives external input (EXT) that drives activation states directly
    TARGET,             // layer receives a target input (TARG) that determines correct activation states, used for training
    OUTPUT,             // layer produces a visible output response but is not a target.  any external input serves as a comparison (COMP) against current activations.
  };

  enum LayerFlags {                     // #BITS flags for layer
    LF_NONE             = 0,            // #NO_BIT
    LESIONED            = 0x0001,       // #READ_ONLY this layer is temporarily lesioned (inactivated for all network-level processing functions) -- IMPORTANT: use the Lesion and UnLesion functions to set this flag -- they provide proper updating after changes -- otherwise network dynamics will be wrong and the display will not be properly updated
    ICONIFIED           = 0x0002,       // only display a single unit showing icon_value (set in algorithm-specific manner)
    NO_ADD_SSE          = 0x0004,       // do NOT add this layer's sse value (sum squared error) to the overall network sse value: this is for all types of SSE computed for ext_flag = TARG (layer_type = TARGET) or ext_flag = COMP (layer_type = OUTPUT) layers
    NO_ADD_COMP_SSE     = 0x0008,       // do NOT add this layer's sse value (sum squared error) to the overall network sse value: ONLY for ext_flag = COMP (OUTPUT) flag settings (NO_ADD_SSE blocks all contributions) -- this is relevant if the layer type or ext_flags are switched dynamically and only TARGET errors are relevant
    PROJECT_WTS_NEXT    = 0x0010,       // #NO_SHOW this layer is next in line for weight projection operation
    PROJECT_WTS_DONE    = 0x0020,       // #NO_SHOW this layer is done with weight projection operation (prevents loops)
    SAVE_UNITS          = 0x0040,       // save this layer's units in the project file (even if Network::SAVE_UNITS off)
    NO_SAVE_UNITS       = 0x0080,       // don't save this layer's units in the project file (even if Network::SAVE_UNITS on)
  };

  enum AccessMode {     // how to access the units in the layer -- only relevant for layers with unit groups (otherwise modes are the same)
    ACC_LAY,            // access as a single layer-wide set of units
    ACC_GP,             // access via their subgroup structure, with group and unit index values
  };


  String                desc;           // #EDIT_DIALOG Description of this layer -- what functional role it plays, how it maps onto the brain, etc
  Network*              own_net;        // #READ_ONLY #NO_SAVE #NO_SHOW #CAT_Structure #NO_SET_POINTER Network this layer is in
  LayerFlags            flags;          // flags controlling various aspects of layer funcdtion
  LayerType             layer_type;     // #CAT_Activation type of layer: determines default way that external inputs are presented, and helps with other automatic functions (e.g., wizards)
  PosVector3i           pos;            // #CAT_Structure position of layer relative to the overall network position (0,0,0 is lower left hand corner)
  PosVector2i       	pos2d;		// #CAT_Structure 2D network view display position of layer relative to the overall nework (0,0 is lower left hand corner)
  float                 disp_scale;     // #DEF_1 #CAT_Structure display scale factor for layer -- multiplies overall layer size -- 1 is normal, < 1 is smaller and > 1 is larger -- can be especially useful for shrinking very large layers to better fit with other smaller layers
  XYNGeom               un_geom;        // #AKA_geom #CAT_Structure two-dimensional layout and number of units within the layer or each unit group within the layer
  bool                  unit_groups;    // #CAT_Structure organize units into subgroups within the layer, with each unit group having the geometry specified by un_geom -- see virt_groups for whether there are actual unit groups allocated, or just virtual organization a flat list of groups
  bool                  virt_groups;    // #CONDSHOW_ON_unit_groups #CAT_Structure #DEF_true if true, do not allocate actual unit groups -- just organize a flat list of units into groups for display and computation purposes -- this is much more efficient and is the default behavior, but some rare cases require explicit unit groups still (e.g. unique positions for unit groups)
  XYNGeom               gp_geom;        // #CONDSHOW_ON_unit_groups #CAT_Structure geometry of unit sub-groups (if unit_groups) -- this is the layout of the groups, with gp_geom defining the layout of units within the groups
  PosVector2i           gp_spc;         // #CONDSHOW_ON_unit_groups #CAT_Structure spacing between unit sub-groups (if unit_groups) -- this is *strictly* for display purposes, and does not affect anything else in terms of projection connectivity calculations etc.
  XYNGeom               flat_geom;      // #EXPERT #READ_ONLY #CAT_Structure geometry of the units flattening out over unit groups -- same as un_geom if !unit_groups; otherwise un_geom * gp_geom -- this is in logical (not display) sizes
  XYNGeom               disp_geom;      // #AKA_act_geom #HIDDEN #READ_ONLY #CAT_Structure actual view geometry, includes spaces and groups and everything: the full extent of units within the layer
  XYNGeom               scaled_disp_geom; // #AKA_scaled_act_geom #HIDDEN #READ_ONLY #CAT_Structure scaled actual view geometry: disp_scale * disp_geom -- use for view computations

  Projection_Group      projections;    // #CAT_Structure group of receiving projections
  Projection_Group      send_prjns;     // #CAT_Structure #HIDDEN #LINK_GROUP group of sending projections
  Unit_Group            units;          // #CAT_Structure #NO_SEARCH units or groups of units
  UnitSpec_SPtr         unit_spec;      // #CAT_Structure default unit specification for units in this layer
  Unit::ExtType         ext_flag;       // #NO_SAVE #CAT_Activation #GUI_READ_ONLY #SHOW indicates which kind of external input layer received -- this is normally set by the ApplyInputData function -- it is not to be manipulated directly
  DMemDist              dmem_dist;      // #CAT_DMem how to distribute units across multiple distributed memory processors

  LayerDistances        dist;           // #CAT_Structure distances from closest input/output layers to this layer

  String                output_name;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)
  String_Matrix         gp_output_names; // #NO_SAVE #SHOW_TREE #CAT_Statistic #CONDTREE_ON_unit_groups output_name's for unit subgroups -- name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)
  float                 sse;            // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW sum squared error over the network, for the current external input pattern
  PRerrVals             prerr;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic precision and recall error values for this layer, for the current pattern
  float                 icon_value;     // #NO_SAVE #GUI_READ_ONLY #HIDDEN #CAT_Statistic value to display if layer is iconified (algorithmically determined)
  int                   units_flat_idx; // #NO_SAVE #READ_ONLY starting index for this layer into the network units_flat list, used in threading
  bool                  units_lesioned; // #GUI_READ_ONLY if units were lesioned in this group, don't complain about rebuilding!
  bool                  gp_unit_names_4d; // #CONDSHOW_ON_unit_groups if there are unit subgroups, create a 4 dimensional set of unit names which allows for distinct names for each unit in the layer -- otherwise a 2d set of names is created of size un_geom, all unit groups have the same repeated set of names
  String_Matrix         unit_names;     // #SHOW_TREE set unit names from corresponding items in this matrix (dims=2 for no group layer or to just label main group, dims=4 for grouped layers, dims=0 to disable)

  String                brain_area;     // #CAT_Structure #REGEXP_DIALOG #TYPE_BrainAtlasRegexpPopulator Which brain area this layer's units should be mapped to in a brain view.  Must match a label from the atlas chosen for the network.  Layer will not render to brain view if LESIONED flag is checked.
  float                 voxel_fill_pct; // #CAT_Structure #MIN_0 #MAX_1 Percent of brain_area voxels to be filled by units in this layer.

  int                   n_units;
  // #HIDDEN #READ_ONLY #NO_SAVE obsolete v3 specification of number of units in layer -- do not use!!
  bool                  lesion_;
  // #AKA_lesion #HIDDEN #READ_ONLY #NO_SAVE obsolete v3 flag to inactivate this layer from processing (reversable)

  ProjectBase*          project(); // #IGNORE this layer's project

  inline void           SetLayerFlag(LayerFlags flg)   { flags = (LayerFlags)(flags | flg); }
  // set flag state on
  inline void           ClearLayerFlag(LayerFlags flg) { flags = (LayerFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasLayerFlag(LayerFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetLayerFlagState(LayerFlags flg, bool on)
  { if(on) SetLayerFlag(flg); else ClearLayerFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  inline bool           lesioned() const { return HasLayerFlag(LESIONED); }
  // check if this layer is lesioned -- use in function calls

  inline UnitSpec* GetUnitSpec() const { return unit_spec.SPtr(); }
  // #CAT_Structure get the unit spec for this unit -- this is controlled entirely by the layer and all units in the layer have the same unit spec

  ////////////////////////////////////////////////////////////////////////////////
  // Unit access API -- all access of units should occur strictly through this API
  // and NOT via unit groups (layers can handle unit groups virtually or with real
  // allocated unit groups -- see virt_groups flag)

  Unit*         UnitAtCoord(const taVector2i& coord) const
  { return UnitAtCoord(coord.x, coord.y); }
  // #CAT_Access get unit at given logical coordinates, taking into account group geometry if present -- this uses *logical* flat 2d coordinates, which exclude any consideration of gp_spc between units (i.e., as if there were no space -- space is only for display)
  Unit*         UnitAtCoord(int x, int y) const;
  // #CAT_Access get unit at given logical coordinates, taking into account group geometry if present -- this uses *logical* flat 2d coordinates, which exclude any consideration of gp_spc between units (i.e., as if there were no space -- space is only for display)

  inline Unit*  UnitAtUnGpIdx(int unidx, int gpidx) const {
    if(unit_groups && !virt_groups) {
      Unit_Group* ug = (Unit_Group*)units.gp.SafeEl(gpidx); if(!ug) return NULL;
      return ug->SafeEl(unidx);
    }
    return units.SafeEl(gpidx * un_geom.n + unidx);
  }
  // #CAT_Access get unit from group and unit indexes -- only valid group index is 0 if no unit groups -- useful for efficient access to units in computational routines
  void          UnGpIdxFmUnitIdx(int uflat_idx, int& unidx, int& gpidx) const
  { gpidx = uflat_idx / un_geom.n;  unidx = uflat_idx % un_geom.n; }
  // #CAT_Access get unit and group index values from a flat unit index

  inline Unit*  UnitAccess(AccessMode mode, int unidx, int gpidx) const {
    if(mode == ACC_GP && unit_groups) {
      if(virt_groups) return units.SafeEl(gpidx * un_geom.n + unidx);
      Unit_Group* ug = (Unit_Group*)units.gp.SafeEl(gpidx); if(!ug) return NULL;
      return ug->SafeEl(unidx);
    }
    return units.Leaf(unidx);
  }
  // #CAT_Access abstracted access of units in layer depending on mode -- if layer-wide mode, unidx is index into full set of units (flat_geom.n items), else if in group mode, get from unit from group and unit indexes
  inline int    UnitAccess_NUnits(AccessMode mode) const {
    if(mode == ACC_GP && unit_groups) return un_geom.n;
    return flat_geom.n;
  }
  // #CAT_Access abstracted access of units in layer depending on mode -- number of units associated with this access mode
  inline int    UnitAccess_NGps(AccessMode mode) const {
    if(mode == ACC_GP && unit_groups) return gp_geom.n;
    return 1;
  }
  // #CAT_Access abstracted access of units in layer depending on mode -- number of groups associated with this access mode

  Unit*         UnitAtGpCoord(const taVector2i& gp_coord, const taVector2i& coord) const
  { return UnitAtGpCoord(gp_coord.x,gp_coord.y, coord.x, coord.y); }
  // #CAT_Access get unit given both unit and group coordinates -- only functional if unit_groups is on -- this uses logical 4d coordinates, relative to gp_geom and un_geom
  Unit*         UnitAtGpCoord(int gp_x, int gp_y, int un_x, int un_y) const;
  // #CAT_Access get unit given both unit and group coordinates -- only functional if unit_groups is on -- this uses logical 4d coordinates, relative to gp_geom and un_geom


  Unit_Group*   UnitGpAtCoord(const taVector2i& coord) const
  { return UnitGpAtCoord(coord.x,coord.y); }
  // #EXPERT #CAT_Access get unit group at logical group coordinates (i.e., within gp_geom) -- note that if virt_groups is on, then there are no unit subgroups -- better to use UnitAtGpCoord to access units directly at the unit level
  Unit_Group*   UnitGpAtCoord(int gp_x, int gp_y) const;
  // #EXPERT #CAT_Access get unit group at logical group coordinates (i.e., within gp_geom) -- note that if virt_groups is on, then there are no unit subgroups -- better to use UnitAtGpCoord to access units directly at the unit level

  void          UnitLogPos(Unit* un, taVector2i& upos) const
  { UnitLogPos(un, upos.x, upos.y); }
  // #CAT_Access get *logical* position for unit, relative to flat_geom (no display spacing) -- based on index within group/layer
  void          UnitLogPos(Unit* un, int& x, int& y) const;
  // #CAT_Access get *logical* position for unit, relative to flat_geom (no display spacing) -- based on index within group/layer
  int           UnitIdxFmPos(taVector2i& pos) const
  { return pos.y * un_geom.x + pos.x; }
  // #CAT_Access get unit index from position for unit within a subgroup or unit in a layer without any subgroups
  bool          UnitIdxIsValid(int unidx) const
  { return unidx >= 0 && unidx < un_geom.n; }
  // #CAT_Access is the unit index valid (within range) for unit in subgroup or unit in layer without subgroups

  taVector2i     UnitGpPosFmIdx(int gpidx) const
  { taVector2i rval; rval.x = gpidx % gp_geom.x; rval.y = gpidx / gp_geom.x; return rval; }
  // #CAT_Access #IGNORE get unit group *logical* position from index
  int           UnitGpIdxFmPos(taVector2i& pos) const
  { return pos.y * gp_geom.x + pos.x; }
  // #CAT_Access get unit group index from position
  bool          UnitGpIdxIsValid(int gpidx) const
  { return gpidx >= 0 && gpidx < gp_geom.n; }
  // #CAT_Access is the group index valid (within range)

  int           UnitGpIdx(Unit* u) const;
  // #CAT_Access get unit's subgroup index (unit must live in this layer, returns -1 if layer does not have unit groups), directly from info avail on unit itself

  ////////////  display coordinate versions

  Unit*         UnitAtDispCoord(int x, int y) const;
  // #CAT_Access get unit at given *display* coordinates relative to layer -- this takes into account spaces between groups etc
  void          UnitDispPos(Unit* un, taVector2i& upos) const
  { UnitDispPos(un, upos.x, upos.y); }
  // #CAT_Access get display position for unit, taking into account spacing, unit group positioning etc
  void          UnitDispPos(Unit* un, int& x, int& y) const;
  // #CAT_Access get display position for unit, taking into account spacing, unit group positioning etc


  ////////////////////////////////////////////////////////////////////////////////
  // obsolete versions -- do not use in new code

  Unit*         FindUnitFmCoord(const taVector2i& coord)
  { return UnitAtCoord(coord.x, coord.y); }
  // #CAT_zzzObsolete get unit at given coordinates, taking into account group geometry if present -- this uses *logical* flat 2d coordinates, which exclude any consideration of gp_spc between units (i.e., as if there were no space -- space is only for display)
  Unit*         FindUnitFmCoord(int x, int y)
  { return UnitAtCoord(x, y); }
  // #CAT_zzzObsolete get unit at given coordinates, taking into account group geometry if present -- this uses *logical* flat 2d coordinates, which exclude any consideration of gp_spc between units (i.e., as if there were no space -- space is only for display)
  Unit*         FindUnitFmGpCoord(const taVector2i& gp_coord, const taVector2i& coord)
  { return UnitAtGpCoord(gp_coord.x, gp_coord.y, coord.x, coord.y); }
  // #CAT_zzzObsolete get unit given both unit and group coordinates -- only functional if unit_groups is on -- this uses logical 4d coordinates, relative to gp_geom and un_geom
  Unit*         FindUnitFmGpCoord(int gp_x, int gp_y, int un_x, int un_y)
  { return UnitAtGpCoord(gp_x, gp_y, un_x, un_y); }
  // #CAT_zzzObsolete get unit given both unit and group coordinates -- only functional if unit_groups is on -- this uses logical 4d coordinates, relative to gp_geom and un_geom
  Unit_Group*   FindUnitGpFmCoord(const taVector2i& coord)
  { return UnitGpAtCoord(coord.x,coord.y); }
  // #CAT_zzzObsolete get unit group at logical group coordinates (i.e., within gp_geom) -- note that if virt_groups is on, then there are no unit subgroups -- better to use UnitAtGpCoord to access units directly at the unit level
  Unit_Group*   FindUnitGpFmCoord(int gp_x, int gp_y)
  { return UnitGpAtCoord(gp_x, gp_y); }
  // #CAT_zzzObsolete get unit group at logical group coordinates (i.e., within gp_geom) -- note that if virt_groups is on, then there are no unit subgroups -- better to use UnitAtGpCoord to access units directly at the unit level

  ////////////  layer display position computation

  bool          InLayerSubGroup();
  // #CAT_Structure is this layer in a layer subgroup or directly in network.layers main layer group?
  void          GetAbsPos(taVector3i& abs_pos) { abs_pos = pos; AddRelPos(abs_pos); }
  // #CAT_Structure get absolute pos, which factors in offsets from layer groups
  void          AddRelPos(taVector3i& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above
  void          GetAbsPos2d(taVector2i& abs_pos) { abs_pos = pos2d; AddRelPos2d(abs_pos); }
  // #CAT_Structure get absolute pos, which factors in offsets from layer groups
  void          AddRelPos2d(taVector2i& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above
  void          SetDefaultPos();
  // #IGNORE initialize position of layer
  void          SetDefaultPos2d();
  // #IGNORE initialize position of layer -- 2d

  virtual void  Copy_Weights(const Layer* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_ObjectMgmt copies weights from other layer (incl wts assoc with unit bias member)

  virtual void  SaveWeights_strm(std::ostream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   LoadWeights_strm(std::istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
                                 bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful
  static int    SkipWeights_strm(std::istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
                                 bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File skip over weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful

  virtual void  SaveWeights(const String& fname="", RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int   LoadWeights(const String& fname="",
                            RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  virtual void  SetNUnits(int n_units);
  // #CAT_Structure set number of units in layer in the un_geom member -- attempts to lay out geometry in closest to a square that fits all the units evenly, if possible.  note: does NOT rebuild the network.  also does not make any changes if current number of units is same as arg, or arg <= 0 (e.g., for startup arg, just init n_units to -1 prior to getting arg val so it won't have any effect if arg not passed)
  virtual void  SetNUnitGroups(int n_groups);
  // #CAT_Structure set number of unit groups in layer in the gp_geom member -- attempts to lay out geometry in closest to a square that fits all the groups evenly, if possible.  note: does NOT rebuild the network.  also does not make any changes if current number of groups is same as arg, or arg <= 0 (e.g., for startup arg, just init n_groups to -1 prior to getting arg val so it won't have any effect if arg not passed)

  virtual void  CheckSpecs();
  // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig
  virtual void  BuildUnits();
  // #MENU #MENU_ON_Structure #CONFIRM #CAT_Structure build the units based current geometry configuration
  virtual void  BuildUnits_Threads(Network* net);
  // #IGNORE build unit-level thread information: flat list of units, etc -- this is called by network BuildUnits_Threads so that layers (and layerspecs) can potentially modify which units get added to the compute lists, and thus which are subject to standard computations -- default is all units in the layer
  virtual void  RecomputeGeometry();
  // #CAT_Structure recompute the layer's geometry specifcations
  virtual void  LayoutUnits();
  // #CAT_Structure layout the units according to layer geometry
  virtual void  ConnectFrom(Layer* lay);
  // #DYN12N #CAT_Structure connect from one or more other layers to this layer (receive from other layer(s)) -- in network view, receiver is FIRST layer selected -- makes a new projection between layers
  virtual void  ConnectBidir(Layer* lay);
  // #DYN12N #CAT_Structure bidirectionally connect with one or more other layers to (receive from and send to other layer(s)) -- makes new projections between layers
  virtual void  ConnectSelf();
  // #MENU #CONFIRM #CAT_Structure #MENU_CONTEXT create a new self-connection within this layer (a projection to/from this layer)
  virtual void  Connect();
  // #MENU #CONFIRM #CAT_Structure connect the layer -- construct connections according to existing projections
  virtual bool  CheckBuild(bool quiet=false);
  // #CAT_Structure check if network is built
  virtual bool  CheckConnect(bool quiet=false);
  // #CAT_Structure check if network is connected
  virtual void  RemoveCons();
  // #MENU #CONFIRM #MENU_SEP_BEFORE #CAT_Structure remove all connections in this layer
  virtual void  RemoveCons_Net();
  // #CAT_Structure remove all connections in this layer, in context of entire network removecons -- calls unit removecons only
  virtual void  RemoveUnits();
  // #MENU #DYN1 #CAT_Structure remove all units in this layer (preserving groups)
  virtual void  RemoveUnitGroups();
  // #MENU #DYN1 #CAT_Structure remove all unit groups in this layer
  virtual void  PreConnect();
  // #EXPERT #CAT_Structure prepare to connect the layer (create con_groups)
  virtual void  SyncSendPrjns();
  // #EXPERT #CAT_Structure synchronize sending projections with the recv projections so everyone's happy
  virtual void  UpdateSendPrjnNames();
  // #EXPERT #CAT_Structure update sending prjn names to reflect any name change that might have occured with this layer
  virtual void  RecvConsPreAlloc(int alloc_no, Projection* prjn);
  // #EXPERT #CAT_Structure allocate given number of recv connections for all units in layer, for given projection
  virtual void  SendConsPreAlloc(int alloc_no, Projection* prjn);
  // #EXPERT #CAT_Structure allocate given number of send connections for all units in layer, for given projection
  virtual void  SendConsPostAlloc(Projection* prjn);
  // #EXPERT #CAT_Structure allocate sending connections based on those allocated previously
  virtual void  RecvConsPostAlloc(Projection* prjn);
  // #EXPERT #CAT_Structure allocate recv connections based on those allocated previously
  virtual void  DisConnect();
  // #MENU #CONFIRM #CAT_Structure disconnect layer from all others
  virtual int   CountRecvCons();
  // #CAT_Structure count recv connections for all units in layer

  void          SetExtFlag(int flg)   { ext_flag = (Unit::ExtType)(ext_flag | flg); }
  // #CAT_Activation set external input data flag
  void          UnSetExtFlag(int flg) { ext_flag = (Unit::ExtType)(ext_flag & ~flg); }
  // #CAT_Activation un-set external input data flag
  bool          HasExtFlag(int flg)   { return ext_flag & flg; }
  // #CAT_Activation check if has given ext flag value set

  virtual void  SetLayUnitExtFlags(int flg);
  // #CAT_Activation set external input data flags for layer and all units in the layer

  virtual void  ApplyInputData(taMatrix* data, Unit::ExtType ext_flags = Unit::NO_EXTERNAL,
      Random* ran = NULL, const PosVector2i* offset = NULL, bool na_by_range=false);
  // #CAT_Activation apply the 2d or 4d external input pattern to the network, optional random additional values, and offsetting; uses a flat 2-d model where grouped layer or 4-d data are flattened to 2d; frame<0 means from end; na_by_range means that values are not applicable if they fall outside act_range on unit spec, and thus don't have flags or values set
  virtual void  TriggerContextUpdate() {} // for algorithms/specs that suport context layers (copy of previous state) this manually triggers an update

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning
  //    Many functions operate directly on the units via threads, and then
  //    call through to the layers for any layer-level subsequent processing
  //    All functions take the pointer to the parent network, just for
  //    convenience and consistency with Unit levels etc

  virtual void  Init_InputData(Network* net);
  // #CAT_Activation Initializes external and target inputs
  virtual void  Init_Acts(Network* net);
  // #CAT_Activation Initialize the unit state variables
  virtual void  Init_dWt(Network* net);
  // #CAT_Learning Initialize the weight change variables
  virtual void  Init_Weights(Network* net);
  // #MENU #MENU_ON_State #CONFIRM #CAT_Learning Initialize the weights
  virtual void  Init_Weights_post(Network* net);
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  virtual float Compute_SSE(Network* net, int& n_vals, bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value -- uses sse_tol so error is 0 if within tolerance on a per unit basis
  virtual int   Compute_PRerr(Network* net);
  // #CAT_Statistic compute precision and recall error statistics over entire layer -- true positive, false positive, and false negative -- returns number of values entering into computation (depends on number of targets) -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance on a per unit basis -- results are stored in prerr values on layer

  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  virtual void  PropagateInputDistance();
  // #EXPERT #CAT_Structure propagate my input distance (dist.fm_input) to layers I send to
  virtual void  PropagateOutputDistance();
  // #EXPERT #CAT_Structure propagate my output distance (dist.fm_output) to layers I receive from
  virtual void  Compute_PrjnDirections();
  // #CAT_Structure compute the directions of projections based on the relative distances from input/output layers

  virtual bool  SetUnitNames(bool force_use_unit_names = false);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Structure set unit names from unit_names matrix (called automatically on Build) -- also ensures unit_names fits geometry of layer -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
  virtual bool  SetUnitNamesFromDataCol(const DataCol* unit_names_col, int max_unit_chars=-1);
  // #MENU #CAT_Structure set unit names from unit names table column (string matrix with one row) -- max_unit_chars is max length of name to apply to unit (-1 = all)
  virtual bool  GetUnitNames(bool force_use_unit_names = true);
  // #MENU #CAT_Structure get unit_names matrix values from current unit name values -- also ensures unit_names fits geometry of layer -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
  virtual void  GetLocalistName();
  // #EXPERT #CAT_Structure look for a receiving projection from a single unit, which has a name: if found, set our unit name to that name (also sets unit_names)
  virtual int   FindUnitNamedIdx(const String& nm, bool err = true);
  // #CAT_Structure look in unit_names for given name -- returns flat index of the corresponding unit in the layer if err = true, issues an error if not found
  virtual Unit* FindUnitNamed(const String& nm, bool err = true);
  // #CAT_Structure look in unit_names for given name -- returns unit at that index if found --  if err = true, issues an error if not found
  

  virtual void  TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void  AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int   PruneCons(const SimpleMathSpec& pre_proc,
                             Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int   ProbAddCons(float p_add_con, float init_wt = 0.0);
  // #MENU #MENU_ON_Structure #USE_RVAL #CAT_Structure probabilistically add new connections (assuming prior pruning), init_wt = initial weight value of new connection
  virtual int   LesionCons(float p_lesion, bool permute=true);
  // #MENU #MENU_ON_Structure #USE_RVAL #CAT_Structure remove connectiosn with prob p_lesion (permute = fixed no. lesioned)
  virtual int   LesionUnits(float p_lesion, bool permute=true);
  // #MENU #MENU_ON_Structure #USE_RVAL #CAT_Structure turn on unit LESIONED flags with prob p_lesion (permute = fixed no. lesioned)
  virtual void  UnLesionUnits();
  // #MENU #USE_RVAL #CAT_Structure un-lesion units: turn off all unit LESIONED flags
  virtual void  UpdtAfterNetModIfNecc();
  // #IGNORE call network UpdtAfterNetMod only if it is not otherwise being called at a higher level

  virtual void  Iconify();
  // #MENU #MENU_ON_State #DYN1 #CAT_Display iconify this layer in the network display (shrink to size of 1 unit)
  virtual void  DeIconify();
  // #MENU #MENU_ON_State #DYN1 #CAT_Display de-iconify this layer in the network display (make full size)
  inline void   SetDispScale(float disp_sc)     { disp_scale = disp_sc; UpdateAfterEdit(); }
  // #MENU #MENU_ON_State #DYN1 #CAT_Display set the display scale for the layer -- can change how much space it takes up relative to other layers

  virtual bool  Iconified() const       { return HasLayerFlag(ICONIFIED); }
  // convenience function for checking iconified flag

  virtual void  SetLayerUnitGeom(int x, int y, bool n_not_xy = false, int n = 0);
  // set layer unit geometry (convenience function for programs)
  virtual void  SetLayerUnitGpGeom(int x, int y, bool n_not_xy = false, int n = 0);
  // set layer unit group geometry (convenience function for programs)

  virtual bool  UpdateUnitSpecs(bool force = false);
  // #CAT_Structure update unit specs for all units in the layer to use unit_spec (only if changed from last update -- force = do regardless); returns true if changed and all units can use given spec
  virtual bool  UpdateConSpecs(bool force = false);
  // #CAT_Structure update connection specs for all projections in the layer (only if changed from last update -- force = do regardless)

  virtual bool  SetLayerSpec(LayerSpec* layspec);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_spec.spec set the layer specification
  virtual LayerSpec* GetLayerSpec()             { return (LayerSpec*)NULL; }
  // #CAT_Structure get the layer spec for this layer (if used)
  virtual bool  SetUnitSpec(UnitSpec* unitspec);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_unit_spec.spec set unit spec for all units in layer
  virtual void  SetUnitType(TypeDef* td);
  // #BUTTON #DYN1 #TYPE_Unit #CAT_Structure #INIT_ARGVAL_ON_units.el_typ set unit type for all units in layer (created by Build)
  virtual void  FixPrjnIndexes();
  // #CAT_Structure fix the projection indexes of the connection groups (other_idx)

  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #DYN1 #CAT_Statistic monitor (record in a datatable) the given variable on this layer (can be a variable on the units or connections -- in which case a matrix with a value for each will be created -- e.g., 'act' will monitor activations of all units within the layer)
  virtual bool  Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #BUTTON #CAT_Statistic take a snapshot of given variable: assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.
  virtual Unit* MostActiveUnit(int& idx);
  // #CAT_Statistic Return the unit with the highest activation (act) value -- index of unit is returned in idx

  virtual void  Lesion();
  // #BUTTON #MENU #MENU_ON_Structure #DYN1 #MENU_SEP_BEFORE #CAT_Structure set the lesion flag on layer -- removes it from all processing operations
  virtual void  UnLesion();
  // #BUTTON #MENU #DYN1 #CAT_Structure un-set the lesion flag on layer -- restores it to engage in normal processing

  virtual int   ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // #CAT_Structure switch any units/layers using old_sp to using new_sp
  virtual int   ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int   ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp
  virtual int   ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp);
  // #CAT_Structure switch any layers using old_sp to using new_sp

  virtual DataTable*    WeightsToTable(DataTable* dt, Layer* send_lay);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send entire set of weights from sending layer to given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit
  virtual DataTable*    VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for projection variables, specify prjns.; for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual DataTable*    ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
               const String& var3 = "", const String& var4 = "", const String& var5 = "",
               const String& var6 = "", const String& var7 = "", const String& var8 = "",
               const String& var9 = "", const String& var10 = "", const String& var11 = "",
               const String& var12 = "", const String& var13 = "", const String& var14 = "",
               Projection* prjn=NULL);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var" -- prjn restricts to that prjn
  virtual DataTable*    PrjnsToTable(DataTable* dt = NULL, bool sending = false);
  // #MENU #MENU_ON_Structure #MENU_SEP_BEFORE #NULL_OK_0 NULL_TEXT_0_NewTable #CAT_Structure record the layer projections (receiving unless sending clicked) to given data table, with one row per projection, including the connection and projection specs used

  virtual bool  VarToVarCopy(const String& dest_var, const String& src_var);
  // #CAT_Structure copy one unit variable to another (un->dest_var = un->src_var) for all units within this layer (must be a float type variable)
  virtual bool  VarToVal(const String& dest_var, float val);
  // #CAT_Structure set variable to given value for all units within this layer (must be a float type variable)

#ifdef DMEM_COMPILE
  DMemShare     dmem_share_units;       // #IGNORE the shared units
  virtual void  DMem_SyncNRecvCons();   // #IGNORE syncronize number of receiving connections (share set 0)
  virtual void  DMem_SyncNet();         // #IGNORE syncronize just the netinputs (share set 1)
  virtual void  DMem_SyncAct();         // #IGNORE syncronize just the activations (share set 2)
  virtual void  DMem_DistributeUnits(); // #IGNORE distribute units to different nodes (for this layer)
  virtual bool  DMem_DistributeUnits_impl(DMemShare& dms); // #IGNORE implementation: if true, a non-standard distribution was used (i.e., unit_groups)
#else
  virtual bool  DMem_DistributeUnits_impl(DMemShare&) { return false; } // #IGNORE to keep the ta file consistent..
#endif

  String GetDesc() const       override { return desc; }
  int    GetEnabled() const    override { return !lesioned(); }
  void   SetEnabled(bool value) override { SetLayerFlagState(LESIONED, !value); }
  String GetTypeDecoKey() const override { return "Layer"; }
  int    GetSpecialState() const override;

  bool ChangeMyType(TypeDef* new_type) override;

  DumpQueryResult Dump_QuerySaveMember(MemberDef* md) override;

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const Layer& cp);
  TA_BASEFUNS(Layer); //

protected:
  UnitSpec*     m_prv_unit_spec; // previous unit spec set for units in layer
  LayerFlags    m_prv_layer_flags; // previous layer flags in layer

  void         UpdateAfterEdit_impl() override;
  void         UpdateAfterMove_impl(taBase* old_owner) override;
  virtual void          ApplyLayerFlags(Unit::ExtType act_ext_flags);
  // #IGNORE set layer flag to reflect the kind of input received
  virtual void          ApplyInputData_1d(taMatrix* data, Unit::ExtType ext_flags,
                               Random* ran, bool na_by_range=false);
  // #IGNORE 1d data -- just go in order -- offsets ignored
  virtual void          ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
                               Random* ran, const taVector2i& offs, bool na_by_range=false);
  // #IGNORE 2d data is always treated the same: UnitAtCoord deals with unit grouping
  virtual void          ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
                               Random* ran, const taVector2i& offs, bool na_by_range=false);
  // #IGNORE flat layer, 4d data
  virtual void          ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags,
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
