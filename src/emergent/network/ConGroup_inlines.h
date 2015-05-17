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

#ifndef ConGroup_inlines_h
#define ConGroup_inlines_h 1

// parent includes:
#include <Network>

// member includes:

// declare all other types mentioned but not required to include:

inline bool ConGroup::PrjnIsActive() {
  return (prjn && prjn->IsActive());
}

inline TypeDef* ConGroup::ConType() const  {
  return prjn->con_type;
}

inline Unit* ConGroup::OwnUn(Network* net) {
  return net->UnFmIdx(own_flat_idx);
}

inline Unit* ConGroup::ThrOwnUn(Network* net, int thr_no) {
  return net->ThrUnit(thr_no, own_thr_idx);
}

inline UnitVars* ConGroup::OwnUnVars(Network* net) {
  return net->UnUnitVars(own_flat_idx);
}

inline UnitVars* ConGroup::ThrOwnUnVars(Network* net, int thr_no) {
  return net->ThrUnitVars(thr_no, own_thr_idx);
}

inline Unit* ConGroup::Un(int idx, Network* net) const {
  return net->UnFmIdx(UnIdx(idx));
}

inline UnitVars* ConGroup::UnVars(int idx, Network* net) const {
  return net->UnUnitVars(UnIdx(idx));
}

inline bool ConGroup::SetUn(int idx, Unit* un) {
  if(!InRange(idx)) return false;
  UnIdx(idx) = un->flat_idx;
  return true;
}

inline ConGroup* ConGroup::UnCons(int idx, Network* net) const {
  if(IsRecv()) return net->SendConGroup(UnIdx(idx), other_idx);
  return net->RecvConGroup(UnIdx(idx), other_idx);
}

inline ConGroup* ConGroup::SafeUnCons(int idx, Network* net) const {
  if(!InRange(idx)) return NULL;
  if(IsRecv()) return net->SendConGroup(UnIdx(idx), other_idx);
  return net->RecvConGroup(UnIdx(idx), other_idx);
}


#endif // ConGroup_inlines_h
