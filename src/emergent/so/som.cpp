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


#include "som.h"

TA_BASEFUNS_CTORS_DEFN(SomUnitSpec);
TA_BASEFUNS_CTORS_DEFN(NeighborEl_List);
TA_BASEFUNS_CTORS_DEFN(NeighborEl);
TA_BASEFUNS_CTORS_DEFN(SomLayerSpec);

void SomUnitSpec::Compute_Netin(UnitVars* u, Network* net, int thr_no) {
  if (u->ext_flag & UnitVars::EXT) {
    u->net = u->ext;
  }
  else {
    // do distance instead of net input
    u->net = 0.0f;
    const int nrcg = net->ThrUnNRecvConGps(thr_no, u->thr_un_idx);
    for(int g=0; g<nrcg; g++) {
      SoConGroup* rgp = (SoConGroup*)net->ThrUnRecvConGroup(thr_no, u->thr_un_idx, g);
      if(rgp->NotActive()) continue;
      u->net += rgp->con_spec->Compute_Dist(rgp, net, thr_no);
    }
  }
}

void NeighborEl::Initialize() {
  act_val = 1.0;
}

void SomLayerSpec::Initialize() {
  wrap = false;
  netin_type = MIN_NETIN_WINS;
}

void SomLayerSpec::KernelEllipse(int half_width, int half_height, int ctr_x, int ctr_y) {
  neighborhood.Reset();
  int strt_x = ctr_x - half_width;
  int end_x = ctr_x + half_width;
  int strt_y = ctr_y - half_height;
  int end_y = ctr_y + half_height;
  if(half_width == half_height) { // circle
    int y;
    for(y = strt_y; y <= end_y; y++) {
      int x;
      for(x = strt_x; x <= end_x; x++) {
	int dist = ((x - ctr_x) * (x - ctr_x)) + ((y - ctr_y) * (y - ctr_y));
	if(dist > (half_width * half_width))
	  continue;		// outside the circle
	NeighborEl* te = (NeighborEl*)neighborhood.New(1, &TA_NeighborEl);
	te->off.x = x;
	te->off.y = y;
      }
    }
  }
  else {			// ellipse
    float f1_x, f1_y;		// foci
    float f2_x, f2_y;
    float two_a;			// two times largest axis
    
    if(half_width > half_height) {
      two_a = (float)half_width * 2;
      float c = sqrtf((float)(half_width * half_width) - (float)(half_height * half_height));
      f1_x = (float)ctr_x - c;
      f1_y = (float)ctr_y;
      f2_x = (float)ctr_x + c;
      f2_y = (float)ctr_y;
    }
    else {
      two_a = (float)half_height * 2;
      float c = sqrtf((float)(half_height * half_height) - (float)(half_width * half_width));
      f1_x = (float)ctr_x;
      f1_y = (float)ctr_y - c;
      f2_x = (float)ctr_x;
      f2_y = (float)ctr_y + c;
    }

    int y;
    for(y = strt_y; y <= end_y; y++) {
      int x;
      for(x = strt_x; x <= end_x; x++) {
	float dist = sqrtf((((float)x - f1_x) * ((float)x - f1_x)) + (((float)y - f1_y) * ((float)y - f1_y))) +
	  sqrtf((((float)x - f2_x) * ((float)x - f2_x)) + (((float)y - f2_y) * ((float)y - f2_y)));
	if(dist > two_a)
	  continue;
	NeighborEl* te = (NeighborEl*)neighborhood.New(1, &TA_NeighborEl);
	te->off.x = x;
	te->off.y = y;
      }
    }
  }
}

void SomLayerSpec::KernelRectangle(int width, int height, int ctr_x, int ctr_y) {
  neighborhood.Reset();
  int strt_x = ctr_x - (width / 2);
  int strt_y = ctr_y - (height / 2);
  int y;
  for(y = 0; y < height; y++) {
    int x;
    for(x = 0; x < width; x++) {
      NeighborEl* te = (NeighborEl*)neighborhood.New(1, &TA_NeighborEl);
      te->off.x = strt_x + x;
      te->off.y = strt_y + y;
    }
  }
}

