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

#ifndef Schedule_h
#define Schedule_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(SchedItem);

class EMERGENT_API SchedItem : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Network one element of a schedule
INHERITED(taOBase)
public:
  int           start_ctr;      // ctr number to start at for this item
  float         start_val;      // starting value for item
  int           duration;       // #HIDDEN duration (from start_ctr)
  float         step;           // #HIDDEN linear step to take for each increment of ctr

  float         GetVal(int ctr)  { return start_val + step * (float)(ctr - start_ctr); }
  // get value for given ctr value

  override String       GetDesc() const;

  TA_SIMPLE_BASEFUNS(SchedItem);
private:
  void  Initialize();
  void  Destroy()       { };
};

TypeDef_Of(Schedule);

class EMERGENT_API Schedule : public taList<SchedItem> {
  // ##CAT_Network #NO_EXPAND_ALL A schedule for parameters that vary over time
INHERITED(taList<SchedItem>)
public:
  int           last_ctr;       // #NO_SAVE the last counter index called
  float         default_val;    // the default if group is empty
  bool          interpolate;    // use linear interpolation between points
  float         cur_val;        // #NO_SAVE #READ_ONLY the current val

  float         GetVal(int ctr);
  // #MENU #MENU_ON_Edit #USE_RVAL get current schedule val, based on counter

  TA_SIMPLE_BASEFUNS(Schedule);
protected:
  override void UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // Schedule_h
