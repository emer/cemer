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

#include "Layer.h"
#include <Network>
#include <MemberDef>
#include <ProjectBase>
#include <taFiler>
#include <NetMonitor>

#include <tabMisc>
#include <taMisc>

using namespace std;


void Layer::Initialize() {
  // desc = ??
  own_net = NULL;
  lesion_ = false;
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
  projections.SetBaseType(&TA_Projection);
  send_prjns.send_prjns = true;
  units.SetBaseType(&TA_Unit);
  // unit_spec = ??
  ext_flag = Unit::NO_EXTERNAL;
  // flat_geom = ??
  disp_geom = un_geom;
  scaled_disp_geom = disp_geom;
  dmem_dist = DMEM_DIST_DEFAULT;
  // dist = ??
  // output_name = ??
  // gp_output_names = ??
  m_prv_unit_spec = NULL;
  m_prv_layer_flags = LF_NONE;

  sse = 0.0f;
  // prerr = ??
  icon_value = 0.0f;
  units_flat_idx = 0;
  units_lesioned = false;
  gp_unit_names_4d = false;
  // unit_names = ??
  brain_area = "";
  voxel_fill_pct = 1.0f;
  n_units = 0;                  // note: v3compat obs
}

void Layer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(unit_spec, this);
  taBase::Own(units, this);
  taBase::Own(projections, this);
  taBase::Own(send_prjns, this);
  taBase::Own(pos, this);
  taBase::Own(un_geom, this);
  taBase::Own(gp_geom, this);
  taBase::Own(gp_spc, this);
  taBase::Own(flat_geom, this);
  taBase::Own(disp_geom, this);
  taBase::Own(scaled_disp_geom, this);
  taBase::Own(prerr, this);
#ifdef DMEM_COMPILE
  taBase::Own(dmem_share_units, this);
#endif
  taBase::Own(gp_output_names, this);
  taBase::Own(unit_names, this);
  own_net = GET_MY_OWNER(Network);
  if(pos == 0)
    SetDefaultPos();
  if(pos2d == 0)
    SetDefaultPos2d();
  units.pos.z = 0;
  unit_spec.SetDefaultSpec(this);
}

void Layer::CutLinks() {
  if(!owner) return; // already replacing or already dead
  DisConnect();
  gp_output_names.CutLinks();
  unit_names.CutLinks();
  disp_geom.CutLinks();
  scaled_disp_geom.CutLinks();
  flat_geom.CutLinks();
  gp_spc.CutLinks();
  gp_geom.CutLinks();
  un_geom.CutLinks();
  pos.CutLinks();
  send_prjns.CutLinks();
  projections.CutLinks();
  units.CutLinks();
  unit_spec.CutLinks();
  m_prv_unit_spec = NULL;
  m_prv_layer_flags = LF_NONE;
  inherited::CutLinks();
}


void Layer::Copy_(const Layer& cp) {
  if(own_net && !own_net->HasBaseFlag(COPYING)) {
    // if we're the only guy copying, then all connections are invalid now -- just nuke
    own_net->RemoveCons();
  }

  layer_type = cp.layer_type;
  flags = cp.flags;
  pos = cp.pos;
  disp_scale = cp.disp_scale;
  un_geom = cp.un_geom;
  unit_groups = cp.unit_groups;
  virt_groups = cp.virt_groups;
  gp_geom = cp.gp_geom;
  gp_spc = cp.gp_spc;
  flat_geom = cp.flat_geom;
  disp_geom = cp.disp_geom;
  scaled_disp_geom = cp.scaled_disp_geom;
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

  n_units = cp.n_units;         // note: v3compat obs

  // this will update all pointers under us to new network if we are copied from other guy
  // only if the network is not otherwise already copying too!!
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Network);

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

  if (lesion_) {                // obs: v3compat conversion obs remove later
    SetLayerFlag(LESIONED);
    lesion_ = false;
  }

  // no negative geoms., y,z must be 1 (for display)
  UpdateUnitSpecs((bool)taMisc::is_loading); // force if loading
  //  SyncSendPrjns(); // this is not a good place to do this -- too frequent and unnec
  // also causes problems during copy..

  if(taMisc::is_loading) {
    if(n_units > 0) {           // obs: v3compat conversion
      if(n_units != un_geom.x * un_geom.y) {
        un_geom.n_not_xy = true;
        un_geom.n = n_units;
      }
      n_units = 0;
    }
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
    RecomputeGeometry();
  }
  else {                        // not loading
    RecomputeGeometry();
    if(own_net)
      own_net->LayerPos_Cleanup();
    UpdateSendPrjnNames();

    if(lesioned() && !(m_prv_layer_flags & LESIONED)) {
      // clear activity if we're lesioned
      if(own_net)
        Init_Acts(own_net);
    }
  }
  m_prv_layer_flags = flags;
}