// void SomLayerSpec::KernelFromNetView(NetView* view) {
//   if((view == NULL) || (view->editor == NULL) || (view->editor->netg == NULL))
//     return;
//   if(view->editor->netg->selectgroup.size <= 0) {
//     taMisc::Error("Must select some units to get connection pattern from");
//     return;
//   }
//   neighborhood.Reset();
//   int i;
//   taBase* itm;
//   Unit* center = NULL;
//   for(i=0; i< view->editor->netg->selectgroup.size; i++) {
//     itm = view->editor->netg->selectgroup.FastEl(i);
//     if(!itm->InheritsFrom(TA_Unit))      continue;
//     Unit* un = (Unit*) itm;
//     if(center == NULL) {
//       center = un;
//     }
//     NeighborEl* te = (NeighborEl*)neighborhood.New(1, &TA_NeighborEl);
//     te->off.x = un->pos.x - center->pos.x;
//     te->off.y = un->pos.y - center->pos.y;
//   }
// }

void SomLayerSpec::StepKernelActs(float val) {
  int i;
  NeighborEl* te;
  for(i = 0; i< neighborhood.size; i++) {
    te = (NeighborEl*)neighborhood.FastEl(i);
    te->act_val = val;
  }
}

void SomLayerSpec::LinearKernelActs(float scale) {
  int i;
  NeighborEl* te;
  for(i = 0; i< neighborhood.size; i++) {
    te = (NeighborEl*)neighborhood.FastEl(i);
    float dist = sqrtf((float)(te->off.x * te->off.x + 
			     te->off.y * te->off.y));
    te->act_val = scale * (1.0f / (dist + 1.0f));
  }
}

void SomLayerSpec::GaussianKernelActs(float scale, float sigma) {
  int i;
  NeighborEl* te;
  for(i = 0; i< neighborhood.size; i++) {
    te = (NeighborEl*)neighborhood.FastEl(i);
    float dist = (float)(te->off.x * te->off.x + 
			 te->off.y * te->off.y);
    te->act_val = scale * exp(-0.5 * dist / sigma);
  }
}

int SomLayerSpec::WrapClip(int coord, int max_coord) {
  if(coord >= max_coord) {
    if(wrap)
      coord = coord % max_coord;
    else
      coord = -1;
  }
  else if(coord < 0) {
    if(wrap)
      coord = max_coord + (coord % max_coord);
    else
      coord = -1;
  }
  return coord;
}


void SomLayerSpec::Compute_Act_post(SoLayer* lay, SoNetwork* net) {
  if(lay->ext_flag & UnitVars::EXT) {  // input layer
    SoLayerSpec::Compute_Act_post(lay, net);
    return;
  }

  // Added by Danke, Feb. 9, 2003
  if(lay->units.leaves > 0 &&      // sync layer
     lay->units.FastEl(0)->ext_flag() & UnitVars::EXT) {
    SoLayerSpec::Compute_Act_post(lay, net);
    return;
  }

  SoUnit* win_u = FindWinner(lay);
  lay->winner = win_u;
  if(win_u == NULL)	return;

  int i;
  NeighborEl* te;
  for(i = 0; i< neighborhood.size; i++) {
    te = (NeighborEl*)neighborhood.FastEl(i);
    int su_x = WrapClip(win_u->pos.x + te->off.x, lay->un_geom.x);
    int su_y = WrapClip(win_u->pos.y + te->off.y, lay->un_geom.y);

    if((su_x < 0) || (su_y < 0))
      continue;

    int su_idx = su_y * lay->un_geom.x + su_x;
    if(su_idx >= lay->units.leaves)
      continue;

    SoUnit* su_u = (SoUnit*)lay->units.Leaf(su_idx);
    su_u->act() = te->act_val;
  }
}
