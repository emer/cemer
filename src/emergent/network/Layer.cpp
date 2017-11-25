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

#include "Layer.h"
#include <Network>
#include <MemberDef>
#include <ProjectBase>
#include <taFiler>
#include <NetMonitor>
#include <DataTable>
#include <taiEdit>

#include <tabMisc>
#include <taMisc>

TA_BASEFUNS_CTORS_LITE_DEFN(LayerDistances);
TA_BASEFUNS_CTORS_DEFN(LayerRelPos);
TA_BASEFUNS_CTORS_DEFN(Layer);

using namespace std;

#include <ProjectionSpec_cpp>

#include <State_main>

#undef LAYER_STATE
#define LAYER_STATE Layer

#include "Layer_core.cpp"

#include <State_main>

void LayerRelPos::Initialize() {
  rel = ABS_POS;
  x_align = LEFT;
  x_off = 0;
  y_align = FRONT;
  y_off = 0;
  space = 2;
}

bool LayerRelPos::ComputePos3D(taVector3i& pos, Layer* lay) {
  if(!other || rel == ABS_POS)
    return false;
  pos.z = other->pos_abs.z;     // default
  ComputePos2D_impl(pos, lay, other->pos_abs);
  if(rel == ABOVE) {
    pos.z = other->pos_abs.z + 1;
  }
  else if(rel == BELOW) {
    pos.z = other->pos_abs.z - 1;
  }
  return true;
}

bool LayerRelPos::ComputePos2D(taVector2i& pos, Layer* lay) {
  if(!other || rel == ABS_POS)
    return false;
  ComputePos2D_impl(pos, lay, other->pos2d_abs);
  if(rel == ABOVE) {
    pos.y = other->pos2d_abs.y + space;
  }
  else if(rel == BELOW) {
    pos.y = other->pos2d_abs.y - space;
  }
  return true;
}


static inline int layer_pos_eff_geom_x(Layer* lay) {
  if(lay->Iconified()) return 0;
  return lay->scaled_disp_geom.x;
}

static inline int layer_pos_eff_geom_y(Layer* lay) {
  if(lay->Iconified()) return 0;
  return lay->scaled_disp_geom.y;
}

bool LayerRelPos::ComputePos2D_impl(taVector2i& pos, Layer* lay, const taVector2i& oth_pos) {
  if(!other)
    return false;
  switch(rel) {
  case ABS_POS: {
    return false;
  }
  case RIGHT_OF: {
    pos.x = oth_pos.x + layer_pos_eff_geom_x(other) + space;
    break;
  }
  case LEFT_OF: {
    pos.x = oth_pos.x - layer_pos_eff_geom_x(lay) - space;
    break;
  }
  case BEHIND: {
    pos.y = oth_pos.y + layer_pos_eff_geom_y(other) + space;
    break;
  }
  case FRONT_OF: {
    pos.y = oth_pos.y - layer_pos_eff_geom_y(lay) - space;
    break;
  }
  default:
    break;
  }

  if(!(rel == LEFT_OF || rel == RIGHT_OF)) {
    switch(x_align) {
    case LEFT: {
      pos.x = x_off + oth_pos.x;
      break;
    }
    case MIDDLE: {
      pos.x = x_off + (oth_pos.x + (other->scaled_disp_geom.x / 2)) - (lay->scaled_disp_geom.x / 2);
      break;
    }
    case RIGHT: {
      pos.x = x_off + (oth_pos.x + other->scaled_disp_geom.x) - lay->scaled_disp_geom.x;
      break;
    }
    }
  }
  if(!(rel == FRONT_OF || rel == BEHIND)) {
    switch(y_align) {
    case FRONT: {
      pos.y = y_off + oth_pos.y;
      break;
    }
    case CENTER: {
      pos.y = y_off + (oth_pos.y + (other->scaled_disp_geom.y / 2)) - (lay->scaled_disp_geom.y / 2);
      break;
    }
    case BACK: {
      pos.y = y_off + (oth_pos.y + other->scaled_disp_geom.y) - lay->scaled_disp_geom.y;
      break;
    }
    }
  }
  return true;
}


