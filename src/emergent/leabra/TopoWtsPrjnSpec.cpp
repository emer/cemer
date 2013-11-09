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

#include "TopoWtsPrjnSpec.h"
#include <Network>
#include <taMath_float>

#include <taMisc>


void	IndexByGps::UpdateAfterEdit_impl() {
	inherited::UpdateAfterEdit_impl();
}


void TopoWtsPrjnSpec::Initialize() {
  wt_range.min = 0.1f;
  wt_range.max = 0.5f;
  wt_range.UpdateAfterEdit_NoGui();
  invert = false;
  topo_pattern = X2X_Y2Y;
  wrap_reflect = NONE;
  grad_type = LINEAR;
  index_by_gps_send.on = false;
  index_by_gps_send.x = false;
  index_by_gps_send.y = false;
  index_by_gps_recv.on = false;
  index_by_gps_recv.x = false;
  index_by_gps_recv.y = false;
  custom_send_range = false;
  custom_recv_range = false;
  send_range_start = 0;
  send_range_end = -1;
  recv_range_start = 0;
  recv_range_end = -1;
  gauss_sig = 0.3f;

  already_warned = 0;

  Defaults_init();
}

void TopoWtsPrjnSpec::Defaults_init() {
  init_wts = true;
  add_rnd_wts = true;
  add_rnd_wts_scale = 1.0f;
}

bool TopoWtsPrjnSpec::TestWarning(bool test, const char* fun_name,
				  const char* a, const char* b, const char* c,
				  const char* d, const char* e, const char* f,
				  const char* g, const char* h) const {
  if(!test) return false;
  const_cast<TopoWtsPrjnSpec*>(this)->already_warned++;
  if(already_warned > 10)
    return true;
  return taMisc::TestWarning(this, test, fun_name, a, b, c, d, e, f, g, h);
}

void TopoWtsPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  already_warned = 0;		// reset

  int recv_no = prjn->from->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    recv_no--;

  int send_no = prjn->layer->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    send_no--;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  if(!index_by_gps_recv.on) { // not using recv_gps
    for(int y = 0; y < prjn->layer->flat_geom.y; y++) {
      for(int x = 0; x < prjn->layer->flat_geom.x; x++) {
	Unit* ru = prjn->layer->UnitAtCoord(x,y);
	//ru->ConnectFrom(su, prjn);
	if(!index_by_gps_send.on) { // not using send_gps
	  for(int y = 0; y < prjn->from->flat_geom.y; y++) {
	    for(int x = 0; x < prjn->from->flat_geom.x; x++) {
	      Unit* su = prjn->from->UnitAtCoord(x,y);
	      ru->ConnectFrom(su, prjn);
	    }
	  }
	}
	else { // send YES -- using send_gps
	  FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
	    if(self_con || (ru != su))
	      ru->ConnectFrom(su, prjn);
	  }
	}
      }
    }
  }
  else { // recv YES -- using recv_gps
    FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
      //bool !use_unit_gps = false;
      //prjn->from.ptr()->unit_groups
      //if(!index_by_gps_send.on && prjn->from.ptr()->unit_groups)
      //!use_unit_gps = true;
      //if(!use_unit_gps) {
      if(!index_by_gps_send.on) { // send NO -- not using send_gps
	for(int y = 0; y < prjn->from->flat_geom.y; y++) {
	  for(int x = 0; x < prjn->from->flat_geom.x; x++) {
	    Unit* su = prjn->from->UnitAtCoord(x,y);
	    ru->ConnectFrom(su, prjn);
	  }
	}
      }
      else { // send YES -- using send gps
	FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
	  if(self_con || (ru != su))
	    ru->ConnectFrom(su, prjn);
	}
      }
    }
  }
}

void TopoWtsPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  if(TestError(((send_range_end.x != -1 && send_range_end.x < send_range_start.x) || (send_range_end.y != -1 && send_range_end.y < send_range_start.y)), "TopoWtsPrjnSpec::C_InitWeights ", " either send_range_end.x or send_range_end.y is less than start_idx. Please correct -- aborting!"))
    return;
  if(TestError(((recv_range_end.x != -1 && recv_range_end.x < recv_range_start.x) || (recv_range_end.y != -1 && recv_range_end.y < recv_range_start.y)), "TopoWtsPrjnSpec::C_InitWeights ", " either recv_range_end.x or recv_range_end.y is less than start_idx. Please correct -- aborting!"))
    return;

  if(TestError((index_by_gps_send.on && !(prjn->from.ptr()->unit_groups)), "TopoWtsPrjnSpec::C_InitWeights ", " index_by_gps_send.on TRUE, but send_lay does not have unit groups. Please fix -- aborting!"))
    return;
  if(TestError((index_by_gps_recv.on && !(prjn->layer->unit_groups)), "TopoWtsPrjnSpec::C_InitWeights ", " index_by_gps_recv.on TRUE, but recv_lay does not have unit groups. Please fix -- aborting!"))
    return;

  //if(prjn->from.ptr()->unit_groups) { send_gps = true; }
  //else { send_gps = false; }
  //if(prjn->layer->unit_groups) { recv_gps = true; }
  //else { recv_gps = false; }

  if(index_by_gps_send.on && (index_by_gps_send.x || index_by_gps_send.y)) { 	// need SendGps
    if (index_by_gps_recv.on && (index_by_gps_recv.x || index_by_gps_recv.y))	// need RecvGps
      InitWeights_SendGpsRecvGps(prjn, cg, ru);
    else // don't need RecvGps
      InitWeights_SendGpsRecvFlat(prjn, cg, ru);
  }
  else // don't need SendGps
    if (index_by_gps_recv.on && (index_by_gps_recv.x || index_by_gps_recv.y))
      InitWeights_SendFlatRecvGps(prjn, cg, ru);
    else // don't need RecvGps either
      InitWeights_SendFlatRecvFlat(prjn, cg, ru);
}

void TopoWtsPrjnSpec::SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist,
				  int cg_idx, bool dbl_add) {
  Network* net = prjn->layer->own_net;
  float wt_val = wt_range.min;
  float wt_add = 0.0f; // how much are we adding over baseline minimum?
  if(grad_type == LINEAR) {
    if(invert)
      wt_val = wt_range.min + dist * wt_range.Range();
    else
      wt_val = wt_range.max - dist * wt_range.Range();
  }
  else if(grad_type == GAUSSIAN) {
    float gaus = taMath_float::gauss_den_nonorm(dist, gauss_sig);
    if(invert)
      wt_val = wt_range.max - gaus * wt_range.Range();
    else
      wt_val = wt_range.min + gaus * wt_range.Range();
  }
  wt_add = wt_val - wt_range.min; // how much did we just add over baseline minimum?
  if(dbl_add) { wt_val += wt_add; } // add that amount again to make up for mirror-image partner that got clipped!
  cg->Cn(cg_idx,BaseCons::WT,net) = wt_val;
}

