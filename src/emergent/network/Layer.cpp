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

#include <tabMisc>
#include <taMisc>

TA_BASEFUNS_CTORS_LITE_DEFN(LayerDistances);
TA_BASEFUNS_CTORS_DEFN(LayerRelPos);
TA_BASEFUNS_CTORS_DEFN(Layer);

using namespace std;

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

bool LayerRelPos::ComputePos2D_impl(taVector2i& pos, Layer* lay, const taVector2i& oth_pos) {
  if(!other)
    return false;
  switch(rel) {
  case ABS_POS: {
    return false;
  }
  case RIGHT_OF: {
    pos.x = oth_pos.x + other->scaled_disp_geom.x + space;
    break;
  }
  case LEFT_OF: {
    pos.x = oth_pos.x - lay->scaled_disp_geom.x - space;
    break;
  }
  case BEHIND: {
    pos.y = oth_pos.y + other->scaled_disp_geom.y + space;
    break;
  }
  case FRONT_OF: {
    pos.y = oth_pos.y - lay->scaled_disp_geom.y - space;
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
  // desc = ??
  own_net = NULL;
  flags = LF_NONE;
  layer_type = HIDDEN;
  // pos = ??
  disp_scale = 1.0f;
  // un_geom = ??
  unit_groups = false;
  virt_groups = true;
  // gp_geom = ??
  gp_spc.x = 1;
  gp_spc.y = 1;
  n_recv_prjns = 0;
  n_send_prjns = 0;
  projections.SetBaseType(&TA_Projection);
  send_prjns.send_prjns = true;
  units.SetBaseType(&TA_Unit);
  // unit_spec = ??
  ext_flag = UnitVars::NO_EXTERNAL;
  // flat_geom = ??
  disp_geom = un_geom;
  scaled_disp_geom = disp_geom;
  // dist = ??
  // output_name = ??
  // gp_output_names = ??
  m_prv_unit_spec = NULL;
  m_prv_layer_flags = LF_NONE;

  sse = 0.0f;
  cnt_err = 0.0f;
  cur_cnt_err = 0.0f;
  pct_err = 0.0f;
  pct_cor = 0.0f;

  // prerr = ??
  icon_value = 0.0f;
  units_flat_idx = 0;
  units_lesioned = false;
  gp_unit_names_4d = false;
  // unit_names = ??
  brain_area = "";
  voxel_fill_pct = 1.0f;
  active_lay_idx = -1;
  
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
  
  taBase::Own(units, this);
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
  units.pos.z = 0;
  unit_spec.SetDefaultSpec(this);

#ifdef DMEM_COMPILE
  taBase::Own(dmem_agg_sum, this);
  DMem_InitAggs();
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
  
  units.CutLinks();
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
  virt_groups = cp.virt_groups;
  gp_geom = cp.gp_geom;
  gp_spc = cp.gp_spc;
  flat_geom = cp.flat_geom;
  disp_geom = cp.disp_geom;
  scaled_disp_geom = cp.scaled_disp_geom;
  n_recv_prjns = cp.n_recv_prjns;
  n_send_prjns = cp.n_send_prjns;
  projections = cp.projections;
  units = cp.units;
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
  UpdateUnitSpecs((bool)taMisc::is_loading); // force if loading
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

    if(lesioned() && !(m_prv_layer_flags & LESIONED)) {
      // clear activity if we're lesioned
      if(own_net)
        Init_Acts(own_net);
    }

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
  m_prv_layer_flags = flags;
}

void Layer::Lesion() {
  if(lesioned()) return;
  StructUpdate(true);
  SetLayerFlag(LESIONED);
  FOREACH_ELEM_IN_GROUP(Unit, u, units) { // unit-level needs lesion flag too
    if(u->lesioned()) continue;
    u->Lesion();
  }
  if(own_net)
    Init_Acts(own_net);
  m_prv_layer_flags = flags;
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
  StructUpdate(true);
  ClearLayerFlag(LESIONED);
  UnLesionUnits();              // all our units were lesioned when parent was
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
  Network* net = GET_MY_OWNER(Network);
  if (!net) return;
  Projection* prjn = net->FindMakePrjn(this, from_lay);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
}

void Layer::ConnectBidir(Layer* from_lay) {
  Network* net = GET_MY_OWNER(Network);
  if (!net) return;
  Projection* prjn = net->FindMakePrjn(this, from_lay);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
  prjn = net->FindMakePrjn(from_lay, this);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
}

void Layer::ConnectSelf() {
  Network* net = GET_MY_OWNER(Network);
  if (!net) return;
  Projection* prjn = net->FindMakeSelfPrjn(this);
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

void Layer::RecvConsPreAlloc(int alloc_no, Projection* prjn) {
  FOREACH_ELEM_IN_GROUP(Unit, ru, units) {
    ru->RecvConsPreAlloc(alloc_no, prjn);
  }
}

void Layer::SendConsPreAlloc(int alloc_no, Projection* prjn) {
  FOREACH_ELEM_IN_GROUP(Unit, su, units) {
    su->SendConsPreAlloc(alloc_no, prjn);
  }
}

void Layer::SendConsPostAlloc(Projection* prjn) {
  FOREACH_ELEM_IN_GROUP(Unit, su, units) {
    su->SendConsPostAlloc(prjn);
  }
}

void Layer::RecvConsPostAlloc(Projection* prjn) {
  FOREACH_ELEM_IN_GROUP(Unit, su, units) {
    su->RecvConsPostAlloc(prjn);
  }
}

void Layer::SyncSendPrjns() {
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    Layer* snd = p->from;
    if(snd == NULL) continue;
    snd->send_prjns.LinkUnique(p); // make sure senders are all represented
  }
  // now make sure that we don't have any spurious ones
  for(int pi = send_prjns.size-1; pi >= 0; pi--) {
    if (Projection* p = (Projection*)send_prjns.FastEl(pi))
      if((!(bool)p->layer) || (p->from.ptr() != this))
        send_prjns.RemoveIdx(pi); // get rid of it!
  }
}

void Layer::UpdateSendPrjnNames() {
  for(int pi=0; pi< send_prjns.size; pi++) {
    Projection* prj = send_prjns.FastEl(pi);
    prj->UpdateName();
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
  if(unit_groups) {
    flat_geom.x = un_geom.x * gp_geom.x;
    flat_geom.y = un_geom.y * gp_geom.y;
    flat_geom.n = un_geom.n * gp_geom.n;
    if(flat_geom.n != flat_geom.x * flat_geom.y)
      flat_geom.n_not_xy = true;
    taVector2i eff_un_sz = un_geom + gp_spc;
    disp_geom = gp_geom * eff_un_sz;
    disp_geom -= gp_spc;        // no space at the end!
  }
  else {
    flat_geom = un_geom;
    disp_geom = un_geom;
  }
  scaled_disp_geom.x = (int)ceil((float)disp_geom.x * disp_scale);
  scaled_disp_geom.y = (int)ceil((float)disp_geom.y * disp_scale);
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

void Layer::LayoutUnits() {
  StructUpdate(true);
  UpdateGeometry();             // triple sure..
  units.pos = 0;                // our base guy must always be 0..
  if(unit_groups) {
    taVector2i eff_un_sz = un_geom + gp_spc;
    taVector2i gpgeo;
    int gi = 0;
    int ui = 0;
    Unit_Group* eff_ug = &units;
    for(gpgeo.y=0; gpgeo.y < gp_geom.y; gpgeo.y++) {
      for(gpgeo.x=0; gpgeo.x < gp_geom.x; gpgeo.x++, gi++) {
        taVector2i gp_pos = gpgeo * eff_un_sz;
        if(!virt_groups) {
          Unit_Group* ug = (Unit_Group*)units.gp.FastEl(gi);
          ug->pos.x = gp_pos.x; ug->pos.y = gp_pos.y;
          eff_ug = ug;
          ui = 0;
        }
        taVector2i ugeo;
        for(ugeo.y=0; ugeo.y < un_geom.y; ugeo.y++) {
          for(ugeo.x=0; ugeo.x < un_geom.x; ugeo.x++) {
            if(ui >= eff_ug->size)
              break;
            Unit* un = (Unit*)eff_ug->FastEl(ui++);
            un->ug_idx = gi;
            taVector2i upos = ugeo;
            if(virt_groups)
              upos += gp_pos;
            un->pos.x = upos.x; un->pos.y = upos.y;
          }
        }
      }
    }
  }
  else {
    taVector2i ugeo;
    int i = 0;
    for(ugeo.y=0; ugeo.y < un_geom.y; ugeo.y++) {
      for(ugeo.x=0; ugeo.x <un_geom.x; ugeo.x++) {
        if(i >= units.size)
          break;
        Unit* un = (Unit*)units.FastEl(i++);
        un->ug_idx = -1;
        un->pos.x = ugeo.x; un->pos.y = ugeo.y;
      }
    }
  }
  StructUpdate(false);
}


void Layer::SetNUnits(int n_units) {
  if(un_geom.n == n_units || n_units <= 0) return; // only if diff or sensible
  un_geom.FitN(n_units);
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
  unit_spec.CheckSpec(units.el_typ);
  UpdateUnitSpecs();

  FOREACH_ELEM_IN_GROUP(Projection, prjn, projections) {
    prjn->CheckSpecs();
  }
}

void Layer::BuildUnits() {
  taMisc::Busy();
  StructUpdate(true);
  UpdateGeometry();
  UpdatePosition();
  UpdatePrjnIdxs();
  units_lesioned = false;
  bool units_changed = false;
  if(unit_groups) {
    gp_output_names.SetGeom(2,gp_geom.x,gp_geom.y);
    if(virt_groups) {
      if(units.gp.size > 0)
        units_changed = true;
      units.gp.RemoveAll();     // in case there were any subgroups..
      if(units.size != flat_geom.n)
        units_changed = true;
      units.SetSize(flat_geom.n);
      units.EnforceType();
      FOREACH_ELEM_IN_GROUP(Unit, u, units) {
        u->BuildUnits();
      }
    }
    else {
      while(units.size > 0) {
        units_changed = true;
        units.RemoveIdx(units.size-1); // get rid of any in top-level
      }
      units.gp.SetSize(gp_geom.n);
      for(int k=0; k< units.gp.size; k++) {
        Unit_Group* ug = (Unit_Group*)units.gp.FastEl(k);
        ug->UpdateAfterEdit_NoGui();
        ug->StructUpdate(true);
        if(ug->size != un_geom.n)
          units_changed = true;
        ug->SetSize(un_geom.n);
        ug->EnforceType();
        FOREACH_ELEM_IN_GROUP(Unit, u, *ug) {
          u->BuildUnits();
        }
        ug->StructUpdate(false);
      }
    }
  }
  else {
    gp_geom.SetXYN(1,1,1);      // reset gp geom to reflect a single group -- used in some computations to generically operate over different geoms
    if(units.gp.size > 0)
      units_changed = true;
    units.gp.RemoveAll();       // in case there were any subgroups..
    if(units.size != un_geom.n)
      units_changed = true;
    units.SetSize(un_geom.n);
    units.EnforceType();
    FOREACH_ELEM_IN_GROUP(Unit, u, units) {
      u->BuildUnits();
    }
  }

  LayoutUnits();
  // assign the spec
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    u->SetUnitSpec(unit_spec.SPtr());
  }
  if(units_changed) {
    // tell all projections that they need to be connected
    FOREACH_ELEM_IN_GROUP(Projection, pjn, projections) {
      pjn->projected = false;
    }
    FOREACH_ELEM_IN_GROUP(Projection, pjn, send_prjns) {
      pjn->projected = false;
    }
  }
  SetUnitNames();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::UpdatePrjnIdxs() {
  // make sure we always have up-to-date count of active prjns..
  n_recv_prjns = 0;
  n_send_prjns = 0;
  FOREACH_ELEM_IN_GROUP(Projection, prj, projections) {
    if(prj->IsActive()) {
      prj->recv_idx = n_recv_prjns++;
    }
    else {
      prj->recv_idx = -1;
    }
  }
  FOREACH_ELEM_IN_GROUP(Projection, prj, send_prjns) {
    if(prj->IsActive()) {
      prj->send_idx = n_send_prjns++;
    }
    else {
      prj->send_idx = -1;
    }
  }
}

void Layer::BuildUnitsFlatList(Network* net) {
  units_flat_idx = net->units_flat.size;
  FOREACH_ELEM_IN_GROUP(Unit, un, units) {
    if(un->lesioned()) continue;
    un->flat_idx = net->units_flat.size;
    net->units_flat.Add(un);
  }
  // this is needed after loading for no_build nets -- _threads called then..
  if(unit_groups && gp_geom.n > 0)
    gp_output_names.SetGeom(2,gp_geom.x,gp_geom.y);
}

bool Layer::CheckBuild(bool quiet) {
  bool rval = true;
  if(!units_lesioned) {
    if(unit_groups && !virt_groups) {
      if(CheckError((units.gp.size != gp_geom.n), quiet, rval,
                    "number of unit groups != target -- is:", String(units.gp.size),
                    "should be:", String(gp_geom.n))) {
        return false;
      }
      for(int g=0; g<units.gp.size; g++) {
        Unit_Group* ug = (Unit_Group*)units.gp.FastEl(g);
        if(CheckError((ug->size != un_geom.n), quiet, rval,
                      "number of units in group:",String(g),"!= target -- is:", String(ug->size),
                      "should be:", String(un_geom.n))) {
          return false;
        }
      }
    }
    else {
      if(CheckError((units.size != flat_geom.n), quiet, rval,
                    "number of units != target -- is:", String(units.size),
                    "should be:", String(flat_geom.n))) {
        return false;
      }
    }
  }

  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(CheckError((u->GetTypeDef() != units.el_typ), quiet, rval,
                  "unit type not correct -- should be:", units.el_typ->name)) {
      return false;
    }
    if(u->lesioned()) continue;
    if(!u->CheckBuild(quiet))
      return false;
  }
  return true;
}

bool Layer::CheckConnect(bool quiet) {
  FOREACH_ELEM_IN_GROUP(Projection, prjn, projections) {
    if(!prjn->CheckConnect(quiet)) return false;
  }
  return true;
}

void Layer::CheckThisConfig_impl(bool quiet, bool& rval) {
  // note: network also called our checks
  // slightly non-standard, since we bail on first error

  if(lesioned()) return;

  CheckSpecs();

  if (!CheckBuild(quiet)) {rval = false; return;}
  if (!CheckConnect(quiet)) {rval = false; return;}

  UnitSpec* us = GetUnitSpec();
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
  units.CheckConfig(quiet, rval);
  projections.CheckConfig(quiet, rval);
}

void Layer::RemoveUnits() {
  taMisc::Busy();
  StructUpdate(true);
  if(units.gp.size == 0) {
    units.RemoveAll();
  }
  else {
    for(int g=0; g<units.gp.size; g++) {
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(g);
      ug->RemoveAll();
    }
  }
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::RemoveUnitGroups() {
  taMisc::Busy();
  StructUpdate(true);
  units.RemoveAll();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::Connect_Sizes(Network* net) {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    if(p->NotActive()) continue;
    p->Connect_Sizes();
  }

  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::Connect_Cons(Network* net) {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    if(p->NotActive()) continue;
    p->Connect_Cons();
  }

  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::DisConnect() {
  StructUpdate(true);
  DisConnect_impl();
  StructUpdate(false);
}

void Layer::DisConnect_impl() {
  int pi;
  for(pi=send_prjns.size-1; pi>=0; pi--) {
    Projection* p = (Projection*)send_prjns.FastEl(pi);
    if(p == NULL) continue;
    if(!(bool)p->layer) {
      send_prjns.RemoveIdx(pi);
      continue;
    }
    p->layer->projections.RemoveLeafEl(p);
  }
  send_prjns.Reset();
  projections.Reset();
}

int Layer::CountCons(Network* net) {
  int n_cons = 0;
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    n_cons += u->CountCons(net);
  }
  return n_cons;
}

void Layer::SetLayUnitExtFlags(int flg) {
  SetExtFlag(flg);
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    u->GetUnitVars()->SetExtFlag((UnitVars::ExtFlags)flg);
  }
}

void Layer::ApplyInputData(taMatrix* data, UnitVars::ExtFlags ext_flags,
           Random* ran, const PosVector2i* offset, bool na_by_range)
{
  // note: when use LayerWriters, we typically always just get a single frame of
  // the exact dimensions, and so ignore 'frame'
  if (!data || lesioned()) return;
  // check correct geom of data
  if(TestError((data->dims() != 2) && (data->dims() != 4), "ApplyInputData",
               "data->dims must be 2 (2-d) or 4 (4-d)")) {
    return;
  }
  taVector2i offs(0,0);
  if(offset) offs = *offset;

  // apply flags if we are the controller (zero offset)
  if((offs.x == 0) && (offs.y == 0)) {
    ApplyLayerFlags(ext_flags);
  }
  if(data->dims() == 1) {
    ApplyInputData_1d(data, ext_flags, ran, na_by_range);
  }
  else if(data->dims() == 2) {
    ApplyInputData_2d(data, ext_flags, ran, offs, na_by_range);
  }
  else {
    if(unit_groups)
      ApplyInputData_Gp4d(data, ext_flags, ran, na_by_range); // note: no offsets -- layerwriter does check
    else
      ApplyInputData_Flat4d(data, ext_flags, ran, offs, na_by_range);
  }
}

void Layer::ApplyInputData_1d(taMatrix* data, UnitVars::ExtFlags ext_flags,
                              Random* ran, bool na_by_range) {
  for(int d_x = 0; d_x < data->dim(0); d_x++) {
    Unit* un = units.Leaf(d_x);
    if(un) {
      float val = data->SafeElAsVar(d_x).toFloat();
      un->ApplyInputData(val, ext_flags, ran, na_by_range);
    }
  }
}

void Layer::ApplyInputData_2d(taMatrix* data, UnitVars::ExtFlags ext_flags,
                              Random* ran, const taVector2i& offs, bool na_by_range) {
  for(int d_y = 0; d_y < data->dim(1); d_y++) {
    int u_y = offs.y + d_y;
    for(int d_x = 0; d_x < data->dim(0); d_x++) {
      int u_x = offs.x + d_x;
      Unit* un = UnitAtCoord(u_x, u_y);
      if(un) {
        float val = data->SafeElAsVar(d_x, d_y).toFloat();
        un->ApplyInputData(val, ext_flags, ran, na_by_range);
      }
    }
  }
}

void Layer::ApplyInputData_Flat4d(taMatrix* data, UnitVars::ExtFlags ext_flags,
                                  Random* ran, const taVector2i& offs, bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int u_y = offs.y + dg_y * data->dim(1) + d_y; // multiply out data indicies
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int u_x = offs.x + dg_x * data->dim(0) + d_x; // multiply out data indicies
          Unit* un = UnitAtCoord(u_x, u_y);
          if(un) {
            float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
            un->ApplyInputData(val, ext_flags, ran, na_by_range);
          }
        }
      }
    }
  }
}