void Layer::Initialize() {

  Initialize_lay_core();
  main_obj = true;
  
  // desc = ??
  own_net = NULL;
  // pos = ??
  disp_scale = 1.0f;
  // un_geom = ??
  unit_groups = false;
  // gp_geom = ??
  gp_spc.x = 1;
  gp_spc.y = 1;
  n_recv_prjns = 0;
  n_send_prjns = 0;
  projections.SetBaseType(&TA_Projection);
  send_prjns.send_prjns = true;
  // unit_spec = ??
  // flat_geom = ??
  disp_geom = un_geom;
  scaled_disp_geom = disp_geom;
  // dist = ??
  // output_name = ??
  // gp_output_names = ??
  m_prv_unit_spec = NULL;
  m_prv_layer_flags = LF_NONE;
  n_units_built = 0;

  // prerr = ??
  units_lesioned = false;
  gp_unit_names_4d = false;
  // unit_names = ??
  brain_area = "";
  voxel_fill_pct = 1.0f;
  layer_idx = -1;
  
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

void Layer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(pos_rel, this);
  taBase::Own(pos, this);
  taBase::Own(pos_abs, this);
  taBase::Own(pos2d, this);
  taBase::Own(pos2d_abs, this);
  
  taBase::Own(un_geom, this);
  taBase::Own(gp_geom, this);
  taBase::Own(gp_spc, this);
  taBase::Own(flat_geom, this);
  taBase::Own(disp_geom, this);
  taBase::Own(scaled_disp_geom, this);
  
  taBase::Own(projections, this);
  taBase::Own(send_prjns, this);
  
  taBase::Own(unit_spec, this);
  taBase::Own(unit_names, this);
  taBase::Own(dist, this);
  taBase::Own(gp_output_names, this);
  
  taBase::Own(avg_sse, this);
  taBase::Own(prerr, this);
  taBase::Own(sum_prerr, this);
  taBase::Own(epc_prerr, this);

  AutoNameMyMembers();

  own_net = GET_MY_OWNER(Network);
  if(own_net)
    own_net->ClearIntact();
  if(!taMisc::is_loading) {
    SetDefaultPos();
  }
  unit_spec.SetDefaultSpec(this);

#ifdef DMEM_COMPILE
  taBase::Own(dmem_agg_sum, this);
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

void Layer::CutLinks() {
  if(!owner) return; // already replacing or already dead
  if(own_net)
    own_net->ClearIntact();
  DisConnect_impl();
  pos_rel.CutLinks();
  pos.CutLinks();
  pos_abs.CutLinks();
  pos2d.CutLinks();
  pos2d_abs.CutLinks();
  
  un_geom.CutLinks();
  gp_geom.CutLinks();
  gp_spc.CutLinks();
  flat_geom.CutLinks();
  disp_geom.CutLinks();
  scaled_disp_geom.CutLinks();
  
  projections.CutLinks();
  send_prjns.CutLinks();
  
  unit_spec.CutLinks();
  unit_names.CutLinks();
  dist.CutLinks();
  gp_output_names.CutLinks();
  
  avg_sse.CutLinks();
  prerr.CutLinks();
  sum_prerr.CutLinks();
  epc_prerr.CutLinks();
  
  m_prv_unit_spec = NULL;
  m_prv_layer_flags = LF_NONE;
  own_net = NULL;
  inherited::CutLinks();
}


void Layer::Copy_(const Layer& cp) {
  // if(own_net && !own_net->HasBaseFlag(COPYING)) {
  //  // if we're the only guy copying, then all connections are invalid now -- just nuke
  //  own_net->UnBuild();
  //  note: this causes crashing!!  update at network level interferes with gui tree
  // }
  desc = cp.desc;
  flags = cp.flags;
  layer_type = cp.layer_type;
  pos_rel = cp.pos_rel;
  pos = cp.pos;
  pos_abs = cp.pos_abs;
  pos2d = cp.pos2d;
  pos2d_abs = cp.pos2d_abs;
  disp_scale = cp.disp_scale;
  un_geom = cp.un_geom;
  unit_groups = cp.unit_groups;
  gp_geom = cp.gp_geom;
  gp_spc = cp.gp_spc;
  flat_geom = cp.flat_geom;
  disp_geom = cp.disp_geom;
  scaled_disp_geom = cp.scaled_disp_geom;
  n_recv_prjns = cp.n_recv_prjns;
  n_send_prjns = cp.n_send_prjns;
  projections = cp.projections;
  unit_spec = cp.unit_spec;
  ext_flag = cp.ext_flag;
  m_prv_unit_spec = cp.m_prv_unit_spec;
  m_prv_layer_flags = cp.m_prv_layer_flags;

  output_name = cp.output_name;
  gp_output_names = cp.gp_output_names;
  sse = cp.sse;
  prerr = cp.prerr;
  icon_value = cp.icon_value;
  gp_unit_names_4d = cp.gp_unit_names_4d;
  unit_names = cp.unit_names;

  brain_area = cp.brain_area;
  voxel_fill_pct = cp.voxel_fill_pct;

  // not copied
  //  send_prjns.BorrowUnique(cp.send_prjns); // link group
}

void Layer::UpdateAfterMove_impl(taBase* old_owner) {
  inherited::UpdateAfterMove_impl(old_owner);

  if(!old_owner) return;
  Network* mynet = GET_MY_OWNER(Network);
  Network* otnet = (Network*)old_owner->GetOwner(&TA_Network);
  if(!mynet || !otnet || mynet == otnet) return;  // don't update if not relevant
  UpdatePointers_NewPar(otnet, mynet);
}

void Layer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  // taMisc::DebugInfo("layer uae", name);

  // no negative geoms., y,z must be 1 (for display)
  UnitSpecUpdated();
  //  SyncSendPrjns(); // this is not a good place to do this -- too frequent and unnec
  // also causes problems during copy..

  if(taMisc::is_loading) {
    if(un_geom.z > 1) {         // obs: v3compat conversion
      gp_geom.UpdateAfterEdit_NoGui();  // get n from xy
      unit_groups = true;
      if(gp_geom.n != un_geom.z) {
        gp_geom.n_not_xy = true;
        gp_geom.n = un_geom.z;
      }
      un_geom.z = 0;
    }
    taVersion v511(5, 1, 1);
    if(taMisc::loading_version < v511) { // update the gp_unit_names_4d flag
      if(unit_groups && unit_names.dims() == 4)
        gp_unit_names_4d = true;
      else
        gp_unit_names_4d = false;
    }
    taVersion v801(8, 0, 1);
    if(taMisc::loading_version < v801) { // update the SAVE_UNIT_NAMES flag
      if(unit_names.dims() > 0)
        SetLayerFlag(SAVE_UNIT_NAMES);
    }
    UpdateGeometry();
    UpdatePosition();
  }
  else {                        // not loading
    // taMisc::DebugInfo("lay uae:", name);
    UpdateGeometry();
    UpdatePosition();
    if(own_net) {
      own_net->LayerPos_RelPos();
    }
    UpdateSendPrjnNames();

    // this flag is read only so we don't really need this -- everything should be in
    // methods!
    // if(lesioned() && !(m_prv_layer_flags & LESIONED)) {
    // }
    // else if(!lesioned() && (m_prv_layer_flags & LESIONED)) {
    // }

    if(HasLayerFlag(SAVE_UNIT_NAMES)) {
      if(unit_groups) {
        if(gp_unit_names_4d) {
          unit_names.SetGeom(4, un_geom.x, un_geom.y, gp_geom.x, gp_geom.y);
        }
        else {
          unit_names.SetGeom(2, un_geom.x, un_geom.y);
        }
      }
      else {
        unit_names.SetGeom(2, un_geom.x, un_geom.y);
      }
    }
    else {
      unit_names.SetGeom(0, 0);
    }
  }

  if(unit_groups && gp_geom.n > 0) {
    gp_output_names.SetGeom(2,gp_geom.x,gp_geom.y);
  }

  m_prv_layer_flags = flags;
}

NetworkState_cpp* Layer::GetValidNetState() const {
  if(!own_net || !own_net->net_state || !own_net->net_state->IsBuiltIntact())
    return NULL;
  return own_net->net_state;
}

void Layer::Lesion() {
  if(lesioned()) return;
  StructUpdate(true);
  SetLayerFlag(LESIONED);
  m_prv_layer_flags = flags;
  UpdateAllPrjns();
  NetworkState_cpp* net = GetValidNetState();
  if(net) {
    LayerState_cpp* lst = GetLayerState(net);
    lst->LesionState(net);
  }
  StructUpdate(false);
  if(own_net)
    own_net->UpdtAfterNetMod();
}

void Layer::LesionIconify() {
  Lesion();
  Iconify();
}

void Layer::UnLesion() {
  if (!lesioned()) return;
  NetworkState_cpp* net = GetValidNetState();
  if(net && layer_idx < 0) {
    if(taMisc::gui_active) {
      int chs = taMisc::Choice("Can only unlesion layers that were originally unlesioned when network was built (only those layers were built into LayerState etc) -- do you want to UnBuild the network and then unlesion this layer?", "UnBuild and Proceed", "Cancel");
      if(chs == 0) {
        own_net->UnBuild();
        net = NULL;
      }
      else {
        return;
      }
    }
    else {
      TestError(true, "UnLesion", "Can only unlesion layers that were originally unlesioned when network was built (only those layers were built into LayerState etc) -- do you want to UnBuild the network and then unlesion this layer?");
      return;
    }
  }
  else if(net) {
    ClearLayerFlag(LESIONED);
    LayerState_cpp* lst = GetLayerState(net);
    lst->UnLesionState(net);
  }
  ClearLayerFlag(LESIONED);
  StructUpdate(true);
  UpdateAllPrjns();
  m_prv_layer_flags = flags;
  StructUpdate(false);
  if(own_net)
    own_net->UpdtAfterNetMod();
}

void Layer::UnLesionDeIconify() {
  UnLesion();
  DeIconify();
}
  
void Layer::Iconify() {
  StructUpdate(true);
  SetLayerFlag(ICONIFIED);
  StructUpdate(false);
}

void Layer::DeIconify() {
  StructUpdate(true);
  ClearLayerFlag(ICONIFIED);
  StructUpdate(false);
}

int Layer::GetSpecialState() const {
  if(layer_type == HIDDEN) return 0;
  if(layer_type == INPUT) return 3; // green
  return 4;     // red for output/target layers
}

void Layer::ConnectFrom(Layer* from_lay) {
  if (!own_net) return;
  Projection* prjn = own_net->FindMakePrjn(this, from_lay);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
}

void Layer::ConnectBidir(Layer* from_lay) {
  if (!own_net) return;
  Projection* prjn = own_net->FindMakePrjn(this, from_lay);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
  prjn = own_net->FindMakePrjn(from_lay, this);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
}

void Layer::ConnectSelf() {
  if (!own_net) return;
  Projection* prjn = own_net->FindMakeSelfPrjn(this);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
}

