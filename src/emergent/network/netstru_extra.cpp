// Copyright, 1995-2007, Regents of the University of Colorado,
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



// netstru_extra.cc


#include "netstru_extra.h"
#include "css_machine.h"

#ifdef TA_GUI
//probnotneeded #include "netstru_qtso.h"
#endif

/////////////////////////////
//          Full           //
/////////////////////////////

/////////////////////////////
//        SmallWorld       //
/////////////////////////////

/////////////////////////////
//        OneToOne         //
/////////////////////////////

/////////////////////////////
//        Tessel           //
/////////////////////////////

/////////////////////////////
//        ReallyUniformRnd         //
/////////////////////////////

/////////////////////////////
//        UniformRnd       //
/////////////////////////////

/////////////////////////////
//        PolarRnd         //
/////////////////////////////

/////////////////////////////
//        Symmetric        //
/////////////////////////////

/////////////////////////////
//        Script           //
/////////////////////////////

/////////////////////////////
//        Program          //
/////////////////////////////

/////////////////////////////
//        Custom           //
/////////////////////////////

//////////////////////////////////////////
//      UnitGroup-based PrjnSpecs       //
//////////////////////////////////////////

//////////////////////////////////////////
//      GpOneToOnePrjnSpec              //
//////////////////////////////////////////

//////////////////////////////////////////
//      RndGpOneToOnePrjnSpec           //
//////////////////////////////////////////

//////////////////////////////////////////
//      GpOneToManyPrjnSpec             //
//////////////////////////////////////////

/////////////////////////////
//      GpRndTessel        //
/////////////////////////////

///////////////////////////////////////////////////////
//              TiledRFPrjnSpec
///////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
//      TiledGpRFPrjnSpec

///////////////////////////////////////////////////////
//              TiledNovlpPrjnSpec
///////////////////////////////////////////////////////

void TiledNovlpPrjnSpec::Initialize() {
  reciprocal = false;
}

bool TiledNovlpPrjnSpec::InitRFSizes(Projection* prjn) {
  if(!(bool)prjn->from) return false;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return false;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {
    recv_lay = prjn->from;
    send_lay = prjn->layer;
  }

  if(TestWarning(!recv_lay->unit_groups, "InitRFSizes",
                 "requires recv layer to have unit groups!")) {
    return false;
  }

  ru_geo = recv_lay->gp_geom;

  su_act_geom = send_lay->flat_geom;

  rf_width.x = (float)su_act_geom.x / (float)ru_geo.x;
  rf_width.y = (float)su_act_geom.y / (float)ru_geo.y;

  return true;
}

