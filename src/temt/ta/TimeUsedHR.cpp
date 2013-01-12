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

#include "TimeUsedHR.h"

#if (defined(TA_OS_WIN))
# include <time.h>
# include <windows.h>
#else
# include <sys/time.h>
# include <sys/times.h>
# include <unistd.h>
#endif


#if defined(TA_OS_WIN)

class TimeUsedHRd {
public:
  static LARGE_INTEGER freq;
  LARGE_INTEGER start;
  LARGE_INTEGER end;
  LARGE_INTEGER used;
  void		GetStartTime() { QueryPerformanceCounter(&start);}
  void		GetEndTime() { 
    QueryPerformanceCounter(&end);
    used.QuadPart = end.QuadPart - start.QuadPart;
    }
  double	GetTotSecs() {
    if (freq.QuadPart == 0) {
      QueryPerformanceFrequency(&freq);
    } 
    double rval = used.QuadPart / (double)(freq.QuadPart);
    return rval;}
};

LARGE_INTEGER TimeUsedHRd::freq;

#else

// computes a-b
#define ONE_MILLION 1000000
timeval timeval_diff(timeval a, timeval b) {
  a.tv_sec -= b.tv_sec;
  a.tv_usec -= b.tv_usec;
  if (a.tv_usec < 0) {
    a.tv_sec-- ;
    a.tv_usec += ONE_MILLION;
  }
  return a;
}

class TimeUsedHRd {
public:
  timeval	start;
  timeval	end;
  timeval	used;
  void		GetStartTime() { gettimeofday(&start, NULL);}
  void		GetEndTime() { 
    gettimeofday(&end, NULL);
    used = timeval_diff(end, start);}
  double	GetTotSecs() { 
    double rval = used.tv_usec / 1000000.0;
    if (used.tv_sec) rval += used.tv_sec;
    return rval;
    }
};

#endif

void TimeUsedHR::Initialize() {
  s_used = 0.0;
  n_used = 0;
  d = new TimeUsedHRd;
}

void TimeUsedHR::Destroy() {
  delete d;
  d = NULL;
}

void TimeUsedHR::Copy_(const TimeUsedHR& cp) {
  *d = *(cp.d);
  s_used = cp.s_used;
  n_used = cp.n_used;
}

void TimeUsedHR::StartTimer(bool reset_used) {
  if (reset_used) ResetUsed();
  d->GetStartTime();
}

void TimeUsedHR::EndTimer() {
  d->GetEndTime();
  s_used += d->GetTotSecs();
  n_used++;
}

void TimeUsedHR::ResetUsed() {
  s_used = 0.0;
  n_used = 0;
}


