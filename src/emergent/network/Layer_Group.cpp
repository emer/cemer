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

#include "Layer_Group.h"
#include <Network>

#include <SigLinkSignal>


bool Layer_Group::nw_itm_def_arg = false;

void Layer_Group::InitLinks() {
  inherited::InitLinks();
  taBase::Own(pos,this);
}

void Layer_Group::CutLinks() {
  pos.CutLinks();
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

void Layer_Group::SigEmit(int dcr, void* op1, void* op2) {
  inherited::SigEmit(dcr, op1, op2);
  if (dcr == SLS_LIST_ITEM_INSERT) {
    Network* net = GET_MY_OWNER(Network);
    if (net)
      net->RebuildAllViews();
  }
}

void Layer_Group::UpdateMaxDispSize() {
  max_disp_size.x = 1;  max_disp_size.y = 1;  max_disp_size.z = 1;
  max_disp_size2d.x = 1;  max_disp_size2d.y = 1;

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
    if(min_size != pos) {
      taVector3i pos_chg = min_size - pos;
      FOREACH_ELEM_IN_GROUP(Layer, l, *this) {
        l->pos -= pos_chg;      // fix up all the layer rels to be less.
      }
      max_disp_size -= pos_chg; // reduce by amount moving
      pos = min_size;           // new position
    }
    if(min_size2d != pos2d) {
      taVector2i pos_chg = min_size2d - pos2d;
      FOREACH_ELEM_IN_GROUP(Layer, l, *this) {
        l->pos2d -= pos_chg;      // fix up all the layer rels to be less.
      }
      max_disp_size2d -= pos_chg; // reduce by amount moving
      pos2d = min_size2d;           // new position
    }
  }

  // iterate on subgroups
  for(int gi=0; gi<gp.size; gi++) {
    Layer_Group* lgp = (Layer_Group*)gp[gi];
    lgp->UpdateMaxDispSize();
  }
}

void Layer_Group::BuildLayers() {
  BuildLayers_impl();
}

void Layer_Group::BuildLayers_impl() {
  Layer_Group* lg;
  for (int i = 0; i < gp.size; ++i) {
    lg = (Layer_Group*)gp.FastEl(i);
    lg->BuildLayers();
  }
}

void Layer_Group::BuildPrjns() {
  BuildPrjns_impl();
}

void Layer_Group::BuildPrjns_impl() {
  Layer_Group* lg;
  for (int i = 0; i < gp.size; ++i) {
    lg = (Layer_Group*)gp.FastEl(i);
    lg->BuildPrjns();
  }
}

void Layer_Group::Clean() {
  Layer_Group* lg;
  for (int i = gp.size - 1;  i >= 0; --i) {
    lg = (Layer_Group*)gp.FastEl(i);
    lg->Clean();
  }
  Clean_impl();
}

void Layer_Group::LayerPos_Cleanup() {
  bool moved = false;
  int n_itr = 0;
  do {
    moved = false;
    for(int i1=0;i1<leaves;i1++) {
      Layer* l1 = Leaf(i1);
      taVector3i l1abs;
      l1->GetAbsPos(l1abs);
      taVector2i l1s = (taVector2i)l1abs;
      taVector2i l1e = l1s + (taVector2i)l1->scaled_disp_geom;

      taVector2i l1abs2d;
      l1->GetAbsPos2d(l1abs2d);
      taVector2i l1s2d = (taVector2i)l1abs2d;
      taVector2i l1e2d = l1s2d + (taVector2i)l1->scaled_disp_geom;

      for(int i2 = i1+1; i2<leaves;i2++) {
        Layer* l2 = Leaf(i2);
        taVector3i l2abs;
        l2->GetAbsPos(l2abs);
        taVector2i l2s = (taVector2i)l2abs;
        taVector2i l2e = l2s + (taVector2i)l2->scaled_disp_geom;

        taVector3i l2abs2d;
        l2->GetAbsPos2d(l2abs2d);
        taVector2i l2s2d = (taVector2i)l2abs2d;
        taVector2i l2e2d = l2s2d + (taVector2i)l2->scaled_disp_geom;

        if(l2abs.z == l1abs.z) { // 3D
	  if(l2s.x >= l1s.x && l2s.x < l1e.x &&
	     l2s.y >= l1s.y && l2s.y < l1e.y) { // l2 starts in l1; move l2 rt/back
	    if(l1e.x - l2s.x <= l1e.y - l2s.y) {    // closer to x than y
	      l2->pos.x += (l1e.x + 2) - l2s.x;
	    }
	    else {
	      l2->pos.y += (l1e.y + 2) - l2s.y;
	    }
	    l2->SigEmitUpdated();
	    moved = true;
	  }
	  else if(l1s.x >= l2s.x && l1s.x < l2e.x &&
		  l1s.y >= l2s.y && l1s.y < l2e.y) { // l1 starts in l2; move l1 rt/back
	    if(l2e.x - l1s.x <= l2e.y - l1s.y) {    // closer to x than y
	      l1->pos.x += (l2e.x + 2) - l1s.x;
	    }
	    else {
	      l1->pos.y += (l2e.y + 2) - l1s.y;
	    }
	    l1->SigEmitUpdated();
	    moved = true;
	  }
	}

	if(l2s2d.x >= l1s2d.x && l2s2d.x < l1e2d.x &&
	   l2s2d.y >= l1s2d.y && l2s2d.y < l1e2d.y) { // l2 starts in l1; move l2 rt/back
	  if(l1e2d.x - l2s2d.x <= l1e2d.y - l2s2d.y) {    // closer to x than y
	    l2->pos2d.x += (l1e2d.x + 2) - l2s2d.x;
	  }
	  else {
	    l2->pos2d.y += (l1e2d.y + 2) - l2s2d.y;
	  }
	  l2->SigEmitUpdated();
	  moved = true;
	}
	else if(l1s2d.x >= l2s2d.x && l1s2d.x < l2e2d.x &&
		l1s2d.y >= l2s2d.y && l1s2d.y < l2e2d.y) { // l1 starts in l2; move l1 rt/back
	  if(l2e2d.x - l1s2d.x <= l2e2d.y - l1s2d.y) {    // closer to x than y
	    l1->pos2d.x += (l2e2d.x + 2) - l1s2d.x;
	  }
	  else {
	    l1->pos2d.y += (l2e2d.y + 2) - l1s2d.y;
	  }
	  l1->SigEmitUpdated();
	  moved = true;
	}
      }
    }
    n_itr++;
  } while(moved && n_itr < 10);
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
  UpdateMaxDispSize();
  UpdateAfterEdit();
}

void Layer_Group::DeIconifyLayers() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, *this) {
    lay->DeIconify();
  }
  UpdateMaxDispSize();
  UpdateAfterEdit();
}

void Layer_Group::DispScaleLayers(float disp_scale) {
  FOREACH_ELEM_IN_GROUP(Layer, lay, *this) {
    lay->SetDispScale(disp_scale);
  }
  UpdateMaxDispSize();
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