taBase::DumpQueryResult Layer::Dump_QuerySaveMember(MemberDef* md) {
  // only save unit_names if any (to ease backwards compat)
  if (md->name != "unit_names")
    return inherited::Dump_QuerySaveMember(md);
  return (unit_names.dims()) ? DQR_SAVE : DQR_NO_SAVE;
}

void Layer::SyncSendPrjns() {
  for(int pi=0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    Layer* snd = prjn->from;
    if(snd == NULL) continue;
    snd->send_prjns.LinkUnique(prjn); // make sure senders are all represented
  }
  // make sure that we don't have any spurious ones, or any duplicates!
  for(int pi = send_prjns.size-1; pi >= 0; pi--) {
    Projection* prjn = send_prjns[pi];
    if(prjn) {
      if((!(bool)prjn->layer) || (prjn->from.ptr() != this)) {
        send_prjns.RemoveIdx(pi); // get rid of it!
      }
      else {
        for(int ji = pi-1; ji >= 0; ji--) {
          Projection* prjn2 = send_prjns[ji];
          if(prjn2 == prjn) {
            send_prjns.RemoveIdx(pi);
          }
        }
      }
    }
  }
}

void Layer::UpdateSendPrjnNames() {
  for(int pi=0; pi < send_prjns.size; pi++) {
    Projection* prjn = send_prjns[pi];
    prjn->UpdateName();
  }
}

void Layer::UpdateAllPrjns() {
  // update all my projections
  for(int pi = 0; pi < send_prjns.size; pi++) {
    Projection* prjn = send_prjns[pi];
    prjn->UpdateAfterEdit();
  }
  for(int pi = 0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    prjn->UpdateAfterEdit();
  }
}

void Layer::AddRelPos(taVector3i& rel_pos) {
  Layer_Group* lgp = dynamic_cast<Layer_Group*>(owner);
  if (lgp) {
    rel_pos += lgp->pos;
    lgp->AddRelPos(rel_pos);
  }
}

void Layer::AddRelPos2d(taVector2i& rel_pos) {
  Layer_Group* lgp = dynamic_cast<Layer_Group*>(owner);
  if (lgp) {
    rel_pos += lgp->pos2d;
    lgp->AddRelPos2d(rel_pos);
  }
}

void Layer::UpdateLayerGroupGeom() {
  Layer_Group* lgp = dynamic_cast<Layer_Group*>(owner);
  if (lgp) {
    lgp->UpdateLayerGroupGeom();
  }
}

void Layer::UpdateGeometry() {
  un_geom.SetGtEq(1);           // can't go < 1
  gp_geom.SetGtEq(1);
  un_geom.UpdateAfterEdit_NoGui();
  gp_geom.UpdateAfterEdit_NoGui();
  SetLayerFlagState(UN_GEOM_NOT_XY, un_geom.n_not_xy);
  if(unit_groups) {
    flat_geom.x = un_geom.x * gp_geom.x;
    flat_geom.y = un_geom.y * gp_geom.y;
    flat_geom.n = un_geom.n * gp_geom.n;
    if(flat_geom.n != flat_geom.x * flat_geom.y)
      flat_geom.n_not_xy = true;
    taVector2i eff_un_sz = un_geom + gp_spc;
    disp_geom = gp_geom * eff_un_sz;
    disp_geom -= gp_spc;        // no space at the end!
    n_ungps = gp_geom.n;
  }
  else {
    gp_geom.SetXYN(1,1,1);      // keep it updated, and non-zero
    flat_geom = un_geom;
    disp_geom = un_geom;
    n_ungps = 0;                // this is key test for no additional unit groups
  }
  scaled_disp_geom.x = (int)ceil((float)disp_geom.x * disp_scale);
  scaled_disp_geom.y = (int)ceil((float)disp_geom.y * disp_scale);

  // update state versions of key geometry
  if(own_net && n_units != flat_geom.n) { // invalidated if size changed
    own_net->ClearIntact();
  }
  n_units = flat_geom.n;
  un_geom_x = un_geom.x; un_geom_y = un_geom.y; un_geom_n = un_geom.n;
  gp_geom_x = gp_geom.x; gp_geom_y = gp_geom.y; gp_geom_n = gp_geom.n;
  flat_geom_x = flat_geom.x; flat_geom_y = flat_geom.y; flat_geom_n = flat_geom.n;
  gp_spc_x = gp_spc.x; gp_spc_y = gp_spc.y;
}

bool Layer::UpdatePosition() {
  bool lay_moved = false;
  if(pos_rel.IsRel()) {
    if(TestWarning((pos_rel.other == this), "RecomputeGeom",
                   "getting relative position from self -- not good! cannot be any loops in the relationship connectivity! I cut the connection")) {
      pos_rel.other = NULL;
    }
    else if(TestWarning((pos_rel.other->pos_rel.other == this), "RecomputeGeom",
                   "two layers have reciprocal relative positions -- not good! cannot be any loops in the relationship connectivity! other layer:", pos_rel.other->name,
                   "I cut our connection")) {
      pos_rel.other = NULL;
    }
    else {
      PosVector3i new_pos;
      PosVector2i new_pos2d;
      pos_rel.ComputePos3D(new_pos, this);
      pos_rel.ComputePos2D(new_pos2d, this);
      new_pos.UpdateAfterEdit();
      new_pos2d.UpdateAfterEdit();
      if(new_pos != pos_abs || new_pos2d != pos2d_abs) {
        lay_moved = true;
        pos_abs = new_pos;
        pos2d_abs = new_pos2d;
      }
    }
  }

  // always do these updates just for kicks..
  taVector3i rp = 0;
  AddRelPos(rp);
  taVector2i rp2 = 0;
  AddRelPos2d(rp2);
  // always update pos based on pos_abs
  pos = pos_abs - rp;         // subtract relative positions
  pos2d = pos2d_abs - rp2;
  if(lay_moved) {
    UpdateLayerGroupGeom();
  }
  return lay_moved;
}

void Layer::PositionUsRelativeToDropped(Layer* lay, LayerRelPos::RelPos rel) {
  pos_rel.other = lay;
  pos_rel.rel = rel;
  SigEmitUpdated();
}

void Layer::PositionDroppedRelativeToUs(Layer* lay, LayerRelPos::RelPos rel) {
  lay->pos_rel.other = this;
  lay->pos_rel.rel = rel;
  lay->SigEmitUpdated();
}

void Layer::ConnectUsFromDropped(Layer* lay) {
  ConnectFrom(lay);
}

void Layer::ConnectDroppedFromUs(Layer* lay) {
  lay->ConnectFrom(this);
}

void Layer::ConnectDroppedBidir(Layer* lay) {
  ConnectBidir(lay);
}


void Layer::SetRelPos(int x, int y, int z) {
  taVector3i ps(x,y,z);
  SetRelPos(ps);
}

void Layer::SetRelPos(taVector3i& ps) {
  pos_rel.SetAbsPos();
  taVector3i rp = 0;
  AddRelPos(rp);
  pos.x = ps.x; pos.y = ps.y; pos.z = ps.z; // avoid pos constraint
  pos_abs = ps + rp;
  UpdateLayerGroupGeom();
  SigEmitUpdated();
}

void Layer::SetAbsPos(int x, int y, int z) {
  taVector3i ps(x,y,z);
  SetAbsPos(ps);
}