bool TopoWtsPrjnSpec::ReflectClippedWt(Projection* prjn, RecvCons* cg, Unit* ru, int i, taVector2i ri_pos,
																			 taVector2i srs, taVector2i sre, taVector2i rrs, taVector2i rre, float ri_x, float ri_y) {
  //Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  Network* net = prjn->layer->own_net;

  Unit* su = cg->Un(i,net);
  int sgpidx = su->UnitGpIdx();

  taVector2i su_pos;
  taVector2i sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // position of unit's group relative to overall gp geom

  // next block of code makes it so don't care if using gps in each dimension or not
  taVector2i si_pos; // doesn't care if index is based on gps or flat!
  taVector2i si_geom;

  if(index_by_gps_send.on) {
    if(!index_by_gps_send.x) {
      if (index_by_gps_send.y) { // x-dimension flat, y-dimension gps
        // compute si.x first (is flat version0
	int sunidx = 0;
	//int sgpidx = 0;
	send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
	su_pos.x = sunidx % send_lay->un_geom.x;
	//ru_pos.y = runidx / recv_lay->un_geom.x;
	sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom
	// convert to planar x, y across whole layer
	su_pos.x += send_lay->un_geom.x * sgp_pos.x;
	//ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;
	si_pos.x = su_pos.x;
	si_pos.y = sgp_pos.y;  // easy!
	si_geom.x = send_lay->un_geom.x * send_lay->gp_geom.x;
	si_geom.y = send_lay->gp_geom.y;
      }
      else { // both x-, y- flat
	// compute si.x first
	int sunidx = 0;
	//int sgpidx = 0;
	send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
	su_pos.x = sunidx % send_lay->un_geom.x;
	su_pos.y = sunidx / send_lay->un_geom.y;
	sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom
	// convert to planar x, y across whole layer
	su_pos.x += send_lay->un_geom.x * sgp_pos.x;
	su_pos.y += send_lay->un_geom.y * sgp_pos.y;
	si_pos.x = su_pos.x;
	si_pos.y = su_pos.y;
	si_geom.x = send_lay->un_geom.x * send_lay->gp_geom.x;
	si_geom.y = send_lay->un_geom.y * send_lay->gp_geom.y;
      }
    }
    else { // index_by_gps_send.x = true
      if(index_by_gps_send.y) { // using gps in both x-, y-dimensions - easy!
	si_pos.x = sgp_pos.x;
	si_pos.y = sgp_pos.y;
	si_geom.x = send_lay->gp_geom.x;
	si_geom.y = send_lay->gp_geom.y;
      }
      else { // y-dimension is flat...
	//si_pos.x = sgp_pos.x;
	// compute si_pos.y as flat
	int sunidx = 0;
	//int sgpidx = 0;
	send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
	//su_pos.x = sunidx % send_lay->un_geom.x;
	su_pos.y = sunidx / send_lay->un_geom.y;
	sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom
	// convert to planar x, y across whole layer
	//su_pos.x += send_lay->un_geom.x * sgp_pos.x;
	su_pos.y += send_lay->un_geom.y * sgp_pos.y;
	si_pos.y = su_pos.y;
	si_pos.x = sgp_pos.x;
	si_geom.x = send_lay->gp_geom.x;
	si_geom.y = send_lay->un_geom.y * send_lay->gp_geom.y;
      }
    }
  }
  else { // not using send gps
    if(!send_lay->unit_groups) { // send layer does NOT have unit groups -- nothing to worry about!
      Unit* su = cg->Un(i,net);
      send_lay->UnitLogPos(su, su_pos);
    }
    else { // send_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
      Unit* su = cg->Un(i,net);
      int sunidx = 0;
      int sgpidx = 0;
      send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp) su_pos.x = sunidx % send_lay->un_geom.x;
      
      su_pos.x = sunidx % send_lay->un_geom.x;
      su_pos.y = sunidx / send_lay->un_geom.x;
      taVector2i sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom
      
      // convert to planar x, y across whole layer
      //su_pos.x = su_pos.x + (send_lay->un_geom.x * sgp_pos.x);
      su_pos.x += send_lay->un_geom.x * sgp_pos.x;
      su_pos.y += send_lay->un_geom.y * sgp_pos.y;
    }
    si_pos = su_pos;
    si_geom = send_lay->flat_geom;
    //si_geom = send_lay->un_geom * send_lay->gp_geom;
    //si_geom.x = send_lay->un_geom.x * send_lay->gp_geom.x;
    //si_geom.y = send_lay->un_geom.y * send_lay->gp_geom.y;
  } // END else { // not using send gps
  
  // now we don't care if we're using gps in each dimension or not!
  
  float dist = 1.0f;
  // sending unit in range?  -- get next unit if not
  if(si_pos.x < srs.x || si_pos.x > sre.x) {
    dist = 1.0f;
    return dist;
  }
  if(si_pos.y < srs.y || si_pos.y > sre.y) {
    dist = 1.0f;
    return dist;
  }
  // SPECIAL CASE: only one send unit in range so everyone gets max wt!
  if(sre.x - srs.x == 0 && sre.y - srs.y == 0) { // only one sending unit!
    dist = 1.0f;
    return dist;
  }
  // if in-range, normalize the send range (for topological congruence to recv range)
  si_pos.x -= srs.x;
  si_pos.y -= srs.y;
  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);
  float si_x = (float)si_pos.x / mxs_x;
  float si_y = (float)si_pos.y / mxs_y;
  
  bool x2x, x2y, y2y, y2x = false;
  bool using_sx, using_sy = false;
  switch ( topo_pattern ) {
  case X2X :	// the usual x to x case
    using_sx = true;
    x2x = true;
    x2y = false;
    using_sy = false;
    y2y = false;
    y2x = false;
    break;
  case X2Y :	// the send_x to recv_y case
    using_sx = true;
    x2x = false;
    x2y = true;
    using_sy = false;
    y2y = false;
    y2x = false;
    break;
  case Y2Y :	// i.e. the usual y to y case
    using_sx = false;
    x2x = false;
    x2y = false;
    using_sy = true;
    y2y = true;
    y2x = false;
    break;
  case Y2X :	// the send_y to recv_x case
    using_sx = false;
    x2x = false;
    x2y = false;
    using_sy = true;
    y2y = false;
    y2x = true;
    break;
  case X2X_Y2Y : // x maps to x and y maps to y
    using_sx = true;
    x2x = true;
    x2y = false;
    using_sy = true;
    y2y = true;
    y2x = false;
    break;
  case X2Y_Y2X :	// send_x maps to recv_y and send_y maps to recv_x
    using_sx = true;
    x2x = false;
    x2y = true;
    using_sy = true;
    y2y = false;
    y2x = true;
    break;
  }
  
  // compute distance of RECV idx from closest edge - first in x-dimension, then y
  float dfe_x = MIN(ri_pos.x - rrs.x, rre.x - ri_pos.x);
  dfe_x /= (float)MAX((rre.x - rrs.x), 1);	// normalize
  dfe_x *= 2; 															// re-normalize to 1.0f
  float dfe_y = MIN(ri_pos.y - rrs.y, rre.y - ri_pos.y); 				// ... in y dimension
  dfe_y /= (float)MAX((rre.y - rrs.y), 1);	// normalize
  dfe_y *= 2; 															// re-normalize to 1.0
  // dfe_x, dfe_y now should be 0.0 - 1.0f
  
  // now get dist_x, dist_y to same scale
  float dist_x = 0.0f;
  if(sre.x - srs.x == 0 || rre.x - rrs.x == 0) { dist_x = 0.0f; }
  else { dist_x = fabsf(si_x - ri_x); }	// x distance as normalized absolute value
  //dist_x /= (float)MAX((rre.x - rrs.x), 1);		// normalize
  //dist_x /= (float)MAX(mxs_x, 1);	// normalize according to send layer scale - NO! already normalized
  if(dist_x > 0.5f) { dist_x = 1.0f; }
  else {
    if(dist_x == 0.5f) { dist_x = 0.99f; } // special case for small ranges - keep under 1.0f
    else { dist_x *= 2; }
  }
  // same for y
  float dist_y = 0.0f;
  if(sre.y - srs.y == 0 || rre.y - rrs.y == 0) { dist_y = 0.0f; }
  else { dist_y = fabsf(si_y - ri_y); }	// y distance
  //dist_y /= (float)MAX(mxs_y, 1);		// normalize according to send layer scale - NO! already normalized!
  if(dist_y > 0.5f) { dist_y = 1.0f; }
  else {
    if(dist_y == 0.5f) { dist_y = 0.99f; } // special case for small ranges - keep under 1.0f
    else { dist_y *= 2; }
  }
  // now the rotated distances for X2Y, Y2X
  float dist_xy = 0.0f;
  if(sre.x - srs.x == 0 || rre.y - rrs.y == 0) { dist_xy = 0.0f; }
  else { dist_xy = fabsf(si_x - ri_y); }
  if(dist_xy >0.5f) { dist_xy = 1.0f; }
  else {
    if(dist_xy == 0.5f) { dist_xy = 0.99f; } // special case for small ranges to keep it under 1.0f
    else { dist_xy *= 2; }
  }
  float dist_yx = 0.0f;
  if(sre.y - srs.y == 0 || rre.x - rrs.x == 0) { dist_yx = 0.0f; }
  else { dist_yx = fabsf(si_y - ri_x); }
  if(dist_yx >0.5f) { dist_yx = 1.0f; }
  else {
    if(dist_yx == 0.5f) { dist_yx = 0.99f; } // special case for small ranges to keep it under 1.0f
    else { dist_yx *= 2; }
  }
  // NOTE: 0.011f is an epsilon factor to prevent inappropriate evaluation to true due to rounding error!
  if((x2x && (dist_x < 1.0f) && (dist_x > (dfe_x+0.011f))) || (y2y && (dist_y < 1.0f) && (dist_y > dfe_y+0.011f)) ||
     (x2y && (dist_xy < 1.0f) && (dist_xy > (dfe_y+0.011f))) || (y2x && (dist_yx < 1.0f) && (dist_yx > dfe_x+0.011f))) {
    return true;
  }
  else { return false; }
}