void Layer::ApplyInputData_Gp4d(taMatrix* data, UnitVars::ExtFlags ext_flags, Random* ran,
                                bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          Unit* un = UnitAtGpCoord(dg_x, dg_y, d_x, d_y);
          if(un) {
            float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
            un->ApplyInputData(val, ext_flags, ran, na_by_range);
          }
        }
      }
    }
  }
}

void Layer::ApplyLayerFlags(UnitVars::ExtFlags act_ext_flags) {
  SetExtFlag(act_ext_flags);
}

////////////////////////////////////////////////////////////////////////////////
//  Below are the primary computational interface to the Network Objects
//  for performing algorithm-specific activation and learning
//  All functions at network level operate directly on the units via threads, with
//  optional call through to the layers for any layer-level subsequent processing

void Layer::Init_InputData(Network* net) {
  ext_flag = UnitVars::NO_EXTERNAL;
  // unit-level done separately!
}

void  Layer::Init_Acts(Network* net) {
  ext_flag = UnitVars::NO_EXTERNAL;
  // unit-level done separately!
}

void Layer::Init_Weights_Layer(Network* net) {
}

void Layer::Init_Weights(bool recv_cons) {
  if(!own_net) return;
  if(!own_net->IsBuiltIntact()) return;
  if(recv_cons) {
    FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
      if(p->NotActive()) continue;
      p->Init_Weights();
    }
  }
  else {
    FOREACH_ELEM_IN_GROUP(Projection, p, send_prjns) {
      if(p->NotActive()) continue;
      p->Init_Weights();
    }
  }
}