void Layer::SetAbsPos(taVector3i& ps) {
  pos_rel.SetAbsPos();
  taVector3i rp = 0;
  AddRelPos(rp);
  pos_abs = ps;
  pos.x = ps.x - rp.x;  pos.y = ps.y - rp.y;  pos.z = ps.z - rp.z;
  UpdateLayerGroupGeom();
  SigEmitUpdated();
}

void Layer::SetRelPos2d(int x, int y) {
  taVector2i ps(x,y);
  SetRelPos2d(ps);
}

void Layer::SetRelPos2d(taVector2i& ps) {
  pos_rel.SetAbsPos();
  taVector2i rp = 0;
  AddRelPos2d(rp);
  pos2d.x = ps.x; pos2d.y = ps.y; // avoid pos constraint
  pos2d_abs = ps + rp;
  UpdateLayerGroupGeom();
  SigEmitUpdated();
}

void Layer::SetAbsPos2d(int x, int y) {
  taVector2i ps(x,y);
  SetAbsPos2d(ps);
}

void Layer::SetAbsPos2d(taVector2i& ps) {
  pos_rel.SetAbsPos();
  taVector2i rp = 0;
  AddRelPos2d(rp);
  pos2d_abs = ps;
  pos2d.x = ps.x - rp.x;  pos2d.y = ps.y - rp.y;
  UpdateLayerGroupGeom();
  SigEmitUpdated();
}

void Layer::MovePos(int x, int y, int z) {
  pos_rel.SetAbsPos();
  taVector3i nps = pos_abs;
  nps.x += x;
  if(nps.x < 0) nps.x = 0;
  nps.y += y;
  if(nps.y < 0) nps.y = 0;
  nps.z += z;
  if(nps.z < 0) nps.z = 0;
  SetAbsPos(nps);
}

void Layer::MovePos2d(int x, int y) {
  pos_rel.SetAbsPos();
  taVector2i nps = pos2d_abs;
  nps.x += x;
  if(nps.x < 0) nps.x = 0;
  nps.y += y;
  if(nps.y < 0) nps.y = 0;
  SetAbsPos2d(nps);
}

ProjectBase* Layer::project() {
  ProjectBase* rval = NULL;
  if (own_net)
    rval = GET_OWNER(own_net, ProjectBase);
  return rval;
}

void Layer::SetLayerUnitGeom(int x, int y, bool n_not_xy, int n) {
  un_geom.x = x; un_geom.y = y; un_geom.n_not_xy = n_not_xy; un_geom.n = n;
  UpdateAfterEdit();
}

void Layer::SetLayerUnitGpGeom(int x, int y, bool n_not_xy, int n) {
  unit_groups = true;
  gp_geom.x = x; gp_geom.y = y; gp_geom.n_not_xy = n_not_xy; gp_geom.n = n;
  UpdateAfterEdit();
}

bool Layer::SetLayerSpec(LayerSpec*) {
  return false;                 // no layer spec for layers!
}

void Layer::SetDefaultPos() {
  if(!own_net) return;
  if(pos_rel.other) return;    // already set
  if(!(pos_abs.x == 0 && pos_abs.y == 0 && pos_abs.z == 0))
    return; // already set
  int index = own_net->layers.FindLeafEl(this);
  if(index == 0) {              // first guy..
    taVector3i ps3 = 0;
    taVector2i ps2 = 0;
    SetAbsPos(ps3);
    SetAbsPos2d(ps2);
    return;
  }
  Layer* prev = own_net->layers.Leaf(index-1);
  PositionAbove(prev);
}

void Layer::PositionRightOf(Layer* lay, int space) {
  if(TestWarning((lay == this || lay->pos_rel.other == this), "PositionRightOf",
                 "two layers cannot have reciprocal relative positions -- not good!  cannot in general be any loops in the relationship connectivity!")) {
    return;
  }
  pos_rel.other = lay;
  pos_rel.rel = LayerRelPos::RIGHT_OF;
  pos_rel.space = space;
  lay->UpdatePosition();
}

void Layer::PositionBehind(Layer* lay, int space) {
  if(TestWarning((lay == this || lay->pos_rel.other == this), "PositionBehind",
                 "two layers cannot have reciprocal relative positions -- not good!  cannot in general be any loops in the relationship connectivity!")) {
    return;
  }
  pos_rel.other = lay;
  pos_rel.rel = LayerRelPos::BEHIND;
  pos_rel.space = space;
  lay->UpdatePosition();
}

void Layer::PositionAbove(Layer* lay, int space) {
  if(TestWarning((lay == this || lay->pos_rel.other == this), "PositionAbove",
                 "two layers cannot have reciprocal relative positions -- not good!  cannot in general be any loops in the relationship connectivity!")) {
    return;
  }
  pos_rel.other = lay;
  pos_rel.rel = LayerRelPos::ABOVE;
  pos_rel.space = space;
  lay->UpdatePosition();
}

void Layer::SetNUnits(int n_un) {
  if(un_geom.n == n_un || n_un <= 0) return; // only if diff or sensible
  un_geom.FitN(n_un);
  UpdateAfterEdit();
}

void Layer::SetNUnitGroups(int n_groups) {
  if(un_geom.n == n_groups || n_groups <= 0) return; // only if diff or sensible
  unit_groups = true;           // presumably this is the point..
  gp_geom.FitN(n_groups);
  UpdateAfterEdit();
}

void Layer::CheckSpecs() {
  // NOTE: if an algo has a layerspec, definitely need to check its spec here!
  if(!own_net) return;
  
  unit_spec.CheckSpec(own_net->UnitStateType());

  for(int pi=0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    prjn->CheckSpecs();
  }
}

void Layer::UpdatePrjnIdxs() {
  // make sure we always have up-to-date count of active prjns..
  n_recv_prjns = 0;
  n_send_prjns = 0;
  for(int pi=0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    prjn->UpdateLesioned();
    if(prjn->MainIsActive()) {
      prjn->recv_idx = n_recv_prjns++;
    }
    else {
      prjn->recv_idx = -1;
      prjn->prjn_idx = -1; // safe
    }
  }
  for(int pi=0; pi < send_prjns.size; pi++) {
    Projection* prjn = send_prjns[pi];
    prjn->UpdateLesioned();
    if(prjn->MainIsActive()) {
      prjn->send_idx = n_send_prjns++;
    }
    else {
      prjn->send_idx = -1;
      prjn->prjn_idx = -1; // safe
    }
  }
}

void Layer::CheckThisConfig_impl(bool quiet, bool& rval) {
  // note: network also called our checks
  // slightly non-standard, since we bail on first error

  if(lesioned()) return;

  CheckSpecs();

  UnitSpec* us = GetMainUnitSpec();
  if(us) {
    bool chk = us->CheckConfig_Unit(this, quiet);
    if(!chk) rval = false;
  }
  
  inherited::CheckThisConfig_impl(quiet, rval);
}

void Layer::CheckChildConfig_impl(bool quiet, bool& rval) {
  if(lesioned()) return;

  inherited::CheckChildConfig_impl(quiet, rval);
  // layerspec should take over this function in layers that have them!
  projections.CheckConfig(quiet, rval);
}

void Layer::DisConnect() {
  StructUpdate(true);
  DisConnect_impl();
  StructUpdate(false);
}

void Layer::DisConnect_impl() {
  for(int pi=send_prjns.size-1; pi >= 0; pi--) {
    Projection* prjn = send_prjns[pi];
    if(prjn == NULL) continue;
    if(!(bool)prjn->layer) {
      send_prjns.RemoveIdx(pi);
      continue;
    }
    prjn->layer->projections.RemoveEl(prjn);
  }
  send_prjns.Reset();
  projections.Reset();
}

