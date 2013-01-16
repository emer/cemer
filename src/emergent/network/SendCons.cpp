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

#include "SendCons.h"
#include <Unit>
#include <Projection>
#include <Layer>



void SendCons::Initialize() {
  ClearBaseFlag(OWN_CONS);      // we don't own by default
  ClearBaseFlag(RECV_CONS);     // definitely Send
}

void SendCons::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);

  if(size > 0) {                // connections exist
    if(CheckError((recv_idx() < 0) || (recv_idx() != prjn->recv_idx), quiet, rval,
                  "unset recv_idx, do FixPrjnIndexes or Connect")) {
      prjn->projected = false;
    }
    Unit* ru = Un(0);
    if(CheckError(!ru, quiet, rval,
                  "recv unit is null when it should not be!  rebuild network!")) {
      prjn->projected = false;
    }
    else if(CheckError((ru->recv.size <= recv_idx()), quiet, rval,
                       "recv_idx is out of range on recv unit. Do Actions/Remove Cons, then Build, Connect on Network")) {
      prjn->projected = false;
    }
    else {
      RecvCons* rucg = ru->recv.SafeEl(recv_idx());
      if(CheckError(!rucg, quiet, rval,
                    "recv_idx is not set correctly (recv guy is NULL). Do Actions/Remove Cons, then Build, Connect on Network")) {
        prjn->projected = false;
      }
      else {
        if(CheckError((rucg->prjn != prjn), quiet, rval,
                      "recv_idx doesn't have correct prjn on recv unit. Do Actions/Remove Cons, then Build, Connect on Network")) {
          prjn->projected = false;
        }
      }
    }
  }
}