void Layer::Init_Stats(Network* net) {
  sse = 0.0f;
  avg_sse.ResetAvg();
  cnt_err = 0.0f;
  cur_cnt_err = 0.0f;
  pct_err = 0.0f;
  pct_cor = 0.0f;

  sum_prerr.InitVals();
  epc_prerr.InitVals();

  output_name = "";
}

float Layer::Compute_SSE(Network* net, int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  sse = 0.0f;
  if(!HasExtFlag(UnitVars::COMP_TARG)) return 0.0f;
  if(layer_type == HIDDEN) return 0.0f;
  
  const int li = active_lay_idx;
  for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
    // integrate over thread raw data
    float& lay_sse = net->ThrLayStats(thr_no, li, 0, Network::SSE);
    float& lay_n = net->ThrLayStats(thr_no, li, 1, Network::SSE);

    sse += lay_sse;
    n_vals += (int)lay_n;
  }
  
  float rval = sse;
  if(unit_avg && n_vals > 0) {
    sse /= (float)n_vals;
  }
  if(sqrt) {
    sse = sqrtf(sse);
  }
  avg_sse.Increment(sse);
  if(sse > net->stats.cnt_err_tol)
    cur_cnt_err += 1.0;
  if(HasLayerFlag(NO_ADD_SSE) || (HasExtFlag(UnitVars::COMP) &&
                                  HasLayerFlag(NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

int Layer::Compute_PRerr(Network* net) {
  int n_vals = 0;
  prerr.InitVals();
  if(!HasExtFlag(UnitVars::COMP_TARG)) return 0;
  if(layer_type == HIDDEN) return 0;

  const int li = active_lay_idx;
  for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
    // integrate over thread raw data
    float& true_pos = net->ThrLayStats(thr_no, li, 0, Network::PRERR);
    float& false_pos = net->ThrLayStats(thr_no, li, 1, Network::PRERR);
    float& false_neg = net->ThrLayStats(thr_no, li, 2, Network::PRERR);
    float& true_neg = net->ThrLayStats(thr_no, li, 3, Network::PRERR);
    float& lay_n = net->ThrLayStats(thr_no, li, 4, Network::PRERR);

    n_vals += (int)lay_n;
    prerr.true_pos += true_pos;
    prerr.false_pos += false_pos;
    prerr.false_neg += false_neg;
    prerr.true_neg += true_neg;
  }
  prerr.ComputePR();
  if(HasLayerFlag(NO_ADD_SSE) || (HasExtFlag(UnitVars::COMP) &&
                                  HasLayerFlag(NO_ADD_COMP_SSE))) {
    n_vals = 0;
  }
  return n_vals;
}

void Layer::Compute_EpochSSE(Network* net) {
  cnt_err = cur_cnt_err;
  if(avg_sse.n > 0) {
    pct_err = cnt_err / (float)avg_sse.n;
    pct_cor = 1.0f - pct_err;
  }
  avg_sse.GetAvg_Reset();

  cur_cnt_err = 0.0f;
}

void Layer::Compute_EpochPRerr(Network* net) {
  epc_prerr = sum_prerr;
  epc_prerr.ComputePR();        // make sure, in case of dmem summing
  sum_prerr.InitVals();         // reset!
}

void Layer::Compute_EpochStats(Network* net) {
#ifdef DMEM_COMPILE
  DMem_ComputeAggs(net->dmem_trl_comm.comm);
#endif
  Compute_EpochSSE(net);
  if(net->stats.prerr)
    Compute_EpochPRerr(net);
}

////////////////////////////////////////////////////////////////////////////////
//      The following are misc functionality not required for primary computing

void Layer::Copy_Weights(const Layer* src) {
  units.Copy_Weights(&(src->units));
}

void Layer::SaveWeights_strm(ostream& strm, ConGroup::WtSaveFormat fmt, Projection* prjn) {
  // name etc is saved & processed by network level guy -- this is equiv to unit group
  
  // save any #SAVE_WTS layer vals
  TypeDef* td = GetTypeDef();
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("SAVE_WTS")) continue;
    strm << "<" << md->name << " " << md->GetValStr((void*)this) << ">\n";
  }
  
  units.SaveWeights_strm(strm, fmt, prjn);
}