void Layer::SyncLayerState() {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  own_net->SyncLayerState_Layer(this);
}

void Layer::SetLayUnitExtFlags(int flg) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;
  
  SetExtFlag(flg);
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    u->SetExtFlag((UnitState_cpp::ExtFlags)flg);
  }
}

void Layer::ApplyInputData(taMatrix* data, ExtFlags ext_flags,
           Random* ran, const PosVector2i* offset, bool na_by_range)
{
  // note: when use LayerWriters, we typically always just get a single frame of
  // the exact dimensions, and so ignore 'frame'
  if (!data || lesioned() || n_units_built == 0) return;
  // check correct geom of data
  if(TestError((data->dims() != 2) && (data->dims() != 4), "ApplyInputData",
               "data->dims must be 2 (2-d) or 4 (4-d)")) {
    return;
  }
  taVector2i offs(0,0);
  if(offset) offs = *offset;

  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;
  
  // apply flags if we are the controller (zero offset)
  if((offs.x == 0) && (offs.y == 0)) {
    ApplyLayerFlags(net, ext_flags);
  }
  if(data->dims() == 1) {
    ApplyInputData_1d(net, data, ext_flags, ran, na_by_range);
  }
  else if(data->dims() == 2) {
    ApplyInputData_2d(net, data, ext_flags, ran, offs, na_by_range);
  }
  else {
    if(unit_groups) {
      ApplyInputData_Gp4d(net, data, ext_flags, ran, na_by_range); // note: no offsets -- layerwriter does check
    }
    else {
      ApplyInputData_Flat4d(net, data, ext_flags, ran, offs, na_by_range);
    }
  }
  SyncLayerState();
}

void Layer::ApplyInputData_1d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                              Random* ran, bool na_by_range) {
  // todo: in the future, put random on unitspec and move this to State
  int max_x = MIN(data->dim(0), n_units_built);
  bool do_rand = (ran && (ran->type != Random::NONE));
  UNIT_SPEC_CPP* us = GetUnitSpec(net);
  for(int d_x = 0; d_x < max_x; d_x++) {
    UNIT_STATE* u = GetUnitStateSafe(net, d_x);
    if(!u || u->lesioned()) continue;
    float val = data->SafeElAsVar(d_x).toFloat();
    if(do_rand) {
      val += ran->Gen();
    }
    us->ApplyInputData(u, net, val, (UnitState_cpp::ExtFlags)ext_flags, na_by_range);
  }
}

void Layer::ApplyInputData_2d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                              Random* ran, const taVector2i& offs, bool na_by_range) {
  int max_x = MIN(data->dim(0)-offs.x, flat_geom.x);
  int max_y = MIN(data->dim(1)-offs.y, flat_geom.y);
  bool do_rand = (ran && (ran->type != Random::NONE));
  UNIT_SPEC_CPP* us = GetUnitSpec(net);
  for(int d_y = 0; d_y < max_y; d_y++) {
    int u_y = offs.y + d_y;
    for(int d_x = 0; d_x < max_x; d_x++) {
      int u_x = offs.x + d_x;
      UNIT_STATE* u = GetUnitStateFlatXY(net, d_x, d_y);
      if(!u || u->lesioned()) continue;
      float val = data->SafeElAsVar(d_x, d_y).toFloat();
      if(do_rand) {
        val += ran->Gen();
      }
      us->ApplyInputData(u, net, val, (UnitState_cpp::ExtFlags)ext_flags, na_by_range);
    }
  }
}

void Layer::ApplyInputData_Flat4d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                  Random* ran, const taVector2i& offs, bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  int max_x = MIN(data->dim(0), un_geom.x);
  int max_y = MIN(data->dim(1), un_geom.y);
  int max_gx = MIN(data->dim(2), gp_geom.x);
  int max_gy = MIN(data->dim(3), gp_geom.y);
  bool do_rand = (ran && (ran->type != Random::NONE));
  UNIT_SPEC_CPP* us = GetUnitSpec(net);
  for(int dg_y = 0; dg_y < max_gy; dg_y++) {
    for(int dg_x = 0; dg_x < max_gx; dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int u_y = offs.y + dg_y * data->dim(1) + d_y; // multiply out data indicies
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int u_x = offs.x + dg_x * data->dim(0) + d_x; // multiply out data indicies
          UNIT_STATE* u = GetUnitStateFlatXY(net, d_x, d_y);
          if(!u || u->lesioned()) continue;
          float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
          if(do_rand) {
            val += ran->Gen();
          }
          us->ApplyInputData(u, net, val, (UnitState_cpp::ExtFlags)ext_flags, na_by_range);
        }
      }
    }
  }
}

void Layer::ApplyInputData_Gp4d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags, Random* ran,
                                bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  int max_x = MIN(data->dim(0), un_geom.x);
  int max_y = MIN(data->dim(1), un_geom.y);
  int max_gx = MIN(data->dim(2), gp_geom.x);
  int max_gy = MIN(data->dim(3), gp_geom.y);
  bool do_rand = (ran && (ran->type != Random::NONE));
  UNIT_SPEC_CPP* us = GetUnitSpec(net);
  for(int dg_y = 0; dg_y < max_gy; dg_y++) {
    for(int dg_x = 0; dg_x < max_gx; dg_x++) {

      for(int d_y = 0; d_y < max_y; d_y++) {
        for(int d_x = 0; d_x < max_x; d_x++) {
          UNIT_STATE* u = GetUnitStateGpUnXY(net, dg_x, dg_y, d_x, d_y);
          if(!u || u->lesioned()) continue;
          float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
          if(do_rand) {
            val += ran->Gen();
          }
          us->ApplyInputData(u, net, val, (UnitState_cpp::ExtFlags)ext_flags, na_by_range);
        }
      }
    }
  }
}

void Layer::ApplyLayerFlags(NETWORK_STATE* net, ExtFlags act_ext_flags) {
  SetExtFlag(act_ext_flags);
}

////////////////////////////////////////////////////////////////////////////////
//  Below are the primary computational interface to the Network Objects
//  for performing algorithm-specific activation and learning
//  All functions at network level operate directly on the units via threads, with
//  optional call through to the layers for any layer-level subsequent processing


void Layer::Init_Weights(bool recv_cons) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  LAYER_STATE* lay = GetLayerState(net);
  lay->Init_Weights(net, recv_cons);
}

void Layer::Copy_Weights(Layer* src, bool recv_cons) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  LAYER_STATE* lay = GetLayerState(net);
  LAYER_STATE* src_lay = src->GetLayerState(net);
  lay->Copy_Weights(net, src_lay, recv_cons);
}



////////////////////////////////////////////////////////////////////////////////
//      The following are misc functionality not required for primary computing


void Layer::PropagateInputDistance() {
  int new_dist = dist.fm_input + 1;
  for(int pi=0; pi < send_prjns.size; pi++) {
    Projection* prjn = send_prjns[pi];
    if(prjn->MainNotActive()) continue;
    if(prjn->layer->dist.fm_input >= 0) { // already set
      if(new_dist < prjn->layer->dist.fm_input) { // but we're closer
        prjn->layer->dist.fm_input = new_dist;
        prjn->layer->PropagateInputDistance(); // note: this could lead back to us, but big deal.
        // the < sign prevents loops from continuing indefinitely.
      }
    }
    else { // not set yet
      prjn->layer->dist.fm_input = new_dist;
      prjn->layer->PropagateInputDistance();
    }
  }
}

