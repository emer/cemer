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

#ifndef taUndoDiffSrc_h
#define taUndoDiffSrc_h 1

// parent includes:
#include <taOBase>

// for smart ptrs need these:
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <taDateTime>
#include <taStringDiff>

// declare all other types mentioned but not required to include:
class taUndoRec; // 


taTypeDef_Of(taUndoDiffSrc);

class TA_API taUndoDiffSrc : public taOBase {
  // ##CAT_Undo full source record for diff-based saving of undo save data -- diffs are computed against this guy
INHERITED(taOBase)
public:
  taDateTime    mod_time;       // time (to seconds level of resolution) when obj was modified
  taBaseRef     save_top;       // top-level object under which the data was saved
  String        save_top_path;  // path to the save_top -- in case it disappears -- again relative to mgr owner
  String        save_data;      // dump-file save from the save_top object (save_top->Save_String)
  taStringDiff  diff;           // diff for this data -- save_data is string_a for this case
  int           last_diff_n;    // raw number of diff records -- when this gets to be too large, then it is time to move on to a new src
  float         last_diff_pct;  // percent diff records are of total save_data lines -- when this gets to be too large, then it is time to move on to a new src

  virtual void  InitFmRec(taUndoRec* rec);
  // initialize our vals from given record
  virtual void  EncodeDiff(taUndoRec* rec);
  // encode a new diff against given record, set rec to point to us
  virtual int   UseCount();
  // report how many undo recs are using this guy -- if none, then remove..

  TA_SIMPLE_BASEFUNS(taUndoDiffSrc);
private:
  void  Initialize();
  void  Destroy()       { CutLinks(); }
};

TA_SMART_PTRS(taUndoDiffSrc);

#endif // taUndoDiffSrc_h