void TiledNovlpPrjnSpec::Connect_impl(Projection* prjn) {
  if(!InitRFSizes(prjn)) return;

  if(reciprocal) {
    Connect_Reciprocal(prjn);
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        taVector2i su_st;
        su_st.x = (int)((float)ruc.x * rf_width.x);
        su_st.y = (int)((float)ruc.y * rf_width.y);

        for(int rui=0; rui < ru_nunits; rui++) {
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          int alloc_sz = ((int)(rf_width.x) + 1) * ((int)(rf_width.y) + 1);
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(alloc_sz, prjn);

          taVector2i suc;
          for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
            for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
              Unit* su_u = send_lay->UnitAtCoord(suc);
              if(su_u == NULL) continue;

              if(!self_con && (su_u == ru_u)) continue;
              ru_u->ConnectFrom(su_u, prjn, alloc_loop);
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void TiledNovlpPrjnSpec::Connect_Reciprocal(Projection* prjn) {
  Layer* recv_lay = prjn->from;
  Layer* send_lay = prjn->layer;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int_Array alloc_sz;
  alloc_sz.SetSize(send_lay->flat_geom.Product());
  alloc_sz.InitVals(0);

  // find alloc sizes
  taVector2i ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      taVector2i su_st;
      su_st.x = (int)((float)ruc.x * rf_width.x);
      su_st.y = (int)((float)ruc.y * rf_width.y);

      taVector2i suc;
      for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
          Unit* su_u = send_lay->UnitAtCoord(suc);
          if(su_u == NULL) continue;
          int sugp_idx = suc.y * send_lay->flat_geom.x + suc.x;
          alloc_sz[sugp_idx] += ru_nunits;
        }
      }
    }
  }

  // do the alloc
  taVector2i suc;
  for(suc.y = 0; suc.y < send_lay->flat_geom.y; suc.y++) {
    for(suc.x = 0; suc.x < send_lay->flat_geom.x; suc.x++) {
      Unit* su_u = send_lay->UnitAtCoord(suc);
      if(su_u == NULL) continue;
      int sugp_idx = suc.y * send_lay->flat_geom.x + suc.x;
      su_u->RecvConsPreAlloc(alloc_sz[sugp_idx], prjn);
    }
  }

  // then make the connections!
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        taVector2i su_st;
        su_st.x = (int)((float)ruc.x * rf_width.x);
        su_st.y = (int)((float)ruc.y * rf_width.y);

        taVector2i suc;
        for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
            Unit* su_u = send_lay->UnitAtCoord(suc);
            if(su_u == NULL) continue;

            for(int rui=0; rui < ru_nunits; rui++) {
              Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
              if(!self_con && (su_u == ru_u)) continue;
              su_u->ConnectFrom(ru_u, prjn, alloc_loop);
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

////////////////////////////////////////////////////////////
//      GpMapConvergePrjnSpec

void GpMapConvergePrjnSpec::Initialize() {
}

void GpMapConvergePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires sending layer to have unit groups!")) {
    return;
  }
  // below assumes the "==" operator has been overloaded for object un_geom in the intuitive way
  if(TestWarning(!(prjn->from->un_geom==prjn->layer->un_geom), "GpMapConvergePrjnSpec::Connect_impl",
                 "requires sending layer unit geometry to match receiving layer unit geometry, i.e., within each unit group!")) {
    return;
  }

Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i su_geo = send_lay->gp_geom;
  int n_su_gps = send_lay->gp_geom.n;

  int alloc_no = n_su_gps;      // number of cons per recv unit

  // pre-alloc senders -- only 1
  FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units)
    su->SendConsPreAlloc(1, prjn);

  for(int ri = 0; ri<recv_lay->units.leaves; ri++) {
    Unit* ru_u = (Unit*)recv_lay->units.Leaf(ri);
    if(!ru_u) break;
    ru_u->RecvConsPreAlloc(alloc_no, prjn);

    taVector2i suc;
    for(suc.y = 0; suc.y < su_geo.y; suc.y++) {
      for(suc.x = 0; suc.x < su_geo.x; suc.x++) {
        int sgpidx = send_lay->UnitGpIdxFmPos(suc);
        Unit* su_u = send_lay->UnitAtUnGpIdx(ri, sgpidx);
        if(su_u) {
          ru_u->ConnectFrom(su_u, prjn);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////
//      GpMapDivergePrjnSpec

void GpMapDivergePrjnSpec::Initialize() {
}

void GpMapDivergePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "GpMapDivergePrjnSpec::Connect_impl",
                 "requires receiving layer to have unit groups!")) {
    return;
  }
  // below assumes the "==" operator has been overloaded for object un_geom in the intuitive way
  if(TestWarning(!(prjn->from->un_geom==prjn->layer->un_geom), "GpMapDivergePrjnSpec::Connect_impl",
                 "requires sending layer unit geometry to match receiving layer unit geometry, i.e., within each unit group!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i ru_geo = recv_lay->gp_geom;
  int n_ru_gps = recv_lay->gp_geom.n;

  int su_alloc_no = n_ru_gps;   // number of cons from each send unit

  // pre-alloc senders
  FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units)
    su->SendConsPreAlloc(su_alloc_no, prjn);

  // pre-alloc receivers -- only 1
  for(int ri = 0; ri<recv_lay->units.leaves; ri++) {
    Unit* ru_u = (Unit*)recv_lay->units.Leaf(ri);
    if(!ru_u) break;
    ru_u->RecvConsPreAlloc(1, prjn);
  }

  // now actually build connections
  for(int si = 0; si<send_lay->units.leaves; si++) {
    Unit* su_u = (Unit*)send_lay->units.Leaf(si);
    taVector2i ruc;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++) {
        int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(si, rgpidx);
        if(ru_u) {
          ru_u->ConnectFrom(su_u, prjn);
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////
//      TiledGpMapConvergePrjnSpec

void TiledGpMapConvergePrjnSpec::Initialize() {
  send_tile_size = 4;
  send_tile_skip = 2;
  wrap = false;
  reciprocal = false;
}

void TiledGpMapConvergePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }

  if(reciprocal) {
    Connect_Reciprocal(prjn);
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->un_geom; // note: unit group geometry!
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;
  int su_ngps = send_lay->gp_geom.n;

  int sg_sz_tot = send_tile_size.Product();
  int alloc_no = sg_sz_tot * su_ngps;

  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        taVector2i su_st;
        if(wrap)        su_st = (ruc-1) * send_tile_skip;
        else            su_st = ruc * send_tile_skip;

        for(int rui=0; rui < ru_nunits; rui++) {
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(alloc_no, prjn);
        }

        taVector2i suc;
        taVector2i suc_wrp;
        for(suc.y = su_st.y; suc.y < su_st.y + send_tile_size.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_st.x + send_tile_size.x; suc.x++) {
            suc_wrp = suc;
            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
              continue;
            int suidx = send_lay->UnitIdxFmPos(suc_wrp);
            if(!send_lay->UnitIdxIsValid(suidx)) continue;

            Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, suidx, alloc_loop);
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void TiledGpMapConvergePrjnSpec::Connect_Reciprocal(Projection* prjn) {
  Layer* recv_lay = prjn->from; // from perspective of non-recip!
  Layer* send_lay = prjn->layer;
  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->un_geom; // note: un_geom
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;
  int su_ngps = send_lay->gp_geom.n;

  int_Array alloc_sz;
  alloc_sz.SetSize(su_geo.Product()); // alloc sizes per each su unit group
  alloc_sz.InitVals(0);

  taVector2i ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      taVector2i su_st;
      if(wrap)  su_st = (ruc-1) * send_tile_skip;
      else      su_st = ruc * send_tile_skip;

      taVector2i suc;
      taVector2i suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_tile_size.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + send_tile_size.x; suc.x++) {
          suc_wrp = suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          int suidx = send_lay->UnitIdxFmPos(suc_wrp);
          if(!send_lay->UnitIdxIsValid(suidx)) continue;
          alloc_sz[suidx] += ru_nunits;
        }
      }
    }
  }

  // now actually allocate
  for(int sug=0; sug < send_lay->gp_geom.n; sug++) {
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sug);
      su_u->RecvConsPreAlloc(alloc_sz[sui], prjn);
    }
  }

  // then connect
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        taVector2i su_st;
        if(wrap)        su_st = (ruc-1) * send_tile_skip;
        else            su_st = ruc * send_tile_skip;

        taVector2i suc;
        taVector2i suc_wrp;
        for(suc.y = su_st.y; suc.y < su_st.y + send_tile_size.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_st.x + send_tile_size.x; suc.x++) {
            suc_wrp = suc;
            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
              continue;
            int suidx = send_lay->UnitIdxFmPos(suc_wrp);
            if(!send_lay->UnitIdxIsValid(suidx)) continue;

            Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, suidx, alloc_loop);
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void TiledGpMapConvergePrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
                                          int rgpidx, int suidx, int alloc_loop) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_ngps = send_lay->gp_geom.n;

  if(reciprocal) {              // reciprocal is backwards!
    for(int sgpi=0; sgpi < su_ngps; sgpi++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(suidx, sgpi);
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        su_u->ConnectFrom(ru_u, prjn, alloc_loop); // recip!
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      for(int sgpi=0; sgpi < su_ngps; sgpi++) {
        Unit* su_u = send_lay->UnitAtUnGpIdx(suidx, sgpi);
        if(!self_con && (su_u == ru_u)) continue;
        ru_u->ConnectFrom(su_u, prjn, alloc_loop); // recip!
      }
    }
  }
}

int TiledGpMapConvergePrjnSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
  // todo: needs impl
  return -1;
}

bool TiledGpMapConvergePrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
  trg_send_geom.x = send_x;
  trg_send_geom.y = send_y;

  if(wrap)
    trg_recv_geom = (trg_send_geom / send_tile_skip);
  else
    trg_recv_geom = (trg_send_geom / send_tile_skip) - 1;

  // now fix it the other way
  if(wrap)
    trg_send_geom = (trg_recv_geom * send_tile_skip);
  else
    trg_send_geom = ((trg_recv_geom +1) * send_tile_skip);

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool TiledGpMapConvergePrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
  trg_recv_geom.x = recv_x;
  trg_recv_geom.y = recv_y;

  if(wrap)
    trg_send_geom = (trg_recv_geom * send_tile_skip);
  else
    trg_send_geom = ((trg_recv_geom+1) * send_tile_skip);

  // now fix it the other way
  if(wrap)
    trg_recv_geom = (trg_send_geom / send_tile_skip);
  else
    trg_recv_geom = (trg_send_geom / send_tile_skip) - 1;

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}

///////////////////////////////////////////////////////////////////
//      GaussRFPrjnSpec

void GaussRFPrjnSpec::Initialize() {
  init_wts = true;
  rf_width = 6;
  rf_move = 3.0f;
  wrap = false;
  gauss_sigma = 1.0f;
}

void GaussRFPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;

  int n_cons = rf_width.Product();
  taVector2i rf_half_wd = rf_width / 2;
  taVector2i rug_geo = prjn->layer->flat_geom; // uses flat geom -- not ug based
  taVector2i su_geo = prjn->from->flat_geom;

  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(ruc.y = 0; ruc.y < rug_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < rug_geo.x; ruc.x++) {
        Unit* ru_u = prjn->layer->UnitAtCoord(ruc);
        if(!ru_u) continue;

        if(!alloc_loop)
          ru_u->RecvConsPreAlloc(n_cons, prjn);

        taVector2i su_st;
        if(wrap) {
          su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
          su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
        }
        else {
          su_st.x = (int)floor((float)ruc.x * rf_move.x);
          su_st.y = (int)floor((float)ruc.y * rf_move.y);
        }

        su_st.WrapClip(wrap, su_geo);
        taVector2i su_ed = su_st + rf_width;
        if(wrap) {
          su_ed.WrapClip(wrap, su_geo); // just wrap ends too
        }
        else {
          if(su_ed.x > su_geo.x) {
            su_ed.x = su_geo.x; su_st.x = su_ed.x - rf_width.x;
          }
          if(su_ed.y > su_geo.y) {
            su_ed.y = su_geo.y; su_st.y = su_ed.y - rf_width.y;
          }
        }


        taVector2i suc;
        taVector2i suc_wrp;
        for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
          for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
            suc_wrp = su_st + suc;
            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
              continue;
            Unit* su_u = prjn->from->UnitAtCoord(suc_wrp);
            if(su_u == NULL) continue;
            if(!self_con && (su_u == ru_u)) continue;

            ru_u->ConnectFrom(su_u, prjn, alloc_loop); // don't check: saves lots of time!
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void GaussRFPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init

  taVector2i rf_half_wd = rf_width / 2;
  taVector2f rf_ctr = rf_half_wd;
  if(rf_half_wd * 2 == rf_width) // even
    rf_ctr -= .5f;

  float sig_sq = gauss_sigma * gauss_sigma;

  for(int i=0; i<cg->size; i++) {
    int su_x = i % rf_width.x;
    int su_y = i / rf_width.x;

    float dst = taMath_float::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
    float wt = expf(-0.5 * dst / sig_sq);

    cg->Cn(i)->wt = wt;
  }
}

bool GaussRFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
  trg_send_geom.x = send_x;
  trg_send_geom.y = send_y;

  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  // now fix it the other way
  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom +1) * rf_move);

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool GaussRFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
  trg_recv_geom.x = recv_x;
  trg_recv_geom.y = recv_y;

  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom+1) * rf_move);

  // now fix it the other way
  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}


/////////////////////////////////////
//        GradientWtsPrjnSpec      //
/////////////////////////////////////

void GradientWtsPrjnSpec::Initialize() {
  wt_range.min = 0.0f;
  wt_range.max = 0.5f;
  wt_range.UpdateAfterEdit_NoGui();
  invert = false;
  grad_x = true;
  grad_y = true;
  wrap = true;
  grad_type = LINEAR;
  use_gps = true;
  gauss_sig = 0.3f;
  Defaults_init();
}

void GradientWtsPrjnSpec::Defaults_init() {
  init_wts = true;
  add_rnd_wts = true;
}


void GradientWtsPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  if(use_gps && prjn->layer->unit_groups)
    InitWeights_RecvGps(prjn, cg, ru);
  else
    InitWeights_RecvFlat(prjn, cg, ru);
}


void GradientWtsPrjnSpec::SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist,
                                      int cg_idx) {
  float wt_val;
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
  cg->Cn(cg_idx)->wt = wt_val;
}

///////////////////////////////////////////////
//      0       1       2       3    recv
//      0       .33     .66     1    rgp_x
//
//      0       1       2       3    send
//      0       .33     .66     1    sgp_x
//      4       5       6       7    wrp_x > .5  int
//      1.33    1.66    2       2.33 wrp_x > .5  flt
//      -4      -3      -2      -1   wrp_x < .5  int
//      -1.33   -1      -.66    -.33 wrp_x < .5  flt

void GradientWtsPrjnSpec::InitWeights_RecvGps(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  Unit* lru = (Unit*)ru;
  int rgpidx = lru->UnitGpIdx();
  taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position relative to overall gp geom
  float rgp_x = (float)rgp_pos.x / (float)MAX(recv_lay->gp_geom.x-1, 1);
  float rgp_y = (float)rgp_pos.y / (float)MAX(recv_lay->gp_geom.y-1, 1);

  float max_dist = 1.0f;
  if(grad_x && grad_y)
    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX(send_lay->flat_geom.x-1, 1);
  float mxs_y = (float)MAX(send_lay->flat_geom.y-1, 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    taVector2i su_pos;
    send_lay->UnitLogPos(su, su_pos);
    float su_x = (float)su_pos.x / mxs_x;
    float su_y = (float)su_pos.y / mxs_y;

    float wrp_x, wrp_y;
    if(wrap) {
      if(rgp_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
      else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
      if(rgp_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
      else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
    }

    float dist = 0.0f;
    if(grad_x && grad_y) {
      dist = taMath_float::euc_dist(su_x, su_y, rgp_x, rgp_y);
      if(wrap) {
        float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_x, rgp_y);
        if(wrp_dist < dist) {
          dist = wrp_dist;
          float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_x, rgp_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
        else {
          float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_x, rgp_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
      }
    }
    else if(grad_x) {
      dist = fabsf(su_x - rgp_x);
      if(wrap) {
        float wrp_dist = fabsf(wrp_x - rgp_x);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    else if(grad_y) {
      dist = fabsf(su_y - rgp_y);
      if(wrap) {
        float wrp_dist = fabsf(wrp_y - rgp_y);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, cg, ru, dist, i);
  }
}

void GradientWtsPrjnSpec::InitWeights_RecvFlat(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  taVector2i ru_pos;
  recv_lay->UnitLogPos(ru, ru_pos);
  float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1);
  float ru_y = (float)ru_pos.y / (float)MAX(recv_lay->flat_geom.y-1, 1);

  float max_dist = 1.0f;
  if(grad_x && grad_y)
    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX(send_lay->flat_geom.x-1, 1);
  float mxs_y = (float)MAX(send_lay->flat_geom.y-1, 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    taVector2i su_pos;
    send_lay->UnitLogPos(su, su_pos);
    float su_x = (float)su_pos.x / mxs_x;
    float su_y = (float)su_pos.y / mxs_y;

    float wrp_x, wrp_y;
    if(wrap) {
      if(ru_x > .5f)    wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
      else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
      if(ru_y > .5f)    wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
      else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
    }

    float dist = 0.0f;
    if(grad_x && grad_y) {
      dist = taMath_float::euc_dist(su_x, su_y, ru_x, ru_y);
      if(wrap) {
        float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_x, ru_y);
        if(wrp_dist < dist) {
          dist = wrp_dist;
          float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_x, ru_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
        else {
          float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_x, ru_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
      }
    }
    else if(grad_x) {
      dist = fabsf(su_x - ru_x);
      if(wrap) {
        float wrp_dist = fabsf(wrp_x - ru_x);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    else if(grad_y) {
      dist = fabsf(su_y - ru_y);
      if(wrap) {
        float wrp_dist = fabsf(wrp_y - ru_y);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, cg, ru, dist, i);
  }
}

