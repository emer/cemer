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

#include "Layer_Group.h"
#include <Network>
#include <Layer>

#include <SigLinkSignal>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Layer_Group);
SMARTREF_OF_CPP(Layer_Group)


bool Layer_Group::nw_itm_def_arg = false;

void Layer_Group::Initialize() {
  setUseStale(true);
}  
  
void Layer_Group::InitLinks() {
  inherited::InitLinks();
  taBase::Own(pos,this);
  taBase::Own(pos2d,this);
  taBase::Own(max_disp_size,this);
  taBase::Own(max_disp_size2d,this);
}

void Layer_Group::CutLinks() {
  pos.CutLinks();
  pos2d.CutLinks();
  inherited::CutLinks();
}

void Layer_Group::Copy_(const Layer_Group& cp) {
  pos = cp.pos;
  pos2d = cp.pos2d;
  max_disp_size = cp.max_disp_size;
  max_disp_size2d = cp.max_disp_size2d;
}

void Layer_Group::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {
    prev_pos = pos;
    prev_pos2d = pos2d;
  }
  // if(pos != prev_pos) {
  //   taVector3i del = pos - prev_pos;
  //   MovePos(del.x, del.y, del.z);
  // }
  // if(pos2d != prev_pos2d) {
  //   taVector2i del = pos2d - prev_pos2d;
  //   MovePos2d(del.x, del.y);
  // }
}

void Layer_Group::AddRelPos(taVector3i& rel_pos) {
  Layer_Group* lg = GET_MY_OWNER(Layer_Group);
  if (lg) {
    rel_pos += lg->pos;
    lg->AddRelPos(rel_pos);
  }
}

void Layer_Group::AddRelPos2d(taVector2i& rel_pos) {
  Layer_Group* lg = GET_MY_OWNER(Layer_Group);
  if (lg) {
    rel_pos += lg->pos2d;
    lg->AddRelPos2d(rel_pos);
  }
}

void Layer_Group::UpdateLayerGroupGeom() {
  max_disp_size = 1;
  max_disp_size2d = 1;

  taVector3i min_size;
  taVector2i min_size2d;
  bool first_min = true;

  taVector3i l_pos; // for abs_pos
  taVector2i l_pos2d; // for abs_pos
  FOREACH_ELEM_IN_GROUP(Layer, l, *this) {
    l->GetAbsPos(l_pos);
    l->GetAbsPos2d(l_pos2d);
    taVector3i lrelpos = l_pos - pos; // subtract us
    taVector2i lrelpos2d = l_pos2d - pos2d; // subtract us
    max_disp_size.z = MAX(max_disp_size.z, 1 + lrelpos.z);
    if(l->Iconified()) {
      max_disp_size.x = MAX(max_disp_size.x, lrelpos.x + 1);
      max_disp_size.y = MAX(max_disp_size.y, lrelpos.y + 1);

      max_disp_size2d.x = MAX(max_disp_size2d.x, lrelpos2d.x + 1);
      max_disp_size2d.y = MAX(max_disp_size2d.y, lrelpos2d.y + 1);
    }
    else {
      max_disp_size.x = MAX(max_disp_size.x, l->scaled_disp_geom.x + lrelpos.x);
      max_disp_size.y = MAX(max_disp_size.y, l->scaled_disp_geom.y + lrelpos.y);

      max_disp_size2d.x = MAX(max_disp_size2d.x, l->scaled_disp_geom.x + lrelpos2d.x);
      max_disp_size2d.y = MAX(max_disp_size2d.y, l->scaled_disp_geom.y + lrelpos2d.y);
    }
    if(first_min) {
      first_min = false;
      min_size = l_pos;
      min_size2d = l_pos2d;
    }
    else {
      min_size.Min(l_pos);      // min of each coord
      min_size2d.Min(l_pos2d);      // min of each coord
    }
  }

  if(!owner->InheritsFrom(&TA_Network)) {
    TestWarning(gp.size > 0, "UpdateLayerGroupGeom",
                "layout of layer groups within layer groups is NOT supported");
    if(min_size != pos) {
      taVector3i pos_chg = min_size - pos;
      FOREACH_ELEM_IN_GROUP(Layer, l, *this) {
        l->pos -= pos_chg;      // fix up all the layer rels to be less.
        l->SigEmitUpdated();
      }
      max_disp_size -= pos_chg; // reduce by amount moving
      pos = min_size;           // new position
    }
    if(min_size2d != pos2d) {
      taVector2i pos_chg = min_size2d - pos2d;
      FOREACH_ELEM_IN_GROUP(Layer, l, *this) {
        l->pos2d -= pos_chg;      // fix up all the layer rels to be less.
        l->SigEmitUpdated();
      }
      max_disp_size2d -= pos_chg; // reduce by amount moving
      pos2d = min_size2d;           // new position
    }
  }
  else {
    // iterate on subgroups -- only for top-level guy
    for(int gi=0; gi<gp.size; gi++) {
      Layer_Group* lgp = (Layer_Group*)FastGp(gi);
      lgp->UpdateLayerGroupGeom();
    }
  }
  prev_pos = pos;
  prev_pos2d = pos2d;
  // IMPORTANT: CANNOT call sigemit here -- this messes with tree updating!
  // SigEmitUpdated();
}

