// Copyright, 1995-2007, Regents of the University of Colorado,
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



#include "ta_time.h"

#if (defined(TA_OS_WIN))
# include <time.h>
#else
# include <sys/time.h>
# include <sys/times.h>
# include <unistd.h>
#endif


//////////////////////////
// 	taTime  	//
//////////////////////////

void taTime::Initialize() {
  ZeroTime();
}

void taTime::ZeroTime() {
  usr = 0; sys = 0; tot = 0;
}

taTime taTime::operator+(const taTime& td) const {
  taTime rv;
  rv.usr = usr + td.usr; rv.sys = sys + td.sys; rv.tot = tot + td.tot;
  return rv;
}
taTime taTime::operator-(const taTime& td) const {
  taTime rv;
  rv.usr = usr - td.usr; rv.sys = sys - td.sys; rv.tot = tot - td.tot;
  return rv;
}
taTime taTime::operator*(const taTime& td) const {
  taTime rv;
  rv.usr = usr * td.usr; rv.sys = sys * td.sys; rv.tot = tot * td.tot;
  return rv;
}
taTime taTime::operator/(const taTime& td) const {
  taTime rv;
  rv.usr = usr / td.usr; rv.sys = sys / td.sys; rv.tot = tot / td.tot;
  return rv;
}

#if defined(TA_OS_WIN)

double taTime::TicksToSecs(double ticks) {
  double ticks_per = (double)CLOCKS_PER_SEC;
  return ticks / ticks_per;
}

void taTime::GetTime() {
  clock_t tottime = clock();
  tot = tottime;
  //NOTE: just allocate all to usr
  usr = tot;
  sys = 0.0;
}

#else

double taTime::TicksToSecs(double ticks) {
  double ticks_per = (double)sysconf(_SC_CLK_TCK);
  return ticks / ticks_per;
}

void taTime::GetTime() {
  struct tms t;
  clock_t tottime = times(&t);
  tot = tottime;
  usr = t.tms_utime;
  sys = t.tms_stime;
}

#endif

String taTime::GetString(int len, int prec) {
  String rval = "usr: " + taMisc::FormatValue(GetUsrSecs(), len, prec)
    + " sys: " + taMisc::FormatValue(GetSysSecs(), len, prec)
    + " tot: " + taMisc::FormatValue(GetTotSecs(), len, prec);
  return rval;
}

//////////////////////////
// 	TimeUsed 	//
//////////////////////////

void TimeUsed::Initialize() {
  n_used = 0;
}

void TimeUsed::StartTimer(bool reset_used) {
  start.GetTime();
  if(reset_used) ResetUsed();
}

void TimeUsed::EndTimer() {
  end.GetTime();
  used = end - start;
  n_used++;
}

void TimeUsed::ResetUsed() {
  used.ZeroTime();
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

//////////////////////////
// 	taDateTime 	//
//////////////////////////

void taDateTime::Initialize() {
  secs_1jan_1970 = 0;
}
