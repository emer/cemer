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


// unfortunately still requiring access to Network here for threads.SyncSpin
#include <Network>
#include <taMisc>

#include <State_cpp>

// #include "NetworkState_cpp.h"

// #include <UnGpState_cpp>

#include <LayerSpec_cpp>
// #include <UnitSpec_cpp>
// #include <ConSpec_cpp>

// #include <LayerState_cpp>
// #include <UnitState_cpp>

// common impl code from NetworkState_core

#include "NetworkState_core.cpp"

void NetworkState_cpp::StateError(const char* a, const char* b, const char* c, const char* d,
                                  const char* e, const char* f, const char* g,
                                  const char* h, const char* i) const {
  String aa; String bb; String cc; String dd; String ee; String ff; String gg;
  String hh; String ii;
  if(a) aa =a;
  if(b) bb =b;
  if(c) cc =c;
  if(d) dd =d;
  if(e) ee =e;
  if(f) ff =f;
  if(g) gg =g;
  if(h) hh =h;
  if(i) ii =i;
  taMisc::Error(aa, bb, cc, dd, ee, ff, gg, hh, ii);
}

void NetworkState_cpp::ThreadSyncSpin(int thr_no, int sync_no) {
  own_net->threads.SyncSpin(thr_no, sync_no);
}
