// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#ifndef ta_time_h
#define ta_time_h

#include "ta_base.h"
#include "ta_TA_type.h"

class TA_API taTime : public taBase {
  // ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##CAT_Program raw time information
INHERITED(taBase)
public:
  double	usr;		// user clock ticks -- time spent on this process
  double	sys;		// system clock ticks -- time spent in the kernel on behalf of this process
  double	tot;		// total time ticks (all clock ticks on the CPU for all processes -- wall-clock time)

  virtual void 	operator += (const taTime& td)	{ usr += td.usr; sys += td.sys; tot += td.tot; }
  virtual void 	operator -= (const taTime& td)	{ usr -= td.usr; sys -= td.sys; tot -= td.tot; }
  virtual void 	operator *= (const taTime& td)	{ usr *= td.usr; sys *= td.sys; tot *= td.tot; }
  virtual void 	operator /= (const taTime& td)	{ usr /= td.usr; sys /= td.sys; tot /= td.tot; }
  virtual taTime operator + (const taTime& td) const;
  virtual taTime operator - (const taTime& td) const;
  virtual taTime operator * (const taTime& td) const;
  virtual taTime operator / (const taTime& td) const;

  virtual double TicksToSecs(double ticks); // convert ticks to seconds

  double GetUsrSecs()		{ return TicksToSecs(usr); }
  // get usr time as seconds and fractions thereof
  double GetSysSecs()		{ return TicksToSecs(sys); }
  // get sys time as seconds and fractions thereof
  double GetTotSecs()		{ return TicksToSecs(tot); }
  // get tot time as seconds and fractions thereof

  virtual void	ZeroTime();	// zero out the times
  virtual void	GetTime();	// get current clock timing information, for computing difference at later point in time
  virtual String GetString(int len=15, int prec=7);
  // get string output as seconds and fractions of seconds, using given length and precision values for the time values

  void 	Initialize();
  void	Destroy()		{ }
  SIMPLE_COPY(taTime);
  COPY_FUNS(taTime, taBase);
  TA_BASEFUNS(taTime);
};

class TA_API TimeUsed : public taNBase {
  // #INLINE #INLINE_DUMP  ##CAT_Program computes amount of time used for a given process: start the timer at the start, then do EndTimer and it computes the amount used
INHERITED(taNBase)
public:
  taTime	start;		// starting time
  taTime	end;		// ending time
  taTime	used;		// total time used
  int		n_used;		// number of individual times the timer has been used without resetting accumulation

  virtual void	StartTimer(bool reset_used = true);
  // record the current time as the starting time, and optionally reset the time used information
  virtual void	EndTimer();	// record the current time as the ending time, and compute difference as the time used
  virtual void	ResetUsed();
  // reset time used information
  virtual String GetString();	// get string output of time used as seconds and fractions of seconds

  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(TimeUsed);
};

#endif // ta_time_h
