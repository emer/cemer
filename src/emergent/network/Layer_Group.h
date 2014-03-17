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

#ifndef Layer_Group_h
#define Layer_Group_h 1

// parent includes:
#include <taGroup>
#include <Layer>

// smartptr, ref:
#include <taSmartPtrT>
#include <taSmartRefT>

// member includes:

// declare all other types mentioned but not required to include:
class Network; //

eTypeDef_Of(Layer_Group);

class E_API Layer_Group : public taGroup<Layer> {
  // ##CAT_Network ##SCOPE_Network group of layers -- this should be used in larger networks to organize subnetworks (e.g., in brain models, different brain areas)
INHERITED(taGroup<Layer>)
public:
  static bool nw_itm_def_arg;   // #IGNORE default arg val for FindMake..

  PosVector3i    pos;           // Position of Group of layers relative to network (0,0,0 is lower left hand corner)
  PosVector2i    pos2d;		// #CAT_Structure 2D network view display position of layer group relative to the overall nework (0,0 is lower left hand corner)
  PosVector3i    max_disp_size;  // #AKA_max_size #READ_ONLY #SHOW #CAT_Structure maximum display size of the layer group -- computed automatically from the layers within the group
  PosVector2i    max_disp_size2d;  // #READ_ONLY #SHOW #CAT_Structure maximum 2D display size of the layer group -- computed automatically from the layers within the group

  void          GetAbsPos(taVector3i& abs_pos)
  { abs_pos = pos; AddRelPos(abs_pos); }
  // #CAT_Structure get absolute pos, which factors in offsets from layer groups
  void          AddRelPos(taVector3i& rel_pos);
  // #CAT_Structure add relative pos from layer groups, which factors in offsets from layer groups
  void          GetAbsPos2d(taVector2i& abs_pos) { abs_pos = pos2d; AddRelPos2d(abs_pos); }
  // #CAT_Structure get absolute pos, which factors in offsets from layer groups
  void          AddRelPos2d(taVector2i& rel_pos);
  // #CAT_Structure add relative pos from layer groups, which factors in offsets from layer groups

  virtual void  BuildLayers();
  // #CAT_Structure create any algorithmically specified layers
  virtual void  BuildPrjns();
  // #CAT_Structure create any algorithmically specified prjns
  virtual void  UpdateMaxDispSize();
  // #IGNORE update max_disp_size of layer group based on current layer layout

  virtual void  LesionLayers();
  // #BUTTON #DYN1 #CAT_Structure set the lesion flag on all the layers within this group -- removes them from all processing operations
  virtual void  UnLesionLayers();
  // #BUTTON #DYN1 #CAT_Structure un-set the lesion flag on all the layers within this group -- restores them to engage in normal processing
  virtual void  IconifyLayers();
  // #BUTTON #DYN1 #CAT_Structure iconi
  virtual void  DeIconifyLayers();
  // #BUTTON #DYN1 #CAT_Structure un-set the lesion flag on all the layers within this group
  virtual void  DispScaleLayers(float disp_scale = 1.0f);
  // #BUTTON #DYN1 #CAT_Structure set the display scale on all the layers in the group (scales the size of the units -- 1 = normal, lower = smaller units, higher = larger units.

  virtual void  Clean();
  // #MENU #MENU_CONTEXT #CAT_Structure remove any algorithmically specified layers/prjns etc.

  virtual void  LayerPos_Cleanup();
  // #MENU #MENU_CONTEXT #CAT_Structure cleanup the layer positions relative to each other (prevent overlap etc) -- both 2d and 3d positions
  virtual void  LayerPos_Cleanup_2d();
  // #MENU #MENU_CONTEXT #CAT_Structure cleanup the layer positions relative to each other (prevent overlap etc) -- 2d positions
  virtual void  LayerPos_Cleanup_3d();
  // #MENU #MENU_CONTEXT #CAT_Structure cleanup the layer positions relative to each other (prevent overlap etc) -- 3d positions

  virtual void  LayerPos_GridLayout_2d(int x_space = 2, int y_space = 3,
                                       int gp_grid_x = -1, int lay_grid_x = -1);
  // #MENU #MENU_CONTEXT #CAT_Structure for the 2D layer positions: arrange layers and layer subgroups into a grid with given spacing, and you can optionally constrain the x (horizontal) dimension of the grid for the subgroups within the network or layers within groups (or just the layers if no subgroups) if gp_grid_x > 0 or layer_grid_x > 0
  virtual void  LayerPos_GridLayout_Sub_2d(int x_space = 2, int y_space = 3,
                                           int lay_grid_x = -1);
  // #IGNORE 
  virtual void  LayerPos_GridLayout_Gps_2d(int x_space = 2, int y_space = 3,
                                           int gp_grid_x = -1, int lay_grid_x = -1);
  // #IGNORE 

  virtual void  LayerPos_GridLayout_3d(int x_space = 2, int y_space = 3,
                                       int z_size = 3, int gp_grid_x = -1,
                                       int lay_grid_x = -1);
  // #MENU #MENU_CONTEXT #CAT_Structure for the 3D layer positions: arrange layers and layer subgroups into a grid with given spacing, distributed across given number of z (vertical) layers, and you can optionally constrain the x (horizontal) dimension of the grid for the subgroups within the network or layers within groups (or just the layers if no subgroups) if gp_grid_x > 0 or layer_grid_x > 0
  virtual void  LayerPos_GridLayout_Sub_3d(int x_space = 2, int y_space = 3,
                                           int lay_grid_x = -1);
  // #IGNORE 
  virtual void  LayerPos_GridLayout_NoSub_3d(int x_space = 2, int y_space = 3,
                                             int z_size = 3, int lay_grid_x = -1);
  // #IGNORE 
  virtual void  LayerPos_GridLayout_Gps_3d(int x_space = 2, int y_space = 3,
                                           int z_size = 3,
                                           int gp_grid_x = -1, int lay_grid_x = -1);
  // #IGNORE 

  virtual Layer* FindMakeLayer(const String& nm, TypeDef* td = NULL,
                               bool& nw_itm = nw_itm_def_arg, const String& alt_nm = "");
  // #CAT_Structure find a given layer and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
  virtual Layer_Group* FindMakeLayerGroup(const String& nm, TypeDef* td = NULL,
                          bool& nw_itm = nw_itm_def_arg, const String& alt_nm = "");
  // #CAT_Structure find a given layer group and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)

  void          TriggerContextUpdate();
  // #CAT_Activation for context layers, manually triggers the update

  String GetTypeDecoKey() const override { return "Layer"; }

  void  SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);
  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(Layer_Group);
protected:
  void UpdateAfterEdit_impl() override;
  virtual void  BuildLayers_impl();
  virtual void  BuildPrjns_impl();
  virtual void  Clean_impl() {}
private:
  void  Initialize()            { };
  void  Destroy()               { };
  void  Copy_(const Layer_Group& cp);
};

TA_SMART_PTRS(E_API, Layer_Group)

#endif // Layer_Group_h