void Layer::PropagateOutputDistance() {
  int new_dist = dist.fm_output + 1;
  for(int pi=0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    if(prjn->MainNotActive()) continue;
    if(prjn->from->dist.fm_output >= 0) { // already set
      if(new_dist < prjn->from->dist.fm_output) { // but we're closer
        prjn->from->dist.fm_output = new_dist;
        prjn->from->PropagateOutputDistance(); // note: this could lead back to us, but big deal.
        // the < sign prevents loops from continuing indefinitely.
      }
    }
    else { // not set yet
      prjn->from->dist.fm_output = new_dist;
      prjn->from->PropagateOutputDistance();
    }
  }
}

void Layer::Compute_PrjnDirections() {
  for(int pi=0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    if(prjn->dir_fixed && prjn->direction != Projection::DIR_UNKNOWN)
      continue;
    if(prjn->MainNotActive()) {
      prjn->direction = Projection::DIR_UNKNOWN;
      continue;
    }
    // use the smallest value first..
    if(prjn->from->dist.fm_input <= prjn->from->dist.fm_output) {
      if(prjn->from->dist.fm_input < dist.fm_input) {
        prjn->direction = Projection::FM_INPUT;
      }
      else if(prjn->from->dist.fm_output < dist.fm_output) {
        prjn->direction = Projection::FM_OUTPUT;
      }
      else {
        prjn->direction = Projection::LATERAL;
      }
    }
    else {
      if(prjn->from->dist.fm_output < dist.fm_output) {
        prjn->direction = Projection::FM_OUTPUT;
      }
      else if(prjn->from->dist.fm_input < dist.fm_input) {
        prjn->direction = Projection::FM_INPUT;
      }
      else {
        prjn->direction = Projection::LATERAL;
      }
    }
  }
}

String Layer::GetUnitNameIdx(int flat_un_idx) const {
  if(!HasLayerFlag(SAVE_UNIT_NAMES)) return _nilString;
  if(unit_groups) {
    if(gp_unit_names_4d) {
      int gp_x, gp_y, un_x, un_y;
      GetGpUnXYFmIdx(flat_un_idx, gp_x, gp_y, un_x, un_y);
      return unit_names.SafeEl(un_x, un_y, gp_x, gp_y);
    }
    else {
      int un_x, un_y;
      GetUnXYFmIdx(flat_un_idx, un_x, un_y);
      return unit_names.SafeEl(un_x, un_y);
    }
  }
  else {
    int un_x, un_y;
    GetUnFlatXYFmIdx(flat_un_idx, un_x, un_y);
    return unit_names.SafeEl(un_x, un_y);
  }
}

String Layer::GetUnitName(UnitState_cpp* un) const {
  return GetUnitNameIdx(un->lay_un_idx);
}

void Layer::SetUnitNameIdx(int flat_un_idx, const String& nm) {
  if(!HasLayerFlag(SAVE_UNIT_NAMES)) {
    SetUnitNames(true);
  }
  if(unit_groups) {
    if(gp_unit_names_4d) {
      int gp_x, gp_y, un_x, un_y;
      GetGpUnXYFmIdx(flat_un_idx, gp_x, gp_y, un_x, un_y);
      unit_names.SetFmVar((Variant)nm, gp_x, gp_y, un_x, un_y);
    }
    else {
      int un_x, un_y;
      GetUnXYFmIdx(flat_un_idx, un_x, un_y);
      unit_names.SetFmVar((Variant)nm, un_x, un_y);
    }
  }
  else {
    int un_x, un_y;
    GetUnFlatXYFmIdx(flat_un_idx, un_x, un_y);
    return unit_names.SetFmVar((Variant)nm, un_x, un_y);
  }
}

void Layer::SetUnitName(UnitState_cpp* un, const String& nm) {
  SetUnitNameIdx(un->lay_un_idx, nm);
}

bool Layer::SetUnitNames(bool force_use_unit_names) {
  if(!force_use_unit_names && !HasLayerFlag(SAVE_UNIT_NAMES)) return false;
  SetLayerFlag(SAVE_UNIT_NAMES);
  // first enforce geom, then do it.
  if(unit_groups) {
    if(gp_unit_names_4d) {
      unit_names.SetGeom(4, un_geom.x, un_geom.y, gp_geom.x, gp_geom.y);
    }
    else {
      unit_names.SetGeom(2, un_geom.x, un_geom.y);
    }
  }
  else {
    unit_names.SetGeom(2, un_geom.x, un_geom.y);
  }
  return true;
}

bool Layer::SetUnitNamesFromDataCol(const DataCol* unit_names_col, int max_un_chars) {
  if(TestError(!unit_names_col, "SetUnitNamesFromDataCol", "null unit_names_col"))
    return false;

  SetLayerFlag(SAVE_UNIT_NAMES);

  const MatrixGeom& cg = unit_names_col->cell_geom;
  taMatrix* nmat = (const_cast<DataCol*>(unit_names_col))->GetValAsMatrix(-1);
  if(!nmat) return false;
  taBase::Ref(nmat);

  if(unit_groups && cg.dims() == 4) { // check if all but first group is empty
    bool hugp_empty = true;
    int gx, gy, ux, uy;
    for(gy = 0; gy<cg.dim(3); gy++) {
      for(gx = 0; gx<cg.dim(2); gx++) {
        if(gx == 0 && gy == 0) continue; // skip 1st gp
        for(uy = 0; uy<cg.dim(1); uy++) {
          for(ux = 0; ux<cg.dim(0); ux++) {
            if(nmat->SafeElAsStr(ux,uy,gx,gy).nonempty()) {
              hugp_empty = false;
              break;
            }
          }
        }
      }
    }
    if(hugp_empty) {
      unit_names.SetGeom(2, cg.dim(0), cg.dim(1)); // just set for 1st gp
    }
    else {
      unit_names.SetGeomN(cg); // get our geom
    }
  }
  else {
    unit_names.SetGeomN(cg); // get our geom
  }
  for(int i=0;i<nmat->size && i<unit_names.size;i++) {
    String cnm = nmat->SafeElAsStr_Flat(i);
    unit_names.SetFmStr_Flat(cnm.elidedTo(max_un_chars), i);
  }
  taBase::unRefDone(nmat);
  SetUnitNames();               // actually set from these names
  return true;
}

void Layer::GetLocalistName() {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    
    const int rsz = u->NRecvConGps(net);
    for(int g = 0; g < rsz; g++) {
      ConState_cpp* cg = u->RecvConState(net, g);
      if(cg->NotActive()) continue;
      if(cg->size != 1) continue; // only 1-to-1
      UnitState_cpp* su = cg->UnState(0,net);
      LayerState_cpp* sls = cg->GetSendLayer(net);
      Layer* slay = own_net->LayerFromState(sls);
      String nm = slay->GetUnitName(su);
      if(nm.nonempty()) {
        SetUnitName(u, nm);
        break;                    // done!
      }
    }
  }
}

int Layer::FindUnitNamedIdx(const String& nm, bool err) const {
  int idx = unit_names.FindVal_Flat(nm);
  TestError(err && idx < 0, "FindUnitNamedIdx",
            "name:", nm, "not found in unit_names");
  return idx;
}

UnitState_cpp* Layer::FindUnitNamed(const String& nm, bool err) const {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return NULL;

  int idx = FindUnitNamedIdx(nm, err);
  if(idx >= 0) {
    return GetUnitState(net, idx);
  }
  return NULL;
}

void Layer::TransformWeights(const SimpleMathSpec& trans) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;
  
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    u->TransformWeights(net, trans);
  }
}