void Layer_Group::MovePos(int x, int y, int z) {
  taVector3i nwpos = pos;  // temp copy of current group position
  nwpos.x += x;  nwpos.y += y;  nwpos.z += z;
  if(nwpos.x < 0) { x -= nwpos.x; }
  if(nwpos.y < 0) { y -= nwpos.y; }
  if(nwpos.z < 0) { z -= nwpos.z; }
  taVector3i del(x,y,z);
  FOREACH_ELEM_IN_GROUP(Layer, l, *this) {
    l->pos += del;
    l->pos_abs += del;
    l->SigEmitUpdated();
  }
  UpdateLayerGroupGeom();       // double check..
}

void Layer_Group::MovePos2d(int x, int y) {
  taVector2i nwpos = pos2d;
  nwpos.x += x;  nwpos.y += y;
  if(nwpos.x < 0) { x -= nwpos.x; }
  if(nwpos.y < 0) { y -= nwpos.y; }
  taVector2i del(x,y);
  FOREACH_ELEM_IN_GROUP(Layer, l, *this) {
    l->pos2d += del;
    l->pos2d_abs += del;
    l->SigEmitUpdated();
  }
  UpdateLayerGroupGeom();       // double check..
}

void Layer_Group::LayerPos_Cleanup() {
  bool moved = false;
  int n_itr = 0;
  do {
    moved = false;
    for(int i1=0;i1<leaves;i1++) {
      Layer* l1 = Leaf(i1);
      taVector2i l1s = (taVector2i)l1->pos_abs;
      taVector2i l1e = l1s + (taVector2i)l1->scaled_disp_geom;
      if(l1->pos_rel.IsRel())
        continue;

      for(int i2 = i1+1; i2<leaves;i2++) {
        Layer* l2 = Leaf(i2);
        taVector2i l2s = (taVector2i)l2->pos_abs;
        taVector2i l2e = l2s + (taVector2i)l2->scaled_disp_geom;
        if(l2->pos_rel.IsRel())
          continue;

        if(l2->pos_abs.z == l1->pos_abs.z) { // 3D
	  if(l2s.x >= l1s.x && l2s.x < l1e.x &&
	     l2s.y >= l1s.y && l2s.y < l1e.y) { // l2 starts in l1; move l2 rt/back
            taVector3i nps = l2->pos_abs;
	    if(l1e.x - l2s.x <= l1e.y - l2s.y) {    // closer to x than y
              l2->PositionRightOf(l1);
	    }
	    else {
              l2->PositionBehind(l1);
	    }
	    moved = true;
	  }
	  else if(l1s.x >= l2s.x && l1s.x < l2e.x &&
		  l1s.y >= l2s.y && l1s.y < l2e.y) { // l1 starts in l2; move l1 rt/back
            taVector3i nps = l1->pos_abs;
	    if(l2e.x - l1s.x <= l2e.y - l1s.y) {    // closer to x than y
              l2->PositionRightOf(l1);               // always move l2 to prevent loops!
	    }
	    else {
              l2->PositionBehind(l1);
	    }
	    moved = true;
	  }
	}
      }
    }
    n_itr++;
  } while(moved && n_itr < 10);
}