void Layer::Lesion() {
  StructUpdate(true);
  SetLayerFlag(LESIONED);
  if(own_net)
    Init_Acts(own_net);
  m_prv_layer_flags = flags;
  StructUpdate(false);
  if(own_net)
    own_net->UpdtAfterNetMod();
}

void Layer::UnLesion()  {
  StructUpdate(true);
  ClearLayerFlag(LESIONED);
  m_prv_layer_flags = flags;
  StructUpdate(false);
  if(own_net)
    own_net->UpdtAfterNetMod();
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

void Layer::RecomputeGeometry() {
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
  int index = own_net->layers.FindLeafEl(this);
  pos = 0;
  for(int i=0;i<index;i++) {
    Layer* lay = (Layer*)own_net->layers.Leaf(i);
    pos.z = MAX(pos.z, lay->pos.z + 1);
  }
}

void Layer::SetDefaultPos2d() {
  if(!own_net) return;
  int index = own_net->layers.FindLeafEl(this);
  pos2d.x = pos.x;		// should transfer..
  pos2d.y= 0;			// adapt y to fit..
  for(int i=0;i<index;i++) {
    Layer* lay = (Layer*)own_net->layers.Leaf(i);
    pos2d.y = MAX(pos2d.y, lay->pos2d.y + lay->scaled_disp_geom.y + 2);
  }
}

void Layer::LayoutUnits() {
  StructUpdate(true);
  RecomputeGeometry();
  units.pos = 0;                // our base guy must always be 0..
  if(unit_groups) {
    taVector2i eff_un_sz = un_geom + gp_spc;
    taVector2i gpgeo;
    int gi = 0;
    int ui = 0;
    Unit_Group* eff_ug = &units;
    for(gpgeo.y=0; gpgeo.y < gp_geom.y; gpgeo.y++) {
      for(gpgeo.x=0; gpgeo.x < gp_geom.x; gpgeo.x++) {
        taVector2i gp_pos = gpgeo * eff_un_sz;
        if(!virt_groups) {
          Unit_Group* ug = (Unit_Group*)units.gp.FastEl(gi++);
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
  RecomputeGeometry();
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

void Layer::BuildUnits_Threads(Network* net) {
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
  inherited::CheckThisConfig_impl(quiet, rval);
}

void Layer::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  // layerspec should take over this function in layers that have them!
  units.CheckConfig(quiet, rval);
  projections.CheckConfig(quiet, rval);
}

void Layer::FixPrjnIndexes() {
  FOREACH_ELEM_IN_GROUP(Projection, p, projections)
    p->FixPrjnIndexes();
}

void Layer::RemoveCons() {
  taMisc::Busy();
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    if(p->spec.spec)
      p->RemoveCons();
  }
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    u->RemoveCons();
  }
  taMisc::DoneBusy();
}

void Layer::RemoveCons_Net() {
  taMisc::Busy();
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    u->RemoveCons();
  }
  taMisc::DoneBusy();
}

void Layer::RemoveUnits() {
  taMisc::Busy();
  StructUpdate(true);
  if(units.gp.size == 0) {
    units.RemoveAll();
  }
  else {
    int g;
    for(g=0; g<units.gp.size; g++) {
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

void Layer::PreConnect() {
  FOREACH_ELEM_IN_GROUP(Projection, p, projections)
    p->PreConnect();
}

void Layer::Connect() {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
    p->Connect();
  }
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    u->BuildUnits();                    // this is for the bias connections!
  }
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::DisConnect() {
  StructUpdate(true);
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
  StructUpdate(false);
}

int Layer::CountRecvCons() {
  int n_cons = 0;
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    n_cons += u->CountRecvCons();
  }
  return n_cons;
}

void Layer::LinkPtrCons() {
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    u->LinkPtrCons();
  }
}

void Layer::SetLayUnitExtFlags(int flg) {
  SetExtFlag(flg);
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    u->SetExtFlag((Unit::ExtType)flg);
  }
}

void Layer::ApplyInputData(taMatrix* data, Unit::ExtType ext_flags,
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

void Layer::ApplyInputData_1d(taMatrix* data, Unit::ExtType ext_flags,
                              Random* ran, bool na_by_range) {
  for(int d_x = 0; d_x < data->dim(0); d_x++) {
    Unit* un = units.Leaf(d_x);
    if(un) {
      float val = data->SafeElAsVar(d_x).toFloat();
      un->ApplyInputData(val, ext_flags, ran, na_by_range);
    }
  }
}

void Layer::ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
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

void Layer::ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
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

void Layer::ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags, Random* ran,
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

void Layer::ApplyLayerFlags(Unit::ExtType act_ext_flags) {
  SetExtFlag(act_ext_flags);
}

////////////////////////////////////////////////////////////////////////////////
//      Below are the primary computational interface to the Network Objects
//      for performing algorithm-specific activation and learning
//      Many functions operate directly on the units via threads, with
//      optional call through to the layers for any layer-level subsequent processing

void Layer::Init_InputData(Network* net) {
  if(ext_flag == Unit::NO_EXTERNAL)
    return;
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    u->Init_InputData();
  }
  ext_flag = Unit::NO_EXTERNAL;
}

void  Layer::Init_Acts(Network* net) {
  ext_flag = Unit::NO_EXTERNAL;
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    u->Init_Acts(net);
  }
}

