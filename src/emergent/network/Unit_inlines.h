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

inline ConGroup* Unit::RecvConGroup(int rcg_idx) const {
  return own_net()->RecvConGroup(flat_idx, rcg_idx);
}

inline ConGroup* Unit::SendConGroup(int scg_idx) const {
  return own_net()->SendConGroup(flat_idx, scg_idx);
}

inline ConGroup* Unit::RecvConGroupPrjn(Projection* prjn) const {
  return RecvConGroup(prjn->recv_idx);
}

inline ConGroup* Unit::SendConGroupPrjn(Projection* prjn) const {
  return SendConGroup(prjn->send_idx);
}

inline UnitVars* Unit::GetUnitVars() const {
  return own_net()->UnUnitVars(flat_idx);
}

inline int Unit::ThrNo() const {
  return own_net()->UnThr(flat_idx);
}

/////////////////////////
//      UnitVars


inline int UnitVars::ThrNo(Network* net) const {
  return net->UnThr(flat_idx);
}

inline Unit*  UnitVars::Un(Network* net, int thr_no) const {
  return net->ThrUnit(thr_no, thr_un_idx);
}

inline int UnitVars::NRecvConGps(Network* net, int thr_no) const {
  return net->ThrUnNRecvConGps(thr_no, thr_un_idx);
}

inline int UnitVars::NSendConGps(Network* net, int thr_no) const {
  return net->ThrUnNSendConGps(thr_no, thr_un_idx);
}

inline ConGroup* UnitVars::RecvConGroup(Network* net, int thr_no, int rcg_idx) const {
  return net->ThrUnRecvConGroup(thr_no, thr_un_idx, rcg_idx);
}

inline ConGroup* UnitVars::SendConGroup(Network* net, int thr_no, int scg_idx) const {
  return net->ThrUnSendConGroup(thr_no, thr_un_idx, scg_idx);
}

inline ConGroup* UnitVars::RecvConGroupPrjn(Network* net, int thr_no, Projection* prjn) const {
  return net->ThrUnRecvConGroup(thr_no, thr_un_idx, prjn->recv_idx);
}

inline ConGroup* UnitVars::SendConGroupPrjn(Network* net, int thr_no, Projection* prjn) const {
  return net->ThrUnSendConGroup(thr_no, thr_un_idx, prjn->send_idx);
}

#endif // Unit_inlines_h