int Layer::LoadWeights_strm(istream& strm, ConGroup::WtSaveFormat fmt, bool quiet, Projection* prjn) {
  // name etc is saved & processed by network level guy -- this is equiv to unit group

  // load any #SAVE_WTS layer vals
  TypeDef* td = GetTypeDef();
  while(true) {
    int c = strm.peek();          // check for <U for UnGp
    if(c == '<') {
      strm.get();
      c = strm.peek();
      if(c == 'U') {
        strm.unget();           // < goes back
        break;                  // done
      }
      else { // got a SAVE_WTS member
        strm.unget();           // < goes back
        String tag;
        String val;
        taMisc::read_tag(strm, tag, val);
        MemberDef* md = td->members.FindName(tag);
        if(md) {
          md->SetValStr(val, (void*)this);
        }
        else {
          TestWarning(true, "LoadWeights",
                      "member not found:", tag, "value:", val);
        }
      }
    }
    else {
      break;                  // some other badness
    }
  }
  
  return units.LoadWeights_strm(strm, fmt, quiet, prjn);
}

int Layer::SkipWeights_strm(istream& strm, ConGroup::WtSaveFormat fmt, bool quiet) {
  return Unit_Group::SkipWeights_strm(strm, fmt, quiet);
}

void Layer::SaveWeights(const String& fname) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm);
  flr->Close();
  taRefN::unRefDone(flr);
}