void Layer_Group::LayerPos_GridLayout_3d(int x_space, int y_space, int z_size,
                                         int gp_grid_x, int lay_grid_x) {
  if(gp.size > 0) {
    LayerPos_GridLayout_Gps_3d(x_space, y_space, z_size, gp_grid_x, lay_grid_x);
  }
  else {
    LayerPos_GridLayout_NoSub_3d(x_space, y_space, z_size, lay_grid_x);
  }
}

void Layer_Group::LayerPos_GridLayout_Sub_3d(int x_space, int y_space,
                                             int lay_grid_x) {
  // this is all within a single z layer
  taVector2i grid;
  if(lay_grid_x < 1) {
    grid.FitN(leaves);
  }
  else {
    grid.x = lay_grid_x;
    grid.y = leaves / grid.x;
    while(grid.x * grid.y < leaves) grid.y++;
  }
  int li = 0;
  for(int y=0; y<grid.y; y++) {
    for(int x=0; x<grid.x; x++, li++) {
      if(li >= leaves) break;
      Layer* l1 = Leaf(li);
      taVector3i nps = pos;     // layer group position
      if(li == 0) {
        // nop
      }
      else if(x == 0) {
        Layer* l2 = Leaf((y-1) * grid.x);
        l1->PositionBehind(l2, y_space);
      }
      else {
        Layer* l2 = Leaf(li-1);
        l1->PositionRightOf(l2, x_space);
      }
    }
  }
  UpdateLayerGroupGeom();
}

void Layer_Group::LayerPos_GridLayout_NoSub_3d(int x_space, int y_space, int z_size,
                                               int lay_grid_x) {
  taVector2i grid;
  int lvmod = leaves / z_size;
  while (lvmod * z_size < leaves) lvmod++;
  if(lay_grid_x < 1) {
    grid.FitN(lvmod);
  }
  else {
    grid.x = lay_grid_x;
    grid.y = lvmod / grid.x;
    while(grid.x * grid.y < lvmod) grid.y++;
  }
  int li = 0;
  for(int z=0; z<z_size; z++) {
    for(int y=0; y<grid.y; y++) {
      for(int x=0; x<grid.x; x++, li++) {
        if(li >= leaves) break;
        Layer* l1 = Leaf(li);
        taVector3i nps;
        if(x == 0 && y == 0) {
          nps = 0;
          nps.z = z;
          l1->SetAbsPos(nps);
        }
        else if(x == 0) {
          Layer* l2 = Leaf(z * grid.x * grid.y + (y-1) * grid.x);
          l1->PositionBehind(l2, y_space);
        }
        else {
          Layer* l2 = Leaf(li-1);
          l1->PositionRightOf(l2, x_space);
        }
      }
    }
  }
  UpdateLayerGroupGeom();
}