void Layer::AddNoiseToWeights(const Random& noise_spec) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;
  
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    u->AddNoiseToWeights(net, noise_spec);
  }
}

int Layer::PruneCons(const SimpleMathSpec& pre_proc, Relation::Relations rel, float cmp_val) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return 0;
  
  int rval = 0;
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    rval += u->PruneCons(net, pre_proc, rel, cmp_val);
  }
  return rval;
}

int Layer::ProbAddCons(float p_add_con, float init_wt) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return 0;
  
  int rval = 0;
  for(int pi=0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    if(prjn->MainNotActive()) continue;
    rval += prjn->ProbAddCons(p_add_con, init_wt);
  }
  return rval;
}

int Layer::LesionCons(float p_lesion, bool permute) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return 0;
  
  int rval = 0;
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    rval += u->LesionCons(net, p_lesion, permute);
  }
  return rval;
}

int Layer::LesionUnits(float p_lesion, bool permute) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return 0;

  if(p_lesion >= 1.0f) {
    for(int j=0; j < n_units_built; j++) {
      UnitState_cpp* u = GetUnitState(net, j);
      u->Lesion(net);
    }
    return n_units_built;
  }
  
  int rval = 0;
  StructUpdate(true);
  UnLesionUnits();              // always start unlesioned
  if(permute) {
    rval = (int) (p_lesion * (float)n_units_built);
    if(rval == 0) return 0;
    int_Array ary;
    int j;
    for(j=0; j<n_units_built; j++)
      ary.Add(j);
    ary.Permute();
    ary.size = rval;
    ary.Sort();
    for(j=ary.size-1; j>=0; j--) {
      UnitState_cpp* u = GetUnitState(net, ary.FastEl(j));
      u->Lesion(net);             // just sets a flag
    }
  }
  else {
    int j;
    for(j=n_units_built-1; j>=0; j--) {
      if(Random::ZeroOne() <= p_lesion) {
        UnitState_cpp* u = GetUnitState(net, j);
        u->Lesion(net);
        rval++;
      }
    }
  }
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
  return rval;
}

void Layer::UnLesionUnits() {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  StructUpdate(true);
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    u->UnLesion(net);
  }
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
}

void Layer::UpdtAfterNetModIfNecc() {
  if(!own_net || own_net->InStructUpdate()) return;
  own_net->UpdtAfterNetMod();
}

bool Layer::UnitSpecUpdated() {
  UnitSpec* sp = unit_spec.SPtr();
  if(!sp) return false;
  if(sp == m_prv_unit_spec) return false;
  m_prv_unit_spec = sp;         // don't redo it
  if(own_net)
    own_net->ClearIntact();
  return true;
}

bool Layer::ApplySpecToMe(BaseSpec* spec) {
  if(spec == NULL) return false;
  if(spec->InheritsFrom(&TA_UnitSpec)) {
    SetUnitSpec((UnitSpec*)spec);
    return true;
  }
  else if(spec->InheritsFrom(&TA_LayerSpec)) {
    SetLayerSpec((LayerSpec*)spec);
    return true;
  }
  return false;
}

bool Layer::SetUnitSpec(UnitSpec* sp) {
  if(!sp)       return false;
  if(!unit_spec.SetSpec(sp)) return false;
  if(!unit_spec.CheckObjTypeForSpec(own_net->UnitStateType())) {
    taMisc::Error("The unit spec you are setting is not compatible with the unit type for units in this network -- which is determined by the network type");
  }
  return UnitSpecUpdated();
}

bool Layer::EditState() {
  if(!taMisc::gui_active) return false;
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;
  
  if (taiEdit *ie = own_net->LayerStateType()->ie) {
    LayerState_cpp* lay = GetLayerState(net);
    if(!lay) return false;
    return ie->Edit((void*)lay, false);
  }
  return false;
}

bool Layer::EditLayUnGpState() {
  if(!taMisc::gui_active) return false;
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;

  if (taiEdit *ie = own_net->UnGpStateType()->ie) {
    UnGpState_cpp* ugp = GetLayUnGpState(net);
    if(!ugp) return false;
    return ie->Edit((void*)ugp, false);
  }
  return false;
}

bool Layer::EditUnGpState(int un_gp_no) {
  if(!taMisc::gui_active) return false;
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;
  
  if (taiEdit *ie = own_net->UnGpStateType()->ie) {
    UnGpState_cpp* ugp = GetUnGpState(net, un_gp_no);
    if(!ugp) return false;
    return ie->Edit((void*)ugp, false);
  }
  return false;
}

bool Layer::EditUnitState(int unit_no) {
  if(!taMisc::gui_active) return false;
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;
  
  if (taiEdit *ie = own_net->UnitStateType()->ie) {
    UnitState_cpp* un = GetUnitStateSafe(net, unit_no);
    if(!un) return false;
    return ie->Edit((void*)un, false);
  }
  return false;
}

bool Layer::EditConState(int unit_no, int prjn_idx, bool recv) {
  if(!taMisc::gui_active) return false;
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;
  
  if (taiEdit *ie = own_net->ConStateType()->ie) {
    UnitState_cpp* un = GetUnitStateSafe(net, unit_no);
    if(!un) return false;
    ConState_cpp* cg = NULL;
    if(recv) {
      cg = un->RecvConStateSafe(net, prjn_idx);
    }
    else {
      cg = un->SendConStateSafe(net, prjn_idx);
    }
    if(!cg) return false;
    return ie->Edit((void*)cg, false);
  }
  return false;
}

void Layer::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

bool Layer::Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;
  
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    if(!u->Snapshot(own_net, variable, math_op, arg_is_snap)) return false;
  }
  return true;
}

Unit* Layer::MostActiveUnit(int& idx) const {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return NULL;
  
  idx = -1;
  if(n_units_built == 0) return NULL;
  UnitState_cpp* max_un = GetUnitState(net, 0);
  float max_act = max_un->act;
  for(int i=1;i<n_units_built;i++) {
    UnitState_cpp* un = GetUnitState(net, i);
    if(un->act > max_act) {
      max_un = un;
      idx = i;
      max_act = max_un->act;
    }
  }
  return (Unit*)max_un;
}

int Layer::ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp, bool prompt) {
  int nchg = 0;
  if(GetMainUnitSpec() != old_sp) return 0;
  String act_nm = "UnitSpec: " + old_sp->name + " with: " + new_sp->name
    + " in layer: " + name;
  if(prompt) {
    BrowserSelectMe();
    int ok = taMisc::Choice("Replace " + act_nm + "?", "Ok", "Skip");
    if(ok != 0) return 0;
  }
  unit_spec.SetSpec(new_sp);
  nchg++;
  UnitSpecUpdated();
  taMisc::Info("Replaced", act_nm);
  return nchg;
}

int Layer::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp, bool prompt) {
  int nchg = 0;
  for(int pi=0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    nchg += prjn->ReplaceConSpec(old_sp, new_sp, prompt);
  }
  return nchg;
}

int Layer::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp, bool prompt) {
  int nchg = 0;
  for(int pi=0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    nchg += prjn->ReplacePrjnSpec(old_sp, new_sp, prompt);
  }
  return nchg;
}

int Layer::ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp, bool prompt) {
  if(GetMainLayerSpec() != old_sp) return 0;
  String act_nm = "LayerSpec: " + old_sp->name + " with: " + new_sp->name
    + " in layer: " + name;
  if(prompt) {
    BrowserSelectMe();
    int ok = taMisc::Choice("Replace " + act_nm + "?", "Ok", "Skip");
    if(ok != 0) return 0;
  }
  SetLayerSpec(new_sp);
  taMisc::Info("Replaced", act_nm);
  return 1;
}