bool Layer::LoadWeights(const String& fname, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  bool rval = false;
  if(flr->istrm) {
    rval = LoadWeights_strm(*flr->istrm, ConGroup::TEXT, quiet);
  }
  else {
    TestError(true, "LoadWeights", "aborted due to inability to load weights file");
    // the above should be unnecessary but we're not getting the error sometimes..
  }
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Layer::PropagateInputDistance() {
  int new_dist = dist.fm_input + 1;
  FOREACH_ELEM_IN_GROUP(Projection, p, send_prjns) {
    if(p->NotActive()) continue;
    if(p->layer->dist.fm_input >= 0) { // already set
      if(new_dist < p->layer->dist.fm_input) { // but we're closer
        p->layer->dist.fm_input = new_dist;
        p->layer->PropagateInputDistance(); // note: this could lead back to us, but big deal.
        // the < sign prevents loops from continuing indefinitely.
      }
    }
    else { // not set yet
      p->layer->dist.fm_input = new_dist;
      p->layer->PropagateInputDistance();
    }
  }
}

void Layer::PropagateOutputDistance() {
  int new_dist = dist.fm_output + 1;
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    if(p->NotActive()) continue;
    if(p->from->dist.fm_output >= 0) { // already set
      if(new_dist < p->from->dist.fm_output) { // but we're closer
        p->from->dist.fm_output = new_dist;
        p->from->PropagateOutputDistance(); // note: this could lead back to us, but big deal.
        // the < sign prevents loops from continuing indefinitely.
      }
    }
    else { // not set yet
      p->from->dist.fm_output = new_dist;
      p->from->PropagateOutputDistance();
    }
  }
}

