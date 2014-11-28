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

#include "UnitVars.h"
#include <Network>

ConGroup* UnitVars::RecvConGroupSafe(Network* net, int thr_no, int rcg_idx) const {
  return net->ThrUnRecvConGroupSafe(thr_no, thr_un_idx, rcg_idx);
}

ConGroup* UnitVars::SendConGroupSafe(Network* net, int thr_no, int scg_idx) const {
  return net->ThrUnSendConGroupSafe(thr_no, thr_un_idx, scg_idx);
}

int UnitVars::NRecvConGpsSafe(Network* net, int thr_no) const {
  return net->ThrUnNRecvConGpsSafe(thr_no, thr_un_idx);
}

int UnitVars::NSendConGpsSafe(Network* net, int thr_no) const {
  return net->ThrUnNSendConGpsSafe(thr_no, thr_un_idx);
}