DataTable* Layer::WeightsToTable(DataTable* dt, Layer* send_lay) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return NULL;
  
  bool new_table = false;
  if (!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable(name + "_Weights", true);
    new_table = true;
  }
  if(send_lay == NULL) return NULL;
  bool gotone = false;
  for(int pi=0; pi < projections.size; pi++) {
    Projection* prjn = projections[pi];
    if(prjn->MainNotActive()) continue;
    if(prjn->from.ptr() != send_lay) continue;
    prjn->WeightsToTable(dt);
    gotone = true;
  }
  TestError(!gotone, "WeightsToTable", "No sending projection from:", send_lay->name);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* Layer::VarToTable(DataTable* dt, const String& variable) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return NULL;
  
  bool new_table = false;
  if (!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
    new_table = true;
  }

  if (!own_net || !own_net->IsBuiltIntact()) return NULL;

  NetMonitor nm;
  nm.OwnTempObj();
  nm.SetDataNetwork(dt, own_net);
  nm.AddLayer(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* Layer::ConVarsToTable(DataTable* dt, const String& var1, const String& var2,
                           const String& var3, const String& var4, const String& var5,
                           const String& var6, const String& var7, const String& var8,
                           const String& var9, const String& var10, const String& var11,
                           const String& var12, const String& var13, const String& var14,
                           PrjnState_cpp* prjn) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return NULL;

  bool new_table = false;
  if(!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    u->ConVarsToTable(net, dt, var1, var2, var3, var4, var5, var6, var7, var8,
                      var9, var10, var11, var12, var13, var14,  prjn);
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* Layer::PrjnsToTable(DataTable* dt, bool sending) {
  bool new_table = false;
  if(!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable("LayerPrjns_" + name, true);
    new_table = true;
  }
  dt->StructUpdate(true);
  int idx;
  dt->RemoveAllRows();
  DataCol* col;
  String colnm = "PrjnFrom";
  if(sending) colnm = "PrjnTo";
  col = dt->FindMakeColName(colnm, idx, VT_STRING);
  col->desc = "receiving projection -- name of sending layer that this layer receives from";
  col = dt->FindMakeColName("PrjnSpec", idx, VT_STRING);
  col->desc = "name of projection spec for this projection";
  col = dt->FindMakeColName("ConSpec", idx, VT_STRING);
  col->desc = "name of connection spec for this projection";

  if(sending) {
    for(int pi=0; pi < send_prjns.size; pi++) {
      Projection* prjn = send_prjns[pi];
      dt->AddBlankRow();
      dt->SetVal(prjn->layer->name, colnm, -1);
      ProjectionSpec* ps = prjn->GetMainPrjnSpec();
      if(ps)
        dt->SetVal(ps->name, "PrjnSpec", -1);
      else
        dt->SetVal("NULL", "PrjnSpec", -1);
      ConSpec* cs = prjn->GetMainConSpec();
      if(cs)
        dt->SetVal(cs->name, "ConSpec", -1);
      else
        dt->SetVal("NULL", "ConSpec", -1);
    }
  }
  else {
    for(int pi=0; pi < projections.size; pi++) {
      Projection* prjn = projections[pi];
      dt->AddBlankRow();
      dt->SetVal(prjn->from->name, colnm, -1);
      ProjectionSpec* ps = prjn->GetMainPrjnSpec();
      if(ps)
        dt->SetVal(ps->name, "PrjnSpec", -1);
      else
        dt->SetVal("NULL", "PrjnSpec", -1);
      ConSpec* cs = prjn->GetMainConSpec();
      if(cs)
        dt->SetVal(cs->name, "ConSpec", -1);
      else
        dt->SetVal("NULL", "ConSpec", -1);
    }
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

bool Layer::VarToVarCopy(const String& dest_var, const String& src_var) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;
  
  TypeDef* unit_typ = own_net->UnitStateType();
  
  MemberDef* dest_md = unit_typ->members.FindName(dest_var);
  if(TestWarning(!dest_md || !dest_md->type->InheritsFrom(TA_float), "VarToVarCopy",
                 "Variable:", dest_var, "not found or not a float on units of type:",
                 unit_typ->name)) {
    return false;
  }
  MemberDef* src_md = unit_typ->members.FindName(src_var);
  if(TestWarning(!src_md || !src_md->type->InheritsFrom(TA_float), "VarToVarCopy",
                 "Variable:", src_var, "not found or not a float on units of type:",
                 unit_typ->name)) {
    return false;
  }
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    *((float*)dest_md->GetOff((void*)u)) = *((float*)src_md->GetOff((void*)u));
  }
  return true;
}

bool Layer::VarToVal(const String& dest_var, float val) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;
  
  TypeDef* unit_typ = own_net->UnitStateType();

  MemberDef* dest_md = unit_typ->members.FindName(dest_var);
  if(TestWarning(!dest_md || !dest_md->type->InheritsFrom(TA_float), "VarToVarCopy",
                 "Variable:", dest_var, "not found or not a float on units of type:",
                 unit_typ->name)) {
    return false;
  }
  for(int ui=0; ui < n_units_built; ui++) {
    UnitState_cpp* u = GetUnitState(net, ui);
    if(u->lesioned()) continue;
    *((float*)dest_md->GetOff((void*)u)) = val;
  }
  return true;
}

UnitState_cpp* Layer::UnitAtDispCoord(int x, int y) const {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return NULL;

  if(unit_groups) {
    int gpsz_x = un_geom.x + gp_spc.x;
    int gpsz_y = un_geom.y + gp_spc.y;
    int gp_x = x / gpsz_x;
    int gp_y = y / gpsz_y;
    int gpst_x = gp_x * gpsz_x;
    int gpst_y = gp_y * gpsz_y;
    int un_x = x - gpst_x;
    int un_y = y - gpst_y;
    return GetUnitStateGpUnXY(net, gp_x, gp_y, un_x, un_y);
  }
  else {
    return GetUnitStateFlatXY(net, x, y);
  }
}

bool Layer::ChangeMyType(TypeDef* new_typ) {
  if(own_net) {
    own_net->ClearIntact();
  }
  return inherited::ChangeMyType(new_typ);
}


void Layer::SaveWeights(const String& fname) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  LayerState_cpp* lst = GetLayerState(net);
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm) {
    net->LayerSaveWeights_strm(*flr->ostrm, lst);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

bool Layer::LoadWeights(const String& fname, bool quiet) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;

  LayerState_cpp* lst = GetLayerState(net);
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  bool rval = false;
  if(flr->istrm) {
    rval = net->LayerLoadWeights_strm(*flr->istrm, lst, NetworkState_cpp::TEXT, quiet);
  }
  else {
    TestError(true, "LoadWeights", "aborted due to inability to load weights file");
    // the above should be unnecessary but we're not getting the error sometimes..
  }
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

#ifdef DMEM_COMPILE

void Layer::DMem_InitAggs() {
  // important: cannot check for valid as this is called during building!
  // NetworkState_cpp* net = GetValidNetState();
  if(!own_net || !own_net->net_state) return;
  NetworkState_cpp* net = own_net->net_state;
  LayerState_cpp* lst = GetLayerState(net);
  if(!lst) return;
  dmem_agg_sum.agg_op = MPI_SUM;
  dmem_agg_sum.ScanMembers(own_net->LayerStateType(), (void*)lst);
  dmem_agg_sum.CompileVars();
}

void Layer::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}

#endif
