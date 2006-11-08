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

#ifndef tatime_h
#define tatime_h

#include "ta_base.h"
#include "ta_TA_type.h"

class TA_API taTime : public taBase {
  // ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER raw time information
INHERITED(taBase)
public:
  double	usr;		// user clock ticks
  double	sys;		// system clock ticks
  double	tot;		// total time ticks (all clock ticks on the CPU)

  virtual void 	operator += (const taTime& td)	{ usr += td.usr; sys += td.sys; tot += td.tot; }
  virtual void 	operator -= (const taTime& td)	{ usr -= td.usr; sys -= td.sys; tot -= td.tot; }
  virtual void 	operator *= (const taTime& td)	{ usr *= td.usr; sys *= td.sys; tot *= td.tot; }
  virtual void 	operator /= (const taTime& td)	{ usr /= td.usr; sys /= td.sys; tot /= td.tot; }
  virtual taTime operator + (const taTime& td) const;
  virtual taTime operator - (const taTime& td) const;
  virtual taTime operator * (const taTime& td) const;
  virtual taTime operator / (const taTime& td) const;

  virtual void	ZeroTime();	// zero out the times
  virtual void	GetTime();	// get current clock timing information, for computing difference at later point in time
  virtual String GetString();	// get string output as seconds and fractions of seconds

  void 	Initialize();
  void	Destroy()		{ }
  SIMPLE_COPY(taTime);
  COPY_FUNS(taTime, taBase);
  TA_BASEFUNS(taTime);
};

class TA_API TimeUsed : public taNBase {
  // #INLINE #INLINE_DUMP computes amount of time used for a given process: start the timer at the start, then do EndTimer and it computes the amount used
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

#endif // tatime_h