void  Layer::Init_dWt(Network* net) {
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    u->Init_dWt(net);
  }
}

void Layer::Init_Weights(Network* net) {
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    u->Init_Weights(net);
  }
  sse = 0.0f;
}

void Layer::Init_Weights_post(Network* net) {
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    u->Init_Weights_post(net);
  }
}

float Layer::Compute_SSE(Network* net, int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  sse = 0.0f;
  if(!HasExtFlag(Unit::TARG | Unit::COMP)) return 0.0f;
  bool has_targ;
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    sse += u->Compute_SSE(net, has_targ);
    if(has_targ) n_vals++;
  }
  float rval = sse;
  if(unit_avg && n_vals > 0)
    sse /= (float)n_vals;
  if(sqrt)
    sse = sqrtf(sse);
  if(HasLayerFlag(NO_ADD_SSE) || (HasExtFlag(Unit::COMP) && HasLayerFlag(NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

int Layer::Compute_PRerr(Network* net) {
  int n_vals = 0;
  prerr.InitVals();
  if(!HasExtFlag(Unit::TARG | Unit::COMP)) return 0;
  float true_pos, false_pos, false_neg;
  FOREACH_ELEM_IN_GROUP(Unit, u, units) {
    if(u->lesioned()) continue;
    bool has_targ = u->Compute_PRerr(net, true_pos, false_pos, false_neg);
    if(has_targ) {
      n_vals++;
      prerr.true_pos += true_pos;
      prerr.false_pos += false_pos;
      prerr.false_neg += false_neg;
    }
  }
  prerr.ComputePR();
  if(HasLayerFlag(NO_ADD_SSE) || (HasExtFlag(Unit::COMP) && HasLayerFlag(NO_ADD_COMP_SSE))) {
    n_vals = 0;
  }
  return n_vals;
}

////////////////////////////////////////////////////////////////////////////////
//      The following are misc functionality not required for primary computing

void Layer::Copy_Weights(const Layer* src) {
  units.Copy_Weights(&(src->units));
}
void Layer::SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt) {
  // name etc is saved & processed by network level guy -- this is equiv to unit group
  units.SaveWeights_strm(strm, fmt);
}

int Layer::LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  return units.LoadWeights_strm(strm, fmt, quiet);
}

int Layer::SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  return Unit_Group::SkipWeights_strm(strm, fmt, quiet);
}