float TopoWtsPrjnSpec::ComputeTopoDist(Projection* prjn, RecvCons* cg, Unit* ru, int i, float ri_x, float ri_y,
																			 taVector2i srs, taVector2i sre, taVector2i rrs, taVector2i rre, taVector2i ri_pos) {
  Layer* send_lay = (Layer*)prjn->from.ptr();
  Network* net = prjn->layer->own_net;

  Unit* su = cg->Un(i,net);
  int sgpidx = su->UnitGpIdx();

  taVector2i su_pos;
  taVector2i sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // position of unit's group relative to overall gp geom

  // following code converts to neutral indices irrespective of using gps or not
  taVector2i si_pos; // `i' = doesn't care if indexing is based on gps or flat!
  taVector2i si_geom;
  if(index_by_gps_send.on) {
    if(!index_by_gps_send.x) {
      if (index_by_gps_send.y) { // x-dimension flat, y-dimension gps
	// compute si.x first (is flat version0
	int sunidx = 0;
	//int sgpidx = 0;
	send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
	su_pos.x = sunidx % send_lay->un_geom.x;
	//ru_pos.y = runidx / recv_lay->un_geom.x;
	sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom
	// convert to planar x, y across whole layer
	su_pos.x += send_lay->un_geom.x * sgp_pos.x;
	//ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;
	si_pos.x = su_pos.x;
	si_pos.y = sgp_pos.y;  // easy!
	si_geom.x = send_lay->un_geom.x * send_lay->gp_geom.x;
	si_geom.y = send_lay->gp_geom.y;
      }
      else { // both x-, y- flat
	// compute si.x first
	int sunidx = 0;
	//int sgpidx = 0;
	send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
	su_pos.x = sunidx % send_lay->un_geom.x;
	su_pos.y = sunidx / send_lay->un_geom.y;
	sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom
	// convert to planar x, y across whole layer
	su_pos.x += send_lay->un_geom.x * sgp_pos.x;
	su_pos.y += send_lay->un_geom.y * sgp_pos.y;
	si_pos.x = su_pos.x;
	si_pos.y = su_pos.y;
	si_geom.x = send_lay->un_geom.x * send_lay->gp_geom.x;
	si_geom.y = send_lay->un_geom.y * send_lay->gp_geom.y;
      }
    }
    else { // index_by_gps_send.x = false
      if(index_by_gps_send.y) { // using gps in both x-, y-dimensions - easy!
	si_pos.x = sgp_pos.x;
	si_pos.y = sgp_pos.y;
	si_geom.x = send_lay->gp_geom.x;
	si_geom.y = send_lay->gp_geom.y;
      }
      else { // y-dimension is flat...
	//si_pos.x = sgp_pos.x;
	// compute si_pos.y as flat
	int sunidx = 0;
	//int sgpidx = 0;
	send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
	//su_pos.x = sunidx % send_lay->un_geom.x;
	su_pos.y = sunidx / send_lay->un_geom.y;
	sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom
	// convert to planar x, y across whole layer
	//su_pos.x += send_lay->un_geom.x * sgp_pos.x;
	su_pos.y += send_lay->un_geom.y * sgp_pos.y;
	si_pos.y = su_pos.y;
	si_pos.x = sgp_pos.x;
	si_geom.x = send_lay->gp_geom.x;
	si_geom.y = send_lay->un_geom.y * send_lay->gp_geom.y;
      }
    }
  }
  else { // index_by_gps_send.on == false -> send is all flat// everything is flat - needed here to cover user error!
    int sunidx = 0;
    //int sgpidx = 0;
    send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
    su_pos.x = sunidx % send_lay->un_geom.x;
    su_pos.y = sunidx / send_lay->un_geom.y;
    sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom
    // convert to planar x, y across whole layer
    su_pos.x += send_lay->un_geom.x * sgp_pos.x;
    su_pos.y += send_lay->un_geom.y * sgp_pos.y;
    si_pos.x = su_pos.x;
    si_pos.y = su_pos.y;
    si_geom.x = send_lay->un_geom.x * send_lay->gp_geom.x;
    si_geom.y = send_lay->un_geom.y * send_lay->gp_geom.y;
  }
  // now we don't care if we're using gps in each dimension or not!

  float dist = 1.0f;
  // sending unit in range?  -- get next unit if not
  if(si_pos.x < srs.x || si_pos.x > sre.x) {
    dist = 1.0f;
    return dist;
  }
  if(si_pos.y < srs.y || si_pos.y > sre.y) {
    dist = 1.0f;
    return dist;
  }
  // SPECIAL CASE: only one send unit in range so everyone gets max wt!
  if(sre.x - srs.x == 0 && sre.y - srs.y == 0) { // only one sending unit!
    dist = 1.0f;
    return dist;
  }
  // if in-range, normalize the send range (for topological congruence to recv range)
  si_pos.x -= srs.x;
  si_pos.y -= srs.y;
  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);
  float si_x = (float)si_pos.x / mxs_x;
  float si_y = (float)si_pos.y / mxs_y;

  bool x2x, x2y, y2y, y2x = false;
  bool using_sx, using_sy = false;
  switch ( topo_pattern ) {
  case X2X :	// the usual x to x case
    using_sx = true;
    x2x = true;
    x2y = false;
    using_sy = false;
    y2y = false;
    y2x = false;
    break;
  case X2Y :	// the send_x to recv_y case
    using_sx = true;
    x2x = false;
    x2y = true;
    using_sy = false;
    y2y = false;
    y2x = false;
    break;
  case Y2Y :	// i.e. the usual y to y case
    using_sx = false;
    x2x = false;
    x2y = false;
    using_sy = true;
    y2y = true;
    y2x = false;
    break;
  case Y2X :	// the send_y to recv_x case
    using_sx = false;
    x2x = false;
    x2y = false;
    using_sy = true;
    y2y = false;
    y2x = true;
    break;
  case X2X_Y2Y : // x maps to x and y maps to y
    using_sx = true;
    x2x = true;
    x2y = false;
    using_sy = true;
    y2y = true;
    y2x = false;
    break;
  case X2Y_Y2X :	// send_x maps to recv_y and send_y maps to recv_x
    using_sx = true;
    x2x = false;
    x2y = true;
    using_sy = true;
    y2y = false;
    y2x = true;
    break;
  }

  float wrp_x, wrp_y;
  if(wrap_reflect==WRAP) {
    if(x2x) {
      if(fabsf(sre.x - srs.x) >= 0.011f && fabsf(rre.x - rrs.x) >= 0.011f) {
	if(ri_x > .5f) 	{ wrp_x = (float)(si_pos.x + si_geom.x) / mxs_x; }
	else           	{ wrp_x = (float)(si_pos.x - si_geom.x) / mxs_x; }
      }
      //else { wrp_x = (float)si_pos.x / mxs_x; } // x2x mapping geom == 1 -> don't wrap!
      else { wrp_x = 0.0f; } // x2x mapping geom == 1 -> don't wrap!
    }
    else if(x2y) {
      if(fabsf(sre.x - srs.x) >= 0.011f && fabsf(rre.y - rrs.y) >= 0.011f) {
	if(ri_y > .5f) 	{ wrp_x = (float)(si_pos.x + si_geom.x) / mxs_x; }
	else           	{ wrp_x = (float)(si_pos.x - si_geom.x) / mxs_x; }
      }
      //else { wrp_x = (float)si_pos.x / mxs_x; } // x2y mapping geom == 1 -> don't wrap
      else { wrp_x = 0.0f; } // x2x mapping geom == 1 -> don't wrap!
    }
    else					{ wrp_x = (float)si_pos.x / mxs_x; } // not using_sx at all so no wrap in x-dimension
    if(y2y) {
      if(fabsf(sre.y - srs.y) >= 0.011f && fabsf(rre.y - rrs.y) >= 0.011f) {
	if(ri_y > .5f)  { wrp_y = (float)(si_pos.y + si_geom.y) / mxs_y; }
	else            { wrp_y = (float)(si_pos.y - si_geom.y) / mxs_y; }
      }
      //else { wrp_y = (float)si_pos.y / mxs_y;  } // y2y mapping geom == 1 -> don't wrap
      else { wrp_y = 0.0f; } // x2x mapping geom == 1 -> don't wrap!
    }
    else if(y2x) {
      if(fabsf(sre.y - srs.y) >= 0.011f && fabsf(rre.x - rrs.x) >= 0.011f) {
	if(ri_x > .5f)  { wrp_y = (float)(si_pos.y + si_geom.y) / mxs_y; }
	else            { wrp_y = (float)(si_pos.y - si_geom.y) / mxs_y; }
      }
      //else { wrp_y = (float)si_pos.y / mxs_y;  } // y2x mapping geom == 1 -> don't wrap
      else { wrp_y = 0.0f; } // x2x mapping geom == 1 -> don't wrap!
    }
    else { wrp_y = (float)si_pos.y / mxs_y; } // not using_sy at all so no wrap in y-dimension
  }

  switch ( topo_pattern ) {
  case X2X :	// the usual x to x case
    if(fabsf(sre.x - srs.x) < 0.011f || fabsf(rre.x - rrs.x) < 0.011f) { dist = 0.0f; } // only one un/gp in x-dimension -> min distance -> max wts.
    else { dist = fabsf(si_x - ri_x); }
    if(wrap_reflect==WRAP) {
      if(fabsf(sre.x - srs.x) >= 0.011f && fabsf(rre.x - rrs.x) >= 0.011f) {
	float wrp_dist = fabsf(wrp_x - ri_x);
	if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    break;
  case X2Y :	// the send_x to recv_y case
    if(fabsf(sre.x - srs.x) < 0.011f || fabsf(rre.y - rrs.y) < 0.011f) { dist = 0.0f; } // geom in send x or recv y == 1 -> min dist (-> max wt)
    else { dist = fabsf(si_x - ri_y); }
    if(wrap_reflect==WRAP) {
      if(fabsf(sre.x - srs.x) >= 0.011f && fabsf(rre.y - rrs.y) >= 0.011f) {
	float wrp_dist = fabsf(wrp_x - ri_y);
	if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    break;
  case Y2Y :	// i.e. the usual y to y case
    if(fabsf(sre.y - srs.y) < 0.011f || fabsf(rre.y - rrs.y) < 0.011f) { dist = 0.0f; } // geom in y-dimension == 1 -> min dist (-> max wt)
    else { dist = fabsf(si_y - ri_y); }
    if(wrap_reflect==WRAP) {
      if(fabsf(sre.y - srs.y) >= 0.011f && fabsf(rre.y - rrs.y) >= 0.011f) {
	float wrp_dist = fabsf(wrp_y - ri_y);
	if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    break;
  case Y2X :	// the send_y to recv_x case
    if(fabsf(sre.y - srs.y) < 0.011f || fabsf(rre.x - rrs.x) < 0.011f) { dist = 0.0f; } // geom in y-dimension == 1 -> min dist (-> max wt)
    else { dist = fabsf(si_y - ri_x); }
    if(wrap_reflect==WRAP) {
      if(fabsf(sre.y - srs.y) >= 0.011f && fabsf(rre.x - rrs.x) >= 0.011f) {
	float wrp_dist = fabsf(wrp_y - ri_x);
	if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    break;
  case X2X_Y2Y : // x maps to x and y maps to y
    if(fabsf(sre.x - srs.x) < 0.011f || fabsf(rre.x - rrs.x < 0.011f)) { // either send or recv has geom in x-dimension == 1 -> x2x dist is misleading
      if(fabsf(sre.y - srs.y) >= 0.011f && fabsf(rre.y - rrs.y) >= 0.011f) { dist = fabsf(si_y - ri_y); } // dist in y2y mapping is governing
      else { dist = 0.0f; }	// both mappings have geom == 1 -> min dist (-> max wt)
    }
    else { // x2x mapping geom > 1
      if(fabsf(sre.y - srs.y) < 0.011f || fabsf(rre.y - rrs.y) < 0.011f) { dist = fabsf(si_x - ri_x); } // only x2x dist matters!
      else { dist = taMath_float::euc_dist(si_x, si_y, ri_x, ri_y); } // both x2x and y2y components matter
    }
    if(wrap_reflect == WRAP) {
      // case: both x2x, y2y mapping geom >1
      if(fabsf(sre.x - srs.x) >= 0.011f && fabsf(rre.x - rrs.x) >= 0.011f &&
	 fabsf(sre.y - srs.y) >= 0.011f && fabsf(rre.y - rrs.y) >= 0.011f >= 0.011f) {
	// test everything..
	float wrp_dist = taMath_float::euc_dist(wrp_x, si_y, ri_x, ri_y);
	if(wrp_dist < dist) { dist = wrp_dist; }
	wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ri_x, ri_y);
	if(wrp_dist < dist) { dist = wrp_dist; }
	wrp_dist = taMath_float::euc_dist(si_x, wrp_y, ri_x, ri_y);
	if(wrp_dist < dist) { dist = wrp_dist; }
      }
      // case: only x2x mapping geom >1
      else if(fabsf(sre.x - srs.x) >= 0.011f && fabsf(rre.x - rrs.x) >= 0.011f) {
	float wrp_dist = fabsf(wrp_x - ri_x);
	if(wrp_dist < dist) { dist = wrp_dist; } 	// whichever way gets you closer1
      }
      // case: only y2y mapping geom >1
      else if(fabsf(sre.y - srs.y) >= 0.011f && fabsf(rre.y - rrs.y) >= 0.011f) {
	float wrp_dist = fabsf(wrp_y - ri_y);
	if(wrp_dist < dist) { dist = wrp_dist; } // whatever gets you closest!
      }
      else { } // both x2x and y2y mapping geom == 1 -> no wrap -> leave dist alone
    }
    break;
  case X2Y_Y2X :	// send_x maps to recv_y and send_y maps to recv_x
    if(fabsf(sre.x - srs.x) < 0.011f || fabsf(rre.y - rrs.y) < 0.011f) { // geom in x-to-y mapping == 1
      if(fabsf(sre.y - srs.y) >= 0.011f && fabsf(rre.x - rrs.x) >= 0.011f) { dist = fabsf(si_y - ri_x); } // ..it's all about y2x!
      else { dist = 0.0f; }	// geom in both x2y and y2x mappings == 1 -> min dist (-> max wt)
    }
    else { // x2y mapping geom > 1
      //if(sre.y - srs.y == 0) { dist = taMath_float::euc_dist(si_x, 0.0f, ri_x, 0.0f); }
      if(fabsf(sre.y - srs.y) < 0.011f || fabsf(rre.x - rrs.x < 0.011f)) { dist = fabsf(si_x - ri_y); } // ..all about x2y
      else { dist = taMath_float::euc_dist(si_x, si_y, ri_y, ri_x); } // both x2y and y2x matter
    }
    // TODO: adapt the X2X_Y2Y to here...
    if(wrap_reflect == WRAP) {
      // first see if wrap in the x-dimension brings you any closer..
      float wrp_dist = taMath_float::euc_dist(wrp_x, si_y, ri_y, ri_x);
      if(wrp_dist < dist) {
	dist = wrp_dist;  	// treat dist as closer than actual distance
				// now test wrapping in both dimensions - since X2X_Y2Y uses both send dimensions!
	float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ri_y, ri_x);
	if(wrp_dist < dist) { dist = wrp_dist; }
      }
      else { // finally - test whether y-wrapping alone gets any closer
	float wrp_dist = taMath_float::euc_dist(si_x, wrp_y, ri_y, ri_x);
	if(wrp_dist < dist) { dist = wrp_dist; }
      }
    }
    break;
  }
  return dist;
}

// wrap calculations for next four functions ///////
////////////////////////////////////////////////////
//      0       1       2       3    recv
//      0       .33     .66     1    rgp_x
//
//      0       1       2       3    send
//      0       .33     .66     1    sgp_x
//      4       5       6       7    wrp_x > .5  int
//      1.33    1.66    2       2.33 wrp_x > .5  flt
//      -4      -3      -2      -1   wrp_x < .5  int
//      -1.33   -1      -.66    -.33 wrp_x < .5  flt

void TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  Network* net = prjn->layer->own_net;

  taVector2i srs = send_range_start; // actual send range start
  taVector2i sre = send_range_end;   // actual send range end
  taVector2i rrs = recv_range_start; // actual recv range start
  taVector2i rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }

  //  end_idx = -1 means set to last unit
  if(sre.x == -1) {
  	if(index_by_gps_send.x) { sre.x = send_lay->gp_geom.x-1; }
  	else { sre.x = send_lay->flat_geom.x-1; }
  }
  if(sre.y == -1) {
  	if(index_by_gps_send.y) { sre.y = send_lay->gp_geom.y-1; }
  else { sre.y = send_lay->flat_geom.y-1; };
  }
  if(rre.x == -1) {
  	if(index_by_gps_recv.x) { rre.x = recv_lay->gp_geom.x-1; }
  	else { rre.x = recv_lay->flat_geom.x-1; }
  }
  if(rre.y == -1) {
  	if(index_by_gps_recv.y) { rre.y = recv_lay->gp_geom.y-1; }
  else { rre.y = recv_lay->flat_geom.y-1; };
  }

  if(TestWarning((index_by_gps_send.x && (sre.x > send_lay->gp_geom.x -1)),
  		"TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
  		"send_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
  	sre.x = send_lay->gp_geom.x-1;
  }
  if(TestWarning((!index_by_gps_send.x && (sre.x > send_lay->flat_geom.x -1)),
  		"TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
  		"send_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
  	sre.x = send_lay->flat_geom.x-1;
  }
  if(TestWarning((index_by_gps_send.y && (sre.y > send_lay->gp_geom.y-1)),
  		"TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
  		"send_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
  	sre.y = send_lay->gp_geom.y-1;
  }
  if(TestWarning((!index_by_gps_send.y && (sre.y > send_lay->flat_geom.y-1)),
  		"TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
  		"send_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
  	sre.y = send_lay->flat_geom.y-1;
  }
  if(TestWarning((index_by_gps_recv.x && (rre.x > recv_lay->gp_geom.x -1)),
  		"TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
  		"recv_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
  	rre.x = recv_lay->gp_geom.x-1;
  }
  if(TestWarning((!index_by_gps_recv.x && (rre.x > recv_lay->flat_geom.x -1)),
  		"TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
  		"recv_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
  	rre.x = recv_lay->flat_geom.x-1;
  }
  if(TestWarning((index_by_gps_recv.y && (rre.y > recv_lay->gp_geom.y-1)),
  		"TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
  		"recv_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
  	rre.y = recv_lay->gp_geom.y-1;
  }
  if(TestWarning((!index_by_gps_recv.y && (rre.y > recv_lay->flat_geom.y-1)),
  		"TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
  		"recv_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
  	rre.y = recv_lay->flat_geom.y-1;
  }
  // else should be the values set by user

  int rgpidx = ru->UnitGpIdx();
  taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position relative to overall gp geom

  float max_dist = 1.0f;
  if(X2X_Y2Y || X2Y_Y2X) {// applies when both gradients in both x and y are used
    max_dist = sqrtf(2.0f);
  }

  float dist = 0.0f;

  taVector2i ru_pos;
  //taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position of unit's group relative to overall gp geom

  // next block of code makes it so don't care if using rgps in each dimension or not
  taVector2i ri_pos; // doesn't care if index is based on gps or flat!
  if(!index_by_gps_recv.x) {
    if (index_by_gps_recv.y) { // x-dimension flat, y-dimension gps
      // compute ri.x first (is flat)
      int runidx = 0;
      //int rgpidx = 0;
      recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
      ru_pos.x = runidx % recv_lay->un_geom.x;
      //ru_pos.y = runidx / recv_lay->un_geom.x;
      rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
      // convert to planar x, y across whole layer
      ru_pos.x += recv_lay->un_geom.x * rgp_pos.x;
      //ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;
      ri_pos.x = ru_pos.x;
      ri_pos.y = rgp_pos.y;  // easy!
    }
    else { // both recv dimensions are flat - needed here to cover user error and/or exploration!
      int runidx = 0;
      //int rgpidx = 0;
      recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
      ru_pos.x = runidx % recv_lay->un_geom.x;
      ru_pos.y = runidx / recv_lay->un_geom.y;
      rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
      // convert to planar x, y across whole layer
      ru_pos.x += recv_lay->un_geom.x * rgp_pos.x;
      ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;
      ri_pos.x = ru_pos.x;
      ri_pos.y = ru_pos.y;
    }
  }
  else { // x-dimension uses gps
    if(index_by_gps_recv.y) { // using gps in both x-, y-dimensions - easy!
      ri_pos.x = rgp_pos.x;
      ri_pos.y = rgp_pos.y;
    }
    else { // compute ri_pos.y as flat
      int runidx = 0;
      //int rgpidx = 0;
      recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
      //su_pos.x = sunidx % send_lay->un_geom.x;
      ru_pos.y = runidx / recv_lay->un_geom.y;
      rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
      // convert to planar x, y across whole layer
      //su_pos.x += send_lay->un_geom.x * sgp_pos.x;
      ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;
      ri_pos.y = ru_pos.y;
      ri_pos.x = rgp_pos.x;
    }
  }
  // now we don't care if we're using rgps in each dimension or not - but wrap code still needed to be tweaked

  // is recv un_gp (and ru) in range?  -- if not, init wts and get next cg/ru
  bool dbl_add = false;
  if(ri_pos.x < rrs.x || ri_pos.x > rre.x || ri_pos.y < rrs.y || ri_pos.y > rre.y) {
    for(int i=0; i<cg->size; i++) {
      dbl_add = false;
      SetWtFmDist(prjn, cg, ru, 1.0f, i, dbl_add); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
    }
    return; // done -- get a new cg/ru
  }

  // if in-range, normalize recv range (for topological congruence to send range)
  ri_pos.x -= rrs.x;
  ri_pos.y -= rrs.y;

  float ri_x = (float)ri_pos.x / (float)MAX((rre.x - rrs.x), 1);
  float ri_y = (float)ri_pos.y / (float)MAX((rre.y - rrs.y), 1);

  // now compute the send maximum distance in each dimension
  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  for(int i=0; i<cg->size; i++) {
    dist = ComputeTopoDist(prjn, cg, ru, i, ri_x, ri_y, srs, sre, rrs, rre, ri_pos);
    dist /= max_dist;           // keep it normalized

    dbl_add = false;
    if(wrap_reflect == REFLECT) {
      dbl_add = ReflectClippedWt(prjn, cg, ru, i, ri_pos, srs, sre, rrs, rre, ri_x, ri_y);
    }

    SetWtFmDist(prjn, cg, ru, dist, i, dbl_add);
    dbl_add = false; // reset for next iteration
  }
}

void TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  Network* net = prjn->layer->own_net;

  taVector2i srs = send_range_start; // actual send range start
  taVector2i sre = send_range_end;   // actual send range end
  taVector2i rrs = recv_range_start; // actual recv range start
  taVector2i rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) {
    if(index_by_gps_send.x) { sre.x = send_lay->gp_geom.x-1; }
    else { sre.x = send_lay->flat_geom.x-1; }
  }
  if(sre.y == -1) {
    if(index_by_gps_send.y) { sre.y = send_lay->gp_geom.y-1; }
    else { sre.y = send_lay->flat_geom.y-1; };
  }

  // recv guys can only be flat!
  if(rre.x == -1) rre.x = recv_lay->flat_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->flat_geom.y-1;

  if(TestWarning((index_by_gps_send.x && (sre.x > send_lay->gp_geom.x -1)),
		 "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
		 "send_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    sre.x = send_lay->gp_geom.x-1;
  }
  if(TestWarning((!index_by_gps_send.x && (sre.x > send_lay->flat_geom.x -1)),
		 "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
		 "send_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    sre.x = send_lay->flat_geom.x-1;
  }
  if(TestWarning((index_by_gps_send.y && (sre.y > send_lay->gp_geom.y-1)),
		 "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
		 "send_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    sre.y = send_lay->gp_geom.y-1;
  }
  if(TestWarning((!index_by_gps_send.y && (sre.y > send_lay->flat_geom.y-1)),
		 "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
		 "send_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    sre.y = send_lay->flat_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->flat_geom.x -1),
		 "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
		 "recv_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    rre.x = recv_lay->flat_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->flat_geom.y-1),
		 "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
		 "recv_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    rre.y = recv_lay->flat_geom.y-1;
  }
  // else should be the values set by user

  float ru_x = 0.0f;
  float ru_y = 0.0f;
  float max_dist = 1.0f;
  if(X2X_Y2Y || X2Y_Y2X) { // applies when both gradients in both x and y are used
    max_dist = sqrtf(2.0f);
  }

  float dist = 0.0f;

  taVector2i ru_pos;
  if (!recv_lay->unit_groups) { // no unit groups to worry about!
    //taVector2i ru_pos;
    recv_lay->UnitLogPos(ru, ru_pos); // ru_pos all loaded up and set...
  }
  else { // recv_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    //taVector2i ru_pos; // hold planar Cartesian coordinates within unit group
    int runidx = 0;
    int rgpidx = 0;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
    ru_pos.x = runidx % recv_lay->un_geom.x;
    ru_pos.y = runidx / recv_lay->un_geom.x;
    taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
    // convert to planar x, y across whole layer
    //ru_pos.x = ru_pos.x + (recv_lay->un_geom.x * rgp_pos.x);
    ru_pos.x += recv_lay->un_geom.x * rgp_pos.x;
    ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;
  }

  // next block of code makes it so don't care if using rgps in each dimension or not
  taVector2i ri_pos; // doesn't care if index is based on gps or flat!
  ri_pos.x = ru_pos.x;
  ri_pos.y = ru_pos.y;
  // now we don't care if we're using gps in each dimension or not!

  // is recv unit in range?  -- if not, init wts and get next cg/ru
  bool dbl_add = false;
  if(ri_pos.x < rrs.x || ri_pos.x > rre.x || ri_pos.y < rrs.y || ri_pos.y > rre.y) {
    for(int i=0; i<cg->size; i++) {
      dbl_add = false;
      SetWtFmDist(prjn, cg, ru, 1.0f, i, dbl_add); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
    }
    return; // done -- get a new cg/ru
  }
  // if in-range, normalize recv range (for topological congruence to send range)
  ri_pos.x -= rrs.x; 	// start at start-indexed units!
  ri_pos.y -= rrs.y;

  float ri_x = (float)ri_pos.x / (float)MAX((rre.x - rrs.x), 1);
  float ri_y = (float)ri_pos.y / (float)MAX((rre.y - rrs.y), 1);

  // now normalize send groups and correct for any offsets!
  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  for(int i=0; i<cg->size; i++) {
    dist = ComputeTopoDist(prjn, cg, ru, i, ri_x, ri_y, srs, sre, rrs, rre, ri_pos);
    dist /= max_dist;           // keep it normalized

    dbl_add = false;
    if(wrap_reflect == REFLECT) {
      dbl_add = ReflectClippedWt(prjn, cg, ru, i, ri_pos, srs, sre, rrs, rre, ri_x, ri_y);
    }

    SetWtFmDist(prjn, cg, ru, dist, i, dbl_add);
    dbl_add = false; // reset for next iteration
  }
}

void TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps(Projection* prjn, RecvCons* cg, Unit* ru) {

  //	if(!index_by_gps_recv.x && !index_by_gps_recv.y) { 	// ignoring recv gps in both x- and y-dimensions - just do flat!
  //		InitWeights_SendFlatRecvFlat(prjn, cg, ru); // in case user selects for exploration (or by mistake)
  //		return;
  //	}

  if(TestWarning((!index_by_gps_recv.x && !index_by_gps_recv.y),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
		 "index_by_gps_recv.x and *.y both OFF -- calling InitWeights_SendFlatRecvFlat instead!")) {
    InitWeights_SendFlatRecvFlat(prjn, cg, ru);
    return;
  }

  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  Network* net = prjn->layer->own_net;

  taVector2i srs = send_range_start; // actual send range start
  taVector2i sre = send_range_end;   // actual send range end
  taVector2i rrs = recv_range_start; // actual recv range start
  taVector2i rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->flat_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->flat_geom.y-1;

  if(rre.x == -1) {
    if(index_by_gps_recv.x) { rre.x = recv_lay->gp_geom.x-1; }
    else { rre.x = recv_lay->flat_geom.x-1; }
  }
  if(rre.y == -1) {
    if(index_by_gps_recv.y) { rre.y = recv_lay->gp_geom.y-1; }
    else { rre.y = recv_lay->flat_geom.y-1; }
  }

  if(TestWarning((sre.x > send_lay->flat_geom.x -1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
		 "send_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    sre.x = send_lay->flat_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->flat_geom.y-1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
		 "send_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    sre.y = send_lay->flat_geom.y-1;
  }
  if(TestWarning((index_by_gps_recv.x && rre.x > recv_lay->gp_geom.x -1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
		 "recv_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    rre.x = recv_lay->gp_geom.x-1;
  }
  if(TestWarning((!index_by_gps_recv.x && rre.x > recv_lay->flat_geom.x -1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
		 "recv_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    rre.x = recv_lay->flat_geom.x-1;
  }
  if(TestWarning((index_by_gps_recv.y && rre.y > recv_lay->gp_geom.y-1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
		 "recv_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    rre.y = recv_lay->gp_geom.y-1;
  }
  if(TestWarning((!index_by_gps_recv.y && rre.y > recv_lay->flat_geom.y-1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
		 "recv_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    rre.y = recv_lay->flat_geom.y-1;
  }

  // TODO: should have tests for start > end at this point, but not very likely

  int rgpidx = ru->UnitGpIdx();
  taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position relative to overall gp geom

  float max_dist = 1.0f;
  if(X2X_Y2Y || X2Y_Y2X) {	// if using gradient in both dimensions...
    max_dist = sqrtf(2.0f);						// ...need to use the hypotenuse!
  }

  float dist = 0.0f;

  taVector2i ru_pos;
  //taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position of unit's group relative to overall gp geom

  // next block converts to unit/gp agnostic indices - so won't care if using rgps in each dimension or not
  taVector2i ri_pos; // doesn't care if index is based on gps or flat!
  if(!index_by_gps_recv.x) {
    if (index_by_gps_recv.y) { // x-dimension flat, y-dimension gps
      // compute ri.x (flat)
      int runidx = 0;
      //int rgpidx = 0;
      recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
      ru_pos.x = runidx % recv_lay->un_geom.x;
      //ru_pos.y = runidx / recv_lay->un_geom.x;
      rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
      // convert to planar x, y across whole layer
      ru_pos.x += recv_lay->un_geom.x * rgp_pos.x;
      //ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;
      ri_pos.x = ru_pos.x;
      ri_pos.y = rgp_pos.y;  // gps - easy!
    }
    else { // both recv dimensions are flat - here for completeness in case not caught at beginning!
      if(TestWarning((!index_by_gps_recv.x && !index_by_gps_recv.y),
		     "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
		     "index_by_gps_recv.x and *.y both OFF -- calling InitWeights_SendFlatRecvFlat instead!")) {
	InitWeights_SendFlatRecvFlat(prjn, cg, ru);
	return;
      }
    }
  }
  else { // x-dimension uses gps
    if(index_by_gps_recv.y) { // using gps in both x-, y-dimensions - easy!
      ri_pos.x = rgp_pos.x;
      ri_pos.y = rgp_pos.y;
    }
    else { // compute ri_pos.y as flat
      int runidx = 0;
      //int rgpidx = 0;
      recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
      //su_pos.x = sunidx % send_lay->un_geom.x;
      ru_pos.y = runidx / recv_lay->un_geom.y;
      rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
      // convert to planar x, y across whole layer
      //su_pos.x += send_lay->un_geom.x * sgp_pos.x;
      ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;
      ri_pos.y = ru_pos.y;
      ri_pos.x = rgp_pos.x;
    }
  }
  // now we don't care if we're using rgps in each dimension or not!

  // is recv un_gp (and ru) in range?  -- if not, init wts and get next cg/ru
  bool dbl_add = false;
  if(ri_pos.x < rrs.x || ri_pos.x > rre.x || ri_pos.y < rrs.y || ri_pos.y > rre.y) {
    for(int i=0; i<cg->size; i++) {
      dbl_add = false;
      SetWtFmDist(prjn, cg, ru, 1.0f, i, dbl_add); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
    }
    return; // done -- get a new cg/ru
  }

  // if in-range, normalize recv range (for topological congruence to send range)
  ri_pos.x -= rrs.x;
  ri_pos.y -= rrs.y;

  float ri_x = (float)ri_pos.x / (float)MAX((rre.x - rrs.x), 1);
  float ri_y = (float)ri_pos.y / (float)MAX((rre.y - rrs.y), 1);

  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  for(int i=0; i<cg->size; i++) {
    dist = ComputeTopoDist(prjn, cg, ru, i, ri_x, ri_y, srs, sre, rrs, rre, ri_pos);
    dist /= max_dist;           // keep it normalized

    dbl_add = false;
    if(wrap_reflect == REFLECT) {
      dbl_add = ReflectClippedWt(prjn, cg, ru, i, ri_pos, srs, sre, rrs, rre, ri_x, ri_y);
    }

    SetWtFmDist(prjn, cg, ru, dist, i, dbl_add);
    dbl_add = false; // reset for next iteration
  }
}

void TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru) {

  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  Network* net = prjn->layer->own_net;

  taVector2i srs = send_range_start; // actual send range start
  taVector2i sre = send_range_end;   // actual send range end
  taVector2i rrs = recv_range_start; // actual recv range start
  taVector2i rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->flat_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->flat_geom.y-1;
  if(rre.x == -1) rre.x = recv_lay->flat_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->flat_geom.y-1;

  if(TestWarning((sre.x > send_lay->flat_geom.x -1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
		 "send_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    sre.x = send_lay->flat_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->flat_geom.y-1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
		 "send_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    sre.y = send_lay->flat_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->flat_geom.x -1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
		 "recv_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    rre.x = recv_lay->flat_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->flat_geom.y-1),
		 "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
		 "recv_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    rre.y = recv_lay->flat_geom.y-1;
  }
  // else should be the values set by user

  //bool dbl_add = false; // used by REFLECT to compensate for clipping at edges

  float ru_x = 0.0f;
  float ru_y = 0.0f;

  float max_dist = 1.0f;
  float dist = 0.0f;

  bool dbl_add = false; // used by REFLECT to compensate for clipping at edges

  taVector2i ru_pos;
  if (!recv_lay->unit_groups) { // no unit groups to worry about!
    //taVector2i ru_pos;
    recv_lay->UnitLogPos(ru, ru_pos);
    if(X2X_Y2Y || X2Y_Y2X) 	// if using gradient in both dimensions...
      max_dist = sqrtf(2.0f);						// ...need to use the hypotenuse!

    // is recv unit in range?  -- if not, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x || ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
      	dbl_add = false;
      	SetWtFmDist(prjn, cg, ru, 1.0f, i, dbl_add); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x; 	// start at start-indexed units!
    //ru_pos.y = ru_pos.y - rrs.y;
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    //float ru_y = (float)ru_pos.y / (float)MAX(recv_lay->flat_geom.y-1, 1);
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }
  else { // recv_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    //taVector2i ru_pos; // hold planar Cartesian coordinates within unit group
    int runidx = 0;
    int rgpidx = 0;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
    ru_pos.x = runidx % recv_lay->un_geom.x;
    ru_pos.y = runidx / recv_lay->un_geom.x;
    taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
    // convert to planar x, y across whole layer
    //ru_pos.x = ru_pos.x + (recv_lay->un_geom.x * rgp_pos.x);
    ru_pos.x += recv_lay->un_geom.x * rgp_pos.x;
    ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;

    if(X2X_Y2Y || X2Y_Y2X) 	// if using gradient in both dimensions...
      max_dist = sqrtf(2.0f);						// ...need to use the hypotenuse!

    // is recv unit out-of-range?  -- if so, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x || ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
	dbl_add = false;
	SetWtFmDist(prjn, cg, ru, 1.0f, i, dbl_add); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x;		  // start at start-indexed units!
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }

  taVector2i ri_pos = ru_pos;
  float ri_x = ru_x;
  float ri_y = ru_y;

  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  for(int i=0; i<cg->size; i++) {
    dist = ComputeTopoDist(prjn, cg, ru, i, ri_x, ri_y, srs, sre, rrs, rre, ri_pos);
    dist /= max_dist;           // keep it normalized

    dbl_add = false;
    if(wrap_reflect == REFLECT) {
      dbl_add = ReflectClippedWt(prjn, cg, ru, i, ri_pos, srs, sre, rrs, rre, ri_x, ri_y);
    }

    SetWtFmDist(prjn, cg, ru, dist, i, dbl_add);
    dbl_add = false; // reset for next iteration
  }
}

