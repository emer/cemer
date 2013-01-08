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

#ifndef TimeUsedHR_h
#define TimeUsedHR_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API TimeUsedHR : public taNBase {
  // #STEM_BASE #INLINE #INLINE_DUMP  ##CAT_Program computes amount of time used (in high resolution) for a given process: start the timer at the start, then do EndTimer and it computes the amount used
INHERITED(taNBase)
public:
  double        s_used;         // #SHOW #GUI_READ_ONLY total number of seconds used
  int           n_used;         // #SHOW #GUI_READ_ONLY number of individual times the timer has been used without resetting accumulation

  virtual void  StartTimer(bool reset_used = true);
  // record the current time as the starting time, and optionally reset the time used information
  virtual void  EndTimer();     // record the current time as the ending time, and compute difference as the time used
  virtual void  ResetUsed();
  // reset time used information

  TA_BASEFUNS(TimeUsedHR);
protected:
  TimeUsedHRd*          d; // private impl data (depends on platform)
private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const TimeUsedHR& cp);
};

#endif // TimeUsedHR_h
