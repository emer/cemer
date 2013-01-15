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

#include "RecvCons.h"
#include <SendCons>

void RecvCons::Initialize() {
  SetBaseFlag(OWN_CONS);        // we own by default
  SetBaseFlag(RECV_CONS);       // definitely recv
}

void RecvCons::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);

  if(size > 0) {                // connections exist
    if(CheckError((send_idx() < 0) || (send_idx() != prjn->send_idx), quiet, rval,
                  "unset send_idx, do FixPrjnIndexes or Connect")) {
      prjn->projected = false;
    }

    Unit* su = Un(0);
    if(CheckError(!su, quiet, rval,
                  "sending unit is null when it should not be!  rebuild network!")) {
      prjn->projected = false;
    }
    else if(CheckError((su->send.size <= send_idx()), quiet, rval,
                  "send_idx is out of range on sending unit. Do Actions/Remove Cons, then Build, Connect on Network")) {
      prjn->projected = false;
    }
    else {
      SendCons* sucg = su->send.SafeEl(send_idx());
      if(CheckError(!sucg, quiet, rval,
                    "send_idx is not set correctly (send guy is NULL). Do Actions/Remove Cons, then Build, Connect on Network")) {
        prjn->projected = false;
      }
      else {
        if(CheckError((sucg->prjn != prjn), quiet, rval,
                      "send_idx doesn't have correct prjn on sending unit. Do Actions/Remove Cons, then Build, Connect on Network")) {
          prjn->projected = false;
        }
      }
    }
  }
  if(!GetConSpec()->CheckConfig_RecvCons(this, quiet))
    rval = false;
}

/////////////////////////////////////////////////////////////
//      Dump Load/Save

// have to implement after save_value because we're not saving a real
// path that can be loaded with Load

int RecvCons::Dump_Save_Value(ostream& strm, taBase* par, int indent) {
  int rval = inherited::Dump_Save_Value(strm, par, indent); // first dump members
  if(!rval)
    return rval;

  // close off the regular members
  taMisc::indent(strm, indent,1) << "};\n";

  return Dump_Save_Cons(strm, indent);
}

int RecvCons::Dump_Load_Value(istream& strm, taBase*) {
  int rval = inherited::Dump_Load_Value(strm); // first dump members
  if((rval == EOF) || (rval == 2))
    return rval;

  return Dump_Load_Cons(strm);
}

int RecvCons::Dump_Load_Old_Cons(Unit* ru, int recv_gp_idx) {
  String key = "OldLoadCons_" + String(recv_gp_idx);
  if(!ru->HasUserData(key)) return false;
  String load_str = ru->GetUserData(key).toString();
  istringstream iss(load_str.chars());
  int rval = Dump_Load_Cons(iss, true); // old_2nd_load = true
  ru->RemoveUserData(key);
  return rval;
}
