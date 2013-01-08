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

#ifndef taTime_h
#define taTime_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taTime : public taBase {
  // #STEM_BASE ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##CAT_Program raw time information
INHERITED(taBase)
public:
  double        usr;            // #HIDDEN user clock ticks -- time spent on this process
  double        sys;            // #HIDDEN system clock ticks -- time spent in the kernel on behalf of this process
  double        tot;            // total time ticks (all clock ticks on the CPU for all processes -- wall-clock time)

  void  operator += (const taTime& td)  { usr += td.usr; sys += td.sys; tot += td.tot; }
  void  operator -= (const taTime& td)  { usr -= td.usr; sys -= td.sys; tot -= td.tot; }
  void  operator *= (const taTime& td)  { usr *= td.usr; sys *= td.sys; tot *= td.tot; }
  void  operator /= (const taTime& td)  { usr /= td.usr; sys /= td.sys; tot /= td.tot; }
  taTime operator + (const taTime& td) const;
  taTime operator - (const taTime& td) const;
  taTime operator * (const taTime& td) const;
  taTime operator / (const taTime& td) const;

  double TicksToSecs(double ticks); // convert ticks to seconds

  double GetUsrSecs()           { return TicksToSecs(usr); }
  // get usr time as seconds and fractions thereof
  double GetSysSecs()           { return TicksToSecs(sys); }
  // get sys time as seconds and fractions thereof
  double GetTotSecs()           { return TicksToSecs(tot); }
  // get tot time as seconds and fractions thereof

  void  ZeroTime();     // zero out the times
  void  GetTime();      // get current clock timing information, for computing difference at later point in time
  String GetString(int len=15, int prec=7);
  // get string output as seconds and fractions of seconds, using given length and precision values for the time values

  TA_BASEFUNS(taTime);
private:
  void  Initialize();
  void  Destroy() { }
  void  Copy_(const taTime& cp) {usr = cp.usr; sys = cp.sys; tot = cp.tot;}
};

#endif // taTime_h
