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

#include "ActrBuffer.h"
#include <ActrModule>

#include <taMisc>

void ActrBuffer::Initialize() {
  act_total = 0.0f;
  flags = BF_NONE;
}

bool ActrBuffer::Matches(const String& query, bool why_not) {
  bool rval = false;
  if(query == "full") {
    rval = IsFull();
  }
  else if(query == "empty") {
    rval = IsEmpty();
  }
  else if(query == "requested") {
    rval = IsReq();
  }
  else if(query == "unrequested") {
    rval = IsUnReq();
  }
  if(!rval && why_not) {
    taMisc::Info("buffer:", GetDisplayName(), "query:", query, "returned false");
  }
  return rval;
}

void ActrBuffer::UpdateState() {
  SetBufferFlagState(FULL, active.size == 1);
}
