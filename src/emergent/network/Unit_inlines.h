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

#ifndef Unit_inlines_h
#define Unit_inlines_h 1

// parent includes:
#include <Unit>

// member includes:
#include <Layer>

// declare all other types mentioned but not required to include:


inline Layer* Unit::own_lay() const {
  return ((Unit_Group*)owner)->own_lay;
}

inline bool Unit::lay_lesioned() const {
  return own_lay()->lesioned();
}

inline int Unit::NRecvConGps() const {
  return own_net()->UnNRecvConGps(flat_idx);
}

inline int Unit::NSendConGps() const {
  return own_net()->UnNSendConGps(flat_idx);
}

inline ConState_cpp* Unit::RecvConState(int rcg_idx) const {
  return own_net()->RecvConState(flat_idx, rcg_idx);
}

inline ConState_cpp* Unit::SendConState(int scg_idx) const {
  return own_net()->SendConState(flat_idx, scg_idx);
}

inline ConState_cpp* Unit::RecvConStatePrjn(Projection* prjn) const {
  return RecvConState(prjn->recv_idx);
}

inline ConState_cpp* Unit::SendConStatePrjn(Projection* prjn) const {
  return SendConState(prjn->send_idx);
}

inline UnitState_cpp* Unit::GetUnitState() const {
  return own_net()->GetUnitState(flat_idx);
}

#endif // Unit_inlines_h
