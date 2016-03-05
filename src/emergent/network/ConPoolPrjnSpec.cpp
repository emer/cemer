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

#include "ConPoolPrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(ConPoolPrjnSpec);

void ConPoolPrjnSpec::Initialize() {
  pool_size = 2;
  stride = 2;
}

void ConPoolPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
  if(!(bool)prjn->from) return;

  if(TestError(prjn->from.ptr() != prjn->layer, "Connect_impl",
               "ConPoolPrjnSpec only works for self projections!")) {
    return;
  }

  Layer* lay = prjn->layer;
  
  // todo: these are different for stride != pool_size
  int recv_no = pool_size.Product();
  if(!self_con)
    recv_no--;

  if(!make_cons) {
    // pre-allocate connections!
    lay->RecvConsPreAlloc(recv_no, prjn);
    lay->SendConsPreAlloc(recv_no, prjn);
  }
  else {
    if(lay->unit_groups) {
      for(int gpi=0; gpi < lay->gp_geom.n; gpi++) {
        for(int yi=0; yi < lay->un_geom.y; yi += stride.y) {
          for(int xi=0; xi < lay->un_geom.x; xi += stride.x) {

            for(int ryi=0; ryi < pool_size.y; ryi++) {
              for(int rxi=0; rxi < pool_size.x; rxi++) {
                Unit* ru = lay->UnitAtGpIdxUnCoord(gpi, xi + rxi, yi + ryi);
                if(!ru) continue;
            
                for(int syi=0; syi < pool_size.y; syi++) {
                  for(int sxi=0; sxi < pool_size.x; sxi++) {
                    Unit* su = lay->UnitAtGpIdxUnCoord(gpi, xi + sxi, yi + syi);
                    if(!su) continue;
                    if(self_con || (ru != su))
                      ru->ConnectFrom(su, prjn);
                  }
                }
              }
            }
          }
        }
      }
    }
    else {
      for(int yi=0; yi < lay->un_geom.y; yi += stride.y) {
        for(int xi=0; xi < lay->un_geom.x; xi += stride.x) {

          for(int ryi=0; ryi < pool_size.y; ryi++) {
            for(int rxi=0; rxi < pool_size.x; rxi++) {
              Unit* ru = lay->UnitAtCoord(xi + rxi, yi + ryi);
              if(!ru) continue;
            
              for(int syi=0; syi < pool_size.y; syi++) {
                for(int sxi=0; sxi < pool_size.x; sxi++) {
                  Unit* su = lay->UnitAtCoord(xi + sxi, yi + syi);
                  if(!su) continue;
                  if(self_con || (ru != su))
                    ru->ConnectFrom(su, prjn);
                }
              }
            }
          }
        }
      }
    }
  }
}

