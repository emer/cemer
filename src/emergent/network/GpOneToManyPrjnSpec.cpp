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

#include "GpOneToManyPrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(GpOneToManyPrjnSpec);


void GpOneToManyPrjnSpec::Initialize() {
  n_con_groups = SEND_ONLY;
}

void GpOneToManyPrjnSpec::GetNGroups(Projection* prjn, int& r_n_ugp, int& s_n_ugp) {
  Unit_Group* recv_ugp = &(prjn->layer->units);
  Unit_Group* send_ugp = &(prjn->from->units);

  s_n_ugp = n_conns;
  if(n_conns < 0)
    s_n_ugp = send_ugp->gp.size - send_start;
  s_n_ugp = MIN(send_ugp->gp.size - send_start, s_n_ugp);
  s_n_ugp = MAX(1, s_n_ugp);    // lower limit of 1

  if(recv_ugp->gp.size > 0)
    r_n_ugp = recv_ugp->gp.size;
  else
    r_n_ugp = 1;

  if(n_con_groups == RECV_SEND_PAIR) {
    prjn->recv_n = s_n_ugp;
    prjn->send_n = r_n_ugp;
  }
  else if(n_con_groups == SEND_ONLY) {
    prjn->recv_n = s_n_ugp;
    prjn->send_n = 1;
  }
  else {
    prjn->recv_n = 1;
    prjn->send_n = 1;
  }
}

// preconnect assumes full interconnectivty in order to maintain
// homogeneity of all unit con_groups in a layer.  this wastes some, but what can you do..
void GpOneToManyPrjnSpec::PreConnect(Projection* prjn) {
  prjn->SetFrom();              // justin case
  if(!(bool)prjn->from) return;

  int r_n_ugp, s_n_ugp;
  int old_send_start = send_start; // temporarilly resort to full connectivity
  int old_n_conns = n_conns;
  send_start = 0;
  n_conns = -1;
  GetNGroups(prjn, r_n_ugp, s_n_ugp);
  send_start = old_send_start;
  n_conns = old_n_conns;

  Unit_Group* recv_ugp = &(prjn->layer->units);
  Unit_Group* send_ugp = &(prjn->from->units);

  // make first set of congroups to get indicies
  Unit* first_ru = (Unit*)recv_ugp->Leaf(0);
  Unit* first_su = (Unit*)send_ugp->Leaf(0);
  if((first_ru == NULL) || (first_su == NULL))
    return;
  RecvCons* recv_gp = first_ru->recv.NewPrjn(prjn);
  prjn->recv_idx = first_ru->recv.size - 1;
  SendCons* send_gp = first_su->send.NewPrjn(prjn);
  prjn->send_idx = first_su->send.size - 1;
  // set reciprocal indicies
  recv_gp->other_idx = prjn->send_idx;
  send_gp->other_idx = prjn->recv_idx;

  // use basic connectivity routine to set indicies..
  int r, s;
  for(r=0; r<r_n_ugp; r++) {
    Unit_Group* rgp;
    if(recv_ugp->gp.size > 0)
      rgp = (Unit_Group*)recv_ugp->gp.FastEl(r);
    else
      rgp = recv_ugp;
    for(s=0; s<s_n_ugp; s++) {
      Unit_Group* sgp;
      if(send_ugp->gp.size > 0)
        sgp = (Unit_Group*)send_ugp->gp.FastEl(s);
      else
        sgp = send_ugp;

      int recv_idx = prjn->recv_idx;
      int send_idx = prjn->send_idx;
      if(n_con_groups != ONE_GROUP)
        recv_idx += s;
      if(n_con_groups == RECV_SEND_PAIR)
        send_idx += r;

      // then its full connectivity..
      FOREACH_ELEM_IN_GROUP(Unit, u, *rgp) {
        if((u == first_ru) && (s == 0)) continue; // skip this one
        recv_gp = u->recv.NewPrjn(prjn);
        recv_gp->other_idx = send_idx;
      }
      FOREACH_ELEM_IN_GROUP(Unit, u, *sgp) {
        if((u == first_su) && (r == 0)) continue; // skip this one
        send_gp = u->send.NewPrjn(prjn);
        send_gp->other_idx = recv_idx;
      }
    }
  }
}

void GpOneToManyPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  int orig_recv_idx = prjn->recv_idx;
  int orig_send_idx = prjn->send_idx;

  int r_n_ugp, s_n_ugp;
  GetNGroups(prjn, r_n_ugp, s_n_ugp);

  Unit_Group* recv_ugp = &(prjn->layer->units);
  Unit_Group* send_ugp = &(prjn->from->units);
  int r, s;
  for(r=0; r<r_n_ugp; r++) {
    Unit_Group* rgp;
    if(recv_ugp->gp.size > 0)
      rgp = (Unit_Group*)recv_ugp->gp.FastEl(r);
    else
      rgp = recv_ugp;
    for(s=0; s<s_n_ugp; s++) {
      Unit_Group* sgp;
      if(send_ugp->gp.size > 0)
        sgp = (Unit_Group*)send_ugp->gp.FastEl(send_start + s);
      else
        sgp = send_ugp;

      if(n_con_groups != ONE_GROUP)
        prjn->recv_idx = orig_recv_idx + s;
      if(n_con_groups == RECV_SEND_PAIR)
        prjn->send_idx = orig_send_idx + r;

      // then its full connectivity..
      FOREACH_ELEM_IN_GROUP(Unit, ru, *rgp) {
        ru->RecvConsPreAlloc(sgp->leaves, prjn);
      }
      FOREACH_ELEM_IN_GROUP(Unit, su, *sgp) {
        su->SendConsPreAlloc(rgp->leaves, prjn);
      }

      FOREACH_ELEM_IN_GROUP(Unit, ru, *rgp) {
        FOREACH_ELEM_IN_GROUP(Unit, su, *sgp) {
          if(self_con || (ru != su))
            ru->ConnectFrom(su, prjn);
        }
      }
    }
  }

  prjn->recv_idx = orig_recv_idx;
  prjn->send_idx = orig_send_idx;
}