void Layer::Compute_PrjnDirections() {
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    if(p->dir_fixed && p->direction != Projection::DIR_UNKNOWN)
      continue;
    if(p->NotActive()) {
      p->direction = Projection::DIR_UNKNOWN;
      continue;
    }
    // use the smallest value first..
    if(p->from->dist.fm_input <= p->from->dist.fm_output) {
      if(p->from->dist.fm_input < dist.fm_input) {
        p->direction = Projection::FM_INPUT;
      }
      else if(p->from->dist.fm_output < dist.fm_output) {
        p->direction = Projection::FM_OUTPUT;
      }
      else {
        p->direction = Projection::LATERAL;
      }
    }
    else {
      if(p->from->dist.fm_output < dist.fm_output) {
        p->direction = Projection::FM_OUTPUT;
      }
      else if(p->from->dist.fm_input < dist.fm_input) {
        p->direction = Projection::FM_INPUT;
      }
      else {
        p->direction = Projection::LATERAL;
      }
    }
  }
}

bool Layer::SetUnitNames(bool force_use_unit_names) {
  if(!force_use_unit_names && !HasLayerFlag(SAVE_UNIT_NAMES)) return false;
  SetLayerFlag(SAVE_UNIT_NAMES);
  // first enforce geom, then do it.
  if(unit_groups) {
    if(gp_unit_names_4d) {
      unit_names.SetGeom(4, un_geom.x, un_geom.y, gp_geom.x, gp_geom.y);
      int gx, gy, ux, uy;
      for (gy = 0; gy < gp_geom.y; ++gy) {
        for (gx = 0; gx < gp_geom.x; ++gx) {
          for (uy = 0; uy < un_geom.y; ++uy) {
            for (ux = 0; ux < un_geom.x; ++ux) {
              Unit* un = UnitAtGpCoord(gx, gy, ux, uy);
              if (!un) continue;
              un->SetName(unit_names.SafeEl(ux, uy, gx, gy));
            }
          }
        }
      }
    }
    else {
      unit_names.SetGeom(2, un_geom.x, un_geom.y);
      int gx, gy, ux, uy;
      for (gy = 0; gy < gp_geom.y; ++gy) {
        for (gx = 0; gx < gp_geom.x; ++gx) {
          for (uy = 0; uy < un_geom.y; ++uy) {
            for (ux = 0; ux < un_geom.x; ++ux) {
              Unit* un = UnitAtGpCoord(gx, gy, ux, uy);
              if (!un) continue;
              un->SetName(unit_names.SafeEl(ux, uy));
            }
          }
        }
      }
    }
  }
  else {
    unit_names.SetGeom(2, un_geom.x, un_geom.y);
    int x, y;
    for (y = 0; y < un_geom.y; ++y) {
      for (x = 0; x < un_geom.x; ++x) {
        Unit* un = UnitAtCoord(x, y);
        if (!un) continue;
        un->SetName(unit_names.SafeEl(x, y));
      }
    }
  }
  return true;
}

bool Layer::GetUnitNames(bool force_use_unit_names) {
  if(!force_use_unit_names && !HasLayerFlag(SAVE_UNIT_NAMES)) return false;
  SetLayerFlag(SAVE_UNIT_NAMES);
  // first enforce geom, then do it.
  if(unit_groups) {
    if(gp_unit_names_4d) {
      unit_names.SetGeom(4, un_geom.x, un_geom.y, gp_geom.x, gp_geom.y);
      int gx, gy, ux, uy;
      for (gy = 0; gy < gp_geom.y; ++gy) {
        for (gx = 0; gx < gp_geom.x; ++gx) {
          for (uy = 0; uy < un_geom.y; ++uy) {
            for (ux = 0; ux < un_geom.x; ++ux) {
              Unit* un = UnitAtGpCoord(gx, gy, ux, uy);
              if (!un) continue;
              unit_names.Set(un->GetName(), ux, uy, gx, gy);
            }
          }
        }
      }
    }
    else {
      unit_names.SetGeom(2, un_geom.x, un_geom.y);
      int gx, gy, ux, uy;
      for (gy = 0; gy < gp_geom.y; ++gy) {
        for (gx = 0; gx < gp_geom.x; ++gx) {
          for (uy = 0; uy < un_geom.y; ++uy) {
            for (ux = 0; ux < un_geom.x; ++ux) {
              Unit* un = UnitAtGpCoord(gx, gy, ux, uy);
              if (!un) continue;
              unit_names.Set(un->GetName(), ux, uy);
            }
          }
        }
      }
    }
  }
  else {
    unit_names.SetGeom(2, un_geom.x, un_geom.y);
    int x, y;
    for (y = 0; y < un_geom.y; ++y) {
      for (x = 0; x < un_geom.x; ++x) {
        Unit* un = UnitAtCoord(x, y);
        if (!un) continue;
        unit_names.Set(un->GetName(), x, y);
      }
    }
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
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    u->GetLocalistName();
  }
  GetUnitNames(); // grab from units
}

int Layer::FindUnitNamedIdx(const String& nm, bool err) {
  int idx = unit_names.FindVal_Flat(nm);
  TestError(err && idx < 0, "FindUnitNamedIdx",
            "name:", nm, "not found in unit_names");
  return idx;
}

Unit* Layer::FindUnitNamed(const String& nm, bool err) {
  int idx = FindUnitNamedIdx(nm, err);
  if(idx >= 0) {
    return units.Leaf(idx);
  }
  return NULL;
}

void Layer::TransformWeights(const SimpleMathSpec& trans) {
  units.TransformWeights(trans);
}

void Layer::AddNoiseToWeights(const Random& noise_spec) {
  units.AddNoiseToWeights(noise_spec);
}