void Layer_Group::LayerPos_GridLayout_Gps_3d(int x_space, int y_space, int z_size,
                                             int gp_grid_x, int lay_grid_x) {
  taVector2i grid;
  int lvmod = gp.size / z_size;
  while (lvmod * z_size < gp.size) lvmod++;
  if(gp_grid_x < 1) {
    grid.FitN(lvmod);
  }
  else {
    grid.x = gp_grid_x;
    grid.y = lvmod / grid.x;
    while(grid.x * grid.y < lvmod) grid.y++;
  }
  int li = 0;
  for(int z=0; z<z_size; z++) {
    int last_max_y = 0;
    for(int y=0; y<grid.y; y++) {
      for(int x=0; x<grid.x; x++, li++) {
        if(li >= gp.size) break;
        Layer_Group* l1 = (Layer_Group*)FastGp(li);
        Layer* ly1 = l1->SafeEl(0);
        if(x == 0 && y == 0) {
        }
        else if(x == 0) {
          Layer_Group* l2 = (Layer_Group*)FastGp(z * grid.x * grid.y + (y-1) * grid.x);
          Layer* ly2 = l2->SafeEl(0);
          if(ly1 && ly2) {
            ly1->PositionBehind(ly2);
          }
        }
        else {
          Layer_Group* l2 = (Layer_Group*)FastGp(li-1);
          Layer* ly2 = l2->SafeEl(0);
          if(ly1 && ly2) {
            ly1->PositionRightOf(ly2);
          }
        }
        l1->LayerPos_GridLayout_Sub_3d(x_space, y_space, lay_grid_x);
        l1->SigEmitUpdated();
      }
    }
  }
}

void Layer_Group::TriggerContextUpdate() {
  FOREACH_ELEM_IN_GROUP_REV(Layer, lay, *this) {
    lay->TriggerContextUpdate();
  }
}

void Layer_Group::LesionLayers() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, *this) {
    lay->Lesion();
  }
}

void Layer_Group::UnLesionLayers() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, *this) {
    lay->UnLesion();
  }
}

void Layer_Group::IconifyLayers() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, *this) {
    lay->Iconify();
  }
  UpdateLayerGroupGeom();
  UpdateAfterEdit();
}

void Layer_Group::DeIconifyLayers() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, *this) {
    lay->DeIconify();
  }
  UpdateLayerGroupGeom();
  UpdateAfterEdit();
}

void Layer_Group::DispScaleLayers(float disp_scale) {
  FOREACH_ELEM_IN_GROUP(Layer, lay, *this) {
    lay->SetDispScale(disp_scale);
  }
  UpdateLayerGroupGeom();
  UpdateAfterEdit();
}

Layer* Layer_Group::FindMakeLayer(const String& nm, TypeDef* td, bool& nw_itm,
                                  const String& alt_nm) {
  nw_itm = false;
  Layer* lay = (Layer*)FindName(nm);
  if(!lay && alt_nm.nonempty()) {
    lay = (Layer*)FindName(alt_nm);
    if(lay) lay->name = nm;
  }
  if(!lay) {
    lay = (Layer*)NewEl(1, td);
    lay->name = nm;
    nw_itm = true;
  }
  if((td) && !lay->InheritsFrom(td)) {
    RemoveEl(lay);
    lay = (Layer*)NewEl(1, td);
    lay->name = nm;
    nw_itm = true;
  }
  return lay;
}

Layer_Group* Layer_Group::FindMakeLayerGroup(const String& nm, TypeDef* td, bool& nw_itm,
                                       const String& alt_nm) {
  nw_itm = false;
  Layer_Group* lay = (Layer_Group*)gp.FindName(nm);
  if(!lay && alt_nm.nonempty()) {
    lay = (Layer_Group*)gp.FindName(alt_nm);
    if(lay) lay->name = nm;
  }
  if(!lay) {
    lay = (Layer_Group*)NewGp(1, td);
    lay->name = nm;
    nw_itm = true;
  }
  if((td) && !lay->InheritsFrom(td)) {
    gp.RemoveEl(lay);
    lay = (Layer_Group*)NewGp(1, td);
    lay->name = nm;
    nw_itm = true;
  }
  return lay;
}

void Layer_Group::RestorePanels() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, *this) {
    if(lay->GetUserDataAsBool("user_pinned")) {
      lay->EditPanel(true, true); // true,true = new tab, pinned in place
    }
  }
}

void Layer_Group::SetLayerSpec(LayerSpec* layspec) {
  FOREACH_ELEM_IN_GROUP(Layer, l, *this) {
    l->SetLayerSpec(layspec);
  }
}

void Layer_Group::SetUnitSpec(UnitSpec* unitspec) {
  FOREACH_ELEM_IN_GROUP(Layer, l, *this) {
    l->SetUnitSpec(unitspec);
  }
}

