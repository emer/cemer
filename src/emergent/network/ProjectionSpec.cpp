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

#include "ProjectionSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(ProjectionSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(ProjectionSpec_SPtr);


void ProjectionSpec::Initialize() {
  min_obj_type = &TA_Projection;
  self_con = false;
  init_wts = false;
  add_rnd_wts = false;
  add_rnd_wts_scale = 1.0f;
}

void ProjectionSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_ProjectionSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void ProjectionSpec::PreConnect(Projection* prjn) {
  if(!(bool)prjn->from) return;

  // make first set of congroups to get indicies
  Unit* first_ru = (Unit*)prjn->layer->units.Leaf(0);
  Unit* first_su = (Unit*)prjn->from->units.Leaf(0);
  if((first_ru == NULL) || (first_su == NULL))
    return;
  RecvCons* recv_gp = first_ru->recv.NewPrjn(prjn);
  prjn->recv_idx = first_ru->recv.size - 1;
  SendCons* send_gp = first_su->send.NewPrjn(prjn);
  prjn->send_idx = first_su->send.size - 1;
  // set reciprocal indicies
  recv_gp->other_idx = prjn->send_idx;
  send_gp->other_idx = prjn->recv_idx;

  // then crank out for remainder of units..
  FOREACH_ELEM_IN_GROUP(Unit, u, prjn->layer->units) {
    if(u == first_ru)   continue; // skip over first one..
    recv_gp = u->recv.NewPrjn(prjn);
    recv_gp->other_idx = prjn->send_idx;
  }
  FOREACH_ELEM_IN_GROUP(Unit, u, prjn->from->units) {
    if(u == first_su)   continue; // skip over first one..
    send_gp = u->send.NewPrjn(prjn);
    send_gp->other_idx = prjn->recv_idx;
  }
}

void ProjectionSpec::Connect(Projection* prjn) {
  prjn->RemoveCons();
  prjn->SetFrom();
  if(TestWarning(!(bool)prjn->from, "Connect", "from pointer is NULL -- cannot make this projection"))
    return;
  if(prjn->off) return;
  if(prjn->from->lesioned()) return;
  PreConnect(prjn);
  Connect_impl(prjn);
  Init_Weights(prjn);
  prjn->projected = true;
}

int ProjectionSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
  return 0;
}

int ProjectionSpec::ProbAddCons(Projection* prjn, float p_add_con, float init_wt) {
  int rval = ProbAddCons_impl(prjn, p_add_con);
  return rval;
}

void ProjectionSpec::Init_Weights(Projection* prjn) {
  if(prjn->off) return;
  Network* net = prjn->layer->own_net;
  FOREACH_ELEM_IN_GROUP(Unit, u, prjn->layer->units) {
    for(int g=0; g < u->recv.size; g++) {
      RecvCons* cg = u->recv.FastEl(g);
      if(cg->prjn == prjn)
        cg->Init_Weights(u, net);
    }
  }
}

void ProjectionSpec::Init_Weights_post(Projection* prjn) {
  if(prjn->off) return;
  Network* net = prjn->layer->own_net;
  FOREACH_ELEM_IN_GROUP(Unit, u, prjn->layer->units) {
    for(int g=0; g < u->recv.size; g++) {
      RecvCons* cg = u->recv.FastEl(g);
      if(cg->prjn == prjn)
        cg->Init_Weights_post(u, net);
    }
  }
}

void ProjectionSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  // default is just to do same thing as the conspec would have done..
  Network* net = prjn->layer->own_net;
  CON_GROUP_LOOP(cg, cg->C_Init_Weights(i, ru, cg->Un(i,net), net));
}

void ProjectionSpec::Init_dWt(Projection* prjn) {
  Network* net = prjn->layer->own_net;
  FOREACH_ELEM_IN_GROUP(Unit, u, prjn->layer->units) {
    int g;
    for(g=0; g < u->recv.size; g++) {
      RecvCons* cg = u->recv.FastEl(g);
      if(cg->prjn == prjn)
        cg->Init_dWt(u,net);
    }
  }
}

bool ProjectionSpec::CheckConnect(Projection* prjn, bool quiet) {
  if(prjn->off) return true;
  bool rval;
  if(CheckError(!prjn->from, quiet, rval, "from is null -- must set from!")) {
    return false;
  }
  if(prjn->from->lesioned()) return true;
  if(CheckError(!prjn->projected, quiet, rval, "not connected!")) {
    return false;
  }
  if(CheckError(!prjn->con_spec.spec, quiet, rval, "has null con_spec")) {
    return false;
  }
  if(CheckError(!prjn->con_spec->CheckObjectType(prjn), quiet, rval,
                "does not have correct spec/object type")) {
    return false;
  }
  return true;
}

