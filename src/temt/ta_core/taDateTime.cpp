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

#include "taDateTime.h"

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(taDateTime);

void taDateTime::Initialize() {
  secs_1jan_1970 = 0;
}

void taDateTime::fromString(const String &s, const String &format) {
  if(format.empty()) fmQDateTime(QDateTime::fromString(s));
  else fmQDateTime(QDateTime::fromString(s, format));
}

String taDateTime::SecondsToDHM(uint64_t duration)  // duration is in seconds
{
  String rval;
  int seconds = (int) (duration % 60);
  duration /= 60;
  int minutes = (int) (duration % 60);
  duration /= 60;
  int hours = (int) (duration % 24);
  int days = (int) (duration / 24);
  
  rval = taMisc::LeadingZeros(days, 2) + "d_" + taMisc::LeadingZeros(hours, 2) + "h_" + taMisc::LeadingZeros(minutes, 2) + "m";
  return rval;
}