int Layer::PruneCons(const SimpleMathSpec& pre_proc,
                        Relation::Relations rel, float cmp_val)
{
  return units.PruneCons(pre_proc, rel, cmp_val);
}

int Layer::ProbAddCons(float p_add_con, float init_wt) {
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    if(p->NotActive()) continue;
    rval += p->ProbAddCons(p_add_con, init_wt);
  }
  return rval;
}

int Layer::LesionCons(float p_lesion, bool permute) {
  return units.LesionCons(p_lesion, permute);
}

int Layer::LesionUnits(float p_lesion, bool permute) {
  StructUpdate(true);
  int rval = units.LesionUnits(p_lesion, permute);
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
  return rval;
}

void Layer::UnLesionUnits() {
  StructUpdate(true);
  units.UnLesionUnits();
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
}

void Layer::UpdtAfterNetModIfNecc() {
  if(!own_net || own_net->InStructUpdate()) return;
  own_net->UpdtAfterNetMod();
}

bool Layer::UpdateUnitSpecs(bool force) {
  if(!force && (unit_spec.SPtr() == m_prv_unit_spec)) return false;
  UnitSpec* sp = unit_spec.SPtr();
  if(!sp) return false;
  m_prv_unit_spec = sp;         // don't redo it
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(sp->CheckObjectType(u))
      u->SetUnitSpec(sp);
    else
      return false;             // don't generate a bunch of redundant messages..
  }
  return true;
}

bool Layer::UpdateConSpecs(bool force) {
  bool rval = true;
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    if(!p->UpdateConSpecs(force))
      rval = false;
  }
  return rval;
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
  if(!unit_spec.CheckObjTypeForSpec(units.el_typ)) {
    if(taMisc::Choice("The unit spec you are setting is not compatible with the unit type for units in this layer -- should I change the unit type to be: " +  unit_spec->min_obj_type->name
                      + " (if you answer No, you will continue to get errors until a compatible selection is made)",
                      "Yes", "No") == 0) {
      units.el_typ = unit_spec->min_obj_type;
    }
  }
  return UpdateUnitSpecs();
}

void Layer::SetUnitType(TypeDef* td) {
  if(td == NULL) return;
  units.el_typ = td;
  if(units.gp.size > 0) {
    int j;
    for(j=0;j<units.gp.size;j++) {
      ((Unit_Group*)units.gp.FastEl(j))->el_typ = td;
    }
  }
}

void Layer::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

bool Layer::Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap) {
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    if(!u->Snapshot(variable, math_op, arg_is_snap)) return false;
  }
  return true;
}

Unit* Layer::MostActiveUnit(int& idx) {
  return units.MostActiveUnit(idx);
}

int Layer::ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp) {
  int nchg = 0;
  if(unit_spec.SPtr() == old_sp) {
    unit_spec.SetSpec(new_sp);
    nchg++;
  }
  UpdateUnitSpecs();
  return nchg;
}

int Layer::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Projection, p, projections)
    nchg += p->ReplaceConSpec(old_sp, new_sp);
  return nchg;
}

int Layer::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Projection, p, projections)
    nchg += p->ReplacePrjnSpec(old_sp, new_sp);
  return nchg;
}

int Layer::ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp) {
  if(GetLayerSpec() != old_sp) return 0;
  SetLayerSpec(new_sp);
  return 1;
}