void Layer::SaveWeights(const String& fname, RecvCons::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

int Layer::LoadWeights(const String& fname, RecvCons::WtSaveFormat fmt, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  int rval = false;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, fmt, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Layer::PropagateInputDistance() {
  int new_dist = dist.fm_input + 1;
  FOREACH_ELEM_IN_GROUP(Projection, p, send_prjns) {
    if(!p->layer || p->layer->lesioned()) continue;
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
    if(!p->from || p->from->lesioned()) continue;
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
    if(!p->from || p->from->lesioned()) {
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
  if(!force_use_unit_names && unit_names.dims() == 0) return false;
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
  if(!force_use_unit_names && unit_names.dims() == 0) return false;
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
  FOREACH_ELEM_IN_GROUP(Projection, p, projections)
    rval += p->ProbAddCons(p_add_con, init_wt);
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
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable(name + "_Weights", true);
    new_table = true;
  }
  if(send_lay == NULL) return NULL;
  bool gotone = false;
  FOREACH_ELEM_IN_GROUP(Projection, p, projections) {
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
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
    new_table = true;
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return NULL;

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.AddLayer(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.SetDataNetwork(dt, net);
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
    taProject* proj = GET_MY_OWNER(taProject);
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
    taProject* proj = GET_MY_OWNER(taProject);
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

int Layer::UnitGpIdx(Unit* u) const {
  if(!u || !unit_groups) return -1;
  Unit_Group* osg = u->own_subgp();
  if(osg) {
    return osg->idx;
  }
  else {
    int osg_uidx;  int osg_gpidx;
    UnGpIdxFmUnitIdx(u->idx, osg_uidx, osg_gpidx);
    return osg_gpidx;
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
void Layer::DMem_DistributeUnits() {
  dmem_share_units.Reset();
  DMem_DistributeUnits_impl(dmem_share_units);
  dmem_share_units.Compile_ShareTypes();
}

bool Layer::DMem_DistributeUnits_impl(DMemShare& dms) {
  int np = 0; MPI_Comm_size(dmem_share_units.comm, &np);
  int this_proc = 0; MPI_Comm_rank(dmem_share_units.comm, &this_proc);
  if((dmem_dist == DMEM_DIST_DEFAULT) || (units.gp.size <= 0)) {
    int cnt = 0;
    FOREACH_ELEM_IN_GROUP(Unit, u, units) {
      if(u->lesioned()) continue;
      u->DMem_SetLocalProc(cnt % np);
      u->DMem_SetThisProc(this_proc);
      dms.Link(u);
      cnt++;
    }
    return false;
  }
  else {
    int g;
    for(g=0; g<units.gp.size; g++) {
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(g);
      int cnt = 0;
      FOREACH_ELEM_IN_GROUP(Unit, u, *ug) {
        if(u->lesioned()) continue;
        u->DMem_SetLocalProc(cnt % np);
        u->DMem_SetThisProc(this_proc);
        dms.Link(u);
        cnt++;
      }
    }
    return true;
  }
}

void Layer::DMem_SyncNRecvCons() {
  if(TestError(own_net->dmem_sync_level != Network::DMEM_SYNC_LAYER, "DMem_SyncNRecvCons",
               "attempt to DMem sync at layer level, should only be at network level!")) {
    return;
  }
  dmem_share_units.Sync(0);
}

void Layer::DMem_SyncNet() {
  if(TestError(own_net->dmem_sync_level != Network::DMEM_SYNC_LAYER, "DMem_SyncNet",
               "attempt to DMem sync layer level, should only be at network level!")) {
    return;
  }
  dmem_share_units.Sync(1);
}

void Layer::DMem_SyncAct() {
  if(TestError(own_net->dmem_sync_level != Network::DMEM_SYNC_LAYER, "DMem_SyncAct",
               "attempt to DMem sync layer level, should only be at network level!")) {
    return;
  }
  dmem_share_units.Sync(2);
}
#endif

