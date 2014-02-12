// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "RndSeed_List.h"
#include <MTRnd>
#include <taMisc>
#include <MemberDef>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(RndSeed_List);


void RndSeed_List::MakeSeeds(int n_seeds) {
  seeds.SetSize(n_seeds);
}

void RndSeed_List::NewSeeds() {
  if(seeds.size == 0) return;
  MTRnd::seed_time_pid();
  int rnd = MTRnd::genrand_int31();
  for(int i=0;i<seeds.size;i++) {
    int seed = rnd + i;
    seeds.FastEl(i)->Init(seed);
  }
}

void RndSeed_List::UseSeed(int idx) {
  if(seeds.size == 0) {
    taMisc::Error("RndSeed_List: no seeds present in list!");
    return;
  }
  int use_idx = idx % seeds.size;
  TestWarning(idx != use_idx, "UseSeed", "requested seed beyond end of list, wrapping around!");
  seeds.FastEl(use_idx)->OldSeed();
}

taBase::DumpQueryResult RndSeed_List::Dump_QuerySaveMember(MemberDef* md) {
  if(md->name == "seeds") {
    if(taMisc::is_undo_saving) {
      if(!tabMisc::cur_undo_save_owner || !IsChildOf(tabMisc::cur_undo_save_owner)) {
        // no need to save b/c unaffected by changes elsewhere..
        return DQR_NO_SAVE;
      }
    }
  }
  return inherited::Dump_QuerySaveMember(md);
}