DataTable* Layer::WeightsToTable(DataTable* dt, Layer* send_lay) {
  bool new_table = false;
  if (!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable(name + "_Weights", true);
    new_table = true;
  }
  if(send_lay == NULL) return NULL;
  bool gotone = false;
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    if(p->NotActive()) continue;
    if(p->from.ptr() != send_lay) continue;
    p->WeightsToTable(dt);
    gotone = true;
  }
  TestError(!gotone, "WeightsToTable", "No sending projection from:", send_lay->name);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* Layer::VarToTable(DataTable* dt, const String& variable) {
  bool new_table = false;
  if (!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
    new_table = true;
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return NULL;

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.SetDataNetwork(dt, net);
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
                           Projection* prjn) {
  bool new_table = false;
  if(!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  units.ConVarsToTable(dt, var1, var2, var3, var4, var5, var6, var7, var8,
                       var9, var10, var11, var12, var13, var14, prjn);
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
    for(int i=0; i<send_prjns.size; i++) {
      Projection* pj = send_prjns.FastEl(i);
      dt->AddBlankRow();
      dt->SetVal(pj->layer->name, colnm, -1);
      ProjectionSpec* ps = pj->GetPrjnSpec();
      if(ps)
        dt->SetVal(ps->name, "PrjnSpec", -1);
      else
        dt->SetVal("NULL", "PrjnSpec", -1);
      ConSpec* cs = pj->GetConSpec();
      if(cs)
        dt->SetVal(cs->name, "ConSpec", -1);
      else
        dt->SetVal("NULL", "ConSpec", -1);
    }
  }
  else {
    for(int i=0; i<projections.size; i++) {
      Projection* pj = projections.FastEl(i);
      dt->AddBlankRow();
      dt->SetVal(pj->from->name, colnm, -1);
      ProjectionSpec* ps = pj->GetPrjnSpec();
      if(ps)
        dt->SetVal(ps->name, "PrjnSpec", -1);
      else
        dt->SetVal("NULL", "PrjnSpec", -1);
      ConSpec* cs = pj->GetConSpec();
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
  return units.VarToVarCopy(dest_var, src_var);
}

bool Layer::VarToVal(const String& dest_var, float val) {
  return units.VarToVal(dest_var, val);
}

Unit* Layer::UnitAtCoord(int x, int y) const {
  if(unit_groups) {
    int gp_x = x / un_geom.x;
    int gp_y = y / un_geom.y;
    int un_x = x % un_geom.x;
    int un_y = y % un_geom.y;
    return UnitAtGpCoord(gp_x, gp_y, un_x, un_y);
  }
  else {
    if(x >= un_geom.x) return NULL; // y will be caught by safe..
    int idx = y * un_geom.x + x;
    return units.SafeEl(idx);
  }
  return NULL;
}

Unit* Layer::UnitAtGpCoord(int gp_x, int gp_y, int un_x, int un_y) const {
  if(TestError(!unit_groups, "UnitAtGpCoord", "Layer is not configured for unit_groups"))
    return NULL;
  if(gp_x >= gp_geom.x || gp_y >= gp_geom.y ||
     un_x >= un_geom.x || un_y >= un_geom.y) return NULL;
  int gpidx = gp_y * gp_geom.x + gp_x;
  int unidx = un_y * un_geom.x + un_x;
  return UnitAtUnGpIdx(unidx, gpidx);
}

Unit* Layer::UnitAtGpIdxUnCoord(int gpidx, int un_x, int un_y) const {
  if(TestError(!unit_groups, "UnitAtGpIdxUnCoord", "Layer is not configured for unit_groups"))
    return NULL;
  int unidx = un_y * un_geom.x + un_x;
  return UnitAtUnGpIdx(unidx, gpidx);
}

Unit_Group* Layer::UnitGpAtCoord(int gp_x, int gp_y) const {
  if(TestError(!unit_groups, "UnitGpAtCoord", "Layer is not configured for unit_groups"))
    return NULL;
  if(gp_x >= gp_geom.x) return NULL; // y will be caught by safe..
  int gidx = gp_y * gp_geom.x + gp_x;
  return (Unit_Group*)units.gp.SafeEl(gidx);
}

void Layer::UnitLogPos(Unit* un, int& x, int& y) const {
  Unit_Group* own_sgp = un->own_subgp();
  if(own_sgp) {
    taVector2i gpos = own_sgp->GpLogPos();
    x = gpos.x + un->pos.x;
    y = gpos.y + un->pos.y;
  }
  else {
    if(unit_groups && virt_groups) {
      int gpidx = un->idx / un_geom.n;
      int unidx = un->idx % un_geom.n;
      int gp_y = gpidx / gp_geom.x;
      int gp_x = gpidx % gp_geom.x;
      int un_y = unidx / un_geom.x;
      int un_x = unidx % un_geom.x;
      y = gp_y * un_geom.y + un_y;
      x = gp_x * un_geom.x + un_x;
    }
    else {
      y = un->idx / flat_geom.x;  // unit index relative to flat geom
      x = un->idx % flat_geom.x;
    }
  }
}

void Layer::UnitInGpLogPos(Unit* un, int& x, int& y) const {
  Unit_Group* own_sgp = un->own_subgp();
  if(own_sgp || !unit_groups) {
    x = un->pos.x;
    y = un->pos.y;
  }
  else {
    if(unit_groups && virt_groups) {
      int gpidx = un->idx / un_geom.n;
      int unidx = un->idx % un_geom.n;
      y = unidx / un_geom.x;
      x = unidx % un_geom.x;
    }
  }
}

int Layer::UnitInGpUnIdx(Unit* un) const {
  Unit_Group* own_sgp = un->own_subgp();
  if(own_sgp || !unit_groups) {
    return un->idx;      // just basic index
  }
  else {
    if(unit_groups && virt_groups) {
      return un->idx % un_geom.n;
    }
  }
  return un->idx;               // bad fallback..
}

Unit* Layer::UnitAtDispCoord(int x, int y) const {
  if(unit_groups && !virt_groups) {
    // unit group can have its own position -- need to search through each one
    for(int gi = 0; gi< units.gp.size; gi++) {
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(gi);
      if(x >= ug->pos.x && x < ug->pos.x + un_geom.x &&
         y >= ug->pos.y && y < ug->pos.y + un_geom.y) {
        return ug->UnitAtCoord(x - ug->pos.x, y - ug->pos.y);
      }
    }
    return NULL;                // not found
  }
  else if(unit_groups && virt_groups) {
    int gpsz_x = un_geom.x + gp_spc.x;
    int gpsz_y = un_geom.y + gp_spc.y;
    int gp_x = x / gpsz_x;
    int gp_y = y / gpsz_y;
    int gpst_x = gp_x * gpsz_x;
    int gpst_y = gp_y * gpsz_y;
    int un_x = x - gpst_x;
    int un_y = y - gpst_y;
    return UnitAtGpCoord(gp_x, gp_y, un_x, un_y);
  }
  else {
    int idx = y * un_geom.x + x;
    return units.SafeEl(idx);
  }
}

void Layer::UnitDispPos(Unit* un, int& x, int& y) const {
  Unit_Group* own_sgp = un->own_subgp();
  if(own_sgp) {
    x = own_sgp->pos.x + un->pos.x;
    y = own_sgp->pos.y + un->pos.y;
  }
  else {                        // otherwise unit has it directly..
    x = un->pos.x;
    y = un->pos.y;
  }
}

bool Layer::InLayerSubGroup() {
  if(owner && owner->GetOwner() && owner->GetOwner()->InheritsFrom(&TA_Network))
    return false;
  return true;
}

bool Layer::ChangeMyType(TypeDef* new_typ) {
  if(TestError(units.leaves > 0, "ChangeMyType", "You must first remove all units in the network before changing type of Layer -- otherwise it takes FOREVER -- do Network/Structure/Remove Units"))
    return false;
  return inherited::ChangeMyType(new_typ);
}


#ifdef DMEM_COMPILE

void Layer::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}

void Layer::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}

#endif
