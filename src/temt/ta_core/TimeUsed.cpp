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

#include "TimeUsed.h"

TA_BASEFUNS_CTORS_DEFN(TimeUsed);

void TimeUsed::Initialize() {
  s_used = 0.0;
  n_used = 0;
}

void TimeUsed::StartTimer(bool reset_used) {
  if(reset_used) ResetUsed();
  start.GetTime();
}

void TimeUsed::EndTimer() {
  end.GetTime();
  used += end - start;
  s_used = used.TicksToSecs(used.tot);
  n_used++;
}

void TimeUsed::ResetUsed() {
  used.ZeroTime();
  s_used = 0.0;
  n_used = 0;
}

String TimeUsed::GetString() {
  String rval = used.GetString();
  rval += " n: " + (String)n_used;
  if(n_used > 1) {
    taTime avgdiv;
    avgdiv.usr = n_used;  avgdiv.sys = n_used; avgdiv.tot = n_used;
    taTime avg = used / avgdiv;
    rval += " avg: " + avg.GetString();
  }
  return rval;
}
