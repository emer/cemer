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

#ifndef BaseCons_inlines_h
#define BaseCons_inlines_h 1

// parent includes:
#include <Network>

// member includes:

// declare all other types mentioned but not required to include:

inline Unit* BaseCons::Un(int idx, Network* net) const {
  return net->UnFmIdx(unit_idxs[idx]);
}

inline bool BaseCons::SetUn(int idx, Unit* un) {
  if(!InRange(idx)) return false;
  unit_idxs[idx] = un->flat_idx;
  return true;
}

inline BaseCons* BaseCons::UnCons(int idx, Network* net) const
{ if(IsRecv()) return Un(idx, net)->send.FastEl(other_idx);
  return Un(idx, net)->recv.FastEl(other_idx); }

inline BaseCons* BaseCons::SafeUnCons(int idx, Network* net) const
{ if(IsRecv()) return Un(idx, net)->send.SafeEl(other_idx);
  return Un(idx, net)->recv.SafeEl(other_idx); }

#endif // BaseCons_inlines_h
